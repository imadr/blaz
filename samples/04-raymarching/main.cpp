#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"
#include "my_time.h"

using namespace blaz;

int main() {
    Window window;
    Error err = window.init("04-raymarching");
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

    renderer.create_uniform_buffer(UniformBuffer{
        .m_name = "u_info",
        .m_uniforms =
            {
                Uniform{
                    .m_name = "u_resolution",
                    .m_type = UNIFORM_VEC2,
                },
                Uniform{
                    .m_name = "u_camera_position",
                    .m_type = UNIFORM_VEC3,
                },
                Uniform{
                    .m_name = "u_camera_target",
                    .m_type = UNIFORM_VEC3,
                },
            },
        .m_should_reload = true,
    });

    auto resize_callback = [&game](Window* window) {
        game.m_renderer->set_uniform_buffer_data(
            "u_info", "u_resolution",
            Vec2(f32(game.m_window->m_size.width), f32(game.m_window->m_size.height)));
    };
    window.m_resize_callbacks.push_back(resize_callback);
    resize_callback(&window);

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

    auto update_camera_uniforms = [&game]() {
        game.m_renderer->set_uniform_buffer_data("u_info", "u_camera_position",
                                                 game.m_scene->m_nodes["main_camera"].m_position);
        game.m_renderer->set_uniform_buffer_data("u_info", "u_camera_target",
                                                 game.main_camera->m_orbit_target);
    };

    window.m_mouse_move_raw_callback = [&game, &update_camera_uniforms](Vec2I delta) {
        game.main_camera->orbit_mouse_move(delta);
        update_camera_uniforms();
    };

    window.m_mouse_wheel_callback = [&game, &update_camera_uniforms](i16 delta) {
        game.main_camera->orbit_mouse_wheel(delta);
        update_camera_uniforms();
    };

    game.main_camera->m_orbit_zoom = 15.0f;
    game.main_camera->m_orbit_spherical_angles = Vec2(f32(PI_HALF), f32(PI) / 10.0);

    game.main_camera->update_orbit_camera();

    update_camera_uniforms();

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