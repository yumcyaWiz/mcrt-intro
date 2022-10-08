#include "camera.h"
#include "core.h"
#include "image.h"
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

  const auto sphere = Sphere(glm::vec3(0), 1.0f);
  const auto floor = Sphere(glm::vec3(0, -10001, 0), 10000.0f);
  const auto white = Material(glm::vec3(0.8f), glm::vec3(0.0f), 1.0f);
  const auto green =
      Material(glm::vec3(0.2f, 0.8f, 0.2f), glm::vec3(0.8f), 0.01f);

  std::vector<Primitive> primitives;
  primitives.emplace_back(&sphere, &green);
  primitives.emplace_back(&floor, &white);

  LinearIntersector intersector(primitives.data(), primitives.size());

  Sampler sampler(12);

  glm::vec3 sun_direction = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));

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
          const glm::vec3 wi = sun_direction;
          const glm::vec3 wo = -ray.direction;
          const glm::vec3 wh = glm::normalize(wo + wi);

          // trace shadow ray
          Ray shadow_ray(info.position + RAY_EPS * info.normal, wi);
          float visibility = 1.0f;
          IntersectInfo shadow_info;
          if (intersector.intersect(shadow_ray, shadow_info)) {
            visibility = 0.0f;
          }

          const glm::vec3 color =
              visibility *
              (info.primitive->material->kd *
                   glm::max(glm::dot(wi, info.normal), 0.0f) +
               info.primitive->material->ks *
                   glm::pow(glm::max(glm::dot(wh, info.normal), 0.0f),
                            1.0f / info.primitive->material->roughness));

          image.addPixel(i, j, color);
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