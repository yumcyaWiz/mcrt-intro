#pragma once

#include <cstdint>
#include <iterator>
#include <stdexcept>

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

// uniform hemisphere sampling
inline glm::vec3 sample_hemisphere(const glm::vec2& u)
{
  // TODO: implement this
  throw std::runtime_error("not implemented");
}

// cosine weighted hemisphere sampling
inline glm::vec3 sample_cosine_weighted_hemisphere(const glm::vec2& u)
{
  // TODO: implement this
  throw std::runtime_error("not implemented");
}