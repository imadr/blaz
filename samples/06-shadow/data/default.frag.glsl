#version 430 core

precision highp float;

in vec4 v_position;
in vec4 v_light_space_position;
in vec3 v_normal;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 1) uniform u_light {
    vec3 u_light_position;
    mat4 u_light_view_mat;
    mat4 u_light_projection_mat;
};

layout(binding = 2) uniform sampler2D u_sampler_shadowmap;

vec3 rotate(vec3 v, vec4 q) {
    return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main() {
    // vec3 light_direction = normalize(rotate(vec3(0.0, 1.0, 0.0), u_light_rotation));
    // vec3 shadowmap = texture(u_sampler_shadowmap, v_position.xy).rgb;
    // vec3 color = dot(light_direction, v_normal.xyz).xxx;
    // color = shadowmap / 10.0;

    vec3 light_space_position = v_light_space_position.xyz / v_light_space_position.w;

    // vec4 color = u_light_space_mat * vec4(v_normal + u_light_position, 1.0);
    o_color = vec4(light_space_position, 1);
}