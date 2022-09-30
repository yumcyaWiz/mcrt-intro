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
  PinholeCamera camera(glm::vec3(0, 0, 0.5), glm::vec3(0, 0, -1),
                       0.33f * M_PIf);

  Scene scene;
  scene.loadObj("./head_with_light/head_with_light.obj");

  BVHOptimized intersector(scene.m_primitives.data(),
                           scene.m_primitives.size());
  intersector.buildBVH();

  UniformSky sky(glm::vec3(0.0f));

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
        const Ray ray = camera.sampleRay(ndc, sampler.next_2d());

        // evaluate incoming radiance
        const glm::vec3 radiance =
            integrator.integrate(ray, &intersector, &sky, sampler);

        image.addPixel(i, j, radiance);
      }
    }
  }
  image.divide(n_samples);

  image.post_process();
  write_png("output.png", width, height, image.getConstPtr());

  return 0;
}