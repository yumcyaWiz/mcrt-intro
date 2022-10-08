#pragma once

#include "core.h"
#include "shape.h"

// primitive holds pointer of shape and material
struct Primitive {
  const Shape* shape;
  const Material* material;

  Primitive(const Shape* shape_, const Material* material_)
      : shape(shape_), material(material_)
  {
  }

  // find ray intersection
  bool intersect(const Ray& ray, IntersectInfo& info) const
  {
    if (shape->intersect(ray, info)) {
      info.primitive = this;
      return true;
    }

    return false;
  }
};