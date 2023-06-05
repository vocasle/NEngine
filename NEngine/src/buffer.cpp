#include "buffer.h"

namespace NEngine::BufferUtils {
void
CreateBuffer(VkDevice              device,
             VkPhysicalDevice      physicalDevice,
             VkDeviceSize          size,
             VkBufferUsageFlags    usage,
             VkMemoryPropertyFlags properties,
             VkBuffer             &buffer,
             VkDeviceMemory       &buffer_memory)
{
    const VulkanUtils::QueueFamilyIndices indices =
        VulkanUtils::FindQueueFamilies(physicalDevice, nullptr);
    const std::set<uint32_t> unique_queue_indices = {
        indices.transfer_family.value(), indices.graphics_family.value()};
    std::vector<uint32_t> queue_indices(unique_queue_indices.size());
    std::copy(unique_queue_indices.begin(),
              unique_queue_indices.end(),
              queue_indices.begin());

    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size;
    info.usage = usage;
    info.sharingMode = indices.GetSharingMode();
    info.queueFamilyIndexCount = std::size(queue_indices);
    info.pQueueFamilyIndices = queue_indices.data();

    VKRESULT(vkCreateBuffer(device, &info, nullptr, &buffer));

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memory_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(
        physicalDevice, memory_requirements.memoryTypeBits, properties);

    VKRESULT(vkAllocateMemory(device, &alloc_info, nullptr, &buffer_memory));

    VKRESULT(vkBindBufferMemory(device, buffer, buffer_memory, 0));
}

void
CopyBuffer(VkDevice      device,
           VkCommandPool transferCmdPool,
           VkQueue       transferQueue,
           VkBuffer      src_buffer,
           VkBuffer      dst_buffer,
           VkDeviceSize  size)
{
    const VkCommandBuffer cb =
        VulkanUtils::BeginSingleTimeCommands(device, transferCmdPool);

    VkBufferCopy copy_region{};
    copy_region.size = size;
    vkCmdCopyBuffer(cb, src_buffer, dst_buffer, 1, &copy_region);

    VulkanUtils::EndSingleTimeCommands(
        cb, device, transferCmdPool, transferQueue);
}
}  // namespace NEngine::BufferUtils