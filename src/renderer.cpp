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

    for (auto& mesh : m_meshes) {
        upload_mesh(&mesh);
    }

    m_uniform_buffers.push_back(UniformBuffer{
        .m_name = "u_mat", .m_size = 192, .m_binding_point = 0,
        .m_uniforms =
            {
                {
                    "u_model_mat",
                    Uniform{
                        .m_name = "u_model_mat",
                        .m_offset = 0,
                        .m_size = 64,
                    },
                },
                {
                    "u_view_mat",
                    Uniform{
                        .m_name = "u_view_mat",
                        .m_offset = 64,
                        .m_size = 64,
                    },
                },
                {
                    "u_projection_mat",
                    Uniform{
                        .m_name = "u_projection_mat",
                        .m_offset = 128,
                        .m_size = 64,
                    },
                },
            },
        .m_should_reload = true,
    });
    m_uniform_buffers_ids["u_mat"] = 0;
    init_uniform_buffer(&m_uniform_buffers[0]);

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

    return compile_shader(shader);
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
            bind_default_framebuffer();
            set_viewport(0, 0, m_game->m_window.m_size.width, m_game->m_window.m_size.height);
        } else {
            Framebuffer* framebuffer = &pipeline.m_framebuffers[pass.m_framebuffer];
            bind_framebuffer(framebuffer);
            set_viewport(0, 0, m_game->m_window.m_size.width, m_game->m_window.m_size.height);
        }

        if (pass.m_enable_depth_test) {
            set_depth_test(true);
        } else {
            set_depth_test(false);
        }

        if (pass.m_enable_face_culling) {
            set_face_culling(true);
            set_face_culling_mode(pass.m_culling_mode);
        } else {
            set_face_culling(false);
        }

        clear(pass.m_clear_flag, pass.m_clear_color, pass.m_clear_depth);

        Shader* pass_shader = &m_shaders[pass.m_shader];

        if (pass_shader->m_should_reload) {
            Error err = reload_shader(pass_shader);
            if (err) {
                logger.error(err);
            }
        }

        if (pass_shader->m_is_error) {
            // pass_shader = m_error_shader;
        }

        bind_shader(pass_shader);

        Camera& camera = m_cameras[pass.m_camera];

        UniformBuffer* mat_buffer = &m_uniform_buffers[m_uniform_buffers_ids["u_mat"]];
        camera.update_projection_matrix();
        camera.update_view_matrix();
        set_uniform_buffer_data(mat_buffer, "u_projection_mat", camera.m_projection_matrix);
        set_uniform_buffer_data(mat_buffer, "u_view_mat", camera.m_view_matrix);

        for (const str tag : pass.m_tags) {
            for (const u32 id : m_tagged_renderables[tag]) {
                Renderable* renderable = &m_renderables[id];
                set_uniform_buffer_data(
                    mat_buffer, "u_model_mat",
                    m_current_scene->m_nodes[renderable->m_node].m_global_matrix);
                draw_vertex_array(&m_meshes[renderable->m_mesh]);
            }
        }

#ifdef DEBUG_RENDERER
        debug_marker_end();
#endif
    }

    present();
}

void Renderer::add_mesh(Mesh mesh) {
    m_meshes.push_back(mesh);
    m_meshes_ids[mesh.m_name] = (u32)m_meshes.size() - 1;
}

void Renderer::add_renderable(Renderable renderable) {
    m_renderables.push_back(renderable);
    u32 id = (u32)m_renderables.size() - 1;
    for (auto tag : renderable.m_tags) {
        m_tagged_renderables[tag].push_back(id);
    }
}

}  // namespace blaz