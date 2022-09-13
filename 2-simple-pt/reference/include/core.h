#pragma once

#include "glm/glm.hpp"

struct Ray {
  glm::vec3 origin;     // ray origin
  glm::vec3 direction;  // ray direction
  float tmin = 0.0f;    // minimum hit distance
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
  glm::vec3 kd;     // diffuse color
  glm::vec3 ks;     // specular color
  float roughness;  // specular roughness

  Material(const glm::vec3& kd_, const glm::vec3& ks_, float roughness_)
      : kd(kd_), ks(ks_), roughness(roughness_)
  {
  }
};

inline glm::vec3 spherical_to_cartesian(float phi, float theta)
{
  const float sin_theta = glm::sin(theta);
  return glm::vec3(glm::cos(phi) * sin_theta, glm::cos(theta),
                   glm::sin(phi) * sin_theta);
}