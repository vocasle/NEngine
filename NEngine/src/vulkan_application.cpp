#include "vulkan_application.h"

#include <SDL_vulkan.h>

#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>

namespace nengine {

#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enable_validation_layers = true;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct queue_family_indices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    [[nodiscard]] bool
    is_complete() const
    {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct swap_chain_support_details
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

static void
vk_result(const VkResult &result, const char *filename, int line)
{
    if (result != VK_SUCCESS) {
        throw std::runtime_error(std::format(
            "{}:{} - ERROR: Vulkan API call failed.", filename, line));
    }
}

#define VKRESULT(result) vk_result(result, __FILE__, __LINE__)

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
    std::cerr << "validation layer: " << p_callback_data->pMessage << std::endl;

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
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }
        if (indices.is_complete()) {
            break;
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

vulkan_application::vulkan_application(SDL_Window *window)
    : window_(window),
      instance_(),
      debug_util_messenger_(),
      physical_device_(),
      device_(),
      queue_(),
      present_queue_(),
      swap_chain_(),
      swap_chain_image_format_(),
      swap_chain_extent_(),
      pipeline_layout_(),
      render_pass_(),
      graphics_pipeline_(),
      command_pool_()
{
    init_vulkan();
}

void
vulkan_application::draw_frame()
{
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
        recreate_swap_chain();
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    VKRESULT(vkResetFences(device_, 1, &in_flight_fences_[current_frame_]));

    VKRESULT(vkResetCommandBuffer(command_buffers_[current_frame_], 0));

    record_command_buffer(command_buffers_[current_frame_], image_idx);

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
        recreate_swap_chain();
    }
    else {
        VKRESULT(result);
    }

    current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}

void
vulkan_application::on_window_resized()
{
    is_framebuffer_resized = true;
}

vulkan_application::~vulkan_application()
{
    VKRESULT(vkDeviceWaitIdle(device_));

    cleanup();
}

void
vulkan_application::create_command_pool()
{
    const queue_family_indices indices =
        find_queue_families(physical_device_, surface_);

    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex = indices.graphics_family.value();

    VKRESULT(
        vkCreateCommandPool(device_, &create_info, nullptr, &command_pool_));
}

void
vulkan_application::init_vulkan()
{
    create_instance();
    setup_debug_messenger();
    create_surface();
    pick_physical_device();
    create_logical_device();
    create_swap_chain();
    create_image_views();
    create_render_pass();
    create_graphics_pipeline();
    create_framebuffers();
    create_command_pool();
    create_command_buffers();
    create_sync_objects();
}

void
vulkan_application::cleanup() const
{
    cleanup_swap_chain();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkDestroySemaphore(device_, image_available_semaphores_[i], nullptr);
        vkDestroySemaphore(device_, render_finished_semaphores_[i], nullptr);
        vkDestroyFence(device_, in_flight_fences_[i], nullptr);
    }

    vkDestroyCommandPool(device_, command_pool_, nullptr);

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
vulkan_application::setup_debug_messenger()
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
vulkan_application::create_instance()
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
vulkan_application::check_device_extension_support(
    VkPhysicalDevice device) const
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
vulkan_application::create_swap_chain()
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
vulkan_application::create_image_views()
{
    swap_chain_image_views_.resize(swap_chain_images_.size());
    for (size_t i = 0; i < swap_chain_images_.size(); ++i) {
        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = swap_chain_images_[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = swap_chain_image_format_;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;
        VKRESULT(vkCreateImageView(
            device_, &create_info, nullptr, &swap_chain_image_views_[i]));
    }
}

VkShaderModule
vulkan_application::create_shader_module(const std::vector<char> &code) const
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
vulkan_application::create_render_pass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swap_chain_image_format_;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VKRESULT(
        vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_));
}

void
vulkan_application::create_framebuffers()
{
    swap_chain_framebuffers_.resize(swap_chain_image_views_.size());

    for (size_t i = 0; i < swap_chain_image_views_.size(); ++i) {
        VkImageView attachments[] = {swap_chain_image_views_[i]};

        VkFramebufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = render_pass_;
        create_info.attachmentCount = 1;
        create_info.pAttachments = attachments;
        create_info.width = swap_chain_extent_.width;
        create_info.height = swap_chain_extent_.height;
        create_info.layers = 1;

        VKRESULT(vkCreateFramebuffer(
            device_, &create_info, nullptr, &swap_chain_framebuffers_[i]));
    }
}

void
vulkan_application::create_command_buffers()
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
vulkan_application::record_command_buffer(VkCommandBuffer cb,
                                          uint32_t image_idx)
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

    const VkClearValue clear_color = {{{0, 0, 0, 1}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(cb, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

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

    vkCmdDraw(cb, 3, 1, 0, 0);

    vkCmdEndRenderPass(cb);

    VKRESULT(vkEndCommandBuffer(cb));
}

void
vulkan_application::create_sync_objects()
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
vulkan_application::recreate_swap_chain()
{
    //using namespace std::chrono_literals;

    //int width = 0;
    //int height = 0;
    //SDL_GetWindowSizeInPixels(window_, &width, &height);

    //while (width == 0 || height == 0) {
    //    std::this_thread::sleep_for(16ms);
    //    SDL_GetWindowSizeInPixels(window_, &width, &height);
    //}

    vkDeviceWaitIdle(device_);

    cleanup_swap_chain();

    create_swap_chain();
    create_image_views();
    create_framebuffers();
}

void
vulkan_application::cleanup_swap_chain() const
{
    for (auto framebuffer : swap_chain_framebuffers_) {
        vkDestroyFramebuffer(device_, framebuffer, nullptr);
    }
    for (auto image_view : swap_chain_image_views_) {
        vkDestroyImageView(device_, image_view, nullptr);
    }

    vkDestroySwapchainKHR(device_, swap_chain_, nullptr);
}

void
vulkan_application::create_graphics_pipeline()
{
    const std::vector<char> vs_code = read_file("shaders/vert.spv");
    const std::vector<char> ps_code = read_file("shaders/frag.spv");

    const VkShaderModule vsm = create_shader_module(vs_code);
    const VkShaderModule psm = create_shader_module(ps_code);

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

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = nullptr;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = nullptr;

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
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0;
    rasterizer.depthBiasClamp = 0;
    rasterizer.depthBiasSlopeFactor = 0;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1;
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

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
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
    pipeline_create_info.pDepthStencilState = nullptr;
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
vulkan_application::is_device_suitable(VkPhysicalDevice device) const
{
    // VkPhysicalDeviceProperties device_properties;
    // vkGetPhysicalDeviceProperties(device, &device_properties);

    // VkPhysicalDeviceFeatures device_features;
    // vkGetPhysicalDeviceFeatures(device, &device_features);

    // return device_properties.deviceType ==
    //            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    //        device_features.geometryShader;

    const queue_family_indices indices = find_queue_families(device, surface_);
    const bool extensions_supported = check_device_extension_support(device);

    bool is_swap_chain_valid = false;
    if (extensions_supported) {
        const swap_chain_support_details details =
            query_swap_chain_support(device, surface_);
        is_swap_chain_valid =
            !details.formats.empty() && !details.present_modes.empty();
    }

    return indices.is_complete() && extensions_supported && is_swap_chain_valid;
}

void
vulkan_application::pick_physical_device()
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
        if (is_device_suitable(device)) {
            physical_device_ = device;
            break;
        }
    }

    if (physical_device_ == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU");
    }
}

void
vulkan_application::create_logical_device()
{
    const queue_family_indices indices =
        find_queue_families(physical_device_, surface_);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(),
                                                indices.present_family.value()};
    float queue_priority = 1;

    for (uint32_t queue_family : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    const VkPhysicalDeviceFeatures device_features{};

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
}

void
vulkan_application::create_surface()
{
    if (SDL_Vulkan_CreateSurface(window_, instance_, &surface_) == SDL_FALSE) {
        throw std::runtime_error("Failed to create surface");
    }
}

}  // namespace nengine
