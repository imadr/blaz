#pragma once

#include <map>
#include <variant>

#include "error.h"
#include "my_math.h"
#include "types.h"

namespace blaz {

enum class CfgNodeType { MAP, ARRAY, STR, FLOAT, VEC4, VEC3, BUFFER_F32, BUFFER_U32, BOOL, NONE };

struct CfgNode {
    std::map<str, CfgNode> map_value;
    vec<CfgNode> array_value;
    str str_value = "";
    float float_value = 0;
    bool bool_value = false;
    Vec4 vec4_value;
    Vec3 vec3_value;
    vec<f32> buffer_f32_value;
    vec<u32> buffer_u32_value;

    CfgNodeType type = CfgNodeType::NONE;

    explicit operator bool() const {
        return type != CfgNodeType::NONE;
    }

    CfgNode operator[](int index) {
        if (type == CfgNodeType::ARRAY && index < array_value.size()) {
            return array_value[index];
        }
        return CfgNode{.type = CfgNodeType::NONE};
    }

    CfgNode operator[](const str& key) {
        if (type == CfgNodeType::MAP && map_value.count(key) > 0) {
            return map_value[key];
        }
        return CfgNode{.type = CfgNodeType::NONE};
    }

    struct VectorIterator {
        vec<CfgNode>::iterator current;

        VectorIterator(vec<CfgNode>::iterator iter) : current(iter) {
        }

        bool operator!=(const VectorIterator& other) const {
            return current != other.current;
        }

        CfgNode& operator*() {
            return *current;
        }

        VectorIterator& operator++() {
            ++current;
            return *this;
        }
    };

    struct MapIterator {
        std::map<std::string, CfgNode>::iterator current;

        MapIterator(std::map<std::string, CfgNode>::iterator iter) : current(iter) {
        }

        bool operator!=(const MapIterator& other) const {
            return current != other.current;
        }

        CfgNode& operator*() {
            return current->second;
        }

        MapIterator& operator++() {
            ++current;
            return *this;
        }
    };

    struct IteratorVariant {
        std::variant<MapIterator, VectorIterator> iterator;

        IteratorVariant(MapIterator mapIter) : iterator(mapIter) {
        }
        IteratorVariant(VectorIterator vectorIter) : iterator(vectorIter) {
        }

        bool operator!=(const IteratorVariant& other) const {
            return iterator != other.iterator;
        }

        CfgNode& operator*() {
            return std::visit([](auto& iter) -> CfgNode& { return *iter; }, iterator);
        }

        IteratorVariant& operator++() {
            std::visit([](auto& iter) { ++iter; }, iterator);
            return *this;
        }
    };

    IteratorVariant begin() {
        if (type == CfgNodeType::ARRAY) {
            return IteratorVariant(VectorIterator(array_value.begin()));
        } else if (type == CfgNodeType::MAP) {
            return IteratorVariant(MapIterator(map_value.begin()));
        }
        return IteratorVariant(VectorIterator(array_value.end()));
    }

    IteratorVariant end() {
        if (type == CfgNodeType::ARRAY) {
            return IteratorVariant(VectorIterator(array_value.end()));
        } else if (type == CfgNodeType::MAP) {
            return IteratorVariant(MapIterator(map_value.end()));
        }
        return IteratorVariant(VectorIterator(array_value.end()));
    }
};
using Cfg = CfgNode;

pair<Error, CfgNode> read_cfg_file(const str& path);

}  // namespace blaz