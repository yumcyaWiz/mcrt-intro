#pragma once
#include <cstdio>

#include "core.h"
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

class Camera
{
 public:
  Camera(const glm::vec3& origin_, const glm::vec3& forward_)
      : m_origin(origin_), m_forward(forward_)

  {
    // calculate right, up direction from forward direction
    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0, 1, 0)));
    m_up = glm::normalize(glm::cross(m_right, m_forward));

    spdlog::info("[Camera] Origin: ({}, {}, {})", m_origin.x, m_origin.y,
                 m_origin.z);
    spdlog::info("[Camera] Forward: ({}, {}, {})", m_forward.x, m_forward.y,
                 m_forward.z);
    spdlog::info("[Camera] Right: ({}, {}, {})", m_right.x, m_right.y,
                 m_right.z);
    spdlog::info("[Camera] Up: ({}, {}, {})", m_up.x, m_up.y, m_up.z);
  }

  // sample ray from pinhole camera
  // ndc(normalized device coordinate): [-1, 1] x [-1, 1]
  Ray sampleRay(const glm::vec2& ndc) const
  {
    Ray ret;
    ret.origin = m_origin;
    ret.direction = glm::normalize(ndc.x * m_right + ndc.y * m_up + m_forward);
    return ret;
  }

 private:
  glm::vec3 m_origin;   // camera position
  glm::vec3 m_forward;  // camera forward direction
  glm::vec3 m_right;    // camera right direction
  glm::vec3 m_up;       // camera up direction
};