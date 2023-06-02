#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

namespace NEngine {
struct PipelineStateCreateInfo
{
    const std::string pixelShader;
    const std::string vertexShader;
    VkPipelineMultisampleStateCreateInfo multisampleState;
    VkPipelineVertexInputStateCreateInfo inputState;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
    VkPipelineRasterizationStateCreateInfo rasterizerState;
    VkPipelineColorBlendAttachmentState blendAttachmentState;
    VkPipelineColorBlendStateCreateInfo blendState;
    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    VkViewport viewport;
    VkRect2D scissor;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    // TODO: Move this out to separate *CreateInfo object
    VkDescriptorSetLayout descriptorSetLayout;
};

class PipelineState
{
public:
    PipelineState(VkDevice device, const PipelineStateCreateInfo &createInfo);

private:
    void CreatePipeline(VkDevice device,
                        const PipelineStateCreateInfo &createInfo);

    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;
    VkRenderPass m_renderPass;
};
}  // namespace NEngine