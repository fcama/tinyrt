//
// Created by dante on 25/03/22.
//

#include "material.h"
#include "common.h"

/* Evaluates the material hit and creates a scattered ray */
/* @param \in rng Random - Number Generator */
/* @param \in RTCRayHit  - Embree RayHit structure of incoming ray */
/* @param \in Material   - Material of the triangle hit by the incoming ray */
/* @param \out Ray       - The scattered ray */
/* @param \out vec3      - The color of the triangle hit */
/*                                           */
/* @return true if a scattered ray is generated, false otherwise */
bool EvaluateMaterial(pcg32 &rng,
					  const RTCRayHit &ray_hit,
					  const Material &mat,
					  Ray &scattered,
					  glm::vec3 &color) {
	
	const glm::vec3 kGeometryNormal = glm::vec3(ray_hit.hit.Ng_x, ray_hit.hit.Ng_y, ray_hit.hit.Ng_z);
	const glm::vec3 kShadingNormal = graphics::CalcShadingNormal(glm::vec3(ray_hit.ray.dir_x, ray_hit.ray.dir_y, ray_hit.ray.dir_z), kGeometryNormal);

	color = mat.evaluatePattern(ray_hit);

	glm::vec3 hit_point = glm::vec3(ray_hit.ray.org_x + ray_hit.ray.tfar * ray_hit.ray.dir_x,
								  ray_hit.ray.org_y + ray_hit.ray.tfar * ray_hit.ray.dir_y,
								  ray_hit.ray.org_z + ray_hit.ray.tfar * ray_hit.ray.dir_z);
	glm::vec3 direction(0);

	switch(mat.material_type_)
	{
		case MatType::DIFFUSE:
		{
			direction = graphics::GetCosHemisphereSample(rng, kShadingNormal);
			break;
		}
		case MatType::REFLECTIVE:
		{
			// glm::vec3 fuzz(random_in_unit_sphere(rng), random_in_unit_sphere(rng), random_in_unit_sphere(rng));
			// fuzz *= 0.2; 
			direction = glm::reflect(glm::vec3(ray_hit.ray.dir_x, ray_hit.ray.dir_y, ray_hit.ray.dir_z), kShadingNormal);
			// direction += fuzz;
			break;
		}
		case MatType::DIELECTRIC:
		{
			// DIELECTRIC default color is white
			color = glm::vec3(1);
			constexpr float kIr = 1.5f;

			const glm::vec3 kRayDirection = glm::vec3(ray_hit.ray.dir_x, ray_hit.ray.dir_y, ray_hit.ray.dir_z);
			//const float kRefractionRatio = glm::dot(kRayDirection, geomNormal) ? (1.0/kIr) : kIr;
			const float kRefractionRatio = glm::dot(kRayDirection, kShadingNormal) ? (1.0/kIr) : kIr;

			float cos_theta = fmin(dot(-kRayDirection, kShadingNormal), 1.0f);
			float sin_theta = sqrt(1.0f - cos_theta*cos_theta);

			bool cannot_refract = kRefractionRatio * sin_theta > 1.0;

			if (cannot_refract || graphics::Shlick(cos_theta, kRefractionRatio) > rng.nextFloat())
				direction = glm::reflect(kRayDirection, kShadingNormal);
			else
				direction = glm::refract(kRayDirection, kShadingNormal, kRefractionRatio);

			break;
		}
		case MatType::EMISSIVE:
		{
			return false;
			break;
		}
		default:
		{
			// invalid material -- unreachable
			break;
		}
	}

	scattered = Ray(hit_point, direction);
	return true;
}

glm::vec3 Material::evaluatePattern(const RTCRayHit &ray_hit) const {
	auto diffuse = glm::vec3(tiny_material_->diffuse[0], tiny_material_->diffuse[1], tiny_material_->diffuse[2]);
	auto ambient = glm::vec3(tiny_material_->ambient[0], tiny_material_->ambient[1], tiny_material_->ambient[2]);

	switch (pattern_type_) {
		case (PatternType::SOLID) :
		{
			return diffuse;
		}
		case (PatternType::CHECKERBOARD) :
		{
			auto &odd = diffuse;
			auto &even = ambient;

//			auto &u = ray_hit.hit.u;
//			auto &v = ray_hit.hit.v;
			glm::vec3 hit_point = glm::vec3(ray_hit.ray.org_x + ray_hit.ray.tfar * ray_hit.ray.dir_x,
											ray_hit.ray.org_y + ray_hit.ray.tfar * ray_hit.ray.dir_y,
											ray_hit.ray.org_z + ray_hit.ray.tfar * ray_hit.ray.dir_z);

			auto sines = sin(10*hit_point.x)*sin(10*hit_point.y)*sin(10*hit_point.z);
			if (sines < 0)
				return odd;
			else
				return even;
		}
	}
}
