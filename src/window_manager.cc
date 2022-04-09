//
// Created by dante on 08/04/22.
//

#include "window_manager.h"

#include <iostream>

inline void GlfwErrorCallback(int error, const char* description)
{
	(void)fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void WindowManager::processMouse() {
	static bool firstMouse = true;

	double xpos, ypos;
	glfwGetCursorPos(window_, &xpos, &ypos);

	if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_RELEASE) {
		if (firstMouse) {
			mouse_lastX_ = xpos;
			mouse_lastY_ = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - mouse_lastX_;
		float yoffset = mouse_lastY_ - ypos; // reversed since y-coordinates go from bottom to top

		mouse_lastX_ = xpos;
		mouse_lastY_ = ypos;


		context_->camera_.processMouseMovement(xoffset, yoffset);
		context_->accumulation_frames = 1;
		std::fill(context_->accumulation_buffer_.begin(), context_->accumulation_buffer_.end(), 0.f);
	}
	else
	{
		firstMouse = true;
	}

}

void WindowManager::processInput(float delta_time) {
	glfwPollEvents();

	if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window_, true);

	if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
		context_->camera_.processKeyboard(CameraMovement::FORWARD, delta_time);
		context_->accumulation_frames = 1;
		std::fill(context_->accumulation_buffer_.begin(), context_->accumulation_buffer_.end(), 0.f);
	}
	if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
		context_->camera_.processKeyboard(CameraMovement::BACKWARD, delta_time);
		context_->accumulation_frames = 1;
		std::fill(context_->accumulation_buffer_.begin(), context_->accumulation_buffer_.end(), 0.f);
	}
	if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
		context_->camera_.processKeyboard(CameraMovement::LEFT, delta_time);
		context_->accumulation_frames = 1;
		std::fill(context_->accumulation_buffer_.begin(), context_->accumulation_buffer_.end(), 0.f);
	}
	if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
		context_->camera_.processKeyboard(CameraMovement::RIGHT, delta_time);
		context_->accumulation_frames = 1;
		std::fill(context_->accumulation_buffer_.begin(), context_->accumulation_buffer_.end(), 0.f);
	}

	processMouse();
}

WindowManager::WindowManager(RenderContext& context) {
	context_ = &context;

	mouse_lastX_ = (float)context_->width_ / 2.f;
	mouse_lastY_ = (float)context_->height_ / 2.f;

	// Setup window
	(void)glfwSetErrorCallback(GlfwErrorCallback);
	if (glfwInit() != 1) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return;
	}

	// GL 4.4 + GLSL 440
	glsl_version_ = "#version 440";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);

	window_ = glfwCreateWindow(context_->width_, context_->height_, "tinyrt", nullptr, nullptr);
	if (window_ == nullptr) {
		std::cout << "Failed to create GLFW_window" << std::endl;
		return;
	}

	glfwMakeContextCurrent(window_);
	glfwSwapInterval(1); // Enable vsync

	// Init Glad
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 1) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	// ***************** OPEN GL ***************** //
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	texture_colorbuffer;
	glGenTextures(1, &texture_colorbuffer);
	glBindTexture(GL_TEXTURE_2D, texture_colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, context.width_, context.height_, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	constexpr float kQuadVertices[] = {
		// positions   // texCoords
		-1.0f, 1.0f, 0.0f, -1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, -1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, -1.0f
	};

	// screen quad VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), &kQuadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_colorbuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Failed to initialize FrameBuffer (fbo)" << std::endl;
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// ***************** OPEN GL ***************** //
}

void WindowManager::renderFrame() const {
	glClearColor(1, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	// Update texture
	glTexSubImage2D(GL_TEXTURE_2D,
					0,
					0,
					0,
					context_->width_,
					context_->height_,
					GL_RGB,
					GL_FLOAT,
					(const void *)context_->accumulation_buffer_.data());

	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, texture_colorbuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

WindowManager::~WindowManager() {
	glfwDestroyWindow(window_);
	glfwTerminate();
}

bool WindowManager::shouldClose() const {
	return glfwWindowShouldClose(window_);
}

