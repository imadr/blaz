#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"
#include "my_time.h"
#include "texture.h"

using namespace blaz;

int main() {
    Window window;
    Error err = window.init("03-pbr");
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

    renderer.create_mesh(load_mesh_from_obj_file("data/damagedhelmet.obj").second);

    add_node(&scene,
             Node{
                 .m_name = "damagedhelmet",
                 .m_position = Vec3(0, 0, 0),
                 .m_rotation = Quat::from_euler(Vec3(0, 0, 0)),
             },
             "root_node");
    renderer.create_renderable(Renderable{
        .m_name = "damagedhelmet",
        .m_tags = {"pbr"},
        .m_mesh = "damagedhelmet",
        .m_node = "damagedhelmet",
    });

    renderer.m_passes[0].m_sampler_uniforms_bindings = {
        {"u_sampler_albedo", "damagedhelmet_albedo"},
        {"u_sampler_metalroughness", "damagedhelmet_metalroughness"},
        {"u_sampler_normals", "damagedhelmet_normals"},
        {"u_sampler_emissive", "damagedhelmet_emissive"},
    };

    game.main_camera->m_orbit_pan_sensitivity = 0.005f;

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

    game.main_loop = [&game]() {
        if (game.m_window->event_loop()) {
            game.m_renderer->update();

            if (!game.took_screenshot_start) {
                game.m_window->screenshot("03-pbr.bmp");
                game.took_screenshot_start = true;
            }

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}