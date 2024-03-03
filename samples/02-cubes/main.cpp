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

    //     game.m_renderer.add_mesh(
    // Mesh{
    //     .m_name = "cube"
    //     vec<f32> m_vertices;
    //     vec<u32> m_indices;
    //     vec<pair<str, u32>> m_attribs;
    //     void* m_api_data = NULL;

    //     bool m_should_reload = false;
    // };
    //         )
    Mesh cube = make_cube();
    game.m_renderer.m_meshes.push_back(cube);
    game.m_renderer.m_meshes_ids["cube"] = 0;

    game.m_levels[game.m_current_level].m_renderables.push_back(Renderable{
        .m_name = "cube",
        .m_tags = {"default"},
        .m_mesh = 0,
        .m_node = 0,
    });
    game.m_levels[game.m_current_level].m_tagged_renderables["default"] = {0};

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