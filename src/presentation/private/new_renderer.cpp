#include "new_renderer.h"

#include <shared/defer.h>
#include <shared/log.h>
#include <shared/hash.h>
#include <shared/math.inl>
#include <shared/optional.h>
#include <shared/string.h>
#include <shared/memory.h>
#include <shared/string_id.inl>

#include <platform/platform.h>

#define PAW_ENABLE_IMGUI

PAW_DISABLE_ALL_WARNINGS_BEGIN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#undef near
#undef far
#include <spirv_reflect.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
PAW_DISABLE_ALL_WARNINGS_END

#include "startup_args.h"
#include "profiler.h"
#include "allocators/page_pool.h"
#include "allocators/frame_allocator.h"
#include "allocators/arena_allocator.h"
#include "swf.h"
#include "debug_draw.h"
#include "client_graph.h"

enum MemExport
{
	MemExport_No,
	MemExport_Yes,
};

struct ImageStore
{
	enum SlotState
	{
		SlotState_Empty,
		SlotState_Active,
		SlotState_ReadyForDeletion,
	};

	enum Type
	{
		Type_Unknown,
		Type_Internal,
		Type_External, // Used for importing images from other APIs - This is used in the tools code
	};

	enum Flags
	{
		Flags_None = 0,
		Flags_PushedToShader = 1 << 0,
	};

	static constexpr usize max_images = 64;
	static constexpr UTF8StringView empty_name = PAW_STR("Empty", 5);
	VkImage images[max_images];
	VkImageView views[max_images];
	u32 ref_counts[max_images];
	usize most_recent_frame_uses[max_images];
	SlotState states[max_images];
	UTF8StringView debug_names[max_images];
	VkDeviceMemory memories[max_images];
	Type types[max_images];
	Flags flags[max_images];
	u32 pushed_shader_slot[max_images];

	void init();
	void deinit();

	void deletePendingImages();
	void deleteImage(usize index);

	usize grabFirstFreeSlot();

	GpuImageHandle createAndAllocate(
		usize width, usize height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags memory_props, VkImageAspectFlags aspect_flags, UTF8StringView debug_name = PAW_STR("Unnamed", 7),
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

	GpuImageHandle createAndImport(
		u64 external_handle, usize width, usize height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags memory_props, VkImageAspectFlags aspect_flags, UTF8StringView debug_name = PAW_STR("Unnamed", 7),
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

	void removeRef(GpuImageHandle handle);
	void addRef(GpuImageHandle handle);
	void addFrameUsage(GpuImageHandle handle, usize frame_index);

	VkImage getVkImage(GpuImageHandle handle) const;
	VkImageView getView(GpuImageHandle handle) const;
	u32 getShaderSlot(GpuImageHandle handle) const;

	u32 pushToShader(GpuImageHandle handle);
};

struct Swapchain
{
	static constexpr usize max_images = 4;
	VkSwapchainKHR swapchain;
	VkImage images[max_images];
	VkImageView image_views[max_images];
	u32 image_count;
	VkExtent2D extent;

	void init(Platform& platform, Swapchain* old_swapchain);
	void deinit();
	Optional<u32> prepareNextImage(VkSemaphore semaphore_to_signal);
	bool present(VkSemaphore semaphore_to_wait_on, u32 image_index);
};

struct ShaderSlotPool
{
	Slice<u32> free_slots;
	usize free_slot_count;

	void init(Allocator* alloctor, usize count)
	{
		free_slots = alloctor->alloc<u32>(count);
		free_slot_count = count;
		for (usize i = 0; i < count; i++)
		{
			free_slots[i] = static_cast<u32>(count - i) - 1;
		}
	}

	u32 alloc()
	{
		PAW_ASSERT(free_slot_count > 0);
		free_slot_count--;
		u32 free_index = free_slots[free_slot_count];
		free_slots[free_slot_count] = 0xDEADBEEF;
		return free_index;
	}

	void free(u32 index)
	{
		PAW_ASSERT(free_slot_count < free_slots.size);
		PAW_ASSERT(free_slots[free_slot_count] == 0xDEADBEEF);
		free_slots[free_slot_count] = index;
		free_slot_count++;
	}

	u32 getMaxVulkanSize()
	{
		return static_cast<u32>(free_slots.size);
	}
};

enum BindSlot
{
	BindSlot_StorageBuffers,
	BindSlot_Images,
	BindSlot_Samplers,
};

static constexpr u32 g_debug_lines_slot = 0;
static constexpr u32 g_debug_line_cmds_slot = 1;

struct BlendStateDesc
{
	bool enabled = false;
	VkBlendFactor src_color_factor = VK_BLEND_FACTOR_ONE;
	VkBlendFactor dst_color_factor = VK_BLEND_FACTOR_ZERO;
	VkBlendOp color_op = VK_BLEND_OP_ADD;
	VkBlendFactor src_alpha_factor = VK_BLEND_FACTOR_ONE;
	VkBlendFactor dst_alpha_factor = VK_BLEND_FACTOR_ZERO;
	VkBlendOp alpha_op = VK_BLEND_OP_ADD;

	static const BlendStateDesc alpha_blend;
};

const BlendStateDesc BlendStateDesc::alpha_blend = {
	.enabled = true,
	.src_color_factor = VK_BLEND_FACTOR_SRC_ALPHA,
	.dst_color_factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	.color_op = VK_BLEND_OP_ADD,
	.src_alpha_factor = VK_BLEND_FACTOR_ONE,
	.dst_alpha_factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	.alpha_op = VK_BLEND_OP_ADD,
};

struct GraphicsPipelineDesc
{
	UTF8StringView name;
	VkShaderModule vert_shader = VK_NULL_HANDLE;
	VkShaderModule frag_shader = VK_NULL_HANDLE;
	VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT;
	BlendStateDesc blend_state = {};
	bool depth_test;
	bool depth_write;
	VkCompareOp depth_compare_op;
	VkFormat color_format = VK_FORMAT_R8G8B8A8_SRGB;
	VkFormat depth_format = VK_FORMAT_D32_SFLOAT;
	VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;
	bool alpha_to_coverage = false;
	bool sample_shading = false;
	f32 min_sample_shading = 1.0f;
};

struct ComputePipelineDesc
{
	UTF8StringView name;
	VkShaderModule shader = VK_NULL_HANDLE;
	VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
};

enum Pipeline
{
	Pipeline_ImGui,
	Pipeline_FullscreenBlit,
	Pipeline_DebugGeom,
	Pipeline_NoDepthDebugGeom,
	Pipeline_Vector,
	Pipeline_EditorMesh,
	Pipeline_DebugGeom2D,
	Pipeline_Mesh,
	Pipeline_Count,
};

enum class PipelineType
{
	Graphics,
	Compute,
};

struct PipelineDesc
{
	PipelineType type;
	union
	{
		GraphicsPipelineDesc graphics;
		ComputePipelineDesc compute;
	};
};

struct PipelineReflectionData
{
	static constexpr usize max_buffers = 16;

	struct Buffer
	{
		usize struct_size;
		u64 struct_name_hash;
	};

	enum class Type
	{
		Graphics,
		Compute,
	};

	UTF8StringView shader_name;
	Buffer buffers[max_buffers];
	usize buffer_count;
	usize push_constant_size;
	Type type;

	Buffer const* findBuffer(u64 name_hash) const
	{
		for (usize i = 0; i < buffer_count; i++)
		{
			if (buffers[i].struct_name_hash == name_hash)
			{
				return &buffers[i];
			}
		}

		return nullptr;
	}

	void emplaceBuffer(Buffer&& buffer)
	{
		buffers[buffer_count++] = buffer;
	}

	void reset()
	{
		buffer_count = 0;
		push_constant_size = 0;
	}
};

struct AllocatedImage
{
	VkImage image;
	VkDeviceMemory memory;
};

struct AllocatedBuffer
{
	VkBuffer buffer;
	VkDeviceMemory memory;
	usize size;

	void* map();
	void unmap();
	VkDeviceAddress getDeviceAddress() const;
};

struct FrameFinishData
{
	VkSemaphore image_available_semaphore;
	VkSemaphore render_finished_semaphore;
	VkFence in_flight_fence;
	VkCommandBuffer command_buffer;
	u32 image_index;
	bool terminated_early;

	struct Game
	{
		GpuImageHandle render_target;
		GpuImageHandle resolved_render_target;
		GpuImageHandle depth_image;
		u32 scene_constants_slot;
	} game;

	usize render_count = 0;
};

static constexpr usize g_frames_in_flight = 2;

#ifdef PAW_ENABLE_IMGUI
struct ImGuiState
{
	GpuImageHandle font_texture;
	VkSampler font_sampler;
	AllocatedBuffer vertex_buffers[g_frames_in_flight];
	AllocatedBuffer index_buffers[g_frames_in_flight];
	// VkDeviceAddress vertex_buffer_addresses[g_frames_in_flight];
	u32 vertex_buffer_slots[g_frames_in_flight];
	u32 sampler_slot;

	struct PushConstants
	{
		// VkDeviceAddress vertex_buffer_address;
		Vec2 translation;
		Vec2 scale;
		u32 texture_id;
		u32 sampler_index;
		u32 vertex_buffer_slot;
		u32 padding;
	};

	void init();
	void deinit();
	void render(VkCommandBuffer command_buffer);
};
#endif

PAW_ERROR_ON_PADDING_BEGIN
struct DebugGeomLine
{
	Vec4 color;
	Vec3 start;
	Vec3 end;
	f32 start_thickness;
	f32 end_thickness;
	f32 lifetime_seconds;
};

struct DebugGeomConstants
{
	Mat4 view_projection;
	u32 lines_buffer_slot;
	Vec3 camera_position;
};

struct VectorConstants
{
	Mat4 projection;
	u32 vertex_buffer_index;
	Vec2 position;
	u32 gradient_texture_index;
	u32 gradient_sampler_index;
	u32 gradient_count;
	u32 padding[2];
};

struct EditorMeshConstants
{
	Mat4 view_projection;
	u32 vertex_buffer_index;
	byte padding[12];
};

struct DebugCommand2D
{
	Vec2 start;
	Vec2 end;
	Vec2 min_uv;
	Vec2 max_uv;
	LinearColor color;
	u32 texture_index;
	f32 thickness;
};

struct alignas(16) DebugGeom2DConstants
{
	Mat4 view_projection;
	u32 command_buffer_index;
	u32 sampler_index;
	u32 padding[2];
};

struct alignas(16) FullscreenBlitConstants
{
	u32 texture_slot;
	u32 sampler_index;
	u32 padding[2];
};

struct alignas(16) SceneConstants
{
	Mat4 view_projection;
};

struct alignas(16) MeshConstants
{
	Mat4 model;
	u32 vertex_buffer_index;
	u32 vertex_buffer_offset;
	u32 scene_constants_buffer_index;
	u32 padding[1];
};

PAW_ERROR_ON_PADDING_END

struct DebugLineData
{
	AllocatedBuffer line_buffer;
	usize line_count;
	usize max_lines;
	DebugGeomLine* line_buffer_ptr;
	u32 line_buffer_slot;
};

struct DebugCommandData2D
{
	AllocatedBuffer command_buffer;
	usize command_count;
	usize command_max;
	DebugCommand2D* command_buffer_ptr;
	u32 command_buffer_slot;
	u32 sampler_slot;
};

struct GameViewportState
{
	usize width;
	usize height;

	DebugLineData line_data;
	DebugLineData no_depth_line_data;
	DebugCommandData2D debug_2d_data;

	SwfStore swf_store;

	StaticScene static_scene;

	GpuBufferView scene_constants_buffer;
	u32 scene_constants_buffer_slot;
};

static struct
{
	bool initialized;

	VkInstance instance;
	VkSurfaceKHR surface;
	VkDevice device;
	VkQueue graphics_queue;
	VkQueue present_queue;
	VkSurfaceFormatKHR surface_format;
	VkPresentModeKHR present_mode;
	VkFormat depth_format;
	VkPhysicalDevice physical_device;
	u32 graphics_family_index;
	u32 present_family_index;

	VkCommandPool command_pool;

#ifdef PAW_DEBUG
	VkDebugUtilsMessengerEXT debug_messenger;
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_func;
	PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
	PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
	PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
#endif

	PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR;
	PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR;

	VkDescriptorSetLayout descriptor_set_layout;
	VkDescriptorPool descriptor_pool;
	VkDescriptorSet descriptor_set;

	ShaderSlotPool storage_buffer_slots;
	ShaderSlotPool sampled_image_slots;
	ShaderSlotPool sampler_slots;

	VkPipeline pipelines[Pipeline_Count];
	VkPipelineLayout pipeline_layouts[Pipeline_Count];
	PipelineDesc pipeline_descs[Pipeline_Count];
	u64 shader_hashes[Pipeline_Count];
	PipelineReflectionData pipeline_reflection_datas[Pipeline_Count];

	ImageStore image_store;

	Swapchain swapchain;

	FrameFinishData frame_datas[g_frames_in_flight];
	usize frame_index;

	VkFence images_in_flight_fences[Swapchain::max_images];

#ifdef PAW_ENABLE_IMGUI
	ImGuiState imgui;
#endif

	bool received_editor_init_data;

	AllocatedBuffer editor_mesh_vb;
	AllocatedBuffer editor_mesh_ib;
	u32 editor_mesh_index_count;
	u32 editor_mesh_vb_slot;
	bool received_editor_mesh;

	u32 fullscreen_sampler_index;
	VkSampler fullscreen_sampler;

	GameViewportState game;
} g{};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
	VkDebugUtilsMessengerCallbackDataEXT const* callback_data, void* user_data)
{
	PAW_UNUSED_ARG(type);
	PAW_UNUSED_ARG(user_data);
	switch (severity)
	{
			/*case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			{
				Log_info("Validation layer: %s", callback_data->pMessage);
			}
			break;*/

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		{
			PAW_LOG_WARNING("Vulkan: %s", callback_data->pMessage);
		}
		break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		{
			PAW_LOG_ERROR("Vulkan: %s", callback_data->pMessage);
		}
		break;

		default:
			break;
	}

	return VK_FALSE;
}

static VkFormat findSupportedFormat(
	VkPhysicalDevice physical_device, VkFormat const* candidates, usize candidate_count, VkImageTiling tiling,
	VkFormatFeatureFlags features)
{
	for (usize i = 0; i < candidate_count; i++)
	{
		const VkFormat format = candidates[i];
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &properties);
		if ((tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) ||
			(tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features))
		{
			return format;
		}
	}

	PAW_ASSERT(false); // Failed to find supported format
	return VK_FORMAT_UNDEFINED;
}

static bool layerSupported(Slice<VkLayerProperties> supported_layers, char const* layer)
{
	for (usize i = 0; i < supported_layers.size; i++)
	{
		char const* supported_layer = supported_layers[i].layerName;
		if (string_cStringEqual(layer, supported_layer))
		{
			return true;
		}
	}
	return false;
}

static bool extensionSupported(Slice<VkExtensionProperties> supported_extensions, char const* extension)
{
	for (usize i = 0; i < supported_extensions.size; i++)
	{
		char const* supported_extension = supported_extensions[i].extensionName;
		if (string_cStringEqual(extension, supported_extension))
		{
			return true;
		}
	}
	return false;
}

static void addToArrayIfUnique(u32* array, u32* size, u32 value)
{
	bool exists = false;
	for (u32 i = 0; i < *size; i++)
	{
		if (array[i] == value)
		{
			exists = true;
			break;
		}
	}
	if (!exists)
	{
		array[*size] = value;
		size++;
	}
}

static void vulkanInit(Platform& /*platform*/, GameStartupArgs const& /* startup_args */, PagePool& page_pool)
{
	PAW_PROFILER_FUNC();
	ArenaAllocator arena{};
	arena.init(&page_pool, "Vulkan Scratch Allocator"_str);

	Defer arena_reset_defer = {[&arena]
							   { arena.reset(); }};

	{
		PAW_PROFILER_SCOPE("Create Instance");
		VkApplicationInfo app_info{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "Pawprint Game",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.pEngineName = "Pawprint",
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = VK_API_VERSION_1_3,
		};

		char const* required_extensions[] = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		};

		VkInstanceCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &app_info,
			.enabledExtensionCount = PAW_ARRAY_COUNT(required_extensions),
			.ppEnabledExtensionNames = required_extensions,
		};

#ifdef PAW_DEBUG
		char const* validation_layers[] = {
			"VK_LAYER_KHRONOS_validation",
		};

		u32 supported_layer_count = 0;
		vkEnumerateInstanceLayerProperties(&supported_layer_count, 0);

		const ArenaMarker arena_marker = arena.get_marker();
		Slice<VkLayerProperties> supported_layers = arena.alloc<VkLayerProperties>(supported_layer_count);
		Defer supported_layers_defer = {[&arena, arena_marker]
										{ arena.reset_to_marker(arena_marker); }};
		vkEnumerateInstanceLayerProperties(&supported_layer_count, supported_layers.ptr);

		for (usize i = 0; i < PAW_ARRAY_COUNT(validation_layers); i++)
		{
			char const* layer = validation_layers[i];
			bool supported = layerSupported(supported_layers, layer);
			PAW_ASSERT(supported);
			PAW_ASSERT_UNUSED(supported);
		}

		create_info.enabledLayerCount = PAW_ARRAY_COUNT(validation_layers);
		create_info.ppEnabledLayerNames = validation_layers;
#else
		create_info.enabledLayerCount = 0;
#endif

#ifdef PAW_DEBUG
		VkDebugUtilsMessengerCreateInfoEXT debug_create_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = &debugCallback,
		};
		create_info.pNext = &debug_create_info;
#endif

		VkResult result = vkCreateInstance(&create_info, 0, &g.instance);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	{
#ifdef PAW_DEBUG
		VkDebugUtilsMessengerCreateInfoEXT create_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = &debugCallback,
		};

		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT_func =
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g.instance, "vkCreateDebugUtilsMessengerEXT");
		PAW_ASSERT(vkCreateDebugUtilsMessengerEXT_func);
		VkResult result = vkCreateDebugUtilsMessengerEXT_func(g.instance, &create_info, 0, &g.debug_messenger);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);

		g.vkDestroyDebugUtilsMessengerEXT_func =
			(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g.instance, "vkDestroyDebugUtilsMessengerEXT");
		PAW_ASSERT(g.vkDestroyDebugUtilsMessengerEXT_func);

		g.vkCmdBeginDebugUtilsLabelEXT =
			(PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(g.instance, "vkCmdBeginDebugUtilsLabelEXT");
		PAW_ASSERT(g.vkCmdBeginDebugUtilsLabelEXT);
		g.vkCmdEndDebugUtilsLabelEXT =
			(PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(g.instance, "vkCmdEndDebugUtilsLabelEXT");
		PAW_ASSERT(g.vkCmdEndDebugUtilsLabelEXT);
		g.vkSetDebugUtilsObjectNameEXT =
			(PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(g.instance, "vkSetDebugUtilsObjectNameEXT");
		PAW_ASSERT(g.vkSetDebugUtilsObjectNameEXT);
#endif

		g.vkGetMemoryWin32HandleKHR =
			(PFN_vkGetMemoryWin32HandleKHR)vkGetInstanceProcAddr(g.instance, "vkGetMemoryWin32HandleKHR");
		PAW_ASSERT(g.vkGetMemoryWin32HandleKHR);

		g.vkGetMemoryWin32HandlePropertiesKHR = (PFN_vkGetMemoryWin32HandlePropertiesKHR)vkGetInstanceProcAddr(
			g.instance, "vkGetMemoryWin32HandlePropertiesKHR");
	}

	{
		// TODO(nathan): This needs to be done better
		const VkWin32SurfaceCreateInfoKHR surface_create_info{
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.hinstance = GetModuleHandle(NULL),
			.hwnd = GetActiveWindow(),
		};
		VkResult result = vkCreateWin32SurfaceKHR(g.instance, &surface_create_info, nullptr, &g.surface);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	{
		char const* required_device_extensions[] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
			VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,
			VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME,
			VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
			VK_KHR_WIN32_KEYED_MUTEX_EXTENSION_NAME,
			/*VK_KHR_PRESENT_WAIT_EXTENSION_NAME,
			VK_KHR_PRESENT_ID_EXTENSION_NAME,*/
		};

		{
			PAW_PROFILER_SCOPE("Pick Physical Device");
			u32 device_count = 0;
			vkEnumeratePhysicalDevices(g.instance, &device_count, 0);
			PAW_ASSERT(device_count > 0);

			const ArenaMarker possible_devices_marker = arena.get_marker();
			Slice<VkPhysicalDevice> possible_devices = arena.alloc<VkPhysicalDevice>(device_count);
			Defer possible_devices_defer = {[&arena, possible_devices_marker]
											{ arena.reset_to_marker(possible_devices_marker); }};
			vkEnumeratePhysicalDevices(g.instance, &device_count, possible_devices.ptr);

			bool found_device = false;

			for (u32 device_index = 0; device_index < device_count; device_index++)
			{
				VkPhysicalDevice device = possible_devices[device_index];
				VkPhysicalDeviceProperties properties;
				vkGetPhysicalDeviceProperties(device, &properties);
				VkPhysicalDeviceFeatures features;
				vkGetPhysicalDeviceFeatures(device, &features);
				u32 family_count = 0;
				bool found_graphics_family = false;
				u32 graphics_family_index = 0;
				bool found_present_family = false;
				u32 present_family_index = 0;

				{
					const ArenaMarker families_marker = arena.get_marker();
					vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, 0);
					Slice<VkQueueFamilyProperties> queue_families = arena.alloc<VkQueueFamilyProperties>(family_count);

					vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, queue_families.ptr);

					for (u32 i = 0; i < family_count; i++)
					{
						VkQueueFamilyProperties const* family = &queue_families[i];

						VkBool32 present_support = false;
						vkGetPhysicalDeviceSurfaceSupportKHR(device, i, g.surface, &present_support);
						if (present_support)
						{
							found_present_family = true;
							present_family_index = i;
						}

						if ((family->queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
						{
							found_graphics_family = true;
							graphics_family_index = i;
						}

						if (found_graphics_family && found_present_family)
						{
							break;
						}
					}
					arena.reset_to_marker(families_marker);
				}

				bool all_extensions_supported = true;
				{
					u32 supported_extension_count = 0;
					vkEnumerateDeviceExtensionProperties(device, 0, &supported_extension_count, 0);

					const ArenaMarker extensions_marker = arena.get_marker();
					Slice<VkExtensionProperties> supported_extensions =
						arena.alloc<VkExtensionProperties>(supported_extension_count);

					vkEnumerateDeviceExtensionProperties(
						device, 0, &supported_extension_count, supported_extensions.ptr);

					for (usize i = 0; i < PAW_ARRAY_COUNT(required_device_extensions); i++)
					{
						char const* required_extension = required_device_extensions[i];
						if (!extensionSupported(supported_extensions, required_extension))
						{
							PAW_LOG_ERROR("%s is not supported", required_extension);
							all_extensions_supported = false;
							break;
						}
					}
					arena.reset_to_marker(extensions_marker);
				}

				VkPhysicalDeviceFeatures supported_features;
				vkGetPhysicalDeviceFeatures(device, &supported_features);

				u32 present_mode_count = 0;
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, g.surface, &present_mode_count, 0);

				u32 surface_format_count = 0;
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, g.surface, &surface_format_count, 0);

				VkPhysicalDeviceScalarBlockLayoutFeatures scalar_block_features{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES,
				};

				VkPhysicalDeviceBufferDeviceAddressFeatures device_address_features{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
					.pNext = &scalar_block_features,
				};

				VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
					.pNext = &device_address_features,
				};

				VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
					.pNext = &dynamic_rendering_features,
				};

				VkPhysicalDeviceFeatures2 extended_device_features{
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
					.pNext = &indexing_features,
				};

				vkGetPhysicalDeviceFeatures2(device, &extended_device_features);

				if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && found_graphics_family &&
					found_present_family && all_extensions_supported && surface_format_count > 0 &&
					present_mode_count > 0 && supported_features.samplerAnisotropy &&
					indexing_features.descriptorBindingPartiallyBound && indexing_features.runtimeDescriptorArray &&
					indexing_features.descriptorBindingUniformBufferUpdateAfterBind &&
					indexing_features.shaderSampledImageArrayNonUniformIndexing &&
					indexing_features.descriptorBindingStorageBufferUpdateAfterBind &&
					dynamic_rendering_features.dynamicRendering && device_address_features.bufferDeviceAddress &&
					extended_device_features.features.shaderInt64 && scalar_block_features.scalarBlockLayout &&
					supported_features.vertexPipelineStoresAndAtomics && supported_features.sampleRateShading)
				{
					g.physical_device = device;
					g.graphics_family_index = graphics_family_index;
					g.present_family_index = present_family_index;
					found_device = true;

					PAW_LOG_INFO("Gpu: %s", properties.deviceName);

					break;
				}
			}
			PAW_ASSERT(found_device);
		}

		{
			PAW_PROFILER_SCOPE("Create Device");

			static constexpr usize max_queue_families = 2;
			u32 unique_queue_families[max_queue_families];
			u32 unique_queue_families_count = 1;
			unique_queue_families[0] = g.graphics_family_index;
			addToArrayIfUnique(unique_queue_families, &unique_queue_families_count, g.present_family_index);

			const f32 queue_priority = 1.0f;

			VkDeviceQueueCreateInfo queue_create_infos[max_queue_families];
			for (u32 i = 0; i < unique_queue_families_count; i++)
			{
				VkDeviceQueueCreateInfo create_info{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.queueFamilyIndex = unique_queue_families[i],
					.queueCount = 1,
					.pQueuePriorities = &queue_priority,
				};
				queue_create_infos[i] = create_info;
			}

			VkPhysicalDeviceScalarBlockLayoutFeatures scalar_block_features{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES,
				.scalarBlockLayout = VK_TRUE,
			};

			VkPhysicalDeviceBufferDeviceAddressFeatures device_address_features{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
				.pNext = &scalar_block_features,
				.bufferDeviceAddress = VK_TRUE,
			};

			VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
				.pNext = &device_address_features,
				.dynamicRendering = VK_TRUE,
			};

			VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
				.pNext = &dynamic_rendering_features,
				.shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
				.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE,
				.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
				.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
				.descriptorBindingPartiallyBound = VK_TRUE,
				.runtimeDescriptorArray = VK_TRUE,
			};

			const VkPhysicalDeviceFeatures2 extended_device_features{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
				.pNext = &indexing_features,
				.features = {
					.sampleRateShading = VK_TRUE,
					.samplerAnisotropy = VK_TRUE,
					.vertexPipelineStoresAndAtomics = VK_TRUE,
					.shaderInt64 = VK_TRUE,
				}};

			VkDeviceCreateInfo create_info{
				.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				.pNext = &extended_device_features,
				.queueCreateInfoCount = unique_queue_families_count,
				.pQueueCreateInfos = queue_create_infos,
				.enabledExtensionCount = PAW_ARRAY_COUNT(required_device_extensions),
				.ppEnabledExtensionNames = required_device_extensions,
			};

			{
				PAW_PROFILER_SCOPE("vkCreateDevice");
				VkResult result = vkCreateDevice(g.physical_device, &create_info, 0, &g.device);
				PAW_ASSERT(result == VK_SUCCESS);
				PAW_ASSERT_UNUSED(result);
			}

			vkGetDeviceQueue(g.device, g.graphics_family_index, 0, &g.graphics_queue);
			vkGetDeviceQueue(g.device, g.present_family_index, 0, &g.present_queue);
		}
	}

	{
		PAW_PROFILER_SCOPE("Query Surface Format");

		VkSurfaceCapabilitiesKHR surface_capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g.physical_device, g.surface, &surface_capabilities);

		u32 surface_format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(g.physical_device, g.surface, &surface_format_count, 0);

		const ArenaMarker surface_formats_marker = arena.get_marker();
		Slice<VkSurfaceFormatKHR> surface_formats = arena.alloc<VkSurfaceFormatKHR>(surface_format_count);
		Defer surface_formats_defer = {[&arena, surface_formats_marker]
									   { arena.reset_to_marker(surface_formats_marker); }};
		vkGetPhysicalDeviceSurfaceFormatsKHR(g.physical_device, g.surface, &surface_format_count, surface_formats.ptr);

		VkSurfaceFormatKHR surface_format = surface_formats[0];

		for (u32 i = 0; i < surface_format_count; i++)
		{
			const VkSurfaceFormatKHR format = surface_formats[i];
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				surface_format = format;
				break;
			}
		}

		g.surface_format = surface_format;

		u32 present_mode_count = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(g.physical_device, g.surface, &present_mode_count, 0);

		const ArenaMarker present_modes_marker = arena.get_marker();
		Slice<VkPresentModeKHR> present_modes = arena.alloc<VkPresentModeKHR>(present_mode_count);
		Defer present_modes_defer = {[&arena, present_modes_marker]
									 { arena.reset_to_marker(present_modes_marker); }};

		vkGetPhysicalDeviceSurfacePresentModesKHR(g.physical_device, g.surface, &present_mode_count, present_modes.ptr);

		VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
		const VkPresentModeKHR desired_present_mode = VK_PRESENT_MODE_FIFO_KHR;
		for (u32 i = 0; i < present_mode_count; i++)
		{
			if (present_modes[i] == desired_present_mode)
			{
				present_mode = desired_present_mode;
				break;
			}
		}

		g.present_mode = present_mode;

		const VkFormat depth_candidates[]{
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
		};

		VkFormat depth_format = findSupportedFormat(
			g.physical_device,
			depth_candidates,
			PAW_ARRAY_COUNT(depth_candidates),
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		g.depth_format = depth_format;
	}

	{
		PAW_PROFILER_SCOPE("Create Command Pool");

		VkCommandPoolCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = g.graphics_family_index,
		};

		VkResult result = vkCreateCommandPool(g.device, &create_info, 0, &g.command_pool);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}
}

static void vulkanDeinit()
{
	vkDestroyCommandPool(g.device, g.command_pool, nullptr);

#ifdef PAW_DEBUG
	g.vkDestroyDebugUtilsMessengerEXT_func(g.instance, g.debug_messenger, nullptr);
#endif
	vkDestroyDevice(g.device, nullptr);
	vkDestroySurfaceKHR(g.instance, g.surface, nullptr);
	vkDestroyInstance(g.instance, nullptr);
}

static void setBufferDebugName(VkBuffer buffer, char const* name)
{
#ifdef PAW_DEBUG
	const VkDebugUtilsObjectNameInfoEXT name_info{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.objectType = VK_OBJECT_TYPE_BUFFER,
		.objectHandle = reinterpret_cast<u64>(buffer),
		.pObjectName = name,
	};

	g.vkSetDebugUtilsObjectNameEXT(g.device, &name_info);
#else
	PAW_UNUSED_ARG(buffer);
	PAW_UNUSED_ARG(name);
#endif
}

static void pushDebugGroup(VkCommandBuffer command_buffer, char const* name)
{
#ifdef PAW_DEBUG
	const VkDebugUtilsLabelEXT label{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
		.pLabelName = name,
	};
	g.vkCmdBeginDebugUtilsLabelEXT(command_buffer, &label);
#else
	PAW_UNUSED_ARG(command_buffer);
	PAW_UNUSED_ARG(name);
#endif
}

static void popDebugGroup(VkCommandBuffer command_buffer)
{
#ifdef PAW_DEBUG
	g.vkCmdEndDebugUtilsLabelEXT(command_buffer);
#else
	PAW_UNUSED_ARG(command_buffer);
#endif
}

static void allocCommandBuffers(usize count, VkCommandBuffer* out_buffers)
{
	VkCommandBufferAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = g.command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<u32>(count),
	};

	VkResult result = vkAllocateCommandBuffers(g.device, &alloc_info, out_buffers);
	PAW_ASSERT(result == VK_SUCCESS);
	PAW_ASSERT_UNUSED(result);
}

static void freeCommandBuffers(Slice<VkCommandBuffer> const& buffers)
{
	vkFreeCommandBuffers(g.device, g.command_pool, static_cast<u32>(buffers.size), buffers.ptr);
}

VkPipeline createGraphicsPipeline(GraphicsPipelineDesc const& desc, VkPipelineLayout layout)
{
	const VkPipelineShaderStageCreateInfo shader_stage_infos[]{
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = desc.vert_shader,
			.pName = "main",
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = desc.frag_shader,
			.pName = "main",
		},
	};

	const VkPipelineVertexInputStateCreateInfo vertex_input_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	};

	const VkPipelineInputAssemblyStateCreateInfo input_assembly_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};

	const VkPipelineViewportStateCreateInfo viewport_state_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount = 1,
	};

	const VkPipelineRasterizationStateCreateInfo rasterizer_state_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = desc.cull_mode,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
		.lineWidth = 1.0f,
	};

	const u32 sample_mask = 0xFFFFFFFF;

	const VkPipelineMultisampleStateCreateInfo multisampling_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = desc.sample_count,
		.sampleShadingEnable = desc.sample_shading,
		.minSampleShading = desc.min_sample_shading,
		.pSampleMask = &sample_mask,
		.alphaToCoverageEnable = desc.alpha_to_coverage,
		.alphaToOneEnable = VK_FALSE,
	};

	const VkPipelineColorBlendAttachmentState color_blend_attachment_state{
		.blendEnable = desc.blend_state.enabled,
		.srcColorBlendFactor = desc.blend_state.src_color_factor,
		.dstColorBlendFactor = desc.blend_state.dst_color_factor,
		.colorBlendOp = desc.blend_state.color_op,
		.srcAlphaBlendFactor = desc.blend_state.src_alpha_factor,
		.dstAlphaBlendFactor = desc.blend_state.dst_alpha_factor,
		.alphaBlendOp = desc.blend_state.alpha_op,
		.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
	};

	const VkPipelineColorBlendStateCreateInfo color_blend_state_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment_state,
		.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
	};

	const VkPipelineDepthStencilStateCreateInfo depth_stencil_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = desc.depth_test,
		.depthWriteEnable = desc.depth_write,
		.depthCompareOp = desc.depth_compare_op,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f,
	};

	const VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

	const VkPipelineDynamicStateCreateInfo dynamic_state_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = PAW_ARRAY_COUNT(dynamic_states),
		.pDynamicStates = dynamic_states,
	};

	const VkPipelineRenderingCreateInfo pipeline_rendering_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &desc.color_format,
		.depthAttachmentFormat = desc.depth_format,
	};

	const VkGraphicsPipelineCreateInfo pipeline_create_info{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &pipeline_rendering_create_info,
		.stageCount = PAW_ARRAY_COUNT(shader_stage_infos),
		.pStages = shader_stage_infos,
		.pVertexInputState = &vertex_input_info,
		.pInputAssemblyState = &input_assembly_info,
		.pViewportState = &viewport_state_info,
		.pRasterizationState = &rasterizer_state_info,
		.pMultisampleState = &multisampling_info,
		.pDepthStencilState = &depth_stencil_info,
		.pColorBlendState = &color_blend_state_info,
		.pDynamicState = &dynamic_state_info,
		.layout = layout,
		.renderPass = VK_NULL_HANDLE,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1,
	};

	VkPipeline pipeline;
	VkResult result = vkCreateGraphicsPipelines(g.device, VK_NULL_HANDLE, 1, &pipeline_create_info, 0, &pipeline);
	PAW_ASSERT(result == VK_SUCCESS);
	PAW_ASSERT_UNUSED(result);
	return pipeline;
}

static VkPipelineLayout createPipelineLayout(PipelineReflectionData const& reflection_data)
{
	const VkPushConstantRange push_constants[] = {{
		.stageFlags = VK_SHADER_STAGE_ALL, // TODO: Should probably detect which stages require them
		.offset = 0,
		.size = static_cast<u32>(reflection_data.push_constant_size),
	}};

	const VkPipelineLayoutCreateInfo pipeline_layout_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 1,
		.pSetLayouts = &g.descriptor_set_layout,
		.pushConstantRangeCount = reflection_data.push_constant_size > 0 ? 1u : 0u,
		.pPushConstantRanges = push_constants,
	};

	VkPipelineLayout out_layout;
	VkResult result = vkCreatePipelineLayout(g.device, &pipeline_layout_create_info, 0, &out_layout);
	PAW_ASSERT(result == VK_SUCCESS);
	PAW_ASSERT_UNUSED(result);
	return out_layout;
}

static constexpr u64 g_seed = 0x353AF014F5AAB899;

static void reflectShader(Slice<byte> shader_mem, UTF8StringView const& name, PipelineReflectionData& out_data)
{
	PAW_ASSERT(reinterpret_cast<u64>(shader_mem.ptr) % 4 == 0); // make sure it's aligned to 32 bits

	spv_reflect::ShaderModule reflect_module(shader_mem.size, shader_mem.ptr, SPV_REFLECT_MODULE_FLAG_NO_COPY);
	u32 push_constant_count = 0;
	reflect_module.EnumeratePushConstantBlocks(&push_constant_count, nullptr);
	if (push_constant_count > 0)
	{
		SpvReflectBlockVariable const* push_constant = reflect_module.GetPushConstantBlock(0);
		PAW_ASSERT(push_constant->size == push_constant->padded_size);
		const usize push_constant_size = static_cast<usize>(push_constant->size);
		if (out_data.push_constant_size == 0)
		{
			out_data.push_constant_size = push_constant_size;
		}
		else if (out_data.push_constant_size != push_constant_size)
		{
			PAW_LOG_ERROR("Shader %p{str} contains mismatching push constant sizes", &name);
		}
	}

	u32 descriptor_set_count = 0;
	reflect_module.EnumerateDescriptorSets(&descriptor_set_count, nullptr);
	if (descriptor_set_count > 0)
	{
		SpvReflectDescriptorSet const* descriptor_set = reflect_module.GetDescriptorSet(0);
		for (u32 i = 0; i < descriptor_set->binding_count; i++)
		{
			SpvReflectDescriptorBinding const* binding = descriptor_set->bindings[i];
			if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER &&
				binding->block.member_count > 0)
			{
				SpvReflectBlockVariable const& values_var = binding->block.members[0];

				const UTF8StringView struct_name{
					values_var.type_description->type_name,
					string_cStringLength(values_var.type_description->type_name),
					true,
				};

				const u64 struct_name_hash = hashUTF8StringViewToStringID(struct_name);

				PAW_ASSERT(values_var.size == values_var.padded_size);
				const usize struct_size = static_cast<usize>(values_var.size);

				PipelineReflectionData::Buffer const* buffer = out_data.findBuffer(struct_name_hash);
				if (buffer != nullptr && buffer->struct_size != struct_size)
				{
					PAW_LOG_ERROR("Duplicate struct %p{str} exists in shader %p{str}", struct_name, name);
				}
				else
				{
					out_data.emplaceBuffer(PipelineReflectionData::Buffer{
						.struct_size = struct_size,
						.struct_name_hash = struct_name_hash,
					});
				}
			}
		}
	}
}

static VkShaderModule createShader(Slice<byte> mem)
{
	PAW_ASSERT(reinterpret_cast<u64>(mem.ptr) % 4 == 0); // make sure it's aligned to 32 bits

	VkShaderModuleCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = mem.size,
		.pCode = reinterpret_cast<u32*>(mem.ptr),
	};
	VkShaderModule module = VK_NULL_HANDLE;
	VkResult result = vkCreateShaderModule(g.device, &create_info, 0, &module);
	PAW_ASSERT(result == VK_SUCCESS);
	PAW_ASSERT_UNUSED(result);
	return module;
}

static void destroyPipeline(Pipeline pipeline)
{
	vkDestroyPipeline(g.device, g.pipelines[pipeline], nullptr);
	vkDestroyPipelineLayout(g.device, g.pipeline_layouts[pipeline], nullptr);
}

static void createGraphicsPipeline(
	Platform& platform, UTF8StringView const& shader, Pipeline pipeline_index, GraphicsPipelineDesc const& desc)
{
	PipelineReflectionData& reflection_data = g.pipeline_reflection_datas[pipeline_index];
	reflection_data.reset();
	reflection_data.shader_name = shader;
	reflection_data.type = PipelineReflectionData::Type::Graphics;

	byte path_buffer[256];
	const UTF8StringView vert_path = string_formatNullTerminated(
		path_buffer, PAW_ARRAY_COUNT(path_buffer), "compiled_data/shaders/%p{str}.vert.spv", &shader);
	Slice<byte> vert_file = platform.DEBUG_load_file_blocking(vert_path, platform.get_system_allocator());
	VkShaderModule vert_shader = createShader(vert_file);
	reflectShader(vert_file, shader, reflection_data);
	platform.get_system_allocator().free(vert_file);

	const UTF8StringView frag_path = string_formatNullTerminated(
		path_buffer, PAW_ARRAY_COUNT(path_buffer), "compiled_data/shaders/%p{str}.frag.spv", &shader);
	Slice<byte> frag_file = platform.DEBUG_load_file_blocking(frag_path, platform.get_system_allocator());
	VkShaderModule frag_shader = createShader(frag_file);
	reflectShader(frag_file, shader, reflection_data);
	platform.get_system_allocator().free(frag_file);

	VkPipelineLayout layout = createPipelineLayout(reflection_data);

	GraphicsPipelineDesc new_desc = desc;
	new_desc.vert_shader = vert_shader;
	new_desc.frag_shader = frag_shader;

	const u64 hash = murmur64(path_buffer, frag_path.size_bytes - 9, g_seed);

	g.pipeline_descs[pipeline_index] = PipelineDesc{
		.type = PipelineType::Graphics,
		.graphics = new_desc,
	};
	g.shader_hashes[pipeline_index] = hash;
	g.pipelines[pipeline_index] = createGraphicsPipeline(new_desc, layout);
	g.pipeline_layouts[pipeline_index] = layout;
}

static void createComputePipeline(
	Platform& platform, UTF8StringView const& shader, Pipeline pipeline_index, ComputePipelineDesc const& desc)
{
	PipelineReflectionData& reflection_data = g.pipeline_reflection_datas[pipeline_index];
	reflection_data.reset();
	reflection_data.shader_name = shader;
	reflection_data.type = PipelineReflectionData::Type::Compute;

	byte path_buffer[256];
	const UTF8StringView path =
		string_format(path_buffer, PAW_ARRAY_COUNT(path_buffer), "compiled_data/shaders/%p{str}.comp.spv", &shader);

	Slice<byte> file = platform.DEBUG_load_file_blocking(path, platform.get_system_allocator());
	VkShaderModule shader_module = createShader(file);
	reflectShader(file, shader, reflection_data);
	platform.get_system_allocator().free(file);

	VkPipelineLayout layout = createPipelineLayout(reflection_data);

	const u64 hash = murmur64(path_buffer, path.size_bytes - 9, g_seed);

	const VkComputePipelineCreateInfo create_info{
		.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		.stage =
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_COMPUTE_BIT,
				.module = shader_module,
				.pName = "main",
			},
		.layout = layout,
	};

	VkPipeline* pipeline = &g.pipelines[pipeline_index];
	vkCreateComputePipelines(g.device, VK_NULL_HANDLE, 1, &create_info, nullptr, pipeline);

	g.pipeline_descs[pipeline_index] = PipelineDesc{
		.type = PipelineType::Compute,
		.compute = desc,
	};
	g.shader_hashes[pipeline_index] = hash;
	g.pipeline_layouts[pipeline_index] = layout;
}

static void verifyPipelineStruct(Pipeline pipeline, StringID name, usize size)
{
#ifdef PAW_DEBUG
	PipelineReflectionData const& reflection_data = g.pipeline_reflection_datas[pipeline];
	PipelineReflectionData::Buffer const* buffer = reflection_data.findBuffer(name);
	if (buffer != nullptr)
	{
		PAW_ASSERT(buffer->struct_size == size);
		if (buffer->struct_size != size)
		{
			PAW_LOG_ERROR(
				"Shader Struct size mismatch:\n"
				"\tStruct: %p{str}\n"
				"\tShader: %p{str}\n"
				"\tCode Size: %llu bytes\n"
				"\tShader Size: %llu",
				&name,
				&reflection_data.shader_name,
				size,
				buffer->struct_size);
		}
	}
	else
	{
		PAW_LOG_ERROR("Buffer of type %p{str} does not exist in %p{str} shader", name, reflection_data.shader_name);
	}
#else
	PAW_UNUSED_ARG(pipeline);
	PAW_UNUSED_ARG(name);
	PAW_UNUSED_ARG(size);
#endif
}

static void verifyPipelinePushConstantInternal(Pipeline pipeline, usize size)
{
	PipelineReflectionData const& reflection_data = g.pipeline_reflection_datas[pipeline];
	if (reflection_data.push_constant_size > 0)
	{
		if (reflection_data.push_constant_size != size)
		{
			PAW_LOG_ERROR(
				"Push constant size mismatch:\n"
				"\tShader: %p{str}\n"
				"\tCode Size: %llu\n"
				"\tShader Size: %llu",
				&reflection_data.shader_name,
				size,
				reflection_data.push_constant_size);
		}
	}
	else
	{
		PAW_LOG_ERROR("No push constants found in %p{str} shader", &reflection_data.shader_name);
	}
}

void bindPipeline(VkCommandBuffer command_buffer, Pipeline pipeline_index)
{
	PipelineReflectionData const& reflection_data = g.pipeline_reflection_datas[pipeline_index];
	VkPipelineLayout layout = g.pipeline_layouts[pipeline_index];
	VkPipeline pipeline = g.pipelines[pipeline_index];
	const VkPipelineBindPoint bind_point = reflection_data.type == PipelineReflectionData::Type::Compute
		? VK_PIPELINE_BIND_POINT_COMPUTE
		: VK_PIPELINE_BIND_POINT_GRAPHICS;
	vkCmdBindDescriptorSets(command_buffer, bind_point, layout, 0, 1, &g.descriptor_set, 0, nullptr);
	vkCmdBindPipeline(command_buffer, bind_point, pipeline);
}

void pushPipelineConstantsInternal(VkCommandBuffer command_buffer, Pipeline pipeline, usize size, void const* data)
{
	VkPipelineLayout layout = g.pipeline_layouts[pipeline];
	PipelineReflectionData const& reflection_data = g.pipeline_reflection_datas[pipeline];
	PAW_ASSERT(size == reflection_data.push_constant_size);
	PAW_ASSERT_UNUSED(reflection_data);
	// TODO: detect correct stages
	vkCmdPushConstants(command_buffer, layout, VK_SHADER_STAGE_ALL, 0, static_cast<u32>(size), data);
}

static u32 pushSamplerToShader(VkSampler sampler)
{
	u32 slot = g.sampler_slots.alloc();
	const VkDescriptorImageInfo sampler_info{
		.sampler = sampler,
	};

	const VkWriteDescriptorSet descriptor_write{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = g.descriptor_set,
		.dstBinding = BindSlot_Samplers,
		.dstArrayElement = slot,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
		.pImageInfo = &sampler_info,
	};

	vkUpdateDescriptorSets(g.device, 1, &descriptor_write, 0, nullptr);
	return slot;
}

static void setBufferInShader(VkBuffer buffer, usize offset, usize size, u32 slot)
{
	const VkDescriptorBufferInfo buffer_info{
		.buffer = buffer,
		.offset = offset,
		.range = size,
	};

	const VkWriteDescriptorSet descriptor_write{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = g.descriptor_set,
		.dstBinding = BindSlot_StorageBuffers,
		.dstArrayElement = slot,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.pBufferInfo = &buffer_info,
	};

	vkUpdateDescriptorSets(g.device, 1, &descriptor_write, 0, nullptr);
}

static u32 pushBufferToShader(VkBuffer buffer, usize offset, usize size)
{
	u32 slot = g.storage_buffer_slots.alloc();
	setBufferInShader(buffer, offset, size, slot);
	return slot;
}

static void freeSampledImageSlot(u32 slot)
{
	g.sampled_image_slots.free(slot);
}

static void freeBufferSlot(u32 slot)
{
	g.storage_buffer_slots.free(slot);
}

static VkBufferMemoryBarrier bufferBarrier(
	VkBuffer buffer, VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask, VkDeviceSize size = VK_WHOLE_SIZE)
{
	VkBufferMemoryBarrier result = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
		.srcAccessMask = src_access_mask,
		.dstAccessMask = dst_access_mask,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.buffer = buffer,
		.offset = 0,
		.size = size,
	};

	return result;
}

static void transitionImageLayout(
	VkCommandBuffer command_buffer, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout)
{

	VkPipelineStageFlags source_stage = VK_PIPELINE_STAGE_NONE_KHR;
	VkPipelineStageFlags destination_stage = VK_PIPELINE_STAGE_NONE_KHR;
	VkAccessFlags src_access_mask = VK_ACCESS_NONE_KHR;
	VkAccessFlags dst_access_mask = VK_ACCESS_NONE_KHR;

	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		src_access_mask = 0;
		dst_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;

		source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (
		old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		src_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
		dst_access_mask = VK_ACCESS_SHADER_READ_BIT;

		source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		PAW_UNREACHABLE; // unsupported layout transition
	}

	VkImageMemoryBarrier barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcAccessMask = src_access_mask,
		.dstAccessMask = dst_access_mask,
		.oldLayout = old_layout,
		.newLayout = new_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange =
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
	};

	vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, 0, 0, 0, 1, &barrier);
}

static u32 findMemoryType(u32 type_filter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(g.physical_device, &memory_properties);

	for (u32 i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	PAW_UNREACHABLE;
	return 0;
}

void* AllocatedBuffer::map()
{
	void* ptr;
	VkResult result = vkMapMemory(g.device, memory, 0, size, 0, &ptr);
	PAW_ASSERT(result == VK_SUCCESS);
	PAW_ASSERT_UNUSED(result);
	return ptr;
}

VkDeviceAddress AllocatedBuffer::getDeviceAddress() const
{
	const VkBufferDeviceAddressInfo address_info{
		.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
		.buffer = buffer,
	};
	return vkGetBufferDeviceAddress(g.device, &address_info);
}

void AllocatedBuffer::unmap()
{
	vkUnmapMemory(g.device, memory);
}

static AllocatedBuffer createAndAllocateBuffer(usize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_props)
{
	const VkBufferCreateInfo buffer_create_info{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};

	VkBuffer buffer;

	{
		VkResult result = vkCreateBuffer(g.device, &buffer_create_info, nullptr, &buffer);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(g.device, buffer, &memory_requirements);

	VkMemoryAllocateFlagsInfo flags_info{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
		.flags = (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) ? VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
																	 : static_cast<VkMemoryAllocateFlags>(0),
		.deviceMask = 0,
	};

	const VkMemoryAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = &flags_info,
		.allocationSize = memory_requirements.size,
		.memoryTypeIndex = findMemoryType(memory_requirements.memoryTypeBits, memory_props),

	};

	VkDeviceMemory memory;
	{
		VkResult result = vkAllocateMemory(g.device, &alloc_info, nullptr, &memory);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	{
		VkResult result = vkBindBufferMemory(g.device, buffer, memory, 0);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	return AllocatedBuffer{.buffer = buffer, .memory = memory, .size = size};
}

static VkCommandBuffer beginSingleTimeCommandBuffer()
{
	VkCommandBuffer command_buffer;

	allocCommandBuffers(1, &command_buffer);

	const VkCommandBufferBeginInfo begin_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};

	VkResult begin_result = vkBeginCommandBuffer(command_buffer, &begin_info);
	PAW_ASSERT(begin_result == VK_SUCCESS);
	PAW_ASSERT_UNUSED(begin_result);

	return command_buffer;
}

static void endSingleTimeCommandBuffer(VkCommandBuffer command_buffer)
{
	vkEndCommandBuffer(command_buffer);

	const VkSubmitInfo submit_info{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &command_buffer,
	};

	vkQueueSubmit(g.graphics_queue, 1, &submit_info, nullptr);
	vkQueueWaitIdle(g.graphics_queue);

	freeCommandBuffers({&command_buffer, 1});
}

static GpuImageHandle createAndUploadImage(
	byte const* pixels, usize width, usize height, usize bytes_per_pixel, VkFormat format, VkImageAspectFlags aspect,
	UTF8StringView debug_name, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT)
{
	const usize image_size = width * height * bytes_per_pixel;
	AllocatedBuffer staging = createAndAllocateBuffer(
		image_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* staging_mem_dest = staging.map();

	memCopy(pixels, staging_mem_dest, image_size);
	staging.unmap();

	GpuImageHandle image_handle = g.image_store.createAndAllocate(
		width,
		height,
		format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		aspect,
		debug_name,
		samples);

	VkCommandBuffer command_buffer = beginSingleTimeCommandBuffer();

	transitionImageLayout(
		command_buffer,
		g.image_store.getVkImage(image_handle),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	const VkBufferImageCopy region{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource =
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		.imageOffset = {0, 0, 0},
		.imageExtent =
			{
				.width = static_cast<u32>(width),
				.height = static_cast<u32>(height),
				.depth = 1,
			},
	};

	vkCmdCopyBufferToImage(
		command_buffer,
		staging.buffer,
		g.image_store.getVkImage(image_handle),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region);

	transitionImageLayout(
		command_buffer,
		g.image_store.getVkImage(image_handle),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	endSingleTimeCommandBuffer(command_buffer);
	vkDestroyBuffer(g.device, staging.buffer, nullptr);
	vkFreeMemory(g.device, staging.memory, nullptr);

	return image_handle;
}

static AllocatedBuffer createAndUploadBuffer(Slice<byte const> const buffer_data, VkBufferUsageFlags usage)
{
	AllocatedBuffer staging = createAndAllocateBuffer(
		buffer_data.size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* ptr = staging.map();
	memCopy(buffer_data.ptr, ptr, buffer_data.size);
	staging.unmap();

	AllocatedBuffer buffer = createAndAllocateBuffer(
		buffer_data.size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkCommandBuffer command_buffer = beginSingleTimeCommandBuffer();
	const VkBufferCopy copy_info{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = buffer_data.size,
	};
	vkCmdCopyBuffer(command_buffer, staging.buffer, buffer.buffer, 1, &copy_info);
	endSingleTimeCommandBuffer(command_buffer);

	vkDestroyBuffer(g.device, staging.buffer, nullptr);
	vkFreeMemory(g.device, staging.memory, nullptr);
	return buffer;
}

static void destroyGameRenderTargets()
{
	for (usize i = 0; i < g_frames_in_flight; i++)
	{
		FrameFinishData::Game& frame_data = g.frame_datas[i].game;

		g.image_store.removeRef(frame_data.render_target);
		g.image_store.removeRef(frame_data.depth_image);
		g.image_store.removeRef(frame_data.resolved_render_target);
	}
}

static void createGameRenderTargets(usize width, usize height)
{
	g.game.width = width;
	g.game.height = height;

	const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	const VkFormat _depth_format = g.depth_format;

	for (usize i = 0; i < g_frames_in_flight; i++)
	{
		FrameFinishData::Game& frame_data = g.frame_datas[i].game;

		frame_data.render_target = g.image_store.createAndAllocate(
			width,
			height,
			format,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			PAW_STR("Game MSAA Render Target", 23),
			VK_SAMPLE_COUNT_8_BIT);

		frame_data.resolved_render_target = g.image_store.createAndAllocate(
			width,
			height,
			format,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			PAW_STR("Game Resolved Render Target", 27),
			VK_SAMPLE_COUNT_1_BIT);

		g.image_store.pushToShader(frame_data.resolved_render_target);

		frame_data.depth_image = g.image_store.createAndAllocate(
			width,
			height,
			_depth_format,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			PAW_STR("Game MSAA Depth Render Target", 29),
			VK_SAMPLE_COUNT_8_BIT);
	}
}

static void initDebugLineData(DebugLineData& data)
{
	const usize line_buffer_size = megabytes(32);

	data.max_lines = (line_buffer_size / sizeof(DebugGeomLine)) - 1;
	data.line_buffer = createAndAllocateBuffer(
		line_buffer_size,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	data.line_count = 0;
	data.line_buffer_ptr = reinterpret_cast<DebugGeomLine*>(data.line_buffer.map());
	data.line_buffer_slot = pushBufferToShader(data.line_buffer.buffer, 0, line_buffer_size);
}

static void initDebugCommand2DData(DebugCommandData2D& data)
{
	const usize buffer_size = megabytes(2);
	data.command_max = (buffer_size / sizeof(DebugCommand2D)) - 1;
	data.command_buffer = createAndAllocateBuffer(buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	data.command_count = 0;
	data.command_buffer_ptr = reinterpret_cast<DebugCommand2D*>(data.command_buffer.map());
	data.command_buffer_slot = pushBufferToShader(data.command_buffer.buffer, 0, buffer_size);

	VkSampler sampler;

	{
		const VkSamplerCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.mipLodBias = 0.0f,
			.anisotropyEnable = VK_FALSE,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE,
		};

		VkResult result = vkCreateSampler(g.device, &create_info, 0, &sampler);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);

		data.sampler_slot = pushSamplerToShader(sampler);
	}
}

static void initGameViewportState(GameViewportState& game, Platform& platform, Allocator& persistent_allocator, PagePool& page_pool)
{
	const Vec2S32 viewport_size = platform.get_drawable_size();
	createGameRenderTargets(viewport_size.x, viewport_size.y);
	initDebugLineData(game.line_data);
	initDebugLineData(game.no_depth_line_data);
	initDebugCommand2DData(game.debug_2d_data);

	ArenaAllocator swf_allocator{};
	swf_allocator.init(&page_pool, "Swf Allocator"_str);

	game.swf_store = loadSwfFromMemory(
		platform.DEBUG_load_file_blocking("source_data/swfs/text.swf"_str, swf_allocator),
		swf_allocator,
		false,
		persistent_allocator);

	swf_allocator.reset();

	game.scene_constants_buffer = rendererCreateAndAllocateBuffer(GpuBufferType_Storage, sizeof(SceneConstants) * g_frames_in_flight, GpuBufferMemoryProps_GPUCPUSync);

	for (usize frame_index = 0; frame_index < g_frames_in_flight; ++frame_index)
	{
		FrameFinishData::Game& frame_data = g.frame_datas[frame_index].game;
		frame_data.scene_constants_slot = pushBufferToShader((VkBuffer)g.game.scene_constants_buffer.handle, sizeof(SceneConstants) * frame_index, sizeof(SceneConstants));
	}
}

void rendererInit(Platform& platform, GameStartupArgs const& startup_args, Allocator& persistent_allocator, PagePool& page_pool)
{
	PAW_PROFILER_FUNC();
	PAW_ASSERT(!g.initialized);
	vulkanInit(platform, startup_args, page_pool);

	g.image_store.init();

	g.swapchain.init(platform, nullptr);

	g.storage_buffer_slots.init(&persistent_allocator, 35);
	g.sampled_image_slots.init(&persistent_allocator, 32);
	g.sampler_slots.init(&persistent_allocator, 32);

	{
		VkDescriptorPoolSize pool_sizes[]{
			{
				.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = g.storage_buffer_slots.getMaxVulkanSize(),
			},
			{
				.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				.descriptorCount = g.sampled_image_slots.getMaxVulkanSize(),
			},
			{
				.type = VK_DESCRIPTOR_TYPE_SAMPLER,
				.descriptorCount = g.sampler_slots.getMaxVulkanSize(),
			},
		};

		u32 max_sets = 0;
		for (usize i = 0; i < PAW_ARRAY_COUNT(pool_sizes); i++)
		{
			max_sets += pool_sizes[i].descriptorCount;
		}

		VkDescriptorPoolCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
			.maxSets = max_sets,
			.poolSizeCount = PAW_ARRAY_COUNT(pool_sizes),
			.pPoolSizes = pool_sizes,
		};

		VkResult create_pool_result = vkCreateDescriptorPool(g.device, &create_info, 0, &g.descriptor_pool);
		PAW_ASSERT(create_pool_result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(create_pool_result);

		const u32 flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
		VkDescriptorBindingFlags bindless_flags[] = {flags, flags, flags};

		VkDescriptorSetLayoutBinding bindings[]{
			{
				.binding = BindSlot_StorageBuffers,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = g.storage_buffer_slots.getMaxVulkanSize(),
				.stageFlags = VK_SHADER_STAGE_ALL,
			},
			{
				.binding = BindSlot_Images,
				.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				.descriptorCount = g.sampled_image_slots.getMaxVulkanSize(),
				.stageFlags = VK_SHADER_STAGE_ALL,
			},
			{
				.binding = BindSlot_Samplers,
				.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
				.descriptorCount = g.sampler_slots.getMaxVulkanSize(),
				.stageFlags = VK_SHADER_STAGE_ALL,
			},
		};

		VkDescriptorSetLayoutBindingFlagsCreateInfo extended_info{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
			.bindingCount = PAW_ARRAY_COUNT(bindings),
			.pBindingFlags = bindless_flags,
		};

		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = &extended_info,
			.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
			.bindingCount = PAW_ARRAY_COUNT(bindings),
			.pBindings = bindings,
		};

		VkResult create_descriptor_set_layout_result =
			vkCreateDescriptorSetLayout(g.device, &descriptor_set_layout_create_info, 0, &g.descriptor_set_layout);
		PAW_ASSERT(create_descriptor_set_layout_result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(create_descriptor_set_layout_result);

		VkDescriptorSetAllocateInfo alloc_info{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = g.descriptor_pool,
			.descriptorSetCount = 1,
			.pSetLayouts = &g.descriptor_set_layout,
		};

		VkResult alloc_result = vkAllocateDescriptorSets(g.device, &alloc_info, &g.descriptor_set);
		PAW_ASSERT(alloc_result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(alloc_result);
	}

	u32 debug_lines_slot = g.storage_buffer_slots.alloc();
	PAW_ASSERT(debug_lines_slot == g_debug_lines_slot);
	PAW_ASSERT_UNUSED(debug_lines_slot);

	u32 debug_lines_cmd_slot = g.storage_buffer_slots.alloc();
	PAW_ASSERT(debug_lines_cmd_slot == g_debug_line_cmds_slot);
	PAW_ASSERT_UNUSED(debug_lines_cmd_slot);

	VkCommandBuffer command_buffers[g_frames_in_flight];

	allocCommandBuffers(g_frames_in_flight, command_buffers);

	for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
	{
		FrameFinishData& frame_data = g.frame_datas[frame_index];
		frame_data.command_buffer = command_buffers[frame_index];
		VkSemaphoreCreateInfo semaphore_create_info{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		};

		{
			VkResult result =
				vkCreateSemaphore(g.device, &semaphore_create_info, 0, &frame_data.image_available_semaphore);
			PAW_ASSERT(result == VK_SUCCESS);
			PAW_ASSERT_UNUSED(result);
		}

		{
			VkResult result =
				vkCreateSemaphore(g.device, &semaphore_create_info, 0, &frame_data.render_finished_semaphore);
			PAW_ASSERT(result == VK_SUCCESS);
			PAW_ASSERT_UNUSED(result);
		}

		{
			VkFenceCreateInfo fence_create_info{
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.flags = VK_FENCE_CREATE_SIGNALED_BIT,
			};

			VkResult result = vkCreateFence(g.device, &fence_create_info, 0, &frame_data.in_flight_fence);
			PAW_ASSERT(result == VK_SUCCESS);
			PAW_ASSERT_UNUSED(result);
		}
	}

	initGameViewportState(g.game, platform, persistent_allocator, page_pool);

#ifdef PAW_ENABLE_IMGUI
	createGraphicsPipeline(
		platform,
		PAW_STR("imgui", 5),
		Pipeline_ImGui,
		GraphicsPipelineDesc{
			.name = PAW_STR("ImGui", 5),
			.cull_mode = VK_CULL_MODE_NONE,
			.blend_state = BlendStateDesc::alpha_blend,
			.depth_test = false,
			.depth_write = false,
			.color_format = g.surface_format.format,
		});

	verifyPipelineStruct(Pipeline_ImGui, PAW_STRING_ID("Vertex"), sizeof(ImDrawVert));
	verifyPipelinePushConstantInternal(Pipeline_ImGui, sizeof(ImGuiState::PushConstants));
#endif

	createGraphicsPipeline(
		platform,
		"fullscreen"_str,
		Pipeline_FullscreenBlit,
		GraphicsPipelineDesc{
			.name = "Fullscreen Blit"_str,
			.cull_mode = VK_CULL_MODE_BACK_BIT,
			.blend_state =
				{
					.enabled = false,
				},
			.depth_test = false,
			.depth_write = false,
			.color_format = g.surface_format.format,
		});

	verifyPipelinePushConstantInternal(Pipeline_FullscreenBlit, sizeof(FullscreenBlitConstants));

	const VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_8_BIT;

	createGraphicsPipeline(
		platform,
		PAW_STR("debug_geom", 10),
		Pipeline_DebugGeom,
		GraphicsPipelineDesc{
			.name = PAW_STR("Debug Geom", 10),
			//.cull_mode = VK_CULL_MODE_NONE,
			.blend_state = BlendStateDesc::alpha_blend,
			.depth_test = true,
			.depth_write = true,
			.depth_compare_op = VK_COMPARE_OP_GREATER,
			.color_format = VK_FORMAT_R8G8B8A8_SRGB,
			.depth_format = g.depth_format,
			.sample_count = sample_count,
		});

	verifyPipelineStruct(Pipeline_DebugGeom, PAW_STRING_ID("Line"), sizeof(DebugGeomLine));
	verifyPipelinePushConstantInternal(Pipeline_DebugGeom, sizeof(DebugGeomConstants));

	createGraphicsPipeline(
		platform,
		PAW_STR("debug_geom", 10),
		Pipeline_NoDepthDebugGeom,
		GraphicsPipelineDesc{
			.name = PAW_STR("No Depth Debug Geom", 19),
			.blend_state = BlendStateDesc::alpha_blend,
			.depth_test = false,
			.depth_write = false,
			.depth_compare_op = VK_COMPARE_OP_GREATER,
			.color_format = VK_FORMAT_R8G8B8A8_SRGB,
			.depth_format = g.depth_format,
			.sample_count = sample_count,
		});

	verifyPipelineStruct(Pipeline_NoDepthDebugGeom, PAW_STRING_ID("Line"), sizeof(DebugGeomLine));
	verifyPipelinePushConstantInternal(Pipeline_NoDepthDebugGeom, sizeof(DebugGeomConstants));

	createGraphicsPipeline(
		platform,
		PAW_STR("vector", 6),
		Pipeline_Vector,
		GraphicsPipelineDesc{
			.name = PAW_STR("Vector", 6),
			.cull_mode = VK_CULL_MODE_NONE,
			.blend_state = BlendStateDesc::alpha_blend,
			.depth_test = false,
			.depth_write = false,
			.depth_compare_op = VK_COMPARE_OP_ALWAYS,
			.color_format = VK_FORMAT_R8G8B8A8_SRGB,
			.depth_format = g.depth_format,
			.sample_count = sample_count,
			// .alpha_to_coverage = true,
			.sample_shading = true,
		});

	verifyPipelinePushConstantInternal(Pipeline_Vector, sizeof(VectorConstants));
	verifyPipelineStruct(Pipeline_Vector, PAW_STRING_ID("Vertex"), sizeof(SwfVertex));

	createGraphicsPipeline(
		platform,
		PAW_STR("editor_mesh", 11),
		Pipeline_EditorMesh,
		GraphicsPipelineDesc{
			.name = PAW_STR("EditorMesh", 10),
			.cull_mode = VK_CULL_MODE_NONE,
			.blend_state = BlendStateDesc::alpha_blend,
			.depth_test = true,
			.depth_write = true,
			.depth_compare_op = VK_COMPARE_OP_GREATER,
			.color_format = VK_FORMAT_R8G8B8A8_SRGB,
			.depth_format = g.depth_format,
			.sample_count = sample_count,
			// .alpha_to_coverage = true,
		});

	verifyPipelinePushConstantInternal(Pipeline_EditorMesh, sizeof(EditorMeshConstants));

	createGraphicsPipeline(
		platform,
		PAW_STR("debug_geom_2d", 13),
		Pipeline_DebugGeom2D,
		GraphicsPipelineDesc{
			.name = PAW_STR("Debug Geom 2D", 13),
			.blend_state = BlendStateDesc::alpha_blend,
			.depth_test = false,
			.depth_write = false,
			.depth_compare_op = VK_COMPARE_OP_GREATER,
			.color_format = VK_FORMAT_R8G8B8A8_SRGB,
			.depth_format = g.depth_format,
			.sample_count = sample_count,
		});

	verifyPipelineStruct(Pipeline_DebugGeom2D, PAW_STRING_ID("Command"), sizeof(DebugCommand2D));
	verifyPipelinePushConstantInternal(Pipeline_DebugGeom2D, sizeof(DebugGeom2DConstants));

	createGraphicsPipeline(platform, "mesh"_str, Pipeline_Mesh, GraphicsPipelineDesc{
																	.name = "Mesh"_str,
																	.depth_test = true,
																	.depth_write = true,
																	.depth_compare_op = VK_COMPARE_OP_GREATER,
																	.color_format = VK_FORMAT_R8G8B8A8_SRGB,
																	.depth_format = g.depth_format,
																	.sample_count = sample_count,
																});

	verifyPipelineStruct(Pipeline_Mesh, PAW_STRING_ID("StaticVertex"), sizeof(StaticSceneVertex));
	verifyPipelinePushConstantInternal(Pipeline_Mesh, sizeof(MeshConstants));

#ifdef PAW_ENABLE_IMGUI
	g.imgui.init();
#endif

	{
		const VkSamplerCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_NEAREST,
			.minFilter = VK_FILTER_NEAREST,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.mipLodBias = 0.0f,
			.anisotropyEnable = VK_FALSE,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE,
		};

		VkResult result = vkCreateSampler(g.device, &create_info, 0, &g.fullscreen_sampler);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);

		g.fullscreen_sampler_index = pushSamplerToShader(g.fullscreen_sampler);
	}

	g.initialized = true;
}

void rendererDeinit()
{
	PAW_ASSERT(g.initialized);

	vkDeviceWaitIdle(g.device);

	vkDestroyDescriptorSetLayout(g.device, g.descriptor_set_layout, nullptr);
	vkDestroyDescriptorPool(g.device, g.descriptor_pool, nullptr);

	destroyGameRenderTargets();

	unloadSwf(&g.game.swf_store);

#ifdef PAW_ENABLE_IMGUI
	g.imgui.deinit();
#endif

	g.swapchain.deinit();

	g.image_store.deinit();

	vulkanDeinit();

	g.initialized = false;
}

void rendererProcessEvent(PlatformEvent const& event)
{
	PAW_ASSERT(g.initialized);
	switch (event.type)
	{
		case PlatformEventType_Resize:
		{
#ifndef PAW_ENABLE_IMGUI
			// This should probably be handleded better
			g.image_store.deletePendingImages();
			rendererGameViewportResize(event.resize.width, event.resize.height);
#endif
		}
		break;
	}
}

void rendererBeginFrame()
{
	PAW_PROFILER_FUNC();
	PAW_ASSERT(g.initialized);
	// g.game.line_data.line_count = 0;
	g.game.no_depth_line_data.line_count = 0;
	g.game.debug_2d_data.command_count = 0;
}

static void recreateSwapchain(Platform& platform)
{
	vkDeviceWaitIdle(g.device);
	Swapchain new_swapchain;
	new_swapchain.init(platform, &g.swapchain);

	g.swapchain.deinit();
	g.swapchain = new_swapchain;
}

static void renderGame(Platform& /*platform*/, VkCommandBuffer command_buffer, FrameFinishData::Game const& frame_data, GameViewportState& game, RendererFrameData const& render_frame_data)
{
	PAW_PROFILER_FUNC();
	PAW_ASSERT(g.initialized);

	/*loadSwfFromMemory(
		platform.DEBUG_loadFileBlocking(PAW_STR("source_data/swfs/gradient_test2.swf", 35), g_frame_allocator),
		g_frame_allocator,
		true);*/

	g.game.line_data.line_count = 0;

	{
		DebugDrawState const* state = render_frame_data.game_state_debug_draw_state;
		DebugCommandStore::PageHeader* page = state->depth_tested_commands_3d.current_page;
		usize written_command_count = 0;
		while (page)
		{
			PAW_ASSERT(written_command_count + page->count <= g.game.line_data.max_lines);
			DebugDrawCommand3D* commands = page->getCommandsPtr();
			memCopy(commands, g.game.line_data.line_buffer_ptr + written_command_count * sizeof(DebugDrawCommand3D), sizeof(DebugDrawCommand3D) * page->count);
			written_command_count += page->count;
			page = page->prev;
		}
		g.game.line_data.line_count += written_command_count;
	}

	{
		DebugDrawState const* state = render_frame_data.debug_draw_state;
		DebugCommandStore::PageHeader* page = state->depth_tested_commands_3d.current_page;
		usize written_command_count = 0;
		while (page)
		{
			PAW_ASSERT(written_command_count + page->count <= g.game.line_data.max_lines);
			DebugDrawCommand3D* commands = page->getCommandsPtr();
			memCopy(commands, g.game.line_data.line_buffer_ptr + written_command_count * sizeof(DebugDrawCommand3D), sizeof(DebugDrawCommand3D) * page->count);
			written_command_count += page->count;
			page = page->prev;
		}
		g.game.line_data.line_count += written_command_count;
	}

	// #TODO: Plug real game viewport size in here
	const Mat4 camera_mvp = rendererGetCameraProjectionMat(math_toRadians(60.0f), (f32)game.width, (f32)game.height, 0.01f, 1000.0f) * render_frame_data.camera_mat;

	pushDebugGroup(command_buffer, "Game");
	Defer game_debug_group_defer = {[command_buffer]
									{ popDebugGroup(command_buffer); }};

	{
		const VkImageMemoryBarrier image_memory_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.image = g.image_store.getVkImage(frame_data.resolved_render_target),
			.subresourceRange =
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
		};

		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&image_memory_barrier);
	}

	const VkRenderingAttachmentInfo color_attachment_info{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = g.image_store.getView(frame_data.render_target),
		.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
		.resolveImageView = g.image_store.getView(frame_data.resolved_render_target),
		.resolveImageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue =
			{
				.color = {.float32 = {0.1f, 0.1f, 0.1f, 1.0f}},
			},
	};

	const VkRenderingAttachmentInfo depth_attachment_info{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = g.image_store.getView(frame_data.depth_image),
		.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.clearValue =
			{
				.depthStencil = {.depth = 0.0f, .stencil = 0},
			},
	};

	const VkRenderingInfo rendering_info{
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.renderArea = {.offset = {0, 0}, .extent = {static_cast<u32>(game.width), static_cast<u32>(game.height)}},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_info,
		.pDepthAttachment = &depth_attachment_info,
	};

	g.image_store.addFrameUsage(frame_data.render_target, g.frame_index);
	g.image_store.addFrameUsage(frame_data.depth_image, g.frame_index);

	g.image_store.addFrameUsage(frame_data.resolved_render_target, g.frame_index);

	vkCmdBeginRendering(command_buffer, &rendering_info);

	const VkViewport viewport{
		.x = 0,
		.y = static_cast<f32>(game.height),
		.width = static_cast<f32>(game.width),
		.height = -static_cast<f32>(game.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	vkCmdSetViewport(command_buffer, 0, 1, &viewport);

	const VkRect2D scissor{
		.offset = {.x = 0, .y = 0},
		.extent = rendering_info.renderArea.extent,
	};

	vkCmdSetScissor(command_buffer, 0, 1, &scissor);

	pushDebugGroup(command_buffer, "Meshes");
	{
		bindPipeline(command_buffer, Pipeline_Mesh);
		SceneConstants* scene_constants = (SceneConstants*)rendererMapBuffer(g.game.scene_constants_buffer, (g.frame_index % g_frames_in_flight) * sizeof(SceneConstants), sizeof(SceneConstants));
		scene_constants->view_projection = camera_mvp;
		rendererUnmapBuffer(g.game.scene_constants_buffer);
		for (StaticMesh const& mesh : g.game.static_scene.meshes)
		{

			const MeshConstants constants{
				.model = mesh.transform,
				.vertex_buffer_index = g.game.static_scene.vertex_buffer_slot,
				.vertex_buffer_offset = mesh.vertex_offset,
				.scene_constants_buffer_index = frame_data.scene_constants_slot,
			};
			pushPipelineConstantsInternal(command_buffer, Pipeline_Mesh, sizeof(MeshConstants), &constants);
			vkCmdDraw(command_buffer, mesh.vertex_count, 1, 0, 0);
		}
	}
	popDebugGroup(command_buffer);

	pushDebugGroup(command_buffer, "Debug Geom");
	{
		bindPipeline(command_buffer, Pipeline_DebugGeom);
		const DebugGeomConstants constants{
			.view_projection = camera_mvp,
			.lines_buffer_slot = game.line_data.line_buffer_slot,
			.camera_position = render_frame_data.camera_position,
		};
		pushPipelineConstantsInternal(command_buffer, Pipeline_DebugGeom, sizeof(DebugGeomConstants), &constants);
		const u32 vert_count = static_cast<u32>(game.line_data.line_count) * 36;
		vkCmdDraw(command_buffer, vert_count, 1, 0, 0);
	}
	popDebugGroup(command_buffer);

	static f32 zoom_level = 1.0f;
	static Vec2 camera_pos = Vec2{20.0f, 20.0f};
	// ImGui::DragFloat("Game Zoom", &zoom_level, 0.001f);
	// ImGui::DragFloat2("Position", camera_pos.getPtr());
	//  const usize shape_col_count = math_floorToUsize(math_squareRoot((f32)game.swf_store.shape_count));

	const Mat4 ortho_proj = Mat4::ortho(
								0.0f,
								0.0f,
								static_cast<f32>(game.width) * zoom_level,
								static_cast<f32>(game.height) * zoom_level,
								0.0f,
								100.0f) *
		Mat4::translate(Vec3{camera_pos.x, camera_pos.y, 0.0f});

	pushDebugGroup(command_buffer, "Vector");
	{
		bindPipeline(command_buffer, Pipeline_Vector);

		/*f32 x_offset = 0.0f;
		f32 y_offset = 0.0f;
		f32 max_y_offset = 0.0f;*/
		for (usize i = 0; i < game.swf_store.shape_count; i++)
		{
			SwfShape const& swf_shape = game.swf_store.shapes[i];
			/*if (i % shape_col_count == 0)
			{
				y_offset += max_y_offset;
				max_y_offset = 0.0f;
				x_offset = 0.0f;
			}*/
			const VectorConstants constants{
				.projection = ortho_proj * Mat4::scale(Vec3{1.0f, 1.0f, 1.0f}),
				.vertex_buffer_index = game.swf_store.vertex_buffer_slot,
				//.position = Vec2{x_offset - swf_shape.min_x, y_offset - swf_shape.min_y},
				.gradient_texture_index = game.swf_store.gradient_texture_slot,
				.gradient_sampler_index = game.debug_2d_data.sampler_slot,
				.gradient_count = game.swf_store.gradient_count,
			};

			pushPipelineConstantsInternal(command_buffer, Pipeline_Vector, sizeof(VectorConstants), &constants);
			vkCmdBindIndexBuffer(
				command_buffer, reinterpret_cast<VkBuffer>(game.swf_store.index_buffer.handle), 0, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(command_buffer, swf_shape.index_count, 1, swf_shape.index_offset, swf_shape.vertex_offset, 0);
			/*const f32 height = swf_shape.max_y - swf_shape.min_y;
			const f32 width = swf_shape.max_x - swf_shape.min_x;*/
			// debugDrawRect2D(Vec2{x_offset, y_offset}, Vec2{x_offset, y_offset} + Vec2{width, height}, LinearColor{1.0f, 0.0f, 0.0f, 0.5f}, 1.0f);
			/*x_offset += width;
			if (height > max_y_offset)
				max_y_offset = height;*/
		}
	}
	popDebugGroup(command_buffer);

	pushDebugGroup(command_buffer, "Debug Geom 2D");
	{
		bindPipeline(command_buffer, Pipeline_DebugGeom2D);
		const DebugGeom2DConstants constants{
			.view_projection = ortho_proj,
			.command_buffer_index = game.debug_2d_data.command_buffer_slot,
			.sampler_index = game.debug_2d_data.sampler_slot,
		};
		pushPipelineConstantsInternal(command_buffer, Pipeline_DebugGeom2D, sizeof(DebugGeom2DConstants), &constants);
		const u32 vert_count = static_cast<u32>(game.debug_2d_data.command_count) * 6;
		vkCmdDraw(command_buffer, vert_count, 1, 0, 0);
	}
	popDebugGroup(command_buffer);

	pushDebugGroup(command_buffer, "Editor");
	if (g.received_editor_mesh)
	{
		const EditorMeshConstants constants{
			.view_projection = Mat4::ortho(-10, 10, 10, -10, -10, 10),
			.vertex_buffer_index = g.editor_mesh_vb_slot,
		};
		bindPipeline(command_buffer, Pipeline_EditorMesh);
		pushPipelineConstantsInternal(command_buffer, Pipeline_EditorMesh, sizeof(constants), &constants);
		vkCmdBindIndexBuffer(command_buffer, g.editor_mesh_ib.buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(command_buffer, g.editor_mesh_index_count, 1, 0, 0, 0);
	}
	popDebugGroup(command_buffer);

	popDebugGroup(command_buffer);

	vkCmdEndRendering(command_buffer);

	const VkImageMemoryBarrier image_memory_barrier{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.image = g.image_store.getVkImage(frame_data.resolved_render_target),
		.subresourceRange =
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
	};

	vkCmdPipelineBarrier(
		command_buffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&image_memory_barrier);
}

void rendererRender(Platform& platform, RendererFrameData const& render_frame_data)
{
	PAW_PROFILER_FUNC();
	PAW_ASSERT(g.initialized);

	FrameFinishData& frame_data = g.frame_datas[g.frame_index % g_frames_in_flight];
	frame_data.terminated_early = false;

	g.image_store.deletePendingImages();

	{
		PAW_PROFILER_SCOPE("WaitForFences");
		VkResult result = vkWaitForFences(g.device, 1, &frame_data.in_flight_fence, VK_TRUE, UINT64_MAX);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	Optional<u32> const op_image_index = g.swapchain.prepareNextImage(frame_data.image_available_semaphore);
	if (!op_image_index.hasValue())
	{
		recreateSwapchain(platform);
		frame_data.terminated_early = true;
		return;
	}

	const u32 image_index = op_image_index.getValue();
	frame_data.image_index = image_index;

	if (g.images_in_flight_fences[image_index] != VK_NULL_HANDLE)
	{
		PAW_PROFILER_SCOPE("WaitForFences");
		VkResult result = vkWaitForFences(g.device, 1, &g.images_in_flight_fences[image_index], VK_TRUE, UINT64_MAX);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	{
		VkResult result = vkResetFences(g.device, 1, &frame_data.in_flight_fence);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	g.images_in_flight_fences[image_index] = frame_data.in_flight_fence;

	vkResetCommandBuffer(frame_data.command_buffer, 0);

	const VkCommandBufferBeginInfo begin_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,
	};

	VkResult begin_result = vkBeginCommandBuffer(frame_data.command_buffer, &begin_info);
	PAW_ASSERT(begin_result == VK_SUCCESS);
	PAW_UNUSED_ARG(begin_result);

	renderGame(platform, frame_data.command_buffer, frame_data.game, g.game, render_frame_data);

	{
		const VkImageMemoryBarrier image_memory_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.image = g.swapchain.images[frame_data.image_index],
			.subresourceRange =
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
		};

		vkCmdPipelineBarrier(
			frame_data.command_buffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&image_memory_barrier);
	}

	const VkRenderingAttachmentInfo color_attachment_info{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = g.swapchain.image_views[frame_data.image_index],
		.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue =
			{
				.color = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}},
			},
	};

	const VkRenderingInfo rendering_info{
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.renderArea = {.offset = {0, 0}, .extent = g.swapchain.extent},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_info,
	};

	vkCmdBeginRendering(frame_data.command_buffer, &rendering_info);

	const VkViewport viewport{
		.x = 0,
		.y = static_cast<f32>(g.swapchain.extent.height),
		.width = static_cast<f32>(g.swapchain.extent.width),
		.height = -static_cast<f32>(g.swapchain.extent.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	vkCmdSetViewport(frame_data.command_buffer, 0, 1, &viewport);

	const VkRect2D scissor{
		.offset = {.x = 0, .y = 0},
		.extent = g.swapchain.extent,
	};

	vkCmdSetScissor(frame_data.command_buffer, 0, 1, &scissor);

#ifdef PAW_ENABLE_IMGUI
	g.imgui.render(frame_data.command_buffer);
#else
	{
		pushDebugGroup(frame_data.command_buffer, "Game Blit");
		const FullscreenBlitConstants constants{
			.texture_slot = g.image_store.getShaderSlot(frame_data.game.resolved_render_target),
			.sampler_index = g.fullscreen_sampler_index,
		};
		bindPipeline(frame_data.command_buffer, Pipeline_FullscreenBlit);
		pushPipelineConstantsInternal(frame_data.command_buffer, Pipeline_FullscreenBlit, sizeof(constants), &constants);
		vkCmdDraw(frame_data.command_buffer, 6, 1, 0, 0);
		popDebugGroup(frame_data.command_buffer);
	}
#endif

	vkCmdEndRendering(frame_data.command_buffer);

	{
		const VkImageMemoryBarrier image_memory_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.image = g.swapchain.images[frame_data.image_index],
			.subresourceRange =
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
		};

		vkCmdPipelineBarrier(
			frame_data.command_buffer,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&image_memory_barrier);
	}

	VkResult end_result = vkEndCommandBuffer(frame_data.command_buffer);
	PAW_ASSERT(end_result == VK_SUCCESS);
	PAW_ASSERT_UNUSED(end_result);

	{
		VkSemaphore wait_semaphores[] = {frame_data.image_available_semaphore};
		VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		VkSemaphore signal_semaphores[] = {frame_data.render_finished_semaphore};
		VkCommandBuffer command_buffers[] = {frame_data.command_buffer};
		const VkSubmitInfo submit_info{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = PAW_ARRAY_COUNT(wait_semaphores),
			.pWaitSemaphores = wait_semaphores,
			.pWaitDstStageMask = wait_stages,
			.commandBufferCount = PAW_ARRAY_COUNT(command_buffers),
			.pCommandBuffers = command_buffers,
			.signalSemaphoreCount = PAW_ARRAY_COUNT(signal_semaphores),
			.pSignalSemaphores = signal_semaphores,
		};

		VkResult result = vkQueueSubmit(g.graphics_queue, 1, &submit_info, frame_data.in_flight_fence);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	{
		PAW_PROFILER_SCOPE("Present");
		if (!g.swapchain.present(frame_data.render_finished_semaphore, frame_data.image_index))
		{
			recreateSwapchain(platform);
		}
	}

	frame_data.render_count++;

	g.frame_index++;
}

void rendererOnShaderModification(Platform& platform, UTF8StringView const& path)
{
	PAW_ASSERT(g.initialized);
	vkDeviceWaitIdle(g.device);

	const u64 hash = murmur64(reinterpret_cast<byte const*>(path.ptr), path.size_bytes - 9, g_seed);
	for (usize hash_index = 0; hash_index < Pipeline_Count; hash_index++)
	{
		if (g.shader_hashes[hash_index] == hash)
		{
			Slice<byte> file = platform.DEBUG_load_file_blocking(path, platform.get_system_allocator());
			VkShaderModule shader = createShader(file);
			platform.get_system_allocator().free(file);

			UTF8StringView name;

			PipelineDesc& pipeline_desc = g.pipeline_descs[hash_index];
			PipelineReflectionData& reflection_data = g.pipeline_reflection_datas[hash_index];
			switch (pipeline_desc.type)
			{
				case PipelineType::Graphics:
				{
					const UTF8StringView vert_comparison = PAW_STR("vert.spv", 8);

					if (memEqual(
							path.ptr + path.size_bytes - vert_comparison.size_bytes,
							vert_comparison.ptr,
							vert_comparison.size_bytes))
					{
						vkDestroyShaderModule(g.device, pipeline_desc.graphics.vert_shader, nullptr);
						pipeline_desc.graphics.vert_shader = shader;
					}
					else
					{
						vkDestroyShaderModule(g.device, pipeline_desc.graphics.frag_shader, nullptr);
						pipeline_desc.graphics.frag_shader = shader;
					}

					name = pipeline_desc.graphics.name;

					createGraphicsPipeline(
						platform,
						reflection_data.shader_name,
						static_cast<Pipeline>(hash_index),
						pipeline_desc.graphics);
				}
				break;

				case PipelineType::Compute:
				{
					vkDestroyShaderModule(g.device, pipeline_desc.compute.shader, nullptr);
					pipeline_desc.compute.shader = shader;
					name = pipeline_desc.compute.name;

					createComputePipeline(
						platform,
						reflection_data.shader_name,
						static_cast<Pipeline>(hash_index),
						pipeline_desc.compute);
				}
				break;
			}

			PAW_LOG_INFO("Recompiled: %p{str}", &name);

			return;
		}
	}
	PAW_LOG_WARNING("Couldn't find %p{str}", &path);
}

void rendererPushStaticScene(StaticScene const& scene)
{
	g.game.static_scene = scene;
}

static VkBufferUsageFlags g_buffer_usage_map[] = {
	VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
	VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
};

static_assert(
	PAW_ARRAY_COUNT(g_buffer_usage_map) == GpuBufferType_Count, "Not all enum values are mapped in GpuBufferType");

GpuBufferView rendererCreateAndAllocateBuffer(GpuBufferType type, usize size, GpuBufferMemoryProps memory_props)
{
	VkMemoryPropertyFlags vk_memory_props = 0;
	if ((memory_props & GpuBufferMemoryProps_CPUVisible) == GpuBufferMemoryProps_CPUVisible)
	{
		vk_memory_props |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	}

	if ((memory_props & GpuBufferMemoryProps_GPUCPUSync) == GpuBufferMemoryProps_GPUCPUSync)
	{
		vk_memory_props |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	}

	if ((memory_props & GpuBufferMemoryProps_GPULocal) == GpuBufferMemoryProps_GPULocal)
	{
		vk_memory_props |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}

	AllocatedBuffer buffer = createAndAllocateBuffer(size, g_buffer_usage_map[type], vk_memory_props);
	return GpuBufferView{
		.handle = reinterpret_cast<u64>(buffer.buffer),
		.memory_handle = reinterpret_cast<u64>(buffer.memory),
		.size = buffer.size,
	};
}

void rendererDestroyAndFreeBuffer(GpuBufferView buffer)
{
	vkDestroyBuffer(g.device, reinterpret_cast<VkBuffer>(buffer.handle), nullptr);
	vkFreeMemory(g.device, reinterpret_cast<VkDeviceMemory>(buffer.memory_handle), nullptr);
}

void* rendererMapBuffer(GpuBufferView buffer, usize offset, usize size)
{
	void* ptr;
	VkResult result = vkMapMemory(g.device, reinterpret_cast<VkDeviceMemory>(buffer.memory_handle), offset, size, 0, &ptr);
	PAW_ASSERT(result == VK_SUCCESS);
	PAW_ASSERT_UNUSED(result);
	return ptr;
}

void rendererUnmapBuffer(GpuBufferView buffer)
{
	vkUnmapMemory(g.device, reinterpret_cast<VkDeviceMemory>(buffer.memory_handle));
}

GpuBufferView rendererCreateAndUploadBuffer(Slice<byte const> const& data, GpuBufferType type)
{
	AllocatedBuffer buffer = createAndUploadBuffer(data, g_buffer_usage_map[type]);
	return GpuBufferView{
		.handle = reinterpret_cast<u64>(buffer.buffer),
		.memory_handle = reinterpret_cast<u64>(buffer.memory),
		.size = buffer.size,
	};
}

u32 rendererPushBufferToShader(GpuBufferView const& buffer)
{
	return pushBufferToShader(reinterpret_cast<VkBuffer>(buffer.handle), 0, buffer.size);
}

GpuImageHandle rendererCreateAndUploadTexture(Slice<byte const> data, GpuTextureFormat format, usize width, usize height, UTF8StringView const& debug_name)
{
	VkFormat vk_format;
	usize bytes_per_pixel;
	switch (format)
	{
		case GpuTextureFormat_R8G8B8A8Unorm:
		{
			bytes_per_pixel = 4;
			vk_format = VK_FORMAT_R8G8B8A8_UNORM;
		}
		break;

		default:
			PAW_UNREACHABLE;
	}

	return createAndUploadImage(data.ptr, width, height, bytes_per_pixel, vk_format, VK_IMAGE_ASPECT_COLOR_BIT, debug_name);
}

u32 rendererPushTextureToShader(GpuImageHandle handle)
{
	return g.image_store.pushToShader(handle);
}

void rendererTextureRemoveRef(GpuImageHandle handle)
{
	g.image_store.removeRef(handle);
}

void Swapchain::init(Platform& platform, Swapchain* old_swapchain)
{

	VkSurfaceCapabilitiesKHR surface_capabilities;
	{
		VkResult result =
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g.physical_device, g.surface, &surface_capabilities);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	VkExtent2D swap_extent = surface_capabilities.currentExtent;
	if (surface_capabilities.currentExtent.width == UINT32_MAX)
	{
		Vec2S32 drawable_size = platform.get_drawable_size();

		VkExtent2D actual_extent = {
			.width = static_cast<u32>(drawable_size.x),
			.height = static_cast<u32>(drawable_size.y),
		};
		actual_extent.width = math_clampU32(
			actual_extent.width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
		actual_extent.height = math_clampU32(
			actual_extent.height,
			surface_capabilities.minImageExtent.height,
			surface_capabilities.maxImageExtent.height);
		swap_extent = actual_extent;
	}

	VkExtent2D new_extent = swap_extent;

	u32 requested_image_count = surface_capabilities.minImageCount + 1;
	if (surface_capabilities.maxImageCount > 0 && requested_image_count > surface_capabilities.maxImageCount)
	{
		requested_image_count = surface_capabilities.maxImageCount;
	}

	VkSurfaceTransformFlagBitsKHR transform = surface_capabilities.currentTransform;

	{
		VkSwapchainCreateInfoKHR create_info = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = g.surface,
			.minImageCount = requested_image_count,
			.imageFormat = g.surface_format.format,
			.imageColorSpace = g.surface_format.colorSpace,
			.imageExtent = new_extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		};

		u32 queue_family_indices[] = {g.graphics_family_index, g.present_family_index};

		if (g.graphics_family_index != g.present_family_index)
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = 0;
		}

		create_info.preTransform = transform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = g.present_mode;
		create_info.clipped = VK_TRUE;
		if (old_swapchain)
		{
			create_info.oldSwapchain = old_swapchain->swapchain;
		}

		VkResult result = vkCreateSwapchainKHR(g.device, &create_info, 0, &swapchain);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);

		u32 new_image_count = 0;
		vkGetSwapchainImagesKHR(g.device, swapchain, &new_image_count, 0);
		PAW_ASSERT(new_image_count <= max_images);

		vkGetSwapchainImagesKHR(g.device, swapchain, &new_image_count, images);
		image_count = new_image_count;
	}

	for (u32 i = 0; i < image_count; i++)
	{
		VkImageViewCreateInfo view_info = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = images[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = g.surface_format.format,
			.subresourceRange =
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
		};

		VkResult result = vkCreateImageView(g.device, &view_info, 0, &image_views[i]);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	extent = new_extent;
}

void Swapchain::deinit()
{

	for (u32 i = 0; i < image_count; i++)
	{
		vkDestroyImageView(g.device, image_views[i], 0);
	}
	vkDestroySwapchainKHR(g.device, swapchain, 0);
}

Optional<u32> Swapchain::prepareNextImage(VkSemaphore semaphore_to_signal)
{
	u32 image_index = 0;
	VkResult acquire_result =
		vkAcquireNextImageKHR(g.device, swapchain, UINT64_MAX, semaphore_to_signal, VK_NULL_HANDLE, &image_index);
	if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return optional::none;
	}
	else
	{
		PAW_ASSERT(acquire_result == VK_SUCCESS || acquire_result == VK_SUBOPTIMAL_KHR);
	}

	return image_index;
}

bool Swapchain::present(VkSemaphore semaphore_to_wait_on, u32 image_index)
{
	VkPresentInfoKHR present_info{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &semaphore_to_wait_on,
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &image_index,
		.pResults = 0,
	};

	VkResult result = vkQueuePresentKHR(g.present_queue, &present_info);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		return false;
	}
	else
	{
		PAW_ASSERT(result == VK_SUCCESS);
	}

	return true;
}

#ifdef PAW_ENABLE_IMGUI

void ImGuiState::init()
{

	{
		byte* pixels;
		s32 texture_width;
		s32 texture_height;
		s32 bytes_per_pixel;
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->GetTexDataAsRGBA32(&pixels, &texture_width, &texture_height, &bytes_per_pixel);
		const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
		font_texture = createAndUploadImage(
			pixels,
			static_cast<usize>(texture_width),
			static_cast<usize>(texture_height),
			static_cast<usize>(bytes_per_pixel),
			format,
			VK_IMAGE_ASPECT_COLOR_BIT,
			PAW_STR("ImGui Font Texture", 18));

		g.image_store.pushToShader(font_texture);

		io.Fonts[0].TexID = reinterpret_cast<ImTextureID>(font_texture);
	}

	{
		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(g.physical_device, &device_properties);

		const VkSamplerCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.mipLodBias = 0.0f,
			.anisotropyEnable = VK_TRUE,
			.maxAnisotropy = device_properties.limits.maxSamplerAnisotropy,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE,
		};

		VkResult result = vkCreateSampler(g.device, &create_info, 0, &font_sampler);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);

		sampler_slot = pushSamplerToShader(font_sampler);
	}

	for (usize i = 0; i < g_frames_in_flight; i++)
	{
		const usize vertex_buffer_size = megabytes(2);
		vertex_buffers[i] = createAndAllocateBuffer(
			vertex_buffer_size,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		setBufferDebugName(vertex_buffers[i].buffer, "ImGui Vertex Buffer");

		// const VkBufferDeviceAddressInfo address_info{
		// 	.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
		// 	.buffer = vertex_buffers[i].buffer,
		// };

		// vertex_buffer_addresses[i] = vkGetBufferDeviceAddress(g.device, &address_info);

		vertex_buffer_slots[i] = pushBufferToShader(vertex_buffers[i].buffer, 0, vertex_buffers[i].size);

		const usize index_buffer_size = megabytes(2);
		index_buffers[i] = createAndAllocateBuffer(
			index_buffer_size,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		setBufferDebugName(index_buffers[i].buffer, "ImGui Index Buffer");
	}
}

void ImGuiState::deinit()
{
	g.image_store.removeRef(font_texture);
	vkDestroySampler(g.device, font_sampler, nullptr);
}

void ImGuiState::render(VkCommandBuffer command_buffer)
{
	PAW_PROFILER_FUNC();
	ImGui::Render();

	// PAW_GPU_PROFILER_ZONE("ImGui", command_buffer);

	ImDrawData const* draw_data = ImGui::GetDrawData();
	const s32 fb_width = static_cast<s32>(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	const s32 fb_height = static_cast<s32>(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);

	if (fb_width <= 0 || fb_height <= 0)
	{
		return;
	}

	const usize frame_index = g.frame_index % g_frames_in_flight;
	// const VkDeviceAddress vertex_buffer_address = vertex_buffer_addresses[frame_index];
	AllocatedBuffer& vertex_buffer = vertex_buffers[frame_index];
	AllocatedBuffer& index_buffer = index_buffers[frame_index];

	PAW_ASSERT(draw_data->TotalVtxCount * sizeof(ImDrawVert) < vertex_buffer.size);
	PAW_ASSERT(draw_data->TotalIdxCount * sizeof(ImDrawIdx) < index_buffer.size);

	const s32 clip_offset_x = static_cast<s32>(draw_data->DisplayPos.x);
	const s32 clip_offset_y = static_cast<s32>(draw_data->DisplayPos.y);
	const s32 clip_scale_x = static_cast<s32>(draw_data->FramebufferScale.x);
	const s32 clip_scale_y = static_cast<s32>(draw_data->FramebufferScale.y);

	PushConstants constants{
		// .vertex_buffer_address = vertex_buffer_address,
		.translation = Vec2{-1.0f, -1.0f},
		.scale = Vec2{2.0f / draw_data->DisplaySize.x, 2.0f / draw_data->DisplaySize.y},
		.sampler_index = sampler_slot,
		.vertex_buffer_slot = vertex_buffer_slots[frame_index],
	};

	const VkViewport viewport{
		.x = 0,
		.y = 0,
		.width = draw_data->DisplaySize.x,
		.height = draw_data->DisplaySize.y,
	};

	pushDebugGroup(command_buffer, "ImGui");
	bindPipeline(command_buffer, Pipeline_ImGui);
	vkCmdSetViewport(command_buffer, 0, 1, &viewport);
	pushPipelineConstantsInternal(command_buffer, Pipeline_ImGui, sizeof(PushConstants), &constants);
	vkCmdBindIndexBuffer(
		command_buffer,
		index_buffer.buffer,
		0,
		sizeof(ImDrawIdx) == sizeof(u32) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
	ImDrawVert* vertex_buffer_mem = reinterpret_cast<ImDrawVert*>(vertex_buffer.map());
	int vertex_count = 0;
	ImDrawIdx* index_buffer_mem = reinterpret_cast<ImDrawIdx*>(index_buffer.map());
	int index_count = 0;
	for (int cmd_list_index = 0; cmd_list_index < draw_data->CmdListsCount; cmd_list_index++)
	{
		ImDrawList const* cmd_list = draw_data->CmdLists[cmd_list_index];
		memCopy(
			cmd_list->VtxBuffer.Data, vertex_buffer_mem + vertex_count, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memCopy(cmd_list->IdxBuffer.Data, index_buffer_mem + index_count, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

		for (int cmd_index = 0; cmd_index < cmd_list->CmdBuffer.Size; cmd_index++)
		{
			ImDrawCmd const& cmd = cmd_list->CmdBuffer[cmd_index];
			const s32 clip_min_x = math_maxS32(0, static_cast<s32>(((cmd.ClipRect.x - clip_offset_x) * clip_scale_x)));
			const s32 clip_min_y = math_maxS32(0, static_cast<s32>(((cmd.ClipRect.y - clip_offset_y) * clip_scale_y)));
			const s32 clip_max_x =
				math_minS32(fb_width, static_cast<s32>(((cmd.ClipRect.z - clip_offset_x) * clip_scale_x)));
			const s32 clip_max_y =
				math_minS32(fb_height, static_cast<s32>(((cmd.ClipRect.w - clip_offset_y) * clip_scale_y)));

			if (clip_max_x > clip_min_x && clip_max_y > clip_min_y)
			{
				const VkRect2D scissor{
					.offset = {.x = clip_min_x, .y = clip_min_y},
					.extent =
						{
							.width = static_cast<u32>(clip_max_x - clip_min_x),
							.height = static_cast<u32>(clip_max_y - clip_min_y),
						},
				};
				vkCmdSetScissor(command_buffer, 0, 1, &scissor);
				constants.texture_id =
					static_cast<u32>(g.image_store.getShaderSlot(reinterpret_cast<GpuImageHandle>(cmd.TextureId)));
				pushPipelineConstantsInternal(command_buffer, Pipeline_ImGui, sizeof(PushConstants), &constants);

				vkCmdDrawIndexed(command_buffer, cmd.ElemCount, 1, cmd.IdxOffset + index_count, vertex_count, 0);
			}
		}

		vertex_count += cmd_list->VtxBuffer.Size;
		index_count += cmd_list->IdxBuffer.Size;
	}
	index_buffer.unmap();
	vertex_buffer.unmap();
	popDebugGroup(command_buffer);
}
#endif

GpuImageHandle rendererGameViewportGetImage()
{
	PAW_ASSERT(g.initialized);
	FrameFinishData& frame_data = g.frame_datas[g.frame_index % g_frames_in_flight];
	return frame_data.game.resolved_render_target;
}

void rendererGameViewportResize(usize new_width, usize new_height)
{
	PAW_ASSERT(g.initialized);
	PAW_LOG_INFO("Resize game viewport to %llu, %llu", new_width, new_height);
	destroyGameRenderTargets();
	createGameRenderTargets(new_width, new_height);
}

void rendererGameViewportAddDebugLine2D(Vec2 const& start, Vec2 const& end, LinearColor const& color, f32 thickness, Vec2 const& min_uv, Vec2 const& max_uv, u32 texture_index)
{
	PAW_ASSERT(g.initialized);
	PAW_ASSERT(g.game.debug_2d_data.command_count < g.game.debug_2d_data.command_max);
	g.game.debug_2d_data.command_buffer_ptr[g.game.debug_2d_data.command_count++] = DebugCommand2D{
		.start = start,
		.end = end,
		.min_uv = min_uv,
		.max_uv = max_uv,
		.color = color,
		.texture_index = texture_index,
		.thickness = thickness,
	};
}

// Based on https://vincent-p.github.io/posts/vulkan_perspective_matrix/
Mat4 rendererGetCameraProjectionMat(
	f32 vertical_fov_radians, f32 viewport_width, f32 viewport_height, f32 near, f32 far)
{
	PAW_ASSERT(g.initialized);
	const f32 aspect_ratio = viewport_width / viewport_height;
	const f32 focal_length = 1.0f / math_tan(vertical_fov_radians * 0.5f);

	const f32 x = focal_length / aspect_ratio;
	const f32 y = focal_length;
	const f32 a = near / (far - near);
	const f32 b = far * a;

	return Mat4{{
		{x, 0.0f, 0.0f, 0.0f},
		{0.0f, y, 0.0f, 0.0f},
		{0.0f, 0.0f, a, -1.0f},
		{0.0f, 0.0f, b, 0.0},
	}};
}

Mat4 rendererGetInverseCameraProjectionMat(Mat4 const& projection)
{
	PAW_ASSERT(g.initialized);
	const f32 x = projection.data[0][0];
	const f32 y = projection.data[1][1];
	const f32 a = projection.data[2][2];
	const f32 b = projection.data[3][2];

	return Mat4{{
		{1.0f / x, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f / y, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f / b},
		{0.0f, 0.0f, -1.0f, a / b},
	}};
}

void ImageStore::init()
{
	for (usize i = 0; i < max_images; i++)
	{
		ref_counts[i] = 0;
		most_recent_frame_uses[i] = 0;
		debug_names[i] = empty_name;
		states[i] = SlotState_Empty;
		images[i] = VK_NULL_HANDLE;
		views[i] = VK_NULL_HANDLE;
		memories[i] = VK_NULL_HANDLE;
		types[i] = Type_Unknown;
		flags[i] = Flags_None;
		pushed_shader_slot[i] = 0;
	}
}

void ImageStore::deletePendingImages()
{
	for (usize i = 0; i < max_images; i++)
	{
		if (states[i] == SlotState_ReadyForDeletion && most_recent_frame_uses[i] + g_frames_in_flight < g.frame_index)
		{
			deleteImage(i);
		}
	}
}

void ImageStore::deleteImage(usize index)
{
	PAW_ASSERT(ref_counts[index] == 0);
	vkDestroyImageView(g.device, views[index], nullptr);
	vkFreeMemory(g.device, memories[index], nullptr);
	vkDestroyImage(g.device, images[index], nullptr);

	if ((flags[index] & Flags_PushedToShader) == Flags_PushedToShader)
	{
		freeSampledImageSlot(pushed_shader_slot[index]);
	}

	PAW_LOG_INFO(
		"Deleted %p{str}, last used in frame %llu, current frame is %llu",
		debug_names[index],
		most_recent_frame_uses[index],
		g.frame_index);

	states[index] = SlotState_Empty;
	debug_names[index] = empty_name;
	images[index] = VK_NULL_HANDLE;
	views[index] = VK_NULL_HANDLE;
	memories[index] = VK_NULL_HANDLE;
	types[index] = Type_Unknown;
	most_recent_frame_uses[index] = 0;
	flags[index] = Flags_None;
	pushed_shader_slot[index] = 0;
}

void ImageStore::deinit()
{
	for (usize i = 0; i < max_images; i++)
	{
		PAW_ASSERT(states[i] == SlotState_Empty || states[i] == SlotState_ReadyForDeletion);
		if (states[i] == SlotState_ReadyForDeletion)
		{
			deleteImage(i);
		}
	}
};

usize ImageStore::grabFirstFreeSlot()
{
	for (usize i = 0; i < max_images; i++)
	{
		if (states[i] == SlotState_Empty)
		{
			states[i] = SlotState_Active;
			return i;
		}
	}

	PAW_UNREACHABLE;
	return ~0u;
}

GpuImageHandle ImageStore::createAndAllocate(
	usize width, usize height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
	VkMemoryPropertyFlags memory_props, VkImageAspectFlags aspect_flags, UTF8StringView debug_name,
	VkSampleCountFlagBits samples /* = VK_SAMPLE_COUNT_1_BIT */)
{

	const usize index = grabFirstFreeSlot();

	const VkImageCreateInfo create_info{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent =
			{
				.width = static_cast<u32>(width),
				.height = static_cast<u32>(height),
				.depth = 1,
			},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = samples,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VkImage image;
	{
		VkResult result = vkCreateImage(g.device, &create_info, nullptr, &image);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	PAW_ASSERT(debug_name.null_terminated);

#ifdef PAW_DEBUG
	const VkDebugUtilsObjectNameInfoEXT debug_info{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.objectType = VK_OBJECT_TYPE_IMAGE,
		.objectHandle = (uint64_t)image,
		.pObjectName = debug_name.ptr,
	};
	g.vkSetDebugUtilsObjectNameEXT(g.device, &debug_info);
#endif

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(g.device, image, &memory_requirements);

	/*const VkExportMemoryAllocateInfo export_info{
		.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO,
		.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT,
	};*/

	const VkMemoryAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memory_requirements.size,
		.memoryTypeIndex = findMemoryType(memory_requirements.memoryTypeBits, memory_props),
	};

	VkDeviceMemory memory;
	{
		VkResult result = vkAllocateMemory(g.device, &alloc_info, nullptr, &memory);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	{
		VkResult result = vkBindImageMemory(g.device, image, memory, 0);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	VkImageViewCreateInfo view_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange =
			{
				.aspectMask = aspect_flags,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
	};

	VkImageView image_view;
	{
		VkResult result = vkCreateImageView(g.device, &view_info, 0, &image_view);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	images[index] = image;
	views[index] = image_view;
	ref_counts[index] = 1;
	debug_names[index] = debug_name;
	memories[index] = memory;
	types[index] = Type_Internal;

	PAW_LOG_INFO("Created %p{str}", debug_name);

	return (GpuImageHandle)index;
}

GpuImageHandle ImageStore::createAndImport(
	u64 external_handle, usize width, usize height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
	VkMemoryPropertyFlags memory_props, VkImageAspectFlags aspect_flags, UTF8StringView debug_name,
	VkSampleCountFlagBits samples)
{
	const VkExternalMemoryHandleTypeFlagBits handle_type = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_KMT_BIT;
	const VkImageType type = VK_IMAGE_TYPE_2D;

	bool dedicated_only = false;

	{
		VkExternalImageFormatProperties external_image_format_props{
			.sType = VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES,
		};

		VkImageFormatProperties2 image_format_props{
			.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2,
			.pNext = &external_image_format_props,
		};

		const VkPhysicalDeviceExternalImageFormatInfo external_image_format_info{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO,
			.handleType = handle_type,
		};

		const VkPhysicalDeviceImageFormatInfo2 device_image_format_info{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
			.pNext = &external_image_format_info,
			.format = format,
			.type = type,
			.tiling = tiling,
			.usage = usage,
		};

		VkResult result = vkGetPhysicalDeviceImageFormatProperties2(
			g.physical_device, &device_image_format_info, &image_format_props);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);

		PAW_ASSERT(
			external_image_format_props.externalMemoryProperties.externalMemoryFeatures &
			VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT);

		PAW_ASSERT(external_image_format_props.externalMemoryProperties.compatibleHandleTypes & handle_type);

		dedicated_only = external_image_format_props.externalMemoryProperties.externalMemoryFeatures &
			VK_EXTERNAL_MEMORY_FEATURE_DEDICATED_ONLY_BIT;
	}

	const VkExternalMemoryImageCreateInfo import_image_info{
		.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
		.handleTypes = handle_type,
	};

	const VkImageCreateInfo create_info{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = &import_image_info,
		.imageType = type,
		.format = format,
		.extent =
			{
				.width = (u32)width,
				.height = (u32)height,
				.depth = 1,
			},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = samples,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VkImage image;
	{
		VkResult result = vkCreateImage(g.device, &create_info, nullptr, &image);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	// https://community.amd.com/t5/opengl-vulkan/importing-d3d11-tex-into-vulkan-with-vk-image-usage-color/m-p/499951#M4145
	// https://developer.nvidia.com/getting-vulkan-ready-vr

	VkMemoryWin32HandlePropertiesKHR handle_properties{
		.sType = VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR,
	};

	{
		VkResult result =
			g.vkGetMemoryWin32HandlePropertiesKHR(g.device, handle_type, (HANDLE)external_handle, &handle_properties);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(g.device, image, &memory_requirements);

	const VkMemoryDedicatedAllocateInfo dedicated_alloc_info{
		.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
		.image = image,
	};

	const VkImportMemoryWin32HandleInfoKHR import_info{
		.sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR,
		.pNext = dedicated_only ? &dedicated_alloc_info : nullptr,
		.handleType = (VkExternalMemoryHandleTypeFlagBits)import_image_info.handleTypes,
		.handle = (HANDLE)external_handle,
		.name = L"Shared Render Texture",
	};

	const VkMemoryAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = &import_info,
		.allocationSize = memory_requirements.size,
		.memoryTypeIndex = findMemoryType(memory_requirements.memoryTypeBits, memory_props),
	};

	VkDeviceMemory memory;
	{
		VkResult result = vkAllocateMemory(g.device, &alloc_info, nullptr, &memory);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	// if (!dedicated_only)
	{
		VkResult result = vkBindImageMemory(g.device, image, memory, 0);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	VkImageViewCreateInfo view_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange =
			{
				.aspectMask = aspect_flags,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
	};

	VkImageView image_view;
	{
		VkResult result = vkCreateImageView(g.device, &view_info, 0, &image_view);
		PAW_ASSERT(result == VK_SUCCESS);
		PAW_ASSERT_UNUSED(result);
	}

	const usize index = grabFirstFreeSlot();
	images[index] = image;
	views[index] = image_view;
	ref_counts[index] = 1;
	debug_names[index] = debug_name;
	memories[index] = memory;
	types[index] = Type_External;

	PAW_LOG_INFO("Imported %p{str}", debug_name);

	return (GpuImageHandle)index;
}

void ImageStore::removeRef(GpuImageHandle handle)
{
	PAW_ASSERT(states[handle] == SlotState_Active);
	PAW_ASSERT(ref_counts[handle] > 0);

	ref_counts[handle]--;
	if (ref_counts[handle] == 0)
	{
		states[handle] = SlotState_ReadyForDeletion;
	}
}

void ImageStore::addRef(GpuImageHandle handle)
{
	PAW_ASSERT(states[handle] == SlotState_Active);
	PAW_ASSERT(ref_counts[handle] > 0);
	ref_counts[handle]++;
}

void ImageStore::addFrameUsage(GpuImageHandle handle, usize frame_index)
{
	PAW_ASSERT(states[handle] == SlotState_Active);
	PAW_ASSERT(ref_counts[handle] > 0);
	if (frame_index > most_recent_frame_uses[handle])
	{
		most_recent_frame_uses[handle] = frame_index;
	}
}

VkImage ImageStore::getVkImage(GpuImageHandle handle) const
{
	PAW_ASSERT(states[handle] == SlotState_Active);
	return images[handle];
}

VkImageView ImageStore::getView(GpuImageHandle handle) const
{
	PAW_ASSERT(states[handle] == SlotState_Active);
	return views[handle];
}

u32 ImageStore::getShaderSlot(GpuImageHandle handle) const
{
	PAW_ASSERT(states[handle] == SlotState_Active);
	PAW_ASSERT((flags[handle] & Flags_PushedToShader) == Flags_PushedToShader);
	return pushed_shader_slot[handle];
}

u32 ImageStore::pushToShader(GpuImageHandle handle)
{
	PAW_ASSERT(states[handle] == SlotState_Active);
	PAW_ASSERT((flags[handle] & Flags_PushedToShader) != Flags_PushedToShader);
	flags[handle] = (Flags)(flags[handle] | Flags_PushedToShader);

	u32 slot = g.sampled_image_slots.alloc();
	const VkDescriptorImageInfo image_info{
		.imageView = views[handle],
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
	};

	const VkWriteDescriptorSet descriptor_write{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = g.descriptor_set,
		.dstBinding = BindSlot_Images,
		.dstArrayElement = slot,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		.pImageInfo = &image_info,
	};

	vkUpdateDescriptorSets(g.device, 1, &descriptor_write, 0, nullptr);

	pushed_shader_slot[handle] = slot;

	return slot;
}
