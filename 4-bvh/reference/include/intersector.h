#pragma once
#include <cstddef>
#include <limits>
#include <memory>
#include <vector>

#include "aabb.h"
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
  struct BVHNode {
    AABB bbox;                          // bounding box
    uint32_t primitive_indices_offset;  // offset of primitives indices
    uint16_t n_primitives;              // number of primitives
    uint8_t axis;                       // split axis
    BVHNode* children[2];               // pointer to child node
  };

  BVHNode* root;  // pointer to root node

  BVHNode* createLeafNode(BVHNode* node, const AABB& bbox,
                          int prim_indices_offset, int n_primitives)
  {
    node->bbox = bbox;
    node->primitive_indices_offset = prim_indices_offset;
    node->n_primitives = n_primitives;
    node->children[0] = nullptr;
    node->children[1] = nullptr;
    return node;
  }

  // build bvh nodes from root to leaves recursively
  // primitive_start: start index of primitives
  // primitive_end: end index of primitives
  BVHNode* buildBVHNode(int primitive_start, int primitive_end)
  {
    BVHNode* node = new BVHNode;

    // calculate AABB
    AABB bbox;
  }
};