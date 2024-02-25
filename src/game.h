#pragma once

#include "error.h"
#include "node.h"
#include "platform.h"
#include "renderer.h"
#include "types.h"

namespace blaz {

struct Level {
    str m_name;
    u32 m_pipeline;
    Scene m_scene;
    vec<Renderable> m_renderables;
    std::unordered_map<str, vec<u32>> m_tagged_renderables;
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
