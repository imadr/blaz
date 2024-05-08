#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"
#include "my_time.h"
#include "texture.h"

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

    game.m_renderer.add_mesh(load_mesh_from_obj_file("data/damagedhelmet.obj").second);

    add_node(&game.m_scene,
             Node{
                 .m_name = "damagedhelmet",
                 .m_position = Vec3(0, 0, 0),
                 .m_rotation = Quat::from_euler(Vec3(0, 0, 0)),
             },
             "root_node");
    game.m_renderer.add_renderable(Renderable{
        .m_name = "damagedhelmet",
        .m_tags = {"pbr"},
        .m_mesh = 0,
        .m_node = game.m_scene.m_nodes_ids["damagedhelmet"],
    });

    err = game.m_renderer.init(&game);
    if (err) {
        logger.error(err);
        return 1;
    }
    game.m_renderer.m_pipelines[0].m_passes[0].m_texture_uniforms_binding = {
        {"u_texture_albedo", "damagedhelmet_albedo"},
        {"u_texture_metalroughness", "damagedhelmet_metalroughness"},
        {"u_texture_normals", "damagedhelmet_normals"},
        {"u_texture_emissive", "damagedhelmet_emissive"},
    };

    game.m_window.m_mouse_move_callback = [&game](Vec2I mouse_delta) {
        // game.main_camera->mouse_move(mouse_delta);
        // logger.info(mouse_delta.to_str());
    };

    game.m_window.m_mouse_click_callback = [&game](Vec2I mouse_position, ButtonState left_button,
                                                   ButtonState right_button) {
        if (left_button == ButtonState::PRESSED) {
            game.main_camera.m_arcball.m_start_mouse_pos = mouse_position;
        }
    };

    game.main_loop = [&game]() {
        if (game.m_window.event_loop()) {
            game.m_renderer.draw();
            return true;
        }
        return false;
    };

    game.run();

    return 0;
}