#pragma once
#include <algorithm>

#include "bsdf.h"
#include "core.h"
#include "intersector.h"
#include "sampler.h"

#define RAY_EPS 0.01f

class Integrator
{
 public:
  // compute incoming radiance
  virtual glm::vec3 integrate(const Ray& ray, const Intersector& intersector,
                              Sampler& sampler) const = 0;
};

class PathTracing : public Integrator
{
 public:
  PathTracing(uint32_t max_depth) : m_max_depth(max_depth) {}

  glm::vec3 integrate(const Ray& ray_in, const Intersector& intersector,
                      Sampler& sampler) const override
  {
    Ray ray = ray_in;
    glm::vec3 radiance(0.0f);
    glm::vec3 throughput(1.0f);

    for (int depth = 0; depth < m_max_depth; ++depth) {
      // russian roulette
      const float russian_roulette_prob =
          glm::max(throughput.x, glm::max(throughput.y, throughput.z));
      if (sampler.next_1d() > russian_roulette_prob) { break; }
      throughput /= russian_roulette_prob;

      IntersectInfo info;
      // ray goes to sky
      if (!intersector.intersect(ray, info)) {
        // evaluate environment light
        radiance += throughput * glm::vec3(1.0f);
        break;
      }

      // ray hits area light
      if (info.primitive->has_emission()) {
        // add Le
        radiance += throughput * info.primitive->material->ke;
        break;
      }

      // compute tangent space basis
      glm::vec3 tangent, bitangent;
      orthonormal_basis(info.normal, tangent, bitangent);

      // setup BSDF
      const auto bsdf = LambertOnly(*info.primitive->material);

      // sample direction from BSDF
      const glm::vec3 wo =
          world_to_local(-ray.direction, tangent, info.normal, bitangent);
      glm::vec3 f;
      float pdf;
      const glm::vec3 wi = bsdf.sampleDirection(sampler.next_2d(), wo, f, pdf);

      // update throughput
      throughput *= f * abs_cos_theta(wi) / pdf;

      // update ray
      ray.origin = info.position + RAY_EPS * info.normal;
      ray.direction = local_to_world(wi, tangent, info.normal, bitangent);
    }

    return radiance;
  }

 private:
  uint32_t m_max_depth;
};