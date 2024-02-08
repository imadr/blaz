#pragma once

#include "error.h"
#include "node.h"
#include "platform.h"
#include "renderer.h"
#include "types.h"

namespace blaz {

struct Level {
    u32 m_pipeline;
    Scene m_scene;
    Renderable m_renderables[100];
};

struct Game {
    Window m_window;
    Renderer m_renderer;
    Level m_levels[10];
    u32 m_current_level = 0;

    Error load_game(str path);
    Error load_level(u32 level);
};

}  // namespace blaz
