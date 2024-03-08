#include "camera.h"

#include <algorithm>

#include "renderer.h"

namespace blaz {

void Camera::update_projection_matrix() {
    if(!m_dirty_projection_matrix) return;
    m_dirty_projection_matrix = false;

    if (m_projection == Projection::PERSPECTIVE) {
        m_projection_matrix = perspective_projection(m_fov, m_aspect_ratio, m_z_near, m_z_far);

    } else if (m_projection == Projection::ORTHOGRAPHIC) {
        m_projection_matrix =
            orthographic_projection(m_left, m_right, m_bottom, m_top, m_z_near, m_z_far);
    }
}

void Camera::update_view_matrix() {
    if(!m_dirty_view_matrix) return;
    m_dirty_view_matrix = false;
    m_view_matrix = (m_scene->m_nodes[m_node].m_global_matrix).invert();
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

}  // namespace blaz
