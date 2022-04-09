//
// Created by dante on 08/04/22.
//

#include "gui.h"

Gui::Gui(RenderContext &render_context, WindowManager &window_manager)
	: render_context_(&render_context), window_manager_(&window_manager)	{

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window_manager_->window_, true);
	ImGui_ImplOpenGL3_Init(window_manager_->glsl_version_.c_str());
}

Gui::~Gui() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Gui::showDebugView() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Performance Metrics");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);
	ImGui::Text("Frame count: %d", render_context_->accumulation_frames);

	ImGui::Text("\nDebug View");
	if (ImGui::RadioButton("Camera", render_context_->current_output_ == RenderOutput::CAMERA)) {
		render_context_->current_output_ = RenderOutput::CAMERA;
		render_context_->accumulation_frames = 1;
		std::fill(render_context_->accumulation_buffer_.begin(), render_context_->accumulation_buffer_.end(), 0.f);
	} else if (ImGui::RadioButton("Normals", render_context_->current_output_ == RenderOutput::NORMALS)) {
		render_context_->current_output_ = RenderOutput::NORMALS;
		render_context_->accumulation_frames = 1;
		std::fill(render_context_->accumulation_buffer_.begin(), render_context_->accumulation_buffer_.end(), 0.f);
	} else if (ImGui::RadioButton("Barycentrics", render_context_->current_output_ == RenderOutput::BARYCENTRICS)) {
		render_context_->current_output_ = RenderOutput::BARYCENTRICS;
		render_context_->accumulation_frames = 1;
		std::fill(render_context_->accumulation_buffer_.begin(), render_context_->accumulation_buffer_.end(), 0.f);
	} else if (ImGui::RadioButton("Ambient Occlusion", render_context_->current_output_ == RenderOutput::AMBIENT_OCCLUSION)) {
		render_context_->current_output_ = RenderOutput::AMBIENT_OCCLUSION;
		render_context_->accumulation_frames = 1;
		std::fill(render_context_->accumulation_buffer_.begin(), render_context_->accumulation_buffer_.end(), 0.f);
	}
	
	ImGui::Render();
}
float Gui::getFrameTime() {
	return 1000.0f / ImGui::GetIO().Framerate;
}
