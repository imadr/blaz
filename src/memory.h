#pragma once

#include "types.h"

namespace blaz {

void* alloc(size_t size);
void dealloc(void* ptr);
void memcopy(void* dst, const void* src, size_t count);

template <typename T>
T read_value(u8* ptr, bool flip_endianness = false) {
    T value;
    memcpy(&value, ptr, sizeof(T));

    if (flip_endianness) {
        if constexpr (sizeof(T) == 2) {
            return (value >> 8) | (value << 8);
        } else if constexpr (sizeof(T) == 4) {
            return (value >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) |
                   (value << 24);
        } else if constexpr (sizeof(T) == 8) {
            return (value >> 56) | ((value & 0x00FF000000000000) >> 40) |
                   ((value & 0x0000FF0000000000) >> 24) | ((value & 0x000000FF00000000) >> 8) |
                   ((value & 0x00000000FF000000) << 8) | ((value & 0x0000000000FF0000) << 24) |
                   ((value & 0x000000000000FF00) << 40) | (value << 56);
        }
    }

    return value;
}

}  // namespace blaz