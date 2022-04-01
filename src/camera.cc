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
			   float time1)
	: lookfrom_(lookfrom), lookat_(lookat), vup_(vup), focus_dist_(focus_dist) {

	float theta = glm::radians(vfov);
	float h = tan(theta / 2);
	viewport_height_ = 2.0 * h;
	viewport_width_ = aspect_ratio * viewport_height_;

	front_ = glm::normalize(lookat - lookfrom);
	right_ = glm::normalize(glm::cross(front_, vup));
	up_ = glm::cross(right_, front_);

	origin_ = lookfrom;
	horizontal_ = focus_dist * viewport_width_ * right_;
	vertical_ = focus_dist * viewport_height_ * up_;
	lower_left_corner_ = origin_ - horizontal_ / glm::vec3(2.f) - vertical_ / glm::vec3(2.f) + focus_dist * front_;

	lens_radius_ = aperture / 2;
	time0_ = time0;
	time1_ = time1;

	//rng_.seed(12u);
}

Ray Camera::getRay(float s, float t) {
	return Ray(origin_,
			   lower_left_corner_ + s * horizontal_ + t * vertical_ - origin_);
}

//Ray Camera::getRay(float s, float t, bool dof) {
//	glm::vec3 rd = lens_radius_ * RandomInUnitDisk(rng_);
//	glm::vec3 offset = u_ * rd.x + v_ * rd.y;
//	return Ray(
//		origin_ + offset,
//		lower_left_corner_ + s * horizontal_ + t * vertical_ - origin_ - offset,
//		rng_.nextFloat(time0_, time1_)
//	);
//}

void Camera::processKeyboard(CameraMovement movement, float delta_time) {
	float velocity = movement_speed_ * delta_time;
	if (movement == CameraMovement::FORWARD)
		origin_ += front_ * velocity;
	if (movement == CameraMovement::BACKWARD)
		origin_ -= front_ * velocity;
	if (movement == CameraMovement::LEFT)
		origin_ -= right_ * velocity;
	if (movement == CameraMovement::RIGHT)
		origin_ += right_ * velocity;

	updateCameraBase();
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrain_pitch) {
	xoffset *= mouse_sensitivity_;
	yoffset *= mouse_sensitivity_;

	yaw_ += xoffset;
	pitch_ += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrain_pitch) {
		if (pitch_ > 89.0f)
			pitch_ = 89.0f;
		if (pitch_ < -89.0f)
			pitch_ = -89.0f;
	}

	// update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}
void Camera::updateCameraVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front.y = sin(glm::radians(pitch_));
	front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

	// Update vectors
	front_ = glm::normalize(front);
	right_ = glm::normalize(glm::cross(front_, vup_));
	up_ = glm::normalize(glm::cross(right_, front_));

	// Update base
	updateCameraBase();

}

void Camera::updateCameraBase() {
	// Update base
	horizontal_ = focus_dist_ * viewport_width_ * right_;
	vertical_ = focus_dist_ * viewport_height_ * up_;
	lower_left_corner_ = origin_ - horizontal_ / glm::vec3(2.f) - vertical_ / glm::vec3(2.f) + focus_dist_ * front_;
}
