#pragma once

#include "my_math.h"
#include "types.h"

namespace blaz {

enum class CameraMode { ORBIT, FLY, FPS };

struct CameraOrbit {
    f32 zoom = 3.0f;
    Vec3 pivot = Vec3(0, 0, 0);
    f32 mouse_sensitivity = 0.003f;
    f32 yaw = 0;
    f32 pitch = 0;
};

struct CameraFly {
    f32 mouse_sensitivity = 0.001f;
    f32 move_speed = 0.01f;
    f32 yaw = 0;
    f32 pitch = 0;
};

enum class Projection { PERSPECTIVE, ORTHOGRAPHIC };

struct Camera {
    // static Camera* main;

    str m_name;
    u32 m_node;

    // Scene* m_scene = NULL;

    // f32 m_fov = rad(90);
    // f32 m_z_near = 0.1;
    // f32 m_z_far = 2000;
    // f32 m_aspect_ratio = 1;
    // f32 m_left = -1.0;
    // f32 m_right = 1.0;
    // f32 m_bottom = -1.0;
    // f32 m_top = 1.0;
    // str m_node;

    // Projection m_projection = Projection::PERSPECTIVE;

    // Mat4 m_projection_matrix;

    // CameraMode m_mode = CameraMode::FLY;
    // CameraOrbit m_camera_orbit;
    // CameraFly m_camera_fly;

    // str m_frustum_renderable;

    // void init(str name, str node, MeshManager* mesh_manager, RenderableManager*
    // renderable_manager,
    //           Scene* scene);
    // void update_frustum_mesh();
    // void update_projection_matrix();
    // void set_aspect_ratio(f32 aspect_ratio);
    // void set_ortho(f32 left, f32 right, f32 bottom, f32 top);
    // void set_clipping_planes(f32 z_near, f32 z_far);
};

}  // namespace blaz