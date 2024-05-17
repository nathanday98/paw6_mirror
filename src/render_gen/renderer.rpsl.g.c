/* Provide Declarations */
#include <stdarg.h>
#include <setjmp.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#ifndef __cplusplus
typedef unsigned char bool;
#endif

/* get a declaration for alloca */
#if defined(__CYGWIN__) || defined(__MINGW32__)
#define  alloca(x) __builtin_alloca((x))
#define _alloca(x) __builtin_alloca((x))
#elif defined(__APPLE__)
extern void *__builtin_alloca(unsigned long);
#define alloca(x) __builtin_alloca(x)
#define longjmp _longjmp
#define setjmp _setjmp
#elif defined(__sun__)
#if defined(__sparcv9)
extern void *__builtin_alloca(unsigned long);
#else
extern void *__builtin_alloca(unsigned int);
#endif
#define alloca(x) __builtin_alloca(x)
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || defined(__arm__)
#define alloca(x) __builtin_alloca(x)
#elif defined(_MSC_VER)
#define alloca(x) _alloca(x)
#else
#include <alloca.h>
#endif

#ifndef _MSC_VER
#define __forceinline __attribute__((always_inline)) inline
#endif

#if defined(__GNUC__)
#define  __ATTRIBUTELIST__(x) __attribute__(x)
#else
#define  __ATTRIBUTELIST__(x)  
#endif

#ifdef _MSC_VER  /* Can only support "linkonce" vars with GCC */
#define __attribute__(X)
#endif

#ifdef _MSC_VER  /* Handle __declspec(dllexport) */
#define __CBE_DLLEXPORT__ __declspec(dllexport)
#define __CBE_DLLIMPORT__ __declspec(dllimport)
#else
#define __CBE_DLLEXPORT__ __attribute__ ((visibility ("default")))
#define __CBE_DLLIMPORT__ __attribute__ ((visibility ("default")))
#endif

#define __asm__(X) /* RPS: Ignore __asm__(Name) */

#if defined(_MSC_VER) && !defined(__clang__)
#define __MSALIGN__(X) __declspec(align(X))
#else
#define __MSALIGN__(X)
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4101)
#pragma warning(disable: 4133)
#pragma warning(disable: 4146)
#endif//_MSC_VER


/* Global Declarations */

/* Types Declarations */
struct l_unnamed_1;
struct l_struct____rpsl_node_info_struct;
struct l_struct____rpsl_entry_desc_struct;
struct l_struct____rpsl_type_info_struct;
struct l_struct____rpsl_params_info_struct;
struct l_struct____rpsl_shader_ref_struct;
struct l_struct____rpsl_pipeline_info_struct;
struct l_struct____rpsl_pipeline_field_info_struct;
struct l_struct____rpsl_pipeline_res_binding_info_struct;
struct l_struct____rpsl_module_info_struct;
struct l_struct_struct_OC_RpsTypeInfo;
struct l_struct_struct_OC_RpsParameterDesc;
struct l_struct_struct_OC_RpsNodeDesc;
struct l_struct_struct_OC_RpslEntry;
struct SubresourceRange;
struct texture;
struct ResourceDesc;
struct l_struct_class_OC_matrix_OC_float_OC_4_OC_4;

/* Function definitions */
typedef void l_fptr_1(uint32_t, uint8_t**, uint32_t);


/* Types Definitions */

/** RPS Built-In Types Begin **/

typedef enum _RpsFormat {
    _RPS_FORMAT_UNKNOWN = 0,
    _RPS_FORMAT_R32G32B32A32_TYPELESS,
    _RPS_FORMAT_R32G32B32A32_FLOAT,
    _RPS_FORMAT_R32G32B32A32_UINT,
    _RPS_FORMAT_R32G32B32A32_SINT,
    _RPS_FORMAT_R32G32B32_TYPELESS,
    _RPS_FORMAT_R32G32B32_FLOAT,
    _RPS_FORMAT_R32G32B32_UINT,
    _RPS_FORMAT_R32G32B32_SINT,
    _RPS_FORMAT_R16G16B16A16_TYPELESS,
    _RPS_FORMAT_R16G16B16A16_FLOAT,
    _RPS_FORMAT_R16G16B16A16_UNORM,
    _RPS_FORMAT_R16G16B16A16_UINT,
    _RPS_FORMAT_R16G16B16A16_SNORM,
    _RPS_FORMAT_R16G16B16A16_SINT,
    _RPS_FORMAT_R32G32_TYPELESS,
    _RPS_FORMAT_R32G32_FLOAT,
    _RPS_FORMAT_R32G32_UINT,
    _RPS_FORMAT_R32G32_SINT,
    _RPS_FORMAT_R32G8X24_TYPELESS,
    _RPS_FORMAT_D32_FLOAT_S8X24_UINT,
    _RPS_FORMAT_R32_FLOAT_X8X24_TYPELESS,
    _RPS_FORMAT_X32_TYPELESS_G8X24_UINT,
    _RPS_FORMAT_R10G10B10A2_TYPELESS,
    _RPS_FORMAT_R10G10B10A2_UNORM,
    _RPS_FORMAT_R10G10B10A2_UINT,
    _RPS_FORMAT_R11G11B10_FLOAT,
    _RPS_FORMAT_R8G8B8A8_TYPELESS,
    _RPS_FORMAT_R8G8B8A8_UNORM,
    _RPS_FORMAT_R8G8B8A8_UNORM_SRGB,
    _RPS_FORMAT_R8G8B8A8_UINT,
    _RPS_FORMAT_R8G8B8A8_SNORM,
    _RPS_FORMAT_R8G8B8A8_SINT,
    _RPS_FORMAT_R16G16_TYPELESS,
    _RPS_FORMAT_R16G16_FLOAT,
    _RPS_FORMAT_R16G16_UNORM,
    _RPS_FORMAT_R16G16_UINT,
    _RPS_FORMAT_R16G16_SNORM,
    _RPS_FORMAT_R16G16_SINT,
    _RPS_FORMAT_R32_TYPELESS,
    _RPS_FORMAT_D32_FLOAT,
    _RPS_FORMAT_R32_FLOAT,
    _RPS_FORMAT_R32_UINT,
    _RPS_FORMAT_R32_SINT,
    _RPS_FORMAT_R24G8_TYPELESS,
    _RPS_FORMAT_D24_UNORM_S8_UINT,
    _RPS_FORMAT_R24_UNORM_X8_TYPELESS,
    _RPS_FORMAT_X24_TYPELESS_G8_UINT,
    _RPS_FORMAT_R8G8_TYPELESS,
    _RPS_FORMAT_R8G8_UNORM,
    _RPS_FORMAT_R8G8_UINT,
    _RPS_FORMAT_R8G8_SNORM,
    _RPS_FORMAT_R8G8_SINT,
    _RPS_FORMAT_R16_TYPELESS,
    _RPS_FORMAT_R16_FLOAT,
    _RPS_FORMAT_D16_UNORM,
    _RPS_FORMAT_R16_UNORM,
    _RPS_FORMAT_R16_UINT,
    _RPS_FORMAT_R16_SNORM,
    _RPS_FORMAT_R16_SINT,
    _RPS_FORMAT_R8_TYPELESS,
    _RPS_FORMAT_R8_UNORM,
    _RPS_FORMAT_R8_UINT,
    _RPS_FORMAT_R8_SNORM,
    _RPS_FORMAT_R8_SINT,
    _RPS_FORMAT_A8_UNORM,
    _RPS_FORMAT_R1_UNORM,
    _RPS_FORMAT_R9G9B9E5_SHAREDEXP,
    _RPS_FORMAT_R8G8_B8G8_UNORM,
    _RPS_FORMAT_G8R8_G8B8_UNORM,
    _RPS_FORMAT_BC1_TYPELESS,
    _RPS_FORMAT_BC1_UNORM,
    _RPS_FORMAT_BC1_UNORM_SRGB,
    _RPS_FORMAT_BC2_TYPELESS,
    _RPS_FORMAT_BC2_UNORM,
    _RPS_FORMAT_BC2_UNORM_SRGB,
    _RPS_FORMAT_BC3_TYPELESS,
    _RPS_FORMAT_BC3_UNORM,
    _RPS_FORMAT_BC3_UNORM_SRGB,
    _RPS_FORMAT_BC4_TYPELESS,
    _RPS_FORMAT_BC4_UNORM,
    _RPS_FORMAT_BC4_SNORM,
    _RPS_FORMAT_BC5_TYPELESS,
    _RPS_FORMAT_BC5_UNORM,
    _RPS_FORMAT_BC5_SNORM,
    _RPS_FORMAT_B5G6R5_UNORM,
    _RPS_FORMAT_B5G5R5A1_UNORM,
    _RPS_FORMAT_B8G8R8A8_UNORM,
    _RPS_FORMAT_B8G8R8X8_UNORM,
    _RPS_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
    _RPS_FORMAT_B8G8R8A8_TYPELESS,
    _RPS_FORMAT_B8G8R8A8_UNORM_SRGB,
    _RPS_FORMAT_B8G8R8X8_TYPELESS,
    _RPS_FORMAT_B8G8R8X8_UNORM_SRGB,
    _RPS_FORMAT_BC6H_TYPELESS,
    _RPS_FORMAT_BC6H_UF16,
    _RPS_FORMAT_BC6H_SF16,
    _RPS_FORMAT_BC7_TYPELESS,
    _RPS_FORMAT_BC7_UNORM,
    _RPS_FORMAT_BC7_UNORM_SRGB,
    _RPS_FORMAT_AYUV,
    _RPS_FORMAT_Y410,
    _RPS_FORMAT_Y416,
    _RPS_FORMAT_NV12,
    _RPS_FORMAT_P010,
    _RPS_FORMAT_P016,
    _RPS_FORMAT_420_OPAQUE,
    _RPS_FORMAT_YUY2,
    _RPS_FORMAT_Y210,
    _RPS_FORMAT_Y216,
    _RPS_FORMAT_NV11,
    _RPS_FORMAT_AI44,
    _RPS_FORMAT_IA44,
    _RPS_FORMAT_P8,
    _RPS_FORMAT_A8P8,
    _RPS_FORMAT_B4G4R4A4_UNORM,

    _RPS_FORMAT_COUNT,

    _RPS_FORMAT_FORCE_INT32 = 0x7FFFFFFF,
} _RpsFormat;

typedef enum _RpsResourceType
{
    _RPS_RESOURCE_TYPE_BUFFER = 0,
    _RPS_RESOURCE_TYPE_IMAGE_1D,
    _RPS_RESOURCE_TYPE_IMAGE_2D,
    _RPS_RESOURCE_TYPE_IMAGE_3D,
    _RPS_RESOURCE_TYPE_UNKNOWN,

    _RPS_RESOURCE_TYPE_FORCE_INT32 = 0x7FFFFFFF,
} _RpsResourceType;

typedef enum _RpsResourceFlags
{
    RPS_RESOURCE_FLAG_NONE                        = 0,
    RPS_RESOURCE_CUBEMAP_COMPATIBLE_BIT           = (1 << 1),
    RPS_RESOURCE_ROWMAJOR_IMAGE_BIT               = (1 << 2),
    RPS_RESOURCE_PREFER_GPU_LOCAL_CPU_VISIBLE_BIT = (1 << 3),
    RPS_RESOURCE_PREFER_DEDICATED_BIT             = (1 << 4),
    RPS_RESOURCE_PERSISTENT_BIT                   = (1 << 15),
} _RpsResourceFlags;


typedef struct SubresourceRange {
  uint16_t base_mip_level;
  uint16_t mip_level_count;
  uint32_t base_array_layer;
  uint32_t array_layer_count;
} SubresourceRange;

typedef struct texture {
  uint32_t Resource;
  _RpsFormat Format;
  uint32_t TemporalLayer;
  uint32_t Flags;
  SubresourceRange SubresourceRange;
  float MinLodClamp;
  uint32_t ComponentMapping;
} texture;

typedef struct buffer {
  uint32_t Resource;
  _RpsFormat Format;
  uint32_t TemporalLayer;
  uint32_t Flags;
  uint64_t Offset;
  uint64_t SizeInBytes;
  uint32_t StructureByteStride;
} buffer;

typedef struct ResourceDesc {
  _RpsResourceType Type;
  uint32_t TemporalLayers;
  _RpsResourceFlags Flags;
  uint32_t Width;
  uint32_t Height;
  uint32_t DepthOrArraySize;
  uint32_t MipLevels;
  _RpsFormat Format;
  uint32_t SampleCount;
} ResourceDesc;

typedef struct RpsViewport {
  float x;
  float y;
  float width;
  float height;
  float minZ;
  float maxZ;
} RpsViewport;

typedef struct ShaderModule {
  uint32_t h;
} ShaderModule;

typedef struct Pipeline {
  uint32_t h;
} Pipeline;
/** RPS Built-In Types End **/

struct l_unnamed_1 {
  uint32_t field0;
  uint32_t field1;
  uint32_t field2;
  uint32_t field3;
};
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct l_struct____rpsl_node_info_struct {
  uint32_t field0;
  uint32_t field1;
  uint32_t field2;
  uint32_t field3;
  uint32_t field4;
} __attribute__ ((packed));
#ifdef _MSC_VER
#pragma pack(pop)
#endif
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct l_struct____rpsl_entry_desc_struct {
  uint32_t field0;
  uint32_t field1;
  uint32_t field2;
  uint32_t field3;
  uint8_t* field4;
  uint8_t* field5;
} __attribute__ ((packed));
#ifdef _MSC_VER
#pragma pack(pop)
#endif
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct l_struct____rpsl_type_info_struct {
  uint8_t field0;
  uint8_t field1;
  uint8_t field2;
  uint8_t field3;
  uint32_t field4;
  uint32_t field5;
  uint32_t field6;
} __attribute__ ((packed));
#ifdef _MSC_VER
#pragma pack(pop)
#endif
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct l_struct____rpsl_params_info_struct {
  uint32_t field0;
  uint32_t field1;
  uint32_t field2;
  uint32_t field3;
  uint32_t field4;
  uint16_t field5;
  uint16_t field6;
} __attribute__ ((packed));
#ifdef _MSC_VER
#pragma pack(pop)
#endif
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct l_struct____rpsl_shader_ref_struct {
  uint32_t field0;
  uint32_t field1;
  uint32_t field2;
  uint32_t field3;
} __attribute__ ((packed));
#ifdef _MSC_VER
#pragma pack(pop)
#endif
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct l_struct____rpsl_pipeline_info_struct {
  uint32_t field0;
  uint32_t field1;
  uint32_t field2;
  uint32_t field3;
} __attribute__ ((packed));
#ifdef _MSC_VER
#pragma pack(pop)
#endif
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct l_struct____rpsl_pipeline_field_info_struct {
  uint32_t field0;
  uint32_t field1;
  uint32_t field2;
  uint32_t field3;
  uint32_t field4;
  uint32_t field5;
  uint32_t field6;
  uint32_t field7;
} __attribute__ ((packed));
#ifdef _MSC_VER
#pragma pack(pop)
#endif
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct l_struct____rpsl_pipeline_res_binding_info_struct {
  uint32_t field0;
  uint32_t field1;
  uint32_t field2;
  uint32_t field3;
} __attribute__ ((packed));
#ifdef _MSC_VER
#pragma pack(pop)
#endif
struct l_array_329_uint8_t {
  uint8_t array[329];
};
struct l_array_5_struct_AC_l_struct____rpsl_node_info_struct {
  struct l_struct____rpsl_node_info_struct array[5];
};
struct l_array_9_struct_AC_l_struct____rpsl_type_info_struct {
  struct l_struct____rpsl_type_info_struct array[9];
};
struct l_array_21_struct_AC_l_struct____rpsl_params_info_struct {
  struct l_struct____rpsl_params_info_struct array[21];
};
struct l_array_2_struct_AC_l_struct____rpsl_entry_desc_struct {
  struct l_struct____rpsl_entry_desc_struct array[2];
};
struct l_array_1_struct_AC_l_struct____rpsl_shader_ref_struct {
  struct l_struct____rpsl_shader_ref_struct array[1];
};
struct l_array_1_struct_AC_l_struct____rpsl_pipeline_info_struct {
  struct l_struct____rpsl_pipeline_info_struct array[1];
};
struct l_array_1_struct_AC_l_struct____rpsl_pipeline_field_info_struct {
  struct l_struct____rpsl_pipeline_field_info_struct array[1];
};
struct l_array_1_struct_AC_l_struct____rpsl_pipeline_res_binding_info_struct {
  struct l_struct____rpsl_pipeline_res_binding_info_struct array[1];
};
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct l_struct____rpsl_module_info_struct {
  uint32_t field0;
  uint32_t field1;
  uint32_t field2;
  uint32_t field3;
  uint32_t field4;
  uint32_t field5;
  uint32_t field6;
  uint32_t field7;
  uint32_t field8;
  uint32_t field9;
  uint32_t field10;
  uint32_t field11;
  uint32_t field12;
  struct l_array_329_uint8_t* field13;
  struct l_array_5_struct_AC_l_struct____rpsl_node_info_struct* field14;
  struct l_array_9_struct_AC_l_struct____rpsl_type_info_struct* field15;
  struct l_array_21_struct_AC_l_struct____rpsl_params_info_struct* field16;
  struct l_array_2_struct_AC_l_struct____rpsl_entry_desc_struct* field17;
  struct l_array_1_struct_AC_l_struct____rpsl_shader_ref_struct* field18;
  struct l_array_1_struct_AC_l_struct____rpsl_pipeline_info_struct* field19;
  struct l_array_1_struct_AC_l_struct____rpsl_pipeline_field_info_struct* field20;
  struct l_array_1_struct_AC_l_struct____rpsl_pipeline_res_binding_info_struct* field21;
  uint32_t field22;
} __attribute__ ((packed));
#ifdef _MSC_VER
#pragma pack(pop)
#endif
struct l_struct_struct_OC_RpsTypeInfo {
  uint16_t field0;
  uint16_t field1;
};
struct l_struct_struct_OC_RpsParameterDesc {
  struct l_struct_struct_OC_RpsTypeInfo field0;
  uint32_t field1;
  struct l_unnamed_1* field2;
  uint8_t* field3;
  uint32_t field4;
};
struct l_struct_struct_OC_RpsNodeDesc {
  uint32_t field0;
  uint32_t field1;
  struct l_struct_struct_OC_RpsParameterDesc* field2;
  uint8_t* field3;
};
struct l_struct_struct_OC_RpslEntry {
  uint8_t* field0;
  l_fptr_1* field1;
  struct l_struct_struct_OC_RpsParameterDesc* field2;
  struct l_struct_struct_OC_RpsNodeDesc* field3;
  uint32_t field4;
  uint32_t field5;
};
struct l_vector_4_float {
  float vector[4];
} __attribute__((aligned(4)));
__MSALIGN__(4) struct l_array_4_struct_AC_l_vector_4_float {
  __MSALIGN__(4) struct l_vector_4_float array[4];
};
struct l_struct_class_OC_matrix_OC_float_OC_4_OC_4 {
  __MSALIGN__(4) struct l_array_4_struct_AC_l_vector_4_float field0;
};
struct l_array_14_uint8_t {
  uint8_t array[14];
};
struct l_array_19_uint8_t {
  uint8_t array[19];
};
struct l_array_30_uint8_t {
  uint8_t array[30];
};
struct l_vector_2_uint32_t {
  uint32_t vector[2];
} __attribute__((aligned(4)));
struct l_array_12_uint8_t {
  uint8_t array[12];
};
struct l_array_2_uint8_t {
  uint8_t array[2];
};
struct l_array_5_uint8_t {
  uint8_t array[5];
};
struct l_array_2_struct_AC_l_struct_struct_OC_RpsParameterDesc {
  struct l_struct_struct_OC_RpsParameterDesc array[2];
};
struct l_array_20_uint8_t {
  uint8_t array[20];
};
struct l_array_7_uint8_t {
  uint8_t array[7];
};
struct l_array_4_struct_AC_l_struct_struct_OC_RpsParameterDesc {
  struct l_struct_struct_OC_RpsParameterDesc array[4];
};
struct l_array_8_uint8_t {
  uint8_t array[8];
};
struct l_array_4_uint8_t {
  uint8_t array[4];
};
struct l_array_10_uint8_t {
  uint8_t array[10];
};
struct l_array_6_struct_AC_l_struct_struct_OC_RpsParameterDesc {
  struct l_struct_struct_OC_RpsParameterDesc array[6];
};
struct l_array_16_uint8_t {
  uint8_t array[16];
};
struct l_array_6_uint8_t {
  uint8_t array[6];
};
struct l_array_4_struct_AC_l_struct_struct_OC_RpsNodeDesc {
  struct l_struct_struct_OC_RpsNodeDesc array[4];
};
struct l_array_11_uint8_t {
  uint8_t array[11];
};
struct l_array_13_uint8_t {
  uint8_t array[13];
};
struct l_array_22_uint8_t {
  uint8_t array[22];
};
struct l_array_18_uint8_t {
  uint8_t array[18];
};
struct l_array_24_uint8_t {
  uint8_t array[24];
};
struct l_array_1_uint32_t {
  uint32_t array[1];
};

/* External Global Variable Declarations */

/* Function Declarations */
static struct texture _BA__PD_make_default_texture_view_from_desc_AE__AE_YA_PD_AUtexture_AE__AE_IUResourceDesc_AE__AE__AE_Z(uint32_t, struct ResourceDesc*) __ATTRIBUTELIST__((nothrow)) __asm__ ("?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z");
void rpsl_M_renderer_Fn_main(struct texture*, struct texture*, bool, struct l_vector_2_uint32_t, struct l_struct_class_OC_matrix_OC_float_OC_4_OC_4, uint32_t) __ATTRIBUTELIST__((nothrow));
void ___rpsl_abort(uint32_t);
uint32_t ___rpsl_node_call(uint32_t, uint32_t, uint8_t**, uint32_t, uint32_t);
void ___rpsl_block_marker(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void ___rpsl_describe_handle(uint8_t*, uint32_t, uint32_t*, uint32_t);
uint32_t ___rpsl_create_resource(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void ___rpsl_name_resource(uint32_t, uint8_t*, uint32_t);
uint32_t ___rpsl_dxop_binary_i32(uint32_t, uint32_t, uint32_t);
void rpsl_M_renderer_Fn_main_wrapper(uint32_t, uint8_t**, uint32_t) __ATTRIBUTELIST__((noinline, nothrow));


/* Global Variable Definitions and Initialization */
static struct l_array_14_uint8_t _AE__AE_rps_Str0 = { "game_color_rt" };
static struct l_array_19_uint8_t _AE__AE_rps_Str1 = { "game_color_rt_msaa" };
static struct l_array_19_uint8_t _AE__AE_rps_Str2 = { "game_depth_rt_msaa" };
static struct l_array_30_uint8_t _AE__AE_rps_Str3 = { "game_color_rt_pre_display_map" };
static __MSALIGN__(4) struct l_array_5_struct_AC_l_struct____rpsl_node_info_struct ___rpsl_nodedefs_renderer __attribute__((aligned(4))) = { { { 0, 176, 0, 2, 1 }, { 1, 188, 2, 4, 1 }, { 2, 208, 6, 6, 1 }, { 3, 216, 12, 2, 1 }, { 0, 0, 0, 0, 0 } } };
static __MSALIGN__(4) struct l_array_2_struct_AC_l_struct____rpsl_entry_desc_struct ___rpsl_entries_renderer __attribute__((aligned(4))) = { { { 0, 232, 14, 6, ((uint8_t*)rpsl_M_renderer_Fn_main), ((uint8_t*)rpsl_M_renderer_Fn_main_wrapper) }, { 0, 0, 0, 0, ((uint8_t*)/*NULL*/0), ((uint8_t*)/*NULL*/0) } } };
static __MSALIGN__(4) struct l_array_9_struct_AC_l_struct____rpsl_type_info_struct ___rpsl_types_metadata_renderer __attribute__((aligned(4))) = { { { 6, 0, 0, 0, 0, 36, 4 }, { 4, 32, 0, 4, 0, 16, 4 }, { 3, 32, 0, 0, 0, 4, 4 }, { 4, 32, 0, 0, 0, 4, 4 }, { 3, 32, 0, 2, 0, 8, 4 }, { 1, 8, 0, 0, 0, 4, 4 }, { 2, 32, 0, 2, 0, 8, 4 }, { 4, 32, 4, 4, 0, 64, 4 }, { 0, 0, 0, 0, 0, 0, 0 } } };
static __MSALIGN__(4) struct l_array_21_struct_AC_l_struct____rpsl_params_info_struct ___rpsl_params_metadata_renderer __attribute__((aligned(4))) = { { { 91, 0, 272629888, -1, 0, 36, 0 }, { 93, 1, 0, -1, 0, 16, 36 }, { 91, 0, 289409536, -1, 0, 36, 0 }, { 98, 2, 0, -1, 0, 4, 36 }, { 105, 3, 0, -1, 0, 4, 40 }, { 107, 2, 0, -1, 0, 4, 44 }, { 109, 0, 65536, -1, 0, 36, 0 }, { 113, 4, 0, -1, 0, 8, 36 }, { 123, 0, 32768, -1, 0, 36, 44 }, { 127, 4, 0, -1, 0, 8, 80 }, { 137, 4, 0, -1, 0, 8, 88 }, { 144, 2, 0, -1, 0, 4, 96 }, { 156, 0, 128, -1, 0, 36, 0 }, { 170, 0, 16, -1, 0, 36, 36 }, { 237, 0, 524288, -1, 0, 36, 0 }, { 248, 0, 16, -1, 0, 36, 36 }, { 261, 5, 0, -1, 0, 4, 72 }, { 265, 6, 0, -1, 0, 8, 76 }, { 287, 7, 0, -1, 0, 64, 84 }, { 305, 2, 0, -1, 0, 4, 148 }, { 0, 0, 0, 0, 0, 0, 0 } } };
static __MSALIGN__(4) struct l_array_1_struct_AC_l_struct____rpsl_shader_ref_struct ___rpsl_shader_refs_renderer __attribute__((aligned(4)));
static __MSALIGN__(4) struct l_array_1_struct_AC_l_struct____rpsl_pipeline_info_struct ___rpsl_pipelines_renderer __attribute__((aligned(4)));
static __MSALIGN__(4) struct l_array_1_struct_AC_l_struct____rpsl_pipeline_field_info_struct ___rpsl_pipeline_fields_renderer __attribute__((aligned(4)));
static __MSALIGN__(4) struct l_array_1_struct_AC_l_struct____rpsl_pipeline_res_binding_info_struct ___rpsl_pipeline_res_bindings_renderer __attribute__((aligned(4)));
__MSALIGN__(4) struct l_array_329_uint8_t ___rpsl_string_table_renderer __attribute__((aligned(4))) = { { 103u, 97u, 109u, 101u, 95u, 99u, 111u, 108u, 111u, 114u, 95u, 114u, 116u, 0, 103u, 97u, 109u, 101u, 95u, 99u, 111u, 108u, 111u, 114u, 95u, 114u, 116u, 95u, 109u, 115u, 97u, 97u, 0, 103u, 97u, 109u, 101u, 95u, 100u, 101u, 112u, 116u, 104u, 95u, 114u, 116u, 95u, 109u, 115u, 97u, 97u, 0, 103u, 97u, 109u, 101u, 95u, 99u, 111u, 108u, 111u, 114u, 95u, 114u, 116u, 95u, 112u, 114u, 101u, 95u, 100u, 105u, 115u, 112u, 108u, 97u, 121u, 95u, 109u, 97u, 112u, 0, 114u, 101u, 110u, 100u, 101u, 114u, 101u, 114u, 0, 116u, 0, 100u, 97u, 116u, 97u, 0, 111u, 112u, 116u, 105u, 111u, 110u, 0, 100u, 0, 115u, 0, 100u, 115u, 116u, 0, 100u, 115u, 116u, 79u, 102u, 102u, 115u, 101u, 116u, 0, 115u, 114u, 99u, 0, 115u, 114u, 99u, 79u, 102u, 102u, 115u, 101u, 116u, 0, 101u, 120u, 116u, 101u, 110u, 116u, 0, 114u, 101u, 115u, 111u, 108u, 118u, 101u, 77u, 111u, 100u, 101u, 0, 114u, 101u, 110u, 100u, 101u, 114u, 95u, 116u, 97u, 114u, 103u, 101u, 116u, 0, 105u, 110u, 112u, 117u, 116u, 0, 99u, 108u, 101u, 97u, 114u, 95u, 99u, 111u, 108u, 111u, 114u, 0, 99u, 108u, 101u, 97u, 114u, 95u, 100u, 101u, 112u, 116u, 104u, 95u, 115u, 116u, 101u, 110u, 99u, 105u, 108u, 0, 114u, 101u, 115u, 111u, 108u, 118u, 101u, 0, 100u, 105u, 115u, 112u, 108u, 97u, 121u, 95u, 109u, 97u, 112u, 95u, 104u, 100u, 114u, 0, 109u, 97u, 105u, 110u, 0, 98u, 97u, 99u, 107u, 98u, 117u, 102u, 102u, 101u, 114u, 0, 116u, 101u, 120u, 116u, 117u, 114u, 101u, 95u, 97u, 99u, 101u, 115u, 0, 104u, 100u, 114u, 0, 105u, 110u, 95u, 103u, 97u, 109u, 101u, 95u, 118u, 105u, 101u, 119u, 112u, 111u, 114u, 116u, 95u, 115u, 105u, 122u, 101u, 0, 99u, 97u, 109u, 101u, 114u, 97u, 95u, 112u, 114u, 111u, 106u, 101u, 99u, 116u, 105u, 111u, 110u, 0, 103u, 97u, 109u, 101u, 95u, 99u, 111u, 108u, 111u, 114u, 95u, 115u, 97u, 109u, 112u, 108u, 101u, 95u, 99u, 111u, 117u, 110u, 116u, 0 } };
__CBE_DLLEXPORT__ __MSALIGN__(4) struct l_struct____rpsl_module_info_struct ___rpsl_module_info_renderer __attribute__((aligned(4))) = { 1297305682u, 3, 9, 82, 329, 4, 8, 20, 1, 0, 0, 0, 0, (&___rpsl_string_table_renderer), (&___rpsl_nodedefs_renderer), (&___rpsl_types_metadata_renderer), (&___rpsl_params_metadata_renderer), (&___rpsl_entries_renderer), (&___rpsl_shader_refs_renderer), (&___rpsl_pipelines_renderer), (&___rpsl_pipeline_fields_renderer), (&___rpsl_pipeline_res_bindings_renderer), 1297305682u };
static struct l_array_12_uint8_t _AE__AE_rps_Str4 = { "clear_color" };
static struct l_array_2_uint8_t _AE__AE_rps_Str5 = { "t" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr6 = { 272629888, 0, 0, 0 };
static struct l_array_5_uint8_t _AE__AE_rps_Str7 = { "data" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr8 = { 0, 0, 27, 0 };
static struct l_array_2_struct_AC_l_struct_struct_OC_RpsParameterDesc _AE__AE_rps_ParamDescArray9 = { { { { 36, 64 }, 0, (&_AE__AE_rps_ParamAttr6), ((&_AE__AE_rps_Str5.array[((int32_t)0)])), 4 }, { { 16, 0 }, 0, (&_AE__AE_rps_ParamAttr8), ((&_AE__AE_rps_Str7.array[((int32_t)0)])), 0 } } };
static struct l_array_20_uint8_t _AE__AE_rps_Str10 = { "clear_depth_stencil" };
static struct l_array_2_uint8_t _AE__AE_rps_Str11 = { "t" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr12 = { 289409536, 0, 0, 0 };
static struct l_array_7_uint8_t _AE__AE_rps_Str13 = { "option" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr14;
static struct l_array_2_uint8_t _AE__AE_rps_Str15 = { "d" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr16 = { 0, 0, 28, 0 };
static struct l_array_2_uint8_t _AE__AE_rps_Str17 = { "s" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr18 = { 0, 0, 29, 0 };
static struct l_array_4_struct_AC_l_struct_struct_OC_RpsParameterDesc _AE__AE_rps_ParamDescArray19 = { { { { 36, 64 }, 0, (&_AE__AE_rps_ParamAttr12), ((&_AE__AE_rps_Str11.array[((int32_t)0)])), 4 }, { { 4, 0 }, 0, (&_AE__AE_rps_ParamAttr14), ((&_AE__AE_rps_Str13.array[((int32_t)0)])), 0 }, { { 4, 0 }, 0, (&_AE__AE_rps_ParamAttr16), ((&_AE__AE_rps_Str15.array[((int32_t)0)])), 0 }, { { 4, 0 }, 0, (&_AE__AE_rps_ParamAttr18), ((&_AE__AE_rps_Str17.array[((int32_t)0)])), 0 } } };
static struct l_array_8_uint8_t _AE__AE_rps_Str20 = { "resolve" };
static struct l_array_4_uint8_t _AE__AE_rps_Str21 = { "dst" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr22 = { 65536, 0, 0, 0 };
static struct l_array_10_uint8_t _AE__AE_rps_Str23 = { "dstOffset" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr24;
static struct l_array_4_uint8_t _AE__AE_rps_Str25 = { "src" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr26 = { 32768, 0, 0, 0 };
static struct l_array_10_uint8_t _AE__AE_rps_Str27 = { "srcOffset" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr28;
static struct l_array_7_uint8_t _AE__AE_rps_Str29 = { "extent" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr30;
static struct l_array_12_uint8_t _AE__AE_rps_Str31 = { "resolveMode" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr32;
static struct l_array_6_struct_AC_l_struct_struct_OC_RpsParameterDesc _AE__AE_rps_ParamDescArray33 = { { { { 36, 64 }, 0, (&_AE__AE_rps_ParamAttr22), ((&_AE__AE_rps_Str21.array[((int32_t)0)])), 4 }, { { 8, 0 }, 0, (&_AE__AE_rps_ParamAttr24), ((&_AE__AE_rps_Str23.array[((int32_t)0)])), 0 }, { { 36, 64 }, 0, (&_AE__AE_rps_ParamAttr26), ((&_AE__AE_rps_Str25.array[((int32_t)0)])), 4 }, { { 8, 0 }, 0, (&_AE__AE_rps_ParamAttr28), ((&_AE__AE_rps_Str27.array[((int32_t)0)])), 0 }, { { 8, 0 }, 0, (&_AE__AE_rps_ParamAttr30), ((&_AE__AE_rps_Str29.array[((int32_t)0)])), 0 }, { { 4, 0 }, 0, (&_AE__AE_rps_ParamAttr32), ((&_AE__AE_rps_Str31.array[((int32_t)0)])), 0 } } };
static struct l_array_16_uint8_t _AE__AE_rps_Str34 = { "display_map_hdr" };
static struct l_array_14_uint8_t _AE__AE_rps_Str35 = { "render_target" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr36 = { 128, 0, 35, 0 };
static struct l_array_6_uint8_t _AE__AE_rps_Str37 = { "input" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr38 = { 16, 10, 0, 0 };
static struct l_array_2_struct_AC_l_struct_struct_OC_RpsParameterDesc _AE__AE_rps_ParamDescArray39 = { { { { 36, 64 }, 0, (&_AE__AE_rps_ParamAttr36), ((&_AE__AE_rps_Str35.array[((int32_t)0)])), 4 }, { { 36, 64 }, 0, (&_AE__AE_rps_ParamAttr38), ((&_AE__AE_rps_Str37.array[((int32_t)0)])), 4 } } };
__CBE_DLLEXPORT__ struct l_array_4_struct_AC_l_struct_struct_OC_RpsNodeDesc NodeDecls_renderer = { { { 1, 2, ((&_AE__AE_rps_ParamDescArray9.array[((int32_t)0)])), ((&_AE__AE_rps_Str4.array[((int32_t)0)])) }, { 1, 4, ((&_AE__AE_rps_ParamDescArray19.array[((int32_t)0)])), ((&_AE__AE_rps_Str10.array[((int32_t)0)])) }, { 1, 6, ((&_AE__AE_rps_ParamDescArray33.array[((int32_t)0)])), ((&_AE__AE_rps_Str20.array[((int32_t)0)])) }, { 1, 2, ((&_AE__AE_rps_ParamDescArray39.array[((int32_t)0)])), ((&_AE__AE_rps_Str34.array[((int32_t)0)])) } } };
static struct l_array_5_uint8_t _AE__AE_rps_Str40 = { "main" };
static struct l_array_11_uint8_t _AE__AE_rps_Str41 = { "backbuffer" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr42 = { 524288, 0, 0, 0 };
static struct l_array_13_uint8_t _AE__AE_rps_Str43 = { "texture_aces" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr44 = { 16, 10, 0, 0 };
static struct l_array_4_uint8_t _AE__AE_rps_Str45 = { "hdr" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr46;
static struct l_array_22_uint8_t _AE__AE_rps_Str47 = { "in_game_viewport_size" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr48;
static struct l_array_18_uint8_t _AE__AE_rps_Str49 = { "camera_projection" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr50;
static struct l_array_24_uint8_t _AE__AE_rps_Str51 = { "game_color_sample_count" };
static struct l_unnamed_1 _AE__AE_rps_ParamAttr52;
static struct l_array_6_struct_AC_l_struct_struct_OC_RpsParameterDesc _AE__AE_rps_ParamDescArray53 = { { { { 36, 64 }, 0, (&_AE__AE_rps_ParamAttr42), ((&_AE__AE_rps_Str41.array[((int32_t)0)])), 4 }, { { 36, 64 }, 0, (&_AE__AE_rps_ParamAttr44), ((&_AE__AE_rps_Str43.array[((int32_t)0)])), 4 }, { { 4, 0 }, 0, (&_AE__AE_rps_ParamAttr46), ((&_AE__AE_rps_Str45.array[((int32_t)0)])), 0 }, { { 8, 0 }, 0, (&_AE__AE_rps_ParamAttr48), ((&_AE__AE_rps_Str47.array[((int32_t)0)])), 0 }, { { 64, 0 }, 0, (&_AE__AE_rps_ParamAttr50), ((&_AE__AE_rps_Str49.array[((int32_t)0)])), 0 }, { { 4, 0 }, 0, (&_AE__AE_rps_ParamAttr52), ((&_AE__AE_rps_Str51.array[((int32_t)0)])), 0 } } };
struct l_struct_struct_OC_RpslEntry rpsl_M_renderer_E_main_AE_value = { ((&_AE__AE_rps_Str40.array[((int32_t)0)])), rpsl_M_renderer_Fn_main_wrapper, ((&_AE__AE_rps_ParamDescArray53.array[((int32_t)0)])), ((&NodeDecls_renderer.array[((int32_t)0)])), 6, 4 };
__CBE_DLLEXPORT__ struct l_struct_struct_OC_RpslEntry* rpsl_M_renderer_E_main = (&rpsl_M_renderer_E_main_AE_value);
__CBE_DLLEXPORT__ struct l_struct_struct_OC_RpslEntry** rpsl_M_renderer_E_main_pp = (&rpsl_M_renderer_E_main);
static struct l_array_1_uint32_t dx_OC_nothing_OC_a;


/* LLVM Intrinsic Builtin Function Bodies */
static __forceinline uint32_t llvm_add_u32(uint32_t a, uint32_t b) {
  uint32_t r = a + b;
  return r;
}
static __forceinline uint32_t llvm_lshr_u32(uint32_t a, uint32_t b) {
  uint32_t r = a >> b;
  return r;
}
static __forceinline struct l_vector_4_float llvm_ctor_f32x4(float x0, float x1, float x2, float x3) {
  __MSALIGN__(4) struct l_vector_4_float r;
  r.vector[0] = x0;
  r.vector[1] = x1;
  r.vector[2] = x2;
  r.vector[3] = x3;
  return r;
}
static __forceinline struct l_vector_2_uint32_t llvm_ctor_u32x2(uint32_t x0, uint32_t x1) {
  __MSALIGN__(4) struct l_vector_2_uint32_t r;
  r.vector[0] = x0;
  r.vector[1] = x1;
  return r;
}


/* Function Bodies */

#line 151 "./___rpsl_builtin_header_.rpsl"
static struct texture _BA__PD_make_default_texture_view_from_desc_AE__AE_YA_PD_AUtexture_AE__AE_IUResourceDesc_AE__AE__AE_Z(uint32_t resourceHdl, struct ResourceDesc* desc) {
  struct texture StructReturn;  /* Struct return temporary */
  struct texture*agg_2e_result = &StructReturn;

  struct {
    struct texture _1;    /* Address-exposed local */
    uint32_t _2;
    uint32_t _3;
    uint32_t _4;
    uint32_t _5;
    uint8_t* _6;
    uint32_t _7;
    uint32_t _8;
    uint32_t _9;
    uint32_t _10;
    uint32_t _11;
    uint32_t _12;
    uint32_t _13;
    uint32_t _14;
    uint32_t _15;
    uint32_t _16;
    uint32_t _17;
    uint32_t _18;
    uint32_t _19;
    uint32_t _20;
    uint32_t _21;
    struct SubresourceRange* _22;
    struct SubresourceRange* _23;
    uint16_t _24;
    uint16_t _25;
    uint32_t _26;
    uint32_t _27;
    float _28;
    uint32_t _29;
    uint32_t _30;
  } _llvm_cbe_tmps;

  struct {
    uint32_t cond;
    uint32_t cond__PHI_TEMPORARY;
  } _llvm_cbe_phi_tmps = {0};

#line 151 "./___rpsl_builtin_header_.rpsl"
  #line 151 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._2 = *((&desc->MipLevels));
#line 151 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._3 = *((&desc->Type));
#line 151 "./___rpsl_builtin_header_.rpsl"
  #line 151 "./___rpsl_builtin_header_.rpsl"
  if ((((((((((_llvm_cbe_tmps._3 == 4u)&1)) != 0)&1)) != 0)&1))) {
#line 151 "./___rpsl_builtin_header_.rpsl"
    goto cond_2e_true;
#line 151 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 151 "./___rpsl_builtin_header_.rpsl"
    goto cond_2e_false;
#line 151 "./___rpsl_builtin_header_.rpsl"
  }

cond_2e_true:
#line 151 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond__PHI_TEMPORARY = 1;   /* for PHI node */
#line 151 "./___rpsl_builtin_header_.rpsl"
  goto cond_2e_end;
#line 151 "./___rpsl_builtin_header_.rpsl"

cond_2e_false:
#line 151 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._4 = *((&desc->DepthOrArraySize));
#line 151 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond__PHI_TEMPORARY = _llvm_cbe_tmps._4;   /* for PHI node */
#line 151 "./___rpsl_builtin_header_.rpsl"
  goto cond_2e_end;
#line 151 "./___rpsl_builtin_header_.rpsl"

cond_2e_end:
#line 151 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond = _llvm_cbe_phi_tmps.cond__PHI_TEMPORARY;
#line 151 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._5 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 151 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._6 = memset((((uint8_t*)(&_llvm_cbe_tmps._1))), 0, 36);
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._7 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._1.Resource)) = resourceHdl;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._8 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._1.Format)) = 0;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._9 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._1.TemporalLayer)) = 0;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._10 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._1.Flags)) = 0;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._11 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._1.SubresourceRange.base_mip_level)) = 0;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._12 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._1.SubresourceRange.mip_level_count)) = (((uint16_t)_llvm_cbe_tmps._2));
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._13 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._1.SubresourceRange.base_array_layer)) = 0;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._14 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._1.SubresourceRange.array_layer_count)) = _llvm_cbe_phi_tmps.cond;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._15 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._1.MinLodClamp)) = ((float)(0.000000e+00));
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._16 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._1.ComponentMapping)) = 50462976;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._17 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._18 = *((&_llvm_cbe_tmps._1.Resource));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_result->Resource)) = _llvm_cbe_tmps._18;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._19 = *((&_llvm_cbe_tmps._1.Format));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_result->Format)) = _llvm_cbe_tmps._19;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._20 = *((&_llvm_cbe_tmps._1.TemporalLayer));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_result->TemporalLayer)) = _llvm_cbe_tmps._20;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._21 = *((&_llvm_cbe_tmps._1.Flags));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_result->Flags)) = _llvm_cbe_tmps._21;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._22 = (&agg_2e_result->SubresourceRange);
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._23 = (&_llvm_cbe_tmps._1.SubresourceRange);
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._24 = *((&_llvm_cbe_tmps._23->base_mip_level));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._22->base_mip_level)) = _llvm_cbe_tmps._24;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._25 = *((&_llvm_cbe_tmps._23->mip_level_count));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._22->mip_level_count)) = _llvm_cbe_tmps._25;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._26 = *((&_llvm_cbe_tmps._23->base_array_layer));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._22->base_array_layer)) = _llvm_cbe_tmps._26;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._27 = *((&_llvm_cbe_tmps._23->array_layer_count));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._22->array_layer_count)) = _llvm_cbe_tmps._27;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._28 = *((&_llvm_cbe_tmps._1.MinLodClamp));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_result->MinLodClamp)) = _llvm_cbe_tmps._28;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._29 = *((&_llvm_cbe_tmps._1.ComponentMapping));
#line 153 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_result->ComponentMapping)) = _llvm_cbe_tmps._29;
#line 153 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._30 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 153 "./___rpsl_builtin_header_.rpsl"
  return StructReturn;
}


#line 13 "renderer.rpsl"
void rpsl_M_renderer_Fn_main(struct texture* backbuffer, struct texture* texture_aces, bool hdr, struct l_vector_2_uint32_t in_game_viewport_size, struct l_struct_class_OC_matrix_OC_float_OC_4_OC_4 camera_projection, uint32_t game_color_sample_count) {
    struct ResourceDesc backbuffer_desc;    /* Address-exposed local */
    struct texture game_color_rt;    /* Address-exposed local */
    struct texture agg_2e_tmp;    /* Address-exposed local */
    struct texture game_color_rt_msaa;    /* Address-exposed local */
    struct texture agg_2e_tmp3;    /* Address-exposed local */
    struct texture game_depth_rt_msaa;    /* Address-exposed local */
    struct texture agg_2e_tmp6;    /* Address-exposed local */
    struct texture game_color_rt_pre_display_map;    /* Address-exposed local */
    struct texture agg_2e_tmp13;    /* Address-exposed local */

  struct {
    struct texture _31;    /* Address-exposed local */
    struct texture _32;    /* Address-exposed local */
    struct texture _33;    /* Address-exposed local */
    struct texture _34;    /* Address-exposed local */
    struct texture _35;    /* Address-exposed local */
    struct texture _36;    /* Address-exposed local */
    struct texture _37;    /* Address-exposed local */
    struct texture _38;    /* Address-exposed local */
    struct texture _39;    /* Address-exposed local */
    uint32_t _40;
    uint32_t _41;
    uint32_t _42;
    uint32_t _43;
    struct SubresourceRange* _44;
    struct SubresourceRange* _45;
    uint16_t _46;
    uint16_t _47;
    uint32_t _48;
    uint32_t _49;
    float _50;
    uint32_t _51;
    uint32_t IMax;
    uint32_t IMax219;
    uint32_t _52;
    uint32_t _53;
    uint32_t _54;
    uint32_t _55;
    uint32_t _56;
    uint32_t _57;
    uint32_t _58;
    uint32_t inc_2e_i_2e_i;
    uint32_t _59;
    uint32_t shr_2e_i_2e_i;
    uint32_t _60;
    uint32_t shr12_2e_i_2e_i;
    uint32_t _61;
    uint32_t shr13_2e_i_2e_i;
    uint32_t _62;
    uint32_t UMin222;
    uint32_t _63;
    uint32_t call1_2e_i;
    uint32_t _64;
    uint32_t _65;
    uint8_t* _66;
    uint32_t _67;
    uint32_t _68;
    uint32_t _69;
    uint32_t _70;
    uint32_t _71;
    uint32_t _72;
    uint32_t _73;
    uint32_t _74;
    uint32_t _75;
    uint32_t _76;
    uint32_t _77;
    uint32_t _78;
    uint32_t _79;
    uint32_t _80;
    uint32_t _81;
    struct SubresourceRange* _82;
    struct SubresourceRange* _83;
    uint16_t _84;
    uint16_t _85;
    uint32_t _86;
    uint32_t _87;
    float _88;
    uint32_t _89;
    uint32_t _90;
    uint32_t _91;
    uint32_t _92;
    uint32_t _93;
    uint32_t _94;
    uint32_t _95;
    struct SubresourceRange* _96;
    struct SubresourceRange* _97;
    uint16_t _98;
    uint16_t _99;
    uint32_t _100;
    uint32_t _101;
    float _102;
    uint32_t _103;
    uint32_t _104;
    uint32_t _105;
    uint32_t _106;
    uint32_t _107;
    uint32_t _108;
    uint32_t _109;
    struct SubresourceRange* _110;
    struct SubresourceRange* _111;
    uint16_t _112;
    uint16_t _113;
    uint32_t _114;
    uint32_t _115;
    float _116;
    uint32_t _117;
    uint32_t _118;
    uint32_t _119;
    uint32_t _120;
    uint32_t _121;
    uint32_t _122;
    uint32_t _123;
    uint32_t inc_2e_i_2e_i_2e_50;
    uint32_t _124;
    uint32_t shr_2e_i_2e_i_2e_51;
    uint32_t _125;
    uint32_t shr12_2e_i_2e_i_2e_52;
    uint32_t _126;
    uint32_t shr13_2e_i_2e_i_2e_53;
    uint32_t _127;
    uint32_t UMin221;
    uint32_t _128;
    uint32_t call1_2e_i_2e_61;
    uint32_t _129;
    uint32_t _130;
    uint8_t* _131;
    uint32_t _132;
    uint32_t _133;
    uint32_t _134;
    uint32_t _135;
    uint32_t _136;
    uint32_t _137;
    uint32_t _138;
    uint32_t _139;
    uint32_t _140;
    uint32_t _141;
    uint32_t _142;
    uint32_t _143;
    uint32_t _144;
    uint32_t _145;
    uint32_t _146;
    struct SubresourceRange* _147;
    struct SubresourceRange* _148;
    uint16_t _149;
    uint16_t _150;
    uint32_t _151;
    uint32_t _152;
    float _153;
    uint32_t _154;
    uint32_t _155;
    uint32_t _156;
    uint32_t _157;
    uint32_t _158;
    uint32_t _159;
    uint32_t _160;
    struct SubresourceRange* _161;
    struct SubresourceRange* _162;
    uint16_t _163;
    uint16_t _164;
    uint32_t _165;
    uint32_t _166;
    float _167;
    uint32_t _168;
    uint32_t _169;
    uint32_t _170;
    uint32_t _171;
    uint32_t _172;
    uint32_t _173;
    uint32_t _174;
    struct SubresourceRange* _175;
    struct SubresourceRange* _176;
    uint16_t _177;
    uint16_t _178;
    uint32_t _179;
    uint32_t _180;
    float _181;
    uint32_t _182;
    uint32_t _183;
    uint32_t _184;
    uint32_t _185;
    uint32_t _186;
    uint32_t _187;
    uint32_t _188;
    uint32_t inc_2e_i_2e_i_2e_112;
    uint32_t _189;
    uint32_t shr_2e_i_2e_i_2e_113;
    uint32_t _190;
    uint32_t shr12_2e_i_2e_i_2e_114;
    uint32_t _191;
    uint32_t shr13_2e_i_2e_i_2e_115;
    uint32_t _192;
    uint32_t UMin220;
    uint32_t _193;
    uint32_t call1_2e_i_2e_123;
    uint32_t _194;
    uint32_t _195;
    uint8_t* _196;
    uint32_t _197;
    uint32_t _198;
    uint32_t _199;
    uint32_t _200;
    uint32_t _201;
    uint32_t _202;
    uint32_t _203;
    uint32_t _204;
    uint32_t _205;
    uint32_t _206;
    uint32_t _207;
    uint32_t _208;
    uint32_t _209;
    uint32_t _210;
    uint32_t _211;
    struct SubresourceRange* _212;
    struct SubresourceRange* _213;
    uint16_t _214;
    uint16_t _215;
    uint32_t _216;
    uint32_t _217;
    float _218;
    uint32_t _219;
    uint32_t _220;
    uint32_t _221;
    uint32_t _222;
    uint32_t _223;
    uint32_t _224;
    uint32_t _225;
    struct SubresourceRange* _226;
    struct SubresourceRange* _227;
    uint16_t _228;
    uint16_t _229;
    uint32_t _230;
    uint32_t _231;
    float _232;
    uint32_t _233;
    uint32_t _234;
    uint32_t _235;
    uint32_t _236;
    uint32_t _237;
    uint32_t _238;
    uint32_t _239;
    struct SubresourceRange* _240;
    struct SubresourceRange* _241;
    uint16_t _242;
    uint16_t _243;
    uint32_t _244;
    uint32_t _245;
    float _246;
    uint32_t _247;
    uint32_t _248;
    uint32_t _249;
    uint8_t** _250;
    __MSALIGN__(4) struct l_vector_4_float* _251;
    uint32_t _252;
    uint32_t _253;
    uint32_t _254;
    uint8_t** _255;
    uint32_t* _256;
    float* _257;
    uint32_t* _258;
    uint32_t _259;
    uint32_t _260;
    uint32_t _261;
    uint32_t _262;
    uint32_t _263;
    uint32_t _264;
    uint32_t _265;
    uint32_t _266;
    uint32_t inc_2e_i_2e_i_2e_174;
    uint32_t _267;
    uint32_t shr_2e_i_2e_i_2e_175;
    uint32_t _268;
    uint32_t shr12_2e_i_2e_i_2e_176;
    uint32_t _269;
    uint32_t shr13_2e_i_2e_i_2e_177;
    uint32_t _270;
    uint32_t UMin;
    uint32_t _271;
    uint32_t call1_2e_i_2e_185;
    uint32_t _272;
    uint32_t _273;
    uint8_t* _274;
    uint32_t _275;
    uint32_t _276;
    uint32_t _277;
    uint32_t _278;
    uint32_t _279;
    uint32_t _280;
    uint32_t _281;
    uint32_t _282;
    uint32_t _283;
    uint32_t _284;
    uint32_t _285;
    uint32_t _286;
    uint32_t _287;
    uint32_t _288;
    uint32_t _289;
    struct SubresourceRange* _290;
    struct SubresourceRange* _291;
    uint16_t _292;
    uint16_t _293;
    uint32_t _294;
    uint32_t _295;
    float _296;
    uint32_t _297;
    uint32_t _298;
    uint32_t _299;
    uint32_t _300;
    uint32_t _301;
    uint32_t _302;
    uint32_t _303;
    struct SubresourceRange* _304;
    struct SubresourceRange* _305;
    uint16_t _306;
    uint16_t _307;
    uint32_t _308;
    uint32_t _309;
    float _310;
    uint32_t _311;
    uint32_t _312;
    uint32_t _313;
    uint32_t _314;
    uint32_t _315;
    uint32_t _316;
    uint32_t _317;
    struct SubresourceRange* _318;
    struct SubresourceRange* _319;
    uint16_t _320;
    uint16_t _321;
    uint32_t _322;
    uint32_t _323;
    float _324;
    uint32_t _325;
    __MSALIGN__(4) struct l_vector_2_uint32_t _326;
    __MSALIGN__(4) struct l_vector_2_uint32_t _327;
    uint8_t** _328;
    __MSALIGN__(4) struct l_vector_2_uint32_t* _329;
    __MSALIGN__(4) struct l_vector_2_uint32_t* _330;
    __MSALIGN__(4) struct l_vector_2_uint32_t* _331;
    uint32_t* _332;
    uint32_t _333;
    uint8_t** _334;
    uint32_t _335;
    uint32_t _336;
  } _llvm_cbe_tmps;

  struct {
    uint32_t w_2e_i_2e_i_2e_0;
    uint32_t w_2e_i_2e_i_2e_0__PHI_TEMPORARY;
    uint32_t h_2e_i_2e_i_2e_0;
    uint32_t h_2e_i_2e_i_2e_0__PHI_TEMPORARY;
    uint32_t d_2e_i_2e_i_2e_0;
    uint32_t d_2e_i_2e_i_2e_0__PHI_TEMPORARY;
    uint32_t mips_2e_i_2e_i_2e_0;
    uint32_t mips_2e_i_2e_i_2e_0__PHI_TEMPORARY;
    bool _337;
    bool _337__PHI_TEMPORARY;
    uint32_t mips_2e_i_2e_i_2e_0_2e_lcssa;
    uint32_t mips_2e_i_2e_i_2e_0_2e_lcssa__PHI_TEMPORARY;
    uint32_t cond_2e_i_2e_i;
    uint32_t cond_2e_i_2e_i__PHI_TEMPORARY;
    uint32_t retval_2e_i_2e_i_2e_0;
    uint32_t retval_2e_i_2e_i_2e_0__PHI_TEMPORARY;
    uint32_t w_2e_i_2e_i_2e_26_2e_0;
    uint32_t w_2e_i_2e_i_2e_26_2e_0__PHI_TEMPORARY;
    uint32_t h_2e_i_2e_i_2e_27_2e_0;
    uint32_t h_2e_i_2e_i_2e_27_2e_0__PHI_TEMPORARY;
    uint32_t d_2e_i_2e_i_2e_28_2e_0;
    uint32_t d_2e_i_2e_i_2e_28_2e_0__PHI_TEMPORARY;
    uint32_t mips_2e_i_2e_i_2e_29_2e_0;
    uint32_t mips_2e_i_2e_i_2e_29_2e_0__PHI_TEMPORARY;
    bool _338;
    bool _338__PHI_TEMPORARY;
    uint32_t mips_2e_i_2e_i_2e_29_2e_0_2e_lcssa;
    uint32_t mips_2e_i_2e_i_2e_29_2e_0_2e_lcssa__PHI_TEMPORARY;
    uint32_t cond_2e_i_2e_i_2e_59;
    uint32_t cond_2e_i_2e_i_2e_59__PHI_TEMPORARY;
    uint32_t retval_2e_i_2e_i_2e_20_2e_0;
    uint32_t retval_2e_i_2e_i_2e_20_2e_0__PHI_TEMPORARY;
    uint32_t w_2e_i_2e_i_2e_88_2e_0;
    uint32_t w_2e_i_2e_i_2e_88_2e_0__PHI_TEMPORARY;
    uint32_t h_2e_i_2e_i_2e_89_2e_0;
    uint32_t h_2e_i_2e_i_2e_89_2e_0__PHI_TEMPORARY;
    uint32_t d_2e_i_2e_i_2e_90_2e_0;
    uint32_t d_2e_i_2e_i_2e_90_2e_0__PHI_TEMPORARY;
    uint32_t mips_2e_i_2e_i_2e_91_2e_0;
    uint32_t mips_2e_i_2e_i_2e_91_2e_0__PHI_TEMPORARY;
    bool _339;
    bool _339__PHI_TEMPORARY;
    uint32_t mips_2e_i_2e_i_2e_91_2e_0_2e_lcssa;
    uint32_t mips_2e_i_2e_i_2e_91_2e_0_2e_lcssa__PHI_TEMPORARY;
    uint32_t cond_2e_i_2e_i_2e_121;
    uint32_t cond_2e_i_2e_i_2e_121__PHI_TEMPORARY;
    uint32_t retval_2e_i_2e_i_2e_82_2e_0;
    uint32_t retval_2e_i_2e_i_2e_82_2e_0__PHI_TEMPORARY;
    uint32_t mips_2e_i_2e_i_2e_153_2e_0;
    uint32_t mips_2e_i_2e_i_2e_153_2e_0__PHI_TEMPORARY;
    uint32_t d_2e_i_2e_i_2e_152_2e_0;
    uint32_t d_2e_i_2e_i_2e_152_2e_0__PHI_TEMPORARY;
    uint32_t h_2e_i_2e_i_2e_151_2e_0;
    uint32_t h_2e_i_2e_i_2e_151_2e_0__PHI_TEMPORARY;
    uint32_t w_2e_i_2e_i_2e_150_2e_0;
    uint32_t w_2e_i_2e_i_2e_150_2e_0__PHI_TEMPORARY;
    bool _340;
    bool _340__PHI_TEMPORARY;
    uint32_t mips_2e_i_2e_i_2e_153_2e_0_2e_lcssa;
    uint32_t mips_2e_i_2e_i_2e_153_2e_0_2e_lcssa__PHI_TEMPORARY;
    uint32_t cond_2e_i_2e_i_2e_183;
    uint32_t cond_2e_i_2e_i_2e_183__PHI_TEMPORARY;
    uint32_t retval_2e_i_2e_i_2e_144_2e_0;
    uint32_t retval_2e_i_2e_i_2e_144_2e_0__PHI_TEMPORARY;
  } _llvm_cbe_phi_tmps = {0};

#line 13 "renderer.rpsl"
  ___rpsl_block_marker(0, 0, 4, 4, -1, 0, -1);
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._40 = *((&backbuffer->Resource));
#line 13 "renderer.rpsl"
  *((&_llvm_cbe_tmps._35.Resource)) = _llvm_cbe_tmps._40;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._41 = *((&backbuffer->Format));
#line 13 "renderer.rpsl"
  *((&_llvm_cbe_tmps._35.Format)) = _llvm_cbe_tmps._41;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._42 = *((&backbuffer->TemporalLayer));
#line 13 "renderer.rpsl"
  *((&_llvm_cbe_tmps._35.TemporalLayer)) = _llvm_cbe_tmps._42;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._43 = *((&backbuffer->Flags));
#line 13 "renderer.rpsl"
  *((&_llvm_cbe_tmps._35.Flags)) = _llvm_cbe_tmps._43;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._44 = (&_llvm_cbe_tmps._35.SubresourceRange);
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._45 = (&backbuffer->SubresourceRange);
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._46 = *((&_llvm_cbe_tmps._45->base_mip_level));
#line 13 "renderer.rpsl"
  *((&_llvm_cbe_tmps._44->base_mip_level)) = _llvm_cbe_tmps._46;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._47 = *((&_llvm_cbe_tmps._45->mip_level_count));
#line 13 "renderer.rpsl"
  *((&_llvm_cbe_tmps._44->mip_level_count)) = _llvm_cbe_tmps._47;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._48 = *((&_llvm_cbe_tmps._45->base_array_layer));
#line 13 "renderer.rpsl"
  *((&_llvm_cbe_tmps._44->base_array_layer)) = _llvm_cbe_tmps._48;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._49 = *((&_llvm_cbe_tmps._45->array_layer_count));
#line 13 "renderer.rpsl"
  *((&_llvm_cbe_tmps._44->array_layer_count)) = _llvm_cbe_tmps._49;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._50 = *((&backbuffer->MinLodClamp));
#line 13 "renderer.rpsl"
  *((&_llvm_cbe_tmps._35.MinLodClamp)) = _llvm_cbe_tmps._50;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._51 = *((&backbuffer->ComponentMapping));
#line 13 "renderer.rpsl"
  *((&_llvm_cbe_tmps._35.ComponentMapping)) = _llvm_cbe_tmps._51;
  #line 15 "renderer.rpsl"
  ___rpsl_describe_handle((((uint8_t*)(&backbuffer_desc))), 36, (((uint32_t*)(&_llvm_cbe_tmps._35))), 1);
#line 19 "renderer.rpsl"
  _llvm_cbe_tmps.IMax = ___rpsl_dxop_binary_i32(37, ((in_game_viewport_size).vector[UINT64_C(0)]), 1);
#line 19 "renderer.rpsl"
  _llvm_cbe_tmps.IMax219 = ___rpsl_dxop_binary_i32(37, ((in_game_viewport_size).vector[UINT64_C(1)]), 1);
#line 19 "renderer.rpsl"
  _llvm_cbe_tmps._52 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 21 "renderer.rpsl"
  _llvm_cbe_tmps._53 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 21 "renderer.rpsl"
  _llvm_cbe_tmps._54 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  goto if_2e_end_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"

if_2e_end_2e_i_2e_i:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._55 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._56 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._57 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._58 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.IMax;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.IMax219;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_0__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_0__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto while_2e_cond_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"

#line 228 "./___rpsl_builtin_header_.rpsl"
  do {     /* Syntactic loop 'while.cond.i.i' to make GCC happy */
while_2e_cond_2e_i_2e_i:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_0 = _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_0 = _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_0 = _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_0 = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_0__PHI_TEMPORARY;
  #line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  if ((((((uint32_t)_llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_0) > ((uint32_t)1u))&1))) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps._337__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_end_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_lhs_2e_false_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

lor_2e_lhs_2e_false_2e_i_2e_i:
#line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  if ((((((uint32_t)_llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_0) > ((uint32_t)1u))&1))) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps._337__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_end_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_rhs_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

lor_2e_rhs_2e_i_2e_i:
#line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps._337__PHI_TEMPORARY = (((((uint32_t)_llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_0) > ((uint32_t)1u))&1));   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto lor_2e_end_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"

lor_2e_end_2e_i_2e_i:
#line 21 "renderer.rpsl"
  _llvm_cbe_phi_tmps._337 = ((_llvm_cbe_phi_tmps._337__PHI_TEMPORARY)&1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  if (_llvm_cbe_phi_tmps._337) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto while_2e_body_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_0_2e_lcssa__PHI_TEMPORARY = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_0;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto while_2e_end_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

while_2e_body_2e_i_2e_i:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.inc_2e_i_2e_i = llvm_add_u32(_llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._59 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr_2e_i_2e_i = llvm_lshr_u32(_llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._60 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr12_2e_i_2e_i = llvm_lshr_u32(_llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._61 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr13_2e_i_2e_i = llvm_lshr_u32(_llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._62 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr_2e_i_2e_i;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr12_2e_i_2e_i;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr13_2e_i_2e_i;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.inc_2e_i_2e_i;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto while_2e_cond_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"

  } while (1); /* end of syntactic loop 'while.cond.i.i' */
while_2e_end_2e_i_2e_i:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_0_2e_lcssa = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_0_2e_lcssa__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto cond_2e_false_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"

cond_2e_false_2e_i_2e_i:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.UMin222 = ___rpsl_dxop_binary_i32(40, 1, _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_0_2e_lcssa);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond_2e_i_2e_i__PHI_TEMPORARY = _llvm_cbe_tmps.UMin222;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto cond_2e_end_2e_i_2e_i;
#line 228 "./___rpsl_builtin_header_.rpsl"

cond_2e_end_2e_i_2e_i:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond_2e_i_2e_i = _llvm_cbe_phi_tmps.cond_2e_i_2e_i__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_0__PHI_TEMPORARY = _llvm_cbe_phi_tmps.cond_2e_i_2e_i;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto _1__3f_create_tex2d_40__40_YA_3f_AUtexture_40__40_IIIIIIIII_40_Z_2e_exit;
#line 228 "./___rpsl_builtin_header_.rpsl"

_1__3f_create_tex2d_40__40_YA_3f_AUtexture_40__40_IIIIIIIII_40_Z_2e_exit:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_0 = _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._63 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 21 "renderer.rpsl"
  _llvm_cbe_tmps.call1_2e_i = ___rpsl_create_resource(3, 0, 10, _llvm_cbe_tmps.IMax, _llvm_cbe_tmps.IMax219, 1, _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_0, 1, 0, 1, 0);
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._64 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 21 "renderer.rpsl"
  _llvm_cbe_tmps._65 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 21 "renderer.rpsl"
  _llvm_cbe_tmps._66 = memset((((uint8_t*)(&_llvm_cbe_tmps._34))), 0, 36);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._67 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._34.Resource)) = _llvm_cbe_tmps.call1_2e_i;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._68 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._34.Format)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._69 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._34.TemporalLayer)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._70 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._34.Flags)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._71 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._34.SubresourceRange.base_mip_level)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._72 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._34.SubresourceRange.mip_level_count)) = (((uint16_t)_llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_0));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._73 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._34.SubresourceRange.base_array_layer)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._74 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._34.SubresourceRange.array_layer_count)) = 1;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._75 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._34.MinLodClamp)) = ((float)(0.000000e+00));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._76 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._34.ComponentMapping)) = 50462976;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._77 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._78 = *((&_llvm_cbe_tmps._34.Resource));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp.Resource)) = _llvm_cbe_tmps._78;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._79 = *((&_llvm_cbe_tmps._34.Format));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp.Format)) = _llvm_cbe_tmps._79;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._80 = *((&_llvm_cbe_tmps._34.TemporalLayer));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp.TemporalLayer)) = _llvm_cbe_tmps._80;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._81 = *((&_llvm_cbe_tmps._34.Flags));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp.Flags)) = _llvm_cbe_tmps._81;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._82 = (&agg_2e_tmp.SubresourceRange);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._83 = (&_llvm_cbe_tmps._34.SubresourceRange);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._84 = *((&_llvm_cbe_tmps._83->base_mip_level));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._82->base_mip_level)) = _llvm_cbe_tmps._84;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._85 = *((&_llvm_cbe_tmps._83->mip_level_count));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._82->mip_level_count)) = _llvm_cbe_tmps._85;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._86 = *((&_llvm_cbe_tmps._83->base_array_layer));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._82->base_array_layer)) = _llvm_cbe_tmps._86;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._87 = *((&_llvm_cbe_tmps._83->array_layer_count));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._82->array_layer_count)) = _llvm_cbe_tmps._87;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._88 = *((&_llvm_cbe_tmps._34.MinLodClamp));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp.MinLodClamp)) = _llvm_cbe_tmps._88;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._89 = *((&_llvm_cbe_tmps._34.ComponentMapping));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp.ComponentMapping)) = _llvm_cbe_tmps._89;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._90 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._91 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._92 = *((&agg_2e_tmp.Resource));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._39.Resource)) = _llvm_cbe_tmps._92;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._93 = *((&agg_2e_tmp.Format));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._39.Format)) = _llvm_cbe_tmps._93;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._94 = *((&agg_2e_tmp.TemporalLayer));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._39.TemporalLayer)) = _llvm_cbe_tmps._94;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._95 = *((&agg_2e_tmp.Flags));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._39.Flags)) = _llvm_cbe_tmps._95;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._96 = (&_llvm_cbe_tmps._39.SubresourceRange);
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._97 = (&agg_2e_tmp.SubresourceRange);
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._98 = *((&_llvm_cbe_tmps._97->base_mip_level));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._96->base_mip_level)) = _llvm_cbe_tmps._98;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._99 = *((&_llvm_cbe_tmps._97->mip_level_count));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._96->mip_level_count)) = _llvm_cbe_tmps._99;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._100 = *((&_llvm_cbe_tmps._97->base_array_layer));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._96->base_array_layer)) = _llvm_cbe_tmps._100;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._101 = *((&_llvm_cbe_tmps._97->array_layer_count));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._96->array_layer_count)) = _llvm_cbe_tmps._101;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._102 = *((&agg_2e_tmp.MinLodClamp));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._39.MinLodClamp)) = _llvm_cbe_tmps._102;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._103 = *((&agg_2e_tmp.ComponentMapping));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._39.ComponentMapping)) = _llvm_cbe_tmps._103;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._104 = *((&_llvm_cbe_tmps._39.Resource));
#line 21 "renderer.rpsl"
  ___rpsl_name_resource(_llvm_cbe_tmps._104, ((&_AE__AE_rps_Str0.array[((int32_t)0)])), 13);
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._105 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._106 = *((&_llvm_cbe_tmps._39.Resource));
#line 21 "renderer.rpsl"
  *((&game_color_rt.Resource)) = _llvm_cbe_tmps._106;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._107 = *((&_llvm_cbe_tmps._39.Format));
#line 21 "renderer.rpsl"
  *((&game_color_rt.Format)) = _llvm_cbe_tmps._107;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._108 = *((&_llvm_cbe_tmps._39.TemporalLayer));
#line 21 "renderer.rpsl"
  *((&game_color_rt.TemporalLayer)) = _llvm_cbe_tmps._108;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._109 = *((&_llvm_cbe_tmps._39.Flags));
#line 21 "renderer.rpsl"
  *((&game_color_rt.Flags)) = _llvm_cbe_tmps._109;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._110 = (&game_color_rt.SubresourceRange);
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._111 = (&_llvm_cbe_tmps._39.SubresourceRange);
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._112 = *((&_llvm_cbe_tmps._111->base_mip_level));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._110->base_mip_level)) = _llvm_cbe_tmps._112;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._113 = *((&_llvm_cbe_tmps._111->mip_level_count));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._110->mip_level_count)) = _llvm_cbe_tmps._113;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._114 = *((&_llvm_cbe_tmps._111->base_array_layer));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._110->base_array_layer)) = _llvm_cbe_tmps._114;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._115 = *((&_llvm_cbe_tmps._111->array_layer_count));
#line 21 "renderer.rpsl"
  *((&_llvm_cbe_tmps._110->array_layer_count)) = _llvm_cbe_tmps._115;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._116 = *((&_llvm_cbe_tmps._39.MinLodClamp));
#line 21 "renderer.rpsl"
  *((&game_color_rt.MinLodClamp)) = _llvm_cbe_tmps._116;
#line 21 "renderer.rpsl"
  _llvm_cbe_tmps._117 = *((&_llvm_cbe_tmps._39.ComponentMapping));
#line 21 "renderer.rpsl"
  *((&game_color_rt.ComponentMapping)) = _llvm_cbe_tmps._117;
  #line 24 "renderer.rpsl"
  _llvm_cbe_tmps._118 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 24 "renderer.rpsl"
  _llvm_cbe_tmps._119 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  if ((((((uint32_t)game_color_sample_count) > ((uint32_t)1u))&1))) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto if_2e_then_2e_i_2e_i_2e_41;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto if_2e_end_2e_i_2e_i_2e_42;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

if_2e_then_2e_i_2e_i_2e_41:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_20_2e_0__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto _1__3f_create_tex2d_40__40_YA_3f_AUtexture_40__40_IIIIIIIII_40_Z_2e_exit_2e_77;
#line 228 "./___rpsl_builtin_header_.rpsl"

if_2e_end_2e_i_2e_i_2e_42:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._120 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._121 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._122 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._123 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_26_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.IMax;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_27_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.IMax219;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_28_2e_0__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_29_2e_0__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto while_2e_cond_2e_i_2e_i_2e_44;
#line 228 "./___rpsl_builtin_header_.rpsl"

#line 228 "./___rpsl_builtin_header_.rpsl"
  do {     /* Syntactic loop 'while.cond.i.i.44' to make GCC happy */
while_2e_cond_2e_i_2e_i_2e_44:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_26_2e_0 = _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_26_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_27_2e_0 = _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_27_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_28_2e_0 = _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_28_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_29_2e_0 = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_29_2e_0__PHI_TEMPORARY;
  #line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  if ((((((uint32_t)_llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_26_2e_0) > ((uint32_t)1u))&1))) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps._338__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_end_2e_i_2e_i_2e_49;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_lhs_2e_false_2e_i_2e_i_2e_46;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

lor_2e_lhs_2e_false_2e_i_2e_i_2e_46:
#line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  if ((((((uint32_t)_llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_27_2e_0) > ((uint32_t)1u))&1))) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps._338__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_end_2e_i_2e_i_2e_49;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_rhs_2e_i_2e_i_2e_48;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

lor_2e_rhs_2e_i_2e_i_2e_48:
#line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps._338__PHI_TEMPORARY = (((((uint32_t)_llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_28_2e_0) > ((uint32_t)1u))&1));   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto lor_2e_end_2e_i_2e_i_2e_49;
#line 228 "./___rpsl_builtin_header_.rpsl"

lor_2e_end_2e_i_2e_i_2e_49:
#line 24 "renderer.rpsl"
  _llvm_cbe_phi_tmps._338 = ((_llvm_cbe_phi_tmps._338__PHI_TEMPORARY)&1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  if (_llvm_cbe_phi_tmps._338) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto while_2e_body_2e_i_2e_i_2e_54;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_29_2e_0_2e_lcssa__PHI_TEMPORARY = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_29_2e_0;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto while_2e_end_2e_i_2e_i_2e_56;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

while_2e_body_2e_i_2e_i_2e_54:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.inc_2e_i_2e_i_2e_50 = llvm_add_u32(_llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_29_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._124 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr_2e_i_2e_i_2e_51 = llvm_lshr_u32(_llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_26_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._125 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr12_2e_i_2e_i_2e_52 = llvm_lshr_u32(_llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_27_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._126 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr13_2e_i_2e_i_2e_53 = llvm_lshr_u32(_llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_28_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._127 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_26_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr_2e_i_2e_i_2e_51;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_27_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr12_2e_i_2e_i_2e_52;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_28_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr13_2e_i_2e_i_2e_53;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_29_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.inc_2e_i_2e_i_2e_50;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto while_2e_cond_2e_i_2e_i_2e_44;
#line 228 "./___rpsl_builtin_header_.rpsl"

  } while (1); /* end of syntactic loop 'while.cond.i.i.44' */
while_2e_end_2e_i_2e_i_2e_56:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_29_2e_0_2e_lcssa = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_29_2e_0_2e_lcssa__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto cond_2e_false_2e_i_2e_i_2e_58;
#line 228 "./___rpsl_builtin_header_.rpsl"

cond_2e_false_2e_i_2e_i_2e_58:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.UMin221 = ___rpsl_dxop_binary_i32(40, 2, _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_29_2e_0_2e_lcssa);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_59__PHI_TEMPORARY = _llvm_cbe_tmps.UMin221;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto cond_2e_end_2e_i_2e_i_2e_60;
#line 228 "./___rpsl_builtin_header_.rpsl"

cond_2e_end_2e_i_2e_i_2e_60:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_59 = _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_59__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_20_2e_0__PHI_TEMPORARY = _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_59;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto _1__3f_create_tex2d_40__40_YA_3f_AUtexture_40__40_IIIIIIIII_40_Z_2e_exit_2e_77;
#line 228 "./___rpsl_builtin_header_.rpsl"

_1__3f_create_tex2d_40__40_YA_3f_AUtexture_40__40_IIIIIIIII_40_Z_2e_exit_2e_77:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_20_2e_0 = _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_20_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._128 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 24 "renderer.rpsl"
  _llvm_cbe_tmps.call1_2e_i_2e_61 = ___rpsl_create_resource(3, 0, 10, _llvm_cbe_tmps.IMax, _llvm_cbe_tmps.IMax219, 1, _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_20_2e_0, game_color_sample_count, 0, 1, 1);
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._129 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 24 "renderer.rpsl"
  _llvm_cbe_tmps._130 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 24 "renderer.rpsl"
  _llvm_cbe_tmps._131 = memset((((uint8_t*)(&_llvm_cbe_tmps._33))), 0, 36);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._132 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._33.Resource)) = _llvm_cbe_tmps.call1_2e_i_2e_61;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._133 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._33.Format)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._134 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._33.TemporalLayer)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._135 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._33.Flags)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._136 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._33.SubresourceRange.base_mip_level)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._137 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._33.SubresourceRange.mip_level_count)) = (((uint16_t)_llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_20_2e_0));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._138 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._33.SubresourceRange.base_array_layer)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._139 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._33.SubresourceRange.array_layer_count)) = 1;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._140 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._33.MinLodClamp)) = ((float)(0.000000e+00));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._141 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._33.ComponentMapping)) = 50462976;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._142 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._143 = *((&_llvm_cbe_tmps._33.Resource));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp3.Resource)) = _llvm_cbe_tmps._143;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._144 = *((&_llvm_cbe_tmps._33.Format));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp3.Format)) = _llvm_cbe_tmps._144;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._145 = *((&_llvm_cbe_tmps._33.TemporalLayer));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp3.TemporalLayer)) = _llvm_cbe_tmps._145;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._146 = *((&_llvm_cbe_tmps._33.Flags));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp3.Flags)) = _llvm_cbe_tmps._146;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._147 = (&agg_2e_tmp3.SubresourceRange);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._148 = (&_llvm_cbe_tmps._33.SubresourceRange);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._149 = *((&_llvm_cbe_tmps._148->base_mip_level));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._147->base_mip_level)) = _llvm_cbe_tmps._149;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._150 = *((&_llvm_cbe_tmps._148->mip_level_count));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._147->mip_level_count)) = _llvm_cbe_tmps._150;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._151 = *((&_llvm_cbe_tmps._148->base_array_layer));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._147->base_array_layer)) = _llvm_cbe_tmps._151;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._152 = *((&_llvm_cbe_tmps._148->array_layer_count));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._147->array_layer_count)) = _llvm_cbe_tmps._152;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._153 = *((&_llvm_cbe_tmps._33.MinLodClamp));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp3.MinLodClamp)) = _llvm_cbe_tmps._153;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._154 = *((&_llvm_cbe_tmps._33.ComponentMapping));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp3.ComponentMapping)) = _llvm_cbe_tmps._154;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._155 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._156 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._157 = *((&agg_2e_tmp3.Resource));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._38.Resource)) = _llvm_cbe_tmps._157;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._158 = *((&agg_2e_tmp3.Format));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._38.Format)) = _llvm_cbe_tmps._158;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._159 = *((&agg_2e_tmp3.TemporalLayer));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._38.TemporalLayer)) = _llvm_cbe_tmps._159;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._160 = *((&agg_2e_tmp3.Flags));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._38.Flags)) = _llvm_cbe_tmps._160;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._161 = (&_llvm_cbe_tmps._38.SubresourceRange);
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._162 = (&agg_2e_tmp3.SubresourceRange);
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._163 = *((&_llvm_cbe_tmps._162->base_mip_level));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._161->base_mip_level)) = _llvm_cbe_tmps._163;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._164 = *((&_llvm_cbe_tmps._162->mip_level_count));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._161->mip_level_count)) = _llvm_cbe_tmps._164;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._165 = *((&_llvm_cbe_tmps._162->base_array_layer));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._161->base_array_layer)) = _llvm_cbe_tmps._165;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._166 = *((&_llvm_cbe_tmps._162->array_layer_count));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._161->array_layer_count)) = _llvm_cbe_tmps._166;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._167 = *((&agg_2e_tmp3.MinLodClamp));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._38.MinLodClamp)) = _llvm_cbe_tmps._167;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._168 = *((&agg_2e_tmp3.ComponentMapping));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._38.ComponentMapping)) = _llvm_cbe_tmps._168;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._169 = *((&_llvm_cbe_tmps._38.Resource));
#line 24 "renderer.rpsl"
  ___rpsl_name_resource(_llvm_cbe_tmps._169, ((&_AE__AE_rps_Str1.array[((int32_t)0)])), 18);
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._170 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._171 = *((&_llvm_cbe_tmps._38.Resource));
#line 24 "renderer.rpsl"
  *((&game_color_rt_msaa.Resource)) = _llvm_cbe_tmps._171;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._172 = *((&_llvm_cbe_tmps._38.Format));
#line 24 "renderer.rpsl"
  *((&game_color_rt_msaa.Format)) = _llvm_cbe_tmps._172;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._173 = *((&_llvm_cbe_tmps._38.TemporalLayer));
#line 24 "renderer.rpsl"
  *((&game_color_rt_msaa.TemporalLayer)) = _llvm_cbe_tmps._173;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._174 = *((&_llvm_cbe_tmps._38.Flags));
#line 24 "renderer.rpsl"
  *((&game_color_rt_msaa.Flags)) = _llvm_cbe_tmps._174;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._175 = (&game_color_rt_msaa.SubresourceRange);
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._176 = (&_llvm_cbe_tmps._38.SubresourceRange);
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._177 = *((&_llvm_cbe_tmps._176->base_mip_level));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._175->base_mip_level)) = _llvm_cbe_tmps._177;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._178 = *((&_llvm_cbe_tmps._176->mip_level_count));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._175->mip_level_count)) = _llvm_cbe_tmps._178;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._179 = *((&_llvm_cbe_tmps._176->base_array_layer));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._175->base_array_layer)) = _llvm_cbe_tmps._179;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._180 = *((&_llvm_cbe_tmps._176->array_layer_count));
#line 24 "renderer.rpsl"
  *((&_llvm_cbe_tmps._175->array_layer_count)) = _llvm_cbe_tmps._180;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._181 = *((&_llvm_cbe_tmps._38.MinLodClamp));
#line 24 "renderer.rpsl"
  *((&game_color_rt_msaa.MinLodClamp)) = _llvm_cbe_tmps._181;
#line 24 "renderer.rpsl"
  _llvm_cbe_tmps._182 = *((&_llvm_cbe_tmps._38.ComponentMapping));
#line 24 "renderer.rpsl"
  *((&game_color_rt_msaa.ComponentMapping)) = _llvm_cbe_tmps._182;
  #line 25 "renderer.rpsl"
  _llvm_cbe_tmps._183 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 25 "renderer.rpsl"
  _llvm_cbe_tmps._184 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  if ((((((uint32_t)game_color_sample_count) > ((uint32_t)1u))&1))) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto if_2e_then_2e_i_2e_i_2e_103;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto if_2e_end_2e_i_2e_i_2e_104;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

if_2e_then_2e_i_2e_i_2e_103:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_82_2e_0__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto _1__3f_create_tex2d_40__40_YA_3f_AUtexture_40__40_IIIIIIIII_40_Z_2e_exit_2e_139;
#line 228 "./___rpsl_builtin_header_.rpsl"

if_2e_end_2e_i_2e_i_2e_104:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._185 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._186 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._187 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._188 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_88_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.IMax;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_89_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.IMax219;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_90_2e_0__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_91_2e_0__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto while_2e_cond_2e_i_2e_i_2e_106;
#line 228 "./___rpsl_builtin_header_.rpsl"

#line 228 "./___rpsl_builtin_header_.rpsl"
  do {     /* Syntactic loop 'while.cond.i.i.106' to make GCC happy */
while_2e_cond_2e_i_2e_i_2e_106:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_88_2e_0 = _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_88_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_89_2e_0 = _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_89_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_90_2e_0 = _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_90_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_91_2e_0 = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_91_2e_0__PHI_TEMPORARY;
  #line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  if ((((((uint32_t)_llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_88_2e_0) > ((uint32_t)1u))&1))) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps._339__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_end_2e_i_2e_i_2e_111;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_lhs_2e_false_2e_i_2e_i_2e_108;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

lor_2e_lhs_2e_false_2e_i_2e_i_2e_108:
#line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  if ((((((uint32_t)_llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_89_2e_0) > ((uint32_t)1u))&1))) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps._339__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_end_2e_i_2e_i_2e_111;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_rhs_2e_i_2e_i_2e_110;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

lor_2e_rhs_2e_i_2e_i_2e_110:
#line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps._339__PHI_TEMPORARY = (((((uint32_t)_llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_90_2e_0) > ((uint32_t)1u))&1));   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto lor_2e_end_2e_i_2e_i_2e_111;
#line 228 "./___rpsl_builtin_header_.rpsl"

lor_2e_end_2e_i_2e_i_2e_111:
#line 25 "renderer.rpsl"
  _llvm_cbe_phi_tmps._339 = ((_llvm_cbe_phi_tmps._339__PHI_TEMPORARY)&1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  if (_llvm_cbe_phi_tmps._339) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto while_2e_body_2e_i_2e_i_2e_116;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_91_2e_0_2e_lcssa__PHI_TEMPORARY = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_91_2e_0;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto while_2e_end_2e_i_2e_i_2e_118;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

while_2e_body_2e_i_2e_i_2e_116:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.inc_2e_i_2e_i_2e_112 = llvm_add_u32(_llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_91_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._189 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr_2e_i_2e_i_2e_113 = llvm_lshr_u32(_llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_88_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._190 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr12_2e_i_2e_i_2e_114 = llvm_lshr_u32(_llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_89_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._191 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr13_2e_i_2e_i_2e_115 = llvm_lshr_u32(_llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_90_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._192 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_88_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr_2e_i_2e_i_2e_113;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_89_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr12_2e_i_2e_i_2e_114;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_90_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr13_2e_i_2e_i_2e_115;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_91_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.inc_2e_i_2e_i_2e_112;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto while_2e_cond_2e_i_2e_i_2e_106;
#line 228 "./___rpsl_builtin_header_.rpsl"

  } while (1); /* end of syntactic loop 'while.cond.i.i.106' */
while_2e_end_2e_i_2e_i_2e_118:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_91_2e_0_2e_lcssa = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_91_2e_0_2e_lcssa__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto cond_2e_false_2e_i_2e_i_2e_120;
#line 228 "./___rpsl_builtin_header_.rpsl"

cond_2e_false_2e_i_2e_i_2e_120:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.UMin220 = ___rpsl_dxop_binary_i32(40, 2, _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_91_2e_0_2e_lcssa);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_121__PHI_TEMPORARY = _llvm_cbe_tmps.UMin220;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto cond_2e_end_2e_i_2e_i_2e_122;
#line 228 "./___rpsl_builtin_header_.rpsl"

cond_2e_end_2e_i_2e_i_2e_122:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_121 = _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_121__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_82_2e_0__PHI_TEMPORARY = _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_121;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto _1__3f_create_tex2d_40__40_YA_3f_AUtexture_40__40_IIIIIIIII_40_Z_2e_exit_2e_139;
#line 228 "./___rpsl_builtin_header_.rpsl"

_1__3f_create_tex2d_40__40_YA_3f_AUtexture_40__40_IIIIIIIII_40_Z_2e_exit_2e_139:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_82_2e_0 = _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_82_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._193 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 25 "renderer.rpsl"
  _llvm_cbe_tmps.call1_2e_i_2e_123 = ___rpsl_create_resource(3, 0, 20, _llvm_cbe_tmps.IMax, _llvm_cbe_tmps.IMax219, 1, _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_82_2e_0, game_color_sample_count, 0, 1, 2);
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._194 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 25 "renderer.rpsl"
  _llvm_cbe_tmps._195 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 25 "renderer.rpsl"
  _llvm_cbe_tmps._196 = memset((((uint8_t*)(&_llvm_cbe_tmps._32))), 0, 36);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._197 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._32.Resource)) = _llvm_cbe_tmps.call1_2e_i_2e_123;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._198 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._32.Format)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._199 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._32.TemporalLayer)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._200 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._32.Flags)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._201 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._32.SubresourceRange.base_mip_level)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._202 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._32.SubresourceRange.mip_level_count)) = (((uint16_t)_llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_82_2e_0));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._203 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._32.SubresourceRange.base_array_layer)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._204 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._32.SubresourceRange.array_layer_count)) = 1;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._205 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._32.MinLodClamp)) = ((float)(0.000000e+00));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._206 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._32.ComponentMapping)) = 50462976;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._207 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._208 = *((&_llvm_cbe_tmps._32.Resource));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp6.Resource)) = _llvm_cbe_tmps._208;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._209 = *((&_llvm_cbe_tmps._32.Format));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp6.Format)) = _llvm_cbe_tmps._209;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._210 = *((&_llvm_cbe_tmps._32.TemporalLayer));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp6.TemporalLayer)) = _llvm_cbe_tmps._210;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._211 = *((&_llvm_cbe_tmps._32.Flags));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp6.Flags)) = _llvm_cbe_tmps._211;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._212 = (&agg_2e_tmp6.SubresourceRange);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._213 = (&_llvm_cbe_tmps._32.SubresourceRange);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._214 = *((&_llvm_cbe_tmps._213->base_mip_level));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._212->base_mip_level)) = _llvm_cbe_tmps._214;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._215 = *((&_llvm_cbe_tmps._213->mip_level_count));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._212->mip_level_count)) = _llvm_cbe_tmps._215;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._216 = *((&_llvm_cbe_tmps._213->base_array_layer));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._212->base_array_layer)) = _llvm_cbe_tmps._216;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._217 = *((&_llvm_cbe_tmps._213->array_layer_count));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._212->array_layer_count)) = _llvm_cbe_tmps._217;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._218 = *((&_llvm_cbe_tmps._32.MinLodClamp));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp6.MinLodClamp)) = _llvm_cbe_tmps._218;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._219 = *((&_llvm_cbe_tmps._32.ComponentMapping));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp6.ComponentMapping)) = _llvm_cbe_tmps._219;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._220 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._221 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._222 = *((&agg_2e_tmp6.Resource));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._37.Resource)) = _llvm_cbe_tmps._222;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._223 = *((&agg_2e_tmp6.Format));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._37.Format)) = _llvm_cbe_tmps._223;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._224 = *((&agg_2e_tmp6.TemporalLayer));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._37.TemporalLayer)) = _llvm_cbe_tmps._224;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._225 = *((&agg_2e_tmp6.Flags));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._37.Flags)) = _llvm_cbe_tmps._225;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._226 = (&_llvm_cbe_tmps._37.SubresourceRange);
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._227 = (&agg_2e_tmp6.SubresourceRange);
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._228 = *((&_llvm_cbe_tmps._227->base_mip_level));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._226->base_mip_level)) = _llvm_cbe_tmps._228;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._229 = *((&_llvm_cbe_tmps._227->mip_level_count));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._226->mip_level_count)) = _llvm_cbe_tmps._229;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._230 = *((&_llvm_cbe_tmps._227->base_array_layer));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._226->base_array_layer)) = _llvm_cbe_tmps._230;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._231 = *((&_llvm_cbe_tmps._227->array_layer_count));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._226->array_layer_count)) = _llvm_cbe_tmps._231;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._232 = *((&agg_2e_tmp6.MinLodClamp));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._37.MinLodClamp)) = _llvm_cbe_tmps._232;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._233 = *((&agg_2e_tmp6.ComponentMapping));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._37.ComponentMapping)) = _llvm_cbe_tmps._233;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._234 = *((&_llvm_cbe_tmps._37.Resource));
#line 25 "renderer.rpsl"
  ___rpsl_name_resource(_llvm_cbe_tmps._234, ((&_AE__AE_rps_Str2.array[((int32_t)0)])), 18);
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._235 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._236 = *((&_llvm_cbe_tmps._37.Resource));
#line 25 "renderer.rpsl"
  *((&game_depth_rt_msaa.Resource)) = _llvm_cbe_tmps._236;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._237 = *((&_llvm_cbe_tmps._37.Format));
#line 25 "renderer.rpsl"
  *((&game_depth_rt_msaa.Format)) = _llvm_cbe_tmps._237;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._238 = *((&_llvm_cbe_tmps._37.TemporalLayer));
#line 25 "renderer.rpsl"
  *((&game_depth_rt_msaa.TemporalLayer)) = _llvm_cbe_tmps._238;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._239 = *((&_llvm_cbe_tmps._37.Flags));
#line 25 "renderer.rpsl"
  *((&game_depth_rt_msaa.Flags)) = _llvm_cbe_tmps._239;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._240 = (&game_depth_rt_msaa.SubresourceRange);
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._241 = (&_llvm_cbe_tmps._37.SubresourceRange);
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._242 = *((&_llvm_cbe_tmps._241->base_mip_level));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._240->base_mip_level)) = _llvm_cbe_tmps._242;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._243 = *((&_llvm_cbe_tmps._241->mip_level_count));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._240->mip_level_count)) = _llvm_cbe_tmps._243;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._244 = *((&_llvm_cbe_tmps._241->base_array_layer));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._240->base_array_layer)) = _llvm_cbe_tmps._244;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._245 = *((&_llvm_cbe_tmps._241->array_layer_count));
#line 25 "renderer.rpsl"
  *((&_llvm_cbe_tmps._240->array_layer_count)) = _llvm_cbe_tmps._245;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._246 = *((&_llvm_cbe_tmps._37.MinLodClamp));
#line 25 "renderer.rpsl"
  *((&game_depth_rt_msaa.MinLodClamp)) = _llvm_cbe_tmps._246;
#line 25 "renderer.rpsl"
  _llvm_cbe_tmps._247 = *((&_llvm_cbe_tmps._37.ComponentMapping));
#line 25 "renderer.rpsl"
  *((&game_depth_rt_msaa.ComponentMapping)) = _llvm_cbe_tmps._247;
#line 26 "renderer.rpsl"
  _llvm_cbe_tmps._248 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 28 "renderer.rpsl"
  _llvm_cbe_tmps._249 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 28 "renderer.rpsl"
  _llvm_cbe_tmps._250 = (uint8_t**) alloca(sizeof(uint8_t*) * (2));
#line 28 "renderer.rpsl"
  *((&(*_llvm_cbe_tmps._250))) = (((uint8_t*)(&game_color_rt_msaa)));
#line 28 "renderer.rpsl"
  _llvm_cbe_tmps._251 = (__MSALIGN__(4) struct l_vector_4_float*) alloca(sizeof(__MSALIGN__(4) struct l_vector_4_float));
#line 28 "renderer.rpsl"
  *_llvm_cbe_tmps._251 = llvm_ctor_f32x4(((float)(3.300000e-01)), ((float)(3.300000e-01)), ((float)(3.300000e-01)), ((float)(1.000000e+00)));
#line 28 "renderer.rpsl"
  *((&_llvm_cbe_tmps._250[((int32_t)1)])) = (((uint8_t*)_llvm_cbe_tmps._251));
#line 28 "renderer.rpsl"
  _llvm_cbe_tmps._252 = ___rpsl_node_call(0, 2, _llvm_cbe_tmps._250, 0, 0);
#line 28 "renderer.rpsl"
  _llvm_cbe_tmps._253 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 30 "renderer.rpsl"
  _llvm_cbe_tmps._254 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 30 "renderer.rpsl"
  _llvm_cbe_tmps._255 = (uint8_t**) alloca(sizeof(uint8_t*) * (4));
#line 30 "renderer.rpsl"
  *((&(*_llvm_cbe_tmps._255))) = (((uint8_t*)(&game_depth_rt_msaa)));
#line 30 "renderer.rpsl"
  _llvm_cbe_tmps._256 = (uint32_t*) alloca(sizeof(uint32_t));
#line 30 "renderer.rpsl"
  *_llvm_cbe_tmps._256 = 6;
#line 30 "renderer.rpsl"
  *((&_llvm_cbe_tmps._255[((int32_t)1)])) = (((uint8_t*)_llvm_cbe_tmps._256));
#line 30 "renderer.rpsl"
  _llvm_cbe_tmps._257 = (float*) alloca(sizeof(float));
#line 30 "renderer.rpsl"
  *_llvm_cbe_tmps._257 = ((float)(0.000000e+00));
#line 30 "renderer.rpsl"
  *((&_llvm_cbe_tmps._255[((int32_t)2)])) = (((uint8_t*)_llvm_cbe_tmps._257));
#line 30 "renderer.rpsl"
  _llvm_cbe_tmps._258 = (uint32_t*) alloca(sizeof(uint32_t));
#line 30 "renderer.rpsl"
  *_llvm_cbe_tmps._258 = 0;
#line 30 "renderer.rpsl"
  *((&_llvm_cbe_tmps._255[((int32_t)3)])) = (((uint8_t*)_llvm_cbe_tmps._258));
#line 30 "renderer.rpsl"
  _llvm_cbe_tmps._259 = ___rpsl_node_call(1, 4, _llvm_cbe_tmps._255, 0, 1);
#line 30 "renderer.rpsl"
  _llvm_cbe_tmps._260 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 34 "renderer.rpsl"
  _llvm_cbe_tmps._261 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 34 "renderer.rpsl"
  _llvm_cbe_tmps._262 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  goto if_2e_end_2e_i_2e_i_2e_166;
#line 228 "./___rpsl_builtin_header_.rpsl"

if_2e_end_2e_i_2e_i_2e_166:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._263 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._264 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._265 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._266 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_153_2e_0__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_152_2e_0__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_151_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.IMax219;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_150_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.IMax;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto while_2e_cond_2e_i_2e_i_2e_168;
#line 228 "./___rpsl_builtin_header_.rpsl"

#line 228 "./___rpsl_builtin_header_.rpsl"
  do {     /* Syntactic loop 'while.cond.i.i.168' to make GCC happy */
while_2e_cond_2e_i_2e_i_2e_168:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_153_2e_0 = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_153_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_152_2e_0 = _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_152_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_151_2e_0 = _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_151_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_150_2e_0 = _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_150_2e_0__PHI_TEMPORARY;
  #line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  if ((((((uint32_t)_llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_150_2e_0) > ((uint32_t)1u))&1))) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps._340__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_end_2e_i_2e_i_2e_173;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_lhs_2e_false_2e_i_2e_i_2e_170;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

lor_2e_lhs_2e_false_2e_i_2e_i_2e_170:
#line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  if ((((((uint32_t)_llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_151_2e_0) > ((uint32_t)1u))&1))) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps._340__PHI_TEMPORARY = 1;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_end_2e_i_2e_i_2e_173;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto lor_2e_rhs_2e_i_2e_i_2e_172;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

lor_2e_rhs_2e_i_2e_i_2e_172:
#line 228 "./___rpsl_builtin_header_.rpsl"
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps._340__PHI_TEMPORARY = (((((uint32_t)_llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_152_2e_0) > ((uint32_t)1u))&1));   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto lor_2e_end_2e_i_2e_i_2e_173;
#line 228 "./___rpsl_builtin_header_.rpsl"

lor_2e_end_2e_i_2e_i_2e_173:
#line 34 "renderer.rpsl"
  _llvm_cbe_phi_tmps._340 = ((_llvm_cbe_phi_tmps._340__PHI_TEMPORARY)&1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  if (_llvm_cbe_phi_tmps._340) {
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto while_2e_body_2e_i_2e_i_2e_178;
#line 228 "./___rpsl_builtin_header_.rpsl"
  } else {
#line 228 "./___rpsl_builtin_header_.rpsl"
    _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_153_2e_0_2e_lcssa__PHI_TEMPORARY = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_153_2e_0;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
    goto while_2e_end_2e_i_2e_i_2e_180;
#line 228 "./___rpsl_builtin_header_.rpsl"
  }

while_2e_body_2e_i_2e_i_2e_178:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.inc_2e_i_2e_i_2e_174 = llvm_add_u32(_llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_153_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._267 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr_2e_i_2e_i_2e_175 = llvm_lshr_u32(_llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_150_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._268 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr12_2e_i_2e_i_2e_176 = llvm_lshr_u32(_llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_151_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._269 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.shr13_2e_i_2e_i_2e_177 = llvm_lshr_u32(_llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_152_2e_0, 1);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._270 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_153_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.inc_2e_i_2e_i_2e_174;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.d_2e_i_2e_i_2e_152_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr13_2e_i_2e_i_2e_177;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.h_2e_i_2e_i_2e_151_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr12_2e_i_2e_i_2e_176;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.w_2e_i_2e_i_2e_150_2e_0__PHI_TEMPORARY = _llvm_cbe_tmps.shr_2e_i_2e_i_2e_175;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto while_2e_cond_2e_i_2e_i_2e_168;
#line 228 "./___rpsl_builtin_header_.rpsl"

  } while (1); /* end of syntactic loop 'while.cond.i.i.168' */
while_2e_end_2e_i_2e_i_2e_180:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_153_2e_0_2e_lcssa = _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_153_2e_0_2e_lcssa__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto cond_2e_false_2e_i_2e_i_2e_182;
#line 228 "./___rpsl_builtin_header_.rpsl"

cond_2e_false_2e_i_2e_i_2e_182:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps.UMin = ___rpsl_dxop_binary_i32(40, 2, _llvm_cbe_phi_tmps.mips_2e_i_2e_i_2e_153_2e_0_2e_lcssa);
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_183__PHI_TEMPORARY = _llvm_cbe_tmps.UMin;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto cond_2e_end_2e_i_2e_i_2e_184;
#line 228 "./___rpsl_builtin_header_.rpsl"

cond_2e_end_2e_i_2e_i_2e_184:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_183 = _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_183__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_144_2e_0__PHI_TEMPORARY = _llvm_cbe_phi_tmps.cond_2e_i_2e_i_2e_183;   /* for PHI node */
#line 228 "./___rpsl_builtin_header_.rpsl"
  goto _1__3f_create_tex2d_40__40_YA_3f_AUtexture_40__40_IIIIIIIII_40_Z_2e_exit_2e_201;
#line 228 "./___rpsl_builtin_header_.rpsl"

_1__3f_create_tex2d_40__40_YA_3f_AUtexture_40__40_IIIIIIIII_40_Z_2e_exit_2e_201:
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_144_2e_0 = _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_144_2e_0__PHI_TEMPORARY;
#line 228 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._271 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 34 "renderer.rpsl"
  _llvm_cbe_tmps.call1_2e_i_2e_185 = ___rpsl_create_resource(3, 0, 10, _llvm_cbe_tmps.IMax, _llvm_cbe_tmps.IMax219, 1, _llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_144_2e_0, 1, 0, 1, 3);
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._272 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 34 "renderer.rpsl"
  _llvm_cbe_tmps._273 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 34 "renderer.rpsl"
  _llvm_cbe_tmps._274 = memset((((uint8_t*)(&_llvm_cbe_tmps._31))), 0, 36);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._275 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._31.Resource)) = _llvm_cbe_tmps.call1_2e_i_2e_185;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._276 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._31.Format)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._277 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._31.TemporalLayer)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._278 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._31.Flags)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._279 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._31.SubresourceRange.base_mip_level)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._280 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._31.SubresourceRange.mip_level_count)) = (((uint16_t)_llvm_cbe_phi_tmps.retval_2e_i_2e_i_2e_144_2e_0));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._281 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._31.SubresourceRange.base_array_layer)) = 0;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._282 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._31.SubresourceRange.array_layer_count)) = 1;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._283 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._31.MinLodClamp)) = ((float)(0.000000e+00));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._284 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._31.ComponentMapping)) = 50462976;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._285 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._286 = *((&_llvm_cbe_tmps._31.Resource));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp13.Resource)) = _llvm_cbe_tmps._286;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._287 = *((&_llvm_cbe_tmps._31.Format));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp13.Format)) = _llvm_cbe_tmps._287;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._288 = *((&_llvm_cbe_tmps._31.TemporalLayer));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp13.TemporalLayer)) = _llvm_cbe_tmps._288;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._289 = *((&_llvm_cbe_tmps._31.Flags));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp13.Flags)) = _llvm_cbe_tmps._289;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._290 = (&agg_2e_tmp13.SubresourceRange);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._291 = (&_llvm_cbe_tmps._31.SubresourceRange);
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._292 = *((&_llvm_cbe_tmps._291->base_mip_level));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._290->base_mip_level)) = _llvm_cbe_tmps._292;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._293 = *((&_llvm_cbe_tmps._291->mip_level_count));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._290->mip_level_count)) = _llvm_cbe_tmps._293;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._294 = *((&_llvm_cbe_tmps._291->base_array_layer));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._290->base_array_layer)) = _llvm_cbe_tmps._294;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._295 = *((&_llvm_cbe_tmps._291->array_layer_count));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&_llvm_cbe_tmps._290->array_layer_count)) = _llvm_cbe_tmps._295;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._296 = *((&_llvm_cbe_tmps._31.MinLodClamp));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp13.MinLodClamp)) = _llvm_cbe_tmps._296;
#line 232 "./___rpsl_builtin_header_.rpsl"
  _llvm_cbe_tmps._297 = *((&_llvm_cbe_tmps._31.ComponentMapping));
#line 232 "./___rpsl_builtin_header_.rpsl"
  *((&agg_2e_tmp13.ComponentMapping)) = _llvm_cbe_tmps._297;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._298 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._299 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._300 = *((&agg_2e_tmp13.Resource));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._36.Resource)) = _llvm_cbe_tmps._300;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._301 = *((&agg_2e_tmp13.Format));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._36.Format)) = _llvm_cbe_tmps._301;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._302 = *((&agg_2e_tmp13.TemporalLayer));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._36.TemporalLayer)) = _llvm_cbe_tmps._302;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._303 = *((&agg_2e_tmp13.Flags));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._36.Flags)) = _llvm_cbe_tmps._303;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._304 = (&_llvm_cbe_tmps._36.SubresourceRange);
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._305 = (&agg_2e_tmp13.SubresourceRange);
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._306 = *((&_llvm_cbe_tmps._305->base_mip_level));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._304->base_mip_level)) = _llvm_cbe_tmps._306;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._307 = *((&_llvm_cbe_tmps._305->mip_level_count));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._304->mip_level_count)) = _llvm_cbe_tmps._307;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._308 = *((&_llvm_cbe_tmps._305->base_array_layer));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._304->base_array_layer)) = _llvm_cbe_tmps._308;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._309 = *((&_llvm_cbe_tmps._305->array_layer_count));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._304->array_layer_count)) = _llvm_cbe_tmps._309;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._310 = *((&agg_2e_tmp13.MinLodClamp));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._36.MinLodClamp)) = _llvm_cbe_tmps._310;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._311 = *((&agg_2e_tmp13.ComponentMapping));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._36.ComponentMapping)) = _llvm_cbe_tmps._311;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._312 = *((&_llvm_cbe_tmps._36.Resource));
#line 34 "renderer.rpsl"
  ___rpsl_name_resource(_llvm_cbe_tmps._312, ((&_AE__AE_rps_Str3.array[((int32_t)0)])), 29);
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._313 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._314 = *((&_llvm_cbe_tmps._36.Resource));
#line 34 "renderer.rpsl"
  *((&game_color_rt_pre_display_map.Resource)) = _llvm_cbe_tmps._314;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._315 = *((&_llvm_cbe_tmps._36.Format));
#line 34 "renderer.rpsl"
  *((&game_color_rt_pre_display_map.Format)) = _llvm_cbe_tmps._315;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._316 = *((&_llvm_cbe_tmps._36.TemporalLayer));
#line 34 "renderer.rpsl"
  *((&game_color_rt_pre_display_map.TemporalLayer)) = _llvm_cbe_tmps._316;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._317 = *((&_llvm_cbe_tmps._36.Flags));
#line 34 "renderer.rpsl"
  *((&game_color_rt_pre_display_map.Flags)) = _llvm_cbe_tmps._317;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._318 = (&game_color_rt_pre_display_map.SubresourceRange);
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._319 = (&_llvm_cbe_tmps._36.SubresourceRange);
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._320 = *((&_llvm_cbe_tmps._319->base_mip_level));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._318->base_mip_level)) = _llvm_cbe_tmps._320;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._321 = *((&_llvm_cbe_tmps._319->mip_level_count));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._318->mip_level_count)) = _llvm_cbe_tmps._321;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._322 = *((&_llvm_cbe_tmps._319->base_array_layer));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._318->base_array_layer)) = _llvm_cbe_tmps._322;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._323 = *((&_llvm_cbe_tmps._319->array_layer_count));
#line 34 "renderer.rpsl"
  *((&_llvm_cbe_tmps._318->array_layer_count)) = _llvm_cbe_tmps._323;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._324 = *((&_llvm_cbe_tmps._36.MinLodClamp));
#line 34 "renderer.rpsl"
  *((&game_color_rt_pre_display_map.MinLodClamp)) = _llvm_cbe_tmps._324;
#line 34 "renderer.rpsl"
  _llvm_cbe_tmps._325 = *((&_llvm_cbe_tmps._36.ComponentMapping));
#line 34 "renderer.rpsl"
  *((&game_color_rt_pre_display_map.ComponentMapping)) = _llvm_cbe_tmps._325;
#line 35 "renderer.rpsl"
  _llvm_cbe_tmps._326 = /*undef*/llvm_ctor_u32x2(0, 0);
#line 35 "renderer.rpsl"
  _llvm_cbe_tmps._326.vector[UINT64_C(0)] = _llvm_cbe_tmps.IMax;
#line 35 "renderer.rpsl"
  _llvm_cbe_tmps._327 = _llvm_cbe_tmps._326;
#line 35 "renderer.rpsl"
  _llvm_cbe_tmps._327.vector[UINT64_C(1)] = _llvm_cbe_tmps.IMax219;
#line 35 "renderer.rpsl"
  _llvm_cbe_tmps._328 = (uint8_t**) alloca(sizeof(uint8_t*) * (6));
#line 35 "renderer.rpsl"
  *((&(*_llvm_cbe_tmps._328))) = (((uint8_t*)(&game_color_rt_pre_display_map)));
#line 35 "renderer.rpsl"
  _llvm_cbe_tmps._329 = (__MSALIGN__(4) struct l_vector_2_uint32_t*) alloca(sizeof(__MSALIGN__(4) struct l_vector_2_uint32_t));
#line 35 "renderer.rpsl"
  *_llvm_cbe_tmps._329 = llvm_ctor_u32x2(0, 0);
#line 35 "renderer.rpsl"
  *((&_llvm_cbe_tmps._328[((int32_t)1)])) = (((uint8_t*)_llvm_cbe_tmps._329));
#line 35 "renderer.rpsl"
  *((&_llvm_cbe_tmps._328[((int32_t)2)])) = (((uint8_t*)(&game_color_rt_msaa)));
#line 35 "renderer.rpsl"
  _llvm_cbe_tmps._330 = (__MSALIGN__(4) struct l_vector_2_uint32_t*) alloca(sizeof(__MSALIGN__(4) struct l_vector_2_uint32_t));
#line 35 "renderer.rpsl"
  *_llvm_cbe_tmps._330 = llvm_ctor_u32x2(0, 0);
#line 35 "renderer.rpsl"
  *((&_llvm_cbe_tmps._328[((int32_t)3)])) = (((uint8_t*)_llvm_cbe_tmps._330));
#line 35 "renderer.rpsl"
  _llvm_cbe_tmps._331 = (__MSALIGN__(4) struct l_vector_2_uint32_t*) alloca(sizeof(__MSALIGN__(4) struct l_vector_2_uint32_t));
#line 35 "renderer.rpsl"
  *_llvm_cbe_tmps._331 = _llvm_cbe_tmps._327;
#line 35 "renderer.rpsl"
  *((&_llvm_cbe_tmps._328[((int32_t)4)])) = (((uint8_t*)_llvm_cbe_tmps._331));
#line 35 "renderer.rpsl"
  _llvm_cbe_tmps._332 = (uint32_t*) alloca(sizeof(uint32_t));
#line 35 "renderer.rpsl"
  *_llvm_cbe_tmps._332 = 0;
#line 35 "renderer.rpsl"
  *((&_llvm_cbe_tmps._328[((int32_t)5)])) = (((uint8_t*)_llvm_cbe_tmps._332));
#line 35 "renderer.rpsl"
  _llvm_cbe_tmps._333 = ___rpsl_node_call(2, 6, _llvm_cbe_tmps._328, 0, 2);
#line 37 "renderer.rpsl"
  _llvm_cbe_tmps._334 = (uint8_t**) alloca(sizeof(uint8_t*) * (2));
#line 37 "renderer.rpsl"
  *((&(*_llvm_cbe_tmps._334))) = (((uint8_t*)(&game_color_rt)));
#line 37 "renderer.rpsl"
  *((&_llvm_cbe_tmps._334[((int32_t)1)])) = (((uint8_t*)(&game_color_rt_pre_display_map)));
#line 37 "renderer.rpsl"
  _llvm_cbe_tmps._335 = ___rpsl_node_call(3, 2, _llvm_cbe_tmps._334, 0, 3);
#line 64 "renderer.rpsl"
  _llvm_cbe_tmps._336 = *((&dx_OC_nothing_OC_a.array[((int32_t)0)]));
  #line 64 "renderer.rpsl"
}


#line 13 "renderer.rpsl"
void rpsl_M_renderer_Fn_main_wrapper(uint32_t llvm_cbe_temp__341, uint8_t** llvm_cbe_temp__342, uint32_t llvm_cbe_temp__343) {

  struct {
    uint8_t* _344;
    struct ResourceDesc* _345;
    struct texture* _346;
    uint32_t* _347;
    struct texture* _348;
    uint32_t _349;
    uint8_t* _350;
    struct ResourceDesc* _351;
    struct texture* _352;
    uint32_t* _353;
    struct texture* _354;
    uint32_t _355;
    uint8_t* _356;
    bool _357;
    uint8_t* _358;
    __MSALIGN__(4) struct l_vector_2_uint32_t _359;
    uint8_t* _360;
    struct l_struct_class_OC_matrix_OC_float_OC_4_OC_4 _361;
    uint8_t* _362;
    uint32_t _363;
  } _llvm_cbe_tmps;

  struct {
    struct texture* _364;
    struct texture* _364__PHI_TEMPORARY;
    struct texture* _365;
    struct texture* _365__PHI_TEMPORARY;
  } _llvm_cbe_phi_tmps = {0};

#line 13 "renderer.rpsl"
#line 13 "renderer.rpsl"
  if ((((llvm_cbe_temp__341 == 6u)&1))) {
#line 13 "renderer.rpsl"
    goto trunk;
#line 13 "renderer.rpsl"
  } else {
#line 13 "renderer.rpsl"
    goto err;
#line 13 "renderer.rpsl"
  }

trunk:
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._344 = *((&(*llvm_cbe_temp__342)));
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._345 = ((struct ResourceDesc*)_llvm_cbe_tmps._344);
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._346 = (struct texture*) alloca(sizeof(struct texture));
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._347 = (uint32_t*) alloca(sizeof(uint32_t));
#line 13 "renderer.rpsl"
  *_llvm_cbe_tmps._347 = 0;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._348 = ((struct texture*)_llvm_cbe_tmps._344);
#line 13 "renderer.rpsl"
  if ((((((bool)(llvm_cbe_temp__343 & 1)&1u))&1))) {
#line 13 "renderer.rpsl"
    _llvm_cbe_phi_tmps._364__PHI_TEMPORARY = _llvm_cbe_tmps._348;   /* for PHI node */
#line 13 "renderer.rpsl"
    goto llvm_cbe_temp__366;
#line 13 "renderer.rpsl"
  } else {
#line 13 "renderer.rpsl"
    goto _2e_preheader_2e_1;
#line 13 "renderer.rpsl"
  }

_2e_preheader_2e_1:
#line 13 "renderer.rpsl"
  goto llvm_cbe_temp__367;
#line 13 "renderer.rpsl"

err:
#line 13 "renderer.rpsl"
  ___rpsl_abort(-3);
#line 13 "renderer.rpsl"
  return;
#line 13 "renderer.rpsl"
  do {     /* Syntactic loop '' to make GCC happy */
llvm_cbe_temp__367:
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._349 = *_llvm_cbe_tmps._347;
#line 13 "renderer.rpsl"
#line 13 "renderer.rpsl"
  if ((((((int32_t)_llvm_cbe_tmps._349) < ((int32_t)1u))&1))) {
#line 13 "renderer.rpsl"
    goto llvm_cbe_temp__368;
#line 13 "renderer.rpsl"
  } else {
#line 13 "renderer.rpsl"
    goto _2e_loopexit_2e_2;
#line 13 "renderer.rpsl"
  }

llvm_cbe_temp__368:
#line 13 "renderer.rpsl"
  *(((&_llvm_cbe_tmps._346[((int32_t)_llvm_cbe_tmps._349)]))) = _BA__PD_make_default_texture_view_from_desc_AE__AE_YA_PD_AUtexture_AE__AE_IUResourceDesc_AE__AE__AE_Z((llvm_add_u32(_llvm_cbe_tmps._349, 0)), ((&_llvm_cbe_tmps._345[((int32_t)_llvm_cbe_tmps._349)])));
#line 13 "renderer.rpsl"
  *_llvm_cbe_tmps._347 = (llvm_add_u32(_llvm_cbe_tmps._349, 1));
#line 13 "renderer.rpsl"
  goto llvm_cbe_temp__367;
#line 13 "renderer.rpsl"

  } while (1); /* end of syntactic loop '' */
_2e_loopexit_2e_2:
#line 13 "renderer.rpsl"
  _llvm_cbe_phi_tmps._364__PHI_TEMPORARY = _llvm_cbe_tmps._346;   /* for PHI node */
#line 13 "renderer.rpsl"
  goto llvm_cbe_temp__366;
#line 13 "renderer.rpsl"

llvm_cbe_temp__366:
#line 13 "renderer.rpsl"
  _llvm_cbe_phi_tmps._364 = _llvm_cbe_phi_tmps._364__PHI_TEMPORARY;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._350 = *((&llvm_cbe_temp__342[((int32_t)1)]));
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._351 = ((struct ResourceDesc*)_llvm_cbe_tmps._350);
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._352 = (struct texture*) alloca(sizeof(struct texture));
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._353 = (uint32_t*) alloca(sizeof(uint32_t));
#line 13 "renderer.rpsl"
  *_llvm_cbe_tmps._353 = 0;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._354 = ((struct texture*)_llvm_cbe_tmps._350);
#line 13 "renderer.rpsl"
  if ((((((bool)(llvm_cbe_temp__343 & 1)&1u))&1))) {
#line 13 "renderer.rpsl"
    _llvm_cbe_phi_tmps._365__PHI_TEMPORARY = _llvm_cbe_tmps._354;   /* for PHI node */
#line 13 "renderer.rpsl"
    goto llvm_cbe_temp__369;
#line 13 "renderer.rpsl"
  } else {
#line 13 "renderer.rpsl"
    goto _2e_preheader;
#line 13 "renderer.rpsl"
  }

_2e_preheader:
#line 13 "renderer.rpsl"
  goto llvm_cbe_temp__370;
#line 13 "renderer.rpsl"

#line 13 "renderer.rpsl"
  do {     /* Syntactic loop '' to make GCC happy */
llvm_cbe_temp__370:
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._355 = *_llvm_cbe_tmps._353;
#line 13 "renderer.rpsl"
#line 13 "renderer.rpsl"
  if ((((((int32_t)_llvm_cbe_tmps._355) < ((int32_t)1u))&1))) {
#line 13 "renderer.rpsl"
    goto llvm_cbe_temp__371;
#line 13 "renderer.rpsl"
  } else {
#line 13 "renderer.rpsl"
    goto _2e_loopexit;
#line 13 "renderer.rpsl"
  }

llvm_cbe_temp__371:
#line 13 "renderer.rpsl"
  *(((&_llvm_cbe_tmps._352[((int32_t)_llvm_cbe_tmps._355)]))) = _BA__PD_make_default_texture_view_from_desc_AE__AE_YA_PD_AUtexture_AE__AE_IUResourceDesc_AE__AE__AE_Z((llvm_add_u32(_llvm_cbe_tmps._355, 1)), ((&_llvm_cbe_tmps._351[((int32_t)_llvm_cbe_tmps._355)])));
#line 13 "renderer.rpsl"
  *_llvm_cbe_tmps._353 = (llvm_add_u32(_llvm_cbe_tmps._355, 1));
#line 13 "renderer.rpsl"
  goto llvm_cbe_temp__370;
#line 13 "renderer.rpsl"

  } while (1); /* end of syntactic loop '' */
_2e_loopexit:
#line 13 "renderer.rpsl"
  _llvm_cbe_phi_tmps._365__PHI_TEMPORARY = _llvm_cbe_tmps._352;   /* for PHI node */
#line 13 "renderer.rpsl"
  goto llvm_cbe_temp__369;
#line 13 "renderer.rpsl"

llvm_cbe_temp__369:
#line 13 "renderer.rpsl"
  _llvm_cbe_phi_tmps._365 = _llvm_cbe_phi_tmps._365__PHI_TEMPORARY;
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._356 = *((&llvm_cbe_temp__342[((int32_t)2)]));
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._357 = ((*(((bool*)_llvm_cbe_tmps._356)))&1);
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._358 = *((&llvm_cbe_temp__342[((int32_t)3)]));
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._359 = *(((__MSALIGN__(4) struct l_vector_2_uint32_t*)_llvm_cbe_tmps._358));
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._360 = *((&llvm_cbe_temp__342[((int32_t)4)]));
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._361 = *(((struct l_struct_class_OC_matrix_OC_float_OC_4_OC_4*)_llvm_cbe_tmps._360));
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._362 = *((&llvm_cbe_temp__342[((int32_t)5)]));
#line 13 "renderer.rpsl"
  _llvm_cbe_tmps._363 = *(((uint32_t*)_llvm_cbe_tmps._362));
#line 13 "renderer.rpsl"
  rpsl_M_renderer_Fn_main(_llvm_cbe_phi_tmps._364, _llvm_cbe_phi_tmps._365, _llvm_cbe_tmps._357, _llvm_cbe_tmps._359, _llvm_cbe_tmps._361, _llvm_cbe_tmps._363);
#line 13 "renderer.rpsl"
}

