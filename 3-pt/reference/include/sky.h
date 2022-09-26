#pragma once
#include <filesystem>

#include "core.h"
#include "glm/glm.hpp"

class Sky
{
 public:
  // evaluate incoming radiance
  virtual glm::vec3 evaluate(const Ray& ray) const = 0;
};

class UniformSky : public Sky
{
 public:
  UniformSky(const glm::vec3& albedo) : m_albedo(albedo) {}

  glm::vec3 evaluate(const Ray& ray) const override { return m_albedo; }

 private:
  glm::vec3 m_albedo;
};

class IBL : public Sky
{
 public:
  IBL(const std::filesystem::path& filepath) {}

 private:
};