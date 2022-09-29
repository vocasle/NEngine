#include <cassert>
#include <cstdint>
#include <d3d11.h>
#include <vector>
#include <wrl/client.h>

enum class VertexItemType {
	Float = sizeof(float),
	Float2 = sizeof(float) * 2,
	Float3 = sizeof(float) * 3,
	Float4 = sizeof(float) * 4,
};

class VertexBufferDesc {
    public:
	void AddItem(VertexItemType inItemType)
	{
		mItems.push_back(inItemType);
	}

	const std::vector<VertexItemType> &GetItems() const
	{
		return mItems;
	}

    private:
	std::vector<VertexItemType> mItems;
};

class VertexBuffer {
    public:
	VertexBuffer(const VertexBufferDesc &inDesc)
		: mDesc(inDesc)
	{
	}

	ID3D11Buffer *GetBuffer() const
	{
		return mBuffer.Get();
	}
	void SetData(const uint8_t *data, const size_t size)
	{
		size_t bytes = 0;
		for (const auto &itemType : mDesc.GetItems()) {
			bytes += static_cast<size_t>(itemType);
		}
		assert(size % bytes == 0 && "data size is not valid");

		mBytes.insert(std::end(mBytes), data, data + size);
	}

    private:
	std::vector<uint8_t> mBytes;
	VertexBufferDesc mDesc;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mBuffer;
};