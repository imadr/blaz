#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_world_position;
layout(location = 2) in vec3 v_world_normal;
layout(location = 3) in vec2 v_uv;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 1) uniform u_view {
    vec3 u_camera_position;
};

layout(binding = 2) uniform sampler2D u_texture_albedo;

const float PI = 3.14159265359;

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
    vec3 light_position = vec3(1, 2, 2);
    vec3 light_color = vec3(10, 10, 10);

    float roughness = 0.3;
    float metallic = 0.9;
    float ambient_occlusion = 1.0;
    vec3 albedo = vec3(1.0, 0.0, 0.0);
    albedo = texture(u_texture_albedo, v_uv).rgb;
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 light_direction = normalize(light_position - v_world_position);
    vec3 view_vector = normalize(u_camera_position - v_world_position);
    vec3 halfway_vector = normalize(light_direction + view_vector);

    float diffuse = max(dot(v_world_normal, light_direction), 0.0);

    float dist = distance(light_position, v_world_position);
    float attenuation = 1.0 / (dist * dist);

    vec3 radiance = light_color * attenuation;

    float normal_distribution = normal_distribution_ggx(v_world_normal, halfway_vector, roughness);
    float geometry = geometry_smith(v_world_normal, view_vector, light_direction, roughness);
    vec3 fresnel = fresnel_schlick(max(dot(halfway_vector, view_vector), 0.0), F0);

    vec3 numerator = normal_distribution * geometry * fresnel;
    float denominator = 4.0 * max(dot(v_world_normal, view_vector), 0.0) *
                            max(dot(v_world_normal, light_direction), 0.0) +
                        0.0001;
    vec3 specular = numerator / denominator;

    vec3 specular_contribution = fresnel;
    vec3 diffuse_contribution = vec3(1.0) - specular_contribution;
    diffuse_contribution *= 1.0 - metallic;

    vec3 outgoing_radiance = (diffuse_contribution * albedo / PI + specular) * radiance * diffuse;

    vec3 ambient = vec3(0.03) * albedo * ambient_occlusion;

    vec3 color = ambient + outgoing_radiance / (outgoing_radiance + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    o_color = vec4(vec3(color), 1);
}