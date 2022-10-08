#pragma once
#include <cmath>
#include <stdexcept>

#include "core.h"
#include "glm/glm.hpp"

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
    // TODO: implement this
    throw std::runtime_error("not implemented");
  }

 private:
  glm::vec3 m_center;  // center of sphere
  float m_radius;      // radius of sphere
};