#include "game.h"

#include "camera.h"
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

    for (auto& shader_cfg : game_cfg["shaders"]) {
        Shader shader;
        shader.m_name = shader_cfg["name"].str_value;
        shader.m_vertex_shader_path = shader_cfg["vertex_shader_path"].str_value;
        shader.m_fragment_shader_path = shader_cfg["fragment_shader_path"].str_value;
        shader.m_should_reload = true;
        m_renderer.m_shaders.push_back(shader);
        m_renderer.m_shaders_ids[shader.m_name] = u32(m_renderer.m_shaders.size()) - 1;
    }

    for (auto& mesh_cfg : game_cfg["meshes"]) {
        Mesh mesh;
        mesh.m_name = mesh_cfg["name"].str_value;
        mesh.m_vertices = mesh_cfg["vertices"].buffer_f32_value;
        mesh.m_indices = mesh_cfg["indices"].buffer_u32_value;
        if (mesh_cfg["primitive"]) {
            mesh.m_primitive = MeshPrimitiveStr[mesh_cfg["primitive"].str_value];
        }
        for (auto& attrib : mesh_cfg["attribs"]) {
            mesh.m_attribs.push_back(
                std::make_pair(attrib[0].str_value, u32(attrib[1].float_value)));
        }
        m_renderer.m_meshes.push_back(mesh);
        m_renderer.m_meshes_ids[mesh.m_name] = u32(m_renderer.m_meshes.size()) - 1;
    }

    for (auto& level_cfg : game_cfg["levels"]) {
        Level new_level;
        new_level.m_name = level_cfg["name"].str_value;
        m_levels.push_back(new_level);
        Level* level = &m_levels.back();
        init_scene(&level->m_scene);

        for (auto& node_cfg : level_cfg["nodes"]) {
            Node node;
            node.m_name = node_cfg["name"].str_value;
            node.m_position = node_cfg["position"].vec3_value;
            node.m_rotation = node_cfg["rotation"].vec4_value;
            node.m_scale = node_cfg["scale"].vec3_value;
            add_node(&level->m_scene, node, node_cfg["parent"].str_value);
        }
        level->m_scene.m_nodes[0].update_matrix();

        for (auto& renderable_cfg : level_cfg["renderables"]) {
            Renderable renderable;
            renderable.m_name = renderable_cfg["name"].str_value;
            for (auto tag : renderable_cfg["tags"]) {
                renderable.m_tags.push_back(tag.str_value);
            }
            renderable.m_mesh = m_renderer.m_meshes_ids[renderable_cfg["mesh"].str_value];
            renderable.m_node = level->m_scene.m_nodes_ids[renderable_cfg["node"].str_value];

            m_renderer.m_renderables.push_back(renderable);
            u32 id = u32(m_renderer.m_renderables.size()) - 1;
            for (auto tag : renderable_cfg["tags"]) {
                m_renderer.m_tagged_renderables[tag.str_value].push_back(id);
            }
        }

        for (auto& camera_cfg : level_cfg["cameras"]) {
            Camera camera;
            camera.m_name = camera_cfg["name"].str_value;
            camera.m_node = level->m_scene.m_nodes_ids[camera_cfg["node"].str_value];
            camera.m_scene = &level->m_scene;
            if (camera_cfg["projection"]) {
                camera.m_projection = camera_cfg["projection"].str_value == "PERSPECTIVE"
                                          ? Projection::PERSPECTIVE
                                          : Projection::ORTHOGRAPHIC;
            }
            if (camera_cfg["fov"]) {
                camera.m_fov = rad(camera_cfg["fov"].float_value);
            }
            camera.set_aspect_ratio(f32(m_window.m_size.width) / f32(m_window.m_size.height));
            m_renderer.add_camera(camera);
        }
    }

    for (auto& pipeline_cfg : game_cfg["pipelines"]) {
        Pipeline pipeline;
        pipeline.m_name = pipeline_cfg["name"].str_value;
        Cfg passes_cfg = pipeline_cfg["passes"];
        for (auto& pass_cfg : passes_cfg) {
            Pass pass;

            pass.m_name = pass_cfg["name"].str_value;

            vec<CfgNode> clears = pass_cfg["clear"].array_value;
            u32 clear_flag = 0;
            for (auto& clear : clears) {
                clear_flag |= ClearStr[clear.str_value];
            }
            pass.m_clear_flag = clear_flag;

            pass.m_type = PassTypeStr[pass_cfg["type"].str_value];

            if (pass_cfg["clear_color"]) {
                pass.m_clear_color = pass_cfg["clear_color"].vec4_value;
            }
            if (pass_cfg["clear_depth"]) {
                pass.m_clear_depth = pass_cfg["clear_depth"].float_value;
            }

            pass.m_shader = m_renderer.m_shaders_ids[pass_cfg["shader"].str_value];

            for (auto tag : pass_cfg["tags"]) {
                pass.m_tags.push_back(tag.str_value);
            }

            pass.m_camera = m_renderer.m_cameras_ids[pass_cfg["camera"].str_value];
            if (pass_cfg["enabled"]) {
                pass.m_enabled = pass_cfg["enabled"].bool_value;
            }
            if (pass_cfg["use_default_framebuffer"]) {
                pass.m_use_default_framebuffer = pass_cfg["use_default_framebuffer"].bool_value;
            }
            if (pass_cfg["enable_depth_test"]) {
                pass.m_enable_depth_test = pass_cfg["enable_depth_test"].bool_value;
            }
            if (pass_cfg["enable_face_culling"]) {
                pass.m_enable_face_culling = pass_cfg["enable_face_culling"].bool_value;
            }
            if (pass_cfg["culling_mode"]) {
                pass.m_culling_mode = CullingModeStr[pass_cfg["culling_mode"].str_value];
            }

            pipeline.m_passes.push_back(pass);
        }
        m_renderer.m_pipelines.push_back(pipeline);
        m_renderer.m_pipelines_ids[pipeline.m_name] = u32(m_renderer.m_pipelines.size()) - 1;
    }

    u32 i = 0;
    for (auto& level_cfg : game_cfg["levels"]) {
        m_levels[i].m_pipeline = m_renderer.m_pipelines_ids[level_cfg["pipeline"].str_value];
        i++;
    }

    m_renderer.m_current_pipeline = m_levels[m_current_level_id].m_pipeline;
    m_renderer.m_current_scene = &m_levels[m_current_level_id].m_scene;
    m_physics.m_current_scene = &m_levels[m_current_level_id].m_scene;

    return load_level(u32(game_cfg["start_level"].float_value));
}

Error Game::load_level(u32 level) {
    m_current_level_id = level;
    m_current_level = &m_levels[m_current_level_id];
    return Error();
}

}  // namespace blaz