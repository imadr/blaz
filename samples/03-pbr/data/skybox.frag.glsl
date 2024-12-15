#version 450

layout(location = 0) out vec4 o_color;

layout(location = 0) in vec3 v_world_position;

layout(binding = 0) uniform sampler2D u_sampler_envmap;

void main() {
    vec3 world_pos = normalize(v_world_position);
    vec2 uv = vec2(atan(world_pos.z, world_pos.x), asin(world_pos.y));
    uv *= vec2(0.1591, 0.3183);
    uv += 0.5;

    uv.y *= -1.0;

    vec3 envmap = texture(u_sampler_envmap, uv).rgb;
    envmap = pow(envmap, vec3(1.0 / 2.2));
    o_color = vec4(envmap, 1);
}