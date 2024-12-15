#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"

using namespace blaz;

int main() {
    Window window;
    Error err = window.init("06-shadow");
    if (err) {
        logger.error(err);
        return 1;
    }

    Renderer renderer;
    err = renderer.init(&window);
    if (err) {
        logger.error(err);
        return 1;
    }

    Scene scene;
    init_scene(&scene);

    Game game;
    game.m_window = &window;
    game.m_renderer = &renderer;
    game.m_scene = &scene;
    err = game.load_game("data/game.cfg");
    if (err) {
        logger.error(err);
    }

    make_cube(&renderer.m_meshes["cube_mesh"]);
    make_plane(&renderer.m_meshes["plane_mesh"]);

    renderer.create_uniform_buffer(UniformBuffer{
        .m_name = "u_light",
        .m_uniforms =
            {
                Uniform{
                    .m_name = "u_light_position",
                    .m_type = UNIFORM_VEC3,
                },
                Uniform{
                    .m_name = "u_light_view_mat",
                    .m_type = UNIFORM_MAT4,
                },
                Uniform{
                    .m_name = "u_light_projection_mat",
                    .m_type = UNIFORM_MAT4,
                },
            },
        .m_should_reload = true,
    });

    Camera* light_camera = &renderer.m_cameras["light_camera"];
    Vec3 light_pos = Vec3(3, 4, 6);
    light_camera->m_z_far = 100.0;
    scene.m_nodes["light_camera_node"].set_position(light_pos);
    scene.m_nodes["light_camera_node"].set_rotation(
        Quat::look_at(light_pos, Vec3(0, 0, 0), Vec3(0, 1, 0)));
    light_camera->update_projection_matrix();
    light_camera->update_view_matrix();
    renderer.set_uniform_buffer_data(
        "u_light", {{"u_light_position", scene.m_nodes[light_camera->m_node].m_position},
                    {"u_light_view_mat", light_camera->m_view_matrix},
                    {"u_light_projection_mat", light_camera->m_projection_matrix}});

    game.main_camera->m_orbit_pan_sensitivity = 0.005f;
    game.main_camera->m_orbit_spherical_angles = Vec2(f32(PI_HALF), f32(PI_HALF) / 2.0);

    window.m_mouse_click_callback = [&game](Vec2I mouse_position, ButtonState left_button,
                                            ButtonState right_button) {
        if (left_button == ButtonState::PRESSED) {
            game.main_camera->m_mouse_left_pressed = true;
        } else if (left_button == ButtonState::RELEASED) {
            game.main_camera->m_mouse_left_pressed = false;
        }

        if (right_button == ButtonState::PRESSED) {
            game.main_camera->m_mouse_right_pressed = true;
        } else if (right_button == ButtonState::RELEASED) {
            game.main_camera->m_mouse_right_pressed = false;
        }
    };

    window.m_mouse_move_raw_callback = [&game](Vec2I delta) {
        game.main_camera->orbit_mouse_move(delta);
    };

    window.m_mouse_wheel_callback = [&game](i16 delta) {
        game.main_camera->orbit_mouse_wheel(delta);
    };

    game.main_camera->update_orbit_camera();

    game.m_main_loop = [&game]() {
        if (game.m_window->event_loop()) {
            game.m_renderer->update();

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}