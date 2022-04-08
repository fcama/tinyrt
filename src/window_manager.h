//
// Created by dante on 08/04/22.
//

#ifndef TINYRT_SRC_WINDOW_MANAGER_H_
#define TINYRT_SRC_WINDOW_MANAGER_H_

#include "render_context.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

class WindowManager {
 public:
	explicit WindowManager(RenderContext& context);
	~WindowManager();

	GLFWwindow *window_;
	RenderContext *context_;
	float mouse_lastX_, mouse_lastY_;

	std::string glsl_version_;
	unsigned int fbo;
	unsigned int texture_colorbuffer;
	unsigned int quadVAO, quadVBO;

	void renderFrame() const;
	void processMouse();
	void processInput(float delta_time);
};

#endif //TINYRT_SRC_WINDOW_MANAGER_H_
