#include "game.h"

#include "cfgreader.h"
#include "error.h"
#include "filesystem.h"
#include "logger.h"
#include "types.h"

namespace blaz {

Error Game::load_game(str path) {
    pair<Error, Cfg> cfg = read_cfg_file(path);
    if (cfg.first) {
        return cfg.first;
    }
    Cfg game_cfg = cfg.second;

    Cfg shaders = game_cfg["shaders"];
    for (auto& shader_cfg : shaders) {
        Shader shader;
        shader.m_vertex_shader_path = shader_cfg["vertex_shader_path"].str_value;
        shader.m_fragment_shader_path = shader_cfg["fragment_shader_path"].str_value;
        shader.m_should_reload = true;
        m_renderer.m_shaders.push_back(shader);
    }

    Cfg meshes = game_cfg["meshes"];
    for (auto& mesh_cfg : meshes) {
        Mesh mesh;
        mesh.m_name = mesh_cfg["name"].str_value;
        mesh.m_vertices = mesh_cfg["vertices"].buffer_f32_value;
        mesh.m_indices = mesh_cfg["indices"].buffer_u32_value;
        for (auto& attrib : mesh_cfg["attribs"]) {
            mesh.m_attribs.push_back(
                std::make_pair(attrib[0].str_value, (u32)attrib[1].float_value));
        }
        m_renderer.m_meshes.push_back(mesh);
    }

    Cfg levels = game_cfg["levels"];
    for (auto& level_cfg : levels) {
        Level level;
        level.m_name = level_cfg["name"].str_value;
        for (auto& node_cfg : level_cfg["nodes"]) {
            Node node;
            node.m_name = node_cfg["name"].str_value;
            node.m_position = node_cfg["position"].vec3_value;
            node.m_rotation = node_cfg["rotation"].vec4_value;
            node.m_scale = node_cfg["scale"].vec3_value;
            level.m_scene.m_nodes.push_back(node);
        }
        for (auto& renderables_cfg : level_cfg["renderables"]) {
            Renderable renderable;
            // node.m_name = node_cfg["name"].str_value;
            // node.m_position = node_cfg["position"].vec3_value;
            // node.m_rotation = node_cfg["rotation"].vec4_value;
            // node.m_scale = node_cfg["scale"].vec3_value;
            // level.m_scene.m_nodes.push_back(node);
        }
        m_levels.push_back(level);
    }

    // Cfg pipelines = game_cfg["pipelines"];
    // for (auto& pipeline_cfg : pipelines) {
    //     Cfg passes_cfg = pipeline_cfg["passes"];
    //     for (auto& pass_cfg : passes_cfg) {
    //         Pass pass;

    //         if (pass_cfg["name"]) pass.m_name = pass_cfg["name"].str_value;

    //         vec<CfgNode> clears = pass_cfg["clear"].array_value;
    //         u32 clear_flag = 0;
    //         for (auto& clear : clears) clear_flag |= ClearStr[clear.str_value];
    //         pass.m_clear_flag = clear_flag;

    //         pass.m_type = PassTypeStr[pass_cfg["type"].str_value];

    //         if(pass_cfg["clear_color"]) pass.m_clear_color =
    //         pass_cfg["clear_color"].vec4_value; if(pass_cfg["clear_depth"])
    //         pass.m_clear_depth = pass_cfg["clear_depth"].float_value;

    //             // .m_shader = pass_cfg["shader"],
    //             // .m_tags = pass_cfg["tags"],
    //             // .m_framebuffer = pass_cfg["framebuffer"],
    //             // .m_camera = pass_cfg["camera"],
    //             // .m_enabled = pass_cfg["enabled"],
    //             // .m_use_default_framebuffer = pass_cfg["use_default_framebuffer"],
    //             // .m_enable_depth_test = pass_cfg["enable_depth_test"],
    //             // .m_enable_face_culling = pass_cfg["enable_face_culling"],
    //             // .m_culling_mode = pass_cfg["culling_mode"],
    //         //};
    //         // logger.info(pass["name"].str_value);
    //     }
    // }

    return Error();
}

Error Game::load_level(u32 level) {
    m_current_level = level;
    return Error();
}

}  // namespace blaz