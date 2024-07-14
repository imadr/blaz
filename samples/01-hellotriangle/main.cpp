#include "error.h"
#include "game.h"
#include "logger.h"
#include "my_time.h"

using namespace blaz;

int main() {
    Window window;
    Error err = window.init("01-hellotriangle");
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