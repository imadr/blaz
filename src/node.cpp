#include "node.h"

namespace blaz {

void Node::update_global_matrix(Mat4 parent_global_matrix) {
    m_global_matrix = m_local_matrix * parent_global_matrix;
}

void Node::update_local_matrix() {
    m_local_matrix = scale_3d(m_scale) * rotate_3d(m_rotation) * translate_3d(m_position);
    if (m_parent > -1) {
        m_global_matrix = m_local_matrix * m_scene->m_nodes[m_parent].m_global_matrix;
    }
    for (u32 child : m_children) {
        m_scene->m_nodes[child].update_global_matrix(m_global_matrix);
    }
}

void Node::set_local_position(Vec3 position) {
    m_position = position;
    update_local_matrix();
}

void Node::set_local_rotation(Quat rotation) {
    m_rotation = rotation;
    update_local_matrix();
}

void Node::set_local_scale(Vec3 scale) {
    m_scale = scale;
    update_local_matrix();
}

Vec3 Node::get_global_position() {
    return Vec3(m_global_matrix[12], m_global_matrix[13], m_global_matrix[14]);
}

void Node::translate(Vec3 translation) {
    m_position += translation;
    update_local_matrix();
}

void Node::rotate(Quat rotation) {
    m_rotation = rotation * m_rotation;
    update_local_matrix();
}

void Node::scale(Vec3 scaling) {
    m_scale += scaling;
    update_local_matrix();
}

Vec3 Node::right() {
    return Vec3(m_global_matrix[0], m_global_matrix[1], m_global_matrix[2]);
}

Vec3 Node::up() {
    return Vec3(m_global_matrix[4], m_global_matrix[5], m_global_matrix[6]);
}

Vec3 Node::forward() {
    return Vec3(m_global_matrix[8], m_global_matrix[9], m_global_matrix[10]);
}

}  // namespace blaz