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

    err = game.m_window.init("03-pbr");
    if (err) {
        logger.error(err);
        return 1;
    }

    // game.m_renderer.add_mesh(make_sphere());
    // f32 rotation = 1;
    // game.m_current_level->m_scene.add_node(
    //     Node{.m_name = "cube", .m_rotation = Quat::from_axis_angle(Vec3(1, 1, 0), rotation)},
    //     "root_node");
    // game.m_renderer.add_renderable(Renderable{
    //     .m_name = "cube",
    //     .m_tags = {"default"},
    //     .m_mesh = 0,
    //     .m_node = game.m_current_level->m_scene.m_nodes_ids["cube"],
    // });

    err = game.m_renderer.init(&game);
    if (err) {
        logger.error(err);
        return 1;
    }

    bool done_screenshot = false;
    while (game.m_window.event_loop()) {
        game.m_renderer.draw();
        if (!done_screenshot) {
            game.m_window.screenshot("../tests/03-pbr.bmp");
            done_screenshot = true;
        }
    }

    game.m_window.close();

    return 0;
}