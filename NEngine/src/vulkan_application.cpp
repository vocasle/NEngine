#include "vulkan_application.h"

#include <SDL_vulkan.h>

#include <format>
#include <iostream>
#include <optional>
#include <set>

namespace nengine {

#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enable_validation_layers = true;
#endif

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

void
vk_result(const VkResult &result, const char *filename, int line)
{
    if (result != VK_SUCCESS) {
        throw std::runtime_error(std::format(
            "{}:{} - ERROR: Vulkan API call failed.", filename, line));
    }
}

#define VKRESULT(result) vk_result(result, __FILE__, __LINE__)

vulkan_application::vulkan_application(SDL_Window *window)
    : window_(window),
      instance_(),
      debug_util_messenger_(),
      physical_device_(),
      device_(),
      queue_(),
      present_queue_()
{
    init_vulkan();
}

vulkan_application::~vulkan_application()
{
    cleanup();
}

void
vulkan_application::init_vulkan()
{
    create_instance();
    setup_debug_messenger();
    create_surface();
    pick_physical_device();
    create_logical_device();
}

void
vulkan_application::cleanup()
{
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

bool
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

std::vector<const char *>
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
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

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
choose_swap_extent(SDL_Window *window, VkSurfaceCapabilitiesKHR &capabilities)
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
