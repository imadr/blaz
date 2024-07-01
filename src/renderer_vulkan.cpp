#include <windows.h>

#include "renderer.h"
#include "vulkan_loader/vulkan_loader.h"

namespace blaz {

#define VULKAN_FUNCTION(name) PFN_##name name;
VULKAN_FUNCTIONS_LIST
#undef VULKAN_FUNCTION

Error Renderer::init_api() {
    VulkanLoader vulkan_loader;

    Error err = vulkan_loader.init();
    if (err) {
        return err;
    }

#define VULKAN_FUNCTION(name) name = (PFN_##name)vulkan_loader.load_function(#name);
    VULKAN_FUNCTIONS_LIST
#undef VULKAN_FUNCTION

    VkInstance instance;

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "blaz";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "blaz";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;

    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
    std::cout << "available extensions:\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }

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