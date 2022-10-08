#pragma once
#include <stdexcept>

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
    // TODO: implement this
    throw std::runtime_error("not implemented");
  }
};