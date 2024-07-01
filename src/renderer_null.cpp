#include "renderer.h"

namespace blaz {

Error Renderer::init_api() {
    return Error();
}

void Renderer::clear(u32 clear_flag, RGBA clear_color, float clear_depth) {
}

void Renderer::present() {
}

void Renderer::set_swap_interval(u32 interval) {
}

Error Renderer::compile_shader(Shader* shader) {
    return Error();
}

Error Renderer::upload_mesh(Mesh* mesh) {
    return Error();
}

Error Renderer::init_uniform_buffer(UniformBuffer* uniform_buffer) {
    return Error();
}

Error Renderer::set_uniform_buffer_data(UniformBuffer* uniform_buffer, str uniform_name,
                                        UniformValue value) {
    return Error();
}

void Renderer::set_shader(Shader* shader) {
}

Error Renderer::upload_texture(Texture* texture) {
    return Error();
}

void Renderer::set_textures(Pass* pass, Shader* shader) {
}

void Renderer::debug_marker_start(str name) {
}

void Renderer::debug_marker_end() {
}

void Renderer::set_default_framebuffer() {
}

void Renderer::set_viewport(u32 x, u32 y, u32 width, u32 height) {
}

void Renderer::set_framebuffer(Framebuffer* framebuffer) {
}

void Renderer::set_depth_test(bool enabled) {
}

void Renderer::set_face_culling(bool enabled, CullingMode mode, CullingOrder order) {
}

void Renderer::draw_mesh(Mesh* mesh) {
}

void Renderer::draw_bufferless(u32 count) {
}

}  // namespace blaz