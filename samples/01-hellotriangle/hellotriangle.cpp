#include "game.h"
#include "logger.h"

using namespace blaz;

int main() {
    Game game;

    Error err = game.load_level("data/level.cfg");

    if (err) {
        logger.error(err);
    }

    err = game.m_window.init();
    if (err) {
        logger.error(err);
        return 1;
    }

    /*err = m_renderer.init(&m_window);
    if (err) {
        logger.error(err);
        return 1;
    }*/

    while (game.m_window.event_loop()) {

    }

    game.m_window.close();

    return 0;
}