#include "camera.h"

#include <algorithm>

#include "renderer.h"

namespace blaz {

// Camera* Camera::main = NULL;

// void Camera::init(str name, str node, MeshManager* mesh_manager,
//                   RenderableManager* renderable_manager, Scene* scene) {
//     m_name = name;
//     m_mesh_manager = mesh_manager;
//     m_renderable_manager = renderable_manager;
//     m_scene = scene;
//     m_node = node;
//     update_frustum_mesh();
// }

// void Camera::update_frustum_mesh() {
//     str mesh_name = m_name + "_frustum";

//     vec<f32> vertices = {
//         m_left,    m_bottom, -m_z_near, m_left,    m_top,    -m_z_near, m_right,  m_top,
//         -m_z_near, m_right,  m_bottom,  -m_z_near, m_left,   m_bottom,  -m_z_far, m_left,
//         m_top,     -m_z_far, m_right,   m_top,     -m_z_far, m_right,   m_bottom, -m_z_far,
//     };

//     vec<u32> indices = {
//         0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7,
//     };

//     if (m_mesh_manager->m_meshes.count(mesh_name)) {
//         m_mesh_manager->update_mesh_buffers(mesh_name, vertices, indices);
//     } else {
//         Mesh frustum_mesh = {.m_name = mesh_name,
//                              .m_vertices = vertices,
//                              .m_indices = indices,
//                              .m_attribs = {{"position_attrib", 3}},
//                              .m_primitive = MeshPrimitive::LINES};
//         m_mesh_manager->add_mesh(std::move(frustum_mesh));
//         m_scene->add_node(m_node + "_frustum", m_node);
//         m_renderable_manager->add_renderable({
//             .m_name = mesh_name,
//             .m_tags = {"gizmo"},
//             .m_mesh = mesh_name,
//             .m_node = m_node + "_frustum",
//         });
//         m_frustum_renderable = mesh_name;
//     }
// }

// void Camera::update_projection_matrix() {
//     if (m_projection == Projection::PERSPECTIVE) {
//         m_projection_matrix = perspective_projection(m_fov, m_aspect_ratio, m_z_near, m_z_far);

//     } else if (m_projection == Projection::ORTHOGRAPHIC) {
//         m_projection_matrix =
//             orthographic_projection(m_left, m_right, m_bottom, m_top, m_z_near, m_z_far);
//     }
// }

// void Camera::set_aspect_ratio(f32 aspect_ratio) {
//     m_aspect_ratio = aspect_ratio;
//     update_projection_matrix();
// }

// void Camera::set_ortho(f32 left, f32 right, f32 bottom, f32 top) {
//     m_left = left;
//     m_right = right;
//     m_bottom = bottom;
//     m_top = top;
//     m_projection = Projection::ORTHOGRAPHIC;
//     update_projection_matrix();
// }

// void Camera::set_clipping_planes(f32 z_near, f32 z_far) {
//     m_z_near = z_near;
//     m_z_far = z_far;
//     update_projection_matrix();
// }

}  // namespace blaz
