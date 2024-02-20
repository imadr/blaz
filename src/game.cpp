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
    for (auto& pipeline_cfg : pipelines) {
        Cfg passes_cfg = pipeline_cfg["passes"];
        for (auto& pass_cfg : passes_cfg) {
            vec<CfgNode> clears = pass_cfg["clear"].array_value;
            u32 clear_flag = 0;
            for (auto& clear : clears) clear_flag |= ClearStr[clear.str_value];

            Pass pass;

            if (pass_cfg["name"]) pass.m_name = pass_cfg["name"].str_value;
            if (pass_cfg["type"]) pass.m_type = pass_cfg["type"].str_value;
                // .m_clear_flag = clear_flag,
                // .m_clear_color = pass_cfg["clear_color"].vec4_value,
                // .m_clear_depth = pass_cfg["clear_depth"],
                // .m_shader = pass_cfg["shader"],
                // .m_tags = pass_cfg["tags"],
                // .m_framebuffer = pass_cfg["framebuffer"],
                // .m_camera = pass_cfg["camera"],
                // .m_enabled = pass_cfg["enabled"],
                // .m_use_default_framebuffer = pass_cfg["use_default_framebuffer"],
                // .m_enable_depth_test = pass_cfg["enable_depth_test"],
                // .m_enable_face_culling = pass_cfg["enable_face_culling"],
                // .m_culling_mode = pass_cfg["culling_mode"],
            //};
            // logger.info(pass["name"].str_value);
        }
    }

    return Error();
}

Error Game::load_level(u32 level) {
    m_current_level = level;
    m_renderer.m_current_pipeline = &m_renderer.m_pipelines[m_levels[m_current_level].m_pipeline];
    return Error();
}

}  // namespace blaz