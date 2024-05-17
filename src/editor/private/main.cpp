// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#pragma warning(push, 0)
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx12.h>
#include <imgui_internal.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#include <pxr/pxr.h>

#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/plug/notice.h>
#include <pxr/base/plug/plugin.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/editContext.h>
#include <pxr/base/arch/fileSystem.h>
#include <pxr/base/arch/timing.h>
#include <pxr/base/arch/systemInfo.h>
#include <pxr/base/tf/debug.h>
#include <pxr/usd/usd/schemaBase.h>
#pragma warning(pop)

#include <asset_converters/render_graph.h>

#include "icons_material_design.h"

static bool g_main_menu_bar_hovered = false;

struct FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64 FenceValue;
};

// Data
static int const NUM_FRAMES_IN_FLIGHT = 2;
static FrameContext g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT g_frameIndex = 0;

static int const NUM_BACK_BUFFERS = 3;
static ID3D12Device* g_pd3dDevice = NULL;
static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = NULL;
static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = NULL;
static ID3D12CommandQueue* g_pd3dCommandQueue = NULL;
static ID3D12GraphicsCommandList* g_pd3dCommandList = NULL;
static ID3D12Fence* g_fence = NULL;
static HANDLE g_fenceEvent = NULL;
static UINT64 g_fenceLastSignaledValue = 0;
static IDXGISwapChain3* g_pSwapChain = NULL;
static HANDLE g_hSwapChainWaitableObject = NULL;
static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext* WaitForNextFrameResources();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace pxr;

static constexpr ImVec2 g_node_window_padding(16.0f, 16.0f);
static constexpr float g_connector_radius = 8.0f;

static TfToken const g_resource_type_token("resourceType");

TF_DEBUG_CODES(PAW_RENDER_GRAPH);

static void node_input(UsdProperty const& input, ImVec2 const /*node_rect_min*/, ImDrawList* const draw_list, UsdStageRefPtr stage, UsdPrim const& prim)
{
	TfToken const input_complete_name_token = input.GetName();
	TfToken const name_token = input.GetName();
	char const* name = name_token.data();

	UsdAttribute input_attribute = prim.GetAttribute(input_complete_name_token);

	std::string type_str;

	TfToken resource_type;
	if (input_attribute.GetMetadata(g_resource_type_token, &resource_type))
	{
		type_str = resource_type.GetString();
	}
	else
	{
		type_str = input_attribute.GetTypeName().GetAsToken().GetString();
	}

	ImGui::PushID(name);

	/*ImVec2 const start_cursor_pos = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(start_cursor_pos);*/

	// float const text_line_height = ImGui::GetTextLineHeightWithSpacing();
	ImGui::Button("##input_button", ImVec2(g_connector_radius * 2, g_connector_radius * 2));
	ImVec2 const button_center = ImGui::GetItemRectMin() + ImGui::GetItemRectSize() * 0.5f;

	if (ImGui::BeginDragDropSource())
	{
		SdfPath property_path = input.GetPath();
		std::string const property_path_str = property_path.GetAsString();

		if (ImGui::SetDragDropPayload((std::string("input_") + type_str).c_str(), property_path_str.c_str(), property_path_str.size()))
		{
		}

		ImVec2 const mouse_pos = ImGui::GetMousePos();
		draw_list->AddBezierCubic(button_center, button_center - ImVec2(50.0f, 0.0f), mouse_pos + ImVec2(50.0f, 0.0f), mouse_pos, IM_COL32(200, 200, 100, 255), 3.0f);

		ImGui::EndDragDropSource();
	}

	{
		UsdEditContext ctx(stage, stage->GetSessionLayer());
		TfToken const draw_pos_token("inputsDrawData:" + name_token.GetString() + "DrawPos");
		UsdAttribute draw_pos_attribute = prim.GetAttribute(draw_pos_token);
		if (!draw_pos_attribute)
		{
			draw_pos_attribute = prim.CreateAttribute(draw_pos_token, SdfValueTypeNames->Float2);
		}
		draw_pos_attribute.Set(GfVec2f(button_center.x, button_center.y));
	}

	if (ImGui::BeginDragDropTarget())
	{
		ImGuiPayload const* payload = ImGui::AcceptDragDropPayload((std::string("output_") + type_str).c_str());
		if (payload)
		{
			std::string const output_property_path_str = std::string((char const* const)payload->Data, size_t(payload->DataSize));
			SdfPath const output_property_path(output_property_path_str);
			input_attribute.SetConnections({output_property_path});
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::SameLine();

	ImGui::Text("%s", name);
	ImGui::PopID();
}

void node_output(UsdPrim const& prim, UsdProperty const& output, float max_output_text_size, ImDrawList* const draw_list, UsdStageRefPtr stage)
{
	UsdAttribute output_attribute = prim.GetAttribute(output.GetName());
	TfToken name_token = output.GetName();
	char const* name = name_token.data();
	ImGui::PushID(name);
	ImVec2 const text_size = ImGui::GetDefaultFont()->CalcTextSizeA(ImGui::GetDefaultFont()->FontSize, FLT_MAX, FLT_MAX, name);

	std::string type_str;

	TfToken resource_type;
	if (output_attribute.GetMetadata(g_resource_type_token, &resource_type))
	{
		type_str = resource_type.GetString();
	}
	else
	{
		type_str = output_attribute.GetTypeName().GetAsToken().GetString();
	}

	ImVec2 const text_cursor_pos = ImGui::GetCursorScreenPos() + ImVec2(max_output_text_size - text_size.x, 0.0f);
	ImVec2 const circle_pos = text_cursor_pos + ImVec2(g_node_window_padding.x + text_size.x, ImGui::GetTextLineHeight() * 0.5f);

	// ImGui::SetCursorScreenPos(circle_pos - ImVec2(connector_radius, connector_radius));
	// ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	// ImGui::Button("button", ImVec2(connector_radius * 2.0f, connector_radius * 2.0f));
	// ImGui::PopStyleVar();

	ImGui::SetCursorScreenPos(text_cursor_pos);
	ImGui::Text("%s", name);
	ImGui::SameLine();
	ImVec2 const button_size = ImVec2(g_connector_radius * 2.0f, g_connector_radius * 2.0f);
	ImGui::Button("##output_button", button_size);
	ImVec2 const button_center = ImGui::GetItemRectMin() + button_size * 0.5f;
	SdfPath const output_property_path = output.GetPath();

	if (ImGui::BeginDragDropSource())
	{
		std::string const property_path_str = output_property_path.GetAsString();

		if (ImGui::SetDragDropPayload((std::string("output_") + type_str).c_str(), property_path_str.c_str(), property_path_str.size()))
		{
		}

		ImVec2 const mouse_pos = ImGui::GetMousePos();
		draw_list->AddBezierCubic(button_center, button_center + ImVec2(50.0f, 0.0f), mouse_pos - ImVec2(50.0f, 0.0f), mouse_pos, IM_COL32(200, 200, 100, 255), 3.0f);

		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		ImGuiPayload const* payload = ImGui::AcceptDragDropPayload((std::string("input_") + type_str).c_str());
		if (payload)
		{
			std::string const input_property_path_str = std::string((char const* const)payload->Data, size_t(payload->DataSize));
			SdfPath const input_property_path(input_property_path_str);
			UsdPrim input_prim = stage->GetPrimAtPath(input_property_path.GetPrimPath());
			UsdProperty input_property = stage->GetPropertyAtPath(input_property_path);
			TfToken const input_property_name = input_property.GetName();

			UsdAttribute input_attribute = stage->GetAttributeAtPath(input_property_path);
			if (!input_attribute)
			{
				input_attribute = input_prim.CreateAttribute(input_property_name, SdfValueTypeNames->Token);
			};
			input_attribute.SetConnections({output_property_path});
		}
		ImGui::EndDragDropTarget();
	}

	{
		UsdEditContext ctx(stage, stage->GetSessionLayer());
		TfToken const draw_pos_token("outputsDrawData:" + name_token.GetString() + "DrawPos");
		UsdAttribute draw_pos_attribute = prim.GetAttribute(draw_pos_token);
		if (!draw_pos_attribute)
		{
			draw_pos_attribute = prim.CreateAttribute(draw_pos_token, SdfValueTypeNames->Float2);
		}
		draw_pos_attribute.Set(GfVec2f(button_center.x, button_center.y));
	}

	// draw_list->AddCircleFilled(circle_pos, connector_radius, circle_color);
	ImGui::PopID();
}

void graph_link(UsdProperty const& read, UsdPrim const& prim, UsdStageRefPtr stage, ImDrawList* const draw_list)
{
	TfToken const read_name = read.GetName();
	ImVec2 start_pos;
	{
		TfToken const draw_pos_token(std::string("inputsDrawData:") + read.GetName().GetString() + "DrawPos");
		UsdAttribute start_pos_attribute = prim.GetAttribute(draw_pos_token);
		GfVec2f start_pos_gf{};
		bool const has_value = start_pos_attribute.Get(&start_pos_gf);
		TF_AXIOM(has_value);
		start_pos = ImVec2(start_pos_gf[0], start_pos_gf[1]);
	}

	UsdAttribute input_attribute = prim.GetAttribute(read_name);
	if (input_attribute && input_attribute.HasAuthoredConnections())
	{
		SdfPathVector connections;
		input_attribute.GetConnections(&connections);
		for (SdfPath const& input_path : connections)
		{
			UsdProperty target_property = stage->GetPropertyAtPath(input_path);
			SdfPath prim_path = input_path.GetPrimPath();
			UsdPrim target_prim = stage->GetPrimAtPath(prim_path);
			TfToken output_name = target_property.GetName();

			GfVec2f target_pos_gf{};
			TfToken const draw_pos_token(std::string("outputsDrawData:") + output_name.GetString() + "DrawPos");
			UsdAttribute target_pos_attribute = target_prim.GetAttribute(draw_pos_token);
			bool const has_value = target_pos_attribute.Get(&target_pos_gf);
			TF_AXIOM(has_value);

			ImVec2 target_pos(target_pos_gf[0], target_pos_gf[1]);

			draw_list->AddBezierCubic(start_pos, start_pos - ImVec2(50.0f, 0.0f), target_pos + ImVec2(50.0f, 0.0f), target_pos, IM_COL32(200, 200, 100, 255), 3.0f);
		}
	}
}

// Main code
int main(int /*arg_count*/, char* /*args*/[])
{

	// Create application window
	ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL};
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX12 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_MAXIMIZE);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;	  // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	  // Enable Multi-Viewport / Platform Windows
														  // io.ConfigViewportsNoAutoMerge = true;
	io.IniFilename = "editor_imgui.ini";
	// io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	// ImGui::StyleColorsDark();
	ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
	// ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	const ImVec4 primary_white = {0.94f, 0.94f, 0.94f, 1.00f};

	const ImVec4 window_bg_color = ImVec4{0.882f, 0.882f, 0.882f, 1.0f};
	style.WindowRounding = 0.0f;
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4{0.8f, 0.8353f, 0.9412f, 1.0f};

	style.Colors[ImGuiCol_WindowBg] = primary_white;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	style.Colors[ImGuiCol_Tab] =
		ImVec4{primary_white.x * 0.75f, primary_white.y * 0.75f, primary_white.z * 0.75f, primary_white.w * 0.75f};
	style.Colors[ImGuiCol_TabUnfocused] =
		ImVec4{primary_white.x * 0.75f, primary_white.y * 0.75f, primary_white.z * 0.75f, primary_white.w * 0.75f};
	style.Colors[ImGuiCol_TabActive] = primary_white;
	style.Colors[ImGuiCol_TabHovered] = primary_white;
	style.Colors[ImGuiCol_TabUnfocusedActive] = primary_white;
	style.Colors[ImGuiCol_ChildBg] = primary_white;
	// style.Colors[ImGuiCol_TitleBgActive] = ImVec4{0.67f, 0.67f, 0.67f, 1.00f};
	style.Colors[ImGuiCol_TitleBgActive] = window_bg_color;
	style.Colors[ImGuiCol_TitleBg] = window_bg_color;
	// style.Colors[ImGuiCol_TitleBg] = ImVec4{0.82f, 0.82f, 0.82f, 1.00f};
	style.Colors[ImGuiCol_ButtonActive] = ImVec4{0.82f, 0.82f, 0.82f, 1.00f};
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4{0.57f, 0.57f, 0.57f, 1.00f};
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4{0.88f, 0.88f, 0.88f, 1.00f};
	style.Colors[ImGuiCol_Header] = ImVec4{0.80f, 0.80f, 0.80f, 1.00f};
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4{0.68f, 0.68f, 0.68f, 0.80f};
	style.Colors[ImGuiCol_HeaderActive] = ImVec4{0.65f, 0.65f, 0.65f, 1.00f};
	style.Colors[ImGuiCol_DockingEmptyBg] = window_bg_color;
	style.Colors[ImGuiCol_Separator] = window_bg_color;

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap, g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use
	// ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application
	// (e.g. use an PAW_ASSERTion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling
	// ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double
	// backslash \\ !
	// io.Fonts->AddFontDefault();
	// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	// ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL,
	// io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);}

	float const dpi_scale = ImGui_ImplWin32_GetDpiScaleForHwnd(hwnd);
	style.ScaleAllSizes(dpi_scale);

	float const font_size = ImFloor(14.0f * dpi_scale);
	float const icon_size = ImFloor(20.0f * dpi_scale);
	io.Fonts->AddFontFromFileTTF("source_data/fonts/DroidSans.ttf", font_size);
	ImFontConfig config{};
	config.MergeMode = true;
	config.GlyphMinAdvanceX = icon_size;
	config.GlyphOffset.y = ImFloor(7 * dpi_scale);
	static const ImWchar icon_ranges[] = {(ImWchar)ICON_MIN_MD, (ImWchar)ICON_MAX_16_MD, 0};
	io.Fonts->AddFontFromFileTTF("source_data/fonts/" FONT_ICON_FILE_NAME_MD, icon_size, &config, icon_ranges);

	/*STARTUPINFO startup_info{.cb = sizeof(startup_info)};
	PROCESS_INFORMATION process_info{};*/

	// SetEnvironmentVariable("PXR_PLUGINPATH_NAME", "C:/dev/paw6/usd_resources/");

	TfDebug::Enable(PAW_RENDER_GRAPH);

	std::string const working_dir = ArchGetCwd();
	PlugRegistry::GetInstance().RegisterPlugins(working_dir + "/schemas/resources/");

	UsdStageRefPtr stage = UsdStage::Open("source_data/render_graph.usda");
	IM_ASSERT(stage);

	UsdSchemaRegistry& schema_registry = UsdSchemaRegistry::GetInstance();

	TfType const render_graph_type = schema_registry.GetTypeFromName(TfToken("RenderGraph"));
	TF_AXIOM(render_graph_type);
	TfType const render_graph_node_type = schema_registry.GetTypeFromName(TfToken("RenderGraphNode"));
	TF_AXIOM(render_graph_node_type);
	std::set<TfType> render_graph_node_types;
	render_graph_node_type.GetAllDerivedTypes(&render_graph_node_types);

	for (TfType const& type : render_graph_node_types)
	{
		UsdSchemaRegistry::SchemaInfo const* const info = schema_registry.FindSchemaInfo(type);
		UsdPrimDefinition const* const prim_definition = schema_registry.FindConcretePrimDefinition(info->identifier);
		std::string const display_name = PlugRegistry::GetInstance().GetStringFromPluginMetaData(type, "displayName");
		TF_DEBUG_MSG(PAW_RENDER_GRAPH, "%s: %s\n", type.GetTypeName().c_str(), display_name.c_str());
		TF_AXIOM(prim_definition);
	}

	UsdPrim root_graph = stage->GetDefaultPrim();
	TF_AXIOM(root_graph);
	TF_AXIOM(root_graph.IsA(render_graph_type));

	convert_render_graph(stage, root_graph);

	SdfPathVector targets_vector;

	UsdPrim null_prim;

	UsdPrim selected_node = null_prim;

	// Our state
	// bool show_demo_window = true;
	// bool show_another_window = false;

	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::PushStyleVar(
			ImGuiStyleVar_FramePadding, ImVec2{ImGui::GetFrameHeight() * 0.4f, ImGui::GetFrameHeight() * 0.4f});
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, ImGui::GetFrameHeight() * 0.1f);
		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, ImGui::GetFrameHeight() * 0.1f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, window_bg_color);
		if (ImGui::BeginMainMenuBar())
		{
			ImRect move_rect = {ImGui::GetItemRectMin(), ImVec2{}};
			float const menu_bar_height = ImGui::GetCurrentWindow()->MenuBarHeight();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{menu_bar_height * 0.25f, menu_bar_height * 0.5f});
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save"))
				{
					stage->Save();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::EndMenu();
			}

			move_rect.Min.x += ImGui::GetCursorPosX();

			float const button_width = 50.0f;

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_MenuBarBg]);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.717f, 0.717f, 0.717f, 1.0f});

			/*ImGui::SetCursorPosX(viewport->Size.x - button_width * 3.0f);
			if (ImGui::Button(ICON_MD_REMOVE, ImVec2{button_width, 0}))
			{
				ShowWindow(hwnd, SW_MINIMIZE);
			}

			ImGui::SetCursorPosX(viewport->Size.x - button_width * 2.0f);

			WINDOWPLACEMENT window_placement = {
				.length = sizeof(WINDOWPLACEMENT),
			};
			GetWindowPlacement(hwnd, &window_placement);
			const bool maximized = window_placement.showCmd == SW_SHOWMAXIMIZED;
			if (maximized)
			{
				if (ImGui::Button(ICON_MD_FULLSCREEN_EXIT, ImVec2{button_width, 0}))
				{
					ShowWindow(hwnd, SW_RESTORE);
				}
			}
			else
			{
				if (ImGui::Button(ICON_MD_FULLSCREEN, ImVec2{button_width, 0}))
				{
					ShowWindow(hwnd, SW_MAXIMIZE);
				}
			}*/

			ImGui::SetCursorPosX(viewport->Size.x - button_width * 1.0f);

			const ImVec4 close_color = {0.8627f, 0.28627f, 0.28235f, 1.0f};
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, close_color);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, close_color * ImVec4{0.8f, 0.8f, 0.8f, 0.8f});
			/*if (ImGui::Button(ICON_MD_CLOSE, ImVec2{button_width, 0}))
			{
				done = true;
			}*/
			ImGui::PopStyleColor(2);

			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar(1);

			ImGui::PopStyleVar();

			move_rect.Max = ImGui::GetItemRectMax();
			move_rect.Max.x -= button_width * 3.0f;

			g_main_menu_bar_hovered = move_rect.Contains(ImGui::GetMousePos());

			ImGui::EndMainMenuBar();
		}
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, style.Colors[ImGuiCol_WindowBg]);

		const ImGuiWindowFlags sidebar_flags =
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;

		if (ImGui::BeginViewportSideBar("##StatusBar", viewport, ImGuiDir_Down, ImGui::GetFrameHeight(), sidebar_flags))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
				ImGui::Text("Connected: %s", "No Client");
				ImGui::Separator();
				ImGui::Text("Frame Time: %gms", ImGui::GetIO().DeltaTime * 1000.0f);
				ImGui::PopStyleVar();
				ImGui::EndMenuBar();
			}
			ImGui::End();
		}
		ImGui::PopStyleColor();

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize - ImVec2{0.0f, ImGui::GetFrameHeight() * 0.05f});
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});

		if (ImGui::Begin(
				"Document Dockspace",
				nullptr,
				ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
					ImGuiWindowFlags_NoNavFocus))
		{
			const ImGuiID document_dockspace_id = ImGui::GetID("DocumentDockSpace");

			ImGuiWindowClass document_dock_window_class{};
			document_dock_window_class.ClassId = ImHashStr("DocumentDockSpace");
			document_dock_window_class.DockingAllowUnclassed = false;

			ImGui::DockSpace(
				document_dockspace_id,
				ImVec2{0, 0},
				ImGuiDockNodeFlags_NoWindowMenuButton,
				&document_dock_window_class);

			static bool level_test = true;

			ImGui::SetNextWindowClass(&document_dock_window_class);
			ImGui::ShowDemoWindow();

			ImGui::SetNextWindowClass(&document_dock_window_class);
			ImGui::Begin("Render Graph");
			{
				ImGui::BeginChild("node_list", ImVec2(100, 0));
				{
					ImGui::Text("Nodes");
					ImGui::Separator();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				ImGui::BeginGroup();
				{
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
					ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(60, 60, 70, 200));
					ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove);
					{
						ImGui::PopStyleVar(); // WindowPadding

						bool open_context_menu = false;
						UsdPrim hovered_node = null_prim;

						// Display Grid
						ImU32 const grid_color = IM_COL32(200, 200, 200, 40);
						float const grid_size = 64.0f;
						ImVec2 const win_pos = ImGui::GetCursorScreenPos();
						ImVec2 const canvas_size = ImGui::GetWindowSize();

						ImDrawList* const draw_list = ImGui::GetWindowDrawList();

						static ImVec2 scrolling(0.0f, 0.0f);

						ImVec2 const offset = scrolling + win_pos;

						for (float x = fmodf(scrolling.x, grid_size); x < canvas_size.x; x += grid_size)
						{
							draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_size.y) + win_pos, grid_color);
						}

						for (float y = fmodf(scrolling.y, grid_size); y < canvas_size.y; y += grid_size)
						{
							draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_size.x, y) + win_pos, grid_color);
						}

						draw_list->ChannelsSplit(2);

						{
							UsdAttribute position_attribute = root_graph.GetAttribute(TfToken("inputPosition"));
							GfVec2f position;
							bool const has_position = position_attribute.Get<GfVec2f>(&position);
							TF_AXIOM(has_position);
							ImGui::PushID("RootNodeInputs");

							ImVec2 const node_rect_min = offset + ImVec2(position[0], position[1]);

							bool old_any_active = ImGui::IsAnyItemActive();

							draw_list->ChannelsSetCurrent(1); // Foreground

							ImGui::SetCursorScreenPos(node_rect_min + g_node_window_padding);
							ImGui::BeginGroup();
							{
								TfToken const node_name_token = root_graph.GetTypeName();
								char const* node_name = "Inputs";
								ImGui::Text("%s", node_name);
								std::vector<UsdProperty> inputs = root_graph.GetPropertiesInNamespace("input");

								ImVec2 const node_name_text_size = ImGui::GetDefaultFont()->CalcTextSizeA(ImGui::GetDefaultFont()->FontSize, FLT_MAX, FLT_MAX, node_name);

								float max_output_text_size = node_name_text_size.x + g_node_window_padding.x * 4.0f;

								for (size_t i = 0; i < inputs.size(); i++)
								{
									UsdProperty const& input = inputs[i];
									TfToken name_token = input.GetName();
									char const* name = name_token.data();
									ImVec2 const text_size = ImGui::GetDefaultFont()->CalcTextSizeA(ImGui::GetDefaultFont()->FontSize, FLT_MAX, FLT_MAX, name);
									if (text_size.x > max_output_text_size)
									{
										max_output_text_size = text_size.x;
									}
								}

								ImGui::BeginGroup();
								ImGui::PushID("inputs");
								for (size_t output_index = 0; output_index < inputs.size(); output_index++)
								{
									UsdProperty const& input = inputs[output_index];
									node_output(root_graph, input, max_output_text_size, draw_list, stage);
								}
								ImGui::PopID();

								ImGui::EndGroup();
							}
							ImGui::EndGroup();

							ImVec2 const size = ImGui::GetItemRectSize() + g_node_window_padding + g_node_window_padding;
							ImVec2 const node_rect_max = node_rect_min + size;

							bool const node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());

							draw_list->ChannelsSetCurrent(0); // Background
							ImGui::SetCursorScreenPos(node_rect_min);
							ImGui::InvisibleButton("node", size);
							if (ImGui::IsItemHovered())
							{
								hovered_node = root_graph;
							}

							bool const node_moving_active = ImGui::IsItemActive();
							if (node_widgets_active || node_moving_active)
							{
								selected_node = root_graph;
							}

							if (node_moving_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
							{
								position += GfVec2f(io.MouseDelta.x, io.MouseDelta.y);
								position_attribute.Set(position);
							}

							bool const hovered = hovered_node == root_graph;
							bool const selected = selected_node == root_graph;

							ImU32 const outline_color = hovered ? IM_COL32(0, 255, 0, 255) : selected ? IM_COL32(255, 0, 0, 255)
																									  : IM_COL32(255, 255, 255, 255);

							draw_list->AddRectFilled(node_rect_min, node_rect_max, IM_COL32(240, 240, 240, 255), 4.0f);
							draw_list->AddRect(node_rect_min, node_rect_max, outline_color, 4.0f);

							ImGui::PopID();
						}

						{
							UsdAttribute position_attribute = root_graph.GetAttribute(TfToken("outputPosition"));
							GfVec2f position;
							bool const has_position = position_attribute.Get<GfVec2f>(&position);
							TF_AXIOM(has_position);
							ImGui::PushID("RootNodeOutputs");

							ImVec2 const node_rect_min = offset + ImVec2(position[0], position[1]);

							bool old_any_active = ImGui::IsAnyItemActive();

							draw_list->ChannelsSetCurrent(1); // Foreground

							ImGui::SetCursorScreenPos(node_rect_min + g_node_window_padding);
							ImGui::BeginGroup();
							{
								TfToken const node_name_token = root_graph.GetTypeName();
								char const* node_name = "Outputs";
								ImGui::Text("%s", node_name);
								std::vector<UsdProperty> outputs = root_graph.GetPropertiesInNamespace("output");

								ImGui::BeginGroup();
								ImGui::PushID("outputs");
								for (size_t i = 0; i < outputs.size(); i++)
								{
									UsdProperty const& input = outputs[i];
									node_input(input, node_rect_min, draw_list, stage, root_graph);
								}
								ImGui::PopID();

								ImGui::EndGroup();
							}
							ImGui::EndGroup();

							ImVec2 const size = ImGui::GetItemRectSize() + g_node_window_padding + g_node_window_padding;
							ImVec2 const node_rect_max = node_rect_min + size;

							bool const node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());

							draw_list->ChannelsSetCurrent(0); // Background
							ImGui::SetCursorScreenPos(node_rect_min);
							ImGui::InvisibleButton("node", size);
							if (ImGui::IsItemHovered())
							{
								hovered_node = root_graph;
							}

							bool const node_moving_active = ImGui::IsItemActive();
							if (node_widgets_active || node_moving_active)
							{
								selected_node = root_graph;
							}

							if (node_moving_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
							{
								position += GfVec2f(io.MouseDelta.x, io.MouseDelta.y);
								position_attribute.Set(position);
							}

							bool const hovered = hovered_node == root_graph;
							bool const selected = selected_node == root_graph;

							ImU32 const outline_color = hovered ? IM_COL32(0, 255, 0, 255) : selected ? IM_COL32(255, 0, 0, 255)
																									  : IM_COL32(255, 255, 255, 255);

							draw_list->AddRectFilled(node_rect_min, node_rect_max, IM_COL32(240, 240, 240, 255), 4.0f);
							draw_list->AddRect(node_rect_min, node_rect_max, outline_color, 4.0f);

							ImGui::PopID();
						}

						// Display nodes
						UsdPrimSiblingRange const range = root_graph.GetAllChildren();
						for (UsdPrim const& prim : range)
						{
							if (prim.IsA(render_graph_node_type))
							{
								UsdAttribute position_attribute = prim.GetAttribute(TfToken("position"));
								GfVec2f position;
								bool const has_position = position_attribute.Get<GfVec2f>(&position);
								TF_AXIOM(has_position);
								ImGui::PushID((void*)prim.GetPath().GetHash());

								ImVec2 const node_rect_min = offset + ImVec2(position[0], position[1]);

								bool old_any_active = ImGui::IsAnyItemActive();

								draw_list->ChannelsSetCurrent(1); // Foreground

								ImGui::SetCursorScreenPos(node_rect_min + g_node_window_padding);
								ImGui::BeginGroup();
								{
									TfToken const node_name_token = prim.GetTypeName();
									std::string const display_name = PlugRegistry::GetInstance().GetStringFromPluginMetaData(prim.GetPrimTypeInfo().GetSchemaType(), "displayName");
									char const* node_name = display_name.c_str();
									ImGui::Text("%s", node_name);
									std::vector<UsdProperty> inputs = prim.GetPropertiesInNamespace("input");
									std::vector<UsdProperty> outputs = prim.GetPropertiesInNamespace("output");

									ImGui::BeginGroup();

									ImGui::PushID("inputs");
									for (size_t i = 0; i < inputs.size(); i++)
									{
										UsdProperty const& input = inputs[i];
										node_input(input, node_rect_min, draw_list, stage, prim);
									}
									ImGui::PopID();

									ImGui::EndGroup();

									ImVec2 const node_name_text_size = ImGui::GetDefaultFont()->CalcTextSizeA(ImGui::GetDefaultFont()->FontSize, FLT_MAX, FLT_MAX, node_name);

									ImGui::SameLine();

									float max_output_text_size = node_name_text_size.x + g_node_window_padding.x * 4.0f;

									for (size_t i = 0; i < outputs.size(); i++)
									{
										UsdProperty const& output = outputs[i];
										TfToken name_token = output.GetName();
										char const* name = name_token.data();
										ImVec2 const text_size = ImGui::GetDefaultFont()->CalcTextSizeA(ImGui::GetDefaultFont()->FontSize, FLT_MAX, FLT_MAX, name);
										if (text_size.x > max_output_text_size)
										{
											max_output_text_size = text_size.x;
										}
									}

									ImGui::BeginGroup();

									ImGui::PushID("outputs");
									for (size_t output_index = 0; output_index < outputs.size(); output_index++)
									{
										UsdProperty const& output = outputs[output_index];
										node_output(prim, output, max_output_text_size, draw_list, stage);
									}
									ImGui::PopID();

									ImGui::EndGroup();
								}
								ImGui::EndGroup();

								ImVec2 const size = ImGui::GetItemRectSize() + g_node_window_padding + g_node_window_padding;
								ImVec2 const node_rect_max = node_rect_min + size;

								bool const node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());

								draw_list->ChannelsSetCurrent(0); // Background
								ImGui::SetCursorScreenPos(node_rect_min);
								ImGui::InvisibleButton("node", size);
								if (ImGui::IsItemHovered())
								{
									hovered_node = prim;
									if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
									{
										open_context_menu = true;
										TF_DEBUG_MSG(PAW_RENDER_GRAPH, "Request open for node\n");
									}
								}

								bool const node_moving_active = ImGui::IsItemActive();
								if (node_widgets_active || node_moving_active)
								{
									selected_node = prim;
								}

								if (node_moving_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
								{
									position += GfVec2f(io.MouseDelta.x, io.MouseDelta.y);
									position_attribute.Set(position);
								}

								bool const hovered = hovered_node == prim;
								bool const selected = selected_node == prim;

								ImU32 const outline_color = hovered ? IM_COL32(0, 255, 0, 255) : selected ? IM_COL32(255, 0, 0, 255)
																										  : IM_COL32(255, 255, 255, 255);

								draw_list->AddRectFilled(node_rect_min, node_rect_max, IM_COL32(240, 240, 240, 255), 4.0f);
								draw_list->AddRect(node_rect_min, node_rect_max, outline_color, 4.0f);

								ImGui::PopID();
							}
						}

						{
							std::vector<UsdProperty> outputs = root_graph.GetPropertiesInNamespace("output");
							for (UsdProperty const& input : outputs)
							{
								graph_link(input, root_graph, stage, draw_list);
							}
						}

						for (UsdPrim const& prim : range)
						{
							if (prim.IsA(render_graph_node_type))
							{
								std::vector<UsdProperty> reads = prim.GetPropertiesInNamespace("read");
								for (UsdProperty const& read : reads)
								{
									graph_link(read, prim, stage, draw_list);
								}

								std::vector<UsdProperty> read_writes = prim.GetPropertiesInNamespace("readWrite");
								for (UsdProperty const& read_write : read_writes)
								{
									graph_link(read_write, prim, stage, draw_list);
								}

								std::vector<UsdProperty> inputs = prim.GetPropertiesInNamespace("input");
								for (UsdProperty const& input : inputs)
								{
									graph_link(input, prim, stage, draw_list);
								}
							}
						}

						draw_list->ChannelsMerge();

						// Open Context Menu
						if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
						{
							if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && !ImGui::IsAnyItemHovered())
							{
								TF_DEBUG_MSG(PAW_RENDER_GRAPH, "Reset selected\n");
								selected_node = null_prim;
								hovered_node = null_prim;
								open_context_menu = true;
							}
						}

						if (open_context_menu)
						{
							ImGui::OpenPopup("render_graph_context_menu");
							TF_DEBUG_MSG(PAW_RENDER_GRAPH, "Open context menu\n");
							if (hovered_node != null_prim)
							{
								selected_node = hovered_node;
							}
						}

						ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
						if (ImGui::BeginPopup("render_graph_context_menu"))
						{
							ImVec2 const graph_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
							if (selected_node != null_prim)
							{
								if (ImGui::MenuItem("Delete"))
								{
									SdfPath const selected_path = selected_node.GetPath();
									for (UsdPrim const& prim : range)
									{
										if (prim.IsA(render_graph_node_type))
										{
											for (UsdAttribute const& attribute : prim.GetAuthoredAttributes())
											{
												if (attribute.HasAuthoredConnections())
												{
													SdfPathVector connections;
													attribute.GetConnections(&connections);
													for (SdfPath const& path : connections)
													{
														if (path.GetPrimPath() == selected_path)
														{
															attribute.RemoveConnection(path);
														}
													}
												}
											}
										}
									}
									stage->RemovePrim(selected_node.GetPath());
								}
							}
							else
							{
								for (TfType const& node_type : render_graph_node_types)
								{
									std::string const name = node_type.GetTypeName();

									if (ImGui::MenuItem(name.c_str()))
									{
										// TfToken child_name("New" + name);
										UsdSchemaRegistry::SchemaInfo const* const schema_info = schema_registry.FindSchemaInfo(node_type);
										SdfPath child_path = root_graph.GetPath().AppendChild(TfToken(schema_info->identifier.GetString() + std::to_string(ArchGetTickTime())));
										UsdPrim new_node = stage->DefinePrim(child_path, schema_info->identifier);
										UsdAttribute position_attribute = new_node.GetAttribute(TfToken("position"));
										position_attribute.Set(GfVec2f(graph_pos.x, graph_pos.y));
									}
								}
							}
							ImGui::EndPopup();
						}
						ImGui::PopStyleVar();

						// Scrolling
						if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive())
						{
							if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f))
							{
								scrolling += io.MouseDelta;
							}

							scrolling.y += io.MouseWheel * 1000 * io.DeltaTime;
							scrolling.x += io.MouseWheelH * 1000 * io.DeltaTime;
						}
					}
					ImGui::PopStyleVar();	// FramePadding
					ImGui::PopStyleColor(); // ChildBg
					ImGui::EndChild();
				}
				ImGui::EndGroup();
			}
			ImGui::End();

			ImGui::PopStyleVar(2);

			ImGui::End();
		}

		ImGui::PopStyleVar(4);

		// Rendering
		ImGui::Render();

		FrameContext* frameCtx = WaitForNextFrameResources();
		UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
		frameCtx->CommandAllocator->Reset();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		g_pd3dCommandList->Reset(frameCtx->CommandAllocator, NULL);
		g_pd3dCommandList->ResourceBarrier(1, &barrier);

		// Render Dear ImGui graphics
		float const clear_color_with_alpha[4] = {0.45f, 0.55f, 0.60f, 1.00f};
		g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, NULL);
		g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
		g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		g_pd3dCommandList->ResourceBarrier(1, &barrier);
		g_pd3dCommandList->Close();

		g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)g_pd3dCommandList);
		}

		g_pSwapChain->Present(1, 0); // Present with vsync
		// g_pSwapChain->Present(0, 0); // Present without vsync

		UINT64 fenceValue = g_fenceLastSignaledValue + 1;
		g_pd3dCommandQueue->Signal(g_fence, fenceValue);
		g_fenceLastSignaledValue = fenceValue;
		frameCtx->FenceValue = fenceValue;
	}

	WaitForLastSubmittedFrame();

	// Cleanup
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = NUM_BACK_BUFFERS;
		sd.Width = 0;
		sd.Height = 0;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.Stereo = FALSE;
	}

	// [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
	ID3D12Debug* pdx12Debug = NULL;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
		pdx12Debug->EnableDebugLayer();
#endif

	// Create device
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
		return false;

		// [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
	if (pdx12Debug != NULL)
	{
		ID3D12InfoQueue* pInfoQueue = NULL;
		g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		pInfoQueue->Release();
		pdx12Debug->Release();
	}
#endif

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = NUM_BACK_BUFFERS;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
			return false;

		SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			g_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
			return false;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 1;
		if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
			return false;
	}

	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
			return false;

	if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
		g_pd3dCommandList->Close() != S_OK)
		return false;

	if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
		return false;

	g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (g_fenceEvent == NULL)
		return false;

	{
		IDXGIFactory4* dxgiFactory = NULL;
		IDXGISwapChain1* swapChain1 = NULL;
		if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
			return false;
		if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK)
			return false;
		if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
			return false;
		swapChain1->Release();
		dxgiFactory->Release();
		g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
		g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
	}

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain)
	{
		g_pSwapChain->SetFullscreenState(false, NULL);
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
	}
	if (g_hSwapChainWaitableObject != NULL)
	{
		CloseHandle(g_hSwapChainWaitableObject);
	}
	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (g_frameContext[i].CommandAllocator)
		{
			g_frameContext[i].CommandAllocator->Release();
			g_frameContext[i].CommandAllocator = NULL;
		}
	if (g_pd3dCommandQueue)
	{
		g_pd3dCommandQueue->Release();
		g_pd3dCommandQueue = NULL;
	}
	if (g_pd3dCommandList)
	{
		g_pd3dCommandList->Release();
		g_pd3dCommandList = NULL;
	}
	if (g_pd3dRtvDescHeap)
	{
		g_pd3dRtvDescHeap->Release();
		g_pd3dRtvDescHeap = NULL;
	}
	if (g_pd3dSrvDescHeap)
	{
		g_pd3dSrvDescHeap->Release();
		g_pd3dSrvDescHeap = NULL;
	}
	if (g_fence)
	{
		g_fence->Release();
		g_fence = NULL;
	}
	if (g_fenceEvent)
	{
		CloseHandle(g_fenceEvent);
		g_fenceEvent = NULL;
	}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}

#ifdef DX12_ENABLE_DEBUG_LAYER
	IDXGIDebug1* pDebug = NULL;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
	{
		pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
		pDebug->Release();
	}
#endif
}

void CreateRenderTarget()
{
	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
	{
		ID3D12Resource* pBackBuffer = NULL;
		g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
		g_mainRenderTargetResource[i] = pBackBuffer;
	}
}

void CleanupRenderTarget()
{
	WaitForLastSubmittedFrame();

	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		if (g_mainRenderTargetResource[i])
		{
			g_mainRenderTargetResource[i]->Release();
			g_mainRenderTargetResource[i] = NULL;
		}
}

void WaitForLastSubmittedFrame()
{
	FrameContext* frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

	UINT64 fenceValue = frameCtx->FenceValue;
	if (fenceValue == 0)
		return; // No fence was signaled

	frameCtx->FenceValue = 0;
	if (g_fence->GetCompletedValue() >= fenceValue)
		return;

	g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
	WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext* WaitForNextFrameResources()
{
	UINT nextFrameIndex = g_frameIndex + 1;
	g_frameIndex = nextFrameIndex;

	HANDLE waitableObjects[] = {g_hSwapChainWaitableObject, NULL};
	DWORD numWaitableObjects = 1;

	FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
	UINT64 fenceValue = frameCtx->FenceValue;
	if (fenceValue != 0) // means no fence was signaled
	{
		frameCtx->FenceValue = 0;
		g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
		waitableObjects[1] = g_fenceEvent;
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	return frameCtx;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				WaitForLastSubmittedFrame();
				CleanupRenderTarget();
				HRESULT result = g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
				assert(SUCCEEDED(result) && "Failed to resize swapchain.");
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
