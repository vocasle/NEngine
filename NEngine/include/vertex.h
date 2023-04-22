#define GLM_ENABLE_EXPERIMENTAL
#include <vulkan/vulkan.hpp>

#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <utility>

namespace NEngine {
struct vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 tex_coord;
    glm::vec3 normal;

    static constexpr VkVertexInputBindingDescription
    get_binding_description()
    {
        VkVertexInputBindingDescription description{};
        description.stride = sizeof(vertex);
        description.binding = 0;
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return description;
    }

    static std::array<VkVertexInputAttributeDescription, 4>
    get_attribute_descriptions()
    {
        std::array<VkVertexInputAttributeDescription, 4>
            attribute_descriptions{};
        attribute_descriptions[0].offset =
            static_cast<uint32_t>(offsetof(vertex, pos));
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;

        attribute_descriptions[1].offset =
            static_cast<uint32_t>(offsetof(vertex, color));
        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;

        attribute_descriptions[2].offset =
            static_cast<uint32_t>(offsetof(vertex, tex_coord));
        attribute_descriptions[2].binding = 0;
        attribute_descriptions[2].location = 2;
        attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;

        attribute_descriptions[3].offset =
            static_cast<uint32_t>(offsetof(vertex, normal));
        attribute_descriptions[3].binding = 0;
        attribute_descriptions[3].location = 3;
        attribute_descriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;

        return attribute_descriptions;
    }

    bool
    operator==(const vertex &other) const
    {
        return pos == other.pos && color == other.color &&
               tex_coord == other.tex_coord;
    }
};

}  // namespace NEngine

template <>
struct std::hash<NEngine::vertex>
{
    size_t
    operator()(const NEngine::vertex &v) const noexcept
    {
        return ((hash<glm::vec3>()(v.pos) ^
                 (hash<glm::vec3>()(v.color) << 1)) >>
                1) ^
               (hash<glm::vec2>()(v.tex_coord) << 1);
    }
};
