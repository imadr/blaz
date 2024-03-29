#include "error.h"
#include "game.h"
#include "logger.h"
#include "my_time.h"

using namespace blaz;

int main() {
    Game game;

    Error err = game.load_game("data/game.cfg");
    if (err) {
        logger.error(err);
    }

    err = game.m_window.init("01-hellotriangle");
    if (err) {
        logger.error(err);
        return 1;
    }

    err = game.m_renderer.init(&game);
    if (err) {
        logger.error(err);
        return 1;
    }

    bool done_screenshot = false;
    while (game.m_window.event_loop()) {
        game.m_renderer.draw();
        if (!done_screenshot) {
            game.m_window.screenshot("../tests/01-hellotriangle.bmp");
            done_screenshot = true;
        }
    }

    game.m_window.close();

    return 0;
}