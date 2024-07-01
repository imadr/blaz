#pragma once

#include "error.h"
#include "vulkan_loader/vulkan_header.h"

namespace blaz {

struct VulkanLoader {
    Error init();
};

}  // namespace blaz