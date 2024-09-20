#pragma once

#include "types.h"
#include "vulkan_loader/vulkan_header.h"

namespace blaz {

#define VULKAN_FUNCTIONS_LIST                           \
    VULKAN_FUNCTION(vkCreateInstance)                   \
    VULKAN_FUNCTION(vkEnumerateInstanceLayerProperties) \
    VULKAN_FUNCTION(vkEnumerateInstanceExtensionProperties)

#define VULKAN_INSTANCE_FUNCTIONS_LIST          \
    VULKAN_FUNCTION(vkEnumeratePhysicalDevices) \
    VULKAN_FUNCTION(vkGetPhysicalDeviceProperties)

struct VulkanLoader {
    Error init();
    void* load_function(str name);
    void* load_instance_function(VkInstance instance, str name);

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
};

}  // namespace blaz