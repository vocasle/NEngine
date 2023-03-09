#pragma once
#include <SDL.h>

#include <vulkan/vulkan.hpp>

namespace nengine {
class vulkan_application
{
public:
    explicit vulkan_application(SDL_Window *window);
    void run();
    ~vulkan_application();

private:
    void init_vulkan();
    void main_loop();
    void cleanup();
    void setup_debug_messenger();

    void create_instance();

    VkInstance instance_;
    VkDebugUtilsMessengerEXT debug_util_messenger_;
    SDL_Window *window_;

    const std::vector<const char *> validation_layers = {
        "VK_LAYER_KHRONOS_validation"};
};

}  // namespace nengine