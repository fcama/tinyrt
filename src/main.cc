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


int main() {
	int width = 900; int height = 900; int components = 3;
	RenderContext context(width, height, components, 16);

	std::vector<float> accumulation_buffer(width * height * components, 0.f); // TODO resizable

	// Setup window
    (void)glfwSetErrorCallback(GlfwErrorCallback);
    if (glfwInit() != 1) { return 1; }

	// GL 4.4 + GLSL 440
	const char* glsl_version = "#version 440";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(context.width_, context.height_, "tinyrt", nullptr, nullptr);
    if (window == nullptr) { return 1; }

    glfwMakeContextCurrent(window);
    //glfwSwapInterval(1); // Enable vsync

    // Init Glad
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 1)
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return 1;
    }

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
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
		-1.0f,  1.0f,  0.0f, -1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, -1.0f
	};

	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_colorbuffer, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return 1;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	sp.use();
	sp.setInt("screenTexture", 0);

	uint32_t frame = 1;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Performance Metrics");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Frame count: %d", frame);
		ImGui::End();

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		glClearColor(1,1,1,1);
		glClear(GL_COLOR_BUFFER_BIT);

		context.render(accumulation_buffer);
		sp.setInt("frame", frame);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, context.width_, context.height_, 0, GL_RGB, GL_FLOAT, (const void*)accumulation_buffer.data());

		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, texture_colorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		++frame;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}