#version 430 core

precision highp float;

in vec4 v_position;
in vec3 v_normal;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 1) uniform u_light {
    vec3 u_light_position;
    vec4 u_light_rotation;
};

vec3 rotate(vec3 v, vec4 q) {
    return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main() {
    vec3 light_direction = normalize(rotate(vec3(0.0, 1.0, 0.0), u_light_rotation));

    vec3 color = dot(light_direction, v_normal.xyz).xxx;

    o_color = vec4(color, 1);
}