#pragma once
#include "glm/glm.hpp"
#include "sampler.h"

// trigonometric functions in tangent space
inline float cos_theta(const glm::vec3& w) { return w.y; }
inline float cos2_theta(const glm::vec3& w) { return w.y * w.y; }
inline float abs_cos_theta(const glm::vec3& w) { return glm::abs(w.y); }

// return reflection direction
inline glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n)
{
  return glm::normalize(-v + 2.0f * glm::dot(v, n) * n);
}

inline glm::vec2 roughness_to_alpha(float roughness, float anisotropy)
{
  // Revisiting Physically Based Shading at Imageworks p.24
  glm::vec2 alpha;
  alpha.x = roughness * roughness * (1.0f + anisotropy);
  alpha.y = roughness * roughness * (1.0f - anisotropy);
  return alpha;
}

// convert reflectivity, edge tint to complex IOR
inline void artist_friendly_metallic_fresnel(const glm::vec3& reflectivity,
                                             const glm::vec3& edge_tint,
                                             glm::vec3& n, glm::vec3& k)
{
  // https://jcgt.org/published/0003/04/03/
  const glm::vec3 r_sqrt = glm::sqrt(reflectivity);
  n = edge_tint * (1.0f - reflectivity) / (1.0f + reflectivity) +
      (1.0f - edge_tint) * (1.0f + r_sqrt) / (1.0f - r_sqrt);
  const glm::vec3 t1 = n + 1.0f;
  const glm::vec3 t2 = n - 1.0f;
  k = glm::sqrt((reflectivity * (t1 * t1) - t2 * t2) / (1.0f - reflectivity));
}

class BxDF
{
 public:
  // return sampled direction and BxDF value, pdf
  // wo: view direction in tangent space
  // u: [0, 1] x [0, 1] random number
  // f: BxDF value
  // pdf: pdf value
  // return: sampled direction in tangent space
  virtual glm::vec3 sampleDirection(const glm::vec2& u, const glm::vec3& wo,
                                    glm::vec3& f, float& pdf) const = 0;
};

// Lambert Diffuse BRDF
class Lambert : public BxDF
{
 public:
  Lambert() {}
  Lambert(const glm::vec3& albedo) : m_albedo(albedo) {}

  glm::vec3 sampleDirection(const glm::vec2& u, const glm::vec3& wo,
                            glm::vec3& f, float& pdf) const override
  {
    const glm::vec3 wi = sample_cosine_weighted_hemisphere(u);
    f = m_albedo / M_PIf;
    pdf = abs_cos_theta(wi) / M_PIf;
    return wi;
  }

 private:
  glm::vec3 m_albedo;  // diffuse albedo
};

// Mirror BRDF
class IdealSpecularReflection : public BxDF
{
 public:
  IdealSpecularReflection() {}
  IdealSpecularReflection(const glm::vec3& albedo) : m_albedo(albedo) {}

  glm::vec3 sampleDirection(const glm::vec2& u, const glm::vec3& wo,
                            glm::vec3& f, float& pdf) const override
  {
    const glm::vec3 wi = reflect(wo, glm::vec3(0, 1, 0));
    f = m_albedo / abs_cos_theta(wi);
    pdf = 1.0f;
    return wi;
  }

 private:
  glm::vec3 m_albedo;  // specular albedo
};

class FresnelDielectric
{
 public:
  FresnelDielectric() {}
  FresnelDielectric(float n) : m_n(n) {}

  // evaluate fresnel reflectance
  // cos: cosine between given direction and normal
  float evaluate(float cos) const
  {
    const float temp = m_n * m_n + cos * cos - 1.0f;
    if (temp < 0.0f) { return 1.0f; }

    const float g = sqrtf(temp);
    const float t0 = (g - cos) / (g + cos);
    const float t1 = ((g + cos) * cos - 1.0f) / ((g - cos) * cos + 1.0f);
    return 0.5f * t0 * t0 * (1.0f + t1 * t1);
  }

 private:
  float m_n;  // IOR(index of refraction)
};

class FresnelConductor
{
 public:
  FresnelConductor() {}
  FresnelConductor(const glm::vec3& n, const glm::vec3& k) : m_n(n), m_k(k) {}

 private:
  glm::vec3 m_n;  // real part of IOR
  glm::vec3 m_k;  // imaginary part of IOR
};

// Dielectric Microfacet Reflection
class MicrofacetReflectionDielectric : public BxDF
{
 public:
  MicrofacetReflectionDielectric() {}

 private:
};

// Conductor Microfacet Reflection
class MicrofacetReflectionConductor : public BxDF
{
 public:
  MicrofacetReflectionConductor() {}

 private:
};