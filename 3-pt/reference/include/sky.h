#pragma once
#include <cmath>
#include <filesystem>

#include "core.h"
#include "glm/glm.hpp"
#include "texture.h"

class Sky
{
 public:
  // evaluate incoming radiance
  virtual glm::vec3 evaluate(const Ray& ray) const = 0;
};

// uniform color sky
class UniformSky : public Sky
{
 public:
  UniformSky(const glm::vec3& albedo) : m_albedo(albedo) {}

  glm::vec3 evaluate(const Ray& ray) const override { return m_albedo; }

 private:
  glm::vec3 m_albedo;  // sky color
};

// image based lighting
class IBL : public Sky
{
 public:
  IBL(const std::filesystem::path& filepath) : m_texture{filepath} {}

  glm::vec3 evaluate(const Ray& ray) const override
  {
    float phi, theta;
    cartesian_to_spherical(ray.direction, phi, theta);

    glm::vec2 texcoord(phi / (2.0f * M_PIf), 1.0f - theta / M_PIf);

    return m_texture.fetch(texcoord);
  }

 private:
  Texture m_texture;  // ibl texture
};