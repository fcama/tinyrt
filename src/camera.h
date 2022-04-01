//
// Created by dante on 24/03/22.
//

#ifndef TINYRT_SRC_CAMERA_H_
#define TINYRT_SRC_CAMERA_H_

#include "ray.h"

#include <glm/glm.hpp>
#include <pcg32.h>

enum class CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
 public:
	Camera() : Camera(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 40, 1, 0, 10) {};
	Camera(
		glm::vec3 lookfrom,
		glm::vec3 lookat,
		glm::vec3 vup,
		float vfov, // vertical field-of-view in degrees
		float aspect_ratio,
		float aperture = 0,
		float focus_dist = 1,
		float time0 = 0,
		float time1 = 0
	);

	void processKeyboard(CameraMovement movement, float delta_time);
	void processMouseMovement(float xoffset, float yoffset, bool constrain_pitch = true);
	void updateCameraVectors();
	void updateCameraBase();
	Ray getRay(float s, float t);
	//Ray getRay(float s, float t, bool dof);

 public:
	//pcg32 rng_;
	glm::vec3 lookfrom_{}, lookat_{}, vup_{};
	glm::vec3 origin_{}, lower_left_corner_{}, horizontal_{}, vertical_{};

	glm::vec3 front_{}, up_{}, right_{};
	float viewport_width_, viewport_height_;
	float vfov_;
	float focus_dist_;
	float lens_radius_;
	float time0_, time1_;  // shutter open/close times

	// Camera control
	float yaw_ = -90.0f;
	float pitch_ = 0.0f;
	float movement_speed_ = 0.0005f * 4;// * 0.001;
	float mouse_sensitivity_ = 0.1f;
	float zoom_ = 45.0f;
};

#endif //TINYRT_SRC_CAMERA_H_
