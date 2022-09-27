#pragma once
#include <memory>

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

  // get bounding box
  AABB getBounds() const { return shape->getBounds(); }

  // has emission or not
  bool has_emission() const
  {
    return material->ke.x > 0.0f || material->ke.y > 0.0f ||
           material->ke.z > 0.0f;
  }
};