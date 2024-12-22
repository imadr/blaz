#include "node.h"

#include "logger.h"

namespace blaz {

void init_scene(Scene* scene) {
    Node root_node = Node{.m_name = "root_node"};
    root_node.m_scene = scene;
    root_node.is_root_node = true;
    scene->m_nodes.add(root_node);
}

void add_node(Scene* scene, Node node, str parent) {
    node.m_scene = scene;
    if (scene->m_nodes.contains(parent)) {
        node.m_parent = parent;
        scene->m_nodes.add(node);
        scene->m_nodes[parent].m_children.push_back(node.m_name);
        node.update_matrix();
    } else {
        logger.error("Parent node \"" + parent + "\" not found");
    }
}

void Node::update_matrix() {
    m_was_dirty = true;
    m_local_matrix = translate_3d(m_position) * rotate_3d(m_rotation) * scale_3d(m_scale);
    if (!is_root_node) {
        m_global_matrix = m_local_matrix * m_scene->m_nodes[m_parent].m_global_matrix;
    }
    for (str child : m_children) {
        m_scene->m_nodes[child].update_matrix();
    }
}

void Node::set_position(Vec3 position) {
    m_position = position;
    update_matrix();
}

void Node::set_rotation(Quat rotation) {
    m_rotation = rotation;
    update_matrix();
}

void Node::set_scale(Vec3 scale) {
    m_scale = scale;
    update_matrix();
}

Vec3 Node::get_global_position() {
    return Vec3(m_global_matrix[12], m_global_matrix[13], m_global_matrix[14]);
}

void Node::translate(Vec3 translation) {
    m_position += translation;
    update_matrix();
}

void Node::rotate(Quat rotation) {
    m_rotation = rotation * m_rotation;
    update_matrix();
}

void Node::scale(Vec3 scaling) {
    m_scale += scaling;
    update_matrix();
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