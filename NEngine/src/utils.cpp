#include "utils.h"

#include <fstream>
#include <sstream>

namespace NEngine::Utils {
std::vector<char>
ReadFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios_base::ate | std::ios_base::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    const size_t file_size = file.tellg();
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), static_cast<long long>(file_size));
    file.close();

    return buffer;
}

std::string
ResolveShaderPath(const char *path)
{
    std::ostringstream out;
    out << SHADERS_HOME_DIR << "/" << path;
    return out.str();
}
}  // namespace NEngine::Utils