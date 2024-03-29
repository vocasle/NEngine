﻿#include "vulkan_application.h"

#include <SDL_vulkan.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <chrono>
#include <fstream>

#include "misc.h"
#include "vertex.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <optional>
#include <set>
#include <unordered_map>

namespace NEngine {
#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enable_validation_layers = true;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct uniform_buffer_object
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct uniform_buffer_object_ps
{
    alignas(16) glm::vec3 light_pos;
    alignas(16) glm::vec3 cam_pos;
};

struct queue_family_indices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;
    std::optional<uint32_t> transfer_family;

    [[nodiscard]] bool
    is_complete() const
    {
        return graphics_family.has_value() && present_family.has_value() &&
               transfer_family.has_value();
    }

	[[nodiscard]] VkSharingMode
	get_sharing_mode() const
	{
		return graphics_family.value() == transfer_family.value()
			? VK_SHARING_MODE_EXCLUSIVE
			: VK_SHARING_MODE_CONCURRENT;
	}
};

struct swap_chain_support_details
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

static VkSurfaceFormatKHR
choose_swap_surface_format(
    const std::vector<VkSurfaceFormatKHR> &available_formats)
{
    for (const auto &available_format : available_formats) {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return available_format;
        }
    }

    return available_formats[0];
}

static VkPresentModeKHR
choose_swap_present_mode(
    const std::vector<VkPresentModeKHR> &available_present_modes)
{
    for (const auto &available_present_mode : available_present_modes) {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available_present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D
choose_swap_extent(SDL_Window *window,
                   const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(window, &width, &height);

    VkExtent2D actual_extent{static_cast<uint32_t>(width),
                             static_cast<uint32_t>(height)};

    actual_extent.width = std::clamp(actual_extent.width,
                                     capabilities.minImageExtent.width,
                                     capabilities.maxImageExtent.width);

    actual_extent.height = std::clamp(actual_extent.height,
                                      capabilities.minImageExtent.height,
                                      capabilities.maxImageExtent.height);

    return actual_extent;
}

static std::vector<char>
read_file(const std::string &filename)
{
    std::ifstream file(filename, std::ios_base::ate | std::ios_base::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    const size_t file_size = file.tellg();
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), static_cast<long long>(file_size));
    file.close();

    return buffer;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
               VkDebugUtilsMessageTypeFlagsEXT message_type,
               const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
               void *p_user_data)
{
    if (message_severity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        std::cerr << "validation layer: " << p_callback_data->pMessage
                  << std::endl;
    }

    return VK_FALSE;
}

static VkResult
create_debug_utils_messenger_ext(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        return func(instance, p_create_info, p_allocator, p_debug_messenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void
destroy_debug_utils_messenger_ext(VkInstance instance,
                                  VkDebugUtilsMessengerEXT debug_messenger,
                                  const VkAllocationCallbacks *p_allocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        func(instance, debug_messenger, p_allocator);
    }
}

static void
populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &create_info)
{
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
}

static swap_chain_support_details
query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    swap_chain_support_details details;
    VKRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device, surface, &details.capabilities));

    uint32_t format_count = 0;
    VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(
        device, surface, &format_count, nullptr));
    details.formats.resize(format_count);
    VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(
        device, surface, &format_count, details.formats.data()));

    uint32_t present_mode_count = 0;
    VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &present_mode_count, nullptr));
    details.present_modes.resize(present_mode_count);
    VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &present_mode_count, details.present_modes.data()));

    return details;
}

static queue_family_indices
find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    queue_family_indices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queue_family_count, queue_families.data());

    int i = 0;
    for (const auto &queue_family : queue_families) {
        if (indices.is_complete()) {
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

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            device, i, surface, &present_support);
        if (present_support) {
            indices.present_family = i;
        }
        ++i;
    }

    return indices;
}

static VkCommandBuffer
begin_single_time_commands(VkDevice device, VkCommandPool pool)
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

static void
end_single_time_commands(VkCommandBuffer cb,
                         VkDevice device,
                         VkCommandPool pool,
                         VkQueue queue)
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

static bool has_stencil_component(VkFormat format);

static void
transition_image_layout(VkImage image,
                        VkFormat format,
                        VkImageLayout old_layout,
                        VkImageLayout new_layout,
                        VkDevice device,
                        VkCommandPool pool,
                        VkQueue queue,
                        uint32_t mip_levels)
{
    const VkCommandBuffer cb = begin_single_time_commands(device, pool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mip_levels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (has_stencil_component(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags dest_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dest_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dest_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
             new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dest_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else {
        throw std::invalid_argument("Unsupported layout transition");
    }

    vkCmdPipelineBarrier(
        cb, source_stage, dest_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    end_single_time_commands(cb, device, pool, queue);
}

static void
copy_buffer_to_image(VkBuffer buffer,
                     VkImage image,
                     uint32_t width,
                     uint32_t height,
                     VkDevice device,
                     VkCommandPool pool,
                     VkQueue queue)
{
    const VkCommandBuffer cb = begin_single_time_commands(device, pool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(
        cb, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    end_single_time_commands(cb, device, pool, queue);
}

static VkFormat
find_supported_format(const std::vector<VkFormat> &candidates,
                      VkImageTiling tiling,
                      VkFormatFeatureFlags features,
                      VkPhysicalDevice physical_device)
{
    for (const VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features) {
            return format;
        }
        if (tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format");
}

static VkFormat
find_depth_format(VkPhysicalDevice physical_device)
{
    return find_supported_format({VK_FORMAT_D32_SFLOAT,
                                  VK_FORMAT_D32_SFLOAT_S8_UINT,
                                  VK_FORMAT_D24_UNORM_S8_UINT},
                                 VK_IMAGE_TILING_OPTIMAL,
                                 VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                 physical_device);
}

static bool
has_stencil_component(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT;
}

static void
generate_mipmaps(VkImage image,
                 VkFormat image_format,
                 uint32_t tex_width,
                 uint32_t tex_height,
                 uint32_t mip_levels,
                 VkDevice device,
                 VkCommandPool pool,
                 VkQueue queue,
                 VkPhysicalDevice physical_device)
{
    VkFormatProperties format_props;
    vkGetPhysicalDeviceFormatProperties(
        physical_device, image_format, &format_props);

    if (!(format_props.optimalTilingFeatures &
          VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error(
            "Texture image format does not support linear blitting");
    }

    const VkCommandBuffer cb = begin_single_time_commands(device, pool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mip_width = static_cast<int32_t>(tex_width);
    int32_t mip_height = static_cast<int32_t>(tex_height);

    for (uint32_t i = 1; i < mip_levels; ++i) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(cb,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mip_width, mip_height, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {mip_width > 1 ? mip_width / 2 : 1,
                              mip_height > 1 ? mip_height / 2 : 1,
                              1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(cb,
                       image,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &blit,
                       VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cb,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);

        if (mip_width > 1) {
            mip_width /= 2;
        }
        if (mip_height > 1) {
            mip_height /= 2;
        }
    }

    barrier.subresourceRange.baseMipLevel = mip_levels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(cb,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);

    end_single_time_commands(cb, device, pool, queue);
}

static VkSampleCountFlagBits
get_max_usable_sample_count(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(physical_device, &props);

    const VkSampleCountFlags counts =
        props.limits.framebufferColorSampleCounts &
        props.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) {
        return VK_SAMPLE_COUNT_64_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_32_BIT) {
        return VK_SAMPLE_COUNT_32_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_16_BIT) {
        return VK_SAMPLE_COUNT_16_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_8_BIT) {
        return VK_SAMPLE_COUNT_8_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_4_BIT) {
        return VK_SAMPLE_COUNT_4_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_2_BIT) {
        return VK_SAMPLE_COUNT_2_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

void
VulkanApplication::CreateBuffer(VkDeviceSize size,
                                VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkBuffer &buffer,
                                VkDeviceMemory &buffer_memory) const
{
    const queue_family_indices indices =
        find_queue_families(physical_device_, surface_);
    const std::set<uint32_t> unique_queue_indices = {indices.transfer_family.value(),
                                      indices.graphics_family.value()};
    std::vector<uint32_t> queue_indices(unique_queue_indices.size());
    std::copy(unique_queue_indices.begin(), unique_queue_indices.end(), queue_indices.begin());

    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size;
    info.usage = usage;
    info.sharingMode = indices.get_sharing_mode();
    info.queueFamilyIndexCount = std::size(queue_indices);
    info.pQueueFamilyIndices = queue_indices.data();

    VKRESULT(vkCreateBuffer(device_, &info, nullptr, &buffer));

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device_, buffer, &memory_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memory_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(
        physical_device_, memory_requirements.memoryTypeBits, properties);

    VKRESULT(vkAllocateMemory(device_, &alloc_info, nullptr, &buffer_memory));

    VKRESULT(vkBindBufferMemory(device_, buffer, buffer_memory, 0));
}

void
VulkanApplication::CopyBuffer(VkBuffer src_buffer,
                              VkBuffer dst_buffer,
                              VkDeviceSize size)
{
    const VkCommandBuffer cb =
        begin_single_time_commands(device_, transfer_command_pool_);

    VkBufferCopy copy_region{};
    copy_region.size = size;
    vkCmdCopyBuffer(cb, src_buffer, dst_buffer, 1, &copy_region);

    end_single_time_commands(
        cb, device_, transfer_command_pool_, transfer_queue_);
}

void
VulkanApplication::CreateIndexBuffer()
{
    const VkDeviceSize buffer_size = sizeof(indices_[0]) * indices_.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateBuffer(buffer_size,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 staging_buffer,
                 staging_buffer_memory);
    void *data = nullptr;
    VKRESULT(
        vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data));
    memcpy(data, indices_.data(), buffer_size);
    vkUnmapMemory(device_, staging_buffer_memory);

    CreateBuffer(
        buffer_size,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        index_buffer_,
        index_buffer_memory_);

    CopyBuffer(staging_buffer, index_buffer_, buffer_size);

    vkDestroyBuffer(device_, staging_buffer, nullptr);
    vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

void
VulkanApplication::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding ubo_layout_binding{};
    ubo_layout_binding.binding = 0;
    ubo_layout_binding.descriptorCount = 1;
    ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    ubo_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding sampler_layout_binding{};
    sampler_layout_binding.binding = 1;
    sampler_layout_binding.descriptorCount = 1;
    sampler_layout_binding.descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.pImmutableSamplers = nullptr;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding ubo_ps_layout_binding{};
    ubo_ps_layout_binding.binding = 2;
    ubo_ps_layout_binding.descriptorCount = 1;
    ubo_ps_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_ps_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    ubo_ps_layout_binding.pImmutableSamplers = nullptr;

    const std::array<VkDescriptorSetLayoutBinding, 3> bindings = {
        ubo_layout_binding, sampler_layout_binding, ubo_ps_layout_binding};

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = static_cast<uint32_t>(bindings.size());
    info.pBindings = bindings.data();

    VKRESULT(vkCreateDescriptorSetLayout(
        device_, &info, nullptr, &descriptor_set_layout_));
}

void
VulkanApplication::CreateUniformBuffers()
{
    {
        const VkDeviceSize buffer_size = sizeof(uniform_buffer_object);

        uniform_buffers_.resize(MAX_FRAMES_IN_FLIGHT);
        uniform_buffers_mapped_.resize(MAX_FRAMES_IN_FLIGHT);
        uniform_buffers_memory_.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            CreateBuffer(buffer_size,
                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         uniform_buffers_[i],
                         uniform_buffers_memory_[i]);

            vkMapMemory(device_,
                        uniform_buffers_memory_[i],
                        0,
                        buffer_size,
                        0,
                        &uniform_buffers_mapped_[i]);
        }
    }

    {
        const VkDeviceSize buffer_size = sizeof(uniform_buffer_object_ps);

        uniform_buffers_ps_.resize(MAX_FRAMES_IN_FLIGHT);
        uniform_buffers_mapped_ps_.resize(MAX_FRAMES_IN_FLIGHT);
        uniform_buffers_memory_ps_.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            CreateBuffer(buffer_size,
                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         uniform_buffers_ps_[i],
                         uniform_buffers_memory_ps_[i]);

            vkMapMemory(device_,
                        uniform_buffers_memory_ps_[i],
                        0,
                        buffer_size,
                        0,
                        &uniform_buffers_mapped_ps_[i]);
        }
    }
}

void
VulkanApplication::UpdateUniformBuffer() const
{
    static const auto start_time = std::chrono::high_resolution_clock::now();

    const auto current_time = std::chrono::high_resolution_clock::now();

    const float time =
        std::chrono::duration<float, std::chrono::seconds::period>(
            current_time - start_time)
            .count();

    {
        uniform_buffer_object ubo{};
        ubo.model = glm::mat4(1.0f);

        ubo.view = camera_->view;

        ubo.proj =
            glm::perspective(glm::radians(45.0f),
                             swap_chain_extent_.width /
                                 static_cast<float>(swap_chain_extent_.height),
                             0.1f,
                             10.0f);

        ubo.proj[1][1] *= -1;  // flip Y

        memcpy(uniform_buffers_mapped_[current_frame_], &ubo, sizeof(ubo));
    }

    {
        uniform_buffer_object_ps ubo{};
        ubo.cam_pos = camera_->cam_pos;
        ubo.light_pos = glm::vec3(2.0f);

        memcpy(uniform_buffers_mapped_ps_[current_frame_], &ubo, sizeof(ubo));
    }
}

void
VulkanApplication::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 3> pool_sizes{};
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    pool_sizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VKRESULT(vkCreateDescriptorPool(
        device_, &pool_info, nullptr, &descriptor_pool_));
}

void
VulkanApplication::CreateDescriptorSets()
{
    const std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                                     descriptor_set_layout_);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool_;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    alloc_info.pSetLayouts = layouts.data();

    descriptor_sets_.resize(MAX_FRAMES_IN_FLIGHT);
    VKRESULT(vkAllocateDescriptorSets(
        device_, &alloc_info, descriptor_sets_.data()));

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        VkDescriptorBufferInfo buffer_info{};
        buffer_info.offset = 0;
        buffer_info.buffer = uniform_buffers_[i];
        buffer_info.range = sizeof(uniform_buffer_object);

        VkDescriptorImageInfo image_info{};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = m_textureImage->GetImageView();
        image_info.sampler = texture_sampler_;

        VkDescriptorBufferInfo buffer_info_ps{};
        buffer_info_ps.offset = 0;
        buffer_info_ps.buffer = uniform_buffers_ps_[i];
        buffer_info_ps.range = sizeof(uniform_buffer_object_ps);

        std::array<VkWriteDescriptorSet, 3> descriptor_writes{};
        descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[0].dstSet = descriptor_sets_[i];
        descriptor_writes[0].dstBinding = 0;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo = &buffer_info;

        descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[1].dstSet = descriptor_sets_[i];
        descriptor_writes[1].dstBinding = 1;
        descriptor_writes[1].dstArrayElement = 0;
        descriptor_writes[1].descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_writes[1].descriptorCount = 1;
        descriptor_writes[1].pImageInfo = &image_info;

        descriptor_writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[2].dstSet = descriptor_sets_[i];
        descriptor_writes[2].dstBinding = 2;
        descriptor_writes[2].dstArrayElement = 0;
        descriptor_writes[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_writes[2].descriptorCount = 1;
        descriptor_writes[2].pBufferInfo = &buffer_info_ps;

        vkUpdateDescriptorSets(device_,
                               descriptor_writes.size(),
                               descriptor_writes.data(),
                               0,
                               nullptr);
    }
}

void
VulkanApplication::CreateTextureImage(const std::string &texture_path)
{
    int tex_width = 0;
    int tex_height = 0;
    int tex_channels = 0;
    stbi_uc *pixels = stbi_load(texture_path.c_str(),
                                &tex_width,
                                &tex_height,
                                &tex_channels,
                                STBI_rgb_alpha);

    mip_levels_ = static_cast<uint32_t>(
                      std::floor(std::log2(std::max(tex_width, tex_height)))) +
                  1;

    const VkDeviceSize image_size = tex_width * tex_height * 4;

    if (!pixels) {
        throw std::runtime_error("Failed to load texture image");
    }

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateBuffer(image_size,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 staging_buffer,
                 staging_buffer_memory);

    void *data = nullptr;
    VKRESULT(
        vkMapMemory(device_, staging_buffer_memory, 0, image_size, 0, &data));
    memcpy(data, pixels, image_size);
    vkUnmapMemory(device_, staging_buffer_memory);

    stbi_image_free(pixels);

    ImageCreateInfo createInfo = {};
    createInfo.width = tex_width;
    createInfo.height = tex_height;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.mipLevels = mip_levels_;
    createInfo.numSamples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                       VK_IMAGE_USAGE_SAMPLED_BIT |
                       VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    createInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    m_textureImage =
        std::make_unique<Image>(createInfo, device_, physical_device_);

    transition_image_layout(m_textureImage->GetImage(),
                            VK_FORMAT_R8G8B8A8_SRGB,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            device_,
                            command_pool_,
                            queue_,
                            mip_levels_);

    copy_buffer_to_image(staging_buffer,
                         m_textureImage->GetImage(),
                         tex_width,
                         tex_height,
                         device_,
                         transfer_command_pool_,
                         transfer_queue_);

    generate_mipmaps(m_textureImage->GetImage(),
                     VK_FORMAT_R8G8B8A8_SRGB,
                     tex_width,
                     tex_height,
                     mip_levels_,
                     device_,
                     command_pool_,
                     queue_,
                     physical_device_);

    vkDestroyBuffer(device_, staging_buffer, nullptr);
    vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

void
VulkanApplication::CreateImage(uint32_t width,
                               uint32_t height,
                               VkFormat format,
                               uint32_t mip_levels,
                               VkSampleCountFlagBits num_samples,
                               VkImageTiling tiling,
                               VkImageUsageFlags usage,
                               VkMemoryPropertyFlags properties,
                               VkImage &image,
                               VkDeviceMemory &image_memory) const
{
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = mip_levels;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = num_samples;

    VKRESULT(vkCreateImage(device_, &image_info, nullptr, &image));

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(device_, image, &memory_requirements);

    VkMemoryAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.allocationSize = memory_requirements.size;
    allocate_info.memoryTypeIndex =
        find_memory_type(physical_device_,
                         memory_requirements.memoryTypeBits,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VKRESULT(vkAllocateMemory(device_, &allocate_info, nullptr, &image_memory));

    VKRESULT(vkBindImageMemory(device_, image, image_memory, 0));
}

void
VulkanApplication::CreateTextureImageView()
{
    m_textureImage->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT, mip_levels_);
}

VkImageView
VulkanApplication::CreateImageView(VkImage image,
                                   VkFormat format,
                                   VkImageAspectFlags aspect_flags,
                                   uint32_t mip_levels) const
{
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = image;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = format;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = aspect_flags;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = mip_levels;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    VkImageView image_view;
    VKRESULT(vkCreateImageView(device_, &create_info, nullptr, &image_view));

    return image_view;
}

void
VulkanApplication::CreateTextureSampler()
{
    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physical_device_, &properties);
    sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = static_cast<float>(mip_levels_);

    VKRESULT(
        vkCreateSampler(device_, &sampler_info, nullptr, &texture_sampler_));
}

void
VulkanApplication::CreateDepthResources()
{
    const VkFormat depth_format = find_depth_format(physical_device_);

    ImageCreateInfo createInfo = {};
    createInfo.format = depth_format;
    createInfo.height = swap_chain_extent_.height;
    createInfo.width = swap_chain_extent_.width;
    createInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createInfo.numSamples = msaa_samples_;
    createInfo.mipLevels = 1;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    m_depthImage =
        std::make_unique<Image>(createInfo, device_, physical_device_);
    m_depthImage->CreateImageView(VK_IMAGE_ASPECT_DEPTH_BIT, 1);

    transition_image_layout(m_depthImage->GetImage(),
                            depth_format,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                            device_,
                            command_pool_,
                            queue_,
                            1);
}

void
VulkanApplication::CreateColorResources()
{
    const VkFormat color_format = swap_chain_image_format_;
    CreateImage(swap_chain_extent_.width,
                swap_chain_extent_.height,
                color_format,
                1,
                msaa_samples_,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                color_image_,
                color_image_memory_);
    color_image_view_ = CreateImageView(
        color_image_, color_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void
VulkanApplication::InitImGui()
{
    const VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VKRESULT(
        vkCreateDescriptorPool(device_, &pool_info, nullptr, &imgui_pool_));

    // 2: initialize imgui library

    // this initializes the core structures of imgui
    ImGui::CreateContext();

    // this initializes imgui for SDL
    ImGui_ImplSDL2_InitForVulkan(window_);

    // this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance_;
    init_info.PhysicalDevice = physical_device_;
    init_info.Device = device_;
    init_info.Queue = queue_;
    init_info.DescriptorPool = imgui_pool_;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = msaa_samples_;

    ImGui_ImplVulkan_Init(&init_info, render_pass_);

    // execute a gpu command to upload imgui font textures
    const VkCommandBuffer cb =
        begin_single_time_commands(device_, command_pool_);

    ImGui_ImplVulkan_CreateFontsTexture(cb);

    end_single_time_commands(cb, device_, command_pool_, queue_);

    // clear font textures from cpu data
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void
VulkanApplication::DestroyImGui() const
{
    vkDestroyDescriptorPool(device_, imgui_pool_, nullptr);
    ImGui_ImplVulkan_Shutdown();
}

VulkanApplication::VulkanApplication(SDL_Window *window)
    : window_(window)
{
    InitVulkan();
    camera_ = std::make_unique<Camera>(
        swap_chain_extent_.width, swap_chain_extent_.height, 10);
}

void
VulkanApplication::DrawFrame()
{
    ImGui::Render();

    VKRESULT(vkWaitForFences(
        device_, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX));

    uint32_t image_idx;
    VkResult result =
        vkAcquireNextImageKHR(device_,
                              swap_chain_,
                              UINT64_MAX,
                              image_available_semaphores_[current_frame_],
                              VK_NULL_HANDLE,
                              &image_idx);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain();
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    camera_->Update();

    UpdateUniformBuffer();

    VKRESULT(vkResetFences(device_, 1, &in_flight_fences_[current_frame_]));

    VKRESULT(vkResetCommandBuffer(command_buffers_[current_frame_], 0));

    RecordCommandBuffer(command_buffers_[current_frame_], image_idx);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const VkSemaphore wait_semaphores[] = {
        image_available_semaphores_[current_frame_]};
    const VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffers_[current_frame_];

    const VkSemaphore signal_semaphores[] = {
        render_finished_semaphores_[current_frame_]};

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    VKRESULT(vkQueueSubmit(
        queue_, 1, &submit_info, in_flight_fences_[current_frame_]));

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    const VkSwapchainKHR swap_chains[] = {swap_chain_};

    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &image_idx;

    result = vkQueuePresentKHR(queue_, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        is_framebuffer_resized) {
        is_framebuffer_resized = false;
        RecreateSwapChain();
    }
    else {
        VKRESULT(result);
    }

    current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}

void
VulkanApplication::OnWindowResized()
{
    is_framebuffer_resized = true;
}

VulkanApplication::~VulkanApplication()
{
    VKRESULT(vkDeviceWaitIdle(device_));

    Cleanup();
}

static void generate_normals(std::vector<vertex> &vertices,
                             const std::vector<uint32_t> &indices);

static std::string
resolve_resource_path(const char *resource_path)
{
    std::ostringstream out;
    out << RES_HOME_DIR << "/" << resource_path;
    return out.str();
}

void
VulkanApplication::LoadModel(const std::string &path)
{
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    const std::string MODEL_PATH = resolve_resource_path("models/teapot.obj");
    const std::string TEXTURE_PATH =
        resolve_resource_path("textures/viking_room.png");

    CreateTextureImage(TEXTURE_PATH);
    CreateTextureImageView();
    CreateTextureSampler();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    if (!tinyobj::LoadObj(
            &attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        throw std::runtime_error("Failed to load model. " + warn + err);
    }

    std::unordered_map<vertex, uint32_t> unique_vertices{};

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            vertex v{};

            v.pos = {attrib.vertices[3 * index.vertex_index + 0],
                     attrib.vertices[3 * index.vertex_index + 1],
                     attrib.vertices[3 * index.vertex_index + 2]};

            if (!attrib.texcoords.empty()) {
                v.tex_coord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            if (!attrib.normals.empty()) {
                v.normal = {attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2]};
            }

            v.color = {1.0f, 1.0f, 1.0f};

            if (!unique_vertices.contains(v)) {
                unique_vertices.insert(
                    std::make_pair(v, static_cast<uint32_t>(vertices_.size())));
                vertices_.push_back(v);
            }

            indices_.push_back(unique_vertices[v]);
        }
    }

    if (attrib.normals.empty()) {
        generate_normals(vertices_, indices_);
    }
}

void
VulkanApplication::OnMouseMove(uint32_t mouse_state, int x, int y)
{
    camera_->UpdateMousePos(mouse_state, glm::vec2(x, y));
}

void
VulkanApplication::CreateCommandPool()
{
    const queue_family_indices indices =
        find_queue_families(physical_device_, surface_);

    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex = indices.graphics_family.value();

    VKRESULT(
        vkCreateCommandPool(device_, &create_info, nullptr, &command_pool_));

    create_info.queueFamilyIndex = indices.transfer_family.value();
    create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    VKRESULT(vkCreateCommandPool(
        device_, &create_info, nullptr, &transfer_command_pool_));
}

void
VulkanApplication::InitVulkan()
{
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateSwapchain();
    CreateImageView();
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreateGraphicsPipeline();
    CreateCommandPool();
    CreateColorResources();
    CreateDepthResources();
    CreateFramebuffers();
    LoadModel("");
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateCommandBuffers();
    CreateSyncObjects();

    InitImGui();
}

void
VulkanApplication::Cleanup() const
{
    DestroyImGui();

    CleanupSwapChain();

    vkDestroySampler(device_, texture_sampler_, nullptr);

    m_textureImage->Cleanup();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkDestroyBuffer(device_, uniform_buffers_[i], nullptr);
        vkFreeMemory(device_, uniform_buffers_memory_[i], nullptr);
        // TODO: Move to helper structure to handle this automatically
        vkDestroyBuffer(device_, uniform_buffers_ps_[i], nullptr);
        vkFreeMemory(device_, uniform_buffers_memory_ps_[i], nullptr);
    }

    vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);

    vkDestroyDescriptorSetLayout(device_, descriptor_set_layout_, nullptr);

    vkDestroyBuffer(device_, vertex_buffer_, nullptr);
    vkFreeMemory(device_, vertex_buffer_memory_, nullptr);

    vkDestroyBuffer(device_, index_buffer_, nullptr);
    vkFreeMemory(device_, index_buffer_memory_, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkDestroySemaphore(device_, image_available_semaphores_[i], nullptr);
        vkDestroySemaphore(device_, render_finished_semaphores_[i], nullptr);
        vkDestroyFence(device_, in_flight_fences_[i], nullptr);
    }

    vkDestroyCommandPool(device_, command_pool_, nullptr);
    vkDestroyCommandPool(device_, transfer_command_pool_, nullptr);

    vkDestroyPipeline(device_, graphics_pipeline_, nullptr);
    vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
    vkDestroyRenderPass(device_, render_pass_, nullptr);

    vkDestroyDevice(device_, nullptr);
    if constexpr (enable_validation_layers) {
        destroy_debug_utils_messenger_ext(
            instance_, debug_util_messenger_, nullptr);
    }
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

void
VulkanApplication::SetupDebugMessenger()
{
    if constexpr (!enable_validation_layers) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT create_info{};
    populate_debug_messenger_create_info(create_info);

    VKRESULT(create_debug_utils_messenger_ext(
        instance_, &create_info, nullptr, &debug_util_messenger_));
}

static bool
check_validation_layer_support(const std::vector<const char *> &required_layers)
{
    uint32_t layer_count = 0;
    VKRESULT(vkEnumerateInstanceLayerProperties(&layer_count, nullptr));

    std::vector<VkLayerProperties> available_layers(layer_count);
    VKRESULT(vkEnumerateInstanceLayerProperties(&layer_count,
                                                available_layers.data()));

    for (const char *layer_name : required_layers) {
        bool layer_found = false;

        for (const auto &layer_properties : available_layers) {
            if (strcmp(layer_name, layer_properties.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            std::cerr << "ERROR: Failed to find required layer " << layer_name
                      << std::endl;
            return false;
        }
    }

    return true;
}

static std::vector<const char *>
get_required_extensions(SDL_Window *window)
{
    uint32_t extension_count = 0;
    std::vector<const char *> extensions;

    if (SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr) ==
        SDL_FALSE) {
        throw std::runtime_error("Failed to get Vulkan extensions");
    }

    extensions.resize(extension_count);
    if (SDL_Vulkan_GetInstanceExtensions(
            window, &extension_count, extensions.data()) == SDL_FALSE) {
        throw std::runtime_error("Failed to get Vulkan extensions");
    }

    if (enable_validation_layers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void
VulkanApplication::CreateInstance()
{
    if (enable_validation_layers &&
        !check_validation_layer_support(validation_layers)) {
        throw std::runtime_error("Requested validation layer is not available");
    }

    constexpr VkApplicationInfo application_info{
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "Hello Triangle",
        VK_MAKE_VERSION(1, 0, 0),
        "NEngine",
        VK_MAKE_VERSION(0, 0, 1),
        VK_API_VERSION_1_3};

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &application_info;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};

    const std::vector<const char *> required_extensions =
        get_required_extensions(window_);

    create_info.ppEnabledExtensionNames = required_extensions.data();
    create_info.enabledExtensionCount =
        static_cast<uint32_t>(required_extensions.size());

    if constexpr (enable_validation_layers) {
        create_info.ppEnabledLayerNames = validation_layers.data();
        create_info.enabledLayerCount =
            static_cast<uint32_t>(validation_layers.size());

        populate_debug_messenger_create_info(debug_create_info);
        create_info.pNext = &debug_create_info;
    }
    else {
        create_info.enabledExtensionCount = 0;
    }

    VKRESULT(vkCreateInstance(&create_info, nullptr, &instance_));
}

bool
VulkanApplication::CheckDeviceExtensionSupport(VkPhysicalDevice device) const
{
    uint32_t extensions_count = 0;
    VKRESULT(vkEnumerateDeviceExtensionProperties(
        device, nullptr, &extensions_count, nullptr));

    std::vector<VkExtensionProperties> available_extensions(extensions_count);
    VKRESULT(vkEnumerateDeviceExtensionProperties(
        device, nullptr, &extensions_count, available_extensions.data()));

    std::set<std::string> required_extensions(device_extensions.begin(),
                                              device_extensions.end());
    for (const auto &extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

void
VulkanApplication::CreateSwapchain()
{
    const swap_chain_support_details details =
        query_swap_chain_support(physical_device_, surface_);

    const VkSurfaceFormatKHR surface_format =
        choose_swap_surface_format(details.formats);
    const VkPresentModeKHR present_mode =
        choose_swap_present_mode(details.present_modes);
    const VkExtent2D extent = choose_swap_extent(window_, details.capabilities);

    uint32_t image_count = details.capabilities.minImageCount + 1;

    if (details.capabilities.maxImageCount > 0 &&
        image_count > details.capabilities.maxImageCount) {
        image_count = details.capabilities.minImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface_;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const queue_family_indices indices =
        find_queue_families(physical_device_, surface_);
    const uint32_t queue_family_indices[] = {indices.graphics_family.value(),
                                             indices.present_family.value()};
    if (indices.graphics_family != indices.present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }
    create_info.preTransform = details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VKRESULT(
        vkCreateSwapchainKHR(device_, &create_info, nullptr, &swap_chain_));

    VKRESULT(
        vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, nullptr));
    swap_chain_images_.resize(image_count);
    VKRESULT(vkGetSwapchainImagesKHR(
        device_, swap_chain_, &image_count, swap_chain_images_.data()));

    swap_chain_image_format_ = create_info.imageFormat;
    swap_chain_extent_ = create_info.imageExtent;
}

void
VulkanApplication::CreateImageView()
{
    swap_chain_image_views_.resize(swap_chain_images_.size());
    for (size_t i = 0; i < swap_chain_images_.size(); ++i) {
        swap_chain_image_views_[i] = CreateImageView(swap_chain_images_[i],
                                                     swap_chain_image_format_,
                                                     VK_IMAGE_ASPECT_COLOR_BIT,
                                                     1);
    }
}

VkShaderModule
VulkanApplication::CreateShaderModule(const std::vector<char> &code) const
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shader_module;
    VKRESULT(
        vkCreateShaderModule(device_, &create_info, nullptr, &shader_module));

    return shader_module;
}

void
VulkanApplication::CreateRenderPass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swap_chain_image_format_;
    color_attachment.samples = msaa_samples_;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = find_depth_format(physical_device_);
    depth_attachment.samples = msaa_samples_;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription color_attachment_resolve{};
    color_attachment_resolve.format = swap_chain_image_format_;
    color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_resolve_ref{};
    color_attachment_resolve_ref.attachment = 2;
    color_attachment_resolve_ref.layout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;
    subpass.pResolveAttachments = &color_attachment_resolve_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                               VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    const std::array<VkAttachmentDescription, 3> attachments = {
        color_attachment, depth_attachment, color_attachment_resolve};
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount =
        static_cast<uint32_t>(attachments.size());
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VKRESULT(
        vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_));
}

void
VulkanApplication::CreateFramebuffers()
{
    swap_chain_framebuffers_.resize(swap_chain_image_views_.size());

    for (size_t i = 0; i < swap_chain_image_views_.size(); ++i) {
        std::array<VkImageView, 3> attachments = {color_image_view_,
                                                  m_depthImage->GetImageView(),
                                                  swap_chain_image_views_[i]};

        VkFramebufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = render_pass_;
        create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        create_info.pAttachments = attachments.data();
        create_info.width = swap_chain_extent_.width;
        create_info.height = swap_chain_extent_.height;
        create_info.layers = 1;

        VKRESULT(vkCreateFramebuffer(
            device_, &create_info, nullptr, &swap_chain_framebuffers_[i]));
    }
}

void
VulkanApplication::CreateCommandBuffers()
{
    command_buffers_.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = command_pool_;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount =
        static_cast<uint32_t>(command_buffers_.size());

    VKRESULT(vkAllocateCommandBuffers(
        device_, &allocate_info, command_buffers_.data()));
}

void
VulkanApplication::RecordCommandBuffer(VkCommandBuffer cb,
                                       uint32_t image_idx) const
{
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;

    VKRESULT(vkBeginCommandBuffer(cb, &begin_info));

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = render_pass_;
    render_pass_info.framebuffer = swap_chain_framebuffers_[image_idx];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swap_chain_extent_;

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = {{0, 0, 0, 1}};
    clear_values[1].depthStencil = {1.0f, 0};

    render_pass_info.clearValueCount =
        static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(cb, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

    const VkBuffer vertex_buffers[] = {vertex_buffer_};
    const VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cb, 0, 1, vertex_buffers, offsets);

    vkCmdBindIndexBuffer(cb, index_buffer_, 0, VK_INDEX_TYPE_UINT32);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swap_chain_extent_.width);
    viewport.height = static_cast<float>(swap_chain_extent_.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cb, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swap_chain_extent_;
    vkCmdSetScissor(cb, 0, 1, &scissor);

    vkCmdBindDescriptorSets(cb,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline_layout_,
                            0,
                            1,
                            &descriptor_sets_[current_frame_],
                            0,
                            nullptr);

    vkCmdDrawIndexed(cb, static_cast<uint32_t>(indices_.size()), 1, 0, 0, 0);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb);

    vkCmdEndRenderPass(cb);

    VKRESULT(vkEndCommandBuffer(cb));
}

void
VulkanApplication::CreateSyncObjects()
{
    image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        VKRESULT(vkCreateSemaphore(device_,
                                   &semaphore_info,
                                   nullptr,
                                   &image_available_semaphores_[i]));
        VKRESULT(vkCreateSemaphore(device_,
                                   &semaphore_info,
                                   nullptr,
                                   &render_finished_semaphores_[i]));
        VKRESULT(vkCreateFence(
            device_, &fence_info, nullptr, &in_flight_fences_[i]));
    }
}

void
VulkanApplication::RecreateSwapChain()
{
    vkDeviceWaitIdle(device_);

    CleanupSwapChain();

    CreateSwapchain();
    CreateImageView();
    CreateColorResources();
    CreateDepthResources();
    CreateFramebuffers();
}

void
VulkanApplication::CleanupSwapChain() const
{
    vkDestroyImageView(device_, color_image_view_, nullptr);
    vkDestroyImage(device_, color_image_, nullptr);
    vkFreeMemory(device_, color_image_memory_, nullptr);

    m_depthImage->Cleanup();

    for (auto framebuffer : swap_chain_framebuffers_) {
        vkDestroyFramebuffer(device_, framebuffer, nullptr);
    }
    for (auto image_view : swap_chain_image_views_) {
        vkDestroyImageView(device_, image_view, nullptr);
    }

    vkDestroySwapchainKHR(device_, swap_chain_, nullptr);
}

void
VulkanApplication::CreateVertexBuffer()
{
    const VkDeviceSize buffer_size = sizeof(vertices_[0]) * vertices_.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateBuffer(buffer_size,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 staging_buffer,
                 staging_buffer_memory);

    void *data = nullptr;
    VKRESULT(
        vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data));

    memcpy(data, vertices_.data(), buffer_size);
    vkUnmapMemory(device_, staging_buffer_memory);

    CreateBuffer(
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertex_buffer_,
        vertex_buffer_memory_);

    CopyBuffer(staging_buffer, vertex_buffer_, buffer_size);

    vkDestroyBuffer(device_, staging_buffer, nullptr);
    vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

static std::string
resolve_shader_path(const char *path)
{
    std::ostringstream out;
    out << SHADERS_HOME_DIR << "/" << path;
    return out.str();
}

void
VulkanApplication::CreateGraphicsPipeline()
{
    const std::vector<char> vs_code =
        read_file(resolve_shader_path("phong_vs.spv"));
    const std::vector<char> ps_code =
        read_file(resolve_shader_path("phong_fs.spv"));

    const VkShaderModule vsm = CreateShaderModule(vs_code);
    const VkShaderModule psm = CreateShaderModule(ps_code);

    VkPipelineShaderStageCreateInfo vs_stage_info{};
    vs_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vs_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vs_stage_info.module = vsm;
    vs_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo ps_stage_info{};
    ps_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ps_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    ps_stage_info.module = psm;
    ps_stage_info.pName = "main";

    const VkPipelineShaderStageCreateInfo shader_stages[] = {vs_stage_info,
                                                             ps_stage_info};

    const auto binding_desc = vertex::get_binding_description();
    const auto attribute_desc = vertex::get_attribute_descriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_desc;
    vertex_input_info.vertexAttributeDescriptionCount = attribute_desc.size();
    vertex_input_info.pVertexAttributeDescriptions = attribute_desc.data();

    VkPipelineInputAssemblyStateCreateInfo ia{};
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ia.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(swap_chain_extent_.width);
    viewport.height = static_cast<float>(swap_chain_extent_.height);
    viewport.minDepth = 0;
    viewport.maxDepth = 1;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swap_chain_extent_;

    const std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state{};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount =
        static_cast<uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0;
    rasterizer.depthBiasClamp = 0;
    rasterizer.depthBiasSlopeFactor = 0;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = msaa_samples_;
    multisampling.sampleShadingEnable = VK_TRUE;
    multisampling.minSampleShading = 0.2f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // additive blending based on opacity
    // finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
    // finalColor.a = newAlpha.a;
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;

    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    depth_stencil.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.minDepthBounds = 0.0f;
    depth_stencil.maxDepthBounds = 1.0f;
    depth_stencil.stencilTestEnable = VK_FALSE;

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &descriptor_set_layout_;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    VKRESULT(vkCreatePipelineLayout(
        device_, &pipeline_layout_info, nullptr, &pipeline_layout_));

    VkGraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.sType =
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.stageCount = 2;
    pipeline_create_info.pStages = shader_stages;
    pipeline_create_info.pVertexInputState = &vertex_input_info;
    pipeline_create_info.pInputAssemblyState = &ia;
    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pRasterizationState = &rasterizer;
    pipeline_create_info.pMultisampleState = &multisampling;
    pipeline_create_info.pDepthStencilState = &depth_stencil;
    pipeline_create_info.pColorBlendState = &color_blending;
    pipeline_create_info.pDynamicState = &dynamic_state;
    pipeline_create_info.layout = pipeline_layout_;
    pipeline_create_info.renderPass = render_pass_;
    pipeline_create_info.subpass = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = -1;

    VKRESULT(vkCreateGraphicsPipelines(device_,
                                       VK_NULL_HANDLE,
                                       1,
                                       &pipeline_create_info,
                                       nullptr,
                                       &graphics_pipeline_));

    vkDestroyShaderModule(device_, vsm, nullptr);
    vkDestroyShaderModule(device_, psm, nullptr);
}

bool
VulkanApplication::IsDeviceSuitable(VkPhysicalDevice device) const
{
    // VkPhysicalDeviceProperties device_properties;
    // vkGetPhysicalDeviceProperties(device, &device_properties);

    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(device, &device_features);

    // return device_properties.deviceType ==
    //            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    //        device_features.geometryShader;

    const queue_family_indices indices = find_queue_families(device, surface_);
    const bool extensions_supported = CheckDeviceExtensionSupport(device);

    bool is_swap_chain_valid = false;
    if (extensions_supported) {
        const swap_chain_support_details details =
            query_swap_chain_support(device, surface_);
        is_swap_chain_valid =
            !details.formats.empty() && !details.present_modes.empty();
    }

    return indices.is_complete() && extensions_supported &&
           is_swap_chain_valid && device_features.samplerAnisotropy;
}

void
VulkanApplication::PickPhysicalDevice()
{
    physical_device_ = VK_NULL_HANDLE;

    uint32_t device_count = 0;
    VKRESULT(vkEnumeratePhysicalDevices(instance_, &device_count, nullptr));

    if (device_count == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    VKRESULT(
        vkEnumeratePhysicalDevices(instance_, &device_count, devices.data()));

    for (const VkPhysicalDevice &device : devices) {
        if (IsDeviceSuitable(device)) {
            physical_device_ = device;
            msaa_samples_ = get_max_usable_sample_count(device);
            break;
        }
    }

    if (physical_device_ == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU");
    }
}

void
VulkanApplication::CreateLogicalDevice()
{
    const queue_family_indices indices =
        find_queue_families(physical_device_, surface_);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {
        indices.graphics_family.value(),
        indices.present_family.value(),
        indices.transfer_family.value()};
    float queue_priority = 1;

    for (uint32_t queue_family : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};
    device_features.samplerAnisotropy = VK_TRUE;
    device_features.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount =
        static_cast<uint32_t>(queue_create_infos.size());
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount =
        static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();

    if constexpr (enable_validation_layers) {
        create_info.enabledLayerCount =
            static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    }
    else {
        create_info.enabledLayerCount = 0;
    }

    VKRESULT(vkCreateDevice(physical_device_, &create_info, nullptr, &device_));

    vkGetDeviceQueue(device_, indices.graphics_family.value(), 0, &queue_);
    vkGetDeviceQueue(
        device_, indices.present_family.value(), 0, &present_queue_);
    vkGetDeviceQueue(
        device_, indices.transfer_family.value(), 0, &transfer_queue_);
}

void
VulkanApplication::CreateSurface()
{
    if (SDL_Vulkan_CreateSurface(window_, instance_, &surface_) == SDL_FALSE) {
        throw std::runtime_error("Failed to create surface");
    }
}

static void
generate_normals(std::vector<vertex> &vertices,
                 const std::vector<uint32_t> &indices)
{
    for (uint32_t i = 0u; i < indices.size(); i += 3u) {
        auto &v0 = vertices.at(indices.at(i));
        auto &v1 = vertices.at(indices.at(i + 1));
        auto &v2 = vertices.at(indices.at(i + 2));

        const auto e0 = v1.pos - v0.pos;
        const auto e1 = v2.pos - v1.pos;

        const auto n0 = glm::cross(e0, e1);

        v0.normal += n0;
        v1.normal += n0;
        v2.normal += n0;
    }

    for (vertex &v : vertices) {
        v.normal = glm::normalize(v.normal);
    }
}
}  // namespace NEngine
