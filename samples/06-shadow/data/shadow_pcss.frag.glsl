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

float calculate_shadow(vec4 light_space_position, vec3 normal, vec3 light_direction,
                   sampler2D shadow_map) {


    vec3 projected_light_coords = v_light_space_position.xyz / v_light_space_position.w;
    if (projected_light_coords.x < -1.0 || projected_light_coords.x > 1.0 ||
        projected_light_coords.y < -1.0 || projected_light_coords.y > 1.0 ||
        projected_light_coords.z < 0.0 || projected_light_coords.z > 1.0) {
        return 0.0;
    }

    projected_light_coords = projected_light_coords * 0.5 + 0.5;

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
    shadow /= (samples+samples+1)*(samples+samples+1);

    return shadow;
}

void main() {
    vec3 light_direction = normalize(u_light_position - v_world_position);

    float shadow = calculate_shadow(v_light_space_position, v_normal, light_direction, u_sampler_shadowmap);

    vec3 color = vec3(dot(v_normal, light_direction));
    o_color = vec4(color * (1.0 - shadow), 1.0);
}