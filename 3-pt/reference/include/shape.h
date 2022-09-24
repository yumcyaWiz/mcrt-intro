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

class Triangle : public Shape
{
 public:
  Triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
           const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2,
           const glm::vec2& t0, const glm::vec2& t1, const glm::vec2& t2)
      : m_v0(v0),
        m_v1(v1),
        m_v2(v2),
        m_n0(n0),
        m_n1(n1),
        m_n2(n2),
        m_t0(t0),
        m_t1(t1),
        m_t2(t2)
  {
  }

  // Möller–Trumbore intersection algorithm
  // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm?oldformat=true
  bool intersect(const Ray& ray, IntersectInfo& info) const override
  {
    const float EPS = 1e-8f;

    const glm::vec3 e0 = m_v1 - m_v0;
    const glm::vec3 e1 = m_v2 - m_v0;
    const glm::vec3 h = glm::cross(ray.direction, e1);
    const float a = glm::dot(e0, h);
    if (a > -EPS && a < EPS) { return false; }

    const float f = 1.0f / a;
    const glm::vec3 s = ray.origin - m_v0;
    const float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) { return false; }

    const glm::vec3 q = glm::cross(s, e0);
    const float v = f * glm::dot(ray.direction, q);
    if (v < 0.0f || u + v > 1.0f) { return false; }

    const float t = f * glm::dot(e1, q);
    if (t < ray.tmin || t > ray.tmax) { return false; }

    info.t = t;
    info.position = (1.0f - u - v) * m_v0 + u * m_v1 + v * m_v2;
    info.normal = (1.0f - u - v) * m_n0 + u * m_n1 + v * m_n2;
    info.texcoord = (1.0f - u - v) * m_t0 + u * m_t1 + v * m_t2;

    return true;
  }

 private:
  // vertex positions
  glm::vec3 m_v0;
  glm::vec3 m_v1;
  glm::vec3 m_v2;

  // vertex normals
  glm::vec3 m_n0;
  glm::vec3 m_n1;
  glm::vec3 m_n2;

  // vertex texcoords
  glm::vec2 m_t0;
  glm::vec2 m_t1;
  glm::vec2 m_t2;
};