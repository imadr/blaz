#include "error.h"
#include "game.h"
#include "logger.h"
#include "memory.h"
#include "my_time.h"

using namespace blaz;

void read_ttf_file() {
    pair<Error, vec<u8>> file_read = read_whole_file_binary("data/OpenSans-Regular.ttf");

    if (file_read.first) {
        logger.error(file_read.first);
        return;
    }

    u8* base_ptr = file_read.second.data();
    u8* ptr = base_ptr;

    ptr += 4;
    u16 num_tables = read_value<u16>(ptr, true);
    printf("num tables: %d\n", (num_tables));

    ptr += 2 * 4;

    std::unordered_map<str, u32> table_offsets;
    for (u32 i = 0; i < num_tables; i++) {
        str name;
        name += read_value<u8>(ptr, false);
        ptr += 1;
        name += read_value<u8>(ptr, false);
        ptr += 1;
        name += read_value<u8>(ptr, false);
        ptr += 1;
        name += read_value<u8>(ptr, false);
        ptr += 1;
        ptr += 4;
        table_offsets[name] = read_value<u32>(ptr, true);
        ptr += 8;
    }

    u32 offset = table_offsets.at("glyf");
    ptr = base_ptr + offset;

    i16 num_contours = read_value<i16>(ptr, true);
    printf("%d ", num_contours);
}

int main() {
    Window window;
    Error err = window.init("");
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
    game.m_renderer = &renderer;
    game.m_scene = &scene;
    err = game.load_game("data/game.cfg");
    if (err) {
        logger.error(err);
    }

    game.m_main_loop = [&game]() {
        if (game.m_window->event_loop()) {
            imdraw.start();
            imdraw.draw_rect(Vec2(10, 10), Vec2(200, 100), RGB(1, 0, 1));
            imdraw.end();

            game.m_renderer->update();

            return true;
        }
        return false;
    };

    game.run();

    return 0;
}