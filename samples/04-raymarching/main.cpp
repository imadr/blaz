#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"
#include "my_time.h"

using namespace blaz;

int main() {
    Game game;

    Error err = game.load_game("data/game.cfg");
    if (err) {
        logger.error(err);
    }

    err = game.m_window.init("04-raymarching");
    if (err) {
        logger.error(err);
        return 1;
    }

    err = game.m_renderer.init(&game);
    if (err) {
        logger.error(err);
        return 1;
    }

    game.m_renderer.add_uniform_buffer(UniformBuffer{
        .m_name = "u_info",
        .m_binding_point = 0,
        .m_uniforms =
            {
                Uniform{
                    .m_name = "u_resolution",
                    .m_type = UNIFORM_VEC2,
                },
            },
        .m_should_reload = true,
    });

    auto callback = [&game](Window* window) {
        UniformBuffer* mat_buffer =
            &game.m_renderer.m_uniform_buffers[game.m_renderer.m_uniform_buffers_ids["u_info"]];
        game.m_renderer.set_uniform_buffer_data(
            mat_buffer, "u_resolution",
            Vec2(f32(game.m_window.m_size.width), f32(game.m_window.m_size.height)));
    };
    game.m_window.m_resize_callbacks.push_back(callback);
    callback(&game.m_window);

    game.main_loop = [&game]() {
        if (game.m_window.event_loop()) {
            game.m_renderer.update();

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}