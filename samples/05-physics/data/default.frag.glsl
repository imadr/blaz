#version 450

layout(location = 0) in vec3 v_world_position;
layout(location = 1) in vec3 v_world_normal;

layout(location = 0) out vec4 o_color;

void main() {
    vec3 light_position = vec3(1, 2, 2);
    vec3 light_direction = normalize(light_position - v_world_position);
    float diffuse = max(dot(v_world_normal, light_direction), 0.0);
    o_color = vec4(vec3(diffuse), 1);
}