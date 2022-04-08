#include <iostream>
#include <glm/glm.hpp>
#include <pcg32.h>

#include <stb/stb_image_write.h>
#include <stb/stb_image.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "common.h"
#include "shader_program.h"
#include "render_context.h"
#include "window_manager.h"


int main() {
	int width = 900;
	int height = 900;
	int components = 3;

	RenderContext context(width, height, components);
	WindowManager window_manager(context);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window_manager.window_, true);
	ImGui_ImplOpenGL3_Init(window_manager.glsl_version_.c_str());

	ShaderProgram sp("shaders/vs.vert", "shaders/fs.frag");
	sp.use();
	sp.setInt("screenTexture", 0);

	while (!glfwWindowShouldClose(window_manager.window_)) {
		glfwPollEvents();
		window_manager.processInput(1000.0f / ImGui::GetIO().Framerate);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Performance Metrics");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
					1000.0f / ImGui::GetIO().Framerate,
					ImGui::GetIO().Framerate);
		ImGui::Text("Frame count: %d", context.accumulation_frames);

		ImGui::Text("\nDebug View");
		if (ImGui::RadioButton("Camera", context.current_output_ == RenderOutput::CAMERA)) {
			context.current_output_ = RenderOutput::CAMERA;
			context.accumulation_frames = 1;
			std::fill(context.accumulation_buffer.begin(), context.accumulation_buffer.end(), 0.f);
		} else if (ImGui::RadioButton("Normals", context.current_output_ == RenderOutput::NORMALS)) {
			context.current_output_ = RenderOutput::NORMALS;
			context.accumulation_frames = 1;
			std::fill(context.accumulation_buffer.begin(), context.accumulation_buffer.end(), 0.f);
		} else if (ImGui::RadioButton("Barycentrics", context.current_output_ == RenderOutput::BARYCENTRICS)) {
			context.current_output_ = RenderOutput::BARYCENTRICS;
			context.accumulation_frames = 1;
			std::fill(context.accumulation_buffer.begin(), context.accumulation_buffer.end(), 0.f);
		} else if (ImGui::RadioButton("Ambient Occlusion", context.current_output_ == RenderOutput::AMBIENT_OCCLUSION)) {
			context.current_output_ = RenderOutput::AMBIENT_OCCLUSION;
			context.accumulation_frames = 1;
			std::fill(context.accumulation_buffer.begin(), context.accumulation_buffer.end(), 0.f);
		}
		ImGui::End();

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window_manager.window_, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		context.render(context.accumulation_buffer);

		sp.setUInt("frame", context.accumulation_frames);

		window_manager.renderFrame();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window_manager.window_);

		++context.accumulation_frames;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();



	return 0;
}