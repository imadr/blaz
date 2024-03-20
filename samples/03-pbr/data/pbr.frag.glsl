#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_world_position;
layout(location = 2) in vec3 v_world_normal;
layout(location = 3) in vec2 v_uv;

layout(location = 0) out vec4 o_color;

void main() {
    vec3 light_direction = normalize(vec3(2, 4, 1) - v_world_position);
    o_color = vec4(vec3(max(dot(v_world_normal, light_direction), 0.0) + 0.4), 1);
}