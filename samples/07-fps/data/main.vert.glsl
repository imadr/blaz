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

layout(std140, binding = 2) uniform u_light {
    vec3 u_light_position;
    mat4 u_light_view_mat;
    mat4 u_light_projection_mat;
};

layout(location = 0) out vec3 v_world_position;
layout(location = 1) out vec3 v_world_normal;
layout(location = 2) out vec3 v_world_tangent;
layout(location = 3) out vec2 v_texcoord;
layout(location = 4) out vec4 v_light_space_position;

void main() {
    vec3 world_position = vec3(u_model_mat * vec4(a_position, 1.0));
    v_world_position = world_position;
    gl_Position = u_projection_mat * u_view_mat * vec4(world_position, 1.0);
    v_light_space_position = u_light_projection_mat * u_light_view_mat * vec4(world_position, 1.0);
    mat3 inv_model = mat3(transpose(inverse(u_model_mat)));
    v_world_normal = inv_model * a_normal;
    v_world_tangent = inv_model * a_tangent;
    v_texcoord = a_texcoord;
}