#pragma once

#include "glm/glm.hpp"

// linear RGB to sRGB conversion
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

    clear();
  }

  ~Image() { delete[] m_pixels; }

  // get const pointer of image
  const float* getConstPtr() const { return m_pixels; }

  // get color of (i, j)
  glm::vec3 getPixel(int i, int j) const
  {
    const int idx = 3 * i + 3 * m_width * j;
    return glm::vec3(m_pixels[idx], m_pixels[idx + 1], m_pixels[idx + 2]);
  }

  // set c to (i, j)
  void setPixel(int i, int j, const glm::vec3& c)
  {
    const int idx = 3 * i + 3 * m_width * j;
    m_pixels[idx] = c.x;
    m_pixels[idx + 1] = c.y;
    m_pixels[idx + 2] = c.z;
  }

  // add c to (i, j)
  void addPixel(int i, int j, const glm::vec3& c)
  {
    const int idx = 3 * i + 3 * m_width * j;
    m_pixels[idx] += c.x;
    m_pixels[idx + 1] += c.y;
    m_pixels[idx + 2] += c.z;
  }

  // clear image
  void clear()
  {
    for (int j = 0; j < m_height; ++j) {
      for (int i = 0; i < m_width; ++i) {
        const int idx = 3 * i + 3 * m_width * j;
        m_pixels[idx] = 0.0f;
        m_pixels[idx + 1] = 0.0f;
        m_pixels[idx + 2] = 0.0f;
      }
    }
  }

  // divide all pixels by k
  void divide(float k)
  {
    for (int j = 0; j < m_height; ++j) {
      for (int i = 0; i < m_width; ++i) {
        const int idx = 3 * i + 3 * m_width * j;
        m_pixels[idx] /= k;
        m_pixels[idx + 1] /= k;
        m_pixels[idx + 2] /= k;
      }
    }
  }

  // apply linear RGB to sRGB conversion
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
  int m_width;      // width of image
  int m_height;     // height of image
  float* m_pixels;  // image data
};