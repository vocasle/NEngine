#include "image.h"

#include "misc.h"

namespace NEngine {
Image::Image(const ImageCreateInfo &createInfo,
             VkDevice device,
             VkPhysicalDevice physicalDevice)
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
}  // namespace NEngine