#include "camera.h"

#include <algorithm>

#include "node.h"
#include "renderer.h"

namespace blaz {

void Camera::update_projection_matrix() {
    if (!m_dirty_projection_matrix) return;
    m_dirty_projection_matrix = false;

    if (m_projection == Projection::PERSPECTIVE) {
        m_projection_matrix = perspective_projection(m_fov, m_aspect_ratio, m_z_near, m_z_far);

    } else if (m_projection == Projection::ORTHOGRAPHIC) {
        f32 left = m_left;
        f32 right = m_right;
        f32 bottom = m_bottom;
        f32 top = m_top;
        if (m_keep_screen_aspect_ratio) {
            f32 width = m_right - m_left;
            f32 height = width / m_aspect_ratio;
            left = -width / 2;
            right = width / 2;
            top = height / 2;
            bottom = -height / 2;
        }
        m_projection_matrix = orthographic_projection(left, right, bottom, top, m_z_near, m_z_far);
    }
}

void Camera::update_view_matrix() {
    if (m_scene->m_nodes[m_node].m_was_dirty) {
        m_scene->m_nodes[m_node].m_was_dirty = false;
    } else {
        return;
    }
    m_view_matrix = m_scene->m_nodes[m_node].m_global_matrix;
    m_view_matrix.m[12] *= -1;
    m_view_matrix.m[13] *= -1;
    m_view_matrix.m[14] *= -1;
}

void Camera::set_aspect_ratio(f32 aspect_ratio) {
    m_aspect_ratio = aspect_ratio;
    m_dirty_projection_matrix = true;
}

void Camera::set_ortho(f32 left, f32 right, f32 bottom, f32 top) {
    m_left = left;
    m_right = right;
    m_bottom = bottom;
    m_top = top;
    m_projection = Projection::ORTHOGRAPHIC;
    m_dirty_projection_matrix = true;
}

void Camera::set_clipping_planes(f32 z_near, f32 z_far) {
    m_z_near = z_near;
    m_z_far = z_far;
    m_dirty_projection_matrix = true;
}

void Camera::update_orbit_camera() {
    Node* camera_node = &m_scene->m_nodes[m_node];

    Vec3 pos =
        Vec3(m_orbit_zoom * cos(m_orbit_spherical_angles[1]) * cos(m_orbit_spherical_angles[0]),
             m_orbit_zoom * sin(m_orbit_spherical_angles[1]),
             m_orbit_zoom * cos(m_orbit_spherical_angles[1]) * sin(m_orbit_spherical_angles[0]));

    camera_node->set_position(pos);

    Vec3 forward = (camera_node->m_position - m_orbit_target).normalize();
    Vec3 right = vec3_cross(Vec3(0, 1, 0), forward).normalize();
    Vec3 up = vec3_cross(forward, right).normalize();
    Mat4 mat = Mat4(right.x(), up.x(), forward.x(), 0.0f, right.y(), up.y(), forward.y(), 0.0f,
                    right.z(), up.z(), forward.z(), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    Quat q = Quat::from_rotation_matrix(mat);
    camera_node->set_rotation(q);
}

void Camera::orbit_mouse_wheel(i8 delta) {
    m_orbit_zoom -= f32(delta) / 200.0f;
    m_orbit_zoom = std::max(0.1f, m_orbit_zoom);

    update_orbit_camera();
}

void Camera::orbit_mouse_move(Vec2I delta) {
    if (m_camera_mode != CameraMode::ORBIT) return;
    if (!m_mouse_pressed) return;

    m_orbit_spherical_angles += Vec2(f32(delta.x()) / 150.0f, f32(delta.y()) / 150.0f);
    m_orbit_spherical_angles[1] =
        std::clamp(m_orbit_spherical_angles[1], f32(-PI_HALF) + 0.1f, f32(PI_HALF) - 0.1f);

    update_orbit_camera();
}

}  // namespace blaz
