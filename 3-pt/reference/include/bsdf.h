#pragma once
#include <memory>

#include "bxdf.h"
#include "core.h"
#include "primitive.h"

class BSDF
{
 public:
  virtual glm::vec3 sampleDirection(const glm::vec2& u, const glm::vec3& wo,
                                    glm::vec3& f, float& pdf) const = 0;
};

class LambertOnly : public BSDF
{
 public:
  LambertOnly(const IntersectInfo& info)
  {
    const Material& material = *info.primitive->material;

    glm::vec3 kd = material.kd;
    if (material.kd_tex != nullptr) {
      kd = glm::vec3(material.kd_tex->fetch(info.texcoord));
    }

    m_lambert = Lambert(kd);
  }

  glm::vec3 sampleDirection(const glm::vec2& u, const glm::vec3& wo,
                            glm::vec3& f, float& pdf) const override
  {
    return m_lambert.sampleDirection(u, wo, f, pdf);
  }

 private:
  Lambert m_lambert;
};