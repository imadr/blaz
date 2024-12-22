#include "error.h"
#include "game.h"
#include "logger.h"
#include "texture.h"

using namespace blaz;

int main() {
    Window window;
    Error err = window.init("08-fps");
    // window.m_cursor_visible = false;
    // window.m_cursor_locked = true;

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
    make_plane(&renderer.m_meshes["plane_mesh"]);
    make_uv_sphere(&renderer.m_meshes["sphere_mesh"], 32, 32);

    renderer.create_uniform_buffer(UniformBuffer{
        .m_name = "u_light",
        .m_uniforms =
            {
                Uniform{
                    .m_name = "u_light_position",
                    .m_type = UNIFORM_VEC3,
                },
                Uniform{
                    .m_name = "u_light_view_mat",
                    .m_type = UNIFORM_MAT4,
                },
                Uniform{
                    .m_name = "u_light_projection_mat",
                    .m_type = UNIFORM_MAT4,
                },
                Uniform{
                    .m_name = "u_light_color",
                    .m_type = UNIFORM_VEC3,
                },
            },
        .m_should_reload = true,
    });

    renderer.create_uniform_buffer(UniformBuffer{
        .m_name = "u_material",
        .m_uniforms =
            {
                Uniform{
                    .m_name = "u_albedo",
                    .m_type = UNIFORM_VEC3,
                },
            },
        .m_should_reload = true,
    });

    Camera* light_camera = &renderer.m_cameras["light_camera"];
    Vec3 light_pos = Vec3(-2, 4, -1);
    light_camera->m_z_far = 100.0;
    scene.m_nodes["light_camera_node"].set_position(light_pos);
    scene.m_nodes["light_camera_node"].set_rotation(
        Quat::look_at(light_pos, Vec3(0, 0, 0), Vec3(0, 1, 0)));
    light_camera->update_projection_matrix();
    light_camera->update_view_matrix();
    renderer.set_uniform_buffer_data(
        "u_light", {{"u_light_position", scene.m_nodes[light_camera->m_node].m_position},
                    {"u_light_view_mat", light_camera->m_view_matrix},
                    {"u_light_projection_mat", light_camera->m_projection_matrix},
                    {"u_light_color", Vec3(50, 50, 50)}});

    game.main_camera->m_fps_yaw = f32(PI_HALF);
    game.main_camera->m_fps_pitch = 0.0f;
    game.main_camera->m_camera_mode = CameraMode::FPS;

    window.m_mouse_move_raw_callback = [&game](Vec2I delta) {
        game.main_camera->fps_mouse_move(delta);
    };

    renderer.create_uniform_buffer(UniformBuffer{
        .m_name = "u_camera",
        .m_uniforms =
            {
                Uniform{
                    .m_name = "u_camera_position",
                    .m_type = UNIFORM_VEC3,
                },
                Uniform{
                    .m_name = "u_skydome_position",
                    .m_type = UNIFORM_VEC3,
                },
            },
        .m_should_reload = true,
    });

    game.m_main_loop = [&game]() {
        if (game.m_window->event_loop()) {
            game.m_renderer->set_uniform_buffer_data(
                "u_camera",
                {{"u_camera_position", game.m_scene->m_nodes[game.main_camera->m_node].m_position},
                 {"u_skydome_position", game.m_scene->m_nodes["skydome_node"].m_position}});

            Vec3 forward(-cos(game.main_camera->m_fps_yaw), 0, -sin(game.main_camera->m_fps_yaw));
            Vec3 right(sin(game.main_camera->m_fps_yaw), 0, -cos(game.main_camera->m_fps_yaw));

            Vec3 move_dir(0, 0, 0);
            const float MOVE_SPEED = 0.01f;

            if (game.m_window->m_keyboard["KEY_W"] == KeyState::PRESSED) {
                move_dir += forward;
            }
            if (game.m_window->m_keyboard["KEY_S"] == KeyState::PRESSED) {
                move_dir -= forward;
            }
            if (game.m_window->m_keyboard["KEY_A"] == KeyState::PRESSED) {
                move_dir -= right;
            }
            if (game.m_window->m_keyboard["KEY_D"] == KeyState::PRESSED) {
                move_dir += right;
            }

            if (game.m_window->m_keyboard["KEY_ESCAPE"] == KeyState::PRESSED) {
                return false;
            }

            if (move_dir.length() * move_dir.length() > 0.0f) {
                move_dir = move_dir.normalize();
                game.m_scene->m_nodes[game.main_camera->m_node].translate(move_dir * MOVE_SPEED);
            }

            game.m_renderer->update();

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}