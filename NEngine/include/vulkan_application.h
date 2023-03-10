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
    void cleanup();
    void setup_debug_messenger();
    void create_instance();
    void pick_physical_device();
    void create_logical_device();
    void create_surface();
    [[nodiscard]] bool is_device_suitable(VkPhysicalDevice device) const;
    [[nodiscard]] bool check_device_extension_support(
        VkPhysicalDevice device) const;
    void create_swap_chain();
    void create_image_views();
    void create_graphics_pipeline();
    [[nodiscard]] VkShaderModule create_shader_module(
        const std::vector<char> &code) const;

    SDL_Window *window_;
    VkInstance instance_;
    VkDebugUtilsMessengerEXT debug_util_messenger_;
    VkPhysicalDevice physical_device_;
    VkDevice device_;
    VkQueue queue_;
    VkSurfaceKHR surface_;
    VkQueue present_queue_;
    VkSwapchainKHR swap_chain_;
    std::vector<VkImage> swap_chain_images_;
    VkFormat swap_chain_image_format_;
    VkExtent2D swap_chain_extent_;
    std::vector<VkImageView> swap_chain_image_views_;

    const std::vector<const char *> validation_layers = {
        "VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}  // namespace nengine