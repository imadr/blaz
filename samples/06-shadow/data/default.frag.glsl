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

void main() {
    // vec3 light_direction = normalize(rotate(vec3(0.0, 1.0, 0.0), u_light_rotation));
    // vec3 shadowmap = texture(u_sampler_shadowmap, v_position.xy).rgb;
    // color = shadowmap / 10.0;

    // vec3 light_space_position = v_light_space_position.xyz;
    // vec3 col = vec3(distance(v_world_position, u_light_position.xyz) / 10.0);
    // col = light_space_position.zzz;
    // vec3 col = projected_coords;
    // vec4 color = u_light_space_mat * vec4(v_normal + u_light_position, 1.0);
    // float closest_depth = texture(u_sampler_shadowmap, projected_coords.xy).x;
    // o_color = vec4(texture(u_sampler_shadowmap, projected_coords.xy).xyz, 1);
    // o_color = vec4(vec3(pow(closest_depth, 100.0) / 10.0), 1);

    vec3 projected_coords = v_light_space_position.xyz / v_light_space_position.w;
    projected_coords = projected_coords * 0.5 + 0.5;
    float shadowmap_sample = texture(u_sampler_shadowmap, projected_coords.xy).r;
    vec3 light_direction = normalize(u_light_position - v_world_position);
    vec3 color = vec3(dot(v_normal, light_direction));
    if (shadowmap_sample < projected_coords.z - 0.00001) {
        color = vec3(0);
    }
    o_color = vec4(color, 1.0);
    // o_color = vec4(vec3(shadow), 1.0);
    // o_color =
    // vec4(vec3(pow(, 200.0f) / 3.0f), 1.0);
}