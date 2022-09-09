#include "camera.h"
#include "image.h"
#include "io.h"

int main()
{
  const int width = 512;
  const int height = 512;

  Image image(width, height);
  Camera camera(glm::vec3(0, 0, 5), glm::vec3(0, 0, -1));

  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      const glm::vec2 ndc =
          glm::vec2((2.0f * i - width) / height, (2.0f * j - height) / height);
      const Ray ray = camera.sampleRay(ndc);

      image.setPixel(i, j, 0.5f * (ray.direction + 1.0f));
    }
  }

  image.post_process();
  write_png("output.png", width, height, image.getConstPtr());

  return 0;
}