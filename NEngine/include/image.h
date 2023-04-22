#pragma once

#include <vulkan/vulkan.hpp>

namespace NEngine {

struct ImageCreateInfo
{
    uint32_t width;
    uint32_t height;
    VkFormat format;
    uint32_t mipLevels;
    VkSampleCountFlagBits numSamples;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;
};

class Image
{
public:
    Image(const ImageCreateInfo &createInfo,
          VkDevice device,
          VkPhysicalDevice physicalDevice);

private:
    VkImage m_image{};
    VkDeviceMemory m_imageMemory{};
    VkImageView m_imageView{};
};
}  // namespace NEngine