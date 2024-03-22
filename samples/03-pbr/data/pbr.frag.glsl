#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_world_position;
layout(location = 2) in vec3 v_world_normal;
layout(location = 3) in vec2 v_uv;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 1) uniform u_view {
    vec3 u_camera_position;
};

const float PI = 3.14159265359;

////////////////////////
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
////////////////////////

void main() {
    vec3 light_position = vec3(1, 2, 2);
    vec3 light_color = vec3(10, 10, 10);

    float roughness = 0.3;
    float metallic = 0.9;
    float ambient_occlusion = 1.0;
    vec3 albedo = vec3(1.0, 0.0, 0.0);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 light_direction = normalize(light_position - v_world_position);
    vec3 view_vector = normalize(u_camera_position - v_world_position);
    vec3 halfway_vector = normalize(light_direction + view_vector);

    float diffuse = max(dot(v_world_normal, light_direction), 0.0);

    float dist = distance(light_position, v_world_position);
    float attenuation = 1.0 / (dist * dist);

    vec3 radiance = light_color * attenuation;

    ////////////////////////
    float NDF = DistributionGGX(v_world_normal, halfway_vector, roughness);
    float G = GeometrySmith(v_world_normal, view_vector, light_direction, roughness);
    vec3 F = fresnelSchlick(max(dot(halfway_vector, view_vector), 0.0), F0);
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(v_world_normal, view_vector), 0.0) *
                            max(dot(v_world_normal, light_direction), 0.0) +
                        0.0001;
    vec3 specular = numerator / denominator;
    ////////////////////////

    vec3 ambient = vec3(0.03) * albedo * ambient_occlusion;
    vec3 Lo = (kD * albedo / PI + specular) * radiance * diffuse;

    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    o_color = vec4(vec3(color), 1);
}