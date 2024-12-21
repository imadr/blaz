#version 450

layout(location = 0) in vec3 v_world_position;
layout(location = 1) in vec3 v_world_normal;
layout(location = 2) in vec3 v_world_tangent;
layout(location = 3) in vec2 v_texcoord;
layout(location = 4) in vec4 v_light_space_position;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 1) uniform u_view {
    vec3 u_camera_position;
};

layout(std140, binding = 2) uniform u_light {
    vec3 u_light_position;
    mat4 u_light_view_mat;
    mat4 u_light_projection_mat;
};

layout(binding = 3) uniform sampler2D u_sampler_shadowmap;

const float PI = 3.14159265359;

float calculate_shadow_pcf(vec3 projected_light_coords, vec3 normal, vec3 light_direction,
                           sampler2D shadow_map) {
    vec2 texel_size = 1.0 / textureSize(u_sampler_shadowmap, 0);
    float slope = abs(dot(normalize(normal), light_direction));
    float bias = 0.00001 * slope + 0.00001;
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

vec3 fresnel_schlick(float view_dot_halfway, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - view_dot_halfway, 0.0, 1.0), 5.0);
}

float normal_distribution_ggx(vec3 normal, vec3 halfway_vector, float roughness) {
    float a = roughness * roughness;
    float a_2 = a * a;
    float normal_dot_halfway = max(dot(normal, halfway_vector), 0.0);
    float normal_dot_halfway_2 = normal_dot_halfway * normal_dot_halfway;

    float num = a_2;
    float denom = (normal_dot_halfway_2 * (a_2 - 1.0) + 1.0);

    return a_2 / (PI * denom * denom);
}

float geometry_schlick_ggx(float normal_dot_view, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return normal_dot_view / (normal_dot_view * (1.0 - k) + k);
}

float geometry_smith(vec3 normal, vec3 view, vec3 light, float roughness) {
    float normal_dot_view = max(dot(normal, view), 0.0);
    float normal_dot_light = max(dot(normal, light), 0.0);
    float ggx2 = geometry_schlick_ggx(normal_dot_view, roughness);
    float ggx1 = geometry_schlick_ggx(normal_dot_light, roughness);

    return ggx1 * ggx2;
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
        shadow = calculate_shadow_pcf(projected_light_coords, v_world_normal, light_direction,
                                      u_sampler_shadowmap);
    }

    vec3 light_color = vec3(10);

    float metalness = 0.0;

    float roughness = 1.0;

    vec3 emissive = vec3(0.0);

    float ambient_occlusion = 1.0;

    vec3 albedo = vec3(1.0, 0.0, 0.0);

    vec3 normal = normalize(v_world_normal);
    vec3 tangent = normalize(v_world_tangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);
    mat3 tbn_matrix = mat3(tangent, bitangent, normal);

    vec3 normal_map = v_world_normal;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metalness);

    vec3 view_vector = normalize(u_camera_position - v_world_position);
    vec3 halfway_vector = normalize(light_direction + view_vector);

    float diffuse = max(dot(normal_map, light_direction), 0.0);

    float dist = distance(u_light_position, v_world_position);
    float attenuation = 1.0 / (dist * dist);
    attenuation = 1.0;
    vec3 radiance = light_color * attenuation;

    radiance *= (1.0 - shadow);
    radiance += 0.3;

    float normal_distribution = normal_distribution_ggx(normal_map, halfway_vector, roughness);
    float geometry = geometry_smith(normal_map, view_vector, light_direction, roughness);
    vec3 fresnel = fresnel_schlick(max(dot(halfway_vector, view_vector), 0.0), F0);

    vec3 numerator = normal_distribution * geometry * fresnel;
    float denominator =
        4.0 * max(dot(normal_map, view_vector), 0.0) * max(dot(normal_map, light_direction), 0.0) +
        0.0001;
    vec3 specular = numerator / denominator;

    vec3 specular_contribution = fresnel;
    vec3 diffuse_contribution = vec3(1.0) - specular_contribution;
    diffuse_contribution *= 1.0 - metalness;

    vec3 outgoing_radiance = (diffuse_contribution * albedo / PI + specular) * radiance + emissive;

    vec3 color = outgoing_radiance / (outgoing_radiance + vec3(1.0));
    o_color = vec4(vec3(color), 1);
}