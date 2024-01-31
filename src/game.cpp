#include "game.h"

namespace blaz {

pair<Error, Level> Game::load_level_from_cfg(CfgNode cfg) {
    return std::make_pair(Error(), Level());
}

Error Game::load_level(str path) {
    pair<Error, CfgNode> cfg = read_cfg_file(path);
    if (cfg.first) {
        return cfg.first;
    }

    pair<Error, Level> level = load_level_from_cfg(cfg.second);
    if (level.first) {
        return level.first;
    }

    m_levels.push_back(level.second);
    return Error();
}

}  // namespace blaz