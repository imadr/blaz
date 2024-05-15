#version 300 es

layout(location = 0) out vec2 v_texcoord;

void main() {
    vec3 position = vec3(vec2(gl_VertexID % 2, gl_VertexID / 2) * 4.0 - 1.0, 0.0);
    v_texcoord = (position.xy + 1.0) * 0.5;
    gl_Position = vec4(position, 1.0);
}