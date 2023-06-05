#include "geometry.h"

namespace NEngine::Geometry {
void
CreatePlane(std::vector<vertex>   &vertices,
            std::vector<uint32_t> &indices,
            float                  width,
            float                  height,
            const glm::vec3       &center,
            Axis                   axis)
{
    const float halfWidth = width * 0.5f;
    const float halfHeight = height * 0.5f;


}
void
CreatePlane(std::vector<vertex>   &vertices,
            std::vector<uint32_t> &indices,
            float                  width,
            float                  height)
{
    CreatePlane(vertices, indices, width, height, glm::vec3(0.0f), Axis::Y);
}
}  // namespace NEngine::Geometry