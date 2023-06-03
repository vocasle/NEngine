#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace NEngine {
struct BindingDesc
{
    uint32_t binding;
    uint32_t descriptorCount;
    VkDescriptorType descriptorType;
    VkShaderStageFlagBits stageFlags;
};

class ShaderBinding
{
public:
    ShaderBinding(VkDevice device,
                  const std::vector<BindingDesc> &bindingDescs);

private:
    VkDescriptorSetLayout m_descriptorSetLayout;
};
}  // namespace NEngine