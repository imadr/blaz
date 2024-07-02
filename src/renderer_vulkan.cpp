#include <windows.h>

#include "renderer.h"
#include "vulkan_loader/vulkan_loader.h"

namespace blaz {

#define VULKAN_FUNCTION(name) PFN_##name name;
VULKAN_FUNCTIONS_LIST
#undef VULKAN_FUNCTION

#define VULKAN_FUNCTION(name) PFN_##name name;
VULKAN_INSTANCE_FUNCTIONS_LIST
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

    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    vec<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
    // std::cout << "available extensions:\n";
    for (const auto& extension : extensions) {
        // std::cout << '\t' << extension.extensionName << '\n';
    }

    bool enable_validation_layers = false;
#ifdef DEBUG_RENDERER
    enable_validation_layers = true;
    const vec<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
#endif

    if (enable_validation_layers) {
        uint32_t validation_layer_count;
        vkEnumerateInstanceLayerProperties(&validation_layer_count, nullptr);

        vec<VkLayerProperties> available_validation_layers(validation_layer_count);
        vkEnumerateInstanceLayerProperties(&validation_layer_count,
                                           available_validation_layers.data());
        // std::cout << "available layers:\n";
        for (const auto& layer : available_validation_layers) {
            // std::cout << '\t' << layer.layerName << '\n';
        }

        for (const char* layer_name : validation_layers) {
            bool found = false;
            for (const auto& layerProperties : available_validation_layers) {
                if (strcmp(layer_name, layerProperties.layerName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                enable_validation_layers = false;
                logger.error(Error(str(layer_name) + " not found"));
            }
        }
    }
    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    if (enable_validation_layers) {
        instance_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        instance_create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        instance_create_info.enabledLayerCount = 0;
    }

    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);

#define VULKAN_FUNCTION(name) \
    name = (PFN_##name)vulkan_loader.load_instance_function(instance, #name);
    VULKAN_INSTANCE_FUNCTIONS_LIST
#undef VULKAN_FUNCTION

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if (device_count == 0) {
        return Error("Failed to find GPUs with Vulkan support");
    }

    vec<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    str device_name;
    for (auto& device : devices) {
        VkPhysicalDeviceProperties device_propreties;
        vkGetPhysicalDeviceProperties(device, &device_propreties);
        device_name = device_propreties.deviceName;
        break;  // @note taking only first device now, should expose list of devices
    }

    logger.info(device_name + ", Vulkan "); /*+ std::to_string(major) + "." + std::to_string(minor)
                + " " + gl_profile);*/

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