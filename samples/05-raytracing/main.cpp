#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"
#include "my_time.h"

using namespace blaz;

int main() {
    Window window;
    Error err = window.init("05-raytracing");
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
                    .m_name = "u_camera_position",
                    .m_type = UNIFORM_VEC3,
                },
                Uniform{
                    .m_name = "u_camera_target",
                    .m_type = UNIFORM_VEC3,
                },
                Uniform{
                    .m_name = "u_reset_accumulation",
                    .m_type = UNIFORM_FLOAT,
                },
            },
        .m_should_reload = true,
    });

    game.m_renderer->set_uniform_buffer_data("u_info", {{"u_reset_accumulation", 0.0f}});

    u32 reset_accumulation_counter = 0;
    auto reset_accumulation = [&game, &reset_accumulation_counter]() {
        game.m_renderer->m_frame_number = 1;
        game.m_renderer->set_uniform_buffer_data("u_info", {{"u_reset_accumulation", 1.0f}});
        reset_accumulation_counter = 2;
    };

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
        game.m_renderer->set_uniform_buffer_data(
            "u_info",
            {{"u_camera_position", game.m_scene->m_nodes["main_camera_node"].m_position}});
        game.m_renderer->set_uniform_buffer_data(
            "u_info", {{"u_camera_target", game.main_camera->m_orbit_target}});
    };

    window.m_mouse_move_raw_callback = [&game, &update_camera_uniforms,
                                        &reset_accumulation](Vec2I delta) {
        if (game.main_camera->m_mouse_left_pressed || game.main_camera->m_mouse_right_pressed) {
            reset_accumulation();
        }

        game.main_camera->orbit_mouse_move(delta);
        update_camera_uniforms();
    };

    window.m_mouse_wheel_callback = [&game, &update_camera_uniforms,
                                     &reset_accumulation](i16 delta) {
        game.main_camera->orbit_mouse_wheel(delta);
        update_camera_uniforms();
        reset_accumulation();
    };

    game.main_camera->m_orbit_zoom = 2.0f;
    game.main_camera->m_orbit_target = Vec3(0.0, 0.0, 1.0);
    game.main_camera->m_orbit_spherical_angles = Vec2(f32(PI_HALF), f32(PI) / 10.0);

    game.main_camera->update_orbit_camera();

    update_camera_uniforms();

    game.m_main_loop = [&game, &reset_accumulation_counter]() {
        if (game.m_window->event_loop()) {
            game.m_renderer->update();

            if (reset_accumulation_counter == 2) {
                reset_accumulation_counter--;
            } else if (reset_accumulation_counter == 1) {
                game.m_renderer->set_uniform_buffer_data("u_info",
                                                         {{"u_reset_accumulation", 0.0f}});
                reset_accumulation_counter--;
            }

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}