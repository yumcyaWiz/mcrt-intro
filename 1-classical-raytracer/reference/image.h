#pragma once

#include "glm/glm.hpp"

inline glm::vec3 linear_to_srgb(const glm::vec3& rgb)
{
  glm::vec3 ret;
  ret.x = rgb.x < 0.0031308 ? 12.92 * rgb.x
                            : 1.055 * glm::pow(rgb.x, 1.0f / 2.4f) - 0.055;
  ret.y = rgb.y < 0.0031308 ? 12.92 * rgb.y
                            : 1.055 * glm::pow(rgb.y, 1.0f / 2.4f) - 0.055;
  ret.z = rgb.z < 0.0031308 ? 12.92 * rgb.z
                            : 1.055 * glm::pow(rgb.z, 1.0f / 2.4f) - 0.055;
  return ret;
}

class Image
{
 public:
  Image(int width, int height) : m_width(width), m_height(height)
  {
    m_pixels = new float[3 * m_width * m_height];
  }

  ~Image() { delete[] m_pixels; }

  const float* getConstPtr() const { return m_pixels; }

  glm::vec3 getPixel(int i, int j) const
  {
    const int idx = 3 * i + 3 * m_width * j;
    return glm::vec3(m_pixels[idx], m_pixels[idx + 1], m_pixels[idx + 2]);
  }

  void setPixel(int i, int j, const glm::vec3& c)
  {
    const int idx = 3 * i + 3 * m_width * j;
    m_pixels[idx] = c.x;
    m_pixels[idx + 1] = c.y;
    m_pixels[idx + 2] = c.z;
  }

  void post_process()
  {
    for (int j = 0; j < m_height; ++j) {
      for (int i = 0; i < m_width; ++i) {
        const int idx = 3 * i + 3 * m_width * j;
        glm::vec3 c =
            glm::vec3(m_pixels[idx], m_pixels[idx + 1], m_pixels[idx + 2]);

        c = linear_to_srgb(c);

        m_pixels[idx] = c.x;
        m_pixels[idx + 1] = c.y;
        m_pixels[idx + 2] = c.z;
      }
    }
  }

 private:
  int m_width;
  int m_height;
  float* m_pixels;
};