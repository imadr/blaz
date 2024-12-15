#include "error.h"
#include "game.h"
#include "logger.h"
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