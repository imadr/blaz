#version 330 core

in vec4 v_position;

out vec4 o_color;

void main() {
    o_color = vec4(a_position.rgb, 1);
}