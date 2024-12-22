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
    while (m_main_loop()) {
    }
#endif
}

Error Game::load_game(const str& path) {
    pair<Error, Cfg> cfg = read_cfg_file(path);
    if (cfg.first) {
        return cfg.first;
    }
    Cfg game_cfg = cfg.second;

    for (auto& shader_cfg : game_cfg["shaders"]) {
        Shader shader;
        shader.m_name = shader_cfg["name"].str_value;
        if (shader_cfg["vertex_shader_path"]) {
            shader.m_vertex_shader_path = shader_cfg["vertex_shader_path"].str_value;
            m_shader_file_dependencies[shader.m_vertex_shader_path].push_back(shader.m_name);
        }
        if (shader_cfg["fragment_shader_path"]) {
            shader.m_fragment_shader_path = shader_cfg["fragment_shader_path"].str_value;
            m_shader_file_dependencies[shader.m_fragment_shader_path].push_back(shader.m_name);
        }
        if (shader_cfg["compute_shader_path"]) {
            shader.m_compute_shader_path = shader_cfg["compute_shader_path"].str_value;
            m_shader_file_dependencies[shader.m_compute_shader_path].push_back(shader.m_name);
        }
        if (shader_cfg["type"].str_value == "VERTEX_FRAGMENT") {
            shader.m_type = ShaderType::VERTEX_FRAGMENT;
        } else if (shader_cfg["type"].str_value == "COMPUTE") {
            shader.m_type = ShaderType::COMPUTE;
        }

        m_renderer->create_shader(shader);
    }

    for (auto& mesh_cfg : game_cfg["meshes"]) {
        Mesh mesh;
        mesh.m_name = mesh_cfg["name"].str_value;
        if (mesh_cfg["path"]) {
            mesh.m_path = mesh_cfg["path"].str_value;
        }
        mesh.m_vertices = mesh_cfg["vertices"].buffer_f32_value;
        mesh.m_indices = mesh_cfg["indices"].buffer_u32_value;
        if (mesh_cfg["primitive"]) {
            mesh.m_primitive = MeshPrimitiveStr.at(mesh_cfg["primitive"].str_value);
        }
        for (auto& attrib : mesh_cfg["attribs"]) {
            mesh.m_attribs.push_back(
                std::make_pair(attrib[0].str_value, u32(attrib[1].float_value)));
        }
        m_renderer->create_mesh(mesh);
    }

    for (auto& texture_cfg : game_cfg["textures"]) {
        Texture texture;
        texture.m_name = texture_cfg["name"].str_value;
        if (texture_cfg["path"]) {
            texture.m_path = texture_cfg["path"].str_value;
        }
        texture.m_texture_params.m_format = TextureFormatStr.at(texture_cfg["format"].str_value);

        if (texture_cfg["target"]) {
            texture.m_texture_params.m_target =
                TextureTargetStr.at(texture_cfg["target"].str_value);
        } else {
            texture.m_texture_params.m_target = TextureTarget::TEXTURE_2D;
        }

        if (texture_cfg["wrap_mode_s"]) {
            texture.m_texture_params.m_wrap_mode_s =
                TextureWrapModeStr.at(texture_cfg["wrap_mode_s"].str_value);
        }
        if (texture_cfg["wrap_mode_t"]) {
            texture.m_texture_params.m_wrap_mode_t =
                TextureWrapModeStr.at(texture_cfg["wrap_mode_t"].str_value);
        }
        if (texture_cfg["filter_mode_min"]) {
            texture.m_texture_params.m_filter_mode_min =
                TextureFilteringModeStr.at(texture_cfg["filter_mode_min"].str_value);
        }
        if (texture_cfg["filter_mode_mag"]) {
            texture.m_texture_params.m_filter_mode_mag =
                TextureFilteringModeStr.at(texture_cfg["filter_mode_mag"].str_value);
        }

        if (texture_cfg["width"]) {
            if (texture_cfg["width"].str_value == "viewport_width") {
                texture.m_resize_to_viewport = true;
            } else {
                texture.m_width = u32(texture_cfg["width"].float_value);
            }
        }

        if (texture_cfg["height"]) {
            if (texture_cfg["height"].str_value == "viewport_height") {
                texture.m_resize_to_viewport = true;
            } else {
                texture.m_height = u32(texture_cfg["height"].float_value);
            }
        }

        m_renderer->create_texture(texture);
    }

    for (auto& framebuffer_cfg : game_cfg["framebuffers"]) {
        Framebuffer framebuffer;
        framebuffer.m_name = framebuffer_cfg["name"].str_value;

        if (framebuffer_cfg["color_attachment_texture"]) {
            framebuffer.m_color_attachment_texture =
                framebuffer_cfg["color_attachment_texture"].str_value;
        }

        if (framebuffer_cfg["depth_attachment_texture"]) {
            framebuffer.m_depth_attachment_texture =
                framebuffer_cfg["depth_attachment_texture"].str_value;
        }

        if (framebuffer_cfg["stencil_attachment_texture"]) {
            framebuffer.m_stencil_attachment_texture =
                framebuffer_cfg["stencil_attachment_texture"].str_value;
        }

        m_renderer->create_framebuffer(framebuffer);
    }

    for (auto& material_cfg : game_cfg["materials"]) {
        Material material;
        material.m_name = material_cfg["name"].str_value;
        material.m_shader = material_cfg["shader"].str_value;

        for (auto& uniform_group : material_cfg["uniforms"].map_value) {
            for (auto& uniform : uniform_group.second.map_value) {
                switch (uniform.second.type) {
                    case CfgNodeType::FLOAT: {
                        material.m_uniforms[uniform_group.first][uniform.first] =
                            UniformValue(uniform.second.float_value);
                    } break;
                    case CfgNodeType::VEC3: {
                        material.m_uniforms[uniform_group.first][uniform.first] =
                            UniformValue(uniform.second.vec3_value);
                    } break;
                    case CfgNodeType::VEC4: {
                        material.m_uniforms[uniform_group.first][uniform.first] =
                            UniformValue(uniform.second.vec4_value);
                    } break;
                    case CfgNodeType::BOOL: {
                        material.m_uniforms[uniform_group.first][uniform.first] =
                            UniformValue(uniform.second.bool_value);
                    } break;
                }
            }
        }

        m_renderer->create_material(material);
    }

    for (auto& node_cfg : game_cfg["nodes"]) {
        Node node;
        node.m_name = node_cfg["name"].str_value;
        node.m_position = node_cfg["position"].vec3_value;
        node.m_rotation = node_cfg["rotation"].vec4_value;
        node.m_scale = node_cfg["scale"].vec3_value;
        add_node(m_scene, node, node_cfg["parent"].str_value);
    }
    m_scene->m_nodes[0].update_matrix();

    for (auto& renderable_cfg : game_cfg["renderables"]) {
        Renderable renderable;
        renderable.m_name = renderable_cfg["name"].str_value;
        for (auto& tag : renderable_cfg["tags"]) {
            renderable.m_tags.push_back(tag.str_value);
        }
        renderable.m_mesh = renderable_cfg["mesh"].str_value;
        renderable.m_node = renderable_cfg["node"].str_value;
        renderable.m_material = renderable_cfg["material"].str_value;

        m_renderer->m_renderables.push_back(renderable);
        u32 id = u32(m_renderer->m_renderables.size()) - 1;
        for (auto& tag : renderable_cfg["tags"]) {
            m_renderer->m_tagged_renderables[tag.str_value].push_back(id);
        }
    }

    for (auto& camera_cfg : game_cfg["cameras"]) {
        Camera camera;
        camera.m_name = camera_cfg["name"].str_value;
        camera.m_node = camera_cfg["node"].str_value;
        camera.m_scene = m_scene;
        if (camera_cfg["projection"]) {
            camera.m_projection = camera_cfg["projection"].str_value == "PERSPECTIVE"
                                      ? Projection::PERSPECTIVE
                                      : Projection::ORTHOGRAPHIC;
        }
        if (camera_cfg["fov"]) {
            camera.m_fov = rad(camera_cfg["fov"].float_value);
        }
        m_renderer->create_camera(camera);
    }

    for (auto& pass_cfg : game_cfg["passes"]) {
        Pass pass;

        pass.m_name = pass_cfg["name"].str_value;

        vec<CfgNode> clears = pass_cfg["clear"].array_value;
        u32 clear_flag = 0;
        for (auto& clear : clears) {
            clear_flag |= ClearStr.at(clear.str_value);
        }
        pass.m_clear_flag = clear_flag;

        pass.m_type = PassTypeStr.at(pass_cfg["type"].str_value);

        if (pass_cfg["clear_color"]) {
            pass.m_clear_color = pass_cfg["clear_color"].vec4_value;
        }
        if (pass_cfg["clear_depth"]) {
            pass.m_clear_depth = pass_cfg["clear_depth"].float_value;
        }

        pass.m_shader = pass_cfg["shader"].str_value;

        for (auto& tag : pass_cfg["tags"]) {
            pass.m_tags.push_back(tag.str_value);
        }

        if (pass_cfg["camera"]) {
            pass.m_camera = pass_cfg["camera"].str_value;
        }
        if (pass_cfg["enabled"]) {
            pass.m_enabled = pass_cfg["enabled"].bool_value;
        }
        if (pass_cfg["use_default_framebuffer"]) {
            pass.m_use_default_framebuffer = pass_cfg["use_default_framebuffer"].bool_value;
        } else {
            if (pass_cfg["framebuffer"]) {
                pass.m_use_default_framebuffer = false;
                pass.m_framebuffer = pass_cfg["framebuffer"].str_value;
            }
        }
        if (pass_cfg["enable_depth_test"]) {
            pass.m_enable_depth_test = pass_cfg["enable_depth_test"].bool_value;
        }
        if (pass_cfg["enable_face_culling"]) {
            pass.m_enable_face_culling = pass_cfg["enable_face_culling"].bool_value;
        }
        if (pass_cfg["culling_mode"]) {
            pass.m_culling_mode = CullingModeStr.at(pass_cfg["culling_mode"].str_value);
        }

        if (pass_cfg["bufferless_draw"]) {
            pass.m_bufferless_draw = true;
            pass.m_bufferless_draw_count = u32(pass_cfg["bufferless_draw"].float_value);
        }

        if (pass_cfg["image_uniforms_bindings"]) {
            vec<CfgNode> image_uniforms_bindings = pass_cfg["image_uniforms_bindings"].array_value;
            for (auto& image_uniform_binding : image_uniforms_bindings) {
                pass.m_image_uniforms_bindings[image_uniform_binding[0].str_value] =
                    std::make_pair(image_uniform_binding[1].str_value,
                                   AccessTypeStr.at(image_uniform_binding[2].str_value));
            }
        }

        if (pass_cfg["sampler_uniforms_bindings"]) {
            vec<CfgNode> sampler_uniforms_bindings =
                pass_cfg["sampler_uniforms_bindings"].array_value;
            for (auto& sampler_uniform_binding : sampler_uniforms_bindings) {
                pass.m_sampler_uniforms_bindings[sampler_uniform_binding[0].str_value] =
                    sampler_uniform_binding[1].str_value;
            }
        }

        if (pass_cfg["compute_work_groups"]) {
            for (u32 i = 0; i < 3; i++) {
                if (pass_cfg["compute_work_groups"][i].str_value != "") {
                    pass.m_compute_work_groups[i] = pass_cfg["compute_work_groups"][i].str_value;
                } else {
                    pass.m_compute_work_groups[i] =
                        u32(pass_cfg["compute_work_groups"][i].float_value);
                }
            }
        }

        if (pass_cfg["copy_src_texture"]) {
            pass.m_copy_src_texture = pass_cfg["copy_src_texture"].str_value;
            pass.m_copy_dst_texture = pass_cfg["copy_dst_texture"].str_value;
        }

        m_renderer->m_passes.push_back(pass);
    }

    m_renderer->m_current_scene = m_scene;
    if (m_physics != NULL) m_physics->m_current_scene = m_scene;

    if (game_cfg["main_camera"]) {
        main_camera = &m_renderer->m_cameras[game_cfg["main_camera"].str_value];
    }

    m_filewatcher.init("data", [this](const str& filename) {
        str filepath = "data/" + filename;
        if (m_shader_file_dependencies.contains(filepath)) {
            for (auto& shader_id : m_shader_file_dependencies.at(filepath)) {
                m_renderer->m_shaders[shader_id].m_should_reload = true;
            }
        }
    });

    for (auto& texture : m_renderer->m_textures) {
        if (texture.m_should_reload) {
            m_renderer->reload_texture(texture.m_name);
        }
    }

    return Error();
}

}  // namespace blaz