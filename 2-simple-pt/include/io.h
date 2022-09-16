#pragma once
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "stb_image_write.h"

inline void write_ppm(const std::string& filename, int width, int height,
                      const float* image)
{
  std::ofstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("failed to open " + filename);
  }

  file << "P3" << std::endl;
  file << width << " " << height << std::endl;
  file << "255" << std::endl;
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      const int idx = 3 * i + 3 * width * j;
      const float R = image[idx];
      const float G = image[idx + 1];
      const float B = image[idx + 2];
      file << glm::clamp(static_cast<int>(255.0f * R), 0, 255) << " "
           << glm::clamp(static_cast<int>(255.0f * G), 0, 255) << " "
           << glm::clamp(static_cast<int>(255.0f * B), 0, 255) << std::endl;
    }
  }

  file.close();
}

inline void write_png(const std::string& filename, int width, int height,
                      const float* image)
{
  // convert float to unsigned char
  std::vector<unsigned char> data(3 * width * height);
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      const int idx = 3 * i + 3 * width * j;
      data[idx] = glm::clamp(static_cast<int>(255.0f * image[idx]), 0, 255);
      data[idx + 1] =
          glm::clamp(static_cast<int>(255.0f * image[idx + 1]), 0, 255);
      data[idx + 2] =
          glm::clamp(static_cast<int>(255.0f * image[idx + 2]), 0, 255);
    }
  }

  if (!stbi_write_png(filename.c_str(), width, height, 3, data.data(),
                      3 * width * sizeof(unsigned char))) {
    std::cerr << "failed to save " << filename << std::endl;
  }
}