#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

layout(std140, binding = 0) uniform u_mat {
    mat4 u_model_mat;
    mat4 u_view_mat;
    mat4 u_projection_mat;
};

layout(location = 0) out vec4 v_position;
layout(location = 1) out vec3 v_normal;

void main() {
    vec3 world_position = vec3(u_model_mat * vec4(a_position, 1.0));
    v_position = u_projection_mat * u_view_mat * vec4(world_position, 1.0);
    v_normal = a_normal;
    gl_Position = v_position;
}