#include "NEngine/Helpers/GLTFLoader.h"
#include "NEngine/Utils/Utils.h"


using namespace NEngine::Utils;

tinygltf::Model
NEngine::Helpers::GLTFLoader::Load(const std::string &path)
{
    tinygltf::Model model;

    const bool ret = m_loader.LoadASCIIFromFile(
        &model,
        &m_err,
        &m_warn,
        path);

    if (!m_err.empty()) {
        UtilsDebugPrint("ERROR: Failed to load model: %s\n", m_err.c_str());
    }
    if (!m_warn.empty()) {
        UtilsDebugPrint("WARN: %s\n", m_warn.c_str());
    }
    if (!ret) {
        UtilsDebugPrint("ERROR: Failed to parse gLTF\n");
    }

    return model;
}
