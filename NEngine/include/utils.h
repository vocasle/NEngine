#pragma once

#include <string>
#include <vector>

namespace NEngine {
namespace Utils {
std::vector<char> ReadFile(const std::string &filename);
std::string ResolveShaderPath(const char *path);
}  // namespace Utils
}  // namespace NEngine