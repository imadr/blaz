#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D u_image_render;
layout(rgba32f, binding = 1) uniform image2D u_image_old_render;

void main() {
    ivec2 texel_coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 resolution = imageSize(u_image_render);
    vec2 uv = texel_coord / resolution.xy - 0.5;
    uv.x *= resolution.x / resolution.y;

    bool inside_circle = (distance(uv, vec2(0.0)) > 0.3 ? 0.0 : 1.0) == 1.0;

    vec3 color = vec3(0.0);

    vec3 old_render = imageLoad(u_image_old_render, ivec2(texel_coord / resolution.xy)).xyz;

    if (inside_circle) {
        color = vec3(0.1);
        color += old_render + 0.01;
    }

    imageStore(u_image_render, texel_coord, vec4(color, 1.0));
}