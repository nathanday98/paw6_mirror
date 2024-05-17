#include <asset_converters/render_graph.h>

#pragma warning(push, 0)
#include <pxr/usd/usd/attribute.h>
#include <stack>
#pragma warning(pop)

using namespace pxr;

TF_DEBUG_CODES(PAW_RENDER_GRAPH);

void convert_render_graph(UsdStageConstRefPtr stage, UsdPrim graph_prim)
{
	UsdAttribute const output_attribute = graph_prim.GetAttribute(TfToken("output:backbuffer"));

	// I can make a separate list of nodes that are dependant on the backbuffer size, ordered by their hierarchy, so that they can be re-calculated
	// when backbuffer size changes in one linear iteration

	UsdSchemaRegistry& schema_registry = UsdSchemaRegistry::GetInstance();

	static TfToken read_texture_api_token("RenderGraphReadTextureAPI");
	TfType const read_texture_api_type = schema_registry.GetTypeFromName(read_texture_api_token);
	TF_AXIOM(read_texture_api_type);
	static TfToken read_write_texture_api_token("RenderGraphReadWriteTextureAPI");
	TfType const read_write_texture_api_type = schema_registry.GetTypeFromName(read_write_texture_api_token);
	TF_AXIOM(read_write_texture_api_type);

	std::set<UsdPrim> nodes;
	std::stack<UsdPrim> process_stack;

	{
		SdfPathVector connection_paths;

		if (output_attribute.GetConnections(&connection_paths))
		{
			SdfPath path = connection_paths[0];
			UsdPrim const node = stage->GetPrimAtPath(path.GetPrimPath());

			process_stack.push(node);
		}
	}

	while (!process_stack.empty())
	{
		UsdPrim const node = process_stack.top();
		process_stack.pop();
		TF_DEBUG_MSG(PAW_RENDER_GRAPH, "processing: %s\n", node.GetTypeName().data());

		nodes.insert(node);
		TfTokenVector applied_schemas = node.GetAppliedSchemas();
		for (TfToken const schema : applied_schemas)
		{
			std::pair<TfToken, TfToken> data = UsdSchemaRegistry::GetTypeNameAndInstance(schema);
			TfToken const& type_name = data.first;
			TfToken const& instance_name = data.second;
			if (type_name == read_texture_api_token)
			{
				TfToken attribute_name = UsdSchemaRegistry::MakeMultipleApplyNameInstance("input:__INSTANCE_NAME__", instance_name);
				UsdAttribute read_attribute = node.GetAttribute(attribute_name);
				SdfPathVector read_connections;
				if (read_attribute.GetConnections(&read_connections))
				{
					SdfPath read_path = read_connections[0];
					UsdPrim const read_node = stage->GetPrimAtPath(read_path.GetPrimPath());
					process_stack.push(read_node);
					TF_DEBUG_MSG(PAW_RENDER_GRAPH, "\t%s push: %s - %s\n", instance_name.data(), read_node.GetTypeName().data(), read_path.GetAsString().c_str());
				}
			}

			if (type_name == read_write_texture_api_token)
			{
				TfToken attribute_name = UsdSchemaRegistry::MakeMultipleApplyNameInstance("input:__INSTANCE_NAME__", instance_name);
				UsdAttribute read_attribute = node.GetAttribute(attribute_name);
				SdfPathVector read_connections;
				if (read_attribute.GetConnections(&read_connections))
				{
					SdfPath read_path = read_connections[0];
					UsdPrim const read_node = stage->GetPrimAtPath(read_path.GetPrimPath());
					process_stack.push(read_node);
					TF_DEBUG_MSG(PAW_RENDER_GRAPH, "\t%s push: %s - %s\n", instance_name.data(), read_node.GetTypeName().data(), read_path.GetAsString().c_str());
				}
			}
		}
	}

	for (UsdPrim const& node : nodes)
	{
		TF_DEBUG_MSG(PAW_RENDER_GRAPH, "node: %s\n", node.GetTypeName().data());
	}
}
