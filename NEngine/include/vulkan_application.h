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

    void create_instance();

    VkInstance instance_;
    SDL_Window *window_;

    const std::vector<const char *> validation_layers = {
        "VK_LAYER_KHRONOS_validation"};
};

}  // namespace nengine