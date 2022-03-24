//
// Created by dante on 24/03/22.
//

#ifndef TINYRT_SRC_RAY_H_
#define TINYRT_SRC_RAY_H_

#include <glm/glm.hpp>

class Ray {
 public:
	Ray() = default;
  	Ray(const glm::vec3 &origin, const glm::vec3 &direction, float time = 0)
		: o_(origin), d_(direction), t_(time) {};

  	[[nodiscard]] glm::vec3 At(float t) const;

	glm::vec3 o_, d_;
	float t_;
};

#endif //TINYRT_SRC_RAY_H_
