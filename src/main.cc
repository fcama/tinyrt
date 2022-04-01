#include <iostream>

#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <pcg32.h>

#include <stb/stb_image_write.h>
#include <stb/stb_image.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "common.h"
#include "shader_program.h"
#include "render_context.h"

float mouse_lastX_, mouse_lastY_;

void processMouse(GLFWwindow *window, RenderContext &context) {
	static bool firstMouse = true;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_RELEASE) {
		if (firstMouse) {
			mouse_lastX_ = xpos;
			mouse_lastY_ = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - mouse_lastX_;
		float yoffset = mouse_lastY_ - ypos; // reversed since y-coordinates go from bottom to top

		mouse_lastX_ = xpos;
		mouse_lastY_ = ypos;


		context.camera_.processMouseMovement(xoffset, yoffset);
		context.accumulation_frames = 1;
		std::fill(context.accumulation_buffer.begin(), context.accumulation_buffer.end(), 0.f);
	}
	else
	{
		firstMouse = true;
	}

}

void processInput(GLFWwindow *window, RenderContext &context, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		context.camera_.processKeyboard(CameraMovement::FORWARD, deltaTime);
		context.accumulation_frames = 1;
		std::fill(context.accumulation_buffer.begin(), context.accumulation_buffer.end(), 0.f);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		context.camera_.processKeyboard(CameraMovement::BACKWARD, deltaTime);
		context.accumulation_frames = 1;
		std::fill(context.accumulation_buffer.begin(), context.accumulation_buffer.end(), 0.f);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		context.camera_.processKeyboard(CameraMovement::LEFT, deltaTime);
		context.accumulation_frames = 1;
		std::fill(context.accumulation_buffer.begin(), context.accumulation_buffer.end(), 0.f);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		context.camera_.processKeyboard(CameraMovement::RIGHT, deltaTime);
		context.accumulation_frames = 1;
		std::fill(context.accumulation_buffer.begin(), context.accumulation_buffer.end(), 0.f);
	}

	processMouse(window, context);
}



int main() {
	int width = 900;
	int height = 900;
	int components = 3;
	RenderContext context(width, height, components);

	mouse_lastX_ = context.width_ / 2.f;
	mouse_lastY_ = context.height_ / 2.f;

	// Setup window
	(void)glfwSetErrorCallback(GlfwErrorCallback);
	if (glfwInit() != 1) { return 1; }

	// GL 4.4 + GLSL 440
	const char *glsl_version = "#version 440";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window with graphics context
	GLFWwindow *window = glfwCreateWindow(context.width_, context.height_, "tinyrt", nullptr, nullptr);
	if (window == nullptr) { return 1; }

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Init Glad
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 1) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return 1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ShaderProgram sp("shaders/vs.vert", "shaders/fs.frag");

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	unsigned int texture_colorbuffer;
	glGenTextures(1, &texture_colorbuffer);
	glBindTexture(GL_TEXTURE_2D, texture_colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, context.width_, context.height_, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	float quadVertices[] = {
		// positions   // texCoords
		-1.0f, 1.0f, 0.0f, -1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, -1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, -1.0f
	};

	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_colorbuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return 1;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	sp.use();
	sp.setInt("screenTexture", 0);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		processInput(window, context, 1000.0f / ImGui::GetIO().Framerate);

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
		}

		ImGui::SliderFloat3("front", &(context.camera_.front_[0]), 0.0f, 1.0f);
		ImGui::SliderFloat3("right", &(context.camera_.right_[0]), 0.0f, 1.0f);
		ImGui::SliderFloat3("up", &(context.camera_.up_[0]), 0.0f, 1.0f);
		context.camera_.updateCameraBase();

		ImGui::End();

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		context.render(context.accumulation_buffer);

		sp.setInt("frame", context.accumulation_frames);

		glTexSubImage2D(GL_TEXTURE_2D,
						0,
						0,
						0,
						context.width_,
						context.height_,
						GL_RGB,
						GL_FLOAT,
						(const void *)context.accumulation_buffer.data());

		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, texture_colorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		++context.accumulation_frames;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}