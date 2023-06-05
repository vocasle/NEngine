#pragma once

#include <optional>
#include <vulkan/vulkan.hpp>

namespace NEngine {
namespace VulkanUtils {

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;
    std::optional<uint32_t> transfer_family;

    [[nodiscard]] bool
    IsComplete() const
    {
        return graphics_family.has_value() && present_family.has_value() &&
               transfer_family.has_value();
    }

    [[nodiscard]] VkSharingMode
    GetSharingMode() const
    {
        return graphics_family.value() == transfer_family.value()
                   ? VK_SHARING_MODE_EXCLUSIVE
                   : VK_SHARING_MODE_CONCURRENT;
    }
};

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR     surface);

VkShaderModule CreateShaderModule(VkDevice                 device,
                                  const std::vector<char> &code);

void EndSingleTimeCommands(VkCommandBuffer cb,
                           VkDevice        device,
                           VkCommandPool   pool,
                           VkQueue         queue);

VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool pool);
}  // namespace VulkanUtils
}  // namespace NEngine