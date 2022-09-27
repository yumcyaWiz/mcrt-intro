#pragma once
#include <cstdint>
#include <filesystem>
#include <stdexcept>

#include "glm/glm.hpp"
#include "spdlog/spdlog.h"
#include "stb_image.h"

class Texture
{
 public:
  Texture() {}

  Texture(const std::filesystem::path& filepath)
  {
    const std::string extension = filepath.extension();
    if (extension == ".jpg" || extension == ".png") {
      loadLDR(filepath);
    } else if (extension == ".hdr") {
      loadHDR(filepath);
    } else {
      throw std::runtime_error("unsupported extension");
    }
  }

  // fetch texture
  glm::vec4 fetch(const glm::vec2& texcoord) const
  {
    const int i = (m_width - 1) * glm::clamp(texcoord.x, 0.0f, 1.0f);
    const int j = (m_height - 1) * glm::clamp(texcoord.y, 0.0f, 1.0f);
    return m_data[i + m_width * j];
  }

 private:
  int m_width;                    // width of texture
  int m_height;                   // height of texture
  std::vector<glm::vec4> m_data;  // texture image data(RGBA)

  // load jpeg, png image
  void loadLDR(const std::filesystem::path& filepath)
  {
    spdlog::info("[Texture] loading {}", filepath.generic_string());

    stbi_set_flip_vertically_on_load(true);

    // load image with stb image
    int c;
    unsigned char* img =
        stbi_load(filepath.c_str(), &m_width, &m_height, &c, STBI_rgb_alpha);
    if (!img) {
      spdlog::error("{}", stbi_failure_reason());
      throw std::runtime_error("failed to load " + filepath.generic_string());
    }

    // convert from u8 to f32
    m_data.resize(m_width * m_height);
    for (int j = 0; j < m_height; ++j) {
      for (int i = 0; i < m_width; ++i) {
        const int idx = 4 * i + 4 * m_width * j;
        m_data[i + m_width * j] =
            glm::vec4(img[idx + 0] / 255.0f, img[idx + 1] / 255.0f,
                      img[idx + 2] / 255.0f, img[idx + 3] / 255.0f);
      }
    }

    stbi_image_free(img);
  }

  // load hdr image
  void loadHDR(const std::filesystem::path& filepath)
  {
    spdlog::info("[Texture] loading {}", filepath.generic_string());

    stbi_set_flip_vertically_on_load(true);

    // load image with stb image
    int c;
    float* img =
        stbi_loadf(filepath.c_str(), &m_width, &m_height, &c, STBI_rgb_alpha);
    if (!img) {
      spdlog::error("{}", stbi_failure_reason());
      throw std::runtime_error("failed to load " + filepath.generic_string());
    }

    m_data.resize(m_width * m_height);
    for (int j = 0; j < m_height; ++j) {
      for (int i = 0; i < m_width; ++i) {
        const int idx = 4 * i + 4 * m_width * j;
        m_data[i + m_width * j] =
            glm::vec4(img[idx + 0], img[idx + 1], img[idx + 2], img[idx + 3]);
      }
    }

    stbi_image_free(img);
  }
};