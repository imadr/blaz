#version 450

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec4 o_color;

layout(binding = 0) uniform sampler2D u_sampler_main;

void main() {
    vec3 color = texture(u_sampler_main, v_texcoord).rgb;

    // color = pow(color, vec3(1.0 / 2.2));

    o_color = vec4(color, 1.0);
}