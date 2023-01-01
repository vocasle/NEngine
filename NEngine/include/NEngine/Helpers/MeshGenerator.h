#pragma once

#include <Windows.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

#include "glm/vec3.hpp"
#include "objloader.h"

namespace NEngine {
namespace Helpers {
std::unique_ptr<Mesh> MGGeneratePlane(const glm::vec3 *origin,
                                      const float width,
                                      const float height);
std::unique_ptr<Mesh> MGCreateSphere(float radius,
                                     unsigned int rings,
                                     unsigned int sectors);

std::unique_ptr<Mesh> MGCreateCube(const glm::vec3 &origin, const float width);
}  // namespace Helpers
}  // namespace NEngine
