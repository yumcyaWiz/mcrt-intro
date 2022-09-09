#include <cstdio>

#include "glm/glm.hpp"

int main()
{
  glm::vec3 v1(0, 0, 0);
  glm::vec3 v2(1, 1, 1);
  const glm::vec3 v = v1 + v2;
  printf("%f %f %f\n", v.x, v.y, v.z);
  return 0;
}