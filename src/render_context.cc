//
// Created by dante on 28/03/22.
//

#include <iostream>
#include "render_context.h"



RenderContext::RenderContext(int width, int height, int comp, int n_threads, int max_depth)
	: width_(width), height_(height), components_(comp), num_threads_(n_threads), max_depth_(max_depth) {

	aspect_ratio_ = float(width_) / height_;
	camera_ = Camera(glm::vec3(0.0, 1.0, 5.15), glm::vec3(0,1,0), glm::vec3(0,1,0), 26.99, aspect_ratio_);
	model_ = LoadObjFile("models/cornellbox/cornellbox.obj", "models/cornellbox");

//	for (int i = 0; i < num_threads_; ++i)
//	{
//		rng_.push_back(pcg32());
//	}
	rng_.resize(num_threads_);
	std::vector<uint64_t> initstate(num_threads_, PCG32_DEFAULT_STATE);
	std::vector<uint64_t> initseq(num_threads_);
	for (size_t i = 0; i < initseq.size(); ++i)
	{
		initseq[i] = i + 1;
	}
	for (int i = 0; i < num_threads_; ++i)
	{
		rng_[i].seed(initstate[i], initseq[i]);
	}

	// Embree code
	device_ = rtcNewDevice(nullptr);
	scene_  = rtcNewScene(device_);
	RTCGeometry geom = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_TRIANGLE);

	std::cout << model_;

	float* vb = (float*) rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3*sizeof(float), model_.vertices_.size()/3);
	for (size_t i = 0; i < model_.vertices_.size(); ++i)
	{
		vb[i] = model_.vertices_[i];
	}

	unsigned* ib = (unsigned*) rtcSetNewGeometryBuffer(geom,
													   RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3*sizeof(unsigned), model_.indices_.size()/3);
	for (size_t i = 0; i < model_.indices_.size(); ++i)
	{
		ib[i] = model_.indices_[i];
	}

	rtcCommitGeometry(geom);
	rtcAttachGeometry(scene_, geom);
	rtcReleaseGeometry(geom);

	rtcSetSceneBuildQuality(scene_, RTC_BUILD_QUALITY_HIGH);
	rtcCommitScene(scene_);

}

void RenderContext::render(std::vector<float> &target) {
	#pragma omp parallel for schedule(dynamic) num_threads(num_threads_) default(none) shared(target)
	for (int j = height_-1; j >= 0; --j)
	{
		for (int i = 0; i < width_; ++i)
		{
			glm::vec3 color(0);

			int threadN = 0;//omp_get_thread_num();
			//std::cout << omp_get_thread_num() << std::endl;
			auto u = float(i + rng_[threadN].nextFloat()) / (width_-1);
			auto v = float(j + rng_[threadN].nextFloat()) / (height_-1);

			Ray r = camera_.getRay(u, v);

			// Switch here
			color += rayColor(rng_[threadN], r);
			//color += rayAO(rng_[omp_get_thread_num()], r);
			//color += rayNormal(rng_[omp_get_thread_num()], r);
			//color += rayBarycentrics(rng_[omp_get_thread_num()], r);


			int index = ((height_-1-j) * (width_) + i) * components_;
			target[index]   += color.r;
			target[index+1] += color.g;
			target[index+2] += color.b;
		}
	}
}


glm::vec3 RenderContext::rayColor(pcg32 &rng, const Ray &ray) {
	Ray cur_ray = ray;
	glm::vec3 cur_attenuation(1.0f);

	for(size_t i = 0; i < max_depth_; ++i)
	{
		RTCRayHit rayhit;
		rayhit.ray.org_x = cur_ray.o_.x;
		rayhit.ray.org_y = cur_ray.o_.y;
		rayhit.ray.org_z = cur_ray.o_.z;

		rayhit.ray.dir_x = cur_ray.d_.x;
		rayhit.ray.dir_y = cur_ray.d_.y;
		rayhit.ray.dir_z = cur_ray.d_.z;
		rayhit.ray.tnear  = kEpsilon;
		rayhit.ray.tfar   = std::numeric_limits<float>::infinity();
		rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

		RTCIntersectContext context;
		rtcInitIntersectContext(&context);

		rtcIntersect1(scene_, &context, &rayhit);

		// Miss Shader
		if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID)
		{
			//return cur_attenuation * glm::vec3(1);
			return glm::vec3(0);
		}

		// Fetch current material
		Material &mat = model_.materials_.at(rayhit.hit.primID);
		glm::vec3 emitted = glm::vec3(mat.tiny_material_->emission[0], mat.tiny_material_->emission[1], mat.tiny_material_->emission[2]);

		Ray scattered;
		glm::vec3 color;
		// No scattering -> emissive material
		if (!EvaluateMaterial(rng, rayhit, mat, scattered, color))
		{
			return cur_attenuation * emitted;
		}

		cur_attenuation *= color; // emitted + color
		cur_ray = scattered;
	}

	return glm::vec3(0); // exceeded recursion
}
void RenderContext::present(std::vector<float> &present_buffer,
							const std::vector<float> &accumulation_buffer,
							const uint32_t frame) {

#pragma omp parallel for default(none) shared(present_buffer, accumulation_buffer, frame)
	for (int i = 0; i < accumulation_buffer.size(); ++i)
	{
		// Apply gamma correction + sample averaging
		present_buffer[i] = sqrt(accumulation_buffer[i] / (float)frame); //(float)(frame * spp));
	}
}
