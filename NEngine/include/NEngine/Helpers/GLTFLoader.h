#pragma once
#include <string>

#include "tinygltf/tiny_gltf.h"

namespace NEngine {
namespace Helpers {
class GLTFLoader
{
public:
    tinygltf::Model Load(const std::string& path);

private:
    tinygltf::TinyGLTF m_loader;
    std::string m_err;
    std::string m_warn;
};
}
}
