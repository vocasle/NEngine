#include <d3d11.h>
#include <wrl/client.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "InputLayout.h"
#include "NEngine/Helpers/DeviceResources.h"

namespace NEngine {
namespace Helpers {
struct ShaderDefine
{
    ShaderDefine() = default;
    ShaderDefine(const std::string &name, const std::string &value)
        : Name(name),
          Value(value)
    {
    }
    std::string Name;
    std::string Value;
};

class ShaderManager
{
public:
    void Initialize(ID3D11Device *device,
                    const std::string &shaderBlobRootDir,
                    const std::string &shaderSrcRootDir);
    size_t GetStrides() const;
    ID3D11InputLayout *GetInputLayout() const;
    ID3D11VertexShader *GetVertexShader(const std::string &name);
    ID3D11PixelShader *GetPixelShader(const std::string &name) const;
    void UpdateVertexShader(const std::string &name,
                            ID3D11VertexShader *shader);
    void UpdatePixelShader(const std::string &name, ID3D11PixelShader *shader);
    void Recompile(ID3D11Device *device);

    static void RecompileShaders(Helpers::DeviceResources &deviceResources);
    static void RecompileShaders(Helpers::DeviceResources &deviceResources,
                                 const std::vector<ShaderDefine> &defines);

private:
    void CreateVertexShader(const std::string &filepath, ID3D11Device *device);
    void CreatePixelShader(const std::string &filepath, ID3D11Device *device);

private:
    std::string m_activeVS;

    typedef std::unordered_map<std::string,
                               Microsoft::WRL::ComPtr<ID3D11VertexShader>>
        VertexShaderMap;
    typedef std::unordered_map<std::string,
                               Microsoft::WRL::ComPtr<ID3D11PixelShader>>
        PixelShaderMap;
    VertexShaderMap m_vertexShaders;
    PixelShaderMap m_pixelShaders;
    typedef std::unordered_map<std::string, InputLayout> InputLayoutMap;
    InputLayoutMap m_inputLayouts;
    std::string m_shaderBlobRootDir;
    std::string m_shaderSrcRootDir;
};

}  // namespace Helpers
}  // namespace NEngine
