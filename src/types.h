#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

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