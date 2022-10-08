#pragma once

#include "core.h"
#include "primitive.h"

class Intersector
{
 public:
  Intersector(Primitive* primitives, uint32_t n_primitives)
      : m_primitives(primitives), m_n_primitives(n_primitives)
  {
  }

  // find closest ray intersection
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;

 protected:
  Primitive* m_primitives;  // array of primitives
  uint32_t m_n_primitives;  // number of primitives
};

// search all intersectables
// O(N)
class LinearIntersector : public Intersector
{
 public:
  LinearIntersector(Primitive* primitives, uint32_t n_primitives)
      : Intersector(primitives, n_primitives)
  {
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const override
  {
    bool hit = false;
    float ray_tmax = ray.tmax;
    for (uint32_t i = 0; i < m_n_primitives; ++i) {
      const Primitive& primitive = m_primitives[i];
      if (primitive.intersect(ray, info)) {
        ray.tmax = info.t;
        hit = true;
      }
    }
    ray.tmax = ray_tmax;
    return hit;
  }
};