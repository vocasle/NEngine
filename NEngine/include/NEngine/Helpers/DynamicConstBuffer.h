#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cassert>
#include <functional>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#include "DeviceResources.h"
#include "NEngine/Renderer/Bindable.h"
#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Helpers {
using namespace Utils;

enum class NodeType {
    Struct,
    Array,
    Bool = sizeof(float),
    Float = sizeof(float),
    Float2 = sizeof(float) * 2,
    Float3 = sizeof(float) * 3,
    Float4 = sizeof(float) * 4,
    Float3X3 = sizeof(float) * 3 * 3,
    Float4X4 = sizeof(float) * 4 * 4,
    None,
};

std::string NodeTypeToString(NodeType inType);

struct Node
{
    std::string Name;
    NodeType Type;
    std::vector<Node> Children;

    Node()
        : Node("", NodeType::None)
    {
    }

    Node(const std::string &inName, NodeType inType)
        : Name(inName),
          Type(inType)
    {
    }

    void
    AddChild(const std::string &inName, NodeType inType)
    {
        assert(Type == NodeType::Array ||
               Type == NodeType::Struct &&
                   "Only composite types allowed to have children");
        Node n;
        n.Name = Type == NodeType::Struct
                     ? UtilsFormatStr("%s.%s", Name.c_str(), inName.c_str())
                     : UtilsFormatStr("s[%lu]", Name.c_str(), Children.size());
        n.Type = inType;
        Children.push_back(n);
    }

    void
    AddChildN(const Node &child, int repeat)
    {
        assert(Type == NodeType::Array &&
               "Only array type allowed to use this function");
        Node n = Node(UtilsFormatStr("%s[%lu]", Name.c_str(), Children.size()),
                      child.Type);
        n.Children = child.Children;
        for (int i = 0; i < repeat; ++i) {
            n.Name = UtilsFormatStr("%s[%lu]", Name.c_str(), Children.size());
            for (Node &node : n.Children) {
                const size_t pos = node.Name.find('.');
                if (pos != std::string::npos) {
                    node.Name = n.Name + node.Name.substr(pos);
                }
            }
            Children.push_back(n);
        }
    }

    void
    Print() const
    {
        UtilsDebugPrint("{ Name: %s, Type: %s }\n",
                        Name.c_str(),
                        NodeTypeToString(Type).c_str());
        for (const auto &child : Children) {
            child.Print();
        }
    }

    void
    Visit(const std::function<void(const Node &node)> &inVisitor) const
    {
        inVisitor(*this);
        for (auto &child : Children) {
            child.Visit(inVisitor);
        }
    }
};

class DynamicConstBufferDesc
{
public:
    DynamicConstBufferDesc()
        : mBindSlot(0)
    {
    }

    void
    SetBindSlot(unsigned int bindSlot)
    {
        mBindSlot = bindSlot;
    }

    void
    AddNode(const Node &inNode)
    {
        mNodes.push_back(inNode);
    }

    void
    Print() const
    {
        for (const auto &node : mNodes) {
            node.Print();
        }
    }

    unsigned int
    GetBindSlot() const
    {
        return mBindSlot;
    }

    const std::vector<Node> &
    GetNodes() const
    {
        return mNodes;
    }

private:
    std::vector<Node> mNodes;
    unsigned int mBindSlot;
};

class DynamicConstBuffer : public NEngine::Renderer::Bindable
{
public:
    DynamicConstBuffer(const DynamicConstBufferDesc &desc,
                       DeviceResources &deviceResources);

    virtual void Bind(Helpers::DeviceResources &deviceResources) const override;
    virtual void Unbind(
        Helpers::DeviceResources &deviceResources) const override;

    [[nodiscard]] ID3D11Buffer *Get() const;

    template <typename T, typename S>
    T *
    operator[](const S &inName)
    {
        for (const auto &[key, value] : mValues) {
            if (key == inName) {
                return static_cast<T *>(value.Ptr);
            }
        }
        return nullptr;
    }

    template <typename T, typename S>
    T *
    GetValue(const S &inName) const
    {
        for (const auto &[key, value] : mValues) {
            if (key == inName) {
                return static_cast<T *>(value.Ptr);
            }
        }
        return nullptr;
    }

    template <typename T, typename S>
    void
    SetValue(const S &inName, const T &inValue)
    {
        bool isSet = false;
        for (const auto &[key, value] : mValues) {
            if (key == inName) {
                *static_cast<T *>(value.Ptr) = inValue;
                isSet = true;
                break;
            }
        }

        if (!isSet) {
            UtilsFormatStr("WARN: key %s does not exist in this cbuffer\n",
                           inName);
        }
    }

    template <typename T, typename S>
    void
    set_array_value(const S &name, const std::vector<T> &in_value)
    {
        bool is_set = false;
        for (const auto &[key, value] : mValues) {
            if (key == name && value.Type == NodeType::Array) {
                *static_cast<std::vector<T> *>(value.Ptr) = in_value;
                is_set = true;
                break;
            }
        }

        if (!is_set) {
            UtilsFormatStr("WARN: key %s does not exist in this cbuffer\n",
                           name);
        }
    }

    void UpdateConstantBuffer() const;
    void CreateConstantBuffer();
    [[nodiscard]] const std::vector<uint8_t> &GetBytes() const;

private:
    struct Value
    {
        NodeType Type;
        void *Ptr;
    };

    void InitializeDefaults();

    std::vector<uint8_t> mBytes;
    std::unordered_map<std::string, Value> mValues;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mBuffer;
    DeviceResources *mDeviceResources;
    unsigned int mBindSlot;
};

}  // namespace Helpers
}  // namespace NEngine
