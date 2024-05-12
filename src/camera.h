#pragma once

#include "logger.h"
#include "my_math.h"
#include "node.h"
#include "platform.h"
#include "types.h"

namespace blaz {

struct Scene;

enum class Projection { PERSPECTIVE, ORTHOGRAPHIC };

enum class CameraMode { ORBIT };

struct Camera {
    str m_name;
    Scene* m_scene = NULL;
    u32 m_node = NULL;

    f32 m_fov = rad(90);
    f32 m_z_near = 0.1f;
    f32 m_z_far = 2000;
    f32 m_aspect_ratio = 1;
    bool m_keep_screen_aspect_ratio = true;
    f32 m_left = -3.0;
    f32 m_right = 3.0;
    f32 m_bottom = -3.0;
    f32 m_top = 3.0;

    Projection m_projection = Projection::PERSPECTIVE;

    Mat4 m_projection_matrix = Mat4();
    bool m_dirty_projection_matrix = true;
    Mat4 m_view_matrix = Mat4();

    CameraMode m_camera_mode = CameraMode::ORBIT;
    Vec3 m_orbit_target = Vec3(0, 0, 0);
    f32 m_orbit_radius = 5.0f;
    Vec2 m_orbit_spherical_angles = Vec2(f32(PI_HALF), 0);
    bool m_mouse_pressed = false;
    void update_orbit_camera();
    void orbit_mouse_move(Vec2I delta);
    void orbit_mouse_wheel(i8 delta);

    void update_projection_matrix();
    void update_view_matrix();
    void set_aspect_ratio(f32 aspect_ratio);
    void set_ortho(f32 left, f32 right, f32 bottom, f32 top);
    void set_clipping_planes(f32 z_near, f32 z_far);
};

}  // namespace blaz