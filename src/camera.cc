//
// Created by dante on 24/03/22.
//

#include "camera.h"

#include "common.h"

Camera::Camera(glm::vec3 lookfrom,
			   glm::vec3 lookat,
			   glm::vec3 vup,
			   float vfov,
			   float aspect_ratio,
			   float aperture,
			   float focus_dist,
			   float time0,
			   float time1) {

	float theta = glm::radians(vfov);
	float h = tan(theta/2);
	float viewport_height = 2.0 * h;
	float viewport_width = aspect_ratio * viewport_height;

	w_ = glm::normalize(lookfrom - lookat);
	u_ = glm::normalize(cross(vup, w_));
	v_ = cross(w_, u_);

	origin_ = lookfrom;
	horizontal_ = focus_dist * viewport_width * u_;
	vertical_ = focus_dist * viewport_height * v_;
	lower_left_corner_ = origin_ - horizontal_/glm::vec3(2.f) - vertical_/glm::vec3(2.f) - focus_dist*w_;

	lens_radius_ = aperture / 2;
	time0_ = time0;
	time1_ = time1;

	rng_.seed(12u);
}

Ray Camera::getRay(float s, float t) {
	return Ray(origin_,
			   lower_left_corner_ + s*horizontal_ + t*vertical_ - origin_);
}


Ray Camera::getRay(float s, float t, bool dof) {
	glm::vec3 rd = lens_radius_ * RandomInUnitDisk(rng_);
	glm::vec3 offset = u_ * rd.x + v_ * rd.y;
	return Ray(
		origin_ + offset,
		lower_left_corner_ + s*horizontal_ + t*vertical_ - origin_ - offset,
		rng_.nextFloat(time0_, time1_)
	);
}