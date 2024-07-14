#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"

using namespace blaz;

int main() {
    Window window;
    Error err = window.init("02-cubes");
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

    make_cube(&renderer.m_meshes["cube_mesh"]);

    f32 rotation = 1;
    game.m_main_loop = [&game, &rotation]() {
        if (game.m_window->event_loop()) {
            rotation += 0.01f;
            game.m_scene->m_nodes[2].set_rotation(Quat::from_axis_angle(Vec3(1, 1, 0), rotation));

            game.m_renderer->update();

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}