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
enum class RenderOutput {CAMERA, NORMALS, BARYCENTRICS, AMBIENT_OCCLUSION };

class RenderContext {
 public:
	RenderContext(int width, int height, int comp  = 3, int n_threads = omp_get_max_threads(), int max_depth = 5);

	void render(std::vector<float> &target);
	template <class F>
	void render(std::vector<float> &target, F rayTarget)
	{
		#pragma omp parallel for schedule(dynamic) num_threads(num_threads_) default(none) shared(target, rayTarget)
		for (int j = height_-1; j >= 0; --j)
		{
			for (int i = 0; i < width_; ++i)
			{
				auto u = float(i + rng_[omp_get_thread_num()].nextFloat()) / (width_-1);
				auto v = float(j + rng_[omp_get_thread_num()].nextFloat()) / (height_-1);

				Ray r = camera_.getRay(u, v);

				glm::vec3 color = rayTarget(rng_[omp_get_thread_num()], r);

				int index = ((height_-1-j) * (width_) + i) * components_;
				target[index]   += color.r;
				target[index+1] += color.g;
				target[index+2] += color.b;
			}
		}
	}


	glm::vec3 rayColor(pcg32 &rng, const Ray& ray);
	glm::vec3 rayNormal(pcg32 &rng, const Ray& ray);
	glm::vec3 rayBarycentrics(pcg32 &rng, const Ray& ray);
	glm::vec3 rayAO(pcg32 &rng, const Ray& ray);

	int width_, height_, components_, num_threads_, max_depth_;
	float aspect_ratio_;
	uint32_t accumulation_frames;
	std::vector<float> accumulation_buffer;

	Camera camera_;
	Model model_;
	RTCDevice device_;
	RTCScene scene_;
	std::vector<pcg32> rng_;

	RenderOutput current_output_;
};

#endif //TINYRT_SRC_RENDER_CONTEXT_H_
