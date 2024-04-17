#include "renderer.h"

#include "filesystem.h"
#include "game.h"
#include "logger.h"
#include "types.h"

namespace blaz {

Error Renderer::init(Game* game) {
    m_game = game;

    Error err = init_api();
    if (err) {
        return err;
    }

    set_swap_interval(1);

    game->m_window.m_resize_callback = [this](Window* window) {
        for (auto& camera : m_cameras) {
            camera.set_aspect_ratio(f32(window->m_size.width) / f32(window->m_size.height));
        }
    };
    game->m_window.m_resize_callback(&game->m_window);

    {
        m_error_shader.m_name = "internal_error_shader";
        m_error_shader.m_vertex_shader_path = "internal_data/error_shader.vert.glsl";
        m_error_shader.m_fragment_shader_path = "internal_data/error_shader.frag.glsl";
        m_shaders.push_back(m_error_shader);
        m_shaders_ids[m_error_shader.m_name] = u32(m_shaders.size()) - 1;
        Error err = reload_shader(&m_error_shader);
        if (err) {
            return err;
        }
    }

    for (auto& mesh : m_meshes) {
        upload_mesh(&mesh);
    }

    for (auto& texture : m_textures) {
        load_texture(&texture);
        upload_texture(&texture);
    }

    add_uniform_buffer(UniformBuffer{
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
    add_uniform_buffer(UniformBuffer{
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

Error Renderer::reload_shader(Shader* shader) {
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

    shader->m_should_reload = false;

    Error err2 = compile_shader(shader);
    if (err2) {
        shader->m_is_error = true;
    }

    return err2;
}

void Renderer::draw() {
    if (m_pipelines.size() <= m_current_pipeline) return;

    Pipeline& pipeline = m_pipelines[m_current_pipeline];

    for (Pass& pass : m_pipelines[m_current_pipeline].m_passes) {
        if (!pass.m_enabled) continue;

#ifdef DEBUG_RENDERER
        debug_marker_start(pass.m_name);
#endif

        if (pass.m_use_default_framebuffer) {
            set_default_framebuffer();
            set_viewport(0, 0, m_game->m_window.m_size.width, m_game->m_window.m_size.height);
        } else {
            Framebuffer* framebuffer = &pipeline.m_framebuffers[pass.m_framebuffer];
            set_framebuffer(framebuffer);
            set_viewport(0, 0, m_game->m_window.m_size.width, m_game->m_window.m_size.height);
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
            Error err = reload_shader(pass_shader);
            if (err) {
                logger.error(err);
            }
        }

        if (pass_shader->m_is_error) {
            pass_shader = &m_error_shader;
        }

        set_shader(pass_shader);

        // set_textures(&pass, pass_shader);

        Camera& camera = m_cameras[pass.m_camera];

        UniformBuffer* mat_buffer = &m_uniform_buffers[m_uniform_buffers_ids["u_mat"]];
        camera.update_projection_matrix();
        camera.update_view_matrix();
        set_uniform_buffer_data(mat_buffer, "u_projection_mat", camera.m_projection_matrix);
        set_uniform_buffer_data(mat_buffer, "u_view_mat", camera.m_view_matrix);

        UniformBuffer* view_buffer = &m_uniform_buffers[m_uniform_buffers_ids["u_view"]];
        set_uniform_buffer_data(view_buffer, "u_camera_position",
                                camera.m_scene->m_nodes[camera.m_node].m_position);

        for (const str tag : pass.m_tags) {
            for (const u32 id : m_tagged_renderables[tag]) {
                Renderable* renderable = &m_renderables[id];
                set_uniform_buffer_data(
                    mat_buffer, "u_model_mat",
                    m_current_scene->m_nodes[renderable->m_node].m_global_matrix);
                draw_mesh(&m_meshes[renderable->m_mesh]);
            }
        }

#ifdef DEBUG_RENDERER
        debug_marker_end();
#endif
    }

    present();
}

void Renderer::add_uniform_buffer(UniformBuffer buffer) {
    u32 aligned_offset = 0;
    u32 total_size = 0;
    for (u32 i = 0; i < buffer.m_uniforms.size(); i++) {
        Uniform& uniform = buffer.m_uniforms[i];
        buffer.m_uniforms_ids[uniform.m_name] = i;
        uniform.m_size = UniformTypeSize[uniform.m_type];
        total_size += uniform.m_size;
        uniform.m_offset = aligned_offset;
        aligned_offset += UniformTypeAlignment[uniform.m_type];
    }
    buffer.m_size = total_size;

    m_uniform_buffers.push_back(buffer);
    u32 id = u32(m_uniform_buffers.size()) - 1;
    m_uniform_buffers_ids[buffer.m_name] = id;
    init_uniform_buffer(&m_uniform_buffers[id]);
}

void Renderer::add_mesh(Mesh mesh) {
    m_meshes.push_back(mesh);
    m_meshes_ids[mesh.m_name] = u32(m_meshes.size()) - 1;
}

void Renderer::add_texture(Texture texture) {
    m_textures.push_back(texture);
    m_textures_ids[texture.m_name] = u32(m_textures.size()) - 1;
}

void Renderer::add_camera(Camera camera) {
    m_cameras.push_back(camera);
    m_cameras_ids[camera.m_name] = u32(m_cameras.size()) - 1;
}

void Renderer::add_renderable(Renderable renderable) {
    m_renderables.push_back(renderable);
    u32 id = u32(m_renderables.size()) - 1;
    for (auto tag : renderable.m_tags) {
        m_tagged_renderables[tag].push_back(id);
    }
}

}  // namespace blaz