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

enum class MatType { DIFFUSE = 0, REFLECTIVE, DIELECTRIC, GLOSSY, EMISSIVE, INVALID };
enum class PatternType { SOLID, CHECKERBOARD };

struct Texture
{
	unsigned char * data = nullptr;
	int width, height, comp;
};

class Material {
 public:
	Material(MatType material_type, const tinyobj::material_t *tiny_material, PatternType pattern_type = PatternType::SOLID)
		: material_type_(material_type), tiny_material_(tiny_material), pattern_type_(pattern_type) {}

	MatType material_type_;
	const tinyobj::material_t *tiny_material_;
	PatternType pattern_type_;

	// Glossy Properties
	// What percentage of the light that hits this object is going to be reflected specularly
	// instead of diffusely
	float percent_specular_; //[0,1]
	// Controls the roughness of the surface, 0 is mirror-like reflection, 1 is fully diffuse
	float roughness_; //[0,1]
	// Color of specular reflection
	glm::vec3 specular_color_;

	// diffuse texture
	Texture* tex_diffuse_ = nullptr;

	[[nodiscard]] glm::vec3 evaluatePattern(const RTCRayHit &ray_hit) const;
};

bool EvaluateMaterial(pcg32& rng, const RTCRayHit& ray_hit, std::vector<glm::vec2> &uvs, const Material &mat, Ray &scattered, glm::vec3 &output_color);

#endif //TINYRT_SRC_MATERIAL_H_
