#pragma once

#include <map>
#include <variant>

#include "error.h"
#include "types.h"

namespace blaz {

enum class CfgNodeType { MAP, ARRAY, STR, FLOAT, BOOL, NONE };

struct CfgNode {
    std::map<str, CfgNode> map_value;
    vec<CfgNode> array_value;
    str str_value = "";
    float float_value = 0;
    bool bool_value = false;

    // std::variant<std::map<str, CfgNode*>, vec<CfgNode*>, str, float, bool> test; @fix

    CfgNodeType type = CfgNodeType::NONE;

    CfgNode operator[](int index) {
        if (type == CfgNodeType::ARRAY) {
            return array_value[index];
        }
    }

    CfgNode operator[](const std::string& key) {
        if (type == CfgNodeType::MAP) {
            return map_value[key];
        }
    }
};

pair<Error, CfgNode> read_cfg_file(str path);

}  // namespace blaz