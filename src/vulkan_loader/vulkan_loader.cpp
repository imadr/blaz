#include "vulkan_loader.h"

#ifdef OS_WINDOWS
#include <windows.h>
#endif

namespace blaz {

Error VulkanLoader::init() {
#ifdef OS_WINDOWS
    HMODULE vulkan_library = LoadLibrary("vulkan-1.dll");
    if (!vulkan_library) {
        return Error("Failed to load vulkan-1.dll");
    }

    vkGetInstanceProcAddr =
        (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkan_library, "vkGetInstanceProcAddr");
    if (!vkGetInstanceProcAddr) {
        FreeLibrary(vulkan_library);
        return Error("Failed to get vkGetInstanceProcAddr");
    }
#endif

    return Error();
}

void* VulkanLoader::load_function(str name) {
    return vkGetInstanceProcAddr(VK_NULL_HANDLE, name.c_str());
}

void* VulkanLoader::load_instance_function(VkInstance instance, str name) {
    return vkGetInstanceProcAddr(instance, name.c_str());
}

};  // namespace blaz