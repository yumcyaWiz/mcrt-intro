#pragma once
#include <algorithm>
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
    BVHNode* node = new BVHNode;

    // calculate AABB
    AABB bbox;
    for (int i = primitive_start; i < primitive_end; ++i) {
      bbox = bbox.mergeAABB(m_primitives[i].getBounds());
    }

    const int n_primitives = primitive_end - primitive_start;
    if (n_primitives <= m_n_primitives_in_leaf_node) {
      // create leaf node
      return createLeafNode(node, bbox, primitive_start, n_primitives);
    }

    // calculate split AABB
    // NOTE: using bbox doesn't work well when splitting
    AABB split_bbox;
    for (int i = primitive_start; i < primitive_end; ++i) {
      split_bbox = split_bbox.mergeAABB(m_primitives[i].getBounds().center());
    }

    // split axis
    const int split_axis = split_bbox.logestAxis();

    // split position
    const float split_pos = split_bbox.center()[split_axis];

    // split bounding box
    const int split_idx = primitive_start + n_primitives / 2;
    std::nth_element(m_primitives + primitive_start, m_primitives + split_idx,
                     m_primitives + primitive_end,
                     [&](const auto& prim1, const auto& prim2) {
                       return prim1.getBounds().center()[split_axis] <
                              prim2.getBounds().center()[split_axis];
                     });

    // if splitting failed, create leaf node
    if (split_idx == primitive_start || split_idx == primitive_end) {
      spdlog::info("[BVH] splitting failed");
      spdlog::info("[BVH] n_primitives: {}", n_primitives);
      spdlog::info("[BVH] split_axis: {}", split_axis);
      spdlog::info("[BVH] split_pos: {}", split_pos);
      spdlog::info("[BVH] primitive_start: {}", primitive_start);
      spdlog::info("[BVH] split_idx: {}", split_idx);
      spdlog::info("[BVH] primitive_end: {}", primitive_end);
      return createLeafNode(node, bbox, primitive_start, n_primitives);
    }

    node->bbox = bbox;
    node->primitive_indices_offset = primitive_start;
    node->axis = split_axis;

    // build left child nodes
    node->children[0] = buildBVHNode(primitive_start, split_idx);
    // build right child nodes
    node->children[1] = buildBVHNode(split_idx, primitive_end);

    m_stats.n_internal_nodes++;

    return node;
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
    bool hit = false;

    info.bvh_depth++;

    // intersect with bounding box
    if (node->bbox.intersect(ray, dir_inv, dir_inv_sign)) {
      if (node->children[0] == nullptr && node->children[1] == nullptr) {
        // when leaf node, intersect with primitives
        const int primitive_end =
            node->primitive_indices_offset + node->n_primitives;
        for (int i = node->primitive_indices_offset; i < primitive_end; ++i) {
          if (m_primitives[i].intersect(ray, info)) {
            hit = true;
            ray.tmax = info.t;
          }
        }
      } else {
        // intersect with child nodes
        hit |= intersectNode(node->children[dir_inv_sign[node->axis]], ray,
                             dir_inv, dir_inv_sign, info);
        hit |= intersectNode(node->children[1 - dir_inv_sign[node->axis]], ray,
                             dir_inv, dir_inv_sign, info);
      }
    }

    return hit;
  }
};

// bounding volume hierarchy(optimized version)
// O(log(N))
class BVHOptimized : public Intersector
{
 public:
  BVHOptimized(Primitive* primitives, uint32_t n_primitives)
      : Intersector(primitives, n_primitives)
  {
  }

  // build bvh nodes
  void buildBVH()
  {
    // build bvh nodes from root
    buildBVHNode(0, m_n_primitives);

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
    bool hit = intersectNode(0, ray, dir_inv, dir_inv_sign, info);
    ray.tmax = ray_tmax;
    return hit;
  }

 private:
  struct alignas(32) BVHNode {
    AABB bbox;  // bounding box
    union {
      uint32_t primitive_indices_offset;  // offset to primitives
      uint32_t second_child_offset;       // offset to second child node
    };
    uint16_t n_primitives = 0;  // number of primitives(0 means internal node)
    uint8_t axis = 0;           // splitting axis
  };

  struct BVHStatistics {
    int n_nodes = 0;           // number of nodes
    int n_internal_nodes = 0;  // number of internal nodes
    int n_leaf_nodes = 0;      // number of leaf nodes
  };

  std::vector<BVHNode> m_nodes;
  BVHStatistics m_stats;

  void addLeafNode(const AABB& bbox, int primitive_start, int n_primitives)
  {
    BVHNode node;
    node.bbox = bbox;
    node.primitive_indices_offset = primitive_start;
    node.n_primitives = n_primitives;
    m_nodes.push_back(node);
    m_stats.n_leaf_nodes++;
  }

  void buildBVHNode(int primitive_start, int primitive_end)
  {
    // calculate AABB
    AABB bbox;
    for (int i = primitive_start; i < primitive_end; ++i) {
      bbox = bbox.mergeAABB(m_primitives[i].getBounds());
    }

    const int n_primitives = primitive_end - primitive_start;
    if (n_primitives <= 4) {
      // create leaf node
      addLeafNode(bbox, primitive_start, n_primitives);
      return;
    }

    // calculate split AABB
    // NOTE: using bbox doesn't work well when splitting
    AABB split_bbox;
    for (int i = primitive_start; i < primitive_end; ++i) {
      split_bbox = split_bbox.mergeAABB(m_primitives[i].getBounds().center());
    }

    // split axis
    const int split_axis = split_bbox.logestAxis();

    // split position
    const float split_pos = split_bbox.center()[split_axis];

    // split bounding box
    const int split_idx = primitive_start + n_primitives / 2;
    std::nth_element(m_primitives + primitive_start, m_primitives + split_idx,
                     m_primitives + primitive_end,
                     [&](const auto& prim1, const auto& prim2) {
                       return prim1.getBounds().center()[split_axis] <
                              prim2.getBounds().center()[split_axis];
                     });

    // if splitting failed, create leaf node
    if (split_idx == primitive_start || split_idx == primitive_end) {
      spdlog::info("[BVH] splitting failed");
      spdlog::info("[BVH] n_primitives: {}", n_primitives);
      spdlog::info("[BVH] split_axis: {}", split_axis);
      spdlog::info("[BVH] split_pos: {}", split_pos);
      spdlog::info("[BVH] primitive_start: {}", primitive_start);
      spdlog::info("[BVH] split_idx: {}", split_idx);
      spdlog::info("[BVH] primitive_end: {}", primitive_end);
      addLeafNode(bbox, primitive_start, n_primitives);
      return;
    }

    // add internal node
    const int parent_offset = m_nodes.size();
    BVHNode node;
    node.bbox = bbox;
    node.primitive_indices_offset = primitive_start;
    node.axis = split_axis;
    m_nodes.push_back(node);
    m_stats.n_internal_nodes++;

    // build left child nodes
    buildBVHNode(primitive_start, split_idx);

    // calculate offset to right child node
    const int second_child_offset = m_nodes.size();
    m_nodes[parent_offset].second_child_offset = second_child_offset;

    // build right child nodes
    buildBVHNode(split_idx, primitive_end);
  }

  // traverse bvh nodes recursively
  bool intersectNode(int node_idx, const Ray& ray, const glm::vec3& dir_inv,
                     const int dir_inv_sign[3], IntersectInfo& info) const
  {
    bool hit = false;
    const BVHNode& node = m_nodes[node_idx];

    // intersect with bounding box
    if (node.bbox.intersect(ray, dir_inv, dir_inv_sign)) {
      if (node.n_primitives > 0) {
        // when leaf node, intersect with primitives
        const int primitive_end =
            node.primitive_indices_offset + node.n_primitives;
        for (int i = node.primitive_indices_offset; i < primitive_end; ++i) {
          if (m_primitives[i].intersect(ray, info)) {
            hit = true;
            ray.tmax = info.t;
          }
        }
      } else {
        // intersect with child nodes
        if (dir_inv_sign[node.axis] == 0) {
          hit |= intersectNode(node_idx + 1, ray, dir_inv, dir_inv_sign, info);
          hit |= intersectNode(node.second_child_offset, ray, dir_inv,
                               dir_inv_sign, info);
        } else {
          hit |= intersectNode(node.second_child_offset, ray, dir_inv,
                               dir_inv_sign, info);
          hit |= intersectNode(node_idx + 1, ray, dir_inv, dir_inv_sign, info);
        }
      }
    }

    return hit;
  }
};