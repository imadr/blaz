#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"
#include "my_time.h"

using namespace blaz;

int main() {
    Window window;
    Error err = window.init("06-physics");
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

    Physics physics;
    physics.init();

    Scene scene;
    init_scene(&scene);

    Game game;
    game.m_window = &window;
    game.m_renderer = &renderer;
    game.m_physics = &physics;
    game.m_scene = &scene;
    err = game.load_game("data/game.cfg");
    if (err) {
        logger.error(err);
    }

    renderer.create_mesh(make_cube_wireframe());
    renderer.create_mesh(make_cube());
    renderer.create_mesh(make_plane());
    renderer.create_mesh(make_wireframe_sphere(64));

    for (u32 i = 0; i < 1; i++) {
        for (u32 j = 0; j < 1; j++) {
            str name = "cube_" + std::to_string(i) + "_" + std::to_string(j);
            // .m_position = Vec3((-1.0f + f32(i)) * 2.5f, (-1.0f + f32(j)) * 2.5f, 0.0f),
            add_node(&scene,
                     Node{
                         .m_name = name,
                         .m_position = Vec3(0.0, 2.0f, 0.0),
                         .m_rotation = Quat::from_euler(Vec3(0, 0, 0)),
                     },
                     "root_node");
            game.m_renderer->create_renderable(Renderable{
                .m_name = name,
                .m_tags = {"default"},
                .m_mesh = "cube",
                .m_node = name,
            });
            game.m_renderer->create_renderable(Renderable{
                .m_name = name,
                .m_tags = {"wireframe"},
                .m_mesh = "cube_wireframe",
                .m_node = name,
            });
            game.m_physics->create_rigidbody(Rigidbody{
                .m_name = name,
                .m_node = name,
                .m_position = game.m_scene->m_nodes[name].m_position,
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
    //     .m_mesh = m_renderer.m_meshes["wireframe_sphere"],
    //     .m_node = game.m_current_level->m_scene.m_nodes["wireframe_sphere"],
    // });

    scene.m_nodes[renderer.m_cameras[0].m_node].set_rotation(
        Quat::from_euler(Vec3(f32(-PI / 8), 0.0, 0.0)));

    u64 last_time = get_timestamp_microsecond();
    game.main_loop = [&game, &last_time]() {
        if (game.m_window->event_loop()) {
            u64 current_time = get_timestamp_microsecond();
            f32 delta_time = ((f32)(current_time - last_time) / 1000);
            last_time = current_time;

            game.m_physics->update(delta_time / 1000.0f);
            game.m_renderer->update();

            if (!game.took_screenshot_start) {
                game.m_window->screenshot("06-physics.bmp");
                game.took_screenshot_start = true;
            }

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}