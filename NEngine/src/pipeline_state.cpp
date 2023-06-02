#include "pipeline_state.h"

#include "utils.h"
#include "vulkan_utils.h"
#include "misc.h"

namespace NEngine {
PipelineState::PipelineState(VkDevice device,
                             const PipelineStateCreateInfo &createInfo)
{
    CreatePipeline(device, createInfo);
}
void
PipelineState::CreatePipeline(VkDevice device,
                              const PipelineStateCreateInfo &createInfo)
{
    using namespace Utils;
    using namespace VulkanUtils;

    const std::vector<char> vs_code =
        ReadFile(ResolveShaderPath(createInfo.vertexShader.c_str()));
    const std::vector<char> ps_code =
        ReadFile(ResolveShaderPath(createInfo.pixelShader.c_str()));

    const VkShaderModule vsm = CreateShaderModule(device, vs_code);
    const VkShaderModule psm = CreateShaderModule(device, ps_code);

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
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0;
    rasterizer.depthBiasClamp = 0;
    rasterizer.depthBiasSlopeFactor = 0;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = createInfo.msaaSamples;
    multisampling.sampleShadingEnable = VK_TRUE;
    multisampling.minSampleShading = 0.2f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &createInfo.descriptorSetLayout;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    VKRESULT(vkCreatePipelineLayout(
        device, &pipeline_layout_info, nullptr, &m_pipelineLayout));

    VkGraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.sType =
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.stageCount = 2;
    pipeline_create_info.pStages = shader_stages;
    pipeline_create_info.pVertexInputState = &createInfo.inputState;
    pipeline_create_info.pInputAssemblyState = &createInfo.inputAssemblyState;
    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pRasterizationState = &rasterizer;
    pipeline_create_info.pMultisampleState = &multisampling;
    pipeline_create_info.pDepthStencilState = &createInfo.depthStencilState;
    pipeline_create_info.pColorBlendState = &createInfo.blendState;
    pipeline_create_info.pDynamicState = &dynamic_state;
    pipeline_create_info.layout = m_pipelineLayout;
    pipeline_create_info.renderPass = m_renderPass;
    pipeline_create_info.subpass = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = -1;

    VKRESULT(vkCreateGraphicsPipelines(device,
                                       VK_NULL_HANDLE,
                                       1,
                                       &pipeline_create_info,
                                       nullptr,
                                       &m_pipeline));

    vkDestroyShaderModule(device, vsm, nullptr);
    vkDestroyShaderModule(device, psm, nullptr);
}
}  // namespace NEngine