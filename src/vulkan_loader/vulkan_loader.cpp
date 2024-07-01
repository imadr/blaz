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

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
        (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkan_library, "vkGetInstanceProcAddr");
    if (!vkGetInstanceProcAddr) {
        FreeLibrary(vulkan_library);
        return Error("Failed to get vkGetInstanceProcAddr");
    }
#endif

    PFN_vkCreateInstance vkCreateInstance =
        (PFN_vkCreateInstance)vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance");

    return Error();
}
};  // namespace blaz