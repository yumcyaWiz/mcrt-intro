#pragma once
#include <cmath>
#include <filesystem>
#include <stdexcept>

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
    // TODO: implement this
    throw std::runtime_error("not implemented");
  }

 private:
  Texture m_texture;  // ibl texture
};