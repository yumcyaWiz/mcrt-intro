#pragma once
#include <algorithm>
#include <stdexcept>

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
    // TODO: implement this
    throw std::runtime_error("not implemented");
  }

 private:
  uint32_t m_max_depth;
};