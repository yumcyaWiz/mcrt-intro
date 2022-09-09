#include <memory>

#include "camera.h"
#include "core.h"
#include "image.h"
#include "intersector.h"
#include "io.h"
#include "primitive.h"
#include "sampler.h"

int main()
{
  const int width = 512;
  const int height = 512;
  const int n_samples = 100;

  Image image(width, height);
  Camera camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, -1));

  const auto sphere = std::make_shared<Sphere>(glm::vec3(0), 1.0f);
  const auto floor =
      std::make_shared<Sphere>(glm::vec3(0, -10001, 0), 10000.0f);
  const auto material = std::make_shared<Material>(glm::vec3(0.8f));

  std::vector<std::shared_ptr<Primitive>> primitives;
  primitives.push_back(std::make_shared<Primitive>(sphere, material));
  primitives.push_back(std::make_shared<Primitive>(floor, material));

  LinearIntersector intersector(primitives);

  Sampler sampler(12);

  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      for (int k = 0; k < n_samples; ++k) {
        glm::vec2 ndc =
            glm::vec2((2.0f * (i + sampler.next_1d()) - width) / height,
                      (2.0f * (j + sampler.next_1d()) - height) / height);
        ndc.y *= -1.0f;
        const Ray ray = camera.sampleRay(ndc);

        IntersectInfo info;
        if (intersector.intersect(ray, info)) {
          image.addPixel(i, j, 0.5f * (info.normal + 1.0f));
        } else {
          image.addPixel(i, j, glm::vec3(0.0f));
        }
      }
    }
  }
  image.divide(n_samples);

  image.post_process();
  write_png("output.png", width, height, image.getConstPtr());

  return 0;
}