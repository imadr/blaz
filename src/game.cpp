#include "game.h"

#include "camera.h"
#include "cfgreader.h"
#include "error.h"
#include "filesystem.h"
#include "logger.h"
#include "types.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

namespace blaz {

#ifdef EMSCRIPTEN
void emscripten_main_loop(void* arg) {
    Game* game = (Game*)arg;
    game->main_loop();
}
#endif

void Game::run() {
#ifdef EMSCRIPTEN
    void* arg = (void*)(this);
    emscripten_set_main_loop_arg(emscripten_main_loop, arg, 0, true);
#else
    while (main_loop()) {
    }
#endif
}

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

    for (auto& texture_cfg : game_cfg["textures"]) {
        Texture texture;
        texture.m_name = texture_cfg["name"].str_value;
        texture.m_path = texture_cfg["path"].str_value;
        texture.m_texture_params.m_format = TextureFormatStr[texture_cfg["format"].str_value];
        m_renderer.m_textures.push_back(texture);
        m_renderer.m_textures_ids[texture.m_name] = u32(m_renderer.m_textures.size()) - 1;
    }

    init_scene(&m_scene);

    for (auto& node_cfg : game_cfg["nodes"]) {
        Node node;
        node.m_name = node_cfg["name"].str_value;
        node.m_position = node_cfg["position"].vec3_value;
        node.m_rotation = node_cfg["rotation"].vec4_value;
        node.m_scale = node_cfg["scale"].vec3_value;
        add_node(&m_scene, node, node_cfg["parent"].str_value);
    }

    m_scene.m_nodes[0].update_matrix();

    for (auto& renderable_cfg : game_cfg["renderables"]) {
        Renderable renderable;
        renderable.m_name = renderable_cfg["name"].str_value;
        for (auto tag : renderable_cfg["tags"]) {
            renderable.m_tags.push_back(tag.str_value);
        }
        renderable.m_mesh = m_renderer.m_meshes_ids[renderable_cfg["mesh"].str_value];
        renderable.m_node = m_scene.m_nodes_ids[renderable_cfg["node"].str_value];

        m_renderer.m_renderables.push_back(renderable);
        u32 id = u32(m_renderer.m_renderables.size()) - 1;
        for (auto tag : renderable_cfg["tags"]) {
            m_renderer.m_tagged_renderables[tag.str_value].push_back(id);
        }
    }

    for (auto& camera_cfg : game_cfg["cameras"]) {
        Camera camera;
        camera.m_name = camera_cfg["name"].str_value;
        camera.m_node = m_scene.m_nodes_ids[camera_cfg["node"].str_value];
        camera.m_scene = &m_scene;
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

            if(pass_cfg["camera"]){
                pass.m_camera = m_renderer.m_cameras_ids[pass_cfg["camera"].str_value];
            }
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

            if (pass_cfg["bufferless_draw"]) {
                pass.m_bufferless_draw = true;
                pass.m_bufferless_draw_count = u32(pass_cfg["bufferless_draw"].float_value);
            }

            for (auto texture : pass_cfg["texture_uniforms"]) {
                pass.m_texture_uniforms.push_back(texture.str_value);
            }

            pipeline.m_passes.push_back(pass);
        }
        m_renderer.m_pipelines.push_back(pipeline);
        m_renderer.m_pipelines_ids[pipeline.m_name] = u32(m_renderer.m_pipelines.size()) - 1;
    }

    m_renderer.m_current_pipeline =
        m_renderer.m_pipelines_ids[game_cfg["current_pipeline"].str_value];
    m_renderer.m_current_scene = &m_scene;
    m_physics.m_current_scene = &m_scene;

    if(game_cfg["main_camera"]){
        main_camera = &m_renderer.m_cameras[m_renderer.m_cameras_ids[game_cfg["main_camera"].str_value]];
    }

    return Error();
}

}  // namespace blaz