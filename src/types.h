#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace blaz {

using u64 = uint64_t;
using i64 = int64_t;
using u32 = uint32_t;
using i32 = int32_t;
using u16 = uint16_t;
using i16 = int16_t;
using u8 = uint8_t;
using i8 = int8_t;
using f32 = float;
using f64 = double;

using str = std::string;

template <class T>
using opt = std::optional<T>;

template <typename T>
using vec = std::vector<T>;

template <class T1, class T2>
using pair = std::pair<T1, T2>;

template <typename T>
bool contains(vec<T> v, T e) {
    return std::find(v.begin(), v.end(), e) != v.end();
}

template <class T>
struct ArrayMap {
    vec<T> array;
    std::unordered_map<str, u32> map;
    u32 add(T element) {
        array.push_back(element);
        u32 index = u32(array.size()) - 1;
        map[element.m_name] = index;
        return index;
    }

    auto begin() {
        return array.begin();
    }

    auto end() {
        return array.end();
    }

    auto begin() const {
        return array.begin();
    }

    auto end() const {
        return array.end();
    }

    T& operator[](u32 index) {
        return array[index];
    }

    const T& operator[](u32 index) const {
        return array[index];
    }

    T& operator[](const str& name) {
        return array[map[name]];
    }

    const T& operator[](const str& name) const {
        return array[map[name]];
    }

    bool contains(const str& name) const {
        return map.find(name) != map.end();
    }

    auto size() {
        return array.size();
    }
};

}  // namespace blaz