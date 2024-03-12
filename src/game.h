#pragma once

#include "error.h"
#include "node.h"
#include "platform.h"
#include "renderer.h"
#include "types.h"

namespace blaz {

struct Level {
    str m_name;
    Scene m_scene;
    u32 m_pipeline;
};

struct Game {
    Window m_window;
    Renderer m_renderer;
    vec<Level> m_levels;
    u32 m_current_level_id = 0;
    Level* m_current_level;

    Error load_game(str path);
    Error load_level(u32 level);
};

}  // namespace blaz
