//
// Created by dante on 25/03/22.
//

#ifndef TINYRT_SRC_MATERIAL_H_
#define TINYRT_SRC_MATERIAL_H_

#include <tinyobjloader/tiny_obj_loader.h>
#include <pcg32.h>
#include <embree3/rtcore.h>
#include <glm/glm.hpp>
#include "ray.h"

enum class MatType { DIFFUSE = 0, REFLECTIVE, DIELECTRIC, EMISSIVE, INVALID };

class Material {
 public:
	Material(MatType material_type, const tinyobj::material_t *tiny_material)
		: material_type_(material_type), tiny_material_(tiny_material) {}

	MatType material_type_;
	const tinyobj::material_t *tiny_material_;
};

bool EvaluateMaterial(pcg32& rng, const RTCRayHit& ray_hit, const Material &mat, Ray &scattered, glm::vec3 &color);

#endif //TINYRT_SRC_MATERIAL_H_
