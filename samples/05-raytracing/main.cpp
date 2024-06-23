#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"
#include "my_time.h"

using namespace blaz;

int main() {
    Game game; 

    Error err = game.load_game("data/game.cfg");
    if (err) {
        logger.error(err);
    }

    err = game.m_window.init("05-raytracing");
    if (err) {
        logger.error(err);
        return 1;
    }

    err = game.m_renderer.init(&game);
    if (err) {
        logger.error(err);
        return 1;
    }

    game.main_loop = [&game]() {
        if (game.m_window.event_loop()) {
            game.m_renderer.update();

            if (!game.took_screen_start) {
                game.m_window.screenshot("05-raytracing.bmp");
                game.took_screen_start = true;
            }

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}