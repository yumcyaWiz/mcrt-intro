#pragma once
#include <memory>

#include "core.h"
#include "shape.h"

// primitive holds pointer of shape and material
struct Primitive {
  std::shared_ptr<Shape> shape;
  std::shared_ptr<Material> material;

  Primitive(const std::shared_ptr<Shape>& shape_,
            const std::shared_ptr<Material>& material_)
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