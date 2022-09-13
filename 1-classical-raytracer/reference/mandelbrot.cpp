#include "camera.h"
#include "image.h"
#include "io.h"

glm::vec2 cmult(const glm::vec2& z0, const glm::vec2& z1)
{
  return glm::vec2(z0.x * z1.x - z0.y * z1.y, z0.x * z1.y + z0.y * z1.x);
}

int main()
{
  const int width = 512;
  const int height = 512;
  const int n_iterations = 100;

  Image image(width, height);

  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      glm::vec2 ndc =
          glm::vec2((2.0f * i - width) / height, (2.0f * j - height) / height);

      bool diverged = false;
      const glm::vec2 c = ndc;
      glm::vec2 z(0, 0);
      for (int k = 0; k < n_iterations; ++k) {
        z = cmult(z, z) + c;
        if (glm::length(z) > 2.0f) {
          diverged = true;
          break;
        }
      }

      if (!diverged) {
        image.setPixel(i, j, glm::vec3(1.0f));
      } else {
        image.setPixel(i, j, glm::vec3(0.0f));
      }
    }
  }

  write_png("output.png", width, height, image.getConstPtr());

  return 0;
}