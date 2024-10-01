#version 430 core

layout(location = 0) in vec3 a_position;

layout(std140, binding = 0) uniform u_mat {
    mat4 u_model_mat;
    mat4 u_view_mat;
    mat4 u_projection_mat;
};

layout(location = 0) out vec3 v_view_position;

void main() {
    vec4 world_position = u_model_mat * vec4(a_position, 1.0);
    vec4 view_position = u_view_mat * world_position;
    vec4 clip_position = u_projection_mat * view_position;

    gl_Position = clip_position;

    v_view_position = view_position.xyz;
}