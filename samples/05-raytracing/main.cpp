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

    Texture texture;
    texture.m_name = "test_texture";
    texture.m_texture_params.m_format = TextureFormat::RGB8;
    texture.m_width = window.m_size.width;
    texture.m_height = window.m_size.height;
    renderer.create_texture(texture);

    Framebuffer framebuffer;
    framebuffer.m_name = "test_framebuffer";
    framebuffer.m_width = window.m_size.width;
    framebuffer.m_height = window.m_size.height;
    framebuffer.m_color_attachment_texture = "test_texture";
    renderer.create_framebuffer(framebuffer);
    renderer.attach_texture_to_framebuffer("test_framebuffer", "test_texture",
                                           AttachementPoint::COLOR_ATTACHMENT);

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