#pragma once
#include <cstdio>

#include "core.h"
#include "glm/glm.hpp"
#include "sampler.h"
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
  // u: [0, 1] x [0, 1] random number
  virtual Ray sampleRay(const glm::vec2& ndc, const glm::vec2& u) const = 0;

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

  Ray sampleRay(const glm::vec2& ndc, const glm::vec2& u) const override
  {
    Ray ret;
    ret.origin = m_origin;
    ret.direction = glm::normalize(ndc.x * m_right + ndc.y * m_up +
                                   m_focal_length * m_forward);
    return ret;
  }

 private:
  float m_focal_length;  // distance from sensor to pinhole
};

class ThinLensCamera : public Camera
{
 public:
  ThinLensCamera(const glm::vec3& origin, const glm::vec3& forward, float fov,
                 float fnumber, float focus_distance)
      : Camera(origin, forward)
  {
    m_focal_length = 1.0f / glm::tan(0.5f * fov);
    m_lens_radius = 2.0f * m_focal_length / fnumber;

    m_b = focus_distance;
    m_a = 1.0f / (1.0f + m_focal_length - 1.0f / m_b);

    spdlog::info("[ThinLensCamera] focal length: {}", m_focal_length);
    spdlog::info("[ThinLensCamera] lens radius: {}", m_lens_radius);
  }

  Ray sampleRay(const glm::vec2& ndc, const glm::vec2& u) const override
  {
    Ray ret;

    const glm::vec3 p_sensor = m_origin - ndc.x * m_right - ndc.y * m_up;
    const glm::vec3 p_lens_center = m_origin + m_focal_length * m_forward;

    const glm::vec2 p_disk = m_lens_radius * sample_uniform_disk(u);
    const glm::vec3 p_lens = p_lens_center + glm::vec3(p_disk, 0.0f);
    const glm::vec3 sensor_to_lens = glm::normalize(p_lens - p_sensor);

    const glm::vec3 sensor_to_lens_center =
        glm::normalize(p_lens_center - p_sensor);
    const glm::vec3 p_object =
        p_sensor + ((m_a + m_b) / glm::dot(sensor_to_lens_center, m_forward)) *
                       sensor_to_lens_center;

    ret.origin = p_lens;
    ret.direction = glm::normalize(p_object - p_lens);
    return ret;
  }

 private:
  float m_lens_radius;   // lens radius
  float m_focal_length;  // distance from sensor to lens
  float m_a;             // lens equation(1/a + 1/b = 1/f) a
  float m_b;             // lens equation b
};