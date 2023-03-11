#pragma once
#include <SDL.h>

#include <vulkan/vulkan.hpp>

namespace nengine {
class vulkan_application
{
public:
    explicit vulkan_application(SDL_Window *window);
    void draw_frame();
    void on_window_resized();
    void on_window_minimized();
    ~vulkan_application();

private:
    void create_command_pool();
    void init_vulkan();
    void cleanup() const;
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
    void create_render_pass();
    void create_framebuffers();
    void create_command_buffers();
    void record_command_buffer(VkCommandBuffer cb, uint32_t image_idx) const;
    void create_sync_objects();
    void recreate_swap_chain();
    void cleanup_swap_chain() const;
    void create_vertex_buffer();

    SDL_Window *window_;
    VkInstance instance_;
    VkDebugUtilsMessengerEXT debug_util_messenger_;
    VkPhysicalDevice physical_device_;
    VkDevice device_;
    VkQueue queue_;
    VkSurfaceKHR surface_{};
    VkQueue present_queue_;
    VkSwapchainKHR swap_chain_;
    std::vector<VkImage> swap_chain_images_;
    VkFormat swap_chain_image_format_;
    VkExtent2D swap_chain_extent_;
    std::vector<VkImageView> swap_chain_image_views_;
    VkPipelineLayout pipeline_layout_;
    VkRenderPass render_pass_;
    VkPipeline graphics_pipeline_;
    std::vector<VkFramebuffer> swap_chain_framebuffers_;
    VkCommandPool command_pool_;
    std::vector<VkCommandBuffer> command_buffers_;
    std::vector<VkSemaphore> image_available_semaphores_;
    std::vector<VkSemaphore> render_finished_semaphores_;
    std::vector<VkFence> in_flight_fences_;
    uint32_t current_frame_ = 0;
    bool is_framebuffer_resized = false;
    VkBuffer vertex_buffer_;
    VkDeviceMemory vertex_buffer_memory_;
    VkQueue transfer_queue_;
    VkCommandPool transfer_command_pool_{};

    const std::vector<const char *> validation_layers = {
        "VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}  // namespace nengine