#version 330 core
layout(location = 0) in vec3 a_position;

uniform mat4 u_model_mat;
uniform mat4 u_view_mat;
uniform mat4 u_projection_mat;

out vec4 v_position;

void main() {
    v_world_position = vec3(u_model_mat * vec4(a_position, 1.0));
    v_position = u_projection_mat * u_view_mat * vec4(v_world_position, 1.0);
    gl_Position = v_position;
}