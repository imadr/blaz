#include "node.h"

#include "logger.h"

namespace blaz {

void Scene::init_scene() {
    Node root_node = Node{.m_name = "root_node"};
    root_node.m_scene = this;
    root_node.is_root_node = true;
    m_nodes.push_back(root_node);
    m_nodes_ids["root_node"] = 0;
}

void Scene::add_node(Node node, str parent) {
    node.m_scene = this;
    if (m_nodes_ids.contains(parent)) {
        node.update_matrix();
        u32 parent_id = m_nodes_ids[parent];
        node.m_parent = parent_id;
        m_nodes.push_back(node);
        u32 node_id = u32(m_nodes.size()) - 1;
        m_nodes_ids[node.m_name] = node_id;
        m_nodes[node.m_parent].m_children.push_back(node_id);

    } else {
        logger.error("Parent node \"%s\" not found", parent);
    }
}

void Node::update_matrix() {
    m_local_matrix = scale_3d(m_scale) * rotate_3d(m_rotation) * translate_3d(m_position);
    if (!is_root_node) {
        m_global_matrix = m_local_matrix * m_scene->m_nodes[m_parent].m_global_matrix;
    }
    for (u32 child : m_children) {
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