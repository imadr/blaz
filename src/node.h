#pragma once

#include <unordered_map>

#include "my_math.h"
#include "types.h"

namespace blaz {

struct Scene;

struct Node {
    str m_name;
    Scene* m_scene = NULL;
    bool is_root_node = false;
    str m_parent;
    vec<str> m_children;

    Vec3 m_position = Vec3(0, 0, 0);
    Quat m_rotation = Quat(0, 0, 0, 1);
    Vec3 m_scale = Vec3(1, 1, 1);

    Mat4 m_global_matrix = Mat4();
    Mat4 m_local_matrix = Mat4();

    bool m_was_dirty = true;

    void update_matrix();
    void set_position(Vec3 position);
    void set_rotation(Quat rotation);
    void set_scale(Vec3 scale);
    Vec3 get_global_position();
    void translate(Vec3 translation);
    void rotate(Quat rotation);
    void scale(Vec3 scaling);
    Vec3 right();
    Vec3 up();
    Vec3 forward();
};

struct Scene {
    ArrayMap<Node> m_nodes;
};

void init_scene(Scene* scene);
void add_node(Scene* scene, Node node, str parent);

}  // namespace blaz