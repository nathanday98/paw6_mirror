#pragma once

#pragma warning(push, 0)
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#pragma warning(pop)
void convert_render_graph(pxr::UsdStageConstRefPtr stage, pxr::UsdPrim graph_prim);