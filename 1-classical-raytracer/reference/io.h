#pragma once
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "glm/glm.hpp"

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