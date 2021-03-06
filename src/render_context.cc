//
// Created by dante on 28/03/22.
//

#include <iostream>
#include "render_context.h"
#include "common.h"
#include <memory>
#include <algorithm>

RenderContext::RenderContext(int width, int height, int comp, int n_threads, int max_depth)
	: width_(width), height_(height), components_(comp), num_threads_(n_threads), max_depth_(max_depth) {

	aspect_ratio_ = float(width_) / height_;
	float vfov = 26.99;
	//float vfov = 45.f;
	camera_ = Camera(glm::vec3(0.0, 1.0, 5.15), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), vfov, aspect_ratio_);
	model_ = LoadObjFile("models/cornellbox/cornellbox.obj", "models/cornellbox/");
//	model_ = LoadObjFile("models/sponza/sponza.obj", "models/sponza/");
//	for (auto &vertex : model_.vertices_)
//	{
//		vertex *= 0.001;
//	}


	rng_.resize(num_threads_);
	std::vector<uint64_t> initstate(num_threads_, PCG32_DEFAULT_STATE);
	std::vector<uint64_t> initseq(num_threads_);
	for (size_t i = 0; i < initseq.size(); ++i) {
		initseq[i] = i + 1;
	}
	for (int i = 0; i < num_threads_; ++i) {
		rng_[i].seed(initstate[i], initseq[i]);
	}

	// Embree code
	device_ = rtcNewDevice(nullptr);
	scene_ = rtcNewScene(device_);
	RTCGeometry geom = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_TRIANGLE);

	std::cout << model_;

	auto *vb = (float *)rtcSetNewGeometryBuffer(geom,
												 RTC_BUFFER_TYPE_VERTEX,
												 0,
												 RTC_FORMAT_FLOAT3,
												 3 * sizeof(float),
												 model_.vertices_.size() / 3);
	for (size_t i = 0; i < model_.vertices_.size(); ++i) {
		vb[i] = model_.vertices_[i];
	}

	auto *ib = (unsigned *)rtcSetNewGeometryBuffer(geom,
													   RTC_BUFFER_TYPE_INDEX,
													   0,
													   RTC_FORMAT_UINT3,
													   3 * sizeof(unsigned),
													   model_.indices_.size() / 3);
	for (size_t i = 0; i < model_.indices_.size(); ++i) {
		ib[i] = model_.indices_[i];
	}

	rtcCommitGeometry(geom);
	rtcAttachGeometry(scene_, geom);
	rtcReleaseGeometry(geom);

	rtcSetSceneBuildQuality(scene_, RTC_BUILD_QUALITY_HIGH);
	rtcCommitScene(scene_);

	current_output_ = RenderOutput::CAMERA;

	accumulation_frames_ = 1;
	accumulation_buffer_.resize(width_ * height_ * components_, 0.f);

}

glm::vec3 RenderContext::rayColor(pcg32 &rng, const Ray &ray) {
	Ray cur_ray = ray;

	glm::vec3 throughput(1.0f);
	glm::vec3 pixel_color(0.0f);

	for (size_t i = 0; i < max_depth_; ++i) {
		RTCRayHit rayhit;
		rayhit.ray.org_x = cur_ray.o_.x;
		rayhit.ray.org_y = cur_ray.o_.y;
		rayhit.ray.org_z = cur_ray.o_.z;

		rayhit.ray.dir_x = cur_ray.d_.x;
		rayhit.ray.dir_y = cur_ray.d_.y;
		rayhit.ray.dir_z = cur_ray.d_.z;
		rayhit.ray.tnear = kEpsilon;
		rayhit.ray.tfar = std::numeric_limits<float>::infinity();
		rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

		RTCIntersectContext context;
		rtcInitIntersectContext(&context);

		rtcIntersect1(scene_, &context, &rayhit);

		// Miss Shader
		if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
			break;
		}

		// Fetch current material
		Material &mat = model_.materials_[rayhit.hit.primID];

		int face_id = rayhit.hit.primID;
		std::vector<glm::vec3> triangle(3);
		std::vector<glm::vec2> uvs(3);
		glm::ivec3 tri_indices(model_.indices_[face_id * 3], model_.indices_[face_id * 3 + 1], model_.indices_[face_id * 3 + 2]);
		for (int i = 0; i < 3; ++i)
		{
			int index = tri_indices[i] * 3;
			triangle[i] = {model_.vertices_[index], model_.vertices_[index+1], model_.vertices_[index+2]};
			int tex_idx = face_id*4 + 2*i;
			uvs[i] = {model_.tex_coords_[tex_idx], model_.tex_coords_[tex_idx + 1]};
		}


		glm::vec3 emitted = glm::vec3(mat.tiny_material_->emission[0],
									  mat.tiny_material_->emission[1],
									  mat.tiny_material_->emission[2]);

		Ray scattered;
		glm::vec3 output_color;

		EvaluateMaterial(rng, rayhit, uvs, mat, scattered, output_color);

		pixel_color += emitted * throughput;
		throughput *= output_color;

		// Russian Roulette
		float survival_rate = std::max(throughput.r, std::max(throughput.g, throughput.b));
		if (rng.nextFloat() > survival_rate)
			break;
		// Add the energy we 'lose' by randomly terminating paths
		throughput *= 1.0f / survival_rate;


		cur_ray = scattered;
	}

	return pixel_color;
}

glm::vec3 RenderContext::rayNormal(pcg32 &rng, const Ray &ray) {
	RTCRayHit rayhit;
	rayhit.ray.org_x = ray.o_.x;
	rayhit.ray.org_y = ray.o_.y;
	rayhit.ray.org_z = ray.o_.z;

	rayhit.ray.dir_x = ray.d_.x;
	rayhit.ray.dir_y = ray.d_.y;
	rayhit.ray.dir_z = ray.d_.z;
	rayhit.ray.tnear = 0.f;
	rayhit.ray.tfar = std::numeric_limits<float>::infinity();
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	rtcIntersect1(scene_, &context, &rayhit);

	// Miss Shader
	if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
		return glm::vec3(0);
	}

	const glm::vec3 geomNormal = glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);
	const glm::vec3 shadingNorm =
		graphics::CalcShadingNormal(glm::vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z), geomNormal);
	return glm::vec3(0.5) * glm::vec3(shadingNorm.x + 1, shadingNorm.y + 1, shadingNorm.z + 1);
}

glm::vec3 RenderContext::rayBarycentrics(pcg32 &rng, const Ray &ray) {
	RTCRayHit rayhit;
	rayhit.ray.org_x = ray.o_.x;
	rayhit.ray.org_y = ray.o_.y;
	rayhit.ray.org_z = ray.o_.z;

	rayhit.ray.dir_x = ray.d_.x;
	rayhit.ray.dir_y = ray.d_.y;
	rayhit.ray.dir_z = ray.d_.z;
	rayhit.ray.tnear = 0.f;
	rayhit.ray.tfar = std::numeric_limits<float>::infinity();
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	rtcIntersect1(scene_, &context, &rayhit);

	// Miss Shader
	if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
		return glm::vec3(0);
	}

	glm::vec3 barycentrics = glm::vec3(1.0 - rayhit.hit.u - rayhit.hit.v, rayhit.hit.u, rayhit.hit.v);
	return barycentrics;
}

glm::vec3 RenderContext::rayAo(pcg32 &rng, const Ray &ray) {
	// Settings
	static constexpr int kAoSamples = 8;

	RTCRayHit rayhit;
	rayhit.ray.org_x = ray.o_.x;
	rayhit.ray.org_y = ray.o_.y;
	rayhit.ray.org_z = ray.o_.z;

	rayhit.ray.dir_x = ray.d_.x;
	rayhit.ray.dir_y = ray.d_.y;
	rayhit.ray.dir_z = ray.d_.z;
	rayhit.ray.tnear = 0.f;
	rayhit.ray.tfar = std::numeric_limits<float>::infinity();
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	rtcIntersect1(scene_, &context, &rayhit);

	// Miss Shader
	if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
		return glm::vec3(0);
	}

	const glm::vec3 p = glm::vec3(rayhit.ray.org_x + rayhit.ray.tfar * rayhit.ray.dir_x,
								  rayhit.ray.org_y + rayhit.ray.tfar * rayhit.ray.dir_y,
								  rayhit.ray.org_z + rayhit.ray.tfar * rayhit.ray.dir_z);

	const glm::vec3 geomNormal = glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);
	const glm::vec3 shadingNorm =
		graphics::CalcShadingNormal(glm::vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z), geomNormal);

	unsigned ao_intensity{0};

	assert(kAoSamples % 8 == 0);

	// SIMD version
	static constexpr int simdSize = 8;
	for (size_t iter = 0; iter < kAoSamples; iter += simdSize) {
		std::unique_ptr<RTCRay8> rayVector = std::make_unique<RTCRay8>();
		static const int valid[simdSize] = {-1, -1, -1, -1, -1, -1, -1, -1};

		for (int ao = 0; ao < simdSize; ++ao) {
			glm::vec3 d = graphics::GetCosHemisphereSample(rng, shadingNorm);

			rayVector->org_x[ao] = p.x;
			rayVector->org_y[ao] = p.y;
			rayVector->org_z[ao] = p.z;

			rayVector->dir_x[ao] = d.x;
			rayVector->dir_y[ao] = d.y;
			rayVector->dir_z[ao] = d.z;

			rayVector->tnear[ao] = kEpsilon;
			rayVector->tfar[ao] = 1.4f;
		}

		RTCIntersectContext aoContext;
		aoContext.flags = RTC_INTERSECT_CONTEXT_FLAG_INCOHERENT;
		rtcInitIntersectContext(&aoContext);

		rtcOccluded8(valid, scene_, &aoContext, rayVector.get());

		for (int ao = 0; ao < simdSize; ++ao) {
			if (rayVector->tfar[ao] >= 0.f) {
				ao_intensity += 1;
			}
		}
	}

	// Sequential Version
	// for (int ao = 0; ao < aoSamples; ++ao)
	// {
	//     RTCRay aoRay;
	//     aoRay.org_x = p.x;
	//     aoRay.org_y = p.y;
	//     aoRay.org_z = p.z;
	//     glm::vec3 d = graphics::getCosHemisphereSample(rng, shadingNorm);

	//     aoRay.dir_x = d.x,
	//     aoRay.dir_y = d.y,
	//     aoRay.dir_z = d.z;
	//     aoRay.tnear = epsilon;
	//     aoRay.tfar  = 1.4f;

	//     RTCIntersectContext context;
	//     rtcInitIntersectContext(&context);

	//     rtcOccluded1(scene, &context, &aoRay);

	//     // hit found
	//     if (aoRay.tfar >= 0.f)
	//     {
	//         aoIntensity += 1;
	//     }
	// }

	return glm::vec3(ao_intensity / (float)kAoSamples);
}

void RenderContext::traceScene() {

	switch (current_output_) {
		case (RenderOutput::CAMERA) : {
			render(accumulation_buffer_, [&](pcg32 &rng, Ray &r) { return rayColor(rng, r); });
			break;
		}
		case (RenderOutput::NORMALS) : {
			render(accumulation_buffer_, [&](pcg32 &rng, Ray &r) { return rayNormal(rng, r); });
			break;
		}
		case (RenderOutput::BARYCENTRICS) : {
			render(accumulation_buffer_, [&](pcg32 &rng, Ray &r) { return rayBarycentrics(rng, r); });
			break;
		}
		case (RenderOutput::AMBIENT_OCCLUSION) : {
			render(accumulation_buffer_, [&](pcg32 &rng, Ray &r) { return rayAo(rng, r); });
			break;
		}

	}
}

