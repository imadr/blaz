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

    err = game.m_renderer.init(&game);
    if (err) {
        logger.error(err);
        return 1;
    }

    f32 rotation = 1;
    game.main_loop = [&game, &rotation]() {
        if (game.m_window.event_loop()) {
            rotation += 0.01f;
            game.m_scene.m_nodes[2].set_rotation(Quat::from_axis_angle(Vec3(1, 1, 0), rotation));

            game.m_renderer.update();

            if (!game.took_screen_start) {
                game.m_window.screenshot("02-cubes.bmp");
                game.took_screen_start = true;
            }

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}