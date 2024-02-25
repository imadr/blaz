#version 330 core

in vec3 v_position;

out vec4 o_color;

void main() {
    o_color = vec4(v_position.xyz, 1);
}