#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec2 a_texcoord;

layout(std140, binding = 0) uniform u_mat {
    mat4 u_model_mat;
    mat4 u_view_mat;
    mat4 u_projection_mat;
};

layout(location = 0) out vec3 v_world_position;
void main() {
    vec3 world_position = vec3(u_model_mat * vec4(a_position, 1.0));
    v_world_position = world_position;
    gl_Position = u_projection_mat * mat4(mat3(u_view_mat)) * vec4(world_position, 1.0);
}