#pragma once

#include <unordered_map>

#include "node.h"
#include "types.h"

namespace blaz {

struct Rigidbody {
    str m_name;
    u32 m_node;

    Vec3 m_position;
    Vec3 m_velocity = Vec3(0, 0, 0);
    Vec3 m_acceleration = Vec3(0, 0, 0);

    // replace with inverse_mass later
    f32 m_mass = 1.0;
};

struct Physics {
    Scene* m_current_scene = NULL;
    vec<Rigidbody> m_rigidbodies;
    std::unordered_map<str, u32> m_rigidbodies_ids;

    void add_rigidbody(Rigidbody rigidbody);

    void init();
    void update(f32 delta_time);
};

}  // namespace blaz