#pragma once
#include <algorithm>

#include "core.h"
#include "intersector.h"

class Integrator
{
 public:
  // compute incoming radiance
  virtual glm::vec3 integrate(const Ray& ray,
                              const Intersector* intersector) const = 0;
};

class PathTracing : public Integrator
{
 public:
  PathTracing(uint32_t n_samples, uint32_t max_depth)
      : m_n_samples(n_samples), m_max_depth(max_depth)
  {
  }

  glm::vec3 integrate(const Ray& ray_in,
                      const Intersector* intersector) const override
  {
    Ray ray = ray_in;
    glm::vec3 radiance(0.0f);
    for (int depth = 0; depth < m_max_depth; ++depth) {}
    return radiance;
  }

 private:
  uint32_t m_n_samples;
  uint32_t m_max_depth;
};