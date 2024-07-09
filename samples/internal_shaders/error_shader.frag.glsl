#version 430 core

precision highp float;

in vec4 v_position;

layout(location = 0) out vec4 o_color;

void main() {
    o_color = vec4(1, 0, 1, 0);
}