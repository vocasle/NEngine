#pragma once

#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

#include <Windows.h>

#include "NEngine/Math/NEMath.h"
#include "objloader.h"

namespace NEngine {
namespace Helpers {
std::unique_ptr<Mesh> MGGeneratePlane(const Math::Vec3D*origin,
                                      const float width,
                                      const float height);
std::unique_ptr<Mesh> MGCreateSphere(float radius,
                                     unsigned int rings,
                                     unsigned int sectors);

std::unique_ptr<Mesh> MGCreateCube(const Math::Vec3D &origin,
                                   const float width);
}
}
