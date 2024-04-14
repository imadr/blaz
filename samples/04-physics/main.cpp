#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"
#include "my_time.h"

using namespace blaz;

int main() {
    Game game;

    game.m_renderer.add_mesh(make_cube_wireframe());
    game.m_renderer.add_mesh(make_cube());
    game.m_renderer.add_mesh(make_plane());
    game.m_renderer.add_mesh(make_wireframe_sphere(64));

    Error err = game.load_game("data/game.cfg");
    if (err) {
        logger.error(err);
    }

    err = game.m_window.init("04-physics");
    if (err) {
        logger.error(err);
        return 1;
    }

    for (u32 i = 0; i < 1; i++) {
        for (u32 j = 0; j < 1; j++) {
            str name = "cube_" + std::to_string(i) + "_" + std::to_string(j);
            // .m_position = Vec3((-1.0f + f32(i)) * 2.5f, (-1.0f + f32(j)) * 2.5f, 0.0f),
            add_node(&game.m_current_level->m_scene,
                     Node{
                         .m_name = name,
                         .m_position = Vec3(0.0, 2.0f, 0.0),
                         .m_rotation = Quat::from_euler(Vec3(0, 0, 0)),
                     },
                     "root_node");
            u32 cube_node_id = game.m_current_level->m_scene.m_nodes_ids[name];
            game.m_renderer.add_renderable(Renderable{
                .m_name = name,
                .m_tags = {"default"},
                .m_mesh = game.m_renderer.m_meshes_ids["cube"],
                .m_node = cube_node_id,
            });
            game.m_renderer.add_renderable(Renderable{
                .m_name = name,
                .m_tags = {"wireframe"},
                .m_mesh = game.m_renderer.m_meshes_ids["cube_wireframe"],
                .m_node = cube_node_id,
            });
            game.m_physics.add_rigidbody(Rigidbody{
                .m_name = name,
                .m_node = cube_node_id,
                .m_position = game.m_current_level->m_scene.m_nodes[cube_node_id]
                                  .m_position,  // this is ugly, do something else
                .m_mass = 1.0,
            });
        }
    }

    // add_node(&game.m_current_level->m_scene,
    //          Node{
    //              .m_name = "wireframe_sphere",
    //              .m_position = Vec3(0.0, -1.0f, 0.0),
    //              .m_rotation = Quat::from_euler(Vec3(0, 0, 0)),
    //          },
    //          "root_node");
    // game.m_renderer.add_renderable(Renderable{
    //     .m_name = "wireframe_sphere",
    //     .m_tags = {"wireframe"},
    //     .m_mesh = game.m_renderer.m_meshes_ids["wireframe_sphere"],
    //     .m_node = game.m_current_level->m_scene.m_nodes_ids["wireframe_sphere"],
    // });

    game.m_current_level->m_scene.m_nodes[game.m_renderer.m_cameras[0].m_node].set_rotation(
        Quat::from_euler(Vec3(f32(-PI / 8), 0.0, 0.0)));

    err = game.m_renderer.init(&game);
    if (err) {
        logger.error(err);
        return 1;
    }

    u64 last_time = get_timestamp_microsecond();
    bool done_screenshot;
    game.main_loop = [&game, &last_time, &done_screenshot]() {
        if (game.m_window.event_loop()) {
            u64 current_time = get_timestamp_microsecond();
            f32 delta_time = ((f32)(current_time - last_time) / 1000);
            last_time = current_time;

            game.m_physics.update(delta_time / 1000.0f);
            game.m_renderer.draw();
            if (!done_screenshot) {
                game.m_window.screenshot("../tests/04-physics.bmp");
                done_screenshot = true;
            }
            return true;
        }
        return false;
    };

    game.run();

    return 0;
}