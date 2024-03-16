#include "my_math.h"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

namespace blaz {

Vec2I operator+(Vec2I a, Vec2I b) {
    return Vec2I(a.x() + b.x(), a.y() + b.y());
}

Vec2I operator-(Vec2I a, Vec2I b) {
    return Vec2I(a.x() - b.x(), a.y() - b.y());
}

Vec3 operator+(Vec3 a, Vec3 b) {
    return Vec3(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}

Vec3 operator-(Vec3 a, Vec3 b) {
    return Vec3(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}

Vec3 operator*(f32 s, Vec3 v) {
    return Vec3(v.x() * s, v.y() * s, v.z() * s);
}

Vec3 operator*(Vec3 a, Vec3 b) {
    return Vec3(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
}

Vec3 operator*(Vec3 v, f32 s) {
    return s * v;
}

Vec3 operator/(Vec3 v, f32 s) {
    return (1 / s) * v;
}

f32 vec3_dot(Vec3 a, Vec3 b) {
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return Vec3(a.y() * b.z() - a.z() * b.y(), a.z() * b.x() - a.x() * b.z(),
                a.x() * b.y() - a.y() * b.x());
}

Vec3 vec3_lerp(Vec3 a, Vec3 b, f32 t) {
    t = t < 0 ? 0 : t;
    t = t > 1 ? 1 : t;
    return b * t + a * (1 - t);
}

Vec3 Vec3::normalize() {
    f32 l = length();
    if (l == 0) return Vec3(0, 0, 0);
    return Vec3(x() / l, y() / l, z() / l);
}

Vec4 operator+(Vec4 a, Vec4 b) {
    return Vec4(a.x() + b.x(), a.y() + b.y(), a.z() + b.z(), a.w() + b.w());
}

Vec4 operator-(Vec4 a, Vec4 b) {
    return Vec4(a.x() - b.x(), a.y() - b.y(), a.z() - b.z(), a.w() - b.w());
}

Vec4 operator*(f32 s, Vec4 v) {
    return Vec4(v.x() * s, v.y() * s, v.z() * s, v.w() * s);
}

Vec4 operator*(Vec4 a, Vec4 b) {
    return Vec4(a.x() * b.x(), a.y() * b.y(), a.z() * b.z(), a.w() * b.w());
}

Vec4 operator*(Vec4 v, f32 s) {
    return s * v;
}

Vec4 operator/(Vec4 v, f32 s) {
    return (1 / s) * v;
}

f32 vec4_dot(Vec4 a, Vec4 b) {
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z() + a.w() * b.w();
}

Vec4 vec4_lerp(Vec4 a, Vec4 b, f32 t) {
    t = t < 0 ? 0 : t;
    t = t > 1 ? 1 : t;
    return b * t + a * (1 - t);
}

Vec4 Vec4::normalize() {
    f32 l = length();
    if (l == 0) return Vec4(0, 0, 0, 0);
    return Vec4(x() / l, y() / l, z() / l, w() / l);
}

Vec2 operator+(Vec2 a, Vec2 b) {
    return Vec2(a.x() + b.x(), a.y() + b.y());
}

Vec2 operator-(Vec2 a, Vec2 b) {
    return Vec2(a.x() - b.x(), a.y() - b.y());
}

Vec2 operator*(f32 s, Vec2 v) {
    return Vec2(v.x() * s, v.y() * s);
}

Vec2 operator*(Vec2 v, f32 s) {
    return s * v;
}

Vec2 operator/(Vec2 v, f32 s) {
    return (1 / s) * v;
}

Vec2 vec2_lerp(Vec2 a, Vec2 b, f32 t) {
    t = t < 0 ? 0 : t;
    t = t > 1 ? 1 : t;
    return b * t + a * (1 - t);
}

Vec2 Vec2::normalize() {
    f32 l = length();
    if (l == 0) return Vec2(0, 0);
    return Vec2(x() / l, y() / l);
}

Mat4 perspective_projection(f32 fov, f32 aspect_ratio, f32 z_near, f32 z_far) {
    f32 f = tan(fov / 2);
    return Mat4(1 / (f * aspect_ratio), 0, 0, 0, 0, 1 / f, 0, 0, 0, 0, z_far / (z_near - z_far), -1,
                0, 0, -(z_far * z_near) / (z_far - z_near), 0);
}

Mat4 orthographic_projection(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far) {
    return Mat4(2 / (right - left), 0, 0, 0, 0, 2 / (top - bottom), 0, 0, 0, 0,
                -2 / (z_far - z_near), 0, -(right + left) / (right - left),
                -(top + bottom) / (top - bottom), -(z_far + z_near) / (z_far - z_near), 1);
}

Mat4 translate_3d(Vec3 t) {
    return Mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, t.x(), t.y(), t.z(), 1);
}

Mat4 scale_3d(Vec3 s) {
    return Mat4(s.x(), 0, 0, 0, 0, s.y(), 0, 0, 0, 0, s.z(), 0, 0, 0, 0, 1);
}

Mat4 rotate_3d(Quat q) {
    f32 xx = q.x() * q.x();
    f32 yy = q.y() * q.y();
    f32 zz = q.z() * q.z();
    return Mat4(1 - 2 * yy - 2 * zz, 2 * q.x() * q.y() + 2 * q.z() * q.w(),
                2 * q.x() * q.z() - 2 * q.y() * q.w(), 0, 2 * q.x() * q.y() - 2 * q.z() * q.w(),
                1 - 2 * xx - 2 * zz, 2 * q.y() * q.z() + 2 * q.x() * q.w(), 0,
                2 * q.x() * q.z() + 2 * q.y() * q.w(), 2 * q.y() * q.z() - 2 * q.x() * q.w(),
                1 - 2 * xx - 2 * yy, 0, 0, 0, 0, 1);
}

Quat operator*(Quat a, Quat b) {
    return Quat(a.w() * b.x() + a.x() * b.w() + a.y() * b.z() - a.z() * b.y(),
                a.w() * b.y() - a.x() * b.z() + a.y() * b.w() + a.z() * b.x(),
                a.w() * b.z() + a.x() * b.y() - a.y() * b.x() + a.z() * b.w(),
                a.w() * b.w() - a.x() * b.x() - a.y() * b.y() - a.z() * b.z());
}

Quat Quat::from_euler(Vec3 euler) {
    f32 cx = cos(euler.v[0] / 2);
    f32 sx = sin(euler.v[0] / 2);
    f32 cy = cos(euler.v[1] / 2);
    f32 sy = sin(euler.v[1] / 2);
    f32 cz = cos(euler.v[2] / 2);
    f32 sz = sin(euler.v[2] / 2);

    return Quat(sx * cy * cz - cx * sy * sz, cx * sy * cz + sx * cy * sz,
                cx * cy * sz - sx * sy * cz, cx * cy * cz + sx * sy * sz);
}

Quat Quat::from_axis_angle(Vec3 axis, f32 angle) {
    axis = axis.normalize();
    f32 s = sin(angle / 2);
    return Quat(axis.x() * s, axis.y() * s, axis.z() * s, cos(angle / 2));
}

Mat4 operator*(f32 s, Mat4 m) {
    return Mat4(s * m[0], s * m[1], s * m[2], s * m[3], s * m[4], s * m[5], s * m[6], s * m[7],
                s * m[8], s * m[9], s * m[10], s * m[11], s * m[12], s * m[13], s * m[14],
                s * m[15]);
}

Mat4 operator*(Mat4 m, f32 s) {
    return s * m;
}

f32 Mat4::determinant() {
    return m[3] * m[6] * m[9] * m[12] - m[2] * m[7] * m[9] * m[12] - m[3] * m[5] * m[10] * m[12] +
           m[1] * m[7] * m[10] * m[12] + m[2] * m[5] * m[11] * m[12] - m[1] * m[6] * m[11] * m[12] -
           m[3] * m[6] * m[8] * m[13] + m[2] * m[7] * m[8] * m[13] + m[3] * m[4] * m[10] * m[13] -
           m[0] * m[7] * m[10] * m[13] - m[2] * m[4] * m[11] * m[13] + m[0] * m[6] * m[11] * m[13] +
           m[3] * m[5] * m[8] * m[14] - m[1] * m[7] * m[8] * m[14] - m[3] * m[4] * m[9] * m[14] +
           m[0] * m[7] * m[9] * m[14] + m[1] * m[4] * m[11] * m[14] - m[0] * m[5] * m[11] * m[14] -
           m[2] * m[5] * m[8] * m[15] + m[1] * m[6] * m[8] * m[15] + m[2] * m[4] * m[9] * m[15] -
           m[0] * m[6] * m[9] * m[15] - m[1] * m[4] * m[10] * m[15] + m[0] * m[5] * m[10] * m[15];
}

Mat4 Mat4::invert() {
    f32 det = determinant();
    Mat4 m_ = Mat4(m[6] * m[11] * m[13] - m[7] * m[10] * m[13] + m[7] * m[9] * m[14] -
                       m[5] * m[11] * m[14] - m[6] * m[9] * m[15] + m[5] * m[10] * m[15],
                   m[3] * m[10] * m[13] - m[2] * m[11] * m[13] - m[3] * m[9] * m[14] +
                       m[1] * m[11] * m[14] + m[2] * m[9] * m[15] - m[1] * m[10] * m[15],
                   m[2] * m[7] * m[13] - m[3] * m[6] * m[13] + m[3] * m[5] * m[14] -
                       m[1] * m[7] * m[14] - m[2] * m[5] * m[15] + m[1] * m[6] * m[15],
                   m[3] * m[6] * m[9] - m[2] * m[7] * m[9] - m[3] * m[5] * m[10] +
                       m[1] * m[7] * m[10] + m[2] * m[5] * m[11] - m[1] * m[6] * m[11],
                   m[7] * m[10] * m[12] - m[6] * m[11] * m[12] - m[7] * m[8] * m[14] +
                       m[4] * m[11] * m[14] + m[6] * m[8] * m[15] - m[4] * m[10] * m[15],
                   m[2] * m[11] * m[12] - m[3] * m[10] * m[12] + m[3] * m[8] * m[14] -
                       m[0] * m[11] * m[14] - m[2] * m[8] * m[15] + m[0] * m[10] * m[15],
                   m[3] * m[6] * m[12] - m[2] * m[7] * m[12] - m[3] * m[4] * m[14] +
                       m[0] * m[7] * m[14] + m[2] * m[4] * m[15] - m[0] * m[6] * m[15],
                   m[2] * m[7] * m[8] - m[3] * m[6] * m[8] + m[3] * m[4] * m[10] -
                       m[0] * m[7] * m[10] - m[2] * m[4] * m[11] + m[0] * m[6] * m[11],
                   m[5] * m[11] * m[12] - m[7] * m[9] * m[12] + m[7] * m[8] * m[13] -
                       m[4] * m[11] * m[13] - m[5] * m[8] * m[15] + m[4] * m[9] * m[15],
                   m[3] * m[9] * m[12] - m[1] * m[11] * m[12] - m[3] * m[8] * m[13] +
                       m[0] * m[11] * m[13] + m[1] * m[8] * m[15] - m[0] * m[9] * m[15],
                   m[1] * m[7] * m[12] - m[3] * m[5] * m[12] + m[3] * m[4] * m[13] -
                       m[0] * m[7] * m[13] - m[1] * m[4] * m[15] + m[0] * m[5] * m[15],
                   m[3] * m[5] * m[8] - m[1] * m[7] * m[8] - m[3] * m[4] * m[9] +
                       m[0] * m[7] * m[9] + m[1] * m[4] * m[11] - m[0] * m[5] * m[11],
                   m[6] * m[9] * m[12] - m[5] * m[10] * m[12] - m[6] * m[8] * m[13] +
                       m[4] * m[10] * m[13] + m[5] * m[8] * m[14] - m[4] * m[9] * m[14],
                   m[1] * m[10] * m[12] - m[2] * m[9] * m[12] + m[2] * m[8] * m[13] -
                       m[0] * m[10] * m[13] - m[1] * m[8] * m[14] + m[0] * m[9] * m[14],
                   m[2] * m[5] * m[12] - m[1] * m[6] * m[12] - m[2] * m[4] * m[13] +
                       m[0] * m[6] * m[13] + m[1] * m[4] * m[14] - m[0] * m[5] * m[14],
                   m[1] * m[6] * m[8] - m[2] * m[5] * m[8] + m[2] * m[4] * m[9] -
                       m[0] * m[6] * m[9] - m[1] * m[4] * m[10] + m[0] * m[5] * m[10]);
    return m_ * (1 / det);
}

Mat4 Mat4::transpose() {
    return Mat4(m[0], m[4], m[8], m[12], m[1], m[5], m[9], m[13], m[2], m[6], m[10], m[14], m[3],
                m[7], m[11], m[15]);
}

Mat4 operator*(Mat4 a, Mat4 b) {
    return Mat4(a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12],
                a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13],
                a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14],
                a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15],

                a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12],
                a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13],
                a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14],
                a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15],

                a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12],
                a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13],
                a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14],
                a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15],

                a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12],
                a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13],
                a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14],
                a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15]);
}

f32 rad(f32 deg) {
    return deg * f32(PI) / 180.0f;
}

str Mat4::to_str() {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(4) << m[0] << " ";
    stream << std::fixed << std::setprecision(4) << m[1] << " ";
    stream << std::fixed << std::setprecision(4) << m[2] << " ";
    stream << std::fixed << std::setprecision(4) << m[3] << std::endl;
    stream << std::fixed << std::setprecision(4) << m[4] << " ";
    stream << std::fixed << std::setprecision(4) << m[5] << " ";
    stream << std::fixed << std::setprecision(4) << m[6] << " ";
    stream << std::fixed << std::setprecision(4) << m[7] << std::endl;
    stream << std::fixed << std::setprecision(4) << m[8] << " ";
    stream << std::fixed << std::setprecision(4) << m[9] << " ";
    stream << std::fixed << std::setprecision(4) << m[10] << " ";
    stream << std::fixed << std::setprecision(4) << m[11] << std::endl;
    stream << std::fixed << std::setprecision(4) << m[12] << " ";
    stream << std::fixed << std::setprecision(4) << m[13] << " ";
    stream << std::fixed << std::setprecision(4) << m[14] << " ";
    stream << std::fixed << std::setprecision(4) << m[15];
    str s = stream.str();
    return s;
}

str Vec3::to_str() {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(4) << v[0] << " ";
    stream << std::fixed << std::setprecision(4) << v[1] << " ";
    stream << std::fixed << std::setprecision(4) << v[2];
    str s = stream.str();
    return s;
}

str Vec4::to_str() {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(4) << v[0] << " ";
    stream << std::fixed << std::setprecision(4) << v[1] << " ";
    stream << std::fixed << std::setprecision(4) << v[2] << " ";
    stream << std::fixed << std::setprecision(4) << v[3];
    str s = stream.str();
    return s;
}

str Quat::to_str() {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(4) << v[0] << " ";
    stream << std::fixed << std::setprecision(4) << v[1] << " ";
    stream << std::fixed << std::setprecision(4) << v[2] << " ";
    stream << std::fixed << std::setprecision(4) << v[3];
    str s = stream.str();
    return s;
}

Vec3 rotate_vector(Vec3 v, Quat q) {
    Quat v_q = Quat(v.x(), v.y(), v.z(), 0);
    Quat c = q.conjugate();
    Quat res = (q * v_q) * c;
    return Vec3(res.x(), res.y(), res.z());
}

f32 Vec3::length() {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

f32 Vec4::length() {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

f32 Vec2::length() {
    return sqrt(v[0] * v[0] + v[1] * v[1]);
}

f32 Quat::length() {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

Quat Quat::conjugate() {
    return Quat(-v[0], -v[1], -v[2], v[3]);
}

Quat Quat::normalize() {
    float m = length();
    if (m == 0) return Quat(0, 0, 0, 0);
    return Quat(v[0] / m, v[1] / m, v[2] / m, v[3] / m);
}

}  // namespace blaz