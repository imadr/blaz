#pragma once

#include "types.h"
#include "my_math.h"

namespace blaz {

#undef RGB

struct RGB {
    f32 values[3];

    f32 &operator[](int i) {
        return values[i];
    }
    f32 &r() {
        return values[0];
    }
    f32 &g() {
        return values[1];
    }
    f32 &b() {
        return values[2];
    }

    RGB() : values{0, 0, 0} {
    }

    RGB(f32 r, f32 g, f32 b) : values{r, g, b} {
    }

    RGB(Vec3 v) : values{v.x(), v.y(), v.z()} {
    }

    f32 operator[](u32 i) {
        return values[i];
    }

    RGB operator=(RGB other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        values[2] = other.values[2];
        return *this;
    }

    RGB(const RGB &other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        values[2] = other.values[2];
    }
};

struct RGBA {
    f32 values[4];

    f32 &operator[](int i) {
        return values[i];
    }
    f32 &r() {
        return values[0];
    }
    f32 &g() {
        return values[1];
    }
    f32 &b() {
        return values[2];
    }
    f32 &a() {
        return values[3];
    }

    RGBA() : values{0, 0, 0, 0} {
    }

    RGBA(f32 r, f32 g, f32 b, f32 a) : values{r, g, b, a} {
    }

    RGBA(Vec4 v) : values{v.x(), v.y(), v.z(), v.w()} {
    }

    f32 operator[](u32 i) {
        return values[i];
    }

    RGBA operator=(RGBA other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        values[2] = other.values[2];
        values[3] = other.values[3];
        return *this;
    }

    RGBA(const RGBA &other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        values[2] = other.values[2];
        values[3] = other.values[3];
    }
};

struct HSV {
    f32 values[3];

    f32 &operator[](int i) {
        return values[i];
    }
    f32 &h() {
        return values[0];
    }
    f32 &s() {
        return values[1];
    }
    f32 &v() {
        return values[2];
    }

    HSV() : values{0, 0, 0} {
    }
    HSV(f32 h, f32 s, f32 v) : values{h, s, v} {
    }

    f32 operator[](u32 i) {
        return values[i];
    }

    HSV operator=(HSV other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        values[2] = other.values[2];
        return *this;
    }

    HSV(const HSV &other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        values[2] = other.values[2];
    }
};

HSV rgb_to_hsv(RGB rgb);
RGB hsv_to_rgb(HSV hsv);

}  // namespace blaz