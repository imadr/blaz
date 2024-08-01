#version 430 core

precision highp float;

in vec3 v_normal;

layout(location = 0) out vec4 o_color;

void main() {
    o_color = vec4(abs(v_normal.xyz), 1);
}