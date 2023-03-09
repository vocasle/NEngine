#include "vulkan_application.h"

#include <SDL_vulkan.h>

#include <format>
#include <iostream>
#include <optional>

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
    : instance_(),
      debug_util_messenger_(),
      window_(window)
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
    pick_physical_device();
}

void
vulkan_application::cleanup()
{
    if constexpr (enable_validation_layers) {
        destroy_debug_utils_messenger_ext(
            instance_, debug_util_messenger_, nullptr);
    }

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

    [[nodiscard]] bool
    is_complete() const
    {
        return graphics_family.has_value();
    }
};

static queue_family_indices
find_queue_families(VkPhysicalDevice device)
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
        ++i;
    }

    return indices;
}

static bool
is_device_suitable(VkPhysicalDevice device)
{
    // VkPhysicalDeviceProperties device_properties;
    // vkGetPhysicalDeviceProperties(device, &device_properties);

    // VkPhysicalDeviceFeatures device_features;
    // vkGetPhysicalDeviceFeatures(device, &device_features);

    // return device_properties.deviceType ==
    //            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    //        device_features.geometryShader;

    const queue_family_indices indices = find_queue_families(device);
    return indices.is_complete();
}

void
vulkan_application::pick_physical_device()
{
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;

    uint32_t device_count = 0;
    VKRESULT(
        vkEnumeratePhysicalDevices(instance_, &device_count, nullptr));

    if (device_count == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    VKRESULT(
        vkEnumeratePhysicalDevices(instance_, &device_count, devices.data()));

    for (const VkPhysicalDevice &device : devices) {
        if (is_device_suitable(device)) {
            physical_device = device;
            break;
        }
    }

    if (physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU");
    }
}

}  // namespace nengine
