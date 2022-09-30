#pragma once
#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

#include "aabb.h"
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

// bounding volume hierarchy
// O(log(N))
class BVH : public Intersector
{
 public:
  BVH(Primitive* primitives, uint32_t n_primitives)
      : Intersector(primitives, n_primitives)
  {
  }

  ~BVH()
  {
    if (m_root) { deleteBVHNode(m_root); }
  }

  // build bvh nodes
  void buildBVH()
  {
    // build bvh nodes from root
    m_root = buildBVHNode(0, m_n_primitives);

    m_stats.n_nodes = m_stats.n_internal_nodes + m_stats.n_leaf_nodes;

    spdlog::info("[BVH] number of nodes: {}", m_stats.n_nodes);
    spdlog::info("[BVH] number of internal nodes: {}",
                 m_stats.n_internal_nodes);
    spdlog::info("[BVH] number of leaf nodes: {}", m_stats.n_leaf_nodes);
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const override
  {
    // precompute inverse of ray direction, sign
    const glm::vec3 dir_inv = 1.0f / ray.direction;
    int dir_inv_sign[3];
    for (int i = 0; i < 3; ++i) { dir_inv_sign[i] = dir_inv[i] > 0 ? 0 : 1; }

    // intersect bvh nodes from root
    float ray_tmax = ray.tmax;
    bool hit = intersectNode(m_root, ray, dir_inv, dir_inv_sign, info);
    ray.tmax = ray_tmax;
    return hit;
  }

 private:
  struct BVHNode {
    AABB bbox;                          // bounding box
    uint32_t primitive_indices_offset;  // offset of primitives indices
    uint16_t n_primitives;              // number of primitives
    uint8_t axis;                       // split axis
    BVHNode* children[2];               // pointer to child node
  };

  struct BVHStatistics {
    int n_nodes = 0;           // number of nodes
    int n_internal_nodes = 0;  // number of internal nodes
    int n_leaf_nodes = 0;      // number of leaf nodes
  };

  BVHNode* m_root;  // pointer to root node
  BVHStatistics m_stats;

  static constexpr int m_n_primitives_in_leaf_node =
      4;  // number of primitives in the leaf node

  BVHNode* createLeafNode(BVHNode* node, const AABB& bbox,
                          int prim_indices_offset, int n_primitives)
  {
    node->bbox = bbox;
    node->primitive_indices_offset = prim_indices_offset;
    node->n_primitives = n_primitives;
    node->children[0] = nullptr;
    node->children[1] = nullptr;
    m_stats.n_leaf_nodes++;
    return node;
  }

  // build bvh nodes from root to leaves recursively
  // primitive_start: start index of primitives
  // primitive_end: end index of primitives
  BVHNode* buildBVHNode(int primitive_start, int primitive_end)
  {
    // TODO: implement this
    throw std::runtime_error("not implemented");
  }

  // delete bvh nodes recursively
  void deleteBVHNode(BVHNode* node)
  {
    if (node->children[0]) { deleteBVHNode(node->children[0]); }
    if (node->children[1]) { deleteBVHNode(node->children[1]); }
    delete node;
  }

  // traverse bvh nodes recursively
  bool intersectNode(const BVHNode* node, const Ray& ray,
                     const glm::vec3& dir_inv, const int dir_inv_sign[3],
                     IntersectInfo& info) const
  {
    // TODO: implement this
    throw std::runtime_error("not implemented");
  }
};