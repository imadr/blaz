#version 450

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec4 o_color;

layout(binding = 0) uniform sampler2D u_sampler_render;

layout(std140, binding = 1) uniform u_time {
    uint u_frame_number;
};

void main() {
    vec4 accumulated = texture2D(u_sampler_render, v_texcoord);
    o_color = vec4(vec3(accumulated.x/accumulated.w), 1.0);
}