#include <memory>

#include "camera.h"
#include "core.h"
#include "image.h"
#include "intersector.h"
#include "io.h"
#include "primitive.h"

int main()
{
  const int width = 512;
  const int height = 512;

  Image image(width, height);
  Camera camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, -1));

  const auto sphere = std::make_shared<Sphere>(glm::vec3(0), 1.0f);
  const auto material = std::make_shared<Material>(glm::vec3(0.8f));

  std::vector<std::shared_ptr<Primitive>> primitives;
  primitives.push_back(std::make_shared<Primitive>(sphere, material));

  LinearIntersector intersector(primitives);

  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      const glm::vec2 ndc =
          glm::vec2((2.0f * i - width) / height, (2.0f * j - height) / height);
      const Ray ray = camera.sampleRay(ndc);

      IntersectInfo info;
      if (intersector.intersect(ray, info)) {
        image.setPixel(i, j, 0.5f * (info.normal + 1.0f));
      } else {
        image.setPixel(i, j, glm::vec3(0.0f));
      }
    }
  }

  image.post_process();
  write_png("output.png", width, height, image.getConstPtr());

  return 0;
}