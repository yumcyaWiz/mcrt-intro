#pragma once
#include <stdexcept>

struct Vec3 {
  float x;
  float y;
  float z;

  Vec3() { x = y = z = 0.0f; }
  Vec3(float x_) { x = y = z = x_; }
  Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

  float operator[](int i) const
  {
    if (i == 0)
      return x;
    else if (i == 1)
      return y;
    else if (i == 2)
      return z;
    else
      throw std::runtime_error("out of bounds");
  }

  float &operator[](int i)
  {
    if (i == 0)
      return x;
    else if (i == 1)
      return y;
    else if (i == 2)
      return z;
    else
      throw std::runtime_error("out of bounds");
  }
};

inline Vec3 operator+(const Vec3 &v1, const Vec3 &v2)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = v1[i] + v2[i]; }
  return ret;
}

inline Vec3 operator+(const Vec3 &v, float k)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = v[i] + k; }
  return ret;
}

inline Vec3 operator+(float k, const Vec3 &v)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = k + v[i]; }
  return ret;
}

inline Vec3 operator-(const Vec3 &v1, const Vec3 &v2)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = v1[i] - v2[i]; }
  return ret;
}

inline Vec3 operator-(const Vec3 &v, float k)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = v[i] - k; }
  return ret;
}

inline Vec3 operator-(float k, const Vec3 &v)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = k - v[i]; }
  return ret;
}

inline Vec3 operator*(const Vec3 &v1, const Vec3 &v2)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = v1[i] * v2[i]; }
  return ret;
}

inline Vec3 operator*(const Vec3 &v, float k)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = v[i] * k; }
  return ret;
}

inline Vec3 operator*(float k, const Vec3 &v)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = k * v[i]; }
  return ret;
}

inline Vec3 operator/(const Vec3 &v1, const Vec3 &v2)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = v1[i] / v2[i]; }
  return ret;
}

inline Vec3 operator/(const Vec3 &v, float k)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = v[i] / k; }
  return ret;
}

inline Vec3 operator/(float k, const Vec3 &v)
{
  Vec3 ret;
  for (int i = 0; i < 3; ++i) { ret[i] = k / v[i]; }
  return ret;
}