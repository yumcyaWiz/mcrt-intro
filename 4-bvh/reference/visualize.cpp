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

  Image image(width, height);
  PinholeCamera camera(glm::vec3(0, 0, 0.5), glm::vec3(0, 0, -1),
                       0.33f * M_PIf);

  Scene scene;
  scene.loadObj("head_with_light/head_with_light.obj");

  BVH intersector(scene.m_primitives.data(), scene.m_primitives.size());
  intersector.buildBVH();

  Sampler sampler(12);

#pragma omp parallel for collapse(2)
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      glm::vec2 ndc =
          glm::vec2((2.0f * i - width) / height, (2.0f * j - height) / height);
      ndc.y *= -1.0f;

      // sample ray from camera
      const Ray ray = camera.sampleRay(ndc, sampler.next_2d());

      // coloring with bvh depth
      glm::vec3 color(0.0f);
      IntersectInfo info;
      if (intersector.intersect(ray, info)) {
        color = glm::vec3(0, glm::log(info.bvh_depth) / 16.0f, 0);
        // color = 0.5f * (info.normal + 1.0f);
      }

      image.setPixel(i, j, color);
    }
  }

  image.post_process();
  write_png("output.png", width, height, image.getConstPtr());

  return 0;
}