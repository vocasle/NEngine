#include "image.h"

#include "misc.h"

namespace NEngine {
Image::Image(const ImageCreateInfo &createInfo,
             VkDevice device,
             VkPhysicalDevice physicalDevice)
    : m_device(device),
      m_format(createInfo.format)
{
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = createInfo.width;
    image_info.extent.height = createInfo.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = createInfo.mipLevels;
    image_info.arrayLayers = 1;
    image_info.format = createInfo.format;
    image_info.tiling = createInfo.tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = createInfo.usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = createInfo.numSamples;

    VKRESULT(vkCreateImage(device, &image_info, nullptr, &m_image));

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(device, m_image, &memory_requirements);

    VkMemoryAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.allocationSize = memory_requirements.size;
    allocate_info.memoryTypeIndex =
        find_memory_type(physicalDevice,
                         memory_requirements.memoryTypeBits,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VKRESULT(vkAllocateMemory(device, &allocate_info, nullptr, &m_imageMemory));

    VKRESULT(vkBindImageMemory(device, m_image, m_imageMemory, 0));
}
Image::~Image()
{
    Cleanup();
}
void
Image::Cleanup() const
{
    vkDestroyImageView(m_device, m_imageView, nullptr);
    vkDestroyImage(m_device, m_image, nullptr);
    vkFreeMemory(m_device, m_imageMemory, nullptr);
}
void
Image::CreateImageView(VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = m_image;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = m_format;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = aspectFlags;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = mipLevels;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    VkImageView image_view;
    VKRESULT(vkCreateImageView(m_device, &create_info, nullptr, &m_imageView));
}
}  // namespace NEngine