//
// Created by dante on 28/03/22.
//

#ifndef TINYRT_SRC_RENDER_CONTEXT_H_
#define TINYRT_SRC_RENDER_CONTEXT_H_

#include "camera.h"
#include "model.h"

#include <embree3/rtcore.h>
#include <glm/vec3.hpp>
#include <omp.h>

constexpr float kEpsilon = 1e-04f;

class RenderContext {
 public:
	RenderContext(int width, int height, int comp  = 3, int n_threads = omp_get_max_threads(), int max_depth = 5);

	void render(std::vector<float> &target);
	void present(std::vector<float> &present_buffer, const std::vector<float> &accumulation_buffer, const uint32_t frame);

	glm::vec3 rayColor(pcg32 &rng, const Ray& ray);
	glm::vec3 rayNormal(pcg32 &rng, const Ray& ray);
	glm::vec3 rayBarycentrics(pcg32 &rng, const Ray& ray);
	glm::vec3 rayAO(pcg32 &rng, const Ray& ray);

	int width_, height_, components_, num_threads_, max_depth_;
	float aspect_ratio_;

	Camera camera_;
	Model model_;
	RTCDevice device_;
	RTCScene scene_;
	std::vector<pcg32> rng_;



};

#endif //TINYRT_SRC_RENDER_CONTEXT_H_
