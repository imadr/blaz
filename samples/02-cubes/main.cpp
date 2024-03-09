#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"

using namespace blaz;

int main() {
    Game game;

    Error err = game.load_game("data/game.cfg");
    if (err) {
        logger.error(err);
    }

    err = game.m_window.init("02-cubes");
    if (err) {
        logger.error(err);
        return 1;
    }

    game.m_renderer.add_mesh(make_cube());
    game.m_renderer.add_renderable(Renderable{
        .m_name = "cube",
        .m_tags = {"default"},
        .m_mesh = 0,
        .m_node = 0,
    });
    /*game.m_level.add_node(Node{
        .m_name = "cube",
        .m_position = Vec3(0, 0, 0),
        .m_rotation = Quat(),
        .m_scale = Vec3(1, 1, 1),
    });*/

    err = game.m_renderer.init(&game);
    if (err) {
        logger.error(err);
        return 1;
    }

    bool done_screenshot = false;
    while (game.m_window.event_loop()) {
        game.m_renderer.draw();
        if (!done_screenshot) {
            game.m_window.screenshot("../tests/02-cubes.bmp");
            done_screenshot = true;
        }
    }

    game.m_window.close();

    return 0;
}