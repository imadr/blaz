#version 430 core

layout(location = 0) in vec3 a_position;

layout(std140, binding = 0) uniform u_mat {
    mat4 u_model_mat;
    mat4 u_view_mat;
    mat4 u_projection_mat;
};

void main() {
    vec3 world_position = vec3(u_model_mat * vec4(a_position, 1.0));
    gl_Position = u_projection_mat * u_view_mat * vec4(world_position, 1.0);
}