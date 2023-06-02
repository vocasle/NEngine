#include "vulkan_utils.h"

#include "misc.h"

namespace NEngine::VulkanUtils {
VkShaderModule
CreateShaderModule(VkDevice device, const std::vector<char> &code)
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shader_module;
    VKRESULT(
        vkCreateShaderModule(device, &create_info, nullptr, &shader_module));

    return shader_module;
}
}  // namespace NEngine::VulkanUtils