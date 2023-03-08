#include "vulkan_application.h"

#include <SDL_vulkan.h>

#include <format>
#include <iostream>

namespace nengine {

#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enable_validation_layers = true;
#endif

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
}

void
vulkan_application::cleanup()
{
    vkDestroyInstance(instance_, nullptr);
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
get_requried_extensions(SDL_Window *window)
{
    uint32_t extension_count = 0;
    std::vector<const char *> extensions;

    if (SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr) ==
        SDL_FALSE) {
        throw std::runtime_error("Failed to get Vulkan extensions");
    }

    extensions.resize(extension_count);
    if (SDL_Vulkan_GetInstanceExtensions(
            window, &extension_count, &extensions[0]) == SDL_FALSE) {
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

    const VkApplicationInfo application_info{VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                             nullptr,
                                             "Hello Triangle",
                                             VK_MAKE_VERSION(1, 0, 0),
                                             "NEngine",
                                             VK_MAKE_VERSION(0, 0, 1),
                                             VK_API_VERSION_1_3};

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &application_info;

    const std::vector<const char *> required_extensions =
        get_requried_extensions(window_);

    create_info.ppEnabledExtensionNames = required_extensions.data();
    create_info.enabledExtensionCount = required_extensions.size();

    if constexpr (enable_validation_layers) {
        create_info.ppEnabledLayerNames = validation_layers.data();
        create_info.enabledLayerCount =
            static_cast<uint32_t>(validation_layers.size());
    }
    else {
        create_info.enabledExtensionCount = 0;
    }

    VKRESULT(vkCreateInstance(&create_info, nullptr, &instance_));
}

}  // namespace nengine
