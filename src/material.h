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
enum class PatternType { SOLID, CHECKERBOARD };

class Material {
 public:
	Material(MatType material_type, const tinyobj::material_t *tiny_material, PatternType pattern_type = PatternType::SOLID)
		: material_type_(material_type), tiny_material_(tiny_material), pattern_type_(pattern_type) {}

	MatType material_type_;
	const tinyobj::material_t *tiny_material_;
	PatternType pattern_type_;


	[[nodiscard]] glm::vec3 evaluatePattern(const RTCRayHit &ray_hit) const;
};

bool EvaluateMaterial(pcg32& rng, const RTCRayHit& ray_hit, const Material &mat, Ray &scattered, glm::vec3 &color);

#endif //TINYRT_SRC_MATERIAL_H_
