#pragma once
#include <memory>
#include <vector>

#include "core.h"
#include "primitive.h"

class Intersector
{
 public:
  // find closest ray intersection
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
};

// search all intersectables
// O(N)
class LinearIntersector : public Intersector
{
 public:
  LinearIntersector(const std::vector<std::shared_ptr<Primitive>>& primitives)
      : m_primitives(primitives)
  {
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const override
  {
    // TODO: implement this
  }

 private:
  std::vector<std::shared_ptr<Primitive>>
      m_primitives;  // array of primitive pointers
};