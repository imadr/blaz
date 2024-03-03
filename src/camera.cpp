#include "camera.h"

#include <algorithm>

#include "renderer.h"

namespace blaz {

void Camera::update_projection_matrix() {
    if (m_projection == Projection::PERSPECTIVE) {
        m_projection_matrix = perspective_projection(m_fov, m_aspect_ratio, m_z_near, m_z_far);

    } else if (m_projection == Projection::ORTHOGRAPHIC) {
        m_projection_matrix =
            orthographic_projection(m_left, m_right, m_bottom, m_top, m_z_near, m_z_far);
    }
}

void Camera::set_aspect_ratio(f32 aspect_ratio) {
    m_aspect_ratio = aspect_ratio;
    update_projection_matrix();
}

void Camera::set_ortho(f32 left, f32 right, f32 bottom, f32 top) {
    m_left = left;
    m_right = right;
    m_bottom = bottom;
    m_top = top;
    m_projection = Projection::ORTHOGRAPHIC;
    update_projection_matrix();
}

void Camera::set_clipping_planes(f32 z_near, f32 z_far) {
    m_z_near = z_near;
    m_z_far = z_far;
    update_projection_matrix();
}

}  // namespace blaz
