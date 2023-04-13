#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace nengine {
struct Camera
{
    glm::mat4x4 view;
    glm::vec3 cam_pos;
    glm::vec2 mouse_pos;
    float radius;
    float pitch;
    float yaw;

    static constexpr float DEFAULT_RADIUS = 3.0f;

    Camera(uint32_t width, uint32_t height);
    void Update();
    void UpdateMousePos(uint32_t state, const glm::vec2 &current_mouse_pos);
};
}  // namespace nengine
