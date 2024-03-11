#pragma once

#include <unordered_map>

#include "camera.h"
#include "my_math.h"
#include "types.h"

namespace blaz {

struct Scene;

struct Node {
    str m_name;
    Scene* m_scene = NULL;
    bool is_root_node = false;
    u32 m_parent = 0;
    vec<u32> m_children;

    Vec3 m_position = Vec3(0, 0, 0);
    Quat m_rotation = Quat(0, 0, 0, 1);
    Vec3 m_scale = Vec3(1, 1, 1);

    Mat4 m_global_matrix = Mat4();
    Mat4 m_local_matrix = Mat4();

    void update_matrix();
    void set_local_position(Vec3 position);
    void set_local_rotation(Quat rotation);
    void set_local_scale(Vec3 scale);
    Vec3 get_global_position();
    void translate(Vec3 translation);
    void rotate(Quat rotation);
    void scale(Vec3 scaling);
    Vec3 right();
    Vec3 up();
    Vec3 forward();
};

struct Scene {
    vec<Node> m_nodes;
    std::unordered_map<str, u32> m_nodes_ids;
    void init_scene();
    void add_node(Node node, str parent);
};

}  // namespace blaz