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

    err = game.m_renderer.init(&game);
    if (err) {
        logger.error(err);
        return 1;
    }

    while (game.m_window.event_loop()) {

        game.m_renderer.draw();
    }

    game.m_window.close();

    return 0;
}