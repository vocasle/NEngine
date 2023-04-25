#include "camera.h"

#include <SDL.h>

#include <algorithm>
#include <glm/ext.hpp>
#include <numbers>

namespace NEngine {
Camera::Camera(uint32_t width, uint32_t height, float radius)
    : view(1.0f),
      cam_pos(0.0f),
      mouse_pos(width / 2.0f, height / 2.0f),
      radius(radius),
      pitch(glm::radians(45.0f)),
      yaw(glm::radians(90.0f))
{
    Update();
}
void
Camera::Update()
{
    // Convert Spherical to Cartesian coordinates.
    float x = radius * sinf(pitch) * cosf(yaw);
    float z = radius * sinf(pitch) * sinf(yaw);
    const float y = radius * cosf(pitch);
    cam_pos = {x, y, z};
    // Build the view matrix.
    const glm::vec3 pos = glm::vec3(x, y, z);
    const glm::vec3 target = glm::vec3(0.0f);
    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(pos, target, up);
}
void
Camera::UpdateMousePos(uint32_t state, const glm::vec2 &current_mouse_pos)
{
    if ((state & SDL_BUTTON_LMASK) != 0) {
        // Make each pixel correspond to a quarter of a degree.
        const float dx = glm::radians(
            0.25f * static_cast<float>(current_mouse_pos.x - mouse_pos.x));
        // Invert Y because I use RH view matrix
        const float dy = glm::radians(
            0.25f * static_cast<float>(mouse_pos.y - current_mouse_pos.y));
        // Update angles based on input to orbit camera around box.
        yaw += dx;
        pitch += dy;
        // Restrict the angle mPhi.
        pitch = std::clamp(pitch, 0.1f, std::numbers::pi_v<float> - 0.1f);
    }
    else if ((state & SDL_BUTTON_RMASK) != 0) {
        // Make each pixel correspond to 0.005 unit in the scene.
        float dx =
            0.005f * static_cast<float>(current_mouse_pos.x - mouse_pos.x);
        float dy =
            0.005f * static_cast<float>(current_mouse_pos.y - mouse_pos.y);
        // Update the camera radius based on input.
        radius += dx - dy;
        // Restrict the radius.
        radius = std::clamp(radius, 3.0f, 15.0f);
    }
    mouse_pos.x = current_mouse_pos.x;
    mouse_pos.y = current_mouse_pos.y;
}
}  // namespace NEngine
