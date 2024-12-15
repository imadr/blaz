#version 430 core

precision highp float;

in vec4 v_position;

layout(location = 0) out vec4 o_color;

void main() {
    float depth = v_position.z / v_position.w;
    depth = depth * 0.5 + 0.5;

    float moment1 = depth;
    float moment2 = depth * depth;

    float dx = dFdx(depth);
    float dy = dFdy(depth);
    moment2 += 0.25 * (dx * dx + dy * dy);

    o_color = vec4(moment1, moment2, 0.0, 0.0);
}