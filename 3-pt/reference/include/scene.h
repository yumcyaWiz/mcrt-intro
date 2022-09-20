#pragma once
#include <filesystem>
#include <stdexcept>
#include <vector>

#include "glm/glm.hpp"
#include "primitive.h"
#include "shape.h"
#include "spdlog/spdlog.h"
#include "tiny_obj_loader.h"

struct Scene {
  Scene() {}

  void loadObj(const std::filesystem::path& filepath)
  {
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath, reader_config)) {
      if (!reader.Error().empty()) {
        spdlog::error("[tinyobjloader] {}", reader.Error());
      }
      throw std::runtime_error("failed to load " + filepath.generic_string());
    }

    if (!reader.Warning().empty()) {
      spdlog::warn("[tinyobjloader] {}", reader.Warning());
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    const auto& materials = reader.GetMaterials();

    // load materials
    for (const auto& m : materials) {
      const Material material = loadMaterial(m);
      m_materials.push_back(material);
    }

    // shapes loop
    for (size_t s = 0; s < shapes.size(); ++s) {
      size_t index_offset = 0;

      // triangle face loop
      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
        size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

        if (fv != 3) { throw std::runtime_error("not triangulated face"); }

        std::vector<glm::vec3> tri_vertices;
        std::vector<glm::vec3> tri_normals;
        std::vector<glm::vec2> tri_texcoords;

        // triangle vertex loop
        for (size_t v = 0; v < fv; ++v) {
          // vertex position
          tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
          tinyobj::real_t vx =
              attrib.vertices[3 * size_t(idx.vertex_index) + 0];
          tinyobj::real_t vy =
              attrib.vertices[3 * size_t(idx.vertex_index) + 1];
          tinyobj::real_t vz =
              attrib.vertices[3 * size_t(idx.vertex_index) + 2];
          tri_vertices.push_back(glm::vec3(vx, vy, vz));

          // vertex normal
          if (idx.normal_index >= 0) {
            tinyobj::real_t nx =
                attrib.normals[3 * size_t(idx.normal_index) + 0];
            tinyobj::real_t ny =
                attrib.normals[3 * size_t(idx.normal_index) + 1];
            tinyobj::real_t nz =
                attrib.normals[3 * size_t(idx.normal_index) + 2];
            tri_normals.push_back(glm::vec3(nx, ny, nz));
          }

          // vertex texcoord
          if (idx.texcoord_index >= 0) {
            tinyobj::real_t tx =
                attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
            tinyobj::real_t ty =
                attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
            tri_texcoords.push_back(glm::vec2(tx, ty));
          }
        }
        index_offset += fv;

        // when there is no vertex normal, use face normal instead
        if (tri_normals.size() == 0) {
          const glm::vec3 face_normal =
              glm::normalize(glm::cross(tri_vertices[1] - tri_vertices[0],
                                        tri_vertices[2] - tri_vertices[0]));
          tri_normals.push_back(face_normal);
          tri_normals.push_back(face_normal);
          tri_normals.push_back(face_normal);
        }

        // when there is no vertex texcoord, use barycentric coordinate instead
        if (tri_texcoords.size() == 0) {
          tri_texcoords.push_back(glm::vec2(0, 0));
          tri_texcoords.push_back(glm::vec2(1, 0));
          tri_texcoords.push_back(glm::vec2(0, 1));
        }

        // load triangles
        const int triangle_id = m_triangles.size();
        m_triangles.emplace_back(
            tri_vertices[0], tri_vertices[1], tri_vertices[2], tri_normals[0],
            tri_normals[1], tri_normals[2], tri_texcoords[0], tri_texcoords[1],
            tri_texcoords[2]);

        // load primitives
        const int material_id = shapes[s].mesh.material_ids[f];
        m_primitives.emplace_back(&m_triangles[triangle_id],
                                  &m_materials[material_id]);
      }
    }

    spdlog::info("[Scene] number of primitives: {}", m_primitives.size());
    spdlog::info("[Scene] number of materials: {}", m_materials.size());
  }

  static Material loadMaterial(const tinyobj::material_t& m)
  {
    // TODO: implement this
    return Material();
  }

  std::vector<Triangle> m_triangles;
  std::vector<Material> m_materials;
  std::vector<Primitive> m_primitives;
};