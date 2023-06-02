#pragma once

#include <vulkan/vulkan.hpp>

namespace NEngine {
namespace VulkanUtils {
VkShaderModule CreateShaderModule(VkDevice device,
                                  const std::vector<char> &code);
}
}  // namespace NEngine