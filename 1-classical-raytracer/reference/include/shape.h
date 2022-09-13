#pragma once
#include <cmath>

#include "core.h"
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

class Shape
{
 public:
  // find ray intersection
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
};

class Sphere : public Shape
{
 public:
  Sphere(const glm::vec3& center, float radius)
      : m_center(center), m_radius(radius)
  {
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const override
  {
    // solve quadratic equation
    const float b = glm::dot(ray.direction, ray.origin - m_center);
    const float c = glm::dot(ray.origin - m_center, ray.origin - m_center) -
                    m_radius * m_radius;
    const float D = b * b - c;
    if (D < 0) return false;

    const float t0 = -b - std::sqrt(D);
    const float t1 = -b + std::sqrt(D);
    float t = t0;
    if (t < ray.tmin || t > ray.tmax) {
      t = t1;
      if (t < ray.tmin || t > ray.tmax) return false;
    }

    info.t = t;
    info.position = ray(t);
    info.normal = glm::normalize(info.position - m_center);

    return true;
  }

 private:
  glm::vec3 m_center;  // center of sphere
  float m_radius;      // radius of sphere
};