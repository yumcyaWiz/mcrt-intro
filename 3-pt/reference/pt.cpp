#include <memory>
//
#include "bsdf.h"
#include "camera.h"
#include "core.h"
#include "image.h"
#include "integrator.h"
#include "intersector.h"
#include "io.h"
#include "primitive.h"
#include "sampler.h"
#include "scene.h"

int main()
{
  const int width = 512;
  const int height = 512;
  const int n_samples = 100;
  const int max_depth = 10;

  Image image(width, height);
  Camera camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, -1));

  const auto sphere = std::make_shared<Sphere>(glm::vec3(0), 1.0f);
  const auto floor =
      std::make_shared<Sphere>(glm::vec3(0, -10001, 0), 10000.0f);
  const auto white = std::make_shared<Material>(
      glm::vec3(0.8f), glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
  const auto green = std::make_shared<Material>(
      glm::vec3(0.2f, 0.8f, 0.2f), glm::vec3(0.8f), glm::vec3(0.0f), 0.01f);

  std::vector<std::shared_ptr<Primitive>> primitives;
  primitives.push_back(std::make_shared<Primitive>(sphere, green));
  primitives.push_back(std::make_shared<Primitive>(floor, white));

  LinearIntersector intersector(primitives);

  Sampler sampler(12);

  PathTracing integrator(max_depth);

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

        // evaluate incoming radiance
        const glm::vec3 radiance =
            integrator.integrate(ray, &intersector, sampler);

        image.addPixel(i, j, radiance);
      }
    }
  }
  image.divide(n_samples);

  image.post_process();
  write_png("output.png", width, height, image.getConstPtr());

  return 0;
}