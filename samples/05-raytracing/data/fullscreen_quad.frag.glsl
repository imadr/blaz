#version 450

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec4 o_color;

layout(binding = 0) uniform sampler2D u_sampler_texture;

void main() {
    o_color = texture2D(u_sampler_texture, v_texcoord);
}