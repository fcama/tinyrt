//
// Created by dante on 24/03/22.
//

#ifndef TINYRT_SRC_CAMERA_H_
#define TINYRT_SRC_CAMERA_H_

#include "ray.h"

#include <glm/glm.hpp>
#include <pcg32.h>

class Camera {
 public:
	Camera() : Camera(glm::vec3(0,0,-1), glm::vec3(0,0,0), glm::vec3(0,1,0), 40, 1, 0, 10) {};
	Camera(
		glm::vec3 lookfrom,
		glm::vec3 lookat,
		glm::vec3   vup,
		float vfov, // vertical field-of-view in degrees
		float aspect_ratio,
		float aperture = 0,
		float focus_dist = 1,
		float time0 = 0,
		float time1 = 0
	);

	Ray getRay(float s, float t);
	Ray getRay(float s, float t, bool dof);

 private:
	pcg32 rng_;
	glm::vec3 origin_{};
	glm::vec3 lower_left_corner_{};
	glm::vec3 horizontal_{};
	glm::vec3 vertical_{};
	glm::vec3 u_{}, v_{}, w_{};
	float lens_radius_;
	float time0_, time1_;  // shutter open/close times
};

#endif //TINYRT_SRC_CAMERA_H_
