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

    game.m_renderer.add_mesh(make_uv_sphere(64, 64));

    game.m_current_level->m_scene.add_node(
        Node{
            .m_name = "sphere",
            .m_position = Vec3(0, 0, 0),
            .m_rotation = Quat::from_euler(Vec3(0, 0, 0)),
        },
        "root_node");
    game.m_renderer.add_renderable(Renderable{
        .m_name = "sphere",
        .m_tags = {"pbr"},
        .m_mesh = 0,
        .m_node = game.m_current_level->m_scene.m_nodes_ids["sphere"],
    });

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