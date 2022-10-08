#pragma once
#include <memory>

#include "bxdf.h"
#include "core.h"

class BSDF
{
 public:
  virtual glm::vec3 sampleDirection(const glm::vec2& u, const glm::vec3& wo,
                                    glm::vec3& f, float& pdf) const = 0;
};

class LambertOnly : public BSDF
{
 public:
  LambertOnly(const Material& material) : m_lambert(material.kd) {}

  glm::vec3 sampleDirection(const glm::vec2& u, const glm::vec3& wo,
                            glm::vec3& f, float& pdf) const override
  {
    return m_lambert.sampleDirection(u, wo, f, pdf);
  }

 private:
  Lambert m_lambert;
};