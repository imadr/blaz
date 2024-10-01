#version 430 core

precision highp float;

layout(location = 0) in vec3 v_view_position;

layout(location = 0) out vec3 o_color;

void main() {
    o_color = v_view_position;
}