#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
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

    T &operator[](u32 index) {
        return array[index];
    }

    const T &operator[](u32 index) const {
        return array[index];
    }

    T &operator[](const str &name) {
        return array[map.at(name)];
    }

    const T &operator[](const str &name) const {
        return array[map.at(name)];
    }

    bool contains(const str &name) const {
        return map.find(name) != map.end();
    }

    auto size() {
        return array.size();
    }
};

struct Error {
    bool m_is_error = false;
    str m_message;

    Error() : m_message(""), m_is_error(false) {
    }

    Error(const str &message) : m_message(message), m_is_error(true) {
    }

    str message() {
        return m_message;
    }

    operator bool() const {
        return m_is_error;
    }
};

template <typename T, u32 MaxCount>
struct Resource {
    T data[MaxCount];
    bool used[MaxCount] = {false};
    std::unordered_map<str, u32> map;
    u32 count = 0;

    std::function<Error(u32)> create_resource_api;
    std::function<Error(u32)> reload_resource_api;
    std::function<Error(u32)> destroy_resource_api;

    pair<Error, u32> create(const str &name) {
        for (u32 i = 0; i < MaxCount; i++) {
            if (!used[i]) {
                used[i] = true;
                map[name] = i;
                data[i].ref_count = 1;
                Error err;
                if (create_resource_api) {
                    err = create_resource_api(i);
                }
                count++;
                return std::make_pair(err, i);
            }
        }

        return std::make_pair(Error("No free slots for resource " + name), 0);
    }

    Error reload(u32 id) {
        if (id < MaxCount && used[id]) {
            Error err;
            if (reload_resource_api) {
                err = reload_resource_api(id);
            }
            return err;
        } else {
            return Error("Invalid resource id " + std::to_string(id) + " for reload");
        }
    }

    Error destroy(u32 id) {
        if (id < MaxCount && used[id]) {
            if (destroy_resource_api) {
                destroy_resource_api(id);
            }

            used[id] = false;
            count--;

            for (auto it = map.begin(); it != map.end();) {
                if (it->second == id) {
                    it = map.erase(it);
                } else {
                    ++it;
                }
            }
        } else {
            return Error("Invalid resource id " + std::to_string(id) + " for destruction");
        }
        return Error();
    }

    Error destroy(const str &name) {
        auto it = map.find(name);
        if (it != map.end()) {
            u32 id = it->second;
            if (id < MaxCount && used[id]) {
                if (destroy_resource_api) {
                    destroy_resource_api(id);
                }

                used[id] = false;
                count--;

                map.erase(it);
            } else {
                return Error("Invalid resource id " + str(id) + " for destruction");
            }
        } else {
            return Error("Invalid resource name " + name + " for destruction");
        }
        return Error();
    }

    T &operator[](u32 index) {
        return data[index];
    }

    const T &operator[](u32 index) const {
        return data[index];
    }

    T &operator[](const str &name) {
        return data[map.at(name)];
    }

    const T &operator[](const str &name) const {
        return data[map.at(name)];
    }

    bool contains(const str &name) const {
        return map.find(name) != map.end();
    }

    auto size() {
        return count;
    }
};

}  // namespace blaz