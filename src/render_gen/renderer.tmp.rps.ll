;
; Note: shader requires additional functionality:
;       Use native low precision
;
; shader debug name: a215f65fa3535f007294c47ca0b73a88.pdb
; shader hash: a215f65fa3535f007294c47ca0b73a88
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%0 = type { i32, i32, i32, i32 }
%___rpsl_node_info_struct = type <{ i32, i32, i32, i32, i32 }>
%___rpsl_entry_desc_struct = type <{ i32, i32, i32, i32, i8*, i8* }>
%___rpsl_type_info_struct = type <{ i8, i8, i8, i8, i32, i32, i32 }>
%___rpsl_params_info_struct = type <{ i32, i32, i32, i32, i32, i16, i16 }>
%___rpsl_shader_ref_struct = type <{ i32, i32, i32, i32 }>
%___rpsl_pipeline_info_struct = type <{ i32, i32, i32, i32 }>
%___rpsl_pipeline_field_info_struct = type <{ i32, i32, i32, i32, i32, i32, i32, i32 }>
%___rpsl_pipeline_res_binding_info_struct = type <{ i32, i32, i32, i32 }>
%___rpsl_module_info_struct = type <{ i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, [329 x i8]*, [5 x %___rpsl_node_info_struct]*, [9 x %___rpsl_type_info_struct]*, [21 x %___rpsl_params_info_struct]*, [2 x %___rpsl_entry_desc_struct]*, [1 x %___rpsl_shader_ref_struct]*, [1 x %___rpsl_pipeline_info_struct]*, [1 x %___rpsl_pipeline_field_info_struct]*, [1 x %___rpsl_pipeline_res_binding_info_struct]*, i32 }>
%struct.RpsParameterDesc = type { %struct.RpsTypeInfo, i32, %0*, i8*, i32 }
%struct.RpsTypeInfo = type { i16, i16 }
%struct.RpsNodeDesc = type { i32, i32, %struct.RpsParameterDesc*, i8* }
%struct.RpslEntry = type { i8*, void (i32, i8**, i32)*, %struct.RpsParameterDesc*, %struct.RpsNodeDesc*, i32, i32 }
%struct.texture = type { i32, i32, i32, i32, %struct.SubresourceRange, float, i32 }
%struct.SubresourceRange = type { i16, i16, i32, i32 }
%struct.ResourceDesc = type { i32, i32, i32, i32, i32, i32, i32, i32, i32 }
%class.matrix.float.4.4 = type { [4 x <4 x float>] }

@"@@rps_Str0" = private unnamed_addr constant [14 x i8] c"game_color_rt\00"
@"@@rps_Str1" = private unnamed_addr constant [19 x i8] c"game_color_rt_msaa\00"
@"@@rps_Str2" = private unnamed_addr constant [19 x i8] c"game_depth_rt_msaa\00"
@"@@rps_Str3" = private unnamed_addr constant [30 x i8] c"game_color_rt_pre_display_map\00"
@___rpsl_nodedefs_renderer = private constant [5 x %___rpsl_node_info_struct] [%___rpsl_node_info_struct <{ i32 0, i32 176, i32 0, i32 2, i32 1 }>, %___rpsl_node_info_struct <{ i32 1, i32 188, i32 2, i32 4, i32 1 }>, %___rpsl_node_info_struct <{ i32 2, i32 208, i32 6, i32 6, i32 1 }>, %___rpsl_node_info_struct <{ i32 3, i32 216, i32 12, i32 2, i32 1 }>, %___rpsl_node_info_struct zeroinitializer], align 4
@___rpsl_entries_renderer = private constant [2 x %___rpsl_entry_desc_struct] [%___rpsl_entry_desc_struct <{ i32 0, i32 232, i32 14, i32 6, i8* bitcast (void (%struct.texture*, %struct.texture*, i1, <2 x i32>, %class.matrix.float.4.4, i32)* @rpsl_M_renderer_Fn_main to i8*), i8* bitcast (void (i32, i8**, i32)* @rpsl_M_renderer_Fn_main_wrapper to i8*) }>, %___rpsl_entry_desc_struct zeroinitializer], align 4
@___rpsl_types_metadata_renderer = private constant [9 x %___rpsl_type_info_struct] [%___rpsl_type_info_struct <{ i8 6, i8 0, i8 0, i8 0, i32 0, i32 36, i32 4 }>, %___rpsl_type_info_struct <{ i8 4, i8 32, i8 0, i8 4, i32 0, i32 16, i32 4 }>, %___rpsl_type_info_struct <{ i8 3, i8 32, i8 0, i8 0, i32 0, i32 4, i32 4 }>, %___rpsl_type_info_struct <{ i8 4, i8 32, i8 0, i8 0, i32 0, i32 4, i32 4 }>, %___rpsl_type_info_struct <{ i8 3, i8 32, i8 0, i8 2, i32 0, i32 8, i32 4 }>, %___rpsl_type_info_struct <{ i8 1, i8 8, i8 0, i8 0, i32 0, i32 4, i32 4 }>, %___rpsl_type_info_struct <{ i8 2, i8 32, i8 0, i8 2, i32 0, i32 8, i32 4 }>, %___rpsl_type_info_struct <{ i8 4, i8 32, i8 4, i8 4, i32 0, i32 64, i32 4 }>, %___rpsl_type_info_struct zeroinitializer], align 4
@___rpsl_params_metadata_renderer = private constant [21 x %___rpsl_params_info_struct] [%___rpsl_params_info_struct <{ i32 91, i32 0, i32 272629888, i32 -1, i32 0, i16 36, i16 0 }>, %___rpsl_params_info_struct <{ i32 93, i32 1, i32 0, i32 -1, i32 0, i16 16, i16 36 }>, %___rpsl_params_info_struct <{ i32 91, i32 0, i32 289409536, i32 -1, i32 0, i16 36, i16 0 }>, %___rpsl_params_info_struct <{ i32 98, i32 2, i32 0, i32 -1, i32 0, i16 4, i16 36 }>, %___rpsl_params_info_struct <{ i32 105, i32 3, i32 0, i32 -1, i32 0, i16 4, i16 40 }>, %___rpsl_params_info_struct <{ i32 107, i32 2, i32 0, i32 -1, i32 0, i16 4, i16 44 }>, %___rpsl_params_info_struct <{ i32 109, i32 0, i32 65536, i32 -1, i32 0, i16 36, i16 0 }>, %___rpsl_params_info_struct <{ i32 113, i32 4, i32 0, i32 -1, i32 0, i16 8, i16 36 }>, %___rpsl_params_info_struct <{ i32 123, i32 0, i32 32768, i32 -1, i32 0, i16 36, i16 44 }>, %___rpsl_params_info_struct <{ i32 127, i32 4, i32 0, i32 -1, i32 0, i16 8, i16 80 }>, %___rpsl_params_info_struct <{ i32 137, i32 4, i32 0, i32 -1, i32 0, i16 8, i16 88 }>, %___rpsl_params_info_struct <{ i32 144, i32 2, i32 0, i32 -1, i32 0, i16 4, i16 96 }>, %___rpsl_params_info_struct <{ i32 156, i32 0, i32 128, i32 -1, i32 0, i16 36, i16 0 }>, %___rpsl_params_info_struct <{ i32 170, i32 0, i32 16, i32 -1, i32 0, i16 36, i16 36 }>, %___rpsl_params_info_struct <{ i32 237, i32 0, i32 524288, i32 -1, i32 0, i16 36, i16 0 }>, %___rpsl_params_info_struct <{ i32 248, i32 0, i32 16, i32 -1, i32 0, i16 36, i16 36 }>, %___rpsl_params_info_struct <{ i32 261, i32 5, i32 0, i32 -1, i32 0, i16 4, i16 72 }>, %___rpsl_params_info_struct <{ i32 265, i32 6, i32 0, i32 -1, i32 0, i16 8, i16 76 }>, %___rpsl_params_info_struct <{ i32 287, i32 7, i32 0, i32 -1, i32 0, i16 64, i16 84 }>, %___rpsl_params_info_struct <{ i32 305, i32 2, i32 0, i32 -1, i32 0, i16 4, i16 148 }>, %___rpsl_params_info_struct zeroinitializer], align 4
@___rpsl_shader_refs_renderer = private constant [1 x %___rpsl_shader_ref_struct] zeroinitializer, align 4
@___rpsl_pipelines_renderer = private constant [1 x %___rpsl_pipeline_info_struct] zeroinitializer, align 4
@___rpsl_pipeline_fields_renderer = private constant [1 x %___rpsl_pipeline_field_info_struct] zeroinitializer, align 4
@___rpsl_pipeline_res_bindings_renderer = private constant [1 x %___rpsl_pipeline_res_binding_info_struct] zeroinitializer, align 4
@___rpsl_string_table_renderer = constant [329 x i8] c"game_color_rt\00game_color_rt_msaa\00game_depth_rt_msaa\00game_color_rt_pre_display_map\00renderer\00t\00data\00option\00d\00s\00dst\00dstOffset\00src\00srcOffset\00extent\00resolveMode\00render_target\00input\00clear_color\00clear_depth_stencil\00resolve\00display_map_hdr\00main\00backbuffer\00texture_aces\00hdr\00in_game_viewport_size\00camera_projection\00game_color_sample_count\00", align 4
@___rpsl_module_info_renderer = dllexport constant %___rpsl_module_info_struct <{ i32 1297305682, i32 3, i32 9, i32 82, i32 329, i32 4, i32 8, i32 20, i32 1, i32 0, i32 0, i32 0, i32 0, [329 x i8]* @___rpsl_string_table_renderer, [5 x %___rpsl_node_info_struct]* @___rpsl_nodedefs_renderer, [9 x %___rpsl_type_info_struct]* @___rpsl_types_metadata_renderer, [21 x %___rpsl_params_info_struct]* @___rpsl_params_metadata_renderer, [2 x %___rpsl_entry_desc_struct]* @___rpsl_entries_renderer, [1 x %___rpsl_shader_ref_struct]* @___rpsl_shader_refs_renderer, [1 x %___rpsl_pipeline_info_struct]* @___rpsl_pipelines_renderer, [1 x %___rpsl_pipeline_field_info_struct]* @___rpsl_pipeline_fields_renderer, [1 x %___rpsl_pipeline_res_binding_info_struct]* @___rpsl_pipeline_res_bindings_renderer, i32 1297305682 }>, align 4
@"@@rps_Str4" = private unnamed_addr constant [12 x i8] c"clear_color\00"
@"@@rps_Str5" = private unnamed_addr constant [2 x i8] c"t\00"
@"@@rps_ParamAttr6" = private constant %0 { i32 272629888, i32 0, i32 0, i32 0 }, align 4
@"@@rps_Str7" = private unnamed_addr constant [5 x i8] c"data\00"
@"@@rps_ParamAttr8" = private constant %0 { i32 0, i32 0, i32 27, i32 0 }, align 4
@"@@rps_ParamDescArray9" = private constant [2 x %struct.RpsParameterDesc] [%struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr6", i8* getelementptr inbounds ([2 x i8], [2 x i8]* @"@@rps_Str5", i32 0, i32 0), i32 4 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 16, i16 0 }, i32 0, %0* @"@@rps_ParamAttr8", i8* getelementptr inbounds ([5 x i8], [5 x i8]* @"@@rps_Str7", i32 0, i32 0), i32 0 }], align 4
@"@@rps_Str10" = private unnamed_addr constant [20 x i8] c"clear_depth_stencil\00"
@"@@rps_Str11" = private unnamed_addr constant [2 x i8] c"t\00"
@"@@rps_ParamAttr12" = private constant %0 { i32 289409536, i32 0, i32 0, i32 0 }, align 4
@"@@rps_Str13" = private unnamed_addr constant [7 x i8] c"option\00"
@"@@rps_ParamAttr14" = private constant %0 zeroinitializer, align 4
@"@@rps_Str15" = private unnamed_addr constant [2 x i8] c"d\00"
@"@@rps_ParamAttr16" = private constant %0 { i32 0, i32 0, i32 28, i32 0 }, align 4
@"@@rps_Str17" = private unnamed_addr constant [2 x i8] c"s\00"
@"@@rps_ParamAttr18" = private constant %0 { i32 0, i32 0, i32 29, i32 0 }, align 4
@"@@rps_ParamDescArray19" = private constant [4 x %struct.RpsParameterDesc] [%struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr12", i8* getelementptr inbounds ([2 x i8], [2 x i8]* @"@@rps_Str11", i32 0, i32 0), i32 4 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 4, i16 0 }, i32 0, %0* @"@@rps_ParamAttr14", i8* getelementptr inbounds ([7 x i8], [7 x i8]* @"@@rps_Str13", i32 0, i32 0), i32 0 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 4, i16 0 }, i32 0, %0* @"@@rps_ParamAttr16", i8* getelementptr inbounds ([2 x i8], [2 x i8]* @"@@rps_Str15", i32 0, i32 0), i32 0 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 4, i16 0 }, i32 0, %0* @"@@rps_ParamAttr18", i8* getelementptr inbounds ([2 x i8], [2 x i8]* @"@@rps_Str17", i32 0, i32 0), i32 0 }], align 4
@"@@rps_Str20" = private unnamed_addr constant [8 x i8] c"resolve\00"
@"@@rps_Str21" = private unnamed_addr constant [4 x i8] c"dst\00"
@"@@rps_ParamAttr22" = private constant %0 { i32 65536, i32 0, i32 0, i32 0 }, align 4
@"@@rps_Str23" = private unnamed_addr constant [10 x i8] c"dstOffset\00"
@"@@rps_ParamAttr24" = private constant %0 zeroinitializer, align 4
@"@@rps_Str25" = private unnamed_addr constant [4 x i8] c"src\00"
@"@@rps_ParamAttr26" = private constant %0 { i32 32768, i32 0, i32 0, i32 0 }, align 4
@"@@rps_Str27" = private unnamed_addr constant [10 x i8] c"srcOffset\00"
@"@@rps_ParamAttr28" = private constant %0 zeroinitializer, align 4
@"@@rps_Str29" = private unnamed_addr constant [7 x i8] c"extent\00"
@"@@rps_ParamAttr30" = private constant %0 zeroinitializer, align 4
@"@@rps_Str31" = private unnamed_addr constant [12 x i8] c"resolveMode\00"
@"@@rps_ParamAttr32" = private constant %0 zeroinitializer, align 4
@"@@rps_ParamDescArray33" = private constant [6 x %struct.RpsParameterDesc] [%struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr22", i8* getelementptr inbounds ([4 x i8], [4 x i8]* @"@@rps_Str21", i32 0, i32 0), i32 4 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 8, i16 0 }, i32 0, %0* @"@@rps_ParamAttr24", i8* getelementptr inbounds ([10 x i8], [10 x i8]* @"@@rps_Str23", i32 0, i32 0), i32 0 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr26", i8* getelementptr inbounds ([4 x i8], [4 x i8]* @"@@rps_Str25", i32 0, i32 0), i32 4 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 8, i16 0 }, i32 0, %0* @"@@rps_ParamAttr28", i8* getelementptr inbounds ([10 x i8], [10 x i8]* @"@@rps_Str27", i32 0, i32 0), i32 0 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 8, i16 0 }, i32 0, %0* @"@@rps_ParamAttr30", i8* getelementptr inbounds ([7 x i8], [7 x i8]* @"@@rps_Str29", i32 0, i32 0), i32 0 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 4, i16 0 }, i32 0, %0* @"@@rps_ParamAttr32", i8* getelementptr inbounds ([12 x i8], [12 x i8]* @"@@rps_Str31", i32 0, i32 0), i32 0 }], align 4
@"@@rps_Str34" = private unnamed_addr constant [16 x i8] c"display_map_hdr\00"
@"@@rps_Str35" = private unnamed_addr constant [14 x i8] c"render_target\00"
@"@@rps_ParamAttr36" = private constant %0 { i32 128, i32 0, i32 35, i32 0 }, align 4
@"@@rps_Str37" = private unnamed_addr constant [6 x i8] c"input\00"
@"@@rps_ParamAttr38" = private constant %0 { i32 16, i32 10, i32 0, i32 0 }, align 4
@"@@rps_ParamDescArray39" = private constant [2 x %struct.RpsParameterDesc] [%struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr36", i8* getelementptr inbounds ([14 x i8], [14 x i8]* @"@@rps_Str35", i32 0, i32 0), i32 4 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr38", i8* getelementptr inbounds ([6 x i8], [6 x i8]* @"@@rps_Str37", i32 0, i32 0), i32 4 }], align 4
@NodeDecls_renderer = dllexport constant [4 x %struct.RpsNodeDesc] [%struct.RpsNodeDesc { i32 1, i32 2, %struct.RpsParameterDesc* getelementptr inbounds ([2 x %struct.RpsParameterDesc], [2 x %struct.RpsParameterDesc]* @"@@rps_ParamDescArray9", i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @"@@rps_Str4", i32 0, i32 0) }, %struct.RpsNodeDesc { i32 1, i32 4, %struct.RpsParameterDesc* getelementptr inbounds ([4 x %struct.RpsParameterDesc], [4 x %struct.RpsParameterDesc]* @"@@rps_ParamDescArray19", i32 0, i32 0), i8* getelementptr inbounds ([20 x i8], [20 x i8]* @"@@rps_Str10", i32 0, i32 0) }, %struct.RpsNodeDesc { i32 1, i32 6, %struct.RpsParameterDesc* getelementptr inbounds ([6 x %struct.RpsParameterDesc], [6 x %struct.RpsParameterDesc]* @"@@rps_ParamDescArray33", i32 0, i32 0), i8* getelementptr inbounds ([8 x i8], [8 x i8]* @"@@rps_Str20", i32 0, i32 0) }, %struct.RpsNodeDesc { i32 1, i32 2, %struct.RpsParameterDesc* getelementptr inbounds ([2 x %struct.RpsParameterDesc], [2 x %struct.RpsParameterDesc]* @"@@rps_ParamDescArray39", i32 0, i32 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @"@@rps_Str34", i32 0, i32 0) }], align 4
@"@@rps_Str40" = private unnamed_addr constant [5 x i8] c"main\00"
@"@@rps_Str41" = private unnamed_addr constant [11 x i8] c"backbuffer\00"
@"@@rps_ParamAttr42" = private constant %0 { i32 524288, i32 0, i32 0, i32 0 }, align 4
@"@@rps_Str43" = private unnamed_addr constant [13 x i8] c"texture_aces\00"
@"@@rps_ParamAttr44" = private constant %0 { i32 16, i32 10, i32 0, i32 0 }, align 4
@"@@rps_Str45" = private unnamed_addr constant [4 x i8] c"hdr\00"
@"@@rps_ParamAttr46" = private constant %0 zeroinitializer, align 4
@"@@rps_Str47" = private unnamed_addr constant [22 x i8] c"in_game_viewport_size\00"
@"@@rps_ParamAttr48" = private constant %0 zeroinitializer, align 4
@"@@rps_Str49" = private unnamed_addr constant [18 x i8] c"camera_projection\00"
@"@@rps_ParamAttr50" = private constant %0 zeroinitializer, align 4
@"@@rps_Str51" = private unnamed_addr constant [24 x i8] c"game_color_sample_count\00"
@"@@rps_ParamAttr52" = private constant %0 zeroinitializer, align 4
@"@@rps_ParamDescArray53" = private constant [6 x %struct.RpsParameterDesc] [%struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr42", i8* getelementptr inbounds ([11 x i8], [11 x i8]* @"@@rps_Str41", i32 0, i32 0), i32 4 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr44", i8* getelementptr inbounds ([13 x i8], [13 x i8]* @"@@rps_Str43", i32 0, i32 0), i32 4 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 4, i16 0 }, i32 0, %0* @"@@rps_ParamAttr46", i8* getelementptr inbounds ([4 x i8], [4 x i8]* @"@@rps_Str45", i32 0, i32 0), i32 0 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 8, i16 0 }, i32 0, %0* @"@@rps_ParamAttr48", i8* getelementptr inbounds ([22 x i8], [22 x i8]* @"@@rps_Str47", i32 0, i32 0), i32 0 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 64, i16 0 }, i32 0, %0* @"@@rps_ParamAttr50", i8* getelementptr inbounds ([18 x i8], [18 x i8]* @"@@rps_Str49", i32 0, i32 0), i32 0 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 4, i16 0 }, i32 0, %0* @"@@rps_ParamAttr52", i8* getelementptr inbounds ([24 x i8], [24 x i8]* @"@@rps_Str51", i32 0, i32 0), i32 0 }], align 4
@"rpsl_M_renderer_E_main@value" = constant %struct.RpslEntry { i8* getelementptr inbounds ([5 x i8], [5 x i8]* @"@@rps_Str40", i32 0, i32 0), void (i32, i8**, i32)* @rpsl_M_renderer_Fn_main_wrapper, %struct.RpsParameterDesc* getelementptr inbounds ([6 x %struct.RpsParameterDesc], [6 x %struct.RpsParameterDesc]* @"@@rps_ParamDescArray53", i32 0, i32 0), %struct.RpsNodeDesc* getelementptr inbounds ([4 x %struct.RpsNodeDesc], [4 x %struct.RpsNodeDesc]* @NodeDecls_renderer, i32 0, i32 0), i32 6, i32 4 }, align 4
@rpsl_M_renderer_E_main = dllexport constant %struct.RpslEntry* @"rpsl_M_renderer_E_main@value", align 4
@rpsl_M_renderer_E_main_pp = dllexport constant %struct.RpslEntry** @rpsl_M_renderer_E_main, align 4
@dx.nothing.a = internal constant [1 x i32] zeroinitializer

; Function Attrs: nounwind
define internal fastcc void @"\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z"(%struct.texture* noalias sret %agg.result, i32 %resourceHdl, %struct.ResourceDesc* %desc) #0 {
entry:
  %0 = alloca %struct.texture
  call void @llvm.dbg.declare(metadata %struct.ResourceDesc* %desc, metadata !214, metadata !215), !dbg !216 ; var:"desc" !DIExpression() func:"make_default_texture_view_from_desc"
  call void @llvm.dbg.value(metadata i32 %resourceHdl, i64 0, metadata !217, metadata !215), !dbg !218 ; var:"resourceHdl" !DIExpression() func:"make_default_texture_view_from_desc"
  %MipLevels = getelementptr inbounds %struct.ResourceDesc, %struct.ResourceDesc* %desc, i32 0, i32 6, !dbg !219 ; line:158 col:14
  %1 = load i32, i32* %MipLevels, align 4, !dbg !219 ; line:158 col:14
  %Type = getelementptr inbounds %struct.ResourceDesc, %struct.ResourceDesc* %desc, i32 0, i32 0, !dbg !220 ; line:157 col:15
  %2 = load i32, i32* %Type, align 4, !dbg !220 ; line:157 col:15
  %cmp = icmp eq i32 %2, 4, !dbg !221 ; line:157 col:20
  %tobool = icmp ne i1 %cmp, false, !dbg !221 ; line:157 col:20
  %tobool1 = icmp ne i1 %tobool, false, !dbg !221 ; line:157 col:20
  call void @llvm.dbg.declare(metadata %struct.texture* %0, metadata !222, metadata !215), !dbg !223 ; var:"result" !DIExpression() func:"make_default_texture_view"
  br i1 %tobool1, label %cond.true, label %cond.false, !dbg !225 ; line:157 col:9

cond.true:                                        ; preds = %entry
  br label %cond.end, !dbg !225 ; line:157 col:9

cond.false:                                       ; preds = %entry
  %DepthOrArraySize = getelementptr inbounds %struct.ResourceDesc, %struct.ResourceDesc* %desc, i32 0, i32 5, !dbg !226 ; line:157 col:54
  %3 = load i32, i32* %DepthOrArraySize, align 4, !dbg !226 ; line:157 col:54
  br label %cond.end, !dbg !225 ; line:157 col:9

cond.end:                                         ; preds = %cond.false, %cond.true
  %cond = phi i32 [ 1, %cond.true ], [ %3, %cond.false ], !dbg !225 ; line:157 col:9
  %4 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !227 ; line:154 col:12
  call void @llvm.dbg.value(metadata i32 %1, i64 0, metadata !228, metadata !215), !dbg !229 ; var:"numMips" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 %cond, i64 0, metadata !230, metadata !215), !dbg !231 ; var:"arraySlices" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 %resourceHdl, i64 0, metadata !232, metadata !215), !dbg !233 ; var:"resourceHdl" !DIExpression() func:"make_default_texture_view"
  %5 = bitcast %struct.texture* %0 to i8*, !dbg !227 ; line:154 col:12
  call void @llvm.memset.p0i8.i32(i8* %5, i8 0, i32 36, i32 4, i1 false) #0, !dbg !227, !noalias !234 ; line:154 col:12
  %Resource.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 0, !dbg !237 ; line:123 col:12
  %6 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !238 ; line:123 col:21
  store i32 %resourceHdl, i32* %Resource.i, align 4, !dbg !238, !noalias !234 ; line:123 col:21
  %ViewFormat.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 1, !dbg !239 ; line:124 col:12
  %7 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !240 ; line:124 col:23
  store i32 0, i32* %ViewFormat.i, align 4, !dbg !240, !noalias !234 ; line:124 col:23
  %TemporalLayer.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 2, !dbg !241 ; line:125 col:12
  %8 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !242 ; line:125 col:26
  store i32 0, i32* %TemporalLayer.i, align 4, !dbg !242, !noalias !234 ; line:125 col:26
  %Flags.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 3, !dbg !243 ; line:126 col:12
  %9 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !244 ; line:126 col:18
  store i32 0, i32* %Flags.i, align 4, !dbg !244, !noalias !234 ; line:126 col:18
  %base_mip_level.i5 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 0, !dbg !245 ; line:127 col:29
  %10 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !246 ; line:127 col:44
  store i16 0, i16* %base_mip_level.i5, align 2, !dbg !246, !noalias !234 ; line:127 col:44
  %conv.i = trunc i32 %1 to i16, !dbg !247 ; line:128 col:56
  %mip_level_count.i4 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 1, !dbg !248 ; line:128 col:29
  %11 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !249 ; line:128 col:45
  store i16 %conv.i, i16* %mip_level_count.i4, align 2, !dbg !249, !noalias !234 ; line:128 col:45
  %base_array_layer.i3 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 2, !dbg !250 ; line:129 col:29
  %12 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !251 ; line:129 col:46
  store i32 0, i32* %base_array_layer.i3, align 4, !dbg !251, !noalias !234 ; line:129 col:46
  %array_layer_count.i2 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 3, !dbg !252 ; line:130 col:29
  %13 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !253 ; line:130 col:47
  store i32 %cond, i32* %array_layer_count.i2, align 4, !dbg !253, !noalias !234 ; line:130 col:47
  %MinLodClamp.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 5, !dbg !254 ; line:131 col:12
  %14 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !255 ; line:131 col:24
  store float 0.000000e+00, float* %MinLodClamp.i, align 4, !dbg !255, !noalias !234 ; line:131 col:24
  %ComponentMapping.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 6, !dbg !256 ; line:132 col:12
  %15 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !257 ; line:132 col:29
  store i32 50462976, i32* %ComponentMapping.i, align 4, !dbg !257, !noalias !234 ; line:132 col:29
  %16 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !258 ; line:134 col:5
  %17 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 0, !dbg !258 ; line:134 col:5
  %18 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 0, !dbg !258 ; line:134 col:5
  %19 = load i32, i32* %18, !dbg !258 ; line:134 col:5
  store i32 %19, i32* %17, !dbg !258 ; line:134 col:5
  %20 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 1, !dbg !258 ; line:134 col:5
  %21 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 1, !dbg !258 ; line:134 col:5
  %22 = load i32, i32* %21, !dbg !258 ; line:134 col:5
  store i32 %22, i32* %20, !dbg !258 ; line:134 col:5
  %23 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 2, !dbg !258 ; line:134 col:5
  %24 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 2, !dbg !258 ; line:134 col:5
  %25 = load i32, i32* %24, !dbg !258 ; line:134 col:5
  store i32 %25, i32* %23, !dbg !258 ; line:134 col:5
  %26 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 3, !dbg !258 ; line:134 col:5
  %27 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 3, !dbg !258 ; line:134 col:5
  %28 = load i32, i32* %27, !dbg !258 ; line:134 col:5
  store i32 %28, i32* %26, !dbg !258 ; line:134 col:5
  %29 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 4, !dbg !258 ; line:134 col:5
  %30 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, !dbg !258 ; line:134 col:5
  %31 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %29, i32 0, i32 0, !dbg !258 ; line:134 col:5
  %32 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %30, i32 0, i32 0, !dbg !258 ; line:134 col:5
  %33 = load i16, i16* %32, !dbg !258 ; line:134 col:5
  store i16 %33, i16* %31, !dbg !258 ; line:134 col:5
  %34 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %29, i32 0, i32 1, !dbg !258 ; line:134 col:5
  %35 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %30, i32 0, i32 1, !dbg !258 ; line:134 col:5
  %36 = load i16, i16* %35, !dbg !258 ; line:134 col:5
  store i16 %36, i16* %34, !dbg !258 ; line:134 col:5
  %37 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %29, i32 0, i32 2, !dbg !258 ; line:134 col:5
  %38 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %30, i32 0, i32 2, !dbg !258 ; line:134 col:5
  %39 = load i32, i32* %38, !dbg !258 ; line:134 col:5
  store i32 %39, i32* %37, !dbg !258 ; line:134 col:5
  %40 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %29, i32 0, i32 3, !dbg !258 ; line:134 col:5
  %41 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %30, i32 0, i32 3, !dbg !258 ; line:134 col:5
  %42 = load i32, i32* %41, !dbg !258 ; line:134 col:5
  store i32 %42, i32* %40, !dbg !258 ; line:134 col:5
  %43 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 5, !dbg !258 ; line:134 col:5
  %44 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 5, !dbg !258 ; line:134 col:5
  %45 = load float, float* %44, !dbg !258 ; line:134 col:5
  store float %45, float* %43, !dbg !258 ; line:134 col:5
  %46 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 6, !dbg !258 ; line:134 col:5
  %47 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 6, !dbg !258 ; line:134 col:5
  %48 = load i32, i32* %47, !dbg !258 ; line:134 col:5
  store i32 %48, i32* %46, !dbg !258 ; line:134 col:5
  %49 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !259 ; line:154 col:5
  ret void, !dbg !259 ; line:154 col:5
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind
define void @rpsl_M_renderer_Fn_main(%struct.texture* %backbuffer, %struct.texture* %texture_aces, i1 zeroext %hdr, <2 x i32> %in_game_viewport_size, %class.matrix.float.4.4 %camera_projection, i32 %game_color_sample_count) #0 {
entry:
  call void @___rpsl_block_marker(i32 0, i32 0, i32 4, i32 4, i32 -1, i32 0, i32 -1)
  %0 = alloca %struct.texture
  %1 = alloca %struct.texture
  %2 = alloca %struct.texture
  %3 = alloca %struct.texture
  %4 = alloca %struct.texture
  %5 = alloca %struct.texture
  %6 = alloca %struct.texture
  %7 = alloca %struct.texture
  %8 = alloca %struct.texture
  %backbuffer_desc = alloca %struct.ResourceDesc, align 4
  %game_color_rt = alloca %struct.texture, align 4
  %agg.tmp = alloca %struct.texture, align 4
  %game_color_rt_msaa = alloca %struct.texture, align 4
  %agg.tmp3 = alloca %struct.texture, align 4
  %game_depth_rt_msaa = alloca %struct.texture, align 4
  %agg.tmp6 = alloca %struct.texture, align 4
  %game_color_rt_pre_display_map = alloca %struct.texture, align 4
  %agg.tmp13 = alloca %struct.texture, align 4
  %9 = getelementptr inbounds %struct.texture, %struct.texture* %4, i32 0, i32 0
  %10 = getelementptr inbounds %struct.texture, %struct.texture* %backbuffer, i32 0, i32 0
  %11 = load i32, i32* %10
  store i32 %11, i32* %9
  %12 = getelementptr inbounds %struct.texture, %struct.texture* %4, i32 0, i32 1
  %13 = getelementptr inbounds %struct.texture, %struct.texture* %backbuffer, i32 0, i32 1
  %14 = load i32, i32* %13
  store i32 %14, i32* %12
  %15 = getelementptr inbounds %struct.texture, %struct.texture* %4, i32 0, i32 2
  %16 = getelementptr inbounds %struct.texture, %struct.texture* %backbuffer, i32 0, i32 2
  %17 = load i32, i32* %16
  store i32 %17, i32* %15
  %18 = getelementptr inbounds %struct.texture, %struct.texture* %4, i32 0, i32 3
  %19 = getelementptr inbounds %struct.texture, %struct.texture* %backbuffer, i32 0, i32 3
  %20 = load i32, i32* %19
  store i32 %20, i32* %18
  %21 = getelementptr inbounds %struct.texture, %struct.texture* %4, i32 0, i32 4
  %22 = getelementptr inbounds %struct.texture, %struct.texture* %backbuffer, i32 0, i32 4
  %23 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %21, i32 0, i32 0
  %24 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %22, i32 0, i32 0
  %25 = load i16, i16* %24
  store i16 %25, i16* %23
  %26 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %21, i32 0, i32 1
  %27 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %22, i32 0, i32 1
  %28 = load i16, i16* %27
  store i16 %28, i16* %26
  %29 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %21, i32 0, i32 2
  %30 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %22, i32 0, i32 2
  %31 = load i32, i32* %30
  store i32 %31, i32* %29
  %32 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %21, i32 0, i32 3
  %33 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %22, i32 0, i32 3
  %34 = load i32, i32* %33
  store i32 %34, i32* %32
  %35 = getelementptr inbounds %struct.texture, %struct.texture* %4, i32 0, i32 5
  %36 = getelementptr inbounds %struct.texture, %struct.texture* %backbuffer, i32 0, i32 5
  %37 = load float, float* %36
  store float %37, float* %35
  %38 = getelementptr inbounds %struct.texture, %struct.texture* %4, i32 0, i32 6
  %39 = getelementptr inbounds %struct.texture, %struct.texture* %backbuffer, i32 0, i32 6
  %40 = load i32, i32* %39
  store i32 %40, i32* %38
  call void @llvm.dbg.value(metadata i32 %game_color_sample_count, i64 0, metadata !260, metadata !215), !dbg !261 ; var:"game_color_sample_count" !DIExpression() func:"main"
  call void @llvm.dbg.value(metadata <2 x i32> %in_game_viewport_size, i64 0, metadata !262, metadata !215), !dbg !263 ; var:"in_game_viewport_size" !DIExpression() func:"main"
  call void @llvm.dbg.declare(metadata %struct.texture* %texture_aces, metadata !264, metadata !215), !dbg !265 ; var:"texture_aces" !DIExpression() func:"main"
  call void @llvm.dbg.declare(metadata %struct.texture* %4, metadata !266, metadata !215), !dbg !267 ; var:"backbuffer" !DIExpression() func:"main"
  call void @llvm.dbg.declare(metadata %struct.ResourceDesc* %backbuffer_desc, metadata !268, metadata !215), !dbg !269 ; var:"backbuffer_desc" !DIExpression() func:"main"
  %41 = bitcast %struct.texture* %4 to i32*, !dbg !270 ; line:16 col:33
  %42 = bitcast %struct.ResourceDesc* %backbuffer_desc to i8*, !dbg !270 ; line:16 col:33
  call void @___rpsl_describe_handle(i8* %42, i32 36, i32* %41, i32 1), !dbg !270 ; line:16 col:33
  %43 = extractelement <2 x i32> %in_game_viewport_size, i64 0, !dbg !271 ; line:20 col:34
  %IMax = call i32 @___rpsl_dxop_binary_i32(i32 37, i32 %43, i32 1), !dbg !271 ; line:20 col:34
  %44 = extractelement <2 x i32> %in_game_viewport_size, i64 1, !dbg !271 ; line:20 col:34
  %IMax219 = call i32 @___rpsl_dxop_binary_i32(i32 37, i32 %44, i32 1), !dbg !271 ; line:20 col:34
  %45 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !272 ; line:20 col:13
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !273, metadata !275), !dbg !272 ; var:"game_viewport_size" !DIExpression(DW_OP_bit_piece, 0, 32) func:"main"
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !273, metadata !276), !dbg !272 ; var:"game_viewport_size" !DIExpression(DW_OP_bit_piece, 32, 32) func:"main"
  call void @llvm.dbg.declare(metadata %struct.texture* %game_color_rt, metadata !277, metadata !215), !dbg !278 ; var:"game_color_rt" !DIExpression() func:"main"
  %46 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !279 ; line:22 col:26
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !280, metadata !215), !dbg !281 ; var:"flags" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !283, metadata !215), !dbg !284 ; var:"sampleQuality" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !285, metadata !215), !dbg !286 ; var:"sampleCount" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !287, metadata !215), !dbg !288 ; var:"numTemporalLayers" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !289, metadata !215), !dbg !290 ; var:"arraySlices" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !291, metadata !215), !dbg !292 ; var:"numMips" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !293, metadata !215), !dbg !294 ; var:"height" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !295, metadata !215), !dbg !296 ; var:"width" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 10, i64 0, metadata !297, metadata !215), !dbg !298 ; var:"format" !DIExpression() func:"create_tex2d"
  %47 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !299 ; line:229 col:15
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !300, metadata !215), !dbg !301 ; var:"sampleCount" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !303, metadata !215), !dbg !304 ; var:"depth" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !305, metadata !215), !dbg !306 ; var:"height" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !307, metadata !215), !dbg !308 ; var:"width" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !309, metadata !215), !dbg !310 ; var:"inMipLevel" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.declare(metadata %struct.texture* %2, metadata !222, metadata !215), !dbg !311 ; var:"result" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.declare(metadata %struct.texture* %1, metadata !222, metadata !215), !dbg !314 ; var:"result" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.declare(metadata %struct.texture* %0, metadata !222, metadata !215), !dbg !317 ; var:"result" !DIExpression() func:"make_default_texture_view"
  br label %if.end.i.i, !dbg !320 ; line:98 col:9

if.end.i.i:                                       ; preds = %entry
  %48 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !321 ; line:103 col:14
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !322, metadata !215), !dbg !321 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %49 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !324 ; line:104 col:14
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !325, metadata !215), !dbg !324 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %50 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !326 ; line:105 col:14
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !327, metadata !215), !dbg !326 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %51 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !328 ; line:106 col:14
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !329, metadata !215), !dbg !328 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  br label %while.cond.i.i, !dbg !330 ; line:108 col:5

while.cond.i.i:                                   ; preds = %while.body.i.i, %if.end.i.i
  %w.i.i.0 = phi i32 [ %IMax, %if.end.i.i ], [ %shr.i.i, %while.body.i.i ]
  %h.i.i.0 = phi i32 [ %IMax219, %if.end.i.i ], [ %shr12.i.i, %while.body.i.i ]
  %d.i.i.0 = phi i32 [ 1, %if.end.i.i ], [ %shr13.i.i, %while.body.i.i ]
  %mips.i.i.0 = phi i32 [ 1, %if.end.i.i ], [ %inc.i.i, %while.body.i.i ]
  call void @llvm.dbg.value(metadata i32 %h.i.i.0, i64 0, metadata !325, metadata !215), !dbg !324 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %mips.i.i.0, i64 0, metadata !329, metadata !215), !dbg !328 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %w.i.i.0, i64 0, metadata !322, metadata !215), !dbg !321 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %d.i.i.0, i64 0, metadata !327, metadata !215), !dbg !326 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %cmp2.i.i = icmp ugt i32 %w.i.i.0, 1, !dbg !331 ; line:108 col:15
  br i1 %cmp2.i.i, label %lor.end.i.i, label %lor.lhs.false.i.i, !dbg !332 ; line:108 col:20

lor.lhs.false.i.i:                                ; preds = %while.cond.i.i
  %cmp5.i.i = icmp ugt i32 %h.i.i.0, 1, !dbg !333 ; line:108 col:26
  br i1 %cmp5.i.i, label %lor.end.i.i, label %lor.rhs.i.i, !dbg !334 ; line:108 col:31

lor.rhs.i.i:                                      ; preds = %lor.lhs.false.i.i
  %cmp8.i.i = icmp ugt i32 %d.i.i.0, 1, !dbg !335 ; line:108 col:37
  br label %lor.end.i.i, !dbg !334 ; line:108 col:31

lor.end.i.i:                                      ; preds = %lor.rhs.i.i, %lor.lhs.false.i.i, %while.cond.i.i
  %52 = phi i1 [ true, %lor.lhs.false.i.i ], [ true, %while.cond.i.i ], [ %cmp8.i.i, %lor.rhs.i.i ], !dbg !299 ; line:229 col:15
  br i1 %52, label %while.body.i.i, label %while.end.i.i, !dbg !330 ; line:108 col:5

while.body.i.i:                                   ; preds = %lor.end.i.i
  %inc.i.i = add i32 %mips.i.i.0, 1, !dbg !336 ; line:110 col:13
  %53 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !336 ; line:110 col:13
  call void @llvm.dbg.value(metadata i32 %inc.i.i, i64 0, metadata !329, metadata !215), !dbg !328 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr.i.i = lshr i32 %w.i.i.0, 1, !dbg !338 ; line:111 col:15
  %54 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !339 ; line:111 col:11
  call void @llvm.dbg.value(metadata i32 %shr.i.i, i64 0, metadata !322, metadata !215), !dbg !321 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr12.i.i = lshr i32 %h.i.i.0, 1, !dbg !340 ; line:112 col:15
  %55 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !341 ; line:112 col:11
  call void @llvm.dbg.value(metadata i32 %shr12.i.i, i64 0, metadata !325, metadata !215), !dbg !324 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr13.i.i = lshr i32 %d.i.i.0, 1, !dbg !342 ; line:113 col:15
  %56 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !343 ; line:113 col:11
  call void @llvm.dbg.value(metadata i32 %shr13.i.i, i64 0, metadata !327, metadata !215), !dbg !326 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  br label %while.cond.i.i, !dbg !330 ; line:108 col:5

while.end.i.i:                                    ; preds = %lor.end.i.i
  %mips.i.i.0.lcssa = phi i32 [ %mips.i.i.0, %lor.end.i.i ]
  br label %cond.false.i.i, !dbg !344 ; line:116 col:12

cond.false.i.i:                                   ; preds = %while.end.i.i
  %UMin222 = call i32 @___rpsl_dxop_binary_i32(i32 40, i32 1, i32 %mips.i.i.0.lcssa), !dbg !345 ; line:116 col:39
  br label %cond.end.i.i, !dbg !344 ; line:116 col:12

cond.end.i.i:                                     ; preds = %cond.false.i.i
  %cond.i.i = phi i32 [ %UMin222, %cond.false.i.i ], !dbg !344 ; line:116 col:12
  br label %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit", !dbg !346 ; line:116 col:5

"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit": ; preds = %cond.end.i.i
  %retval.i.i.0 = phi i32 [ %cond.i.i, %cond.end.i.i ]
  %57 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !347 ; line:117 col:1
  call void @llvm.dbg.value(metadata i32 %retval.i.i.0, i64 0, metadata !291, metadata !215), !dbg !292 ; var:"numMips" !DIExpression() func:"create_tex2d"
  %call1.i = call i32 @___rpsl_create_resource(i32 3, i32 0, i32 10, i32 %IMax, i32 %IMax219, i32 1, i32 %retval.i.i.0, i32 1, i32 0, i32 1, i32 0) #0, !dbg !348, !noalias !349 ; line:231 col:30
  %58 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !352 ; line:231 col:16
  call void @llvm.dbg.value(metadata i32 %call1.i, i64 0, metadata !353, metadata !215), !dbg !352 ; var:"resourceHdl" !DIExpression() func:"create_tex2d"
  %59 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !355 ; line:233 col:12
  call void @llvm.dbg.value(metadata i32 %retval.i.i.0, i64 0, metadata !228, metadata !215), !dbg !356 ; var:"numMips" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !230, metadata !215), !dbg !358 ; var:"arraySlices" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 %call1.i, i64 0, metadata !232, metadata !215), !dbg !359 ; var:"resourceHdl" !DIExpression() func:"make_default_texture_view"
  %60 = bitcast %struct.texture* %3 to i8*, !dbg !355 ; line:233 col:12
  call void @llvm.memset.p0i8.i32(i8* %60, i8 0, i32 36, i32 4, i1 false) #0, !dbg !355, !noalias !360 ; line:233 col:12
  %Resource.i.i = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 0, !dbg !363 ; line:123 col:12
  %61 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !364 ; line:123 col:21
  store i32 %call1.i, i32* %Resource.i.i, align 4, !dbg !364, !noalias !360 ; line:123 col:21
  %ViewFormat.i.i = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 1, !dbg !365 ; line:124 col:12
  %62 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !366 ; line:124 col:23
  store i32 0, i32* %ViewFormat.i.i, align 4, !dbg !366, !noalias !360 ; line:124 col:23
  %TemporalLayer.i.i = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 2, !dbg !367 ; line:125 col:12
  %63 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !368 ; line:125 col:26
  store i32 0, i32* %TemporalLayer.i.i, align 4, !dbg !368, !noalias !360 ; line:125 col:26
  %Flags.i.i = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 3, !dbg !369 ; line:126 col:12
  %64 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !370 ; line:126 col:18
  store i32 0, i32* %Flags.i.i, align 4, !dbg !370, !noalias !360 ; line:126 col:18
  %base_mip_level.i.i217 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 4, i32 0, !dbg !371 ; line:127 col:29
  %65 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !372 ; line:127 col:44
  store i16 0, i16* %base_mip_level.i.i217, align 2, !dbg !372, !noalias !360 ; line:127 col:44
  %conv.i.i = trunc i32 %retval.i.i.0 to i16, !dbg !373 ; line:128 col:56
  %mip_level_count.i.i216 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 4, i32 1, !dbg !374 ; line:128 col:29
  %66 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !375 ; line:128 col:45
  store i16 %conv.i.i, i16* %mip_level_count.i.i216, align 2, !dbg !375, !noalias !360 ; line:128 col:45
  %base_array_layer.i.i215 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 4, i32 2, !dbg !376 ; line:129 col:29
  %67 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !377 ; line:129 col:46
  store i32 0, i32* %base_array_layer.i.i215, align 4, !dbg !377, !noalias !360 ; line:129 col:46
  %array_layer_count.i.i214 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 4, i32 3, !dbg !378 ; line:130 col:29
  %68 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !379 ; line:130 col:47
  store i32 1, i32* %array_layer_count.i.i214, align 4, !dbg !379, !noalias !360 ; line:130 col:47
  %MinLodClamp.i.i = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 5, !dbg !380 ; line:131 col:12
  %69 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !381 ; line:131 col:24
  store float 0.000000e+00, float* %MinLodClamp.i.i, align 4, !dbg !381, !noalias !360 ; line:131 col:24
  %ComponentMapping.i.i = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 6, !dbg !382 ; line:132 col:12
  %70 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !383 ; line:132 col:29
  store i32 50462976, i32* %ComponentMapping.i.i, align 4, !dbg !383, !noalias !360 ; line:132 col:29
  %71 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !384 ; line:134 col:5
  %72 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 0, !dbg !384 ; line:134 col:5
  %73 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 0, !dbg !384 ; line:134 col:5
  %74 = load i32, i32* %73, !dbg !384 ; line:134 col:5
  store i32 %74, i32* %72, !dbg !384 ; line:134 col:5
  %75 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 1, !dbg !384 ; line:134 col:5
  %76 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 1, !dbg !384 ; line:134 col:5
  %77 = load i32, i32* %76, !dbg !384 ; line:134 col:5
  store i32 %77, i32* %75, !dbg !384 ; line:134 col:5
  %78 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 2, !dbg !384 ; line:134 col:5
  %79 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 2, !dbg !384 ; line:134 col:5
  %80 = load i32, i32* %79, !dbg !384 ; line:134 col:5
  store i32 %80, i32* %78, !dbg !384 ; line:134 col:5
  %81 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 3, !dbg !384 ; line:134 col:5
  %82 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 3, !dbg !384 ; line:134 col:5
  %83 = load i32, i32* %82, !dbg !384 ; line:134 col:5
  store i32 %83, i32* %81, !dbg !384 ; line:134 col:5
  %84 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 4, !dbg !384 ; line:134 col:5
  %85 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 4, !dbg !384 ; line:134 col:5
  %86 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %84, i32 0, i32 0, !dbg !384 ; line:134 col:5
  %87 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %85, i32 0, i32 0, !dbg !384 ; line:134 col:5
  %88 = load i16, i16* %87, !dbg !384 ; line:134 col:5
  store i16 %88, i16* %86, !dbg !384 ; line:134 col:5
  %89 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %84, i32 0, i32 1, !dbg !384 ; line:134 col:5
  %90 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %85, i32 0, i32 1, !dbg !384 ; line:134 col:5
  %91 = load i16, i16* %90, !dbg !384 ; line:134 col:5
  store i16 %91, i16* %89, !dbg !384 ; line:134 col:5
  %92 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %84, i32 0, i32 2, !dbg !384 ; line:134 col:5
  %93 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %85, i32 0, i32 2, !dbg !384 ; line:134 col:5
  %94 = load i32, i32* %93, !dbg !384 ; line:134 col:5
  store i32 %94, i32* %92, !dbg !384 ; line:134 col:5
  %95 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %84, i32 0, i32 3, !dbg !384 ; line:134 col:5
  %96 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %85, i32 0, i32 3, !dbg !384 ; line:134 col:5
  %97 = load i32, i32* %96, !dbg !384 ; line:134 col:5
  store i32 %97, i32* %95, !dbg !384 ; line:134 col:5
  %98 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 5, !dbg !384 ; line:134 col:5
  %99 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 5, !dbg !384 ; line:134 col:5
  %100 = load float, float* %99, !dbg !384 ; line:134 col:5
  store float %100, float* %98, !dbg !384 ; line:134 col:5
  %101 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 6, !dbg !384 ; line:134 col:5
  %102 = getelementptr inbounds %struct.texture, %struct.texture* %3, i32 0, i32 6, !dbg !384 ; line:134 col:5
  %103 = load i32, i32* %102, !dbg !384 ; line:134 col:5
  store i32 %103, i32* %101, !dbg !384 ; line:134 col:5
  %104 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !385 ; line:233 col:5
  %105 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !279 ; line:22 col:26
  %106 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 0, !dbg !279 ; line:22 col:26
  %107 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 0, !dbg !279 ; line:22 col:26
  %108 = load i32, i32* %107, !dbg !279 ; line:22 col:26
  store i32 %108, i32* %106, !dbg !279 ; line:22 col:26
  %109 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 1, !dbg !279 ; line:22 col:26
  %110 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 1, !dbg !279 ; line:22 col:26
  %111 = load i32, i32* %110, !dbg !279 ; line:22 col:26
  store i32 %111, i32* %109, !dbg !279 ; line:22 col:26
  %112 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 2, !dbg !279 ; line:22 col:26
  %113 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 2, !dbg !279 ; line:22 col:26
  %114 = load i32, i32* %113, !dbg !279 ; line:22 col:26
  store i32 %114, i32* %112, !dbg !279 ; line:22 col:26
  %115 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 3, !dbg !279 ; line:22 col:26
  %116 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 3, !dbg !279 ; line:22 col:26
  %117 = load i32, i32* %116, !dbg !279 ; line:22 col:26
  store i32 %117, i32* %115, !dbg !279 ; line:22 col:26
  %118 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 4, !dbg !279 ; line:22 col:26
  %119 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 4, !dbg !279 ; line:22 col:26
  %120 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %118, i32 0, i32 0, !dbg !279 ; line:22 col:26
  %121 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %119, i32 0, i32 0, !dbg !279 ; line:22 col:26
  %122 = load i16, i16* %121, !dbg !279 ; line:22 col:26
  store i16 %122, i16* %120, !dbg !279 ; line:22 col:26
  %123 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %118, i32 0, i32 1, !dbg !279 ; line:22 col:26
  %124 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %119, i32 0, i32 1, !dbg !279 ; line:22 col:26
  %125 = load i16, i16* %124, !dbg !279 ; line:22 col:26
  store i16 %125, i16* %123, !dbg !279 ; line:22 col:26
  %126 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %118, i32 0, i32 2, !dbg !279 ; line:22 col:26
  %127 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %119, i32 0, i32 2, !dbg !279 ; line:22 col:26
  %128 = load i32, i32* %127, !dbg !279 ; line:22 col:26
  store i32 %128, i32* %126, !dbg !279 ; line:22 col:26
  %129 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %118, i32 0, i32 3, !dbg !279 ; line:22 col:26
  %130 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %119, i32 0, i32 3, !dbg !279 ; line:22 col:26
  %131 = load i32, i32* %130, !dbg !279 ; line:22 col:26
  store i32 %131, i32* %129, !dbg !279 ; line:22 col:26
  %132 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 5, !dbg !279 ; line:22 col:26
  %133 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 5, !dbg !279 ; line:22 col:26
  %134 = load float, float* %133, !dbg !279 ; line:22 col:26
  store float %134, float* %132, !dbg !279 ; line:22 col:26
  %135 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 6, !dbg !279 ; line:22 col:26
  %136 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp, i32 0, i32 6, !dbg !279 ; line:22 col:26
  %137 = load i32, i32* %136, !dbg !279 ; line:22 col:26
  store i32 %137, i32* %135, !dbg !279 ; line:22 col:26
  %138 = getelementptr %struct.texture, %struct.texture* %8, i32 0, i32 0, !dbg !279 ; line:22 col:26
  %139 = load i32, i32* %138, !dbg !279 ; line:22 col:26
  call void @___rpsl_name_resource(i32 %139, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @"@@rps_Str0", i32 0, i32 0), i32 13), !dbg !279 ; line:22 col:26
  %140 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !279 ; line:22 col:26
  %141 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt, i32 0, i32 0, !dbg !279 ; line:22 col:26
  %142 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 0, !dbg !279 ; line:22 col:26
  %143 = load i32, i32* %142, !dbg !279 ; line:22 col:26
  store i32 %143, i32* %141, !dbg !279 ; line:22 col:26
  %144 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt, i32 0, i32 1, !dbg !279 ; line:22 col:26
  %145 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 1, !dbg !279 ; line:22 col:26
  %146 = load i32, i32* %145, !dbg !279 ; line:22 col:26
  store i32 %146, i32* %144, !dbg !279 ; line:22 col:26
  %147 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt, i32 0, i32 2, !dbg !279 ; line:22 col:26
  %148 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 2, !dbg !279 ; line:22 col:26
  %149 = load i32, i32* %148, !dbg !279 ; line:22 col:26
  store i32 %149, i32* %147, !dbg !279 ; line:22 col:26
  %150 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt, i32 0, i32 3, !dbg !279 ; line:22 col:26
  %151 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 3, !dbg !279 ; line:22 col:26
  %152 = load i32, i32* %151, !dbg !279 ; line:22 col:26
  store i32 %152, i32* %150, !dbg !279 ; line:22 col:26
  %153 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt, i32 0, i32 4, !dbg !279 ; line:22 col:26
  %154 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 4, !dbg !279 ; line:22 col:26
  %155 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %153, i32 0, i32 0, !dbg !279 ; line:22 col:26
  %156 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %154, i32 0, i32 0, !dbg !279 ; line:22 col:26
  %157 = load i16, i16* %156, !dbg !279 ; line:22 col:26
  store i16 %157, i16* %155, !dbg !279 ; line:22 col:26
  %158 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %153, i32 0, i32 1, !dbg !279 ; line:22 col:26
  %159 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %154, i32 0, i32 1, !dbg !279 ; line:22 col:26
  %160 = load i16, i16* %159, !dbg !279 ; line:22 col:26
  store i16 %160, i16* %158, !dbg !279 ; line:22 col:26
  %161 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %153, i32 0, i32 2, !dbg !279 ; line:22 col:26
  %162 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %154, i32 0, i32 2, !dbg !279 ; line:22 col:26
  %163 = load i32, i32* %162, !dbg !279 ; line:22 col:26
  store i32 %163, i32* %161, !dbg !279 ; line:22 col:26
  %164 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %153, i32 0, i32 3, !dbg !279 ; line:22 col:26
  %165 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %154, i32 0, i32 3, !dbg !279 ; line:22 col:26
  %166 = load i32, i32* %165, !dbg !279 ; line:22 col:26
  store i32 %166, i32* %164, !dbg !279 ; line:22 col:26
  %167 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt, i32 0, i32 5, !dbg !279 ; line:22 col:26
  %168 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 5, !dbg !279 ; line:22 col:26
  %169 = load float, float* %168, !dbg !279 ; line:22 col:26
  store float %169, float* %167, !dbg !279 ; line:22 col:26
  %170 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt, i32 0, i32 6, !dbg !279 ; line:22 col:26
  %171 = getelementptr inbounds %struct.texture, %struct.texture* %8, i32 0, i32 6, !dbg !279 ; line:22 col:26
  %172 = load i32, i32* %171, !dbg !279 ; line:22 col:26
  store i32 %172, i32* %170, !dbg !279 ; line:22 col:26
  call void @llvm.dbg.declare(metadata %struct.texture* %game_color_rt_msaa, metadata !386, metadata !215), !dbg !387 ; var:"game_color_rt_msaa" !DIExpression() func:"main"
  %173 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !388 ; line:25 col:31
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !280, metadata !215), !dbg !389 ; var:"flags" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !283, metadata !215), !dbg !390 ; var:"sampleQuality" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 %game_color_sample_count, i64 0, metadata !285, metadata !215), !dbg !391 ; var:"sampleCount" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !287, metadata !215), !dbg !392 ; var:"numTemporalLayers" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !289, metadata !215), !dbg !393 ; var:"arraySlices" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 2, i64 0, metadata !291, metadata !215), !dbg !394 ; var:"numMips" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !293, metadata !215), !dbg !395 ; var:"height" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !295, metadata !215), !dbg !396 ; var:"width" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 10, i64 0, metadata !297, metadata !215), !dbg !397 ; var:"format" !DIExpression() func:"create_tex2d"
  %174 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !398 ; line:229 col:15
  call void @llvm.dbg.value(metadata i32 %game_color_sample_count, i64 0, metadata !300, metadata !215), !dbg !399 ; var:"sampleCount" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !303, metadata !215), !dbg !401 ; var:"depth" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !305, metadata !215), !dbg !402 ; var:"height" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !307, metadata !215), !dbg !403 ; var:"width" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 2, i64 0, metadata !309, metadata !215), !dbg !404 ; var:"inMipLevel" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %cmp.i.i.40 = icmp ugt i32 %game_color_sample_count, 1, !dbg !405 ; line:98 col:21
  br i1 %cmp.i.i.40, label %if.then.i.i.41, label %if.end.i.i.42, !dbg !407 ; line:98 col:9

if.then.i.i.41:                                   ; preds = %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit"
  br label %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.77", !dbg !408 ; line:100 col:9

if.end.i.i.42:                                    ; preds = %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit"
  %175 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !410 ; line:103 col:14
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !322, metadata !215), !dbg !410 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %176 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !411 ; line:104 col:14
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !325, metadata !215), !dbg !411 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %177 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !412 ; line:105 col:14
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !327, metadata !215), !dbg !412 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %178 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !413 ; line:106 col:14
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !329, metadata !215), !dbg !413 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  br label %while.cond.i.i.44, !dbg !414 ; line:108 col:5

while.cond.i.i.44:                                ; preds = %while.body.i.i.54, %if.end.i.i.42
  %w.i.i.26.0 = phi i32 [ %IMax, %if.end.i.i.42 ], [ %shr.i.i.51, %while.body.i.i.54 ]
  %h.i.i.27.0 = phi i32 [ %IMax219, %if.end.i.i.42 ], [ %shr12.i.i.52, %while.body.i.i.54 ]
  %d.i.i.28.0 = phi i32 [ 1, %if.end.i.i.42 ], [ %shr13.i.i.53, %while.body.i.i.54 ]
  %mips.i.i.29.0 = phi i32 [ 1, %if.end.i.i.42 ], [ %inc.i.i.50, %while.body.i.i.54 ]
  call void @llvm.dbg.value(metadata i32 %w.i.i.26.0, i64 0, metadata !322, metadata !215), !dbg !410 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %mips.i.i.29.0, i64 0, metadata !329, metadata !215), !dbg !413 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %h.i.i.27.0, i64 0, metadata !325, metadata !215), !dbg !411 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %d.i.i.28.0, i64 0, metadata !327, metadata !215), !dbg !412 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %cmp2.i.i.43 = icmp ugt i32 %w.i.i.26.0, 1, !dbg !415 ; line:108 col:15
  br i1 %cmp2.i.i.43, label %lor.end.i.i.49, label %lor.lhs.false.i.i.46, !dbg !416 ; line:108 col:20

lor.lhs.false.i.i.46:                             ; preds = %while.cond.i.i.44
  %cmp5.i.i.45 = icmp ugt i32 %h.i.i.27.0, 1, !dbg !417 ; line:108 col:26
  br i1 %cmp5.i.i.45, label %lor.end.i.i.49, label %lor.rhs.i.i.48, !dbg !418 ; line:108 col:31

lor.rhs.i.i.48:                                   ; preds = %lor.lhs.false.i.i.46
  %cmp8.i.i.47 = icmp ugt i32 %d.i.i.28.0, 1, !dbg !419 ; line:108 col:37
  br label %lor.end.i.i.49, !dbg !418 ; line:108 col:31

lor.end.i.i.49:                                   ; preds = %lor.rhs.i.i.48, %lor.lhs.false.i.i.46, %while.cond.i.i.44
  %179 = phi i1 [ true, %lor.lhs.false.i.i.46 ], [ true, %while.cond.i.i.44 ], [ %cmp8.i.i.47, %lor.rhs.i.i.48 ], !dbg !398 ; line:229 col:15
  br i1 %179, label %while.body.i.i.54, label %while.end.i.i.56, !dbg !414 ; line:108 col:5

while.body.i.i.54:                                ; preds = %lor.end.i.i.49
  %inc.i.i.50 = add i32 %mips.i.i.29.0, 1, !dbg !420 ; line:110 col:13
  %180 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !420 ; line:110 col:13
  call void @llvm.dbg.value(metadata i32 %inc.i.i.50, i64 0, metadata !329, metadata !215), !dbg !413 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr.i.i.51 = lshr i32 %w.i.i.26.0, 1, !dbg !421 ; line:111 col:15
  %181 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !422 ; line:111 col:11
  call void @llvm.dbg.value(metadata i32 %shr.i.i.51, i64 0, metadata !322, metadata !215), !dbg !410 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr12.i.i.52 = lshr i32 %h.i.i.27.0, 1, !dbg !423 ; line:112 col:15
  %182 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !424 ; line:112 col:11
  call void @llvm.dbg.value(metadata i32 %shr12.i.i.52, i64 0, metadata !325, metadata !215), !dbg !411 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr13.i.i.53 = lshr i32 %d.i.i.28.0, 1, !dbg !425 ; line:113 col:15
  %183 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !426 ; line:113 col:11
  call void @llvm.dbg.value(metadata i32 %shr13.i.i.53, i64 0, metadata !327, metadata !215), !dbg !412 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  br label %while.cond.i.i.44, !dbg !414 ; line:108 col:5

while.end.i.i.56:                                 ; preds = %lor.end.i.i.49
  %mips.i.i.29.0.lcssa = phi i32 [ %mips.i.i.29.0, %lor.end.i.i.49 ]
  br label %cond.false.i.i.58, !dbg !427 ; line:116 col:12

cond.false.i.i.58:                                ; preds = %while.end.i.i.56
  %UMin221 = call i32 @___rpsl_dxop_binary_i32(i32 40, i32 2, i32 %mips.i.i.29.0.lcssa), !dbg !428 ; line:116 col:39
  br label %cond.end.i.i.60, !dbg !427 ; line:116 col:12

cond.end.i.i.60:                                  ; preds = %cond.false.i.i.58
  %cond.i.i.59 = phi i32 [ %UMin221, %cond.false.i.i.58 ], !dbg !427 ; line:116 col:12
  br label %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.77", !dbg !429 ; line:116 col:5

"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.77": ; preds = %cond.end.i.i.60, %if.then.i.i.41
  %retval.i.i.20.0 = phi i32 [ 1, %if.then.i.i.41 ], [ %cond.i.i.59, %cond.end.i.i.60 ]
  %184 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !430 ; line:117 col:1
  call void @llvm.dbg.value(metadata i32 %retval.i.i.20.0, i64 0, metadata !291, metadata !215), !dbg !394 ; var:"numMips" !DIExpression() func:"create_tex2d"
  %call1.i.61 = call i32 @___rpsl_create_resource(i32 3, i32 0, i32 10, i32 %IMax, i32 %IMax219, i32 1, i32 %retval.i.i.20.0, i32 %game_color_sample_count, i32 0, i32 1, i32 1) #0, !dbg !431, !noalias !432 ; line:231 col:30
  %185 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !435 ; line:231 col:16
  call void @llvm.dbg.value(metadata i32 %call1.i.61, i64 0, metadata !353, metadata !215), !dbg !435 ; var:"resourceHdl" !DIExpression() func:"create_tex2d"
  %186 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !436 ; line:233 col:12
  call void @llvm.dbg.value(metadata i32 %retval.i.i.20.0, i64 0, metadata !228, metadata !215), !dbg !437 ; var:"numMips" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !230, metadata !215), !dbg !438 ; var:"arraySlices" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 %call1.i.61, i64 0, metadata !232, metadata !215), !dbg !439 ; var:"resourceHdl" !DIExpression() func:"make_default_texture_view"
  %187 = bitcast %struct.texture* %2 to i8*, !dbg !436 ; line:233 col:12
  call void @llvm.memset.p0i8.i32(i8* %187, i8 0, i32 36, i32 4, i1 false) #0, !dbg !436, !noalias !440 ; line:233 col:12
  %Resource.i.i.62 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 0, !dbg !443 ; line:123 col:12
  %188 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !444 ; line:123 col:21
  store i32 %call1.i.61, i32* %Resource.i.i.62, align 4, !dbg !444, !noalias !440 ; line:123 col:21
  %ViewFormat.i.i.63 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 1, !dbg !445 ; line:124 col:12
  %189 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !446 ; line:124 col:23
  store i32 0, i32* %ViewFormat.i.i.63, align 4, !dbg !446, !noalias !440 ; line:124 col:23
  %TemporalLayer.i.i.64 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 2, !dbg !447 ; line:125 col:12
  %190 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !448 ; line:125 col:26
  store i32 0, i32* %TemporalLayer.i.i.64, align 4, !dbg !448, !noalias !440 ; line:125 col:26
  %Flags.i.i.65 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 3, !dbg !449 ; line:126 col:12
  %191 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !450 ; line:126 col:18
  store i32 0, i32* %Flags.i.i.65, align 4, !dbg !450, !noalias !440 ; line:126 col:18
  %base_mip_level.i.i.67213 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 4, i32 0, !dbg !451 ; line:127 col:29
  %192 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !452 ; line:127 col:44
  store i16 0, i16* %base_mip_level.i.i.67213, align 2, !dbg !452, !noalias !440 ; line:127 col:44
  %conv.i.i.68 = trunc i32 %retval.i.i.20.0 to i16, !dbg !453 ; line:128 col:56
  %mip_level_count.i.i.70212 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 4, i32 1, !dbg !454 ; line:128 col:29
  %193 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !455 ; line:128 col:45
  store i16 %conv.i.i.68, i16* %mip_level_count.i.i.70212, align 2, !dbg !455, !noalias !440 ; line:128 col:45
  %base_array_layer.i.i.72211 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 4, i32 2, !dbg !456 ; line:129 col:29
  %194 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !457 ; line:129 col:46
  store i32 0, i32* %base_array_layer.i.i.72211, align 4, !dbg !457, !noalias !440 ; line:129 col:46
  %array_layer_count.i.i.74210 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 4, i32 3, !dbg !458 ; line:130 col:29
  %195 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !459 ; line:130 col:47
  store i32 1, i32* %array_layer_count.i.i.74210, align 4, !dbg !459, !noalias !440 ; line:130 col:47
  %MinLodClamp.i.i.75 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 5, !dbg !460 ; line:131 col:12
  %196 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !461 ; line:131 col:24
  store float 0.000000e+00, float* %MinLodClamp.i.i.75, align 4, !dbg !461, !noalias !440 ; line:131 col:24
  %ComponentMapping.i.i.76 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 6, !dbg !462 ; line:132 col:12
  %197 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !463 ; line:132 col:29
  store i32 50462976, i32* %ComponentMapping.i.i.76, align 4, !dbg !463, !noalias !440 ; line:132 col:29
  %198 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !464 ; line:134 col:5
  %199 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 0, !dbg !464 ; line:134 col:5
  %200 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 0, !dbg !464 ; line:134 col:5
  %201 = load i32, i32* %200, !dbg !464 ; line:134 col:5
  store i32 %201, i32* %199, !dbg !464 ; line:134 col:5
  %202 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 1, !dbg !464 ; line:134 col:5
  %203 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 1, !dbg !464 ; line:134 col:5
  %204 = load i32, i32* %203, !dbg !464 ; line:134 col:5
  store i32 %204, i32* %202, !dbg !464 ; line:134 col:5
  %205 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 2, !dbg !464 ; line:134 col:5
  %206 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 2, !dbg !464 ; line:134 col:5
  %207 = load i32, i32* %206, !dbg !464 ; line:134 col:5
  store i32 %207, i32* %205, !dbg !464 ; line:134 col:5
  %208 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 3, !dbg !464 ; line:134 col:5
  %209 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 3, !dbg !464 ; line:134 col:5
  %210 = load i32, i32* %209, !dbg !464 ; line:134 col:5
  store i32 %210, i32* %208, !dbg !464 ; line:134 col:5
  %211 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 4, !dbg !464 ; line:134 col:5
  %212 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 4, !dbg !464 ; line:134 col:5
  %213 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %211, i32 0, i32 0, !dbg !464 ; line:134 col:5
  %214 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %212, i32 0, i32 0, !dbg !464 ; line:134 col:5
  %215 = load i16, i16* %214, !dbg !464 ; line:134 col:5
  store i16 %215, i16* %213, !dbg !464 ; line:134 col:5
  %216 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %211, i32 0, i32 1, !dbg !464 ; line:134 col:5
  %217 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %212, i32 0, i32 1, !dbg !464 ; line:134 col:5
  %218 = load i16, i16* %217, !dbg !464 ; line:134 col:5
  store i16 %218, i16* %216, !dbg !464 ; line:134 col:5
  %219 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %211, i32 0, i32 2, !dbg !464 ; line:134 col:5
  %220 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %212, i32 0, i32 2, !dbg !464 ; line:134 col:5
  %221 = load i32, i32* %220, !dbg !464 ; line:134 col:5
  store i32 %221, i32* %219, !dbg !464 ; line:134 col:5
  %222 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %211, i32 0, i32 3, !dbg !464 ; line:134 col:5
  %223 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %212, i32 0, i32 3, !dbg !464 ; line:134 col:5
  %224 = load i32, i32* %223, !dbg !464 ; line:134 col:5
  store i32 %224, i32* %222, !dbg !464 ; line:134 col:5
  %225 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 5, !dbg !464 ; line:134 col:5
  %226 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 5, !dbg !464 ; line:134 col:5
  %227 = load float, float* %226, !dbg !464 ; line:134 col:5
  store float %227, float* %225, !dbg !464 ; line:134 col:5
  %228 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 6, !dbg !464 ; line:134 col:5
  %229 = getelementptr inbounds %struct.texture, %struct.texture* %2, i32 0, i32 6, !dbg !464 ; line:134 col:5
  %230 = load i32, i32* %229, !dbg !464 ; line:134 col:5
  store i32 %230, i32* %228, !dbg !464 ; line:134 col:5
  %231 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !465 ; line:233 col:5
  %232 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !388 ; line:25 col:31
  %233 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 0, !dbg !388 ; line:25 col:31
  %234 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 0, !dbg !388 ; line:25 col:31
  %235 = load i32, i32* %234, !dbg !388 ; line:25 col:31
  store i32 %235, i32* %233, !dbg !388 ; line:25 col:31
  %236 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 1, !dbg !388 ; line:25 col:31
  %237 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 1, !dbg !388 ; line:25 col:31
  %238 = load i32, i32* %237, !dbg !388 ; line:25 col:31
  store i32 %238, i32* %236, !dbg !388 ; line:25 col:31
  %239 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 2, !dbg !388 ; line:25 col:31
  %240 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 2, !dbg !388 ; line:25 col:31
  %241 = load i32, i32* %240, !dbg !388 ; line:25 col:31
  store i32 %241, i32* %239, !dbg !388 ; line:25 col:31
  %242 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 3, !dbg !388 ; line:25 col:31
  %243 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 3, !dbg !388 ; line:25 col:31
  %244 = load i32, i32* %243, !dbg !388 ; line:25 col:31
  store i32 %244, i32* %242, !dbg !388 ; line:25 col:31
  %245 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 4, !dbg !388 ; line:25 col:31
  %246 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 4, !dbg !388 ; line:25 col:31
  %247 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %245, i32 0, i32 0, !dbg !388 ; line:25 col:31
  %248 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %246, i32 0, i32 0, !dbg !388 ; line:25 col:31
  %249 = load i16, i16* %248, !dbg !388 ; line:25 col:31
  store i16 %249, i16* %247, !dbg !388 ; line:25 col:31
  %250 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %245, i32 0, i32 1, !dbg !388 ; line:25 col:31
  %251 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %246, i32 0, i32 1, !dbg !388 ; line:25 col:31
  %252 = load i16, i16* %251, !dbg !388 ; line:25 col:31
  store i16 %252, i16* %250, !dbg !388 ; line:25 col:31
  %253 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %245, i32 0, i32 2, !dbg !388 ; line:25 col:31
  %254 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %246, i32 0, i32 2, !dbg !388 ; line:25 col:31
  %255 = load i32, i32* %254, !dbg !388 ; line:25 col:31
  store i32 %255, i32* %253, !dbg !388 ; line:25 col:31
  %256 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %245, i32 0, i32 3, !dbg !388 ; line:25 col:31
  %257 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %246, i32 0, i32 3, !dbg !388 ; line:25 col:31
  %258 = load i32, i32* %257, !dbg !388 ; line:25 col:31
  store i32 %258, i32* %256, !dbg !388 ; line:25 col:31
  %259 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 5, !dbg !388 ; line:25 col:31
  %260 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 5, !dbg !388 ; line:25 col:31
  %261 = load float, float* %260, !dbg !388 ; line:25 col:31
  store float %261, float* %259, !dbg !388 ; line:25 col:31
  %262 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 6, !dbg !388 ; line:25 col:31
  %263 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp3, i32 0, i32 6, !dbg !388 ; line:25 col:31
  %264 = load i32, i32* %263, !dbg !388 ; line:25 col:31
  store i32 %264, i32* %262, !dbg !388 ; line:25 col:31
  %265 = getelementptr %struct.texture, %struct.texture* %7, i32 0, i32 0, !dbg !388 ; line:25 col:31
  %266 = load i32, i32* %265, !dbg !388 ; line:25 col:31
  call void @___rpsl_name_resource(i32 %266, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @"@@rps_Str1", i32 0, i32 0), i32 18), !dbg !388 ; line:25 col:31
  %267 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !388 ; line:25 col:31
  %268 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_msaa, i32 0, i32 0, !dbg !388 ; line:25 col:31
  %269 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 0, !dbg !388 ; line:25 col:31
  %270 = load i32, i32* %269, !dbg !388 ; line:25 col:31
  store i32 %270, i32* %268, !dbg !388 ; line:25 col:31
  %271 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_msaa, i32 0, i32 1, !dbg !388 ; line:25 col:31
  %272 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 1, !dbg !388 ; line:25 col:31
  %273 = load i32, i32* %272, !dbg !388 ; line:25 col:31
  store i32 %273, i32* %271, !dbg !388 ; line:25 col:31
  %274 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_msaa, i32 0, i32 2, !dbg !388 ; line:25 col:31
  %275 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 2, !dbg !388 ; line:25 col:31
  %276 = load i32, i32* %275, !dbg !388 ; line:25 col:31
  store i32 %276, i32* %274, !dbg !388 ; line:25 col:31
  %277 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_msaa, i32 0, i32 3, !dbg !388 ; line:25 col:31
  %278 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 3, !dbg !388 ; line:25 col:31
  %279 = load i32, i32* %278, !dbg !388 ; line:25 col:31
  store i32 %279, i32* %277, !dbg !388 ; line:25 col:31
  %280 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_msaa, i32 0, i32 4, !dbg !388 ; line:25 col:31
  %281 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 4, !dbg !388 ; line:25 col:31
  %282 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %280, i32 0, i32 0, !dbg !388 ; line:25 col:31
  %283 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %281, i32 0, i32 0, !dbg !388 ; line:25 col:31
  %284 = load i16, i16* %283, !dbg !388 ; line:25 col:31
  store i16 %284, i16* %282, !dbg !388 ; line:25 col:31
  %285 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %280, i32 0, i32 1, !dbg !388 ; line:25 col:31
  %286 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %281, i32 0, i32 1, !dbg !388 ; line:25 col:31
  %287 = load i16, i16* %286, !dbg !388 ; line:25 col:31
  store i16 %287, i16* %285, !dbg !388 ; line:25 col:31
  %288 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %280, i32 0, i32 2, !dbg !388 ; line:25 col:31
  %289 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %281, i32 0, i32 2, !dbg !388 ; line:25 col:31
  %290 = load i32, i32* %289, !dbg !388 ; line:25 col:31
  store i32 %290, i32* %288, !dbg !388 ; line:25 col:31
  %291 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %280, i32 0, i32 3, !dbg !388 ; line:25 col:31
  %292 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %281, i32 0, i32 3, !dbg !388 ; line:25 col:31
  %293 = load i32, i32* %292, !dbg !388 ; line:25 col:31
  store i32 %293, i32* %291, !dbg !388 ; line:25 col:31
  %294 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_msaa, i32 0, i32 5, !dbg !388 ; line:25 col:31
  %295 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 5, !dbg !388 ; line:25 col:31
  %296 = load float, float* %295, !dbg !388 ; line:25 col:31
  store float %296, float* %294, !dbg !388 ; line:25 col:31
  %297 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_msaa, i32 0, i32 6, !dbg !388 ; line:25 col:31
  %298 = getelementptr inbounds %struct.texture, %struct.texture* %7, i32 0, i32 6, !dbg !388 ; line:25 col:31
  %299 = load i32, i32* %298, !dbg !388 ; line:25 col:31
  store i32 %299, i32* %297, !dbg !388 ; line:25 col:31
  call void @llvm.dbg.declare(metadata %struct.texture* %game_depth_rt_msaa, metadata !466, metadata !215), !dbg !467 ; var:"game_depth_rt_msaa" !DIExpression() func:"main"
  %300 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !468 ; line:26 col:31
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !280, metadata !215), !dbg !469 ; var:"flags" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !283, metadata !215), !dbg !470 ; var:"sampleQuality" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 %game_color_sample_count, i64 0, metadata !285, metadata !215), !dbg !471 ; var:"sampleCount" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !287, metadata !215), !dbg !472 ; var:"numTemporalLayers" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !289, metadata !215), !dbg !473 ; var:"arraySlices" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 2, i64 0, metadata !291, metadata !215), !dbg !474 ; var:"numMips" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !293, metadata !215), !dbg !475 ; var:"height" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !295, metadata !215), !dbg !476 ; var:"width" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 20, i64 0, metadata !297, metadata !215), !dbg !477 ; var:"format" !DIExpression() func:"create_tex2d"
  %301 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !478 ; line:229 col:15
  call void @llvm.dbg.value(metadata i32 %game_color_sample_count, i64 0, metadata !300, metadata !215), !dbg !479 ; var:"sampleCount" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !303, metadata !215), !dbg !481 ; var:"depth" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !305, metadata !215), !dbg !482 ; var:"height" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !307, metadata !215), !dbg !483 ; var:"width" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 2, i64 0, metadata !309, metadata !215), !dbg !484 ; var:"inMipLevel" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %cmp.i.i.102 = icmp ugt i32 %game_color_sample_count, 1, !dbg !485 ; line:98 col:21
  br i1 %cmp.i.i.102, label %if.then.i.i.103, label %if.end.i.i.104, !dbg !486 ; line:98 col:9

if.then.i.i.103:                                  ; preds = %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.77"
  br label %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.139", !dbg !487 ; line:100 col:9

if.end.i.i.104:                                   ; preds = %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.77"
  %302 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !488 ; line:103 col:14
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !322, metadata !215), !dbg !488 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %303 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !489 ; line:104 col:14
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !325, metadata !215), !dbg !489 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %304 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !490 ; line:105 col:14
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !327, metadata !215), !dbg !490 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %305 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !491 ; line:106 col:14
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !329, metadata !215), !dbg !491 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  br label %while.cond.i.i.106, !dbg !492 ; line:108 col:5

while.cond.i.i.106:                               ; preds = %while.body.i.i.116, %if.end.i.i.104
  %w.i.i.88.0 = phi i32 [ %IMax, %if.end.i.i.104 ], [ %shr.i.i.113, %while.body.i.i.116 ]
  %h.i.i.89.0 = phi i32 [ %IMax219, %if.end.i.i.104 ], [ %shr12.i.i.114, %while.body.i.i.116 ]
  %d.i.i.90.0 = phi i32 [ 1, %if.end.i.i.104 ], [ %shr13.i.i.115, %while.body.i.i.116 ]
  %mips.i.i.91.0 = phi i32 [ 1, %if.end.i.i.104 ], [ %inc.i.i.112, %while.body.i.i.116 ]
  call void @llvm.dbg.value(metadata i32 %mips.i.i.91.0, i64 0, metadata !329, metadata !215), !dbg !491 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %d.i.i.90.0, i64 0, metadata !327, metadata !215), !dbg !490 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %h.i.i.89.0, i64 0, metadata !325, metadata !215), !dbg !489 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %w.i.i.88.0, i64 0, metadata !322, metadata !215), !dbg !488 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %cmp2.i.i.105 = icmp ugt i32 %w.i.i.88.0, 1, !dbg !493 ; line:108 col:15
  br i1 %cmp2.i.i.105, label %lor.end.i.i.111, label %lor.lhs.false.i.i.108, !dbg !494 ; line:108 col:20

lor.lhs.false.i.i.108:                            ; preds = %while.cond.i.i.106
  %cmp5.i.i.107 = icmp ugt i32 %h.i.i.89.0, 1, !dbg !495 ; line:108 col:26
  br i1 %cmp5.i.i.107, label %lor.end.i.i.111, label %lor.rhs.i.i.110, !dbg !496 ; line:108 col:31

lor.rhs.i.i.110:                                  ; preds = %lor.lhs.false.i.i.108
  %cmp8.i.i.109 = icmp ugt i32 %d.i.i.90.0, 1, !dbg !497 ; line:108 col:37
  br label %lor.end.i.i.111, !dbg !496 ; line:108 col:31

lor.end.i.i.111:                                  ; preds = %lor.rhs.i.i.110, %lor.lhs.false.i.i.108, %while.cond.i.i.106
  %306 = phi i1 [ true, %lor.lhs.false.i.i.108 ], [ true, %while.cond.i.i.106 ], [ %cmp8.i.i.109, %lor.rhs.i.i.110 ], !dbg !478 ; line:229 col:15
  br i1 %306, label %while.body.i.i.116, label %while.end.i.i.118, !dbg !492 ; line:108 col:5

while.body.i.i.116:                               ; preds = %lor.end.i.i.111
  %inc.i.i.112 = add i32 %mips.i.i.91.0, 1, !dbg !498 ; line:110 col:13
  %307 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !498 ; line:110 col:13
  call void @llvm.dbg.value(metadata i32 %inc.i.i.112, i64 0, metadata !329, metadata !215), !dbg !491 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr.i.i.113 = lshr i32 %w.i.i.88.0, 1, !dbg !499 ; line:111 col:15
  %308 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !500 ; line:111 col:11
  call void @llvm.dbg.value(metadata i32 %shr.i.i.113, i64 0, metadata !322, metadata !215), !dbg !488 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr12.i.i.114 = lshr i32 %h.i.i.89.0, 1, !dbg !501 ; line:112 col:15
  %309 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !502 ; line:112 col:11
  call void @llvm.dbg.value(metadata i32 %shr12.i.i.114, i64 0, metadata !325, metadata !215), !dbg !489 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr13.i.i.115 = lshr i32 %d.i.i.90.0, 1, !dbg !503 ; line:113 col:15
  %310 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !504 ; line:113 col:11
  call void @llvm.dbg.value(metadata i32 %shr13.i.i.115, i64 0, metadata !327, metadata !215), !dbg !490 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  br label %while.cond.i.i.106, !dbg !492 ; line:108 col:5

while.end.i.i.118:                                ; preds = %lor.end.i.i.111
  %mips.i.i.91.0.lcssa = phi i32 [ %mips.i.i.91.0, %lor.end.i.i.111 ]
  br label %cond.false.i.i.120, !dbg !505 ; line:116 col:12

cond.false.i.i.120:                               ; preds = %while.end.i.i.118
  %UMin220 = call i32 @___rpsl_dxop_binary_i32(i32 40, i32 2, i32 %mips.i.i.91.0.lcssa), !dbg !506 ; line:116 col:39
  br label %cond.end.i.i.122, !dbg !505 ; line:116 col:12

cond.end.i.i.122:                                 ; preds = %cond.false.i.i.120
  %cond.i.i.121 = phi i32 [ %UMin220, %cond.false.i.i.120 ], !dbg !505 ; line:116 col:12
  br label %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.139", !dbg !507 ; line:116 col:5

"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.139": ; preds = %cond.end.i.i.122, %if.then.i.i.103
  %retval.i.i.82.0 = phi i32 [ 1, %if.then.i.i.103 ], [ %cond.i.i.121, %cond.end.i.i.122 ]
  %311 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !508 ; line:117 col:1
  call void @llvm.dbg.value(metadata i32 %retval.i.i.82.0, i64 0, metadata !291, metadata !215), !dbg !474 ; var:"numMips" !DIExpression() func:"create_tex2d"
  %call1.i.123 = call i32 @___rpsl_create_resource(i32 3, i32 0, i32 20, i32 %IMax, i32 %IMax219, i32 1, i32 %retval.i.i.82.0, i32 %game_color_sample_count, i32 0, i32 1, i32 2) #0, !dbg !509, !noalias !510 ; line:231 col:30
  %312 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !513 ; line:231 col:16
  call void @llvm.dbg.value(metadata i32 %call1.i.123, i64 0, metadata !353, metadata !215), !dbg !513 ; var:"resourceHdl" !DIExpression() func:"create_tex2d"
  %313 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !514 ; line:233 col:12
  call void @llvm.dbg.value(metadata i32 %retval.i.i.82.0, i64 0, metadata !228, metadata !215), !dbg !515 ; var:"numMips" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !230, metadata !215), !dbg !516 ; var:"arraySlices" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 %call1.i.123, i64 0, metadata !232, metadata !215), !dbg !517 ; var:"resourceHdl" !DIExpression() func:"make_default_texture_view"
  %314 = bitcast %struct.texture* %1 to i8*, !dbg !514 ; line:233 col:12
  call void @llvm.memset.p0i8.i32(i8* %314, i8 0, i32 36, i32 4, i1 false) #0, !dbg !514, !noalias !518 ; line:233 col:12
  %Resource.i.i.124 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 0, !dbg !521 ; line:123 col:12
  %315 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !522 ; line:123 col:21
  store i32 %call1.i.123, i32* %Resource.i.i.124, align 4, !dbg !522, !noalias !518 ; line:123 col:21
  %ViewFormat.i.i.125 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 1, !dbg !523 ; line:124 col:12
  %316 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !524 ; line:124 col:23
  store i32 0, i32* %ViewFormat.i.i.125, align 4, !dbg !524, !noalias !518 ; line:124 col:23
  %TemporalLayer.i.i.126 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 2, !dbg !525 ; line:125 col:12
  %317 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !526 ; line:125 col:26
  store i32 0, i32* %TemporalLayer.i.i.126, align 4, !dbg !526, !noalias !518 ; line:125 col:26
  %Flags.i.i.127 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 3, !dbg !527 ; line:126 col:12
  %318 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !528 ; line:126 col:18
  store i32 0, i32* %Flags.i.i.127, align 4, !dbg !528, !noalias !518 ; line:126 col:18
  %base_mip_level.i.i.129209 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 4, i32 0, !dbg !529 ; line:127 col:29
  %319 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !530 ; line:127 col:44
  store i16 0, i16* %base_mip_level.i.i.129209, align 2, !dbg !530, !noalias !518 ; line:127 col:44
  %conv.i.i.130 = trunc i32 %retval.i.i.82.0 to i16, !dbg !531 ; line:128 col:56
  %mip_level_count.i.i.132208 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 4, i32 1, !dbg !532 ; line:128 col:29
  %320 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !533 ; line:128 col:45
  store i16 %conv.i.i.130, i16* %mip_level_count.i.i.132208, align 2, !dbg !533, !noalias !518 ; line:128 col:45
  %base_array_layer.i.i.134207 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 4, i32 2, !dbg !534 ; line:129 col:29
  %321 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !535 ; line:129 col:46
  store i32 0, i32* %base_array_layer.i.i.134207, align 4, !dbg !535, !noalias !518 ; line:129 col:46
  %array_layer_count.i.i.136206 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 4, i32 3, !dbg !536 ; line:130 col:29
  %322 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !537 ; line:130 col:47
  store i32 1, i32* %array_layer_count.i.i.136206, align 4, !dbg !537, !noalias !518 ; line:130 col:47
  %MinLodClamp.i.i.137 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 5, !dbg !538 ; line:131 col:12
  %323 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !539 ; line:131 col:24
  store float 0.000000e+00, float* %MinLodClamp.i.i.137, align 4, !dbg !539, !noalias !518 ; line:131 col:24
  %ComponentMapping.i.i.138 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 6, !dbg !540 ; line:132 col:12
  %324 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !541 ; line:132 col:29
  store i32 50462976, i32* %ComponentMapping.i.i.138, align 4, !dbg !541, !noalias !518 ; line:132 col:29
  %325 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !542 ; line:134 col:5
  %326 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 0, !dbg !542 ; line:134 col:5
  %327 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 0, !dbg !542 ; line:134 col:5
  %328 = load i32, i32* %327, !dbg !542 ; line:134 col:5
  store i32 %328, i32* %326, !dbg !542 ; line:134 col:5
  %329 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 1, !dbg !542 ; line:134 col:5
  %330 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 1, !dbg !542 ; line:134 col:5
  %331 = load i32, i32* %330, !dbg !542 ; line:134 col:5
  store i32 %331, i32* %329, !dbg !542 ; line:134 col:5
  %332 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 2, !dbg !542 ; line:134 col:5
  %333 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 2, !dbg !542 ; line:134 col:5
  %334 = load i32, i32* %333, !dbg !542 ; line:134 col:5
  store i32 %334, i32* %332, !dbg !542 ; line:134 col:5
  %335 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 3, !dbg !542 ; line:134 col:5
  %336 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 3, !dbg !542 ; line:134 col:5
  %337 = load i32, i32* %336, !dbg !542 ; line:134 col:5
  store i32 %337, i32* %335, !dbg !542 ; line:134 col:5
  %338 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 4, !dbg !542 ; line:134 col:5
  %339 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 4, !dbg !542 ; line:134 col:5
  %340 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %338, i32 0, i32 0, !dbg !542 ; line:134 col:5
  %341 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %339, i32 0, i32 0, !dbg !542 ; line:134 col:5
  %342 = load i16, i16* %341, !dbg !542 ; line:134 col:5
  store i16 %342, i16* %340, !dbg !542 ; line:134 col:5
  %343 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %338, i32 0, i32 1, !dbg !542 ; line:134 col:5
  %344 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %339, i32 0, i32 1, !dbg !542 ; line:134 col:5
  %345 = load i16, i16* %344, !dbg !542 ; line:134 col:5
  store i16 %345, i16* %343, !dbg !542 ; line:134 col:5
  %346 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %338, i32 0, i32 2, !dbg !542 ; line:134 col:5
  %347 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %339, i32 0, i32 2, !dbg !542 ; line:134 col:5
  %348 = load i32, i32* %347, !dbg !542 ; line:134 col:5
  store i32 %348, i32* %346, !dbg !542 ; line:134 col:5
  %349 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %338, i32 0, i32 3, !dbg !542 ; line:134 col:5
  %350 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %339, i32 0, i32 3, !dbg !542 ; line:134 col:5
  %351 = load i32, i32* %350, !dbg !542 ; line:134 col:5
  store i32 %351, i32* %349, !dbg !542 ; line:134 col:5
  %352 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 5, !dbg !542 ; line:134 col:5
  %353 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 5, !dbg !542 ; line:134 col:5
  %354 = load float, float* %353, !dbg !542 ; line:134 col:5
  store float %354, float* %352, !dbg !542 ; line:134 col:5
  %355 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 6, !dbg !542 ; line:134 col:5
  %356 = getelementptr inbounds %struct.texture, %struct.texture* %1, i32 0, i32 6, !dbg !542 ; line:134 col:5
  %357 = load i32, i32* %356, !dbg !542 ; line:134 col:5
  store i32 %357, i32* %355, !dbg !542 ; line:134 col:5
  %358 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !543 ; line:233 col:5
  %359 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !468 ; line:26 col:31
  %360 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 0, !dbg !468 ; line:26 col:31
  %361 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 0, !dbg !468 ; line:26 col:31
  %362 = load i32, i32* %361, !dbg !468 ; line:26 col:31
  store i32 %362, i32* %360, !dbg !468 ; line:26 col:31
  %363 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 1, !dbg !468 ; line:26 col:31
  %364 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 1, !dbg !468 ; line:26 col:31
  %365 = load i32, i32* %364, !dbg !468 ; line:26 col:31
  store i32 %365, i32* %363, !dbg !468 ; line:26 col:31
  %366 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 2, !dbg !468 ; line:26 col:31
  %367 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 2, !dbg !468 ; line:26 col:31
  %368 = load i32, i32* %367, !dbg !468 ; line:26 col:31
  store i32 %368, i32* %366, !dbg !468 ; line:26 col:31
  %369 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 3, !dbg !468 ; line:26 col:31
  %370 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 3, !dbg !468 ; line:26 col:31
  %371 = load i32, i32* %370, !dbg !468 ; line:26 col:31
  store i32 %371, i32* %369, !dbg !468 ; line:26 col:31
  %372 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 4, !dbg !468 ; line:26 col:31
  %373 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 4, !dbg !468 ; line:26 col:31
  %374 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %372, i32 0, i32 0, !dbg !468 ; line:26 col:31
  %375 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %373, i32 0, i32 0, !dbg !468 ; line:26 col:31
  %376 = load i16, i16* %375, !dbg !468 ; line:26 col:31
  store i16 %376, i16* %374, !dbg !468 ; line:26 col:31
  %377 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %372, i32 0, i32 1, !dbg !468 ; line:26 col:31
  %378 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %373, i32 0, i32 1, !dbg !468 ; line:26 col:31
  %379 = load i16, i16* %378, !dbg !468 ; line:26 col:31
  store i16 %379, i16* %377, !dbg !468 ; line:26 col:31
  %380 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %372, i32 0, i32 2, !dbg !468 ; line:26 col:31
  %381 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %373, i32 0, i32 2, !dbg !468 ; line:26 col:31
  %382 = load i32, i32* %381, !dbg !468 ; line:26 col:31
  store i32 %382, i32* %380, !dbg !468 ; line:26 col:31
  %383 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %372, i32 0, i32 3, !dbg !468 ; line:26 col:31
  %384 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %373, i32 0, i32 3, !dbg !468 ; line:26 col:31
  %385 = load i32, i32* %384, !dbg !468 ; line:26 col:31
  store i32 %385, i32* %383, !dbg !468 ; line:26 col:31
  %386 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 5, !dbg !468 ; line:26 col:31
  %387 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 5, !dbg !468 ; line:26 col:31
  %388 = load float, float* %387, !dbg !468 ; line:26 col:31
  store float %388, float* %386, !dbg !468 ; line:26 col:31
  %389 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 6, !dbg !468 ; line:26 col:31
  %390 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp6, i32 0, i32 6, !dbg !468 ; line:26 col:31
  %391 = load i32, i32* %390, !dbg !468 ; line:26 col:31
  store i32 %391, i32* %389, !dbg !468 ; line:26 col:31
  %392 = getelementptr %struct.texture, %struct.texture* %6, i32 0, i32 0, !dbg !468 ; line:26 col:31
  %393 = load i32, i32* %392, !dbg !468 ; line:26 col:31
  call void @___rpsl_name_resource(i32 %393, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @"@@rps_Str2", i32 0, i32 0), i32 18), !dbg !468 ; line:26 col:31
  %394 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !468 ; line:26 col:31
  %395 = getelementptr inbounds %struct.texture, %struct.texture* %game_depth_rt_msaa, i32 0, i32 0, !dbg !468 ; line:26 col:31
  %396 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 0, !dbg !468 ; line:26 col:31
  %397 = load i32, i32* %396, !dbg !468 ; line:26 col:31
  store i32 %397, i32* %395, !dbg !468 ; line:26 col:31
  %398 = getelementptr inbounds %struct.texture, %struct.texture* %game_depth_rt_msaa, i32 0, i32 1, !dbg !468 ; line:26 col:31
  %399 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 1, !dbg !468 ; line:26 col:31
  %400 = load i32, i32* %399, !dbg !468 ; line:26 col:31
  store i32 %400, i32* %398, !dbg !468 ; line:26 col:31
  %401 = getelementptr inbounds %struct.texture, %struct.texture* %game_depth_rt_msaa, i32 0, i32 2, !dbg !468 ; line:26 col:31
  %402 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 2, !dbg !468 ; line:26 col:31
  %403 = load i32, i32* %402, !dbg !468 ; line:26 col:31
  store i32 %403, i32* %401, !dbg !468 ; line:26 col:31
  %404 = getelementptr inbounds %struct.texture, %struct.texture* %game_depth_rt_msaa, i32 0, i32 3, !dbg !468 ; line:26 col:31
  %405 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 3, !dbg !468 ; line:26 col:31
  %406 = load i32, i32* %405, !dbg !468 ; line:26 col:31
  store i32 %406, i32* %404, !dbg !468 ; line:26 col:31
  %407 = getelementptr inbounds %struct.texture, %struct.texture* %game_depth_rt_msaa, i32 0, i32 4, !dbg !468 ; line:26 col:31
  %408 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 4, !dbg !468 ; line:26 col:31
  %409 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %407, i32 0, i32 0, !dbg !468 ; line:26 col:31
  %410 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %408, i32 0, i32 0, !dbg !468 ; line:26 col:31
  %411 = load i16, i16* %410, !dbg !468 ; line:26 col:31
  store i16 %411, i16* %409, !dbg !468 ; line:26 col:31
  %412 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %407, i32 0, i32 1, !dbg !468 ; line:26 col:31
  %413 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %408, i32 0, i32 1, !dbg !468 ; line:26 col:31
  %414 = load i16, i16* %413, !dbg !468 ; line:26 col:31
  store i16 %414, i16* %412, !dbg !468 ; line:26 col:31
  %415 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %407, i32 0, i32 2, !dbg !468 ; line:26 col:31
  %416 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %408, i32 0, i32 2, !dbg !468 ; line:26 col:31
  %417 = load i32, i32* %416, !dbg !468 ; line:26 col:31
  store i32 %417, i32* %415, !dbg !468 ; line:26 col:31
  %418 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %407, i32 0, i32 3, !dbg !468 ; line:26 col:31
  %419 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %408, i32 0, i32 3, !dbg !468 ; line:26 col:31
  %420 = load i32, i32* %419, !dbg !468 ; line:26 col:31
  store i32 %420, i32* %418, !dbg !468 ; line:26 col:31
  %421 = getelementptr inbounds %struct.texture, %struct.texture* %game_depth_rt_msaa, i32 0, i32 5, !dbg !468 ; line:26 col:31
  %422 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 5, !dbg !468 ; line:26 col:31
  %423 = load float, float* %422, !dbg !468 ; line:26 col:31
  store float %423, float* %421, !dbg !468 ; line:26 col:31
  %424 = getelementptr inbounds %struct.texture, %struct.texture* %game_depth_rt_msaa, i32 0, i32 6, !dbg !468 ; line:26 col:31
  %425 = getelementptr inbounds %struct.texture, %struct.texture* %6, i32 0, i32 6, !dbg !468 ; line:26 col:31
  %426 = load i32, i32* %425, !dbg !468 ; line:26 col:31
  store i32 %426, i32* %424, !dbg !468 ; line:26 col:31
  %427 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !544 ; line:27 col:8
  %428 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !545 ; line:29 col:2
  call void @llvm.dbg.value(metadata <4 x float> <float 0x3FD51EB860000000, float 0x3FD51EB860000000, float 0x3FD51EB860000000, float 1.000000e+00>, i64 0, metadata !546, metadata !215), !dbg !547 ; var:"val" !DIExpression() func:"clear"
  call void @llvm.dbg.declare(metadata %struct.texture* %game_color_rt_msaa, metadata !549, metadata !215) #0, !dbg !550 ; var:"t" !DIExpression() func:"clear"
  %429 = alloca i8*, i32 2, !dbg !551 ; line:294 col:12
  %430 = bitcast %struct.texture* %game_color_rt_msaa to i8*, !dbg !551 ; line:294 col:12
  %431 = getelementptr i8*, i8** %429, i32 0, !dbg !551 ; line:294 col:12
  store i8* %430, i8** %431, !dbg !551 ; line:294 col:12
  %432 = alloca <4 x float>, !dbg !551 ; line:294 col:12
  store <4 x float> <float 0x3FD51EB860000000, float 0x3FD51EB860000000, float 0x3FD51EB860000000, float 1.000000e+00>, <4 x float>* %432, !dbg !551 ; line:294 col:12
  %433 = bitcast <4 x float>* %432 to i8*, !dbg !551 ; line:294 col:12
  %434 = getelementptr i8*, i8** %429, i32 1, !dbg !551 ; line:294 col:12
  store i8* %433, i8** %434, !dbg !551 ; line:294 col:12
  %435 = call i32 @___rpsl_node_call(i32 0, i32 2, i8** %429, i32 0, i32 0), !dbg !551 ; line:294 col:12
  %436 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !552 ; line:294 col:5
  %437 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !553 ; line:31 col:2
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !554, metadata !215), !dbg !555 ; var:"stencil" !DIExpression() func:"clear"
  call void @llvm.dbg.value(metadata float 0.000000e+00, i64 0, metadata !557, metadata !215), !dbg !558 ; var:"depth" !DIExpression() func:"clear"
  call void @llvm.dbg.declare(metadata %struct.texture* %game_depth_rt_msaa, metadata !559, metadata !215) #0, !dbg !560 ; var:"t" !DIExpression() func:"clear"
  %438 = alloca i8*, i32 4, !dbg !561 ; line:304 col:12
  %439 = bitcast %struct.texture* %game_depth_rt_msaa to i8*, !dbg !561 ; line:304 col:12
  %440 = getelementptr i8*, i8** %438, i32 0, !dbg !561 ; line:304 col:12
  store i8* %439, i8** %440, !dbg !561 ; line:304 col:12
  %441 = alloca i32, !dbg !561 ; line:304 col:12
  store i32 6, i32* %441, !dbg !561 ; line:304 col:12
  %442 = bitcast i32* %441 to i8*, !dbg !561 ; line:304 col:12
  %443 = getelementptr i8*, i8** %438, i32 1, !dbg !561 ; line:304 col:12
  store i8* %442, i8** %443, !dbg !561 ; line:304 col:12
  %444 = alloca float, !dbg !561 ; line:304 col:12
  store float 0.000000e+00, float* %444, !dbg !561 ; line:304 col:12
  %445 = bitcast float* %444 to i8*, !dbg !561 ; line:304 col:12
  %446 = getelementptr i8*, i8** %438, i32 2, !dbg !561 ; line:304 col:12
  store i8* %445, i8** %446, !dbg !561 ; line:304 col:12
  %447 = alloca i32, !dbg !561 ; line:304 col:12
  store i32 0, i32* %447, !dbg !561 ; line:304 col:12
  %448 = bitcast i32* %447 to i8*, !dbg !561 ; line:304 col:12
  %449 = getelementptr i8*, i8** %438, i32 3, !dbg !561 ; line:304 col:12
  store i8* %448, i8** %449, !dbg !561 ; line:304 col:12
  %450 = call i32 @___rpsl_node_call(i32 1, i32 4, i8** %438, i32 0, i32 1), !dbg !561 ; line:304 col:12
  %451 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !562 ; line:304 col:5
  call void @llvm.dbg.declare(metadata %struct.texture* %game_color_rt_pre_display_map, metadata !563, metadata !215), !dbg !564 ; var:"game_color_rt_pre_display_map" !DIExpression() func:"main"
  %452 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !565 ; line:35 col:42
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !280, metadata !215), !dbg !566 ; var:"flags" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !283, metadata !215), !dbg !567 ; var:"sampleQuality" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !285, metadata !215), !dbg !568 ; var:"sampleCount" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !287, metadata !215), !dbg !569 ; var:"numTemporalLayers" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !289, metadata !215), !dbg !570 ; var:"arraySlices" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 2, i64 0, metadata !291, metadata !215), !dbg !571 ; var:"numMips" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !293, metadata !215), !dbg !572 ; var:"height" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !295, metadata !215), !dbg !573 ; var:"width" !DIExpression() func:"create_tex2d"
  call void @llvm.dbg.value(metadata i32 10, i64 0, metadata !297, metadata !215), !dbg !574 ; var:"format" !DIExpression() func:"create_tex2d"
  %453 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !575 ; line:229 col:15
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !300, metadata !215), !dbg !576 ; var:"sampleCount" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !303, metadata !215), !dbg !578 ; var:"depth" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !305, metadata !215), !dbg !579 ; var:"height" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !307, metadata !215), !dbg !580 ; var:"width" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 2, i64 0, metadata !309, metadata !215), !dbg !581 ; var:"inMipLevel" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  br label %if.end.i.i.166, !dbg !582 ; line:98 col:9

if.end.i.i.166:                                   ; preds = %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.139"
  %454 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !583 ; line:103 col:14
  call void @llvm.dbg.value(metadata i32 %IMax, i64 0, metadata !322, metadata !215), !dbg !583 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %455 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !584 ; line:104 col:14
  call void @llvm.dbg.value(metadata i32 %IMax219, i64 0, metadata !325, metadata !215), !dbg !584 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %456 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !585 ; line:105 col:14
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !327, metadata !215), !dbg !585 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %457 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !586 ; line:106 col:14
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !329, metadata !215), !dbg !586 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  br label %while.cond.i.i.168, !dbg !587 ; line:108 col:5

while.cond.i.i.168:                               ; preds = %while.body.i.i.178, %if.end.i.i.166
  %mips.i.i.153.0 = phi i32 [ 1, %if.end.i.i.166 ], [ %inc.i.i.174, %while.body.i.i.178 ]
  %d.i.i.152.0 = phi i32 [ 1, %if.end.i.i.166 ], [ %shr13.i.i.177, %while.body.i.i.178 ]
  %h.i.i.151.0 = phi i32 [ %IMax219, %if.end.i.i.166 ], [ %shr12.i.i.176, %while.body.i.i.178 ]
  %w.i.i.150.0 = phi i32 [ %IMax, %if.end.i.i.166 ], [ %shr.i.i.175, %while.body.i.i.178 ]
  call void @llvm.dbg.value(metadata i32 %w.i.i.150.0, i64 0, metadata !322, metadata !215), !dbg !583 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %d.i.i.152.0, i64 0, metadata !327, metadata !215), !dbg !585 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %mips.i.i.153.0, i64 0, metadata !329, metadata !215), !dbg !586 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  call void @llvm.dbg.value(metadata i32 %h.i.i.151.0, i64 0, metadata !325, metadata !215), !dbg !584 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %cmp2.i.i.167 = icmp ugt i32 %w.i.i.150.0, 1, !dbg !588 ; line:108 col:15
  br i1 %cmp2.i.i.167, label %lor.end.i.i.173, label %lor.lhs.false.i.i.170, !dbg !589 ; line:108 col:20

lor.lhs.false.i.i.170:                            ; preds = %while.cond.i.i.168
  %cmp5.i.i.169 = icmp ugt i32 %h.i.i.151.0, 1, !dbg !590 ; line:108 col:26
  br i1 %cmp5.i.i.169, label %lor.end.i.i.173, label %lor.rhs.i.i.172, !dbg !591 ; line:108 col:31

lor.rhs.i.i.172:                                  ; preds = %lor.lhs.false.i.i.170
  %cmp8.i.i.171 = icmp ugt i32 %d.i.i.152.0, 1, !dbg !592 ; line:108 col:37
  br label %lor.end.i.i.173, !dbg !591 ; line:108 col:31

lor.end.i.i.173:                                  ; preds = %lor.rhs.i.i.172, %lor.lhs.false.i.i.170, %while.cond.i.i.168
  %458 = phi i1 [ true, %lor.lhs.false.i.i.170 ], [ true, %while.cond.i.i.168 ], [ %cmp8.i.i.171, %lor.rhs.i.i.172 ], !dbg !575 ; line:229 col:15
  br i1 %458, label %while.body.i.i.178, label %while.end.i.i.180, !dbg !587 ; line:108 col:5

while.body.i.i.178:                               ; preds = %lor.end.i.i.173
  %inc.i.i.174 = add i32 %mips.i.i.153.0, 1, !dbg !593 ; line:110 col:13
  %459 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !593 ; line:110 col:13
  call void @llvm.dbg.value(metadata i32 %inc.i.i.174, i64 0, metadata !329, metadata !215), !dbg !586 ; var:"mips" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr.i.i.175 = lshr i32 %w.i.i.150.0, 1, !dbg !594 ; line:111 col:15
  %460 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !595 ; line:111 col:11
  call void @llvm.dbg.value(metadata i32 %shr.i.i.175, i64 0, metadata !322, metadata !215), !dbg !583 ; var:"w" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr12.i.i.176 = lshr i32 %h.i.i.151.0, 1, !dbg !596 ; line:112 col:15
  %461 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !597 ; line:112 col:11
  call void @llvm.dbg.value(metadata i32 %shr12.i.i.176, i64 0, metadata !325, metadata !215), !dbg !584 ; var:"h" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  %shr13.i.i.177 = lshr i32 %d.i.i.152.0, 1, !dbg !598 ; line:113 col:15
  %462 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !599 ; line:113 col:11
  call void @llvm.dbg.value(metadata i32 %shr13.i.i.177, i64 0, metadata !327, metadata !215), !dbg !585 ; var:"d" !DIExpression() func:"___rpsl_canonicalize_mip_level"
  br label %while.cond.i.i.168, !dbg !587 ; line:108 col:5

while.end.i.i.180:                                ; preds = %lor.end.i.i.173
  %mips.i.i.153.0.lcssa = phi i32 [ %mips.i.i.153.0, %lor.end.i.i.173 ]
  br label %cond.false.i.i.182, !dbg !600 ; line:116 col:12

cond.false.i.i.182:                               ; preds = %while.end.i.i.180
  %UMin = call i32 @___rpsl_dxop_binary_i32(i32 40, i32 2, i32 %mips.i.i.153.0.lcssa), !dbg !601 ; line:116 col:39
  br label %cond.end.i.i.184, !dbg !600 ; line:116 col:12

cond.end.i.i.184:                                 ; preds = %cond.false.i.i.182
  %cond.i.i.183 = phi i32 [ %UMin, %cond.false.i.i.182 ], !dbg !600 ; line:116 col:12
  br label %"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.201", !dbg !602 ; line:116 col:5

"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z.exit.201": ; preds = %cond.end.i.i.184
  %retval.i.i.144.0 = phi i32 [ %cond.i.i.183, %cond.end.i.i.184 ]
  %463 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !603 ; line:117 col:1
  call void @llvm.dbg.value(metadata i32 %retval.i.i.144.0, i64 0, metadata !291, metadata !215), !dbg !571 ; var:"numMips" !DIExpression() func:"create_tex2d"
  %call1.i.185 = call i32 @___rpsl_create_resource(i32 3, i32 0, i32 10, i32 %IMax, i32 %IMax219, i32 1, i32 %retval.i.i.144.0, i32 1, i32 0, i32 1, i32 3) #0, !dbg !604, !noalias !605 ; line:231 col:30
  %464 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !608 ; line:231 col:16
  call void @llvm.dbg.value(metadata i32 %call1.i.185, i64 0, metadata !353, metadata !215), !dbg !608 ; var:"resourceHdl" !DIExpression() func:"create_tex2d"
  %465 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !609 ; line:233 col:12
  call void @llvm.dbg.value(metadata i32 %retval.i.i.144.0, i64 0, metadata !228, metadata !215), !dbg !610 ; var:"numMips" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !230, metadata !215), !dbg !611 ; var:"arraySlices" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 %call1.i.185, i64 0, metadata !232, metadata !215), !dbg !612 ; var:"resourceHdl" !DIExpression() func:"make_default_texture_view"
  %466 = bitcast %struct.texture* %0 to i8*, !dbg !609 ; line:233 col:12
  call void @llvm.memset.p0i8.i32(i8* %466, i8 0, i32 36, i32 4, i1 false) #0, !dbg !609, !noalias !613 ; line:233 col:12
  %Resource.i.i.186 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 0, !dbg !616 ; line:123 col:12
  %467 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !617 ; line:123 col:21
  store i32 %call1.i.185, i32* %Resource.i.i.186, align 4, !dbg !617, !noalias !613 ; line:123 col:21
  %ViewFormat.i.i.187 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 1, !dbg !618 ; line:124 col:12
  %468 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !619 ; line:124 col:23
  store i32 0, i32* %ViewFormat.i.i.187, align 4, !dbg !619, !noalias !613 ; line:124 col:23
  %TemporalLayer.i.i.188 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 2, !dbg !620 ; line:125 col:12
  %469 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !621 ; line:125 col:26
  store i32 0, i32* %TemporalLayer.i.i.188, align 4, !dbg !621, !noalias !613 ; line:125 col:26
  %Flags.i.i.189 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 3, !dbg !622 ; line:126 col:12
  %470 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !623 ; line:126 col:18
  store i32 0, i32* %Flags.i.i.189, align 4, !dbg !623, !noalias !613 ; line:126 col:18
  %base_mip_level.i.i.191205 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 0, !dbg !624 ; line:127 col:29
  %471 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !625 ; line:127 col:44
  store i16 0, i16* %base_mip_level.i.i.191205, align 2, !dbg !625, !noalias !613 ; line:127 col:44
  %conv.i.i.192 = trunc i32 %retval.i.i.144.0 to i16, !dbg !626 ; line:128 col:56
  %mip_level_count.i.i.194204 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 1, !dbg !627 ; line:128 col:29
  %472 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !628 ; line:128 col:45
  store i16 %conv.i.i.192, i16* %mip_level_count.i.i.194204, align 2, !dbg !628, !noalias !613 ; line:128 col:45
  %base_array_layer.i.i.196203 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 2, !dbg !629 ; line:129 col:29
  %473 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !630 ; line:129 col:46
  store i32 0, i32* %base_array_layer.i.i.196203, align 4, !dbg !630, !noalias !613 ; line:129 col:46
  %array_layer_count.i.i.198202 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 3, !dbg !631 ; line:130 col:29
  %474 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !632 ; line:130 col:47
  store i32 1, i32* %array_layer_count.i.i.198202, align 4, !dbg !632, !noalias !613 ; line:130 col:47
  %MinLodClamp.i.i.199 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 5, !dbg !633 ; line:131 col:12
  %475 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !634 ; line:131 col:24
  store float 0.000000e+00, float* %MinLodClamp.i.i.199, align 4, !dbg !634, !noalias !613 ; line:131 col:24
  %ComponentMapping.i.i.200 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 6, !dbg !635 ; line:132 col:12
  %476 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !636 ; line:132 col:29
  store i32 50462976, i32* %ComponentMapping.i.i.200, align 4, !dbg !636, !noalias !613 ; line:132 col:29
  %477 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !637 ; line:134 col:5
  %478 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 0, !dbg !637 ; line:134 col:5
  %479 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 0, !dbg !637 ; line:134 col:5
  %480 = load i32, i32* %479, !dbg !637 ; line:134 col:5
  store i32 %480, i32* %478, !dbg !637 ; line:134 col:5
  %481 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 1, !dbg !637 ; line:134 col:5
  %482 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 1, !dbg !637 ; line:134 col:5
  %483 = load i32, i32* %482, !dbg !637 ; line:134 col:5
  store i32 %483, i32* %481, !dbg !637 ; line:134 col:5
  %484 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 2, !dbg !637 ; line:134 col:5
  %485 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 2, !dbg !637 ; line:134 col:5
  %486 = load i32, i32* %485, !dbg !637 ; line:134 col:5
  store i32 %486, i32* %484, !dbg !637 ; line:134 col:5
  %487 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 3, !dbg !637 ; line:134 col:5
  %488 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 3, !dbg !637 ; line:134 col:5
  %489 = load i32, i32* %488, !dbg !637 ; line:134 col:5
  store i32 %489, i32* %487, !dbg !637 ; line:134 col:5
  %490 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 4, !dbg !637 ; line:134 col:5
  %491 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, !dbg !637 ; line:134 col:5
  %492 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %490, i32 0, i32 0, !dbg !637 ; line:134 col:5
  %493 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %491, i32 0, i32 0, !dbg !637 ; line:134 col:5
  %494 = load i16, i16* %493, !dbg !637 ; line:134 col:5
  store i16 %494, i16* %492, !dbg !637 ; line:134 col:5
  %495 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %490, i32 0, i32 1, !dbg !637 ; line:134 col:5
  %496 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %491, i32 0, i32 1, !dbg !637 ; line:134 col:5
  %497 = load i16, i16* %496, !dbg !637 ; line:134 col:5
  store i16 %497, i16* %495, !dbg !637 ; line:134 col:5
  %498 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %490, i32 0, i32 2, !dbg !637 ; line:134 col:5
  %499 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %491, i32 0, i32 2, !dbg !637 ; line:134 col:5
  %500 = load i32, i32* %499, !dbg !637 ; line:134 col:5
  store i32 %500, i32* %498, !dbg !637 ; line:134 col:5
  %501 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %490, i32 0, i32 3, !dbg !637 ; line:134 col:5
  %502 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %491, i32 0, i32 3, !dbg !637 ; line:134 col:5
  %503 = load i32, i32* %502, !dbg !637 ; line:134 col:5
  store i32 %503, i32* %501, !dbg !637 ; line:134 col:5
  %504 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 5, !dbg !637 ; line:134 col:5
  %505 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 5, !dbg !637 ; line:134 col:5
  %506 = load float, float* %505, !dbg !637 ; line:134 col:5
  store float %506, float* %504, !dbg !637 ; line:134 col:5
  %507 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 6, !dbg !637 ; line:134 col:5
  %508 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 6, !dbg !637 ; line:134 col:5
  %509 = load i32, i32* %508, !dbg !637 ; line:134 col:5
  store i32 %509, i32* %507, !dbg !637 ; line:134 col:5
  %510 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !638 ; line:233 col:5
  %511 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !565 ; line:35 col:42
  %512 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 0, !dbg !565 ; line:35 col:42
  %513 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 0, !dbg !565 ; line:35 col:42
  %514 = load i32, i32* %513, !dbg !565 ; line:35 col:42
  store i32 %514, i32* %512, !dbg !565 ; line:35 col:42
  %515 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 1, !dbg !565 ; line:35 col:42
  %516 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 1, !dbg !565 ; line:35 col:42
  %517 = load i32, i32* %516, !dbg !565 ; line:35 col:42
  store i32 %517, i32* %515, !dbg !565 ; line:35 col:42
  %518 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 2, !dbg !565 ; line:35 col:42
  %519 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 2, !dbg !565 ; line:35 col:42
  %520 = load i32, i32* %519, !dbg !565 ; line:35 col:42
  store i32 %520, i32* %518, !dbg !565 ; line:35 col:42
  %521 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 3, !dbg !565 ; line:35 col:42
  %522 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 3, !dbg !565 ; line:35 col:42
  %523 = load i32, i32* %522, !dbg !565 ; line:35 col:42
  store i32 %523, i32* %521, !dbg !565 ; line:35 col:42
  %524 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 4, !dbg !565 ; line:35 col:42
  %525 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 4, !dbg !565 ; line:35 col:42
  %526 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %524, i32 0, i32 0, !dbg !565 ; line:35 col:42
  %527 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %525, i32 0, i32 0, !dbg !565 ; line:35 col:42
  %528 = load i16, i16* %527, !dbg !565 ; line:35 col:42
  store i16 %528, i16* %526, !dbg !565 ; line:35 col:42
  %529 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %524, i32 0, i32 1, !dbg !565 ; line:35 col:42
  %530 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %525, i32 0, i32 1, !dbg !565 ; line:35 col:42
  %531 = load i16, i16* %530, !dbg !565 ; line:35 col:42
  store i16 %531, i16* %529, !dbg !565 ; line:35 col:42
  %532 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %524, i32 0, i32 2, !dbg !565 ; line:35 col:42
  %533 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %525, i32 0, i32 2, !dbg !565 ; line:35 col:42
  %534 = load i32, i32* %533, !dbg !565 ; line:35 col:42
  store i32 %534, i32* %532, !dbg !565 ; line:35 col:42
  %535 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %524, i32 0, i32 3, !dbg !565 ; line:35 col:42
  %536 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %525, i32 0, i32 3, !dbg !565 ; line:35 col:42
  %537 = load i32, i32* %536, !dbg !565 ; line:35 col:42
  store i32 %537, i32* %535, !dbg !565 ; line:35 col:42
  %538 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 5, !dbg !565 ; line:35 col:42
  %539 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 5, !dbg !565 ; line:35 col:42
  %540 = load float, float* %539, !dbg !565 ; line:35 col:42
  store float %540, float* %538, !dbg !565 ; line:35 col:42
  %541 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 6, !dbg !565 ; line:35 col:42
  %542 = getelementptr inbounds %struct.texture, %struct.texture* %agg.tmp13, i32 0, i32 6, !dbg !565 ; line:35 col:42
  %543 = load i32, i32* %542, !dbg !565 ; line:35 col:42
  store i32 %543, i32* %541, !dbg !565 ; line:35 col:42
  %544 = getelementptr %struct.texture, %struct.texture* %5, i32 0, i32 0, !dbg !565 ; line:35 col:42
  %545 = load i32, i32* %544, !dbg !565 ; line:35 col:42
  call void @___rpsl_name_resource(i32 %545, i8* getelementptr inbounds ([30 x i8], [30 x i8]* @"@@rps_Str3", i32 0, i32 0), i32 29), !dbg !565 ; line:35 col:42
  %546 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !565 ; line:35 col:42
  %547 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_pre_display_map, i32 0, i32 0, !dbg !565 ; line:35 col:42
  %548 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 0, !dbg !565 ; line:35 col:42
  %549 = load i32, i32* %548, !dbg !565 ; line:35 col:42
  store i32 %549, i32* %547, !dbg !565 ; line:35 col:42
  %550 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_pre_display_map, i32 0, i32 1, !dbg !565 ; line:35 col:42
  %551 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 1, !dbg !565 ; line:35 col:42
  %552 = load i32, i32* %551, !dbg !565 ; line:35 col:42
  store i32 %552, i32* %550, !dbg !565 ; line:35 col:42
  %553 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_pre_display_map, i32 0, i32 2, !dbg !565 ; line:35 col:42
  %554 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 2, !dbg !565 ; line:35 col:42
  %555 = load i32, i32* %554, !dbg !565 ; line:35 col:42
  store i32 %555, i32* %553, !dbg !565 ; line:35 col:42
  %556 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_pre_display_map, i32 0, i32 3, !dbg !565 ; line:35 col:42
  %557 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 3, !dbg !565 ; line:35 col:42
  %558 = load i32, i32* %557, !dbg !565 ; line:35 col:42
  store i32 %558, i32* %556, !dbg !565 ; line:35 col:42
  %559 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_pre_display_map, i32 0, i32 4, !dbg !565 ; line:35 col:42
  %560 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 4, !dbg !565 ; line:35 col:42
  %561 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %559, i32 0, i32 0, !dbg !565 ; line:35 col:42
  %562 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %560, i32 0, i32 0, !dbg !565 ; line:35 col:42
  %563 = load i16, i16* %562, !dbg !565 ; line:35 col:42
  store i16 %563, i16* %561, !dbg !565 ; line:35 col:42
  %564 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %559, i32 0, i32 1, !dbg !565 ; line:35 col:42
  %565 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %560, i32 0, i32 1, !dbg !565 ; line:35 col:42
  %566 = load i16, i16* %565, !dbg !565 ; line:35 col:42
  store i16 %566, i16* %564, !dbg !565 ; line:35 col:42
  %567 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %559, i32 0, i32 2, !dbg !565 ; line:35 col:42
  %568 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %560, i32 0, i32 2, !dbg !565 ; line:35 col:42
  %569 = load i32, i32* %568, !dbg !565 ; line:35 col:42
  store i32 %569, i32* %567, !dbg !565 ; line:35 col:42
  %570 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %559, i32 0, i32 3, !dbg !565 ; line:35 col:42
  %571 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %560, i32 0, i32 3, !dbg !565 ; line:35 col:42
  %572 = load i32, i32* %571, !dbg !565 ; line:35 col:42
  store i32 %572, i32* %570, !dbg !565 ; line:35 col:42
  %573 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_pre_display_map, i32 0, i32 5, !dbg !565 ; line:35 col:42
  %574 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 5, !dbg !565 ; line:35 col:42
  %575 = load float, float* %574, !dbg !565 ; line:35 col:42
  store float %575, float* %573, !dbg !565 ; line:35 col:42
  %576 = getelementptr inbounds %struct.texture, %struct.texture* %game_color_rt_pre_display_map, i32 0, i32 6, !dbg !565 ; line:35 col:42
  %577 = getelementptr inbounds %struct.texture, %struct.texture* %5, i32 0, i32 6, !dbg !565 ; line:35 col:42
  %578 = load i32, i32* %577, !dbg !565 ; line:35 col:42
  store i32 %578, i32* %576, !dbg !565 ; line:35 col:42
  %579 = insertelement <2 x i32> undef, i32 %IMax, i64 0, !dbg !639 ; line:36 col:92
  %580 = insertelement <2 x i32> %579, i32 %IMax219, i64 1, !dbg !639 ; line:36 col:92
  %581 = alloca i8*, i32 6, !dbg !640 ; line:36 col:2
  %582 = bitcast %struct.texture* %game_color_rt_pre_display_map to i8*, !dbg !640 ; line:36 col:2
  %583 = getelementptr i8*, i8** %581, i32 0, !dbg !640 ; line:36 col:2
  store i8* %582, i8** %583, !dbg !640 ; line:36 col:2
  %584 = alloca <2 x i32>, !dbg !640 ; line:36 col:2
  store <2 x i32> zeroinitializer, <2 x i32>* %584, !dbg !640 ; line:36 col:2
  %585 = bitcast <2 x i32>* %584 to i8*, !dbg !640 ; line:36 col:2
  %586 = getelementptr i8*, i8** %581, i32 1, !dbg !640 ; line:36 col:2
  store i8* %585, i8** %586, !dbg !640 ; line:36 col:2
  %587 = bitcast %struct.texture* %game_color_rt_msaa to i8*, !dbg !640 ; line:36 col:2
  %588 = getelementptr i8*, i8** %581, i32 2, !dbg !640 ; line:36 col:2
  store i8* %587, i8** %588, !dbg !640 ; line:36 col:2
  %589 = alloca <2 x i32>, !dbg !640 ; line:36 col:2
  store <2 x i32> zeroinitializer, <2 x i32>* %589, !dbg !640 ; line:36 col:2
  %590 = bitcast <2 x i32>* %589 to i8*, !dbg !640 ; line:36 col:2
  %591 = getelementptr i8*, i8** %581, i32 3, !dbg !640 ; line:36 col:2
  store i8* %590, i8** %591, !dbg !640 ; line:36 col:2
  %592 = alloca <2 x i32>, !dbg !640 ; line:36 col:2
  store <2 x i32> %580, <2 x i32>* %592, !dbg !640 ; line:36 col:2
  %593 = bitcast <2 x i32>* %592 to i8*, !dbg !640 ; line:36 col:2
  %594 = getelementptr i8*, i8** %581, i32 4, !dbg !640 ; line:36 col:2
  store i8* %593, i8** %594, !dbg !640 ; line:36 col:2
  %595 = alloca i32, !dbg !640 ; line:36 col:2
  store i32 0, i32* %595, !dbg !640 ; line:36 col:2
  %596 = bitcast i32* %595 to i8*, !dbg !640 ; line:36 col:2
  %597 = getelementptr i8*, i8** %581, i32 5, !dbg !640 ; line:36 col:2
  store i8* %596, i8** %597, !dbg !640 ; line:36 col:2
  %598 = call i32 @___rpsl_node_call(i32 2, i32 6, i8** %581, i32 0, i32 2), !dbg !640 ; line:36 col:2
  %599 = alloca i8*, i32 2, !dbg !641 ; line:38 col:2
  %600 = bitcast %struct.texture* %game_color_rt to i8*, !dbg !641 ; line:38 col:2
  %601 = getelementptr i8*, i8** %599, i32 0, !dbg !641 ; line:38 col:2
  store i8* %600, i8** %601, !dbg !641 ; line:38 col:2
  %602 = bitcast %struct.texture* %game_color_rt_pre_display_map to i8*, !dbg !641 ; line:38 col:2
  %603 = getelementptr i8*, i8** %599, i32 1, !dbg !641 ; line:38 col:2
  store i8* %602, i8** %603, !dbg !641 ; line:38 col:2
  %604 = call i32 @___rpsl_node_call(i32 3, i32 2, i8** %599, i32 0, i32 3), !dbg !641 ; line:38 col:2
  %605 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !642 ; line:65 col:1
  call void @llvm.dbg.declare(metadata %struct.texture* %3, metadata !222, metadata !215), !dbg !643 ; var:"result" !DIExpression() func:"make_default_texture_view"
  ret void, !dbg !642 ; line:65 col:1
}

; Function Attrs: nounwind
declare void @llvm.memset.p0i8.i32(i8* nocapture, i8, i32, i32, i1) #0

; Function Attrs: nounwind readnone
declare void @llvm.dbg.value(metadata, i64, metadata, metadata) #1

declare void @___rpsl_abort(i32)

declare i32 @___rpsl_node_call(i32, i32, i8**, i32, i32)

declare void @___rpsl_block_marker(i32, i32, i32, i32, i32, i32, i32)

declare void @___rpsl_describe_handle(i8*, i32, i32*, i32)

declare i32 @___rpsl_create_resource(i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32)

declare void @___rpsl_name_resource(i32, i8*, i32)

declare i32 @___rpsl_dxop_binary_i32(i32, i32, i32)

; Function Attrs: noinline nounwind
define void @rpsl_M_renderer_Fn_main_wrapper(i32, i8**, i32) #2 {
body:
  %3 = icmp eq i32 %0, 6, !dbg !644 ; line:14 col:0
  br i1 %3, label %trunk, label %err, !dbg !644 ; line:14 col:0

trunk:                                            ; preds = %body
  %4 = getelementptr i8*, i8** %1, i32 0, !dbg !644 ; line:14 col:0
  %5 = load i8*, i8** %4, !dbg !644 ; line:14 col:0
  %6 = bitcast i8* %5 to %struct.ResourceDesc*, !dbg !644 ; line:14 col:0
  %7 = alloca %struct.texture, !dbg !644 ; line:14 col:0
  %8 = alloca i32, !dbg !644 ; line:14 col:0
  store i32 0, i32* %8, !dbg !644 ; line:14 col:0
  %9 = and i32 %2, 1, !dbg !644 ; line:14 col:0
  %10 = trunc i32 %9 to i1, !dbg !644 ; line:14 col:0
  %11 = bitcast i8* %5 to %struct.texture*, !dbg !644 ; line:14 col:0
  br i1 %10, label %20, label %.preheader.1, !dbg !644 ; line:14 col:0

.preheader.1:                                     ; preds = %trunk
  br label %12, !dbg !644 ; line:14 col:0

err:                                              ; preds = %body
  call void @___rpsl_abort(i32 -3), !dbg !644 ; line:14 col:0
  ret void, !dbg !644 ; line:14 col:0

; <label>:12                                      ; preds = %15, %.preheader.1
  %13 = load i32, i32* %8, !dbg !644 ; line:14 col:0
  %14 = icmp slt i32 %13, 1, !dbg !644 ; line:14 col:0
  br i1 %14, label %15, label %.loopexit.2, !dbg !644 ; line:14 col:0

; <label>:15                                      ; preds = %12
  %16 = getelementptr %struct.ResourceDesc, %struct.ResourceDesc* %6, i32 %13, !dbg !644 ; line:14 col:0
  %17 = getelementptr %struct.texture, %struct.texture* %7, i32 %13, !dbg !644 ; line:14 col:0
  %18 = add i32 %13, 0, !dbg !644 ; line:14 col:0
  call fastcc void @"\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z"(%struct.texture* %17, i32 %18, %struct.ResourceDesc* %16), !dbg !644 ; line:14 col:0
  %19 = add i32 %13, 1, !dbg !644 ; line:14 col:0
  store i32 %19, i32* %8, !dbg !644 ; line:14 col:0
  br label %12, !dbg !644 ; line:14 col:0

.loopexit.2:                                      ; preds = %12
  br label %20, !dbg !644 ; line:14 col:0

; <label>:20                                      ; preds = %.loopexit.2, %trunk
  %21 = phi %struct.texture* [ %11, %trunk ], [ %7, %.loopexit.2 ], !dbg !644 ; line:14 col:0
  %22 = getelementptr i8*, i8** %1, i32 1, !dbg !644 ; line:14 col:0
  %23 = load i8*, i8** %22, !dbg !644 ; line:14 col:0
  %24 = bitcast i8* %23 to %struct.ResourceDesc*, !dbg !644 ; line:14 col:0
  %25 = alloca %struct.texture, !dbg !644 ; line:14 col:0
  %26 = alloca i32, !dbg !644 ; line:14 col:0
  store i32 0, i32* %26, !dbg !644 ; line:14 col:0
  %27 = and i32 %2, 1, !dbg !644 ; line:14 col:0
  %28 = trunc i32 %27 to i1, !dbg !644 ; line:14 col:0
  %29 = bitcast i8* %23 to %struct.texture*, !dbg !644 ; line:14 col:0
  br i1 %28, label %38, label %.preheader, !dbg !644 ; line:14 col:0

.preheader:                                       ; preds = %20
  br label %30, !dbg !644 ; line:14 col:0

; <label>:30                                      ; preds = %33, %.preheader
  %31 = load i32, i32* %26, !dbg !644 ; line:14 col:0
  %32 = icmp slt i32 %31, 1, !dbg !644 ; line:14 col:0
  br i1 %32, label %33, label %.loopexit, !dbg !644 ; line:14 col:0

; <label>:33                                      ; preds = %30
  %34 = getelementptr %struct.ResourceDesc, %struct.ResourceDesc* %24, i32 %31, !dbg !644 ; line:14 col:0
  %35 = getelementptr %struct.texture, %struct.texture* %25, i32 %31, !dbg !644 ; line:14 col:0
  %36 = add i32 %31, 1, !dbg !644 ; line:14 col:0
  call fastcc void @"\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z"(%struct.texture* %35, i32 %36, %struct.ResourceDesc* %34), !dbg !644 ; line:14 col:0
  %37 = add i32 %31, 1, !dbg !644 ; line:14 col:0
  store i32 %37, i32* %26, !dbg !644 ; line:14 col:0
  br label %30, !dbg !644 ; line:14 col:0

.loopexit:                                        ; preds = %30
  br label %38, !dbg !644 ; line:14 col:0

; <label>:38                                      ; preds = %.loopexit, %20
  %39 = phi %struct.texture* [ %29, %20 ], [ %25, %.loopexit ], !dbg !644 ; line:14 col:0
  %40 = getelementptr i8*, i8** %1, i32 2, !dbg !644 ; line:14 col:0
  %41 = load i8*, i8** %40, !dbg !644 ; line:14 col:0
  %42 = bitcast i8* %41 to i1*, !dbg !644 ; line:14 col:0
  %43 = load i1, i1* %42, align 4, !dbg !644 ; line:14 col:0
  %44 = getelementptr i8*, i8** %1, i32 3, !dbg !644 ; line:14 col:0
  %45 = load i8*, i8** %44, !dbg !644 ; line:14 col:0
  %46 = bitcast i8* %45 to <2 x i32>*, !dbg !644 ; line:14 col:0
  %47 = load <2 x i32>, <2 x i32>* %46, align 4, !dbg !644 ; line:14 col:0
  %48 = getelementptr i8*, i8** %1, i32 4, !dbg !644 ; line:14 col:0
  %49 = load i8*, i8** %48, !dbg !644 ; line:14 col:0
  %50 = bitcast i8* %49 to %class.matrix.float.4.4*, !dbg !644 ; line:14 col:0
  %51 = load %class.matrix.float.4.4, %class.matrix.float.4.4* %50, align 4, !dbg !644 ; line:14 col:0
  %52 = getelementptr i8*, i8** %1, i32 5, !dbg !644 ; line:14 col:0
  %53 = load i8*, i8** %52, !dbg !644 ; line:14 col:0
  %54 = bitcast i8* %53 to i32*, !dbg !644 ; line:14 col:0
  %55 = load i32, i32* %54, align 4, !dbg !644 ; line:14 col:0
  call void @rpsl_M_renderer_Fn_main(%struct.texture* %21, %struct.texture* %39, i1 %43, <2 x i32> %47, %class.matrix.float.4.4 %51, i32 %55), !dbg !644 ; line:14 col:0
  ret void
}

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }
attributes #2 = { noinline nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!156, !157}
!llvm.ident = !{!158}
!dx.source.contents = !{!159, !160}
!dx.source.defines = !{!2}
!dx.source.mainFileName = !{!161}
!dx.source.args = !{!162}
!dx.version = !{!163}
!dx.valver = !{!164}
!dx.shaderModel = !{!165}
!dx.typeAnnotations = !{!166, !190}
!dx.entryPoints = !{!210, !212}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "dxc(private) 1.7.0.3789 (rps-merge-dxc1_7_2212_squash, b8686eeb8)", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, retainedTypes: !3, subprograms: !36, globals: !141)
!1 = !DIFile(filename: "renderer.rpsl", directory: "")
!2 = !{}
!3 = !{!4, !6, !15, !16, !26, !34}
!4 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint64_t", file: !1, line: 165, baseType: !5)
!5 = !DIBasicType(name: "uint64_t", size: 64, align: 64, encoding: DW_ATE_unsigned)
!6 = !DIDerivedType(tag: DW_TAG_typedef, name: "int2", file: !1, line: 166, baseType: !7)
!7 = !DICompositeType(tag: DW_TAG_class_type, name: "vector<int, 2>", file: !1, line: 166, size: 64, align: 32, elements: !8, templateParams: !12)
!8 = !{!9, !11}
!9 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !7, file: !1, line: 166, baseType: !10, size: 32, align: 32, flags: DIFlagPublic)
!10 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!11 = !DIDerivedType(tag: DW_TAG_member, name: "y", scope: !7, file: !1, line: 166, baseType: !10, size: 32, align: 32, offset: 32, flags: DIFlagPublic)
!12 = !{!13, !14}
!13 = !DITemplateTypeParameter(name: "element", type: !10)
!14 = !DITemplateValueParameter(name: "element_count", type: !10, value: i32 2)
!15 = !DIBasicType(name: "float", size: 32, align: 32, encoding: DW_ATE_float)
!16 = !DIDerivedType(tag: DW_TAG_typedef, name: "float4", file: !1, line: 29, baseType: !17)
!17 = !DICompositeType(tag: DW_TAG_class_type, name: "vector<float, 4>", file: !1, line: 29, size: 128, align: 32, elements: !18, templateParams: !23)
!18 = !{!19, !20, !21, !22}
!19 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !17, file: !1, line: 29, baseType: !15, size: 32, align: 32, flags: DIFlagPublic)
!20 = !DIDerivedType(tag: DW_TAG_member, name: "y", scope: !17, file: !1, line: 29, baseType: !15, size: 32, align: 32, offset: 32, flags: DIFlagPublic)
!21 = !DIDerivedType(tag: DW_TAG_member, name: "z", scope: !17, file: !1, line: 29, baseType: !15, size: 32, align: 32, offset: 64, flags: DIFlagPublic)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "w", scope: !17, file: !1, line: 29, baseType: !15, size: 32, align: 32, offset: 96, flags: DIFlagPublic)
!23 = !{!24, !25}
!24 = !DITemplateTypeParameter(name: "element", type: !15)
!25 = !DITemplateValueParameter(name: "element_count", type: !10, value: i32 4)
!26 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint2", file: !1, line: 36, baseType: !27)
!27 = !DICompositeType(tag: DW_TAG_class_type, name: "vector<unsigned int, 2>", file: !1, line: 36, size: 64, align: 32, elements: !28, templateParams: !32)
!28 = !{!29, !31}
!29 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !27, file: !1, line: 36, baseType: !30, size: 32, align: 32, flags: DIFlagPublic)
!30 = !DIBasicType(name: "unsigned int", size: 32, align: 32, encoding: DW_ATE_unsigned)
!31 = !DIDerivedType(tag: DW_TAG_member, name: "y", scope: !27, file: !1, line: 36, baseType: !30, size: 32, align: 32, offset: 32, flags: DIFlagPublic)
!32 = !{!33, !14}
!33 = !DITemplateTypeParameter(name: "element", type: !30)
!34 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint16_t", file: !1, line: 128, baseType: !35)
!35 = !DIBasicType(name: "uint16_t", size: 16, align: 16, encoding: DW_ATE_unsigned)
!36 = !{!37, !77, !92, !118, !122, !125, !129, !132, !138}
!37 = !DISubprogram(name: "make_default_texture_view_from_desc", linkageName: "\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z", scope: !38, file: !38, line: 152, type: !39, isLocal: false, isDefinition: true, scopeLine: 153, flags: DIFlagPrototyped, isOptimized: false, function: void (%struct.texture*, i32, %struct.ResourceDesc*)* @"\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z")
!38 = !DIFile(filename: "./___rpsl_builtin_header_.rpsl", directory: "")
!39 = !DISubroutineType(types: !40)
!40 = !{!41, !65, !66}
!41 = !DICompositeType(tag: DW_TAG_structure_type, name: "texture", file: !1, size: 288, align: 32, elements: !42)
!42 = !{!43, !44, !45, !46, !47, !54, !55, !56, !59, !62}
!43 = !DIDerivedType(tag: DW_TAG_member, name: "Resource", scope: !41, file: !1, baseType: !30, size: 32, align: 32)
!44 = !DIDerivedType(tag: DW_TAG_member, name: "ViewFormat", scope: !41, file: !1, baseType: !30, size: 32, align: 32, offset: 32)
!45 = !DIDerivedType(tag: DW_TAG_member, name: "TemporalLayer", scope: !41, file: !1, baseType: !30, size: 32, align: 32, offset: 64)
!46 = !DIDerivedType(tag: DW_TAG_member, name: "Flags", scope: !41, file: !1, baseType: !30, size: 32, align: 32, offset: 96)
!47 = !DIDerivedType(tag: DW_TAG_member, name: "SubresourceRange", scope: !41, file: !1, baseType: !48, size: 96, align: 32, offset: 128)
!48 = !DICompositeType(tag: DW_TAG_structure_type, name: "SubresourceRange", file: !1, size: 96, align: 32, elements: !49)
!49 = !{!50, !51, !52, !53}
!50 = !DIDerivedType(tag: DW_TAG_member, name: "base_mip_level", scope: !48, file: !1, baseType: !35, size: 16, align: 16)
!51 = !DIDerivedType(tag: DW_TAG_member, name: "mip_level_count", scope: !48, file: !1, baseType: !35, size: 16, align: 16, offset: 16)
!52 = !DIDerivedType(tag: DW_TAG_member, name: "base_array_layer", scope: !48, file: !1, baseType: !30, size: 32, align: 32, offset: 32)
!53 = !DIDerivedType(tag: DW_TAG_member, name: "array_layer_count", scope: !48, file: !1, baseType: !30, size: 32, align: 32, offset: 64)
!54 = !DIDerivedType(tag: DW_TAG_member, name: "MinLodClamp", scope: !41, file: !1, baseType: !15, size: 32, align: 32, offset: 224)
!55 = !DIDerivedType(tag: DW_TAG_member, name: "ComponentMapping", scope: !41, file: !1, baseType: !30, size: 32, align: 32, offset: 256)
!56 = !DISubprogram(name: "create1D", linkageName: "\01?create1D@texture@@SA?AU1@IIIIII@Z", scope: !41, file: !1, type: !57, isLocal: false, isDefinition: false, flags: DIFlagPrototyped, isOptimized: false)
!57 = !DISubroutineType(types: !58)
!58 = !{!41, !30, !30, !30, !30, !30, !30}
!59 = !DISubprogram(name: "create2D", linkageName: "\01?create2D@texture@@SA?AU1@IIIIIIIII@Z", scope: !41, file: !1, type: !60, isLocal: false, isDefinition: false, flags: DIFlagPrototyped, isOptimized: false)
!60 = !DISubroutineType(types: !61)
!61 = !{!41, !30, !30, !30, !30, !30, !30, !30, !30, !30}
!62 = !DISubprogram(name: "create3D", linkageName: "\01?create3D@texture@@SA?AU1@IIIIIII@Z", scope: !41, file: !1, type: !63, isLocal: false, isDefinition: false, flags: DIFlagPrototyped, isOptimized: false)
!63 = !DISubroutineType(types: !64)
!64 = !{!41, !30, !30, !30, !30, !30, !30, !30}
!65 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint", file: !1, baseType: !30)
!66 = !DICompositeType(tag: DW_TAG_structure_type, name: "ResourceDesc", file: !1, size: 288, align: 32, elements: !67)
!67 = !{!68, !69, !70, !71, !72, !73, !74, !75, !76}
!68 = !DIDerivedType(tag: DW_TAG_member, name: "Type", scope: !66, file: !1, baseType: !30, size: 32, align: 32)
!69 = !DIDerivedType(tag: DW_TAG_member, name: "TemporalLayers", scope: !66, file: !1, baseType: !30, size: 32, align: 32, offset: 32)
!70 = !DIDerivedType(tag: DW_TAG_member, name: "Flags", scope: !66, file: !1, baseType: !30, size: 32, align: 32, offset: 64)
!71 = !DIDerivedType(tag: DW_TAG_member, name: "Width", scope: !66, file: !1, baseType: !30, size: 32, align: 32, offset: 96)
!72 = !DIDerivedType(tag: DW_TAG_member, name: "Height", scope: !66, file: !1, baseType: !30, size: 32, align: 32, offset: 128)
!73 = !DIDerivedType(tag: DW_TAG_member, name: "DepthOrArraySize", scope: !66, file: !1, baseType: !30, size: 32, align: 32, offset: 160)
!74 = !DIDerivedType(tag: DW_TAG_member, name: "MipLevels", scope: !66, file: !1, baseType: !30, size: 32, align: 32, offset: 192)
!75 = !DIDerivedType(tag: DW_TAG_member, name: "Format", scope: !66, file: !1, baseType: !30, size: 32, align: 32, offset: 224)
!76 = !DIDerivedType(tag: DW_TAG_member, name: "SampleCount", scope: !66, file: !1, baseType: !30, size: 32, align: 32, offset: 256)
!77 = !DISubprogram(name: "make_default_buffer_view_from_desc", linkageName: "\01?make_default_buffer_view_from_desc@@YA?AUbuffer@@IUResourceDesc@@@Z", scope: !38, file: !38, line: 161, type: !78, isLocal: false, isDefinition: true, scopeLine: 162, flags: DIFlagPrototyped, isOptimized: false)
!78 = !DISubroutineType(types: !79)
!79 = !{!80, !65, !66}
!80 = !DICompositeType(tag: DW_TAG_structure_type, name: "buffer", file: !1, line: 159, size: 320, align: 64, elements: !81)
!81 = !{!82, !83, !84, !85, !86, !87, !88, !89}
!82 = !DIDerivedType(tag: DW_TAG_member, name: "Resource", scope: !80, file: !1, line: 159, baseType: !30, size: 32, align: 32)
!83 = !DIDerivedType(tag: DW_TAG_member, name: "ViewFormat", scope: !80, file: !1, line: 159, baseType: !30, size: 32, align: 32, offset: 32)
!84 = !DIDerivedType(tag: DW_TAG_member, name: "TemporalLayer", scope: !80, file: !1, line: 159, baseType: !30, size: 32, align: 32, offset: 64)
!85 = !DIDerivedType(tag: DW_TAG_member, name: "Flags", scope: !80, file: !1, line: 159, baseType: !30, size: 32, align: 32, offset: 96)
!86 = !DIDerivedType(tag: DW_TAG_member, name: "Offset", scope: !80, file: !1, line: 159, baseType: !5, size: 64, align: 64, offset: 128)
!87 = !DIDerivedType(tag: DW_TAG_member, name: "SizeInBytes", scope: !80, file: !1, line: 159, baseType: !5, size: 64, align: 64, offset: 192)
!88 = !DIDerivedType(tag: DW_TAG_member, name: "Stride", scope: !80, file: !1, line: 159, baseType: !30, size: 32, align: 32, offset: 256)
!89 = !DISubprogram(name: "create", linkageName: "\01?create@buffer@@SA?AU1@_KII@Z", scope: !80, file: !1, line: 159, type: !90, isLocal: false, isDefinition: false, scopeLine: 159, flags: DIFlagPrototyped, isOptimized: false)
!90 = !DISubroutineType(types: !91)
!91 = !{!80, !5, !30, !30}
!92 = !DISubprogram(name: "main", linkageName: "\01?main@@YAXUtexture@@0_NV?$vector@H$01@@V?$matrix@M$03$03@@I@Z", scope: !1, file: !1, line: 14, type: !93, isLocal: false, isDefinition: true, scopeLine: 15, flags: DIFlagPrototyped, isOptimized: false, function: void (%struct.texture*, %struct.texture*, i1, <2 x i32>, %class.matrix.float.4.4, i32)* @rpsl_M_renderer_Fn_main)
!93 = !DISubroutineType(types: !94)
!94 = !{null, !41, !41, !95, !6, !96, !65}
!95 = !DIBasicType(name: "bool", size: 32, align: 32, encoding: DW_ATE_boolean)
!96 = !DIDerivedType(tag: DW_TAG_typedef, name: "float4x4", file: !1, line: 166, baseType: !97)
!97 = !DICompositeType(tag: DW_TAG_class_type, name: "matrix<float, 4, 4>", file: !1, line: 166, size: 512, align: 32, elements: !98, templateParams: !115)
!98 = !{!99, !100, !101, !102, !103, !104, !105, !106, !107, !108, !109, !110, !111, !112, !113, !114}
!99 = !DIDerivedType(tag: DW_TAG_member, name: "_11", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, flags: DIFlagPublic)
!100 = !DIDerivedType(tag: DW_TAG_member, name: "_12", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 32, flags: DIFlagPublic)
!101 = !DIDerivedType(tag: DW_TAG_member, name: "_13", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 64, flags: DIFlagPublic)
!102 = !DIDerivedType(tag: DW_TAG_member, name: "_14", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 96, flags: DIFlagPublic)
!103 = !DIDerivedType(tag: DW_TAG_member, name: "_21", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 128, flags: DIFlagPublic)
!104 = !DIDerivedType(tag: DW_TAG_member, name: "_22", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 160, flags: DIFlagPublic)
!105 = !DIDerivedType(tag: DW_TAG_member, name: "_23", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 192, flags: DIFlagPublic)
!106 = !DIDerivedType(tag: DW_TAG_member, name: "_24", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 224, flags: DIFlagPublic)
!107 = !DIDerivedType(tag: DW_TAG_member, name: "_31", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 256, flags: DIFlagPublic)
!108 = !DIDerivedType(tag: DW_TAG_member, name: "_32", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 288, flags: DIFlagPublic)
!109 = !DIDerivedType(tag: DW_TAG_member, name: "_33", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 320, flags: DIFlagPublic)
!110 = !DIDerivedType(tag: DW_TAG_member, name: "_34", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 352, flags: DIFlagPublic)
!111 = !DIDerivedType(tag: DW_TAG_member, name: "_41", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 384, flags: DIFlagPublic)
!112 = !DIDerivedType(tag: DW_TAG_member, name: "_42", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 416, flags: DIFlagPublic)
!113 = !DIDerivedType(tag: DW_TAG_member, name: "_43", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 448, flags: DIFlagPublic)
!114 = !DIDerivedType(tag: DW_TAG_member, name: "_44", scope: !97, file: !1, line: 166, baseType: !15, size: 32, align: 32, offset: 480, flags: DIFlagPublic)
!115 = !{!24, !116, !117}
!116 = !DITemplateValueParameter(name: "row_count", type: !10, value: i32 4)
!117 = !DITemplateValueParameter(name: "col_count", type: !10, value: i32 4)
!118 = !DISubprogram(name: "make_default_texture_view", linkageName: "\01?make_default_texture_view@@YA?AUtexture@@IIII@Z", scope: !38, file: !38, line: 119, type: !119, isLocal: false, isDefinition: true, scopeLine: 120, flags: DIFlagPrototyped, isOptimized: false)
!119 = !DISubroutineType(types: !120)
!120 = !{!41, !65, !121, !65, !65}
!121 = !DIDerivedType(tag: DW_TAG_typedef, name: "RPS_FORMAT", file: !1, line: 65, baseType: !30)
!122 = !DISubprogram(name: "make_default_buffer_view", linkageName: "\01?make_default_buffer_view@@YA?AUbuffer@@I_K@Z", scope: !38, file: !38, line: 137, type: !123, isLocal: false, isDefinition: true, scopeLine: 138, flags: DIFlagPrototyped, isOptimized: false)
!123 = !DISubroutineType(types: !124)
!124 = !{!80, !65, !4}
!125 = !DISubprogram(name: "create_tex2d", linkageName: "\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z", scope: !38, file: !38, line: 216, type: !126, isLocal: false, isDefinition: true, scopeLine: 226, flags: DIFlagPrototyped, isOptimized: false)
!126 = !DISubroutineType(types: !127)
!127 = !{!41, !121, !65, !65, !65, !65, !65, !65, !65, !128}
!128 = !DIDerivedType(tag: DW_TAG_typedef, name: "RPS_RESOURCE_FLAGS", file: !1, line: 361, baseType: !30)
!129 = !DISubprogram(name: "___rpsl_canonicalize_mip_level", linkageName: "\01?___rpsl_canonicalize_mip_level@@YAIIIIII@Z", scope: !38, file: !38, line: 96, type: !130, isLocal: false, isDefinition: true, scopeLine: 97, flags: DIFlagPrototyped, isOptimized: false)
!130 = !DISubroutineType(types: !131)
!131 = !{!65, !65, !65, !65, !65, !65}
!132 = !DISubprogram(name: "clear", linkageName: "\01?clear@@YA?AUnodeidentifier@@Utexture@@V?$vector@M$03@@@Z", scope: !38, file: !38, line: 292, type: !133, isLocal: false, isDefinition: true, scopeLine: 293, flags: DIFlagPrototyped, isOptimized: false)
!133 = !DISubroutineType(types: !134)
!134 = !{!135, !41, !16}
!135 = !DICompositeType(tag: DW_TAG_structure_type, name: "nodeidentifier", file: !38, line: 2, size: 32, align: 32, elements: !136)
!136 = !{!137}
!137 = !DIDerivedType(tag: DW_TAG_member, name: "unused", scope: !135, file: !38, line: 2, baseType: !65, size: 32, align: 32)
!138 = !DISubprogram(name: "clear", linkageName: "\01?clear@@YA?AUnodeidentifier@@Utexture@@MI@Z", scope: !38, file: !38, line: 302, type: !139, isLocal: false, isDefinition: true, scopeLine: 303, flags: DIFlagPrototyped, isOptimized: false)
!139 = !DISubroutineType(types: !140)
!140 = !{!135, !41, !15, !65}
!141 = !{!142, !144, !145, !146, !147, !148, !152, !155}
!142 = !DIGlobalVariable(name: "RPS_RESOURCE_TEX3D", scope: !0, file: !1, line: 157, type: !143, isLocal: true, isDefinition: true, variable: i32 4)
!143 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !30)
!144 = !DIGlobalVariable(name: "RPS_FORMAT_R16G16B16A16_FLOAT", scope: !0, file: !1, line: 22, type: !143, isLocal: true, isDefinition: true, variable: i32 10)
!145 = !DIGlobalVariable(name: "RPS_FORMAT_D32_FLOAT_S8X24_UINT", scope: !0, file: !1, line: 26, type: !143, isLocal: true, isDefinition: true, variable: i32 20)
!146 = !DIGlobalVariable(name: "RPS_RESOLVE_MODE_AVERAGE", scope: !0, file: !1, line: 36, type: !143, isLocal: true, isDefinition: true, variable: i32 0)
!147 = !DIGlobalVariable(name: "RPS_FORMAT_UNKNOWN", scope: !0, file: !1, line: 124, type: !143, isLocal: true, isDefinition: true, variable: i32 0)
!148 = !DIGlobalVariable(name: "___rpsl_pso_tmp", scope: !0, file: !38, line: 361, type: !149, isLocal: true, isDefinition: true)
!149 = !DICompositeType(tag: DW_TAG_structure_type, name: "Pipeline", file: !1, line: 361, size: 32, align: 32, elements: !150)
!150 = !{!151}
!151 = !DIDerivedType(tag: DW_TAG_member, name: "h", scope: !149, file: !1, line: 361, baseType: !10, size: 32, align: 32, flags: DIFlagPrivate)
!152 = !DIGlobalVariable(name: "type", scope: !125, file: !38, line: 227, type: !153, isLocal: true, isDefinition: true)
!153 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !154)
!154 = !DIDerivedType(tag: DW_TAG_typedef, name: "RPS_RESOURCE_TYPE", file: !1, line: 227, baseType: !30)
!155 = !DIGlobalVariable(name: "RPS_CLEAR_DEPTHSTENCIL", scope: !0, file: !1, line: 304, type: !143, isLocal: true, isDefinition: true, variable: i32 6)
!156 = !{i32 2, !"Dwarf Version", i32 4}
!157 = !{i32 2, !"Debug Info Version", i32 3}
!158 = !{!"dxc(private) 1.7.0.3789 (rps-merge-dxc1_7_2212_squash, b8686eeb8)"}
!159 = !{!"renderer.rpsl", !"#include \22___rpsl_builtin_header_.rpsl\22\0Agraphics node draw_triangle(rtv render_target : SV_Target0, dsv depth_target : SV_DepthStencil, float3 color, float one_over_aspect_ratio, float4x4 camera_projection);\0D\0A\0D\0Agraphics node draw_imgui(rtv render_target : SV_Target0, srv game_color_rt, srv texture_acescg);\0D\0A\0D\0Agraphics node aces_to_acescg(rtv render_target : SV_Target0, srv texture_aces);\0D\0A\0D\0Agraphics node oetf(rtv render_target : SV_Target0, srv pre_tonemap_rt);\0D\0A\0D\0Agraphics node display_map_sdr(rtv render_target : SV_Target0, srv input);\0D\0A\0D\0Agraphics node display_map_hdr(rtv render_target : SV_Target0, srv input);\0D\0A\0D\0Aexport void main([readonly(present)] texture backbuffer, srv texture_aces, bool hdr, int2 in_game_viewport_size, float4x4 camera_projection, uint game_color_sample_count) \0D\0A{\0D\0A\09ResourceDesc backbuffer_desc = backbuffer.desc();\0D\0A\0D\0A\09 \0D\0A\0D\0A\09const int2 game_viewport_size = max(in_game_viewport_size, int2(1, 1));\0D\0A\0D\0A\09texture game_color_rt = create_tex2d(RPS_FORMAT_R16G16B16A16_FLOAT, game_viewport_size.x, game_viewport_size.y, 1);\0D\0A\09\09\0D\0A\0D\0A\09texture game_color_rt_msaa = create_tex2d(RPS_FORMAT_R16G16B16A16_FLOAT, game_viewport_size.x, game_viewport_size.y, 2, 1, 1, game_color_sample_count);\0D\0A\09texture game_depth_rt_msaa = create_tex2d(RPS_FORMAT_D32_FLOAT_S8X24_UINT, game_viewport_size.x, game_viewport_size.y, 2, 1, 1, game_color_sample_count);\0D\0A\09float one_over_aspect_ratio = (float)game_viewport_size.x / (float)game_viewport_size.y;\0D\0A\09\09\0D\0A\09clear(game_color_rt_msaa, float4(0.33, 0.33, 0.33, 1.0));\0D\0A\09// Reverse depth\0D\0A\09clear(game_depth_rt_msaa, 0.0f, 0);\0D\0A\0D\0A\09//draw_triangle(game_color_rt_msaa, game_depth_rt_msaa, float3(0.0, 20.5, 0.0), one_over_aspect_ratio, camera_projection);\0D\0A\0D\0A\09texture game_color_rt_pre_display_map = create_tex2d(RPS_FORMAT_R16G16B16A16_FLOAT, game_viewport_size.x, game_viewport_size.y, 2);\0D\0A\09resolve(game_color_rt_pre_display_map, uint2(0, 0), game_color_rt_msaa, uint2(0, 0), uint2(game_viewport_size.x, game_viewport_size.y), RPS_RESOLVE_MODE_AVERAGE);\0D\0A\0D\0A\09display_map_hdr(game_color_rt, game_color_rt_pre_display_map);\0D\0A\0D\0A\09/*\0D\0A\09if(hdr) {\0D\0A\09} else {\0D\0A\09\09display_map_sdr(game_color_rt, game_color_rt_pre_display_map);\0D\0A\09}\0D\0A\09*/\0D\0A\0D\0A\09/*\0D\0A\09ResourceDesc texture_aces_desc = texture_aces.desc();\0D\0A\0D\0A\09texture texture_acescg = create_tex2d(RPS_FORMAT_R32G32B32A32_FLOAT, texture_aces_desc.Width, texture_aces_desc.Height, 1);\0D\0A\09aces_to_acescg(texture_acescg, texture_aces);\0D\0A\09*/\0D\0A\0D\0A\09/*\0D\0A\09texture pre_oetf_rt = hdr ? create_tex2d(RPS_FORMAT_R16G16B16A16_FLOAT, backbuffer_desc.Width, backbuffer_desc.Height, 1) :  backbuffer;\0D\0A\0D\0A\09clear(pre_oetf_rt, float4(0.33, 0.33, 0.33, 1.0));\0D\0A\09draw_imgui(pre_oetf_rt, game_color_rt, null);\0D\0A\0D\0A\09if(hdr) \0D\0A\09{\0D\0A\09\09oetf(backbuffer, pre_oetf_rt);\0D\0A\09}\0D\0A\09*/\0D\0A}"}
!160 = !{!".\5C___rpsl_builtin_header_.rpsl", !"\0Astruct nodeidentifier { uint unused; };\0A#define node nodeidentifier\0A\0Auint ___rpsl_asyncmarker();\0A#define async ___rpsl_asyncmarker();\0A\0Avoid ___rpsl_barrier();\0A#define sch_barrier ___rpsl_barrier\0A\0Avoid ___rpsl_subgraph_begin(uint flags, uint nameOffs, uint nameLen);\0Avoid ___rpsl_subgraph_end();\0A\0Avoid ___rpsl_abort(int errorCode);\0A\0A#define abort ___rpsl_abort\0A\0Atypedef int RpsBool;\0Atypedef RPS_FORMAT RpsFormat;\0A\0A// Syntax sugars\0A#define rtv         [readwrite(rendertarget)] texture\0A#define discard_rtv [writeonly(rendertarget)] texture\0A#define srv         [readonly(ps, cs)] texture\0A#define srv_buf     [readonly(ps, cs)] buffer\0A#define ps_srv      [readonly(ps)] texture\0A#define ps_srv_buf  [readonly(ps)] buffer\0A#define dsv         [readwrite(depth, stencil)] texture\0A#define uav         [readwrite(ps, cs)] texture\0A#define uav_buf     [readwrite(ps, cs)] buffer\0A\0Atexture ___rpsl_set_resource_name(texture h, uint nameOffset, uint nameLength);\0Abuffer ___rpsl_set_resource_name(buffer h, uint nameOffset, uint nameLength);\0A\0Astruct RpsViewport\0A{\0A    float x, y, width, height, minZ, maxZ;\0A};\0A\0Ainline RpsViewport viewport(float x, float y, float width, float height, float minZ = 0.0f, float maxZ = 1.0f)\0A{\0A    RpsViewport result = { x, y, width, height, minZ, maxZ };\0A    return result;\0A}\0A\0Ainline RpsViewport viewport(float width, float height)\0A{\0A    RpsViewport result = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };\0A    return result;\0A}\0A\0Ainline ResourceDesc     describe_resource      ( texture t ) { return t.desc(); }\0Ainline ResourceDesc     describe_resource      ( buffer b  ) { return b.desc(); }\0Ainline ResourceDesc     describe_texture       ( texture t ) { return t.desc(); }\0Ainline ResourceDesc     describe_buffer        ( buffer b  ) { return b.desc(); }\0A\0Auint             ___rpsl_create_resource( RPS_RESOURCE_TYPE  type,\0A                                          RPS_RESOURCE_FLAGS flags,\0A                                          RPS_FORMAT         format,\0A                                          uint               width,\0A                                          uint               height,\0A                                          uint               depthOrArraySize,\0A                                          uint               mipLevels,\0A                                          uint               sampleCount,\0A                                          uint               sampleQuality,\0A                                          uint               temporalLayers,\0A                                          uint               id = 0xFFFFFFFFu );\0A\0A// Built in nodes\0Atemplate<uint MaxRects>\0Agraphics node clear_color_regions( [readwrite(rendertarget, clear)] texture t, float4 data : SV_ClearColor, uint numRects, int4 rects[MaxRects] );\0Atemplate<uint MaxRects>\0Agraphics node clear_depth_stencil_regions( [readwrite(depth, stencil, clear)] texture t, RPS_CLEAR_FLAGS option, float d : SV_ClearDepth, uint s : SV_ClearStencil, uint numRects, int4 rects[MaxRects] );\0Atemplate<uint MaxRects>\0Acompute  node clear_texture_regions( [readwrite(clear)] texture t, uint4 data : SV_ClearColor, uint numRects, int4 rects[MaxRects] );\0A\0Agraphics node    clear_color            ( [writeonly(rendertarget, clear)] texture t, float4 data : SV_ClearColor );\0Agraphics node    clear_depth_stencil    ( [writeonly(depth, stencil, clear)] texture t, RPS_CLEAR_FLAGS option, float d : SV_ClearDepth, uint s : SV_ClearStencil );\0Acompute  node    clear_texture          ( [writeonly(clear)] texture t, uint4 data : SV_ClearColor );\0Acopy     node    clear_buffer           ( [writeonly(clear)] buffer b, uint4 data );\0Acopy     node    copy_texture           ( [readwrite(copy)] texture dst, uint3 dstOffset, [readonly(copy)] texture src, uint3 srcOffset, uint3 extent );\0Acopy     node    copy_buffer            ( [readwrite(copy)] buffer dst, uint64_t dstOffset, [readonly(copy)] buffer src, uint64_t srcOffset, uint64_t size );\0Acopy     node    copy_texture_to_buffer ( [readwrite(copy)] buffer dst, uint64_t dstByteOffset, uint rowPitch, uint3 bufferImageSize, uint3 dstOffset, [readonly(copy)] texture src, uint3 srcOffset, uint3 extent );\0Acopy     node    copy_buffer_to_texture ( [readwrite(copy)] texture dst, uint3 dstOffset, [readonly(copy)] buffer src, uint64_t srcByteOffset, uint rowPitch, uint3 bufferImageSize, uint3 srcOffset, uint3 extent );\0Agraphics node    resolve                ( [readwrite(resolve)] texture dst, uint2 dstOffset, [readonly(resolve)] texture src, uint2 srcOffset, uint2 extent, RPS_RESOLVE_MODE resolveMode );\0A\0Agraphics node    draw                   ( uint vertexCountPerInstance, uint instanceCount, uint startVertexLocation, uint startInstanceLocation );\0Agraphics node    draw_indexed           ( uint indexCountPerInstance, uint instanceCount, uint startIndexLocation, int baseVertexLocation, uint startInstanceLocation );\0Acompute  node    dispatch               ( uint3 numGroups );\0Acompute  node    dispatch_threads       ( uint3 numThreads );\0Agraphics node    draw_indirect          ( buffer args, uint64_t offset, uint32_t drawCount, uint32_t stride );\0Agraphics node    draw_indexed_indirect  ( buffer args, uint64_t offset, uint32_t drawCount, uint32_t stride );\0A\0Avoid             bind                   ( Pipeline pso );\0A\0Ainline uint ___rpsl_canonicalize_mip_level(uint inMipLevel, uint width, uint height = 1, uint depth = 1, uint sampleCount = 1)\0A{\0A    if (sampleCount > 1)\0A    {\0A        return 1;\0A    }\0A\0A    uint32_t w    = width;\0A    uint32_t h    = height;\0A    uint32_t d    = depth;\0A    uint32_t mips = 1;\0A\0A    while ((w > 1) || (h > 1) || (d > 1))\0A    {\0A        mips++;\0A        w = w >> 1;\0A        h = h >> 1;\0A        d = d >> 1;\0A    }\0A\0A    return (inMipLevel == 0) ? mips : min(inMipLevel, mips);\0A}\0A\0Ainline texture make_default_texture_view( uint resourceHdl, RPS_FORMAT format, uint arraySlices, uint numMips )\0A{\0A    texture result;\0A\0A    result.Resource = resourceHdl;\0A    result.ViewFormat = RPS_FORMAT_UNKNOWN;\0A    result.TemporalLayer = 0;\0A    result.Flags = 0;\0A    result.SubresourceRange.base_mip_level = 0;\0A    result.SubresourceRange.mip_level_count = uint16_t(numMips);\0A    result.SubresourceRange.base_array_layer = 0;\0A    result.SubresourceRange.array_layer_count = arraySlices;\0A    result.MinLodClamp = 0.0f;\0A    result.ComponentMapping = 0x3020100; // Default channel mapping\0A\0A    return result;\0A}\0A\0Ainline buffer make_default_buffer_view( uint resourceHdl, uint64_t size )\0A{\0A    buffer result;\0A\0A    result.Resource = resourceHdl;\0A    result.ViewFormat = 0;\0A    result.TemporalLayer = 0;\0A    result.Flags = 0;\0A    result.Offset = 0;\0A    result.SizeInBytes = size;\0A    result.Stride = 0;\0A\0A    return result;\0A}\0A\0Atexture make_default_texture_view_from_desc( uint resourceHdl, ResourceDesc desc )\0A{\0A    return make_default_texture_view(\0A        resourceHdl,\0A        desc.Format,\0A        (desc.Type == RPS_RESOURCE_TEX3D) ? 1 : desc.DepthOrArraySize,\0A        desc.MipLevels);\0A}\0A\0Abuffer make_default_buffer_view_from_desc( uint resourceHdl, ResourceDesc desc )\0A{\0A    return make_default_buffer_view(\0A        resourceHdl,\0A        (uint64_t(desc.Height) << 32u) | uint64_t(desc.Width));\0A}\0A\0Ainline texture create_texture( ResourceDesc desc )\0A{\0A    // TODO: Report error if type is not texture.\0A\0A    desc.MipLevels = ___rpsl_canonicalize_mip_level(desc.MipLevels,\0A                                                    desc.Width,\0A                                                    (desc.Type != RPS_RESOURCE_TEX1D) ? desc.Height : 1,\0A                                                    (desc.Type == RPS_RESOURCE_TEX3D) ? desc.DepthOrArraySize : 1,\0A                                                    desc.SampleCount);\0A\0A    uint resourceHdl = ___rpsl_create_resource( desc.Type, desc.Flags, desc.Format, desc.Width, desc.Height, desc.DepthOrArraySize, desc.MipLevels, desc.SampleCount, 0, desc.TemporalLayers );\0A\0A    uint arraySize = (desc.Type != RPS_RESOURCE_TEX3D) ? desc.DepthOrArraySize : 1;\0A\0A    return make_default_texture_view( resourceHdl, desc.Format, arraySize, desc.MipLevels );\0A}\0A\0Ainline buffer create_buffer( ResourceDesc desc )\0A{\0A    // TODO: Report error if type is not buffer.\0A\0A    uint resourceHdl = ___rpsl_create_resource( desc.Type, desc.Flags, desc.Format, desc.Width, desc.Height, desc.DepthOrArraySize, desc.MipLevels, desc.SampleCount, 0, desc.TemporalLayers );\0A\0A    uint64_t byteWidth = ((uint64_t)(desc.Height) << 32ULL) | ((uint64_t)(desc.Width));\0A\0A    return make_default_buffer_view( resourceHdl, byteWidth );\0A}\0A\0Ainline texture create_tex1d(\0A    RPS_FORMAT format,\0A    uint width,\0A    uint numMips = 1,\0A    uint arraySlices = 1,\0A    uint numTemporalLayers = 1,\0A    RPS_RESOURCE_FLAGS flags = RPS_RESOURCE_FLAG_NONE )\0A{\0A    const RPS_RESOURCE_TYPE type = RPS_RESOURCE_TEX1D;\0A    const uint height = 1;\0A    const uint sampleCount = 1;\0A    const uint sampleQuality = 0;\0A\0A    numMips = ___rpsl_canonicalize_mip_level(numMips, width, height, 1, sampleCount);\0A\0A    uint resourceHdl = ___rpsl_create_resource( type, flags, format, width, height, arraySlices, numMips, sampleCount, sampleQuality, numTemporalLayers );\0A\0A    return make_default_texture_view( resourceHdl, format, arraySlices, numMips );\0A}\0A\0Ainline texture create_tex2d(\0A    RPS_FORMAT format,\0A    uint width,\0A    uint height,\0A    uint numMips = 1,\0A    uint arraySlices = 1,\0A    uint numTemporalLayers = 1,\0A    uint sampleCount = 1,\0A    uint sampleQuality = 0,\0A    RPS_RESOURCE_FLAGS flags = RPS_RESOURCE_FLAG_NONE )\0A{\0A    const RPS_RESOURCE_TYPE type = RPS_RESOURCE_TEX2D;\0A\0A    numMips = ___rpsl_canonicalize_mip_level(numMips, width, height, 1, sampleCount);\0A\0A    const uint resourceHdl = ___rpsl_create_resource( type, flags, format, width, height, arraySlices, numMips, sampleCount, sampleQuality, numTemporalLayers );\0A\0A    return make_default_texture_view( resourceHdl, format, arraySlices, numMips );\0A}\0A\0Ainline texture create_tex3d(\0A    RPS_FORMAT format,\0A    uint width,\0A    uint height,\0A    uint depth,\0A    uint numMips = 1,\0A    uint numTemporalLayers = 1,\0A    RPS_RESOURCE_FLAGS flags = RPS_RESOURCE_FLAG_NONE )\0A{\0A    const RPS_RESOURCE_TYPE type = RPS_RESOURCE_TEX3D;\0A    const uint sampleCount = 1;\0A    const uint sampleQuality = 0;\0A    const uint arraySlices = 1;\0A\0A    numMips = ___rpsl_canonicalize_mip_level(numMips, width, height, depth, sampleCount);\0A\0A    uint resourceHdl = ___rpsl_create_resource( type, flags, format, width, height, depth, numMips, sampleCount, sampleQuality, numTemporalLayers );\0A\0A    return make_default_texture_view( resourceHdl, format, arraySlices, numMips );\0A}\0A\0Ainline buffer create_buffer( uint64_t width, uint numTemporalLayers = 1, RPS_RESOURCE_FLAGS flags = RPS_RESOURCE_FLAG_NONE )\0A{\0A    uint resourceHdl = ___rpsl_create_resource( RPS_RESOURCE_BUFFER,\0A                                              flags,\0A                                              RPS_FORMAT_UNKNOWN,\0A                                              (uint)width,\0A                                              (uint)(width >> 32ULL),\0A                                              1, 1, 1, 0, numTemporalLayers );\0A\0A    return make_default_buffer_view( resourceHdl, width );\0A}\0A\0Ainline texture create_texture_view(\0A    texture r,\0A    uint baseMip = 0,\0A    uint mipLevels = 1,\0A    uint baseArraySlice = 0,\0A    uint numArraySlices = 1,\0A    uint temporalLayer = 0,\0A    RPS_FORMAT format = RPS_FORMAT_UNKNOWN )\0A{\0A    return r.temporal(temporalLayer).mips(baseMip, mipLevels).array(baseArraySlice, numArraySlices).format(format);\0A}\0A\0Ainline buffer create_buffer_view(\0A    buffer r,\0A    uint64_t offset = 0,\0A    uint64_t sizeInBytes = 0,\0A    uint temporalLayer = 0,\0A    RPS_FORMAT format = RPS_FORMAT_UNKNOWN,\0A    uint structureStride = 0 )\0A{\0A    return r.temporal(temporalLayer).bytes(offset, sizeInBytes).format(format).stride(structureStride);\0A}\0A\0Ainline node clear( texture t, float4 val )\0A{\0A    return clear_color( t, val );\0A}\0A\0Ainline node clear( texture t, uint4 val )\0A{\0A    return clear_color( t, float4(val) );\0A}\0A\0Ainline node clear( texture t, float depth, uint stencil )\0A{\0A    return clear_depth_stencil( t, RPS_CLEAR_DEPTHSTENCIL, depth, stencil );\0A}\0A\0Ainline node clear_depth( texture t, float depth )\0A{\0A    return clear_depth_stencil( t, RPS_CLEAR_DEPTH, depth, 0 );\0A}\0A\0Ainline node clear_stencil( texture t, uint stencil )\0A{\0A    return clear_depth_stencil( t, RPS_CLEAR_STENCIL, 0.0f, stencil );\0A}\0A\0Atemplate<uint MaxRects>\0Ainline node clear_depth_regions( texture t, float depth, int4 rects[MaxRects] )\0A{\0A    return clear_depth_stencil_regions( t, RPS_CLEAR_DEPTH, depth, 0, MaxRects, rects );\0A}\0A\0Atemplate<uint MaxRects>\0Ainline node clear_stencil_regions( texture t, uint stencil, int4 rects[MaxRects] )\0A{\0A    return clear_depth_stencil_regions( t, RPS_CLEAR_STENCIL, 0.0f, stencil, MaxRects, rects );\0A}\0A\0Ainline node clear_uav( texture t, float4 val )\0A{\0A    return clear_texture( t, asuint(val) );\0A}\0A\0Ainline node clear_uav( texture t, uint4 val )\0A{\0A    return clear_texture( t, val );\0A}\0A\0Ainline node clear( buffer b, float4 val )\0A{\0A    return clear_buffer( b, asuint(val) );\0A}\0A\0Ainline node clear( buffer b, uint4 val )\0A{\0A    return clear_buffer( b, val );\0A}\0A\0Ainline node copy_texture( texture dst, texture src )\0A{\0A    return copy_texture( dst, uint3(0, 0, 0), src, uint3(0, 0, 0), uint3(0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU) );\0A}\0A\0Ainline node copy_buffer( buffer dst, buffer src )\0A{\0A    return copy_buffer( dst, 0, src, 0, 0xFFFFFFFFFFFFFFFFULL );\0A}\0A\0A// Experimental Pipeline states\0A\0Astatic Pipeline ___rpsl_pso_tmp;\0A\0Atemplate<typename T>\0Avoid ___rpsl_pso_modify_field(Pipeline p, uint i, T val);\0A\0Aenum RPS_DEPTH_WRITE_MASK {\0A  RPS_DEPTH_WRITE_MASK_ZERO,\0A  RPS_DEPTH_WRITE_MASK_ALL\0A};\0A\0Aenum RPS_COMPARISON_FUNC {\0A  RPS_COMPARISON_FUNC_NEVER,\0A  RPS_COMPARISON_FUNC_LESS,\0A  RPS_COMPARISON_FUNC_EQUAL,\0A  RPS_COMPARISON_FUNC_LESS_EQUAL,\0A  RPS_COMPARISON_FUNC_GREATER,\0A  RPS_COMPARISON_FUNC_NOT_EQUAL,\0A  RPS_COMPARISON_FUNC_GREATER_EQUAL,\0A  RPS_COMPARISON_FUNC_ALWAYS\0A};\0A\0Aenum RPS_STENCIL_OP {\0A  RPS_STENCIL_OP_KEEP,\0A  RPS_STENCIL_OP_ZERO,\0A  RPS_STENCIL_OP_REPLACE,\0A  RPS_STENCIL_OP_INCR_SAT,\0A  RPS_STENCIL_OP_DECR_SAT,\0A  RPS_STENCIL_OP_INVERT,\0A  RPS_STENCIL_OP_INCR,\0A  RPS_STENCIL_OP_DECR\0A};\0A\0Astruct RpsStencilOpDesc {\0A  RPS_STENCIL_OP      StencilFailOp;\0A  RPS_STENCIL_OP      StencilDepthFailOp;\0A  RPS_STENCIL_OP      StencilPassOp;\0A  RPS_COMPARISON_FUNC StencilFunc;\0A};\0A\0Astruct RpsDepthStencilDesc {\0A  RpsBool              DepthEnable;\0A  RPS_DEPTH_WRITE_MASK DepthWriteMask;\0A  RPS_COMPARISON_FUNC  DepthFunc;\0A  RpsBool              StencilEnable;\0A  uint                 StencilReadMask;\0A  uint                 StencilWriteMask;\0A  RpsStencilOpDesc     FrontFace;\0A  RpsStencilOpDesc     BackFace;\0A};\0A\0Aenum RPS_BLEND {\0A  RPS_BLEND_ZERO = 0,\0A  RPS_BLEND_ONE,\0A  RPS_BLEND_SRC_COLOR,\0A  RPS_BLEND_INV_SRC_COLOR,\0A  RPS_BLEND_SRC_ALPHA,\0A  RPS_BLEND_INV_SRC_ALPHA,\0A  RPS_BLEND_DEST_ALPHA,\0A  RPS_BLEND_INV_DEST_ALPHA,\0A  RPS_BLEND_DEST_COLOR,\0A  RPS_BLEND_INV_DEST_COLOR,\0A  RPS_BLEND_SRC_ALPHA_SAT,\0A  RPS_BLEND_BLEND_FACTOR,\0A  RPS_BLEND_INV_BLEND_FACTOR,\0A  RPS_BLEND_SRC1_COLOR,\0A  RPS_BLEND_INV_SRC1_COLOR,\0A  RPS_BLEND_SRC1_ALPHA,\0A  RPS_BLEND_INV_SRC1_ALPHA\0A};\0A\0Aenum RPS_BLEND_OP {\0A  RPS_BLEND_OP_ADD,\0A  RPS_BLEND_OP_SUBTRACT,\0A  RPS_BLEND_OP_REV_SUBTRACT,\0A  RPS_BLEND_OP_MIN,\0A  RPS_BLEND_OP_MAX\0A};\0A\0Aenum RPS_LOGIC_OP {\0A  RPS_LOGIC_OP_CLEAR = 0,\0A  RPS_LOGIC_OP_SET,\0A  RPS_LOGIC_OP_COPY,\0A  RPS_LOGIC_OP_COPY_INVERTED,\0A  RPS_LOGIC_OP_NOOP,\0A  RPS_LOGIC_OP_INVERT,\0A  RPS_LOGIC_OP_AND,\0A  RPS_LOGIC_OP_NAND,\0A  RPS_LOGIC_OP_OR,\0A  RPS_LOGIC_OP_NOR,\0A  RPS_LOGIC_OP_XOR,\0A  RPS_LOGIC_OP_EQUIV,\0A  RPS_LOGIC_OP_AND_REVERSE,\0A  RPS_LOGIC_OP_AND_INVERTED,\0A  RPS_LOGIC_OP_OR_REVERSE,\0A  RPS_LOGIC_OP_OR_INVERTED\0A};\0A\0Astruct RpsRenderTargetBlendDesc {\0A  RpsBool      BlendEnable;\0A  RpsBool      LogicOpEnable;\0A  RPS_BLEND    SrcBlend;\0A  RPS_BLEND    DestBlend;\0A  RPS_BLEND_OP BlendOp;\0A  RPS_BLEND    SrcBlendAlpha;\0A  RPS_BLEND    DestBlendAlpha;\0A  RPS_BLEND_OP BlendOpAlpha;\0A  RPS_LOGIC_OP LogicOp;\0A  uint         RenderTargetWriteMask;\0A};\0A\0Astruct RpsBlendDesc {\0A  RpsBool AlphaToCoverageEnable;\0A  RpsBool IndependentBlendEnable;\0A};\0A\0A#define per_vertex \22per_vertex\22\0A#define per_instance \22per_instance\22\0A\0ARpsVertexLayout vertex_layout(uint stride);\0A"}
!161 = !{!"renderer.rpsl"}
!162 = !{!"-T", !"rps_6_2", !"-Vd", !"-default-linkage", !"external", !"-res_may_alias", !"-Zi", !"-Qembed_debug", !"-enable-16bit-types", !"-O0", !"-Wno-for-redefinition", !"-Wno-comma-in-init", !"-rps-module-name", !"renderer", !"-HV", !"2021"}
!163 = !{i32 1, i32 2}
!164 = !{i32 0, i32 0}
!165 = !{!"rps", i32 6, i32 2}
!166 = !{i32 0, %struct.texture undef, !167, %struct.SubresourceRange undef, !175, %struct.ResourceDesc undef, !180}
!167 = !{i32 36, !168, !169, !170, !171, !172, !173, !174}
!168 = !{i32 6, !"Resource", i32 3, i32 0, i32 7, i32 5}
!169 = !{i32 6, !"ViewFormat", i32 3, i32 4, i32 7, i32 5}
!170 = !{i32 6, !"TemporalLayer", i32 3, i32 8, i32 7, i32 5}
!171 = !{i32 6, !"Flags", i32 3, i32 12, i32 7, i32 5}
!172 = !{i32 6, !"SubresourceRange", i32 3, i32 16}
!173 = !{i32 6, !"MinLodClamp", i32 3, i32 28, i32 7, i32 9}
!174 = !{i32 6, !"ComponentMapping", i32 3, i32 32, i32 7, i32 5}
!175 = !{i32 12, !176, !177, !178, !179}
!176 = !{i32 6, !"base_mip_level", i32 3, i32 0, i32 7, i32 3}
!177 = !{i32 6, !"mip_level_count", i32 3, i32 2, i32 7, i32 3}
!178 = !{i32 6, !"base_array_layer", i32 3, i32 4, i32 7, i32 5}
!179 = !{i32 6, !"array_layer_count", i32 3, i32 8, i32 7, i32 5}
!180 = !{i32 36, !181, !182, !183, !184, !185, !186, !187, !188, !189}
!181 = !{i32 6, !"Type", i32 3, i32 0, i32 7, i32 5}
!182 = !{i32 6, !"TemporalLayers", i32 3, i32 4, i32 7, i32 5}
!183 = !{i32 6, !"Flags", i32 3, i32 8, i32 7, i32 5}
!184 = !{i32 6, !"Width", i32 3, i32 12, i32 7, i32 5}
!185 = !{i32 6, !"Height", i32 3, i32 16, i32 7, i32 5}
!186 = !{i32 6, !"DepthOrArraySize", i32 3, i32 20, i32 7, i32 5}
!187 = !{i32 6, !"MipLevels", i32 3, i32 24, i32 7, i32 5}
!188 = !{i32 6, !"Format", i32 3, i32 28, i32 7, i32 5}
!189 = !{i32 6, !"SampleCount", i32 3, i32 32, i32 7, i32 5}
!190 = !{i32 1, void (%struct.texture*, i32, %struct.ResourceDesc*)* @"\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z", !191, void (%struct.texture*, %struct.texture*, i1, <2 x i32>, %class.matrix.float.4.4, i32)* @rpsl_M_renderer_Fn_main, !196}
!191 = !{!192, !193, !194, !192}
!192 = !{i32 0, !2, !2}
!193 = !{i32 1, !2, !2}
!194 = !{i32 0, !195, !2}
!195 = !{i32 7, i32 5}
!196 = !{!193, !197, !199, !201, !203, !205, !208}
!197 = !{i32 0, !198, !2}
!198 = !{i32 6, !"backbuffer"}
!199 = !{i32 0, !200, !2}
!200 = !{i32 6, !"texture_aces"}
!201 = !{i32 0, !202, !2}
!202 = !{i32 6, !"hdr", i32 7, i32 1}
!203 = !{i32 0, !204, !2}
!204 = !{i32 6, !"in_game_viewport_size", i32 7, i32 4}
!205 = !{i32 0, !206, !2}
!206 = !{i32 6, !"camera_projection", i32 2, !207, i32 7, i32 9}
!207 = !{i32 4, i32 4, i32 2}
!208 = !{i32 0, !209, !2}
!209 = !{i32 6, !"game_color_sample_count", i32 7, i32 5}
!210 = !{null, !"", null, null, !211}
!211 = !{i32 0, i64 8388640}
!212 = !{void (%struct.texture*, %struct.texture*, i1, <2 x i32>, %class.matrix.float.4.4, i32)* @rpsl_M_renderer_Fn_main, !"rpsl_M_renderer_Fn_main", null, null, !213}
!213 = !{i32 8, i32 15}
!214 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "desc", arg: 2, scope: !37, file: !38, line: 152, type: !66)
!215 = !DIExpression()
!216 = !DILocation(line: 152, column: 77, scope: !37)
!217 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "resourceHdl", arg: 1, scope: !37, file: !38, line: 152, type: !65)
!218 = !DILocation(line: 152, column: 51, scope: !37)
!219 = !DILocation(line: 158, column: 14, scope: !37)
!220 = !DILocation(line: 157, column: 15, scope: !37)
!221 = !DILocation(line: 157, column: 20, scope: !37)
!222 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "result", arg: 0, scope: !118, file: !38, line: 121, type: !41)
!223 = !DILocation(line: 121, column: 13, scope: !118, inlinedAt: !224)
!224 = distinct !DILocation(line: 154, column: 12, scope: !37)
!225 = !DILocation(line: 157, column: 9, scope: !37)
!226 = !DILocation(line: 157, column: 54, scope: !37)
!227 = !DILocation(line: 154, column: 12, scope: !37)
!228 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "numMips", arg: 4, scope: !118, file: !38, line: 119, type: !65)
!229 = !DILocation(line: 119, column: 103, scope: !118, inlinedAt: !224)
!230 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "arraySlices", arg: 3, scope: !118, file: !38, line: 119, type: !65)
!231 = !DILocation(line: 119, column: 85, scope: !118, inlinedAt: !224)
!232 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "resourceHdl", arg: 1, scope: !118, file: !38, line: 119, type: !65)
!233 = !DILocation(line: 119, column: 48, scope: !118, inlinedAt: !224)
!234 = !{!235}
!235 = distinct !{!235, !236, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z: %agg.result"}
!236 = distinct !{!236, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z"}
!237 = !DILocation(line: 123, column: 12, scope: !118, inlinedAt: !224)
!238 = !DILocation(line: 123, column: 21, scope: !118, inlinedAt: !224)
!239 = !DILocation(line: 124, column: 12, scope: !118, inlinedAt: !224)
!240 = !DILocation(line: 124, column: 23, scope: !118, inlinedAt: !224)
!241 = !DILocation(line: 125, column: 12, scope: !118, inlinedAt: !224)
!242 = !DILocation(line: 125, column: 26, scope: !118, inlinedAt: !224)
!243 = !DILocation(line: 126, column: 12, scope: !118, inlinedAt: !224)
!244 = !DILocation(line: 126, column: 18, scope: !118, inlinedAt: !224)
!245 = !DILocation(line: 127, column: 29, scope: !118, inlinedAt: !224)
!246 = !DILocation(line: 127, column: 44, scope: !118, inlinedAt: !224)
!247 = !DILocation(line: 128, column: 56, scope: !118, inlinedAt: !224)
!248 = !DILocation(line: 128, column: 29, scope: !118, inlinedAt: !224)
!249 = !DILocation(line: 128, column: 45, scope: !118, inlinedAt: !224)
!250 = !DILocation(line: 129, column: 29, scope: !118, inlinedAt: !224)
!251 = !DILocation(line: 129, column: 46, scope: !118, inlinedAt: !224)
!252 = !DILocation(line: 130, column: 29, scope: !118, inlinedAt: !224)
!253 = !DILocation(line: 130, column: 47, scope: !118, inlinedAt: !224)
!254 = !DILocation(line: 131, column: 12, scope: !118, inlinedAt: !224)
!255 = !DILocation(line: 131, column: 24, scope: !118, inlinedAt: !224)
!256 = !DILocation(line: 132, column: 12, scope: !118, inlinedAt: !224)
!257 = !DILocation(line: 132, column: 29, scope: !118, inlinedAt: !224)
!258 = !DILocation(line: 134, column: 5, scope: !118, inlinedAt: !224)
!259 = !DILocation(line: 154, column: 5, scope: !37)
!260 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "game_color_sample_count", arg: 6, scope: !92, file: !1, line: 14, type: !65)
!261 = !DILocation(line: 14, column: 147, scope: !92)
!262 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "in_game_viewport_size", arg: 4, scope: !92, file: !1, line: 14, type: !6)
!263 = !DILocation(line: 14, column: 91, scope: !92)
!264 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "texture_aces", arg: 2, scope: !92, file: !1, line: 14, type: !41)
!265 = !DILocation(line: 14, column: 62, scope: !92)
!266 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "backbuffer", arg: 1, scope: !92, file: !1, line: 14, type: !41)
!267 = !DILocation(line: 14, column: 46, scope: !92)
!268 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "backbuffer_desc", scope: !92, file: !1, line: 16, type: !66)
!269 = !DILocation(line: 16, column: 15, scope: !92)
!270 = !DILocation(line: 16, column: 33, scope: !92)
!271 = !DILocation(line: 20, column: 34, scope: !92)
!272 = !DILocation(line: 20, column: 13, scope: !92)
!273 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "game_viewport_size", scope: !92, file: !1, line: 20, type: !274)
!274 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !6)
!275 = !DIExpression(DW_OP_bit_piece, 0, 32)
!276 = !DIExpression(DW_OP_bit_piece, 32, 32)
!277 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "game_color_rt", scope: !92, file: !1, line: 22, type: !41)
!278 = !DILocation(line: 22, column: 10, scope: !92)
!279 = !DILocation(line: 22, column: 26, scope: !92)
!280 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "flags", arg: 9, scope: !125, file: !38, line: 225, type: !128)
!281 = !DILocation(line: 225, column: 24, scope: !125, inlinedAt: !282)
!282 = distinct !DILocation(line: 22, column: 26, scope: !92)
!283 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "sampleQuality", arg: 8, scope: !125, file: !38, line: 224, type: !65)
!284 = !DILocation(line: 224, column: 10, scope: !125, inlinedAt: !282)
!285 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "sampleCount", arg: 7, scope: !125, file: !38, line: 223, type: !65)
!286 = !DILocation(line: 223, column: 10, scope: !125, inlinedAt: !282)
!287 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "numTemporalLayers", arg: 6, scope: !125, file: !38, line: 222, type: !65)
!288 = !DILocation(line: 222, column: 10, scope: !125, inlinedAt: !282)
!289 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "arraySlices", arg: 5, scope: !125, file: !38, line: 221, type: !65)
!290 = !DILocation(line: 221, column: 10, scope: !125, inlinedAt: !282)
!291 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "numMips", arg: 4, scope: !125, file: !38, line: 220, type: !65)
!292 = !DILocation(line: 220, column: 10, scope: !125, inlinedAt: !282)
!293 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "height", arg: 3, scope: !125, file: !38, line: 219, type: !65)
!294 = !DILocation(line: 219, column: 10, scope: !125, inlinedAt: !282)
!295 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "width", arg: 2, scope: !125, file: !38, line: 218, type: !65)
!296 = !DILocation(line: 218, column: 10, scope: !125, inlinedAt: !282)
!297 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "format", arg: 1, scope: !125, file: !38, line: 217, type: !121)
!298 = !DILocation(line: 217, column: 16, scope: !125, inlinedAt: !282)
!299 = !DILocation(line: 229, column: 15, scope: !125, inlinedAt: !282)
!300 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "sampleCount", arg: 5, scope: !129, file: !38, line: 96, type: !65)
!301 = !DILocation(line: 96, column: 111, scope: !129, inlinedAt: !302)
!302 = distinct !DILocation(line: 229, column: 15, scope: !125, inlinedAt: !282)
!303 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "depth", arg: 4, scope: !129, file: !38, line: 96, type: !65)
!304 = !DILocation(line: 96, column: 95, scope: !129, inlinedAt: !302)
!305 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "height", arg: 3, scope: !129, file: !38, line: 96, type: !65)
!306 = !DILocation(line: 96, column: 78, scope: !129, inlinedAt: !302)
!307 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "width", arg: 2, scope: !129, file: !38, line: 96, type: !65)
!308 = !DILocation(line: 96, column: 66, scope: !129, inlinedAt: !302)
!309 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "inMipLevel", arg: 1, scope: !129, file: !38, line: 96, type: !65)
!310 = !DILocation(line: 96, column: 49, scope: !129, inlinedAt: !302)
!311 = !DILocation(line: 121, column: 13, scope: !118, inlinedAt: !312)
!312 = distinct !DILocation(line: 233, column: 12, scope: !125, inlinedAt: !313)
!313 = distinct !DILocation(line: 25, column: 31, scope: !92)
!314 = !DILocation(line: 121, column: 13, scope: !118, inlinedAt: !315)
!315 = distinct !DILocation(line: 233, column: 12, scope: !125, inlinedAt: !316)
!316 = distinct !DILocation(line: 26, column: 31, scope: !92)
!317 = !DILocation(line: 121, column: 13, scope: !118, inlinedAt: !318)
!318 = distinct !DILocation(line: 233, column: 12, scope: !125, inlinedAt: !319)
!319 = distinct !DILocation(line: 35, column: 42, scope: !92)
!320 = !DILocation(line: 98, column: 9, scope: !129, inlinedAt: !302)
!321 = !DILocation(line: 103, column: 14, scope: !129, inlinedAt: !302)
!322 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "w", scope: !129, file: !38, line: 103, type: !323)
!323 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint32_t", file: !1, line: 103, baseType: !30)
!324 = !DILocation(line: 104, column: 14, scope: !129, inlinedAt: !302)
!325 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "h", scope: !129, file: !38, line: 104, type: !323)
!326 = !DILocation(line: 105, column: 14, scope: !129, inlinedAt: !302)
!327 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "d", scope: !129, file: !38, line: 105, type: !323)
!328 = !DILocation(line: 106, column: 14, scope: !129, inlinedAt: !302)
!329 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "mips", scope: !129, file: !38, line: 106, type: !323)
!330 = !DILocation(line: 108, column: 5, scope: !129, inlinedAt: !302)
!331 = !DILocation(line: 108, column: 15, scope: !129, inlinedAt: !302)
!332 = !DILocation(line: 108, column: 20, scope: !129, inlinedAt: !302)
!333 = !DILocation(line: 108, column: 26, scope: !129, inlinedAt: !302)
!334 = !DILocation(line: 108, column: 31, scope: !129, inlinedAt: !302)
!335 = !DILocation(line: 108, column: 37, scope: !129, inlinedAt: !302)
!336 = !DILocation(line: 110, column: 13, scope: !337, inlinedAt: !302)
!337 = distinct !DILexicalBlock(scope: !129, file: !38, line: 109, column: 5)
!338 = !DILocation(line: 111, column: 15, scope: !337, inlinedAt: !302)
!339 = !DILocation(line: 111, column: 11, scope: !337, inlinedAt: !302)
!340 = !DILocation(line: 112, column: 15, scope: !337, inlinedAt: !302)
!341 = !DILocation(line: 112, column: 11, scope: !337, inlinedAt: !302)
!342 = !DILocation(line: 113, column: 15, scope: !337, inlinedAt: !302)
!343 = !DILocation(line: 113, column: 11, scope: !337, inlinedAt: !302)
!344 = !DILocation(line: 116, column: 12, scope: !129, inlinedAt: !302)
!345 = !DILocation(line: 116, column: 39, scope: !129, inlinedAt: !302)
!346 = !DILocation(line: 116, column: 5, scope: !129, inlinedAt: !302)
!347 = !DILocation(line: 117, column: 1, scope: !129, inlinedAt: !302)
!348 = !DILocation(line: 231, column: 30, scope: !125, inlinedAt: !282)
!349 = !{!350}
!350 = distinct !{!350, !351, !"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z: %agg.result"}
!351 = distinct !{!351, !"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z"}
!352 = !DILocation(line: 231, column: 16, scope: !125, inlinedAt: !282)
!353 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "resourceHdl", scope: !125, file: !38, line: 231, type: !354)
!354 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !65)
!355 = !DILocation(line: 233, column: 12, scope: !125, inlinedAt: !282)
!356 = !DILocation(line: 119, column: 103, scope: !118, inlinedAt: !357)
!357 = distinct !DILocation(line: 233, column: 12, scope: !125, inlinedAt: !282)
!358 = !DILocation(line: 119, column: 85, scope: !118, inlinedAt: !357)
!359 = !DILocation(line: 119, column: 48, scope: !118, inlinedAt: !357)
!360 = !{!361, !350}
!361 = distinct !{!361, !362, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z: %agg.result"}
!362 = distinct !{!362, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z"}
!363 = !DILocation(line: 123, column: 12, scope: !118, inlinedAt: !357)
!364 = !DILocation(line: 123, column: 21, scope: !118, inlinedAt: !357)
!365 = !DILocation(line: 124, column: 12, scope: !118, inlinedAt: !357)
!366 = !DILocation(line: 124, column: 23, scope: !118, inlinedAt: !357)
!367 = !DILocation(line: 125, column: 12, scope: !118, inlinedAt: !357)
!368 = !DILocation(line: 125, column: 26, scope: !118, inlinedAt: !357)
!369 = !DILocation(line: 126, column: 12, scope: !118, inlinedAt: !357)
!370 = !DILocation(line: 126, column: 18, scope: !118, inlinedAt: !357)
!371 = !DILocation(line: 127, column: 29, scope: !118, inlinedAt: !357)
!372 = !DILocation(line: 127, column: 44, scope: !118, inlinedAt: !357)
!373 = !DILocation(line: 128, column: 56, scope: !118, inlinedAt: !357)
!374 = !DILocation(line: 128, column: 29, scope: !118, inlinedAt: !357)
!375 = !DILocation(line: 128, column: 45, scope: !118, inlinedAt: !357)
!376 = !DILocation(line: 129, column: 29, scope: !118, inlinedAt: !357)
!377 = !DILocation(line: 129, column: 46, scope: !118, inlinedAt: !357)
!378 = !DILocation(line: 130, column: 29, scope: !118, inlinedAt: !357)
!379 = !DILocation(line: 130, column: 47, scope: !118, inlinedAt: !357)
!380 = !DILocation(line: 131, column: 12, scope: !118, inlinedAt: !357)
!381 = !DILocation(line: 131, column: 24, scope: !118, inlinedAt: !357)
!382 = !DILocation(line: 132, column: 12, scope: !118, inlinedAt: !357)
!383 = !DILocation(line: 132, column: 29, scope: !118, inlinedAt: !357)
!384 = !DILocation(line: 134, column: 5, scope: !118, inlinedAt: !357)
!385 = !DILocation(line: 233, column: 5, scope: !125, inlinedAt: !282)
!386 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "game_color_rt_msaa", scope: !92, file: !1, line: 25, type: !41)
!387 = !DILocation(line: 25, column: 10, scope: !92)
!388 = !DILocation(line: 25, column: 31, scope: !92)
!389 = !DILocation(line: 225, column: 24, scope: !125, inlinedAt: !313)
!390 = !DILocation(line: 224, column: 10, scope: !125, inlinedAt: !313)
!391 = !DILocation(line: 223, column: 10, scope: !125, inlinedAt: !313)
!392 = !DILocation(line: 222, column: 10, scope: !125, inlinedAt: !313)
!393 = !DILocation(line: 221, column: 10, scope: !125, inlinedAt: !313)
!394 = !DILocation(line: 220, column: 10, scope: !125, inlinedAt: !313)
!395 = !DILocation(line: 219, column: 10, scope: !125, inlinedAt: !313)
!396 = !DILocation(line: 218, column: 10, scope: !125, inlinedAt: !313)
!397 = !DILocation(line: 217, column: 16, scope: !125, inlinedAt: !313)
!398 = !DILocation(line: 229, column: 15, scope: !125, inlinedAt: !313)
!399 = !DILocation(line: 96, column: 111, scope: !129, inlinedAt: !400)
!400 = distinct !DILocation(line: 229, column: 15, scope: !125, inlinedAt: !313)
!401 = !DILocation(line: 96, column: 95, scope: !129, inlinedAt: !400)
!402 = !DILocation(line: 96, column: 78, scope: !129, inlinedAt: !400)
!403 = !DILocation(line: 96, column: 66, scope: !129, inlinedAt: !400)
!404 = !DILocation(line: 96, column: 49, scope: !129, inlinedAt: !400)
!405 = !DILocation(line: 98, column: 21, scope: !406, inlinedAt: !400)
!406 = distinct !DILexicalBlock(scope: !129, file: !38, line: 98, column: 9)
!407 = !DILocation(line: 98, column: 9, scope: !129, inlinedAt: !400)
!408 = !DILocation(line: 100, column: 9, scope: !409, inlinedAt: !400)
!409 = distinct !DILexicalBlock(scope: !406, file: !38, line: 99, column: 5)
!410 = !DILocation(line: 103, column: 14, scope: !129, inlinedAt: !400)
!411 = !DILocation(line: 104, column: 14, scope: !129, inlinedAt: !400)
!412 = !DILocation(line: 105, column: 14, scope: !129, inlinedAt: !400)
!413 = !DILocation(line: 106, column: 14, scope: !129, inlinedAt: !400)
!414 = !DILocation(line: 108, column: 5, scope: !129, inlinedAt: !400)
!415 = !DILocation(line: 108, column: 15, scope: !129, inlinedAt: !400)
!416 = !DILocation(line: 108, column: 20, scope: !129, inlinedAt: !400)
!417 = !DILocation(line: 108, column: 26, scope: !129, inlinedAt: !400)
!418 = !DILocation(line: 108, column: 31, scope: !129, inlinedAt: !400)
!419 = !DILocation(line: 108, column: 37, scope: !129, inlinedAt: !400)
!420 = !DILocation(line: 110, column: 13, scope: !337, inlinedAt: !400)
!421 = !DILocation(line: 111, column: 15, scope: !337, inlinedAt: !400)
!422 = !DILocation(line: 111, column: 11, scope: !337, inlinedAt: !400)
!423 = !DILocation(line: 112, column: 15, scope: !337, inlinedAt: !400)
!424 = !DILocation(line: 112, column: 11, scope: !337, inlinedAt: !400)
!425 = !DILocation(line: 113, column: 15, scope: !337, inlinedAt: !400)
!426 = !DILocation(line: 113, column: 11, scope: !337, inlinedAt: !400)
!427 = !DILocation(line: 116, column: 12, scope: !129, inlinedAt: !400)
!428 = !DILocation(line: 116, column: 39, scope: !129, inlinedAt: !400)
!429 = !DILocation(line: 116, column: 5, scope: !129, inlinedAt: !400)
!430 = !DILocation(line: 117, column: 1, scope: !129, inlinedAt: !400)
!431 = !DILocation(line: 231, column: 30, scope: !125, inlinedAt: !313)
!432 = !{!433}
!433 = distinct !{!433, !434, !"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z: %agg.result"}
!434 = distinct !{!434, !"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z"}
!435 = !DILocation(line: 231, column: 16, scope: !125, inlinedAt: !313)
!436 = !DILocation(line: 233, column: 12, scope: !125, inlinedAt: !313)
!437 = !DILocation(line: 119, column: 103, scope: !118, inlinedAt: !312)
!438 = !DILocation(line: 119, column: 85, scope: !118, inlinedAt: !312)
!439 = !DILocation(line: 119, column: 48, scope: !118, inlinedAt: !312)
!440 = !{!441, !433}
!441 = distinct !{!441, !442, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z: %agg.result"}
!442 = distinct !{!442, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z"}
!443 = !DILocation(line: 123, column: 12, scope: !118, inlinedAt: !312)
!444 = !DILocation(line: 123, column: 21, scope: !118, inlinedAt: !312)
!445 = !DILocation(line: 124, column: 12, scope: !118, inlinedAt: !312)
!446 = !DILocation(line: 124, column: 23, scope: !118, inlinedAt: !312)
!447 = !DILocation(line: 125, column: 12, scope: !118, inlinedAt: !312)
!448 = !DILocation(line: 125, column: 26, scope: !118, inlinedAt: !312)
!449 = !DILocation(line: 126, column: 12, scope: !118, inlinedAt: !312)
!450 = !DILocation(line: 126, column: 18, scope: !118, inlinedAt: !312)
!451 = !DILocation(line: 127, column: 29, scope: !118, inlinedAt: !312)
!452 = !DILocation(line: 127, column: 44, scope: !118, inlinedAt: !312)
!453 = !DILocation(line: 128, column: 56, scope: !118, inlinedAt: !312)
!454 = !DILocation(line: 128, column: 29, scope: !118, inlinedAt: !312)
!455 = !DILocation(line: 128, column: 45, scope: !118, inlinedAt: !312)
!456 = !DILocation(line: 129, column: 29, scope: !118, inlinedAt: !312)
!457 = !DILocation(line: 129, column: 46, scope: !118, inlinedAt: !312)
!458 = !DILocation(line: 130, column: 29, scope: !118, inlinedAt: !312)
!459 = !DILocation(line: 130, column: 47, scope: !118, inlinedAt: !312)
!460 = !DILocation(line: 131, column: 12, scope: !118, inlinedAt: !312)
!461 = !DILocation(line: 131, column: 24, scope: !118, inlinedAt: !312)
!462 = !DILocation(line: 132, column: 12, scope: !118, inlinedAt: !312)
!463 = !DILocation(line: 132, column: 29, scope: !118, inlinedAt: !312)
!464 = !DILocation(line: 134, column: 5, scope: !118, inlinedAt: !312)
!465 = !DILocation(line: 233, column: 5, scope: !125, inlinedAt: !313)
!466 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "game_depth_rt_msaa", scope: !92, file: !1, line: 26, type: !41)
!467 = !DILocation(line: 26, column: 10, scope: !92)
!468 = !DILocation(line: 26, column: 31, scope: !92)
!469 = !DILocation(line: 225, column: 24, scope: !125, inlinedAt: !316)
!470 = !DILocation(line: 224, column: 10, scope: !125, inlinedAt: !316)
!471 = !DILocation(line: 223, column: 10, scope: !125, inlinedAt: !316)
!472 = !DILocation(line: 222, column: 10, scope: !125, inlinedAt: !316)
!473 = !DILocation(line: 221, column: 10, scope: !125, inlinedAt: !316)
!474 = !DILocation(line: 220, column: 10, scope: !125, inlinedAt: !316)
!475 = !DILocation(line: 219, column: 10, scope: !125, inlinedAt: !316)
!476 = !DILocation(line: 218, column: 10, scope: !125, inlinedAt: !316)
!477 = !DILocation(line: 217, column: 16, scope: !125, inlinedAt: !316)
!478 = !DILocation(line: 229, column: 15, scope: !125, inlinedAt: !316)
!479 = !DILocation(line: 96, column: 111, scope: !129, inlinedAt: !480)
!480 = distinct !DILocation(line: 229, column: 15, scope: !125, inlinedAt: !316)
!481 = !DILocation(line: 96, column: 95, scope: !129, inlinedAt: !480)
!482 = !DILocation(line: 96, column: 78, scope: !129, inlinedAt: !480)
!483 = !DILocation(line: 96, column: 66, scope: !129, inlinedAt: !480)
!484 = !DILocation(line: 96, column: 49, scope: !129, inlinedAt: !480)
!485 = !DILocation(line: 98, column: 21, scope: !406, inlinedAt: !480)
!486 = !DILocation(line: 98, column: 9, scope: !129, inlinedAt: !480)
!487 = !DILocation(line: 100, column: 9, scope: !409, inlinedAt: !480)
!488 = !DILocation(line: 103, column: 14, scope: !129, inlinedAt: !480)
!489 = !DILocation(line: 104, column: 14, scope: !129, inlinedAt: !480)
!490 = !DILocation(line: 105, column: 14, scope: !129, inlinedAt: !480)
!491 = !DILocation(line: 106, column: 14, scope: !129, inlinedAt: !480)
!492 = !DILocation(line: 108, column: 5, scope: !129, inlinedAt: !480)
!493 = !DILocation(line: 108, column: 15, scope: !129, inlinedAt: !480)
!494 = !DILocation(line: 108, column: 20, scope: !129, inlinedAt: !480)
!495 = !DILocation(line: 108, column: 26, scope: !129, inlinedAt: !480)
!496 = !DILocation(line: 108, column: 31, scope: !129, inlinedAt: !480)
!497 = !DILocation(line: 108, column: 37, scope: !129, inlinedAt: !480)
!498 = !DILocation(line: 110, column: 13, scope: !337, inlinedAt: !480)
!499 = !DILocation(line: 111, column: 15, scope: !337, inlinedAt: !480)
!500 = !DILocation(line: 111, column: 11, scope: !337, inlinedAt: !480)
!501 = !DILocation(line: 112, column: 15, scope: !337, inlinedAt: !480)
!502 = !DILocation(line: 112, column: 11, scope: !337, inlinedAt: !480)
!503 = !DILocation(line: 113, column: 15, scope: !337, inlinedAt: !480)
!504 = !DILocation(line: 113, column: 11, scope: !337, inlinedAt: !480)
!505 = !DILocation(line: 116, column: 12, scope: !129, inlinedAt: !480)
!506 = !DILocation(line: 116, column: 39, scope: !129, inlinedAt: !480)
!507 = !DILocation(line: 116, column: 5, scope: !129, inlinedAt: !480)
!508 = !DILocation(line: 117, column: 1, scope: !129, inlinedAt: !480)
!509 = !DILocation(line: 231, column: 30, scope: !125, inlinedAt: !316)
!510 = !{!511}
!511 = distinct !{!511, !512, !"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z: %agg.result"}
!512 = distinct !{!512, !"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z"}
!513 = !DILocation(line: 231, column: 16, scope: !125, inlinedAt: !316)
!514 = !DILocation(line: 233, column: 12, scope: !125, inlinedAt: !316)
!515 = !DILocation(line: 119, column: 103, scope: !118, inlinedAt: !315)
!516 = !DILocation(line: 119, column: 85, scope: !118, inlinedAt: !315)
!517 = !DILocation(line: 119, column: 48, scope: !118, inlinedAt: !315)
!518 = !{!519, !511}
!519 = distinct !{!519, !520, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z: %agg.result"}
!520 = distinct !{!520, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z"}
!521 = !DILocation(line: 123, column: 12, scope: !118, inlinedAt: !315)
!522 = !DILocation(line: 123, column: 21, scope: !118, inlinedAt: !315)
!523 = !DILocation(line: 124, column: 12, scope: !118, inlinedAt: !315)
!524 = !DILocation(line: 124, column: 23, scope: !118, inlinedAt: !315)
!525 = !DILocation(line: 125, column: 12, scope: !118, inlinedAt: !315)
!526 = !DILocation(line: 125, column: 26, scope: !118, inlinedAt: !315)
!527 = !DILocation(line: 126, column: 12, scope: !118, inlinedAt: !315)
!528 = !DILocation(line: 126, column: 18, scope: !118, inlinedAt: !315)
!529 = !DILocation(line: 127, column: 29, scope: !118, inlinedAt: !315)
!530 = !DILocation(line: 127, column: 44, scope: !118, inlinedAt: !315)
!531 = !DILocation(line: 128, column: 56, scope: !118, inlinedAt: !315)
!532 = !DILocation(line: 128, column: 29, scope: !118, inlinedAt: !315)
!533 = !DILocation(line: 128, column: 45, scope: !118, inlinedAt: !315)
!534 = !DILocation(line: 129, column: 29, scope: !118, inlinedAt: !315)
!535 = !DILocation(line: 129, column: 46, scope: !118, inlinedAt: !315)
!536 = !DILocation(line: 130, column: 29, scope: !118, inlinedAt: !315)
!537 = !DILocation(line: 130, column: 47, scope: !118, inlinedAt: !315)
!538 = !DILocation(line: 131, column: 12, scope: !118, inlinedAt: !315)
!539 = !DILocation(line: 131, column: 24, scope: !118, inlinedAt: !315)
!540 = !DILocation(line: 132, column: 12, scope: !118, inlinedAt: !315)
!541 = !DILocation(line: 132, column: 29, scope: !118, inlinedAt: !315)
!542 = !DILocation(line: 134, column: 5, scope: !118, inlinedAt: !315)
!543 = !DILocation(line: 233, column: 5, scope: !125, inlinedAt: !316)
!544 = !DILocation(line: 27, column: 8, scope: !92)
!545 = !DILocation(line: 29, column: 2, scope: !92)
!546 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "val", arg: 2, scope: !132, file: !38, line: 292, type: !16)
!547 = !DILocation(line: 292, column: 38, scope: !132, inlinedAt: !548)
!548 = distinct !DILocation(line: 29, column: 2, scope: !92)
!549 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "t", arg: 1, scope: !132, file: !38, line: 292, type: !41)
!550 = !DILocation(line: 292, column: 28, scope: !132, inlinedAt: !548)
!551 = !DILocation(line: 294, column: 12, scope: !132, inlinedAt: !548)
!552 = !DILocation(line: 294, column: 5, scope: !132, inlinedAt: !548)
!553 = !DILocation(line: 31, column: 2, scope: !92)
!554 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "stencil", arg: 3, scope: !138, file: !38, line: 302, type: !65)
!555 = !DILocation(line: 302, column: 49, scope: !138, inlinedAt: !556)
!556 = distinct !DILocation(line: 31, column: 2, scope: !92)
!557 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "depth", arg: 2, scope: !138, file: !38, line: 302, type: !15)
!558 = !DILocation(line: 302, column: 37, scope: !138, inlinedAt: !556)
!559 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "t", arg: 1, scope: !138, file: !38, line: 302, type: !41)
!560 = !DILocation(line: 302, column: 28, scope: !138, inlinedAt: !556)
!561 = !DILocation(line: 304, column: 12, scope: !138, inlinedAt: !556)
!562 = !DILocation(line: 304, column: 5, scope: !138, inlinedAt: !556)
!563 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "game_color_rt_pre_display_map", scope: !92, file: !1, line: 35, type: !41)
!564 = !DILocation(line: 35, column: 10, scope: !92)
!565 = !DILocation(line: 35, column: 42, scope: !92)
!566 = !DILocation(line: 225, column: 24, scope: !125, inlinedAt: !319)
!567 = !DILocation(line: 224, column: 10, scope: !125, inlinedAt: !319)
!568 = !DILocation(line: 223, column: 10, scope: !125, inlinedAt: !319)
!569 = !DILocation(line: 222, column: 10, scope: !125, inlinedAt: !319)
!570 = !DILocation(line: 221, column: 10, scope: !125, inlinedAt: !319)
!571 = !DILocation(line: 220, column: 10, scope: !125, inlinedAt: !319)
!572 = !DILocation(line: 219, column: 10, scope: !125, inlinedAt: !319)
!573 = !DILocation(line: 218, column: 10, scope: !125, inlinedAt: !319)
!574 = !DILocation(line: 217, column: 16, scope: !125, inlinedAt: !319)
!575 = !DILocation(line: 229, column: 15, scope: !125, inlinedAt: !319)
!576 = !DILocation(line: 96, column: 111, scope: !129, inlinedAt: !577)
!577 = distinct !DILocation(line: 229, column: 15, scope: !125, inlinedAt: !319)
!578 = !DILocation(line: 96, column: 95, scope: !129, inlinedAt: !577)
!579 = !DILocation(line: 96, column: 78, scope: !129, inlinedAt: !577)
!580 = !DILocation(line: 96, column: 66, scope: !129, inlinedAt: !577)
!581 = !DILocation(line: 96, column: 49, scope: !129, inlinedAt: !577)
!582 = !DILocation(line: 98, column: 9, scope: !129, inlinedAt: !577)
!583 = !DILocation(line: 103, column: 14, scope: !129, inlinedAt: !577)
!584 = !DILocation(line: 104, column: 14, scope: !129, inlinedAt: !577)
!585 = !DILocation(line: 105, column: 14, scope: !129, inlinedAt: !577)
!586 = !DILocation(line: 106, column: 14, scope: !129, inlinedAt: !577)
!587 = !DILocation(line: 108, column: 5, scope: !129, inlinedAt: !577)
!588 = !DILocation(line: 108, column: 15, scope: !129, inlinedAt: !577)
!589 = !DILocation(line: 108, column: 20, scope: !129, inlinedAt: !577)
!590 = !DILocation(line: 108, column: 26, scope: !129, inlinedAt: !577)
!591 = !DILocation(line: 108, column: 31, scope: !129, inlinedAt: !577)
!592 = !DILocation(line: 108, column: 37, scope: !129, inlinedAt: !577)
!593 = !DILocation(line: 110, column: 13, scope: !337, inlinedAt: !577)
!594 = !DILocation(line: 111, column: 15, scope: !337, inlinedAt: !577)
!595 = !DILocation(line: 111, column: 11, scope: !337, inlinedAt: !577)
!596 = !DILocation(line: 112, column: 15, scope: !337, inlinedAt: !577)
!597 = !DILocation(line: 112, column: 11, scope: !337, inlinedAt: !577)
!598 = !DILocation(line: 113, column: 15, scope: !337, inlinedAt: !577)
!599 = !DILocation(line: 113, column: 11, scope: !337, inlinedAt: !577)
!600 = !DILocation(line: 116, column: 12, scope: !129, inlinedAt: !577)
!601 = !DILocation(line: 116, column: 39, scope: !129, inlinedAt: !577)
!602 = !DILocation(line: 116, column: 5, scope: !129, inlinedAt: !577)
!603 = !DILocation(line: 117, column: 1, scope: !129, inlinedAt: !577)
!604 = !DILocation(line: 231, column: 30, scope: !125, inlinedAt: !319)
!605 = !{!606}
!606 = distinct !{!606, !607, !"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z: %agg.result"}
!607 = distinct !{!607, !"\01?create_tex2d@@YA?AUtexture@@IIIIIIIII@Z"}
!608 = !DILocation(line: 231, column: 16, scope: !125, inlinedAt: !319)
!609 = !DILocation(line: 233, column: 12, scope: !125, inlinedAt: !319)
!610 = !DILocation(line: 119, column: 103, scope: !118, inlinedAt: !318)
!611 = !DILocation(line: 119, column: 85, scope: !118, inlinedAt: !318)
!612 = !DILocation(line: 119, column: 48, scope: !118, inlinedAt: !318)
!613 = !{!614, !606}
!614 = distinct !{!614, !615, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z: %agg.result"}
!615 = distinct !{!615, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z"}
!616 = !DILocation(line: 123, column: 12, scope: !118, inlinedAt: !318)
!617 = !DILocation(line: 123, column: 21, scope: !118, inlinedAt: !318)
!618 = !DILocation(line: 124, column: 12, scope: !118, inlinedAt: !318)
!619 = !DILocation(line: 124, column: 23, scope: !118, inlinedAt: !318)
!620 = !DILocation(line: 125, column: 12, scope: !118, inlinedAt: !318)
!621 = !DILocation(line: 125, column: 26, scope: !118, inlinedAt: !318)
!622 = !DILocation(line: 126, column: 12, scope: !118, inlinedAt: !318)
!623 = !DILocation(line: 126, column: 18, scope: !118, inlinedAt: !318)
!624 = !DILocation(line: 127, column: 29, scope: !118, inlinedAt: !318)
!625 = !DILocation(line: 127, column: 44, scope: !118, inlinedAt: !318)
!626 = !DILocation(line: 128, column: 56, scope: !118, inlinedAt: !318)
!627 = !DILocation(line: 128, column: 29, scope: !118, inlinedAt: !318)
!628 = !DILocation(line: 128, column: 45, scope: !118, inlinedAt: !318)
!629 = !DILocation(line: 129, column: 29, scope: !118, inlinedAt: !318)
!630 = !DILocation(line: 129, column: 46, scope: !118, inlinedAt: !318)
!631 = !DILocation(line: 130, column: 29, scope: !118, inlinedAt: !318)
!632 = !DILocation(line: 130, column: 47, scope: !118, inlinedAt: !318)
!633 = !DILocation(line: 131, column: 12, scope: !118, inlinedAt: !318)
!634 = !DILocation(line: 131, column: 24, scope: !118, inlinedAt: !318)
!635 = !DILocation(line: 132, column: 12, scope: !118, inlinedAt: !318)
!636 = !DILocation(line: 132, column: 29, scope: !118, inlinedAt: !318)
!637 = !DILocation(line: 134, column: 5, scope: !118, inlinedAt: !318)
!638 = !DILocation(line: 233, column: 5, scope: !125, inlinedAt: !319)
!639 = !DILocation(line: 36, column: 92, scope: !92)
!640 = !DILocation(line: 36, column: 2, scope: !92)
!641 = !DILocation(line: 38, column: 2, scope: !92)
!642 = !DILocation(line: 65, column: 1, scope: !92)
!643 = !DILocation(line: 121, column: 13, scope: !118, inlinedAt: !357)
!644 = !DILocation(line: 14, scope: !92)
 