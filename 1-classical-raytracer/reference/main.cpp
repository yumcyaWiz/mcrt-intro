#include "image.h"
#include "io.h"

int main()
{
  const int width = 512;
  const int height = 512;

  Image image(width, height);
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      const glm::vec2 uv = glm::vec2(static_cast<float>(i) / width,
                                     static_cast<float>(j) / height);
      image.setPixel(i, j, glm::vec3(uv.x, uv.y, 1.0f));
    }
  }

  // write_ppm("output.ppm", width, height, image.getConstPtr());
  image.post_process();
  write_png("output.png", width, height, image.getConstPtr());

  return 0;
}