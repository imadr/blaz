#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;

layout(std140, binding = 0) uniform u_mat {
    mat4 u_model_mat;
    mat4 u_view_mat;
    mat4 u_projection_mat;
};

layout(location = 0) out vec4 v_clip_position;
layout(location = 1) out vec3 v_world_position;
layout(location = 2) out vec3 v_world_normal;
layout(location = 3) out vec3 v_world_tangent;

void main() {
    vec4 world_position = u_model_mat * vec4(a_position, 1.0);
    vec4 view_position = u_view_mat * vec4(world_position.xyz, 1.0);
    vec4 clip_position = u_projection_mat * vec4(view_position.xyz, 1.0);

    gl_Position = clip_position;

    v_clip_position = clip_position;
    v_world_position = world_position.xyz;

    mat3 inv_model = mat3(transpose(inverse(u_model_mat)));
    v_world_normal = inv_model * a_normal;
    v_world_tangent = inv_model * a_tangent;
}