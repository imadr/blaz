#include "renderer.h"

#include "filesystem.h"
#include "game.h"
#include "logger.h"
#include "types.h"

namespace blaz {

Error Renderer::init(Window* window) {
    m_window = window;

    Error err = init_api();
    if (err) {
        return err;
    }

    set_swap_interval(1);

    auto callback = [this](Window* window) {
        for (auto& camera : m_cameras) {
            camera.set_aspect_ratio(f32(window->m_size.width) / f32(window->m_size.height));
        }
    };
    m_window->m_resize_callbacks.push_back(callback);
    callback(m_window);

    m_error_shader.m_name = "internal_error_shader";
    m_error_shader.m_vertex_shader_path = "internal_data/error_shader.vert.glsl";
    m_error_shader.m_fragment_shader_path = "internal_data/error_shader.frag.glsl";
    create_shader(m_error_shader);
    create_uniform_buffer(UniformBuffer{
        .m_name = "u_mat",
        .m_binding_point = 0,
        .m_uniforms =
            {
                Uniform{
                    .m_name = "u_model_mat",
                    .m_type = UNIFORM_MAT4,
                },

                Uniform{
                    .m_name = "u_view_mat",
                    .m_type = UNIFORM_MAT4,
                },

                Uniform{
                    .m_name = "u_projection_mat",
                    .m_type = UNIFORM_MAT4,
                },
            },
        .m_should_reload = true,
    });
    create_uniform_buffer(UniformBuffer{
        .m_name = "u_view",
        .m_binding_point = 1,
        .m_uniforms =
            {
                Uniform{
                    .m_name = "u_camera_position",
                    .m_type = UNIFORM_VEC3,
                },
            },
        .m_should_reload = true,
    });

    return Error();
}

void Renderer::update() {
    for (Pass& pass : m_passes) {
        if (!pass.m_enabled) continue;

#ifdef DEBUG_RENDERER
        debug_marker_start(pass.m_name);
#endif

        set_viewport(0, 0, m_window->m_size.width, m_window->m_size.height);
        if (pass.m_use_default_framebuffer) {
            set_default_framebuffer();
        } else {
            set_current_framebuffer(pass.m_framebuffer);
        }

        if (pass.m_enable_depth_test) {
            set_depth_test(true);
        } else {
            set_depth_test(false);
        }

        set_face_culling(pass.m_enable_face_culling, pass.m_culling_mode, pass.m_culling_order);

        clear(pass.m_clear_flag, pass.m_clear_color, pass.m_clear_depth);

        Shader* pass_shader = &m_shaders[pass.m_shader];
        if (pass_shader->m_should_reload) {
            Error err = reload_shader(pass.m_shader);
            if (err) {
                logger.error(err);
            }
        }

        if (pass_shader->m_is_error) {
            pass_shader = &m_error_shader;
        }

        set_current_shader(pass.m_shader);

        for (auto& texture : m_textures) {
            if (texture.m_should_reload) {
                reload_texture(texture.m_name);
            }
        }
        set_textures(&pass, pass_shader);

        if (pass.m_camera != "") {
            Camera* camera = &m_cameras[pass.m_camera];
            camera->update_projection_matrix();
            camera->update_view_matrix();
            set_uniform_buffer_data("u_mat", "u_projection_mat", camera->m_projection_matrix);
            set_uniform_buffer_data("u_mat", "u_view_mat", camera->m_view_matrix);
            set_uniform_buffer_data("u_view", "u_camera_position",
                                    camera->m_scene->m_nodes[camera->m_node].m_position);
        }

        if (pass.m_bufferless_draw) {
            set_bufferless_mesh();
            draw(MeshPrimitive::TRIANGLES, pass.m_bufferless_draw_count);
        } else {
            for (const str tag : pass.m_tags) {
                for (const u32 id : m_tagged_renderables[tag]) {
                    Renderable* renderable = &m_renderables[id];
                    set_uniform_buffer_data(
                        "u_mat", "u_model_mat",
                        m_current_scene->m_nodes[renderable->m_node].m_global_matrix);
                    Mesh* mesh = &m_meshes[renderable->m_mesh];

                    if (mesh->m_should_reload) {
                        Error err = reload_mesh(renderable->m_mesh);
                    }

                    set_current_mesh(renderable->m_mesh);
                    draw_indexed(mesh->m_primitive, mesh->m_indices.size());
                }
            }
        }

#ifdef DEBUG_RENDERER
        debug_marker_end();
#endif
    }

    present();
}

Error Renderer::create_uniform_buffer(UniformBuffer uniform_buffer) {
    u32 aligned_offset = 0;
    u32 total_size = 0;
    for (u32 i = 0; i < uniform_buffer.m_uniforms.size(); i++) {
        Uniform& uniform = uniform_buffer.m_uniforms[i];
        uniform_buffer.m_uniforms_ids[uniform.m_name] = i;
        uniform.m_size = UniformTypeSize[uniform.m_type];
        total_size += uniform.m_size;
        uniform.m_offset = aligned_offset;
        aligned_offset += UniformTypeAlignment[uniform.m_type];
    }
    uniform_buffer.m_size = total_size;

    m_uniform_buffers.add(uniform_buffer);
    return create_uniform_buffer_api(uniform_buffer.m_name);
}

void Renderer::create_renderable(Renderable renderable) {
    m_renderables.push_back(renderable);
    u32 id = u32(m_renderables.size()) - 1;
    for (auto tag : renderable.m_tags) {
        m_tagged_renderables[tag].push_back(id);
    }
}

Error Renderer::create_shader(Shader shader) {
    m_shaders.add(shader);
    return create_shader_api(shader.m_name);
}

Error Renderer::reload_shader(str shader_id) {
    Shader* shader = &m_shaders[shader_id];

    if (shader->m_type == ShaderType::VERTEX_FRAGMENT) {
        pair<Error, str> err = read_whole_file(shader->m_vertex_shader_path);
        if (err.first) {
            shader->m_is_error = true;
            return err.first;
        }
        shader->m_vertex_shader_source = err.second;

        err = read_whole_file(shader->m_fragment_shader_path);
        if (err.first) {
            shader->m_is_error = true;
            return err.first;
        }
        shader->m_fragment_shader_source = err.second;
    } else {
        pair<Error, str> err = read_whole_file(shader->m_compute_shader_path);
        if (err.first) {
            shader->m_is_error = true;
            return err.first;
        }
        shader->m_compute_shader_source = err.second;
    }

    shader->m_should_reload = false;

    Error err2 = reload_shader_api(shader_id);
    if (err2) {
        shader->m_is_error = true;
    }

    return err2;
}

Error Renderer::create_texture(Texture texture) {
    m_textures.add(texture);
    return create_texture_api(texture.m_name);
}

Error Renderer::reload_texture(str texture_id) {
    load_texture_data_from_file(&m_textures[texture_id]);
    return reload_texture_api(texture_id);
}

Error Renderer::create_mesh(Mesh mesh) {
    m_meshes.add(mesh);
    return create_mesh_api(mesh.m_name);
}

Error Renderer::reload_mesh(str mesh_id) {
    return reload_mesh_api(mesh_id);
}

Error Renderer::create_camera(Camera camera) {
    m_cameras.add(camera);
    return Error();
}

Error Renderer::create_framebuffer(Framebuffer framebuffer) {
    m_framebuffers.add(framebuffer);
    return create_framebuffer_api(framebuffer.m_name);
}

}  // namespace blaz