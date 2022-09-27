#pragma once
#include <limits>
#include <memory>
#include <vector>

#include "core.h"
#include "primitive.h"

class Intersector
{
 public:
  Intersector(const Primitive* primitives, uint32_t n_primitives)
      : m_primitives(primitives), m_n_primitives(n_primitives)
  {
  }

  // find closest ray intersection
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;

 protected:
  const Primitive* m_primitives;  // array of primitives
  uint32_t m_n_primitives;        // number of primitives
};

// search all intersectables
// O(N)
class LinearIntersector : public Intersector
{
 public:
  LinearIntersector(const Primitive* primitives, uint32_t n_primitives)
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

struct AABB {
  glm::vec3 bounds[2];  // 0 - min, 1 - max

  AABB()
      : bounds{glm::vec3(std::numeric_limits<float>::max()),
               glm::vec3(std::numeric_limits<float>::min())}
  {
  }

  AABB(const glm::vec3& pmin, const glm::vec3& pmax) : bounds{pmin, pmax} {}

  // get center position of bounding box
  glm::vec3 center() const { return 0.5f * (bounds[0] + bounds[1]); }

  // get longest axis
  // 0 - x, 1 - y, 2 - z
  int logestAxis() const
  {
    const glm::vec3 length = bounds[1] - bounds[0];
    // x
    if (length.x >= length.y && length.x >= length.z) {
      return 0;
    }
    // y
    else if (length.y >= length.x && length.y >= length.z) {
      return 1;
    }
    // z
    else {
      return 2;
    }
  }

  bool intersect(const Ray& ray, const glm::vec3& dir_inv,
                 const int dir_inv_sign[3]) const
  {
    // https://dl.acm.org/doi/abs/10.1145/1198555.1198748
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (bounds[dir_inv_sign[0]].x - ray.origin.x) * dir_inv.x;
    tmax = (bounds[1 - dir_inv_sign[0]].x - ray.origin.x) * dir_inv.x;
    tymin = (bounds[dir_inv_sign[1]].y - ray.origin.y) * dir_inv.y;
    tymax = (bounds[1 - dir_inv_sign[1]].y - ray.origin.y) * dir_inv.y;
    if (tmin > tymax || tymin > tmax) return false;
    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    tzmin = (bounds[dir_inv_sign[2]].z - ray.origin.z) * dir_inv.z;
    tzmax = (bounds[1 - dir_inv_sign[2]].z - ray.origin.z) * dir_inv.z;
    if (tmin > tzmax || tzmin > tmax) return false;
    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return tmin < ray.tmax && tmax > ray.tmin;
  }

  // extend bounding box by p
  AABB mergeAABB(const glm::vec3& p)
  {
    AABB ret;
    for (int i = 0; i < 3; ++i) {
      ret.bounds[0][i] = glm::min(bounds[0][i], p[i]);
      ret.bounds[1][i] = glm::max(bounds[1][i], p[i]);
    }
    return ret;
  }

  // merge two bounding box
  AABB mergeAABB(const AABB& b)
  {
    AABB ret;
    for (int i = 0; i < 3; ++i) {
      ret.bounds[0][i] = glm::min(bounds[0][i], b.bounds[0][i]);
      ret.bounds[1][i] = glm::min(bounds[1][i], b.bounds[1][i]);
    }
    return ret;
  }
};

// bounding volume hierarchy
// O(log(N))
class BVH : public Intersector
{
 public:
  BVH(const Primitive* primitives, uint32_t n_primitives)
      : Intersector(primitives, n_primitives)
  {
  }

  bool intersect(const Ray& ray, Intersector& info) { return false; }

 private:
  const Primitive* m_primitives;  // array of primitives
  uint32_t m_n_primitives;        // number of primitives
};