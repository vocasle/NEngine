#pragma once

#include <set>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "misc.h"
#include "vulkan_utils.h"

namespace NEngine {

template <typename Data>
class Buffer
{
public:
    Buffer(VkDevice          device,
           VkPhysicalDevice  physicalDevice,
           std::vector<Data> data)
        : m_data(std::move(data))
    {
        CreateBuffer(device, physicalDevice);
    }

    static void
    CreateBuffer(VkDevice              device,
                 VkPhysicalDevice      physicalDevice,
                 VkDeviceSize          size,
                 VkBufferUsageFlags    usage,
                 VkMemoryPropertyFlags properties,
                 VkBuffer             &buffer,
                 VkDeviceMemory       &buffer_memory)
    {
        const VulkanUtils::queue_family_indices indices =
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
        info.sharingMode = indices.get_sharing_mode();
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

        VKRESULT(
            vkAllocateMemory(device, &alloc_info, nullptr, &buffer_memory));

        VKRESULT(vkBindBufferMemory(device, buffer, buffer_memory, 0));
    }

    void
    CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
    {
        //const VkCommandBuffer cb =
        //    begin_single_time_commands(device, transfer_command_pool_);

        //VkBufferCopy copy_region{};
        //copy_region.size = size;
        //vkCmdCopyBuffer(cb, src_buffer, dst_buffer, 1, &copy_region);

        //end_single_time_commands(
        //    cb, device, transfer_command_pool_, transfer_queue_);
    }

private:
    void
    CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice)
    {
        const VkDeviceSize buffer_size = sizeof(m_data[0]) * m_data.size();

        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        CreateBuffer(device,
                     physicalDevice,
                     buffer_size,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     staging_buffer,
                     staging_buffer_memory);

        void *data = nullptr;
        VKRESULT(vkMapMemory(
            device, staging_buffer_memory, 0, buffer_size, 0, &data));

        memcpy(data, m_data.data(), buffer_size);
        vkUnmapMemory(device, staging_buffer_memory);

        CreateBuffer(device,
                     physicalDevice,
                     buffer_size,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     m_buffer,
                     m_bufferMemory);

        CopyBuffer(staging_buffer, m_buffer, buffer_size);

        vkDestroyBuffer(device, staging_buffer, nullptr);
        vkFreeMemory(device, staging_buffer_memory, nullptr);
    }

    std::vector<Data> m_data;

    VkBuffer       m_buffer;
    VkDeviceMemory m_bufferMemory;
};
}  // namespace NEngine