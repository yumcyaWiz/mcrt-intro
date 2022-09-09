#include <cstdio>

#include "vec3.h"

int main()
{
  Vec3 v1(0, 0, 0);
  Vec3 v2(1, 1, 1);
  const Vec3 v = v1 + v2;
  printf("%f %f %f\n", v.x, v.y, v.z);
  return 0;
}