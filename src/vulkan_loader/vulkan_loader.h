#pragma once

#include "error.h"
#include "types.h"
#include "vulkan_loader/vulkan_header.h"

namespace blaz {

#define VULKAN_FUNCTIONS_LIST         \
    VULKAN_FUNCTION(vkCreateInstance) \
    VULKAN_FUNCTION(vkEnumerateInstanceExtensionProperties)

struct VulkanLoader {
    Error init();
    void* load_function(str name);
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
};

}  // namespace blaz