#pragma once
#include <memory>

#include "bxdf.h"
#include "core.h"
#include "primitive.h"

class BSDF
{
 public:
  // return sampled direction and BSDF value, pdf
  // u: [0, 1] x [0, 1] random number
  // v: [0, 1] random number
  // wo: view direction in tangent space
  // f: BSDF value
  // pdf: pdf value
  // return: sampled direction in tangent space
  virtual glm::vec3 sampleDirection(const glm::vec2& u, float v,
                                    const glm::vec3& wo, glm::vec3& f,
                                    float& pdf) const = 0;
};

// Lambert Diffuse BRDF only
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

  glm::vec3 sampleDirection(const glm::vec2& u, float v, const glm::vec3& wo,
                            glm::vec3& f, float& pdf) const override
  {
    return m_lambert.sampleDirection(u, wo, f, pdf);
  }

 private:
  Lambert m_lambert;
};

// Diffuse + Specular BSDF
class DiffuseSpecular : public BSDF
{
 public:
  DiffuseSpecular(const IntersectInfo& info)
  {
    const Material& material = *info.primitive->material;

    glm::vec3 kd = material.kd;
    if (material.kd_tex != nullptr) {
      kd = glm::vec3(material.kd_tex->fetch(info.texcoord));
    }

    glm::vec3 ks = material.ks;
    if (material.ks_tex != nullptr) {
      ks = glm::vec3(material.ks_tex->fetch(info.texcoord));
    }

    float roughness = glm::clamp(material.roughness, 0.01f, 1.0f);

    m_lambert = Lambert(kd);
    m_specular = MicrofacetReflectionDielectric(1.5f, roughness, 0.0f);
  }

  glm::vec3 sampleDirection(const glm::vec2& u, float v, const glm::vec3& wo,
                            glm::vec3& f, float& pdf) const override
  {
    if (v < 0.5f) {
      const glm::vec3 wi = m_lambert.sampleDirection(u, wo, f, pdf);
      pdf *= 0.5f;
      return wi;
    } else {
      const glm::vec3 wi = m_specular.sampleDirection(u, wo, f, pdf);
      pdf *= 0.5f;
      return wi;
    }
  }

 private:
  Lambert m_lambert;
  MicrofacetReflectionDielectric m_specular;
};