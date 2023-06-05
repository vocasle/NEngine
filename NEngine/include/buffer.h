#pragma once

#include <set>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "misc.h"
#include "vulkan_utils.h"

namespace NEngine {

namespace BufferUtils {
void CreateBuffer(VkDevice              device,
                  VkPhysicalDevice      physicalDevice,
                  VkDeviceSize          size,
                  VkBufferUsageFlags    usage,
                  VkMemoryPropertyFlags properties,
                  VkBuffer             &buffer,
                  VkDeviceMemory       &buffer_memory);

void CopyBuffer(VkDevice      device,
                VkCommandPool transferCmdPool,
                VkQueue       transferQueue,
                VkBuffer      src_buffer,
                VkBuffer      dst_buffer,
                VkDeviceSize  size);
}  // namespace BufferUtils

template <typename T>
struct BufferCreateInfo
{
    VkDevice           device;
    VkPhysicalDevice   physicalDevice;
    VkCommandPool      transferCmdPool;
    VkQueue            transferQueue;
    std::vector<T>     data;
    VkBufferUsageFlags usageFlags;
};

template <typename Data>
class Buffer
{
public:
    Buffer(const BufferCreateInfo<Data> &createInfo)
        : m_data(createInfo.data),
          m_buffer(nullptr),
          m_bufferMemory(nullptr),
          m_device(createInfo.device)
    {
        CreateBuffer(createInfo);
    }

    void
    Destroy()
    {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
        vkFreeMemory(m_device, m_bufferMemory, nullptr);
        m_buffer = nullptr;
        m_bufferMemory = nullptr;
    }

    [[nodiscard]] VkBuffer
    Get() const
    {
        return m_buffer;
    }

    ~Buffer()
    {
        ASSERT(m_buffer == nullptr, "ERROR: Destroy() was not called!");
        ASSERT(m_bufferMemory == nullptr, "ERROR: Destroy() was not called!");
    }

private:
    void
    CreateBuffer(const BufferCreateInfo<Data> &createInfo)
    {
        const VkDeviceSize buffer_size = sizeof(m_data[0]) * m_data.size();

        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        BufferUtils::CreateBuffer(createInfo.device,
                                  createInfo.physicalDevice,
                                  buffer_size,
                                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  staging_buffer,
                                  staging_buffer_memory);

        void *data = nullptr;
        VKRESULT(vkMapMemory(createInfo.device,
                             staging_buffer_memory,
                             0,
                             buffer_size,
                             0,
                             &data));

        memcpy(data, m_data.data(), buffer_size);
        vkUnmapMemory(createInfo.device, staging_buffer_memory);

        BufferUtils::CreateBuffer(createInfo.device,
                                  createInfo.physicalDevice,
                                  buffer_size,
                                  createInfo.usageFlags,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                  m_buffer,
                                  m_bufferMemory);

        BufferUtils::CopyBuffer(createInfo.device,
                                createInfo.transferCmdPool,
                                createInfo.transferQueue,
                                staging_buffer,
                                m_buffer,
                                buffer_size);

        vkDestroyBuffer(createInfo.device, staging_buffer, nullptr);
        vkFreeMemory(createInfo.device, staging_buffer_memory, nullptr);
    }

    std::vector<Data> m_data;
    VkBuffer          m_buffer;
    VkDeviceMemory    m_bufferMemory;
    VkDevice          m_device;
};
}  // namespace NEngine