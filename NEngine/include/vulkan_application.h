#pragma once
#include <SDL.h>

#include <memory>
#include <vulkan/vulkan.hpp>

#include "camera.h"
#include "image.h"
#include "shader_binding.h"

namespace NEngine {
struct vertex;

class VulkanApplication
{
public:
    VulkanApplication(VulkanApplication &&) = delete;
    VulkanApplication(const VulkanApplication &) = delete;
    explicit VulkanApplication(SDL_Window *window);
    void DrawFrame();
    void OnWindowResized();
    ~VulkanApplication();
    void LoadModel(const std::string &path);
    void OnMouseMove(uint32_t mouse_state, int x, int y);

private:
    void CreateCommandPool();
    void InitVulkan();
    void Cleanup() const;
    void SetupDebugMessenger();
    void CreateInstance();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSurface();
    [[nodiscard]] bool IsDeviceSuitable(VkPhysicalDevice device) const;
    [[nodiscard]] bool CheckDeviceExtensionSupport(
        VkPhysicalDevice device) const;
    void CreateSwapchain();
    void CreateImageView();
    void CreateGraphicsPipeline();
    void CreateRenderPass();
    void CreateFramebuffers();
    void CreateCommandBuffers();
    void RecordCommandBuffer(VkCommandBuffer cb, uint32_t image_idx) const;
    void CreateSyncObjects();
    void RecreateSwapChain();
    void CleanupSwapChain() const;
    void CreateVertexBuffer();
    void CreateBuffer(VkDeviceSize size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &buffer_memory) const;
    void CopyBuffer(VkBuffer src_buffer,
                    VkBuffer dst_buffer,
                    VkDeviceSize size);
    void CreateIndexBuffer();
    void CreateDescriptorSetLayout();
    void CreateUniformBuffers();
    void UpdateUniformBuffer() const;
    void CreateDescriptorPool();
    void CreateDescriptorSets();
    void CreateTextureImage(const std::string &texture_path);
    void CreateImage(uint32_t width,
                     uint32_t height,
                     VkFormat format,
                     uint32_t mip_levels,
                     VkSampleCountFlagBits num_samples,
                     VkImageTiling tiling,
                     VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VkImage &image,
                     VkDeviceMemory &image_memory) const;
    void CreateTextureImageView();
    VkImageView CreateImageView(VkImage image,
                                VkFormat format,
                                VkImageAspectFlags aspect_flags,
                                uint32_t mip_levels) const;
    void CreateTextureSampler();
    void CreateDepthResources();
    void CreateColorResources();
    void InitImGui();
    void DestroyImGui() const;

    SDL_Window *window_;
    VkInstance instance_{};
    VkDebugUtilsMessengerEXT debug_util_messenger_{};
    VkPhysicalDevice physical_device_{};
    VkDevice device_{};
    VkQueue queue_{};
    VkSurfaceKHR surface_{};
    VkQueue present_queue_{};
    VkSwapchainKHR swap_chain_{};
    std::vector<VkImage> swap_chain_images_;
    VkFormat swap_chain_image_format_{};
    VkExtent2D swap_chain_extent_{};
    std::vector<VkImageView> swap_chain_image_views_;
    VkDescriptorSetLayout descriptor_set_layout_{};
    VkPipelineLayout pipeline_layout_{};
    VkRenderPass render_pass_{};
    VkPipeline graphics_pipeline_{};
    std::vector<VkFramebuffer> swap_chain_framebuffers_;
    VkCommandPool command_pool_{};
    std::vector<VkCommandBuffer> command_buffers_;
    std::vector<VkSemaphore> image_available_semaphores_;
    std::vector<VkSemaphore> render_finished_semaphores_;
    std::vector<VkFence> in_flight_fences_;
    uint32_t current_frame_ = 0;
    bool is_framebuffer_resized = false;
    VkBuffer vertex_buffer_{};
    VkDeviceMemory vertex_buffer_memory_{};
    VkQueue transfer_queue_{};
    VkCommandPool transfer_command_pool_{};
    VkBuffer index_buffer_{};
    VkDeviceMemory index_buffer_memory_{};
    std::vector<VkBuffer> uniform_buffers_;
    std::vector<VkDeviceMemory> uniform_buffers_memory_;
    std::vector<void *> uniform_buffers_mapped_;
    std::vector<VkBuffer> uniform_buffers_ps_;
    std::vector<VkDeviceMemory> uniform_buffers_memory_ps_;
    std::vector<void *> uniform_buffers_mapped_ps_;
    VkDescriptorPool descriptor_pool_{};
    std::vector<VkDescriptorSet> descriptor_sets_;
    uint32_t mip_levels_ = 0;
    VkSampler texture_sampler_{};
    VkSampleCountFlagBits msaa_samples_ = VK_SAMPLE_COUNT_1_BIT;
    VkImage color_image_{};
    VkDeviceMemory color_image_memory_{};
    VkImageView color_image_view_{};
    VkDescriptorPool imgui_pool_{};

    std::vector<vertex> vertices_;
    std::vector<uint32_t> indices_;

    const std::vector<const char *> validation_layers = {
        "VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    std::unique_ptr<Camera> camera_;
    std::unique_ptr<Image> m_depthImage;
    std::unique_ptr<Image> m_textureImage;
};

}  // namespace NEngine
