#pragma once

#include "glm/glm.hpp"

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
  float tmin = 0.001f;  // minimum hit distance
  mutable float tmax = 1e9f;
  ;  // maximum hit distance

  Ray() {}
  Ray(const glm::vec3& origin_, const glm::vec3& direction_)
      : origin(origin_), direction(direction_)
  {
  }

  // return position on the ray with distance t
  glm::vec3 operator()(float t) const { return origin + t * direction; }
};

// forward declaration
struct Primitive;

struct IntersectInfo {
  float t;                     // hit distance
  glm::vec3 position;          // hit position
  glm::vec3 normal;            // hit normal
  const Primitive* primitive;  // hit primitive pointer
};

struct Material {
  glm::vec3 kd;  // diffuse color

  Material(const glm::vec3& kd_) : kd(kd_) {}
};