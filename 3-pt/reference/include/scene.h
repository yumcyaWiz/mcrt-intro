#pragma once
#include <filesystem>
#include <stdexcept>
#include <vector>

#include "core.h"
#include "glm/glm.hpp"
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
        spdlog::error("[tinyobjloader]: {}", reader.Error());
      }
      throw std::runtime_error("failed to load " + filepath.generic_string());
    }

    if (!reader.Warning().empty()) {
      spdlog::warn("[tinyobjloader]: {}", reader.Warning());
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

        // load vertices
        m_vertices.push_back(tri_vertices[0]);
        m_vertices.push_back(tri_vertices[1]);
        m_vertices.push_back(tri_vertices[2]);

        // load indices
        const uint32_t n_faces = m_indices.size();
        m_indices.push_back(
            glm::uvec3(3 * n_faces + 0, 3 * n_faces + 1, 3 * n_faces + 2));

        // load normals
        m_normals.push_back(tri_normals[0]);
        m_normals.push_back(tri_normals[1]);
        m_normals.push_back(tri_normals[2]);

        // load texcoords
        m_texcoords.push_back(tri_texcoords[0]);
        m_texcoords.push_back(tri_texcoords[1]);
        m_texcoords.push_back(tri_texcoords[2]);

        // load material id
        const auto material_id = shapes[s].mesh.material_ids[f];
        m_material_ids.push_back(material_id);
      }
    }
  }

  static Material loadMaterial(const tinyobj::material_t& m)
  {
    // TODO: implement this
    return Material();
  }

  std::vector<glm::vec3> m_vertices;
  std::vector<glm::uvec3> m_indices;
  std::vector<glm::vec3> m_normals;
  std::vector<glm::vec2> m_texcoords;

  std::vector<Material> m_materials;
  std::vector<uint32_t> m_material_ids;  // per face material id
};