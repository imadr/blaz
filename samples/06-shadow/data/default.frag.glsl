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

float calculate_shadow_pcf(vec3 projected_light_coords, vec3 normal, vec3 light_direction,
                           sampler2D shadow_map) {
    vec2 texel_size = 1.0 / textureSize(u_sampler_shadowmap, 0);
    float slope = abs(dot(normalize(v_normal), light_direction));
    float bias = 0.00005 * slope + 0.00002;
    float shadow = 0.0;

    int samples = 3;
    for (int x = -samples; x <= samples; x++) {
        for (int y = -samples; y <= samples; y++) {
            float shadowmap_sample =
                texture(u_sampler_shadowmap, projected_light_coords.xy + vec2(x, y) * texel_size).r;

            if (projected_light_coords.z > shadowmap_sample + bias) {
                shadow += 1.0;
            }
        }
    }
    shadow /= (samples + samples + 1) * (samples + samples + 1);

    return shadow;
}

float calculate_shadow_pcss(vec3 projected_light_coords) {
    vec2 texel_size = 1.0 / textureSize(u_sampler_shadowmap, 0);

    float bias = 0.00001;
    float kernel_scale = 10.0;
    int sample_size = 1;

    float average_blockers_depth = 0.0;
    float num_blockers = 0.0;
    for (int i = -sample_size; i <= sample_size; i++) {
        for (int j = -sample_size; j <= sample_size; j++) {
            vec2 offset = vec2(i, j) * texel_size * kernel_scale;
            float shadowmap_sample =
                texture(u_sampler_shadowmap, projected_light_coords.xy + offset).r;
            if (shadowmap_sample < projected_light_coords.z - bias) {
                average_blockers_depth += shadowmap_sample;
                num_blockers += 1.0;
            }
        }
    }

    average_blockers_depth /= num_blockers;
    return average_blockers_depth;
}

void main() {
    vec3 light_direction = normalize(u_light_position - v_world_position);

    float shadow = 0.0;

    vec3 projected_light_coords = v_light_space_position.xyz / v_light_space_position.w;
    projected_light_coords = projected_light_coords * 0.5 + 0.5;

    bool out_of_bounds = projected_light_coords.x < 0.0 || projected_light_coords.x > 1.0 ||
                         projected_light_coords.y < 0.0 || projected_light_coords.y > 1.0 ||
                         projected_light_coords.z < 0.0 || projected_light_coords.z > 1.0;

    if (!out_of_bounds) {
        shadow = calculate_shadow_pcf(projected_light_coords, v_normal, light_direction,
                                      u_sampler_shadowmap);
    }

    vec3 color = vec3(dot(v_normal, light_direction));
    o_color = vec4(color * (1.0 - shadow), 1.0);

    // o_color = vec4(vec3(calculate_shadow_pcss(projected_light_coords)), 1.0);
    // o_color = vec4(vec3(pow(texture(u_sampler_shadowmap, projected_light_coords.xy).r, 30.0)), 1.0);
    // o_color = vec4(vec3(pow(projected_light_coords.z, 30.0)), 1.0);
}