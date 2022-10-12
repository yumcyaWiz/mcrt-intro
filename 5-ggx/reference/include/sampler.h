#pragma once

#include <cstdint>
#include <iterator>

#include "core.h"
#include "glm/glm.hpp"

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
typedef struct {
  uint64_t state;
  uint64_t inc;
} pcg32_random_t;

inline uint32_t pcg32_random_r(pcg32_random_t* rng)
{
  uint64_t oldstate = rng->state;
  // Advance internal state
  rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
  // Calculate output function (XSH RR), uses old state for max ILP
  uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  uint32_t rot = oldstate >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

class Sampler
{
 public:
  Sampler(uint64_t seed)
  {
    state.state = seed;
    state.inc = 0xdeadbeef;

    // warm up
    for (int i = 0; i < 10; ++i) { next_1d(); }
  }

  // generate 1d float sample
  float next_1d() { return pcg32_random_r(&state) * (1.0f / (1ULL << 32)); }

  // generate 2d float sample
  glm::vec2 next_2d() { return glm::vec2(next_1d(), next_1d()); }

 private:
  pcg32_random_t state;
};

// uniform disk sampling
inline glm::vec2 sample_uniform_disk(const glm::vec2& u)
{
  const float r = glm::sqrt(u.x);
  const float theta = 2.0f * M_PIf * u.y;
  return r * glm::vec2(glm::cos(theta), glm::sin(theta));
}

// uniform hemisphere sampling
inline glm::vec3 sample_hemisphere(const glm::vec2& u)
{
  const float theta = glm::acos(glm::clamp(1.0f - u[0], -1.0f, 1.0f));
  const float phi = 2.0f * M_PIf * u[1];
  return spherical_to_cartesian(phi, theta);
}

// cosine weighted hemisphere sampling
inline glm::vec3 sample_cosine_weighted_hemisphere(const glm::vec2& u)
{
  const float theta =
      0.5f * glm::acos(glm::clamp(1.0f - 2.0f * u[0], -1.0f, 1.0f));
  const float phi = 2.0f * M_PIf * u[1];
  return spherical_to_cartesian(phi, theta);
}

class DiscreteDistribution1D
{
 public:
  DiscreteDistribution1D() {}
  DiscreteDistribution1D(const float* values, int size)
  {
    m_cdf.resize(size + 1);

    float sum = 0.0f;
    for (int i = 0; i < size; ++i) { sum += values[i]; }

    // compute cdf
    m_cdf[0] = 0.0f;
    for (int i = 1; i < size + 1; ++i) {
      m_cdf[i] = m_cdf[i - 1] + values[i - 1] / sum;
    }
  }

  int sample(float u, float& pmf) const
  {
    const int idx = binary_search(m_cdf.data(), m_cdf.size(), u);
    pmf = m_cdf[idx + 1] - m_cdf[idx];
    return idx;
  }

 private:
  static int binary_search(const float* values, int size, float value)
  {
    int idx_min = 0;
    int idx_max = size - 1;
    while (idx_max >= idx_min) {
      const int idx_mid = (idx_min + idx_max) / 2;
      const float mid = values[idx_mid];
      if (value < mid) {
        idx_max = idx_mid - 1;
      } else if (value > mid) {
        idx_min = idx_mid + 1;
      } else {
        return idx_mid;
      }
    }
    return idx_max;
  }

  std::vector<float> m_cdf;
};