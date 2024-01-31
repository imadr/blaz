#pragma once

#include "cfgreader.h"
#include "error.h"
#include "platform.h"

namespace blaz {

struct Level {
    /*Pipeline m_pipeline;
    Scene m_scene;
    vec<Mesh> m_meshes;
    vec<Shader> m_shaders;
    vec<Renderable> m_renderables;*/
};

struct Game {
    Window m_window;
    //Renderer m_renderer;
    vec<Level> m_levels;

    Error load_level(str path);
    pair<Error, Level> load_level_from_cfg(CfgNode cfg);
};

}  // namespace blaz
