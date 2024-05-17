#pragma once

#include "error.h"
#include "node.h"
#include "physics.h"
#include "platform.h"
#include "renderer.h"
#include "types.h"

namespace blaz {

struct Game {
    Window m_window;
    Renderer m_renderer;
    Physics m_physics;
    Scene m_scene;

    Camera* main_camera = NULL;

    Error load_game(str path);

    Node* get_node_by_name(str name) {
        return &m_scene.m_nodes[m_scene.m_nodes_ids[name]];
    }

    std::function<bool()> main_loop;
    void run();
};

}  // namespace blaz
