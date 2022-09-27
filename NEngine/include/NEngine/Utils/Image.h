#include <cstdint>
#include <string>

#include "NEngine/Utils/Utils.h"
#include "stb_image/stb_image.h"

namespace NEngine
{
	namespace Utils
	{
		class Image {
		public:
			Image(const std::string& filepath)
				: m_bytes{ nullptr }
				, m_width{ 0 }
				, m_height{ 0 }
				, m_channels{ 0 }
			{
				m_bytes = stbi_load(filepath.c_str(), &m_width, &m_height,
					&m_channels, DESIRED_CHANNELS);

				if (!m_bytes) {
					UtilsFatalError("Failed to load image from %s\n",
						filepath.c_str());
				}
			}

			~Image()
			{
				stbi_image_free(m_bytes);
			}

			bool IsValid() const
			{
				return m_bytes != nullptr;
			}

			uint32_t GetWidth() const
			{
				return static_cast<uint32_t>(m_width);
			}
			uint32_t GetHeight() const
			{
				return static_cast<uint32_t>(m_height);
			}
			uint32_t GetChannels() const
			{
				return static_cast<uint32_t>(m_channels);
			}
			uint8_t* GetBytes() const
			{
				return m_bytes;
			}

			static const int DESIRED_CHANNELS = 4; // for RGBA

		private:
			uint8_t* m_bytes;
			int m_width;
			int m_height;
			int m_channels;
		};

	}
}