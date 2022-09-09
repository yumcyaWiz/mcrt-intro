#pragma once

#include "glm/glm.hpp"

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
  float tmin;  // minimum hit distance
  float tmax;  // maximum hit distance
};

struct IntersectInfo {
  float t;             // hit distance
  glm::vec3 position;  // hit position
  glm::vec3 normal;    // hit normal
  int primitive_id;    // hit primitive id
};