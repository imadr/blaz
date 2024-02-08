#include "game.h"

#include "cfgreader.h"
#include "error.h"
#include "logger.h"
#include "types.h"

namespace blaz {

Error Game::load_game(str path) {
    pair<Error, Cfg> cfg = read_cfg_file(path);
    if (cfg.first) {
        return cfg.first;
    }
    Cfg game_cfg = cfg.second;

    Cfg pipelines = game_cfg["pipelines"];
    /*for (Cfg& pipeline : pipelines) {
        Cfg passes = pipeline["passes"];
        // for (Cfg& pass : passes) {
        //     logger.info(pass["name"]);
        // }
    }*/

    return Error();
}

Error Game::load_level(u32 level) {
    m_current_level = level;
    m_renderer.m_current_pipeline = &m_renderer.m_pipelines[m_levels[m_current_level].m_pipeline];
    return Error();
}

}  // namespace blaz