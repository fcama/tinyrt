//
// Created by dante on 24/03/22.
//

#ifndef TINYRT_SRC_COMMON_H_
#define TINYRT_SRC_COMMON_H_

#include <cmath>
#include <glm/glm.hpp>
#include <pcg32/pcg32.h>
#include <cstdio>

namespace graphics
{

inline glm::vec3 RandomInUnitSphere(pcg32 &rng) {
	while (true) {
		auto p = glm::vec3(rng.nextFloat(-1,1), rng.nextFloat(-1,1), rng.nextFloat(-1,1));
		if (glm::dot(p, p) >= 1) continue;

		return p;
	}
}

inline glm::vec3 RandomInUnitDisk(pcg32 &rng) {
	while (true) {
		auto p = glm::vec3(rng.nextFloat(-1,1), rng.nextFloat(-1,1), 0);
		if (glm::dot(p, p) >= 1) continue;

		return p;
	}
}

inline glm::vec3 LessThan(glm::vec3 f, float value)
{
	return {
		(f.x < value) ? 1.0f : 0.0f,
		(f.y < value) ? 1.0f : 0.0f,
		(f.z < value) ? 1.0f : 0.0f
	};
}

inline glm::vec3 SRGBToLinear(glm::vec3 rgb)
{
	rgb = clamp(rgb, 0.0f, 1.0f);

	return mix(
		pow(((rgb + 0.055f) / 1.055f), glm::vec3(2.4f)),
		rgb / 12.92f,
		LessThan(rgb, 0.04045f)
	);
}


// Utility function to get a vector perpendicular to an input vector
//    (from "Efficient Construction of Perpendicular Vectors Without Branching")
[[nodiscard]] inline glm::vec3 GetPerpendicularVector(const glm::vec3 &u)
{
	glm::vec3 a = glm::vec3(std::abs(u.x), std::abs(u.y), std::abs(u.z));
	uint xm = ((a.x - a.y) < 0 && (a.x - a.z) < 0) ? 1 : 0;
	uint ym = (a.y - a.z) < 0 ? (1 ^ xm) : 0;
	uint zm = 1 ^ (xm | ym);
	return cross(u, glm::vec3(xm, ym, zm));
}

// Get a cosine-weighted random vector centered around a specified normal direction.
// Credit: Chris Wyman
[[nodiscard]] inline glm::vec3 GetCosHemisphereSample(pcg32 &rng, const glm::vec3 &hit_norm)
{
	// Get 2 random numbers to select our sample with
	glm::vec3 rand_val = glm::vec3(rng.nextFloat(), rng.nextFloat(), 0);

	// Cosine weighted hemisphere sample from RNG
	glm::vec3 bitangent = GetPerpendicularVector(hit_norm);
	glm::vec3 tangent = cross(bitangent, hit_norm);
	float r = std::sqrt(rand_val.x);
	float phi = 2.0f * 3.14159265f * rand_val.y;

	// Get our cosine-weighted hemisphere lobe sample direction
	return tangent * glm::vec3(r * std::cos(phi)) + bitangent * glm::vec3(r * std::sin(phi)) + hit_norm * glm::vec3(std::sqrt(1 - rand_val.x));
}

[[nodiscard]] inline glm::vec3 CalcShadingNormal(const glm::vec3 &ray, const glm::vec3 &geom_normal)
{
	auto ret = glm::normalize(geom_normal);
	if (glm::dot(ray, geom_normal) < 0)
		return ret;

	return -ret;
}

[[nodiscard]] inline float Shlick(float cosine, float ref_idx) {
	// Use Schlick's approximation for reflectance.
	auto r0 = (1-ref_idx) / (1+ref_idx);
	r0 = r0*r0;
	return r0 + (1-r0)*pow((1 - cosine),5);
}
}

#endif //TINYRT_SRC_COMMON_H_