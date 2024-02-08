#include "renderer.h"

#include "shader.h"
#include "types.h"

namespace blaz {

void reload_shader(Shader& shader_to_reload) {
}

// void resize_callback(Window* window) {
//     //     if (Camera::main != NULL) {  // @note do something cleaner than this
//     //         Camera::main->set_aspect_ratio((f32)window->m_size.width /
//     //         (f32)window->m_size.height);
//     //     }
// }

void Renderer::draw_pass(u32 pass_id) {
    Pass pass = m_passes[pass_id];

    if (!pass.m_enabled) return;

#ifdef DEBUG_RENDERER
    debug_marker_start(pass.m_name);
#endif

    if (pass.m_use_default_framebuffer) {
        bind_default_framebuffer();
        set_viewport(0, 0, m_window->m_size.width, m_window->m_size.height);
    } else {
        bind_framebuffer(pass.m_framebuffer);
        set_viewport(0, 0, m_window->m_size.width, m_window->m_size.height);
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

    Shader pass_shader = m_shaders[pass.m_shader];

    if (pass_shader.m_should_reload) {
        reload_shader(pass_shader);
    }

    if (pass_shader.m_is_error) {
        pass_shader = m_error_shader;
    }

    bind_shader(pass_shader);

    // set_shader_uniform_Mat4(shader, "u_projection_mat", pass.m_camera->m_projection_matrix);
    // const Mat4 view_matrix = (m_scene->m_nodes[pass.m_camera->m_node]->m_global_matrix).invert();
    // set_shader_uniform_Mat4(shader, "u_view_mat", view_matrix);
    // set_shader_uniform_Vec2(shader, "u_screen_resolution",
    //                         Vec2(m_window->m_size.width, m_window->m_size.height));
    // set_shader_uniform_Vec3(shader, "u_view_position",
    //                         m_scene->m_nodes[pass.m_camera->m_node]->get_global_position());

    // bind_textures(pass, shader);

    // for (const str& tag : pass.m_tags) {
    //     for (const str& name : m_renderable_manager->get_renderables_with_tag(tag)) {
    //         Renderable renderable = m_renderable_manager->m_renderables[name];
    //         Mesh* mesh = &m_mesh_manager->m_meshes[renderable.m_mesh];

    //         // @note get material

    //         set_shader_uniform_Mat4(shader, "u_model_mat",
    //                                 m_scene->m_nodes[renderable.m_node]->m_global_matrix);

    //         draw_vertex_array(mesh);
    //     }
    // }

#ifdef DEBUG_RENDERER
    debug_marker_end();
#endif
}

void Renderer::draw() {
    if (!m_current_pipeline) return;
    for (u32 pass : m_current_pipeline->m_passes) {
        draw_pass(pass);
    }
}

}  // namespace blaz