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

    for (u32 i = 0; i < 3; i++) {
        for (u32 j = 0; j < 3; j++) {
            str id = "sphere_" + std::to_string(i) + "_" + std::to_string(j);
            game.m_current_level->m_scene.add_node( 
                Node{
                    .m_name = id,
                    .m_position = Vec3((-1.0f + f32(i)) * 2.5f, (-1.0f + f32(j)) * 2.5f, 0.0f),
                    .m_rotation = Quat::from_euler(Vec3(0, 0, 0)),
                },
                "root_node");
            game.m_renderer.add_renderable(Renderable{
                .m_name = id,
                .m_tags = {"pbr"},
                .m_mesh = 0,
                .m_node = game.m_current_level->m_scene.m_nodes_ids[id],
            });
        }
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
            game.m_window.screenshot("../tests/03-pbr.bmp");
            done_screenshot = true;
        }
    }

    game.m_window.close();

    return 0;
}