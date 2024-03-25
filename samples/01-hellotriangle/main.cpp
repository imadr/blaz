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

    // bool done_screenshot = false;
    u64 last_time = get_timestamp_microsecond();
    while (game.m_window.event_loop()) {
        u64 current_time = get_timestamp_microsecond();
        f32 delta_time = ((f32)(current_time - last_time) / 1000);
        last_time = current_time;
        game.m_renderer.draw();

        logger.info("", 1000.0 / delta_time, " fps");

        // if (!done_screenshot) {
        //     game.m_window.screenshot("../tests/01-hellotriangle.bmp");
        //     done_screenshot = true;
        // }
    }

    game.m_window.close();

    return 0;
}