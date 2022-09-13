#include <memory>

#include "bsdf.h"
#include "camera.h"
#include "core.h"
#include "image.h"
#include "integrator.h"
#include "intersector.h"
#include "io.h"
#include "primitive.h"
#include "sampler.h"

#define RAY_EPS 0.001f

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
  const auto white =
      std::make_shared<Material>(glm::vec3(0.8f), glm::vec3(0.0f), 1.0f);
  const auto green = std::make_shared<Material>(glm::vec3(0.2f, 0.8f, 0.2f),
                                                glm::vec3(0.8f), 0.01f);

  std::vector<std::shared_ptr<Primitive>> primitives;
  primitives.push_back(std::make_shared<Primitive>(sphere, green));
  primitives.push_back(std::make_shared<Primitive>(floor, white));

  LinearIntersector intersector(primitives);

  Sampler sampler(12);

#pragma omp parallel for collapse(2)
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      for (int k = 0; k < n_samples; ++k) {
        glm::vec2 ndc =
            glm::vec2((2.0f * (i + sampler.next_1d()) - width) / height,
                      (2.0f * (j + sampler.next_1d()) - height) / height);
        ndc.y *= -1.0f;

        // sample ray from camera
        const Ray ray = camera.sampleRay(ndc);

        IntersectInfo info;
        if (intersector.intersect(ray, info)) {
        } else {
          // ray doesn't hit anything
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