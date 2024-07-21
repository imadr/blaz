#pragma once

#include <cmath>

#include "types.h"

namespace blaz {

const f64 PI_HALF = 1.57079632679489661923132169163975144;
const f64 PI = 3.14159265358979323846264338327950288;
const f64 PI2 = 6.28318530717958647692528676655900576;

struct Vec2I {
    i32 v[2];
    i32 &operator[](int i) {
        return v[i];
    }

    void *ptr(Vec2I &v) {
        return v.v;
    }

    i32 &x() {
        return v[0];
    }
    i32 &y() {
        return v[1];
    }

    Vec2I() : v{0, 0} {
    }
    Vec2I(i32 x, i32 y) : v{x, y} {
    }

    Vec2I operator=(Vec2I other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
        return *this;
    }

    Vec2I(const Vec2I &other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
    }

    Vec2I operator-() {
        return Vec2I(-v[0], -v[1]);
    }

    Vec2I operator+=(Vec2I other) {
        v[0] += other.v[0];
        v[1] += other.v[1];
        return *this;
    }

    Vec2I operator-=(Vec2I other) {
        v[0] -= other.v[0];
        v[1] -= other.v[1];
        return *this;
    }

    str to_str();
};

Vec2I operator*(f32 s, Vec2I v);
Vec2I operator+(Vec2I a, Vec2I b);
Vec2I operator-(Vec2I a, Vec2I b);

struct Vec3I {
    i32 v[3];
    i32 &operator[](int i) {
        return v[i];
    }

    void *ptr(Vec3I &v) {
        return v.v;
    }

    i32 &x() {
        return v[0];
    }
    i32 &y() {
        return v[1];
    }
    i32 &z() {
        return v[2];
    }

    Vec3I() : v{0, 0, 0} {
    }
    Vec3I(i32 x, i32 y, i32 z) : v{x, y, z} {
    }

    Vec3I operator=(Vec3I other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
        return *this;
    }

    Vec3I(const Vec3I &other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
    }

    Vec3I operator-() {
        return Vec3I(-v[0], -v[1], -v[2]);
    }

    Vec3I operator+=(Vec3I other) {
        v[0] += other.v[0];
        v[1] += other.v[1];
        v[2] += other.v[2];
        return *this;
    }

    Vec3I operator-=(Vec3I other) {
        v[0] -= other.v[0];
        v[1] -= other.v[1];
        v[2] -= other.v[2];
        return *this;
    }

    str to_str();
};

Vec3I operator*(f32 s, Vec3I v);
Vec3I operator+(Vec3I a, Vec3I b);
Vec3I operator-(Vec3I a, Vec3I b);

struct Vec3 {
    f32 v[3];
    f32 &operator[](int i) {
        return v[i];
    }

    void *ptr(Vec3 &v) {
        return v.v;
    }

    f32 &x() {
        return v[0];
    }
    f32 &y() {
        return v[1];
    }
    f32 &z() {
        return v[2];
    }

    Vec3() : v{0, 0, 0} {
    }
    Vec3(f32 x, f32 y, f32 z) : v{x, y, z} {
    }

    Vec3 operator=(Vec3 other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
        return *this;
    }

    Vec3(const Vec3 &other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
    }

    Vec3 operator-() {
        return Vec3(-v[0], -v[1], -v[2]);
    }

    Vec3 operator+=(Vec3 other) {
        v[0] += other.v[0];
        v[1] += other.v[1];
        v[2] += other.v[2];
        return *this;
    }

    Vec3 operator-=(Vec3 other) {
        v[0] -= other.v[0];
        v[1] -= other.v[1];
        v[2] -= other.v[2];
        return *this;
    }

    f32 length();
    Vec3 normalize();
    str to_str();
};

Vec3 vec3_lerp(Vec3 a, Vec3 b, f32 t);
Vec3 vec3_cross(Vec3 a, Vec3 b);
f32 vec3_dot(Vec3 a, Vec3 b);
Vec3 operator/(Vec3 v, f32 s);
Vec3 operator*(Vec3 a, Vec3 b);
Vec3 operator*(Vec3 v, f32 s);
Vec3 operator*(f32 s, Vec3 v);
Vec3 operator-(Vec3 a, Vec3 b);
Vec3 operator+(Vec3 a, Vec3 b);

struct Quat;

struct Vec4 {
    f32 v[4];
    f32 &operator[](int i) {
        return v[i];
    }

    void *ptr(Vec4 &v) {
        return v.v;
    }

    f32 &x() {
        return v[0];
    }
    f32 &y() {
        return v[1];
    }
    f32 &z() {
        return v[2];
    }
    f32 &w() {
        return v[3];
    }

    Vec4() : v{0, 0, 0, 0} {
    }
    Vec4(f32 x, f32 y, f32 z, f32 w) : v{x, y, z, w} {
    }

    Vec4 operator=(Vec4 other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
        v[3] = other.v[3];
        return *this;
    }

    Vec4(const Vec4 &other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
        v[3] = other.v[3];
    }

    Vec4 operator-() {
        return Vec4(-v[0], -v[1], -v[2], -v[3]);
    }

    Vec4 operator+=(Vec4 other) {
        v[0] += other.v[0];
        v[1] += other.v[1];
        v[2] += other.v[2];
        v[3] += other.v[3];
        return *this;
    }

    Vec4 operator-=(Vec4 other) {
        v[0] -= other.v[0];
        v[1] -= other.v[1];
        v[2] -= other.v[2];
        v[3] -= other.v[3];
        return *this;
    }

    Vec4(Quat q);

    f32 length();
    Vec4 normalize();
    str to_str();
};

Vec4 vec4_lerp(Vec4 a, Vec4 b, f32 t);
Vec4 vec4_cross(Vec4 a, Vec4 b);
f32 vec4_dot(Vec4 a, Vec4 b);
Vec4 operator/(Vec4 v, f32 s);
Vec4 operator*(Vec4 a, Vec4 b);
Vec4 operator*(Vec4 v, f32 s);
Vec4 operator*(f32 s, Vec4 v);
Vec4 operator-(Vec4 a, Vec4 b);
Vec4 operator+(Vec4 a, Vec4 b);

struct Vec2 {
    f32 v[2];
    f32 &operator[](int i) {
        return v[i];
    }

    void *ptr(Vec2 &v) {
        return v.v;
    }

    f32 &x() {
        return v[0];
    }
    f32 &y() {
        return v[1];
    }

    Vec2() : v{0, 0} {
    }
    Vec2(f32 x, f32 y) : v{x, y} {
    }

    Vec2 operator=(Vec2 other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
        return *this;
    }

    Vec2(const Vec2 &other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
    }

    Vec2 operator-() {
        return Vec2(-v[0], -v[1]);
    }

    Vec2 operator+=(Vec2 other) {
        v[0] += other.v[0];
        v[1] += other.v[1];
        return *this;
    }

    Vec2 operator-=(Vec2 other) {
        v[0] += other.v[0];
        v[1] += other.v[1];
        return *this;
    }

    f32 length();
    Vec2 normalize();

    str to_str();
};

Vec2 vec2_lerp(Vec2 a, Vec2 b, f32 t);
Vec2 operator/(Vec2 v, f32 s);
Vec2 operator*(Vec2 v, f32 s);
Vec2 operator*(f32 s, Vec2 v);
Vec2 operator-(Vec2 a, Vec2 b);
Vec2 operator+(Vec2 a, Vec2 b);

f32 rad(f32 deg);

struct Mat4 {
    f32 m[16];
    const f32 &operator[](int i) const {
        return m[i];
    }

    void *ptr(Mat4 &mat) {
        return mat.m;
    }

    Mat4() : m{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1} {
    }

    Mat4(f32 m00, f32 m10, f32 m20, f32 m30, f32 m01, f32 m11, f32 m21, f32 m31, f32 m02, f32 m12,
         f32 m22, f32 m32, f32 m03, f32 m13, f32 m23, f32 m33)
        : m{m00, m10, m20, m30, m01, m11, m21, m31, m02, m12, m22, m32, m03, m13, m23, m33} {
    }

    Mat4 operator=(Mat4 other) {
        m[0] = other.m[0];
        m[1] = other.m[1];
        m[2] = other.m[2];
        m[3] = other.m[3];
        m[4] = other.m[4];
        m[5] = other.m[5];
        m[6] = other.m[6];
        m[7] = other.m[7];
        m[8] = other.m[8];
        m[9] = other.m[9];
        m[10] = other.m[10];
        m[11] = other.m[11];
        m[12] = other.m[12];
        m[13] = other.m[13];
        m[14] = other.m[14];
        m[15] = other.m[15];
        return *this;
    }

    Mat4(const Mat4 &other) {
        m[0] = other.m[0];
        m[1] = other.m[1];
        m[2] = other.m[2];
        m[3] = other.m[3];
        m[4] = other.m[4];
        m[5] = other.m[5];
        m[6] = other.m[6];
        m[7] = other.m[7];
        m[8] = other.m[8];
        m[9] = other.m[9];
        m[10] = other.m[10];
        m[11] = other.m[11];
        m[12] = other.m[12];
        m[13] = other.m[13];
        m[14] = other.m[14];
        m[15] = other.m[15];
    }

    f32 determinant();
    Mat4 invert();
    Mat4 transpose();

    str to_str();
};
Mat4 operator*(Mat4 a, Mat4 b);
Mat4 operator*(Mat4 v, f32 s);
Mat4 operator*(f32 s, Mat4 v);

struct Quat {
    f32 v[4];
    const f32 &operator[](int i) const {
        return v[i];
    }

    void *ptr(Quat &q) {
        return q.v;
    }

    f32 &x() {
        return v[0];
    }
    f32 &y() {
        return v[1];
    }
    f32 &z() {
        return v[2];
    }
    f32 &w() {
        return v[3];
    }

    Quat() : v{0, 0, 0, 1} {
    }
    Quat(f32 x, f32 y, f32 z, f32 w) : v{x, y, z, w} {
    }

    Quat(Vec4 v) : v{v.x(), v.y(), v.z(), v.w()} {
    }

    Quat operator=(Quat other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
        v[3] = other.v[3];
        return *this;
    }

    Quat(const Quat &other) {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
        v[3] = other.v[3];
    }

    Quat conjugate();
    f32 length();
    Quat normalize();

    str to_str();

    static Quat from_euler(Vec3 euler);
    static Quat from_axis_angle(Vec3 axis, f32 angle);
    static Quat from_rotation_matrix(const Mat4 &m);
};

Quat operator*(Quat a, Quat b);
Vec3 rotate_vector(Vec3 v, Quat q);

Mat4 perspective_projection(f32 fov, f32 aspect_ratio, f32 z_near, f32 z_far);
Mat4 orthographic_projection(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far);
Mat4 rotate_3d(Quat q);
Mat4 scale_3d(Vec3 s);
Mat4 translate_3d(Vec3 t);

}  // namespace blaz