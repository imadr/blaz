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
};

struct Game {
    Window m_window;
    Renderer m_renderer;
    vec<Level> m_levels;
    u32 m_current_level = 0;

    Error load_game(str path);
    Error load_level(u32 level);
};

}  // namespace blaz
