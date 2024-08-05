#version 430 core

precision highp float;

in vec3 v_world_position;
in vec4 v_light_space_position;
in vec3 v_normal;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 1) uniform u_light {
    vec3 u_light_position;
    mat4 u_light_view_mat;
    mat4 u_light_projection_mat;
};

layout(binding = 2) uniform sampler2D u_sampler_shadowmap;

#define SHADOW_BIAS 0.00001

void main() {
    vec3 projected_light_coords = v_light_space_position.xyz / v_light_space_position.w;
    projected_light_coords = projected_light_coords * 0.5 + 0.5;
    float shadowmap_sample = texture(u_sampler_shadowmap, projected_light_coords.xy).r;
    vec3 light_direction = normalize(u_light_position - v_world_position);
    vec3 color = vec3(dot(v_normal, light_direction));
    if (shadowmap_sample < projected_light_coords.z - SHADOW_BIAS) {
        color = vec3(0);
    }
    o_color = vec4(color, 1.0);
}