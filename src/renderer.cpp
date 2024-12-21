#include "renderer.h"

#include "filesystem.h"
#include "game.h"
#include "logger.h"
#include "my_time.h"
#include "types.h"

namespace blaz {

Error Renderer::init(Window* window) {
    m_window = window;

    Error err = init_api();
    if (err) {
        return err;
    }

    set_swap_interval(1);

    auto resize_callback = [this](Window* window) {
        for (auto& texture : m_textures) {
            if (texture.m_resize_to_viewport) {
                texture.m_width = m_window->m_size.width;
                texture.m_height = m_window->m_size.height;
                reload_texture(texture.m_name);
            }
        }
    };
    m_window->m_resize_callbacks.push_back(resize_callback);
    resize_callback(m_window);

    Shader internal_error_shader;
    internal_error_shader.m_name = "internal_error_shader";
    internal_error_shader.m_vertex_shader_path = "internal_data/error_shader.vert.glsl";
    internal_error_shader.m_fragment_shader_path = "internal_data/error_shader.frag.glsl";
    create_shader(internal_error_shader);
    reload_shader("internal_error_shader");
    create_uniform_buffer(UniformBuffer{
        .m_name = "u_mat",
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
        .m_uniforms =
            {
                Uniform{
                    .m_name = "u_camera_position",
                    .m_type = UNIFORM_VEC3,
                },
            },
        .m_should_reload = true,
    });
    create_uniform_buffer(UniformBuffer{
        .m_name = "u_time",
        .m_uniforms =
            {
                Uniform{
                    .m_name = "u_frame_number",
                    .m_type = UNIFORM_UINT,
                },
            },
        .m_should_reload = true,
    });

    for (Pass& pass : m_passes_do_once) {
        do_pass(pass);
    }

    return Error();
}

u32 Renderer::special_value(const str& name) {
    if (name == "viewport_width") {
        return m_window->m_size.width;
    } else if (name == "viewport_height") {
        return m_window->m_size.height;
    } else {
        return 0;
    }
}

void Renderer::do_pass(Pass& pass) {
    if (!pass.m_enabled) return;

    u64 pass_start_cpu_time = get_timestamp_microsecond();

#ifdef DEBUG_RENDERER
    debug_marker_start(pass.m_name);
#endif

    if (pass.m_type == PassType::COPY) {
        copy_texture(pass.m_copy_src_texture, pass.m_copy_dst_texture, Vec3I(0, 0, 0),
                     Vec2I(m_textures[pass.m_copy_src_texture].m_width,
                           m_textures[pass.m_copy_src_texture].m_height),
                     Vec3I(0, 0, 0));

    } else if (pass.m_type == PassType::COMPUTE) {
        Shader pass_shader = m_shaders[pass.m_shader];
        if (pass_shader.m_should_reload) {
            Error err = reload_shader(pass.m_shader);
            if (err) {
                logger.error(err);
            }
        }

        str shader_to_use = pass.m_shader;

        set_current_shader(shader_to_use);

        set_uniform_buffer_data("u_time", {{"u_frame_number", m_frame_number}});

        bind_uniforms(&pass, shader_to_use);

        u32 work_groups[3];
        for (u32 i = 0; i < 3; i++) {
            if (std::holds_alternative<u32>(pass.m_compute_work_groups[i])) {
                work_groups[i] = std::get<u32>(pass.m_compute_work_groups[i]);
            } else if (std::holds_alternative<str>(pass.m_compute_work_groups[i])) {
                work_groups[i] = special_value(std::get<str>(pass.m_compute_work_groups[i]));
            }
        }
        dispatch_compute(work_groups[0], work_groups[1], work_groups[2]);

    } else if (pass.m_type == PassType::RENDER) {
        f32 framebuffer_aspect_ratio = 1.0;
        if (pass.m_use_default_framebuffer) {
            set_default_framebuffer();
            framebuffer_aspect_ratio = f32(m_window->m_size.width) / f32(m_window->m_size.height);
            set_viewport(0, 0, m_window->m_size.width, m_window->m_size.height);
        } else {
            set_current_framebuffer(pass.m_framebuffer);

            Texture* texture = NULL;
            Framebuffer* framebuffer = &m_framebuffers[pass.m_framebuffer];
            if (framebuffer->m_color_attachment_texture != "") {
                texture = &m_textures[framebuffer->m_color_attachment_texture];
            } else if (framebuffer->m_depth_attachment_texture != "") {
                texture = &m_textures[framebuffer->m_depth_attachment_texture];
            }

            if (texture != NULL) {
                framebuffer_aspect_ratio = f32(texture->m_width) / f32(texture->m_height);
                set_viewport(0, 0, texture->m_width, texture->m_height);
            } else {
                logger.error("Framebuffer " + framebuffer->m_name + " doesn't have any attachment");
            }
        }

        if (pass.m_enable_depth_test) {
            set_depth_test(true);
        } else {
            set_depth_test(false);
        }

        set_face_culling(pass.m_enable_face_culling, pass.m_culling_mode, pass.m_culling_order);

        clear(pass.m_clear_flag, pass.m_clear_color, pass.m_clear_depth);

        Shader pass_shader = m_shaders[pass.m_shader];
        if (pass_shader.m_should_reload) {
            Error err = reload_shader(pass.m_shader);
            if (err) {
                logger.error(err);
            }
        }

        str shader_to_use = pass.m_shader;
        if (pass_shader.m_is_error) {
            shader_to_use = "internal_error_shader";
        }

        set_current_shader(shader_to_use);

        if (pass.m_camera != "") {
            Camera* camera = &m_cameras[pass.m_camera];
            camera->set_aspect_ratio(framebuffer_aspect_ratio);
            camera->update_projection_matrix();
            camera->update_view_matrix();
            set_uniform_buffer_data("u_mat", {{"u_projection_mat", camera->m_projection_matrix},
                                              {"u_view_mat", camera->m_view_matrix}});
            set_uniform_buffer_data(
                "u_view",
                {{"u_camera_position", camera->m_scene->m_nodes[camera->m_node].m_position}});
        }

        set_uniform_buffer_data("u_time", {{"u_frame_number", m_frame_number}});

        bind_uniforms(&pass, shader_to_use);

        if (pass.m_bufferless_draw) {
            set_bufferless_mesh();
            draw(MeshPrimitive::TRIANGLES, pass.m_bufferless_draw_count);
        } else {
            for (const str tag : pass.m_tags) {
                for (const u32 id : m_tagged_renderables[tag]) {
                    Renderable* renderable = &m_renderables[id];
                    set_uniform_buffer_data(
                        "u_mat", {{"u_model_mat",
                                   m_current_scene->m_nodes[renderable->m_node].m_global_matrix}});
                    Mesh* mesh = &m_meshes[renderable->m_mesh];

                    if (mesh->m_should_reload) {
                        Error err = reload_mesh(renderable->m_mesh);
                    }

                    set_current_mesh(renderable->m_mesh);
                    draw_indexed(mesh->m_primitive, mesh->m_indices.size());
                }
            }
        }
    }

#ifdef DEBUG_RENDERER
    debug_marker_end();
#endif

    u64 pass_cpu_time = get_timestamp_microsecond() - pass_start_cpu_time;
}

void Renderer::update() {
    u64 frame_start_cpu_time = get_timestamp_microsecond();
    for (Pass& pass : m_passes) {
        do_pass(pass);
    }

    u64 frame_cpu_time = get_timestamp_microsecond() - frame_start_cpu_time;
    m_frame_number++;

    present();
}

Error Renderer::create_uniform_buffer(UniformBuffer uniform_buffer) {
    u32 aligned_offset = 0;
    u32 total_size = 0;
    for (u32 i = 0; i < uniform_buffer.m_uniforms.size(); i++) {
        Uniform& uniform = uniform_buffer.m_uniforms[i];
        uniform_buffer.m_uniforms_ids[uniform.m_name] = i;
        uniform.m_size = UniformTypeSize[uniform.m_type];

        u32 alignment = UniformTypeAlignment[uniform.m_type];
        aligned_offset = (aligned_offset + alignment - 1) & ~(alignment - 1);
        uniform.m_offset = aligned_offset;
        aligned_offset += uniform.m_size;
        total_size = aligned_offset;
    }
    uniform_buffer.m_size = total_size;

    m_uniform_buffers.add(uniform_buffer);
    return create_uniform_buffer_api(uniform_buffer.m_name);
}

void Renderer::create_renderable(Renderable renderable) {
    m_renderables.push_back(renderable);
    u32 id = u32(m_renderables.size()) - 1;
    for (auto& tag : renderable.m_tags) {
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
    } else {
        shader->m_is_error = false;
    }

    return err2;
}

Error Renderer::create_texture(Texture texture) {
    if (texture.m_resize_to_viewport) {
        texture.m_width = m_window->m_size.width;
        texture.m_height = m_window->m_size.height;
    }
    m_textures.add(texture);
    return create_texture_api(texture.m_name);
}

Error Renderer::reload_texture(str texture_id) {
    Texture* texture = &m_textures[texture_id];
    if (texture->m_resize_to_viewport) {
        texture->m_width = m_window->m_size.width;
        texture->m_height = m_window->m_size.height;
    }

    if (texture->m_path != "") {
        Error err = load_texture_data_from_file(texture);
        if (err) {
            return err;
        }
        return reload_texture_api(texture_id);
    }
    texture->m_should_reload = false;
    return reload_texture_api(texture_id);
}

Error Renderer::create_mesh(Mesh mesh) {
    m_meshes.add(mesh);
    return create_mesh_api(mesh.m_name);
}

Error Renderer::reload_mesh(str mesh_id) {
    if (m_meshes[mesh_id].m_path != "") {
        Error err = load_mesh_from_file(&m_meshes[mesh_id]);
        if (err) {
            return err;
        }
        return reload_mesh_api(mesh_id);
    }
    m_meshes[mesh_id].m_should_reload = false;
    return reload_mesh_api(mesh_id);
}

Error Renderer::create_camera(Camera camera) {
    m_cameras.add(camera);
    return Error();
}

Error Renderer::create_framebuffer(Framebuffer framebuffer) {
    m_framebuffers.add(framebuffer);
    Error err = create_framebuffer_api(framebuffer.m_name);
    if (err) {
        return err;
    }

    err = attach_texture_to_framebuffer(framebuffer.m_name);
    if (err) {
        return err;
    }

    return Error();
}

}  // namespace blaz