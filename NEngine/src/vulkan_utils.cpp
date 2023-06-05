#include "vulkan_utils.h"

#include "misc.h"

namespace NEngine::VulkanUtils {
VkShaderModule
CreateShaderModule(VkDevice device, const std::vector<char> &code)
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shader_module;
    VKRESULT(
        vkCreateShaderModule(device, &create_info, nullptr, &shader_module));

    return shader_module;
}

QueueFamilyIndices
FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queue_family_count, queue_families.data());

    int i = 0;
    for (const auto &queue_family : queue_families) {
        if (indices.IsComplete()) {
            break;
        }

        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
            // On some devices there might be no transfer queue
            indices.transfer_family = i;
        }
        else if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.transfer_family = i;
        }

        if (surface) {
            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                device, i, surface, &present_support);
            if (present_support) {
                indices.present_family = i;
            }
        }
        ++i;
    }

    return indices;
}

VkCommandBuffer
BeginSingleTimeCommands(VkDevice device, VkCommandPool pool)
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer cb;
    VKRESULT(vkAllocateCommandBuffers(device, &alloc_info, &cb));

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VKRESULT(vkBeginCommandBuffer(cb, &begin_info));

    return cb;
}

void
EndSingleTimeCommands(VkCommandBuffer cb,
                      VkDevice        device,
                      VkCommandPool   pool,
                      VkQueue         queue)
{
    VKRESULT(vkEndCommandBuffer(cb));

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cb;

    VKRESULT(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE));
    VKRESULT(vkQueueWaitIdle(queue));

    vkFreeCommandBuffers(device, pool, 1, &cb);
}
}  // namespace NEngine::VulkanUtils