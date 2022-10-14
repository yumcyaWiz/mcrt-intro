#pragma once
#include <stdexcept>

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

// -----
// microfacet functions

// GGX NDF(Normal Distribution Function)
// wh: half-vector
inline float ggx_ndf(const glm::vec3& wh, const glm::vec2& alpha)
{
  const float t = wh.x * wh.x / (alpha.x * alpha.x) +
                  wh.z * wh.z / (alpha.y * alpha.y) + wh.y * wh.y;
  return 1.0f / (M_PIf * alpha.x * alpha.y * t * t);
}

// GGX shadowing-masking lambda
// w: direction
inline float ggx_lambda(const glm::vec3& w, const glm::vec2& alpha)
{
  const float t =
      (alpha.x * alpha.x * w.x * w.x + alpha.y * alpha.y * w.z * w.z) /
      (w.y * w.y);
  return 0.5f * (-1.0f + glm::sqrt(1.0f + t));
}

// GGX shadowing-masking G1
// w: direction
inline float ggx_g1(const glm::vec3& w, const glm::vec2& alpha)
{
  return 1.0f / (1.0f + ggx_lambda(w, alpha));
}

// GGX shadowing-masking G2
// wo: view direction
// wi: incident direction
inline float ggx_g2(const glm::vec3& wo, const glm::vec3& wi,
                    const glm::vec2& alpha)
{
  return 1.0f / (1.0f + ggx_lambda(wo, alpha) + ggx_lambda(wi, alpha));
}

// GGX VNDF(Visible Normal Distribution Function)
// w: direction
// wh: half-vector
inline float ggx_vndf(const glm::vec3& w, const glm::vec3& wh,
                      const glm::vec2& alpha)
{
  return ggx_g1(w, alpha) * glm::abs(glm::dot(w, wh)) * ggx_ndf(wh, alpha) /
         abs_cos_theta(w);
}

// sample direction from GGX VNDF
// u: [0, 1] x [0, 1] random number
// wo: view direction
inline glm::vec3 sample_ggx_vndf(const glm::vec2& u, const glm::vec3& wo,
                                 const glm::vec2& alpha)
{
  // https://jcgt.org/published/0007/04/01/
  const glm::vec3 Vh =
      glm::normalize(glm::vec3(alpha.x * wo.x, wo.y, alpha.y * wo.z));

  const float lensq = Vh.x * Vh.x + Vh.z * Vh.z;
  const glm::vec3 T1 = lensq > 0 ? glm::vec3(Vh.z, 0, -Vh.x) / glm::sqrt(lensq)
                                 : glm::vec3(0, 0, 1);
  const glm::vec3 T2 = glm::cross(Vh, T1);

  const float r = glm::sqrt(u.x);
  const float phi = 2.0f * M_PIf * u.y;
  const float t1 = r * glm::cos(phi);
  float t2 = r * glm::sin(phi);
  const float s = 0.5f * (1.0f + Vh.y);
  t2 = (1.0f - s) * glm::sqrt(glm::max(1.0f - t1 * t1, 0.0f)) + s * t2;
  const glm::vec3 Nh = t1 * T1 + t2 * T2 +
                       glm::sqrt(glm::max(1.0f - t1 * t1 - t2 * t2, 0.0f)) * Vh;
  const glm::vec3 Ne = glm::normalize(
      glm::vec3(alpha.x * Nh.x, glm::max(0.0f, Nh.y), alpha.y * Nh.z));

  return Ne;
}

// VNDF sampling pdf
inline float sample_ggx_vndf_pdf(const glm::vec3& wo, const glm::vec3& wh,
                                 const glm::vec2& alpha)
{
  return 0.25f * ggx_vndf(wo, wh, alpha) / glm::abs(glm::dot(wo, wh));
}

// -----

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

// dielectric fresnel reflectance
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

// conductor fresnel reflectance
class FresnelConductor
{
 public:
  FresnelConductor() {}
  FresnelConductor(const glm::vec3& n, const glm::vec3& k) : m_n(n), m_k(k) {}

  // evaluate fresnel reflectance
  // cos: cosine between given direction and normal
  glm::vec3 evaluate(float cos) const
  {
    const float c2 = cos * cos;
    const glm::vec3 two_eta_cos = 2.0f * m_n * cos;

    const glm::vec3 t0 = m_n * m_n + m_k * m_k;
    const glm::vec3 t1 = t0 * c2;
    const glm::vec3 Rs = (t0 - two_eta_cos + c2) / (t0 + two_eta_cos + c2);
    const glm::vec3 Rp = (t1 - two_eta_cos + 1.0f) / (t1 + two_eta_cos + 1.0f);

    return 0.5f * (Rp + Rs);
  }

 private:
  glm::vec3 m_n;  // real part of IOR
  glm::vec3 m_k;  // imaginary part of IOR
};

// Dielectric Microfacet Reflection
class MicrofacetReflectionDielectric : public BxDF
{
 public:
  MicrofacetReflectionDielectric() {}
  MicrofacetReflectionDielectric(float ior, float roughness, float anisotropy)
      : m_fresnel(ior)
  {
    m_alpha = roughness_to_alpha(roughness, anisotropy);
  }

  glm::vec3 sampleDirection(const glm::vec2& u, const glm::vec3& wo,
                            glm::vec3& f, float& pdf) const override
  {
    // TODO: implement this
    throw std::runtime_error("not implemented");

    // sample half-vector

    // compute incident direction

    // evaluate BRDF

    // evaluate pdf
  }

 private:
  FresnelDielectric m_fresnel;
  glm::vec2 m_alpha;
};

// Conductor Microfacet Reflection
class MicrofacetReflectionConductor : public BxDF
{
 public:
  MicrofacetReflectionConductor() {}
  MicrofacetReflectionConductor(const glm::vec3& n, const glm::vec3& k,
                                float roughness, float anisotropy)
      : m_fresnel(n, k)
  {
    m_alpha = roughness_to_alpha(roughness, anisotropy);
  }

  glm::vec3 sampleDirection(const glm::vec2& u, const glm::vec3& wo,
                            glm::vec3& f, float& pdf) const override
  {
    // TODO: implement this
    throw std::runtime_error("not implemented");

    // sample half-vector

    // compute incident direction

    // evaluate BRDF

    // evaluate pdf
  }

 private:
  FresnelConductor m_fresnel;
  glm::vec2 m_alpha;
};