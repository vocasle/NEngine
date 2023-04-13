#include "camera.h"

#include <glm/ext.hpp>

namespace nengine {
Camera::Camera(uint32_t width, uint32_t height)
    : view(1.0f),
      cam_pos(0.0f),
      mouse_pos(width / 2.0f, height / 2.0f),
      radius(DEFAULT_RADIUS),
      pitch(glm::radians(45.0f)),
      yaw(glm::radians(90.0f))
{
    Update();
}
void
Camera::Update()
{
    // Convert Spherical to Cartesian coordinates.
    float x = radius * std::sinf(pitch) * std::cosf(yaw);
    float z = radius * std::sinf(pitch) * std::sinf(yaw);
    const float y = radius * std::cosf(pitch);
    cam_pos = {x, y, z};
    // Build the view matrix.
    const glm::vec3 pos = glm::vec3(x, y, z);
    const glm::vec3 target = glm::vec3(0.0f);
    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(pos, target, up);
}
}  // namespace nengine