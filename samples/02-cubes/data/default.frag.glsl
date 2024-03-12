#version 450

layout(location = 0) in vec4 v_position;
layout(location = 1) in vec3 v_normal;

layout(location = 0) out vec4 o_color;

void main() {
    o_color = vec4(abs(v_normal.xyz), 1);
}