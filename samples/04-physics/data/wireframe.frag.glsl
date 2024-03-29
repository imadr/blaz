#version 450

layout(std140, binding = 0) uniform u_wireframe {
    vec3 u_color;
};

layout(location = 0) out vec4 o_color;

void main() {
    o_color = vec4(u_color, 1);
}