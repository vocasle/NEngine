#include <cstdint>
#include <string>

#include "NEngine/Utils/Utils.h"
#include "stb_image/stb_image.h"

namespace NEngine {
namespace Utils {
class Image
{
public:
    Image(std::vector<unsigned char> data,
          int width,
          int height,
          int channels,
          int bits)
        : m_bytes(std::move(data)),
          m_width(width),
          m_height(height),
          m_channels(channels)
    {
    }
    explicit Image(const std::string &filepath)
        : m_bytes{},
          m_width{0},
          m_height{0},
          m_channels{0}
    {
        auto bytes = stbi_load(filepath.c_str(),
                               &m_width,
                               &m_height,
                               &m_channels,
                               DESIRED_CHANNELS);

        if (!bytes) {
            UtilsFatalError("Failed to load image from %s\n", filepath.c_str());
            return;
        }

        const size_t sz = static_cast<size_t>(m_width) *
                          static_cast<size_t>(m_height) *
                          static_cast<size_t>(m_channels);
        m_bytes.resize(sz);
        memcpy(&m_bytes[0], bytes, sz);
        stbi_image_free(bytes);
    }

    ~Image()
    {
    }

    bool
    IsValid() const
    {
        return m_bytes.size() > 0;
    }

    uint32_t
    GetWidth() const
    {
        return static_cast<uint32_t>(m_width);
    }
    uint32_t
    GetHeight() const
    {
        return static_cast<uint32_t>(m_height);
    }
    uint32_t
    GetChannels() const
    {
        return static_cast<uint32_t>(m_channels);
    }
    const unsigned char *
    GetBytes() const
    {
        return &m_bytes[0];
    }

    static const int DESIRED_CHANNELS = 4;  // for RGBA

private:
    std::vector<unsigned char> m_bytes;
    int m_width;
    int m_height;
    int m_channels;
};

}  // namespace Utils
}  // namespace NEngine