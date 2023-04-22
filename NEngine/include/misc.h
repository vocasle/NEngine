#pragma once

#include <cassert>
#include <format>
#include <string>
#include <vulkan/vulkan.hpp>

namespace NEngine {

inline std::string
vk_result(const VkResult &result, const char *filename, int line)
{
    if (result != VK_SUCCESS) {
        const std::string errMsg = std::format(
            "{}:{} - ERROR: Vulkan API call failed.", filename, line);
        return errMsg.c_str();
    }
    return "";
}

inline uint32_t
find_memory_type(VkPhysicalDevice physical_device,
                 uint32_t type_filter,
                 VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if (type_filter & (1 << i) &&
            (memory_properties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type");
}

#if _NDEBUG
#define ASSERT(...)
#define VKRESULT(...)
#else
#define ASSERT(x, msg) assert((x) && (msg))
#define VKRESULT(result)                                           \
    do {                                                           \
        const auto errMsg = vk_result(result, __FILE__, __LINE__); \
        ASSERT(errMsg.empty(), errMsg.c_str());                    \
    }                                                              \
    while (0)
#endif

}  // namespace NEngine