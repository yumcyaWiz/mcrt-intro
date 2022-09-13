#pragma once
#include "glm/glm.hpp"
#include "sampler.h"

inline float cos_theta(const glm::vec3& w) { return w.y; }
inline float cos2_theta(const glm::vec3& w) { return w.y * w.y; }
inline float abs_cos_theta(const glm::vec3& w) { return glm::abs(w.y); }

class BxDF
{
 public:
  // return sampled direction and BxDF value, pdf
  // u: [0, 1] x [0, 1] random number
  // f: BxDF value
  // pdf: pdf value
  virtual glm::vec3 sampleDirection(const glm::vec2& u, glm::vec3& f,
                                    float& pdf) const = 0;
};

class Lambert : public BxDF
{
 public:
  Lambert(const glm::vec3& albedo) : m_albedo(albedo) {}

  glm::vec3 sampleDirection(const glm::vec2& u, glm::vec3& f,
                            float& pdf) const override
  {
    const glm::vec3 wi = sample_cosine_weighted_hemisphere(u);
    f = m_albedo / M_PIf;
    pdf = abs_cos_theta(wi) / M_PIf;

    return wi;
  }

 private:
  glm::vec3 m_albedo;  // diffuse albedo
};