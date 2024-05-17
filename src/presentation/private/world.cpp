#include "world.h"

#include <shared/std.h>
#include <shared/assert.h>
#include <shared/log.h>
#include <shared/math.inl>

PAW_DISABLE_ALL_WARNINGS_BEGIN
#include <pxr/usdImaging/usdImaging/meshAdapter.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/plug/notice.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/primCompositionQuery.h>
#include <pxr/usd/usdGeom/xformable.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/plane.h>
#include <pxr/imaging/hd/meshUtil.h>
#include <pxr/imaging/hd/vtBufferSource.h>
PAW_DISABLE_ALL_WARNINGS_END

#include "debug_draw.h"
#include "new_renderer.h"
#include "allocators/page_pool.h"
#include "allocators/arena_allocator.h"
#include "profiler.h"

static void processPrim(const pxr::UsdPrim& prim, const Mat4& parent_mat, Slice<StaticSceneVertex>& vertex_buffer, u32& vertex_count, Slice<StaticMesh>& mesh_buffer, u32& mesh_count)
{
	Mat4 local_mat{};
	if (prim.IsA<pxr::UsdGeomXformable>())
	{
		pxr::UsdGeomXformable xform(prim);
		pxr::GfMatrix4d usd_mat;
		bool reset_xform_stack;
		bool result = xform.GetLocalTransformation(&usd_mat, &reset_xform_stack);
		PAW_ASSERT(result);
		PAW_ASSERT_UNUSED(result);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				local_mat.data[i][j] = (f32)usd_mat.data()[i * 4 + j];
			}
		}
	}

	Mat4 world_mat = parent_mat * local_mat;

	if (prim.IsA<pxr::UsdGeomMesh>())
	{
		pxr::UsdGeomMesh mesh(prim);
		pxr::UsdImagingMeshAdapter adapter;
		pxr::VtValue topology =
			adapter.GetTopology(prim, prim.GetPath(), pxr::UsdTimeCode::Default());
		PAW_ASSERT(!topology.IsEmpty());

		pxr::HdMeshUtil mesh_util(&topology.Get<pxr::HdMeshTopology>(), prim.GetPath());

		const pxr::TfToken normals_interpolation = mesh.GetNormalsInterpolation();
		// ImGui::Text("Normal interpolation: %s", normals_interpolation.GetText());

		pxr::VtVec3iArray indices;
		pxr::VtIntArray primitive_params;
		mesh_util.ComputeTriangleIndices(&indices, &primitive_params);
		/*ImGui::SameLine();
		ImGui::Text("Index count: %llu", indices.size() * 3);*/

		pxr::VtVec3fArray vertices;
		mesh.GetPointsAttr().Get(&vertices);

		pxr::VtVec3fArray normals;
		bool has_normals = mesh.GetNormalsAttr().Get(&normals);
		PAW_ASSERT(has_normals);
		PAW_ASSERT_UNUSED(has_normals);
		pxr::VtValue triangulated_normals_val;
		pxr::TfToken name("temp");
		pxr::VtValue normals_val(normals);
		pxr::HdVtBufferSource buffer(name, normals_val);
		bool success = mesh_util.ComputeTriangulatedFaceVaryingPrimvar(buffer.GetData(), (int)buffer.GetNumElements(), buffer.GetTupleType().type, &triangulated_normals_val);
		PAW_ASSERT(success);
		PAW_ASSERT_UNUSED(success);
		pxr::VtVec3fArray triangulated_normals = triangulated_normals_val.Get<pxr::VtVec3fArray>();

		PAW_LOG_INFO("Triangulated %s", prim.GetName().GetText());

		StaticMesh& out_mesh = mesh_buffer[mesh_count++];
		out_mesh.vertex_offset = vertex_count;
		out_mesh.vertex_count = (u32)indices.size() * 3;
		out_mesh.transform = world_mat;

		usize normal_index = 0;
		for (const pxr::GfVec3i& triangle : indices)
		{

			for (u8 i = 0; i < 3; i++)
			{
				StaticSceneVertex& out_vertex = vertex_buffer[vertex_count++];
				out_vertex.position = Vec3{
					vertices[triangle[i]][0],
					vertices[triangle[i]][1],
					vertices[triangle[i]][2],
				};

				out_vertex.normal = Vec3{
					triangulated_normals[normal_index][0],
					triangulated_normals[normal_index][1],
					triangulated_normals[normal_index][2],
				};
				normal_index++;
			}
		}
	}

	for (const pxr::UsdPrim& child : prim.GetChildren())
	{
		processPrim(child, world_mat, vertex_buffer, vertex_count, mesh_buffer, mesh_count);
	}
}

void tempLoadWorld(const char* path, ArenaAllocator& persistent_allocator, PagePool& page_pool)
{
	PAW_PROFILER_FUNC();
	pxr::PlugRegistry::GetInstance().RegisterPlugins("C:/dev/paw6/usd/usd/");

	// pxr::UsdGeomXformCache xform_cache();
	//  static std::string path = "C:\\Users\\natha\\Documents\\maya\\projects\\default\\scenes\\stageShape1-vRrcTM.usd";
	pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(path);
	// pxr::UsdStageRefPtr stage = pxr::UsdStage::Open("C:\\Users\\natha\\Downloads\\main_sponza\\Main.1_Sponza\\NewSponza_Main_USD_Yup_002.usda");
	PAW_ASSERT(stage);

	ArenaAllocator temp_allocator;
	temp_allocator.init(&page_pool, "World Load Temp Allocator"_str);
	Slice<StaticSceneVertex> vertex_buffer = temp_allocator.alloc<StaticSceneVertex>(temp_allocator.get_page_size() / sizeof(StaticSceneVertex));

	Slice<StaticMesh> mesh_buffer = persistent_allocator.alloc<StaticMesh>(256);

	u32 vertex_count = 0;
	u32 mesh_count = 0;

	// processPrim(stage->GetPseudoRoot(), Mat4{}, vertex_buffer, vertex_count, mesh_buffer, mesh_count);

	pxr::UsdGeomXformCache xform_cache;

	for (pxr::UsdPrim prim : stage->TraverseAll())
	{
		PAW_LOG_INFO("Traversing %s", prim.GetName().GetText());

		Mat4 world_mat{};

		if (prim.IsA<pxr::UsdGeomXformable>())
		{
			pxr::GfMatrix4d transform = xform_cache.GetLocalToWorldTransform(prim);
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					world_mat.data[i][j] = (f32)transform.data()[i * 4 + j];
				}
			}

			debugDrawTransformFromMat(world_mat, true, 0.025f, PAW_LIFETIME_INFINITE);
		}

		if (prim.IsA<pxr::UsdGeomMesh>())
		{
			pxr::UsdGeomMesh mesh(prim);
			pxr::UsdImagingMeshAdapter adapter;
			pxr::VtValue topology =
				adapter.GetTopology(prim, prim.GetPath(), pxr::UsdTimeCode::Default());
			PAW_ASSERT(!topology.IsEmpty());

			pxr::HdMeshUtil mesh_util(&topology.Get<pxr::HdMeshTopology>(), prim.GetPath());

			const pxr::TfToken normals_interpolation = mesh.GetNormalsInterpolation();
			// ImGui::Text("Normal interpolation: %s", normals_interpolation.GetText());

			pxr::VtVec3iArray indices;
			pxr::VtIntArray primitive_params;
			mesh_util.ComputeTriangleIndices(&indices, &primitive_params);
			/*ImGui::SameLine();
			ImGui::Text("Index count: %llu", indices.size() * 3);*/

			pxr::VtVec3fArray vertices;
			mesh.GetPointsAttr().Get(&vertices);

			pxr::VtVec3fArray normals;
			bool has_normals = mesh.GetNormalsAttr().Get(&normals);
			PAW_ASSERT_UNUSED(has_normals);
			PAW_ASSERT(has_normals);
			pxr::VtValue triangulated_normals_val;
			pxr::TfToken name("temp");
			pxr::VtValue normals_val(normals);
			pxr::HdVtBufferSource buffer(name, normals_val);
			bool success = mesh_util.ComputeTriangulatedFaceVaryingPrimvar(buffer.GetData(), (int)buffer.GetNumElements(), buffer.GetTupleType().type, &triangulated_normals_val);
			PAW_ASSERT_UNUSED(success);
			PAW_ASSERT(success);
			pxr::VtVec3fArray triangulated_normals = triangulated_normals_val.Get<pxr::VtVec3fArray>();

			PAW_LOG_INFO("Triangulated %s", prim.GetName().GetText());

			StaticMesh& out_mesh = mesh_buffer[mesh_count++];
			out_mesh.vertex_offset = vertex_count;
			out_mesh.vertex_count = (u32)indices.size() * 3;
			out_mesh.transform = world_mat;

			usize normal_index = 0;
			for (const pxr::GfVec3i& triangle : indices)
			{

				for (u8 i = 0; i < 3; i++)
				{
					StaticSceneVertex& out_vertex = vertex_buffer[vertex_count++];
					out_vertex.position = Vec3{
						vertices[triangle[i]][0],
						vertices[triangle[i]][1],
						vertices[triangle[i]][2],
					};

					out_vertex.normal = Vec3{
						triangulated_normals[normal_index][0],
						triangulated_normals[normal_index][1],
						triangulated_normals[normal_index][2],
					};
					normal_index++;
				}
			}
		}
	}

	/*const GpuBufferView gpu_vertex_buffer = rendererCreateAndUploadBuffer(vertex_buffer.toConstByteSlice(), GpuBufferType_Storage);
	const u32 gpu_vertex_buffer_slot = rendererPushBufferToShader(gpu_vertex_buffer);
	rendererPushStaticScene(StaticScene{
		.vertex_buffer_slot = gpu_vertex_buffer_slot,
		.meshes = Slice<StaticMesh>{mesh_buffer.ptr, (usize)mesh_count},
	});*/

	temp_allocator.reset();
}
