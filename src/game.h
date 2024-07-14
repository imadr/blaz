#pragma once

#include "error.h"
#include "node.h"
#include "physics.h"
#include "platform.h"
#include "renderer.h"
#include "types.h"

namespace blaz {

struct Game {
    Window* m_window = NULL;
    Renderer* m_renderer = NULL;
    Physics* m_physics = NULL;
    Scene* m_scene = NULL;
    Camera* main_camera = NULL;

    Error load_game(str path);

    std::function<bool()> m_main_loop;
    void run();
};

}  // namespace blaz
