#version 450

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec4 o_color;

layout(binding = 0) uniform sampler2D u_sampler_tex;

#define BLUR_SIZE 2

void main() {
    vec2 texel_size = 1.0 / vec2(textureSize(u_sampler_tex, 0));
    float sum = 0.0;
    for (int x = -BLUR_SIZE; x < BLUR_SIZE; ++x) {
        for (int y = -BLUR_SIZE; y < BLUR_SIZE; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            sum += texture2D(u_sampler_tex, v_texcoord + offset).r;
        }
    }
    o_color = vec4(vec3(sum / (BLUR_SIZE * 2.0 * BLUR_SIZE * 2.0)), 1.0);
}