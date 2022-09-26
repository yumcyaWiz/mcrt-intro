#pragma once
#include <cstdio>

#include "core.h"
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

class Camera
{
 public:
  Camera(const glm::vec3& origin, const glm::vec3& forward)
      : m_origin(origin), m_forward(forward)

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

  // sample ray from camera
  // ndc(normalized device coordinate): [-1, 1] x [-1, 1]
  virtual Ray sampleRay(const glm::vec2& ndc) const = 0;

 protected:
  glm::vec3 m_origin;   // camera position
  glm::vec3 m_forward;  // camera forward direction
  glm::vec3 m_right;    // camera right direction
  glm::vec3 m_up;       // camera up direction
};

class PinholeCamera : public Camera
{
 public:
  PinholeCamera(const glm::vec3& origin, const glm::vec3& forward, float fov)
      : Camera(origin, forward)
  {
    m_focal_length = 1.0f / glm::tan(0.5f * fov);
  }

  Ray sampleRay(const glm::vec2& ndc) const override
  {
    Ray ret;
    ret.origin = m_origin;
    ret.direction = glm::normalize(ndc.x * m_right + ndc.y * m_up +
                                   m_focal_length * m_forward);
    return ret;
  }

 private:
  float m_focal_length;
};