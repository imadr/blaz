#version 330 core

in vec2 v_uv;

out vec4 o_color;

void main() {
    o_color = vec4(v_uv.xy, 0, 1);
}