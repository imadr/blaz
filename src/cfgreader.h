#pragma once

#include <map>

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

    CfgNodeType type = CfgNodeType::NONE;

    CfgNode operator[](int index) {
        if (type == CfgNodeType::ARRAY) {
            return array_value[index];
        }
    }

    CfgNode operator[](const str& key) {
        if (type == CfgNodeType::MAP) {
            return map_value[key];
        }
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
    }

    IteratorVariant end() {
        if (type == CfgNodeType::ARRAY) {
            return IteratorVariant(VectorIterator(array_value.end()));
        } else if (type == CfgNodeType::MAP) {
            return IteratorVariant(MapIterator(map_value.end()));
        }
    }
};
using Cfg = CfgNode;

pair<Error, CfgNode> read_cfg_file(str path);

}  // namespace blaz