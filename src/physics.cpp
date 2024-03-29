#include "physics.h"

namespace blaz {

void Physics::add_rigidbody(Rigidbody rigidbody) {
    m_rigidbodies.push_back(rigidbody);
    m_rigidbodies_ids[rigidbody.m_name] = u32(m_rigidbodies.size()) - 1;
}

void Physics::init() {
}

void Physics::update(f32 delta_time) {
    for (u32 i = 0; i < m_rigidbodies.size(); i++) {
        Rigidbody& rigidbody = m_rigidbodies[i];

        Node* node = &m_current_scene->m_nodes[rigidbody.m_node];

        Vec3 gravity = Vec3(0, -10, 0);
        rigidbody.m_acceleration = gravity;
        rigidbody.m_velocity += rigidbody.m_acceleration * delta_time;
        rigidbody.m_position += rigidbody.m_velocity * delta_time;

        if (rigidbody.m_position.y() < -3.0){
            rigidbody.m_position =  Vec3(0.0, 2.0f, 0.0);           
            rigidbody.m_velocity = Vec3(0.0, 0.0, 0.0);
        }
        node->set_position(rigidbody.m_position);
    }
}

}  // namespace blaz