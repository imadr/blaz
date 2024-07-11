#include "error.h"
#include "game.h"
#include "logger.h"
#include "mesh.h"
#include "my_time.h"

using namespace blaz;

int main() {
    Window window;
    Error err = window.init("05-raytracing");
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

    Texture compute_output_texture;
    compute_output_texture.m_name = "compute_output";
    compute_output_texture.m_texture_params.m_format = TextureFormat::RGBA32F;
    compute_output_texture.m_width = window.m_size.width;
    compute_output_texture.m_height = window.m_size.height;
    renderer.create_texture(compute_output_texture);

    renderer.m_passes[0].m_image_uniforms_binding = {
        {"u_image_compute_output", "compute_output"},
    };

    renderer.m_passes[1].m_sampler_uniforms_binding = {
        {"u_sampler_compute_output", "compute_output"},
    };

    game.main_loop = [&game]() {
        if (game.m_window->event_loop()) {
            game.m_renderer->update();

            if (!game.took_screenshot_start) {
                game.m_window->screenshot("05-raytracing.bmp");
                game.took_screenshot_start = true;
            }

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}