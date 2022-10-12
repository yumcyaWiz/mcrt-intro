#pragma once
#include <memory>

#include "bxdf.h"
#include "core.h"
#include "primitive.h"
#include "sampler.h"

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

    glm::vec3 kd = material.base_color;
    if (material.base_color_tex != nullptr) {
      kd = glm::vec3(material.base_color_tex->fetch(info.texcoord));
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

// Diffuse + Specular + Metal BSDF
class DiffuseSpecularMetal : public BSDF
{
 public:
  DiffuseSpecularMetal(const IntersectInfo& info)
  {
    const Material& material = *info.primitive->material;

    glm::vec3 base_color = material.base_color;
    if (material.base_color_tex != nullptr) {
      base_color = glm::vec3(material.base_color_tex->fetch(info.texcoord));
    }

    glm::vec3 specular_color = material.specular_color;
    if (material.specular_color_tex != nullptr) {
      specular_color =
          glm::vec3(material.specular_color_tex->fetch(info.texcoord));
    }

    float specular = glm::clamp(material.specular, 0.0f, 1.0f);
    float specular_roughness =
        glm::clamp(material.specular_roughness, 0.01f, 1.0f);
    float specular_anisotropy = 0.0f;

    float metalness = glm::clamp(material.metalness, 0.0f, 1.0f);

    m_lambert = Lambert(base_color);
    m_specular = MicrofacetReflectionDielectric(1.5f, specular_roughness,
                                                specular_anisotropy);

    glm::vec3 n, k;
    artist_friendly_metallic_fresnel(
        glm::clamp(base_color, glm::vec3(0.01f), glm::vec3(0.99f)),
        glm::clamp(specular_color, glm::vec3(0.01), glm::vec3(0.99f)), n, k);
    m_metal = MicrofacetReflectionConductor(n, k, specular_roughness,
                                            specular_anisotropy);

    m_bxdf_weights[0] = glm::vec3(1.0f - metalness);
    m_bxdf_weights[1] = specular * (1.0f - metalness) * specular_color;
    m_bxdf_weights[2] = glm::vec3(metalness);

    float bxdf_sampling_weight[] = {(1.0f - metalness),
                                    specular * (1.0f - metalness), metalness};
    m_distribution = DiscreteDistribution1D(bxdf_sampling_weight, 3);
  }

  glm::vec3 sampleDirection(const glm::vec2& u, float v, const glm::vec3& wo,
                            glm::vec3& f, float& pdf) const override
  {
    // sample BxDF
    float bxdf_pmf;
    const int bxdf_idx = m_distribution.sample(v, bxdf_pmf);

    switch (bxdf_idx) {
      // diffuse
      case 0: {
        const glm::vec3 wi = m_lambert.sampleDirection(u, wo, f, pdf);
        f *= m_bxdf_weights[0];
        pdf *= bxdf_pmf;
        return wi;
      } break;
      // specular
      case 1: {
        const glm::vec3 wi = m_specular.sampleDirection(u, wo, f, pdf);
        f *= m_bxdf_weights[1];
        pdf *= bxdf_pmf;
        return wi;
      } break;
      // metal
      case 2: {
        const glm::vec3 wi = m_metal.sampleDirection(u, wo, f, pdf);
        f *= m_bxdf_weights[2];
        pdf *= bxdf_pmf;
        return wi;
      } break;
    }

    return glm::vec3(0.0f);
  }

 private:
  glm::vec3 m_bxdf_weights[3];

  Lambert m_lambert;
  MicrofacetReflectionDielectric m_specular;
  MicrofacetReflectionConductor m_metal;

  DiscreteDistribution1D m_distribution;
};