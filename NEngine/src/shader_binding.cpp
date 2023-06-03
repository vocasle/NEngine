#include "shader_binding.h"

#include "misc.h"

namespace NEngine {
ShaderBinding::ShaderBinding(VkDevice device,
                             const std::vector<BindingDesc> &bindingDescs)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(bindingDescs.size());

    for (auto &bd : bindingDescs) {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = bd.binding;
        binding.descriptorCount = bd.descriptorCount;
        binding.descriptorType = bd.descriptorType;
        binding.stageFlags = bd.stageFlags;
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(std::move(binding));
    }

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = static_cast<uint32_t>(bindings.size());
    info.pBindings = bindings.data();

    VKRESULT(vkCreateDescriptorSetLayout(
        device, &info, nullptr, &m_descriptorSetLayout));
}
}  // namespace NEngine