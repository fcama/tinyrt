#include <iostream>

#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <pcg32.h>

#include <stb/stb_image_write.h>
#include <stb/stb_image.h>

#include <embree3/rtcore.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "shader_program.h"

static void GlfwErrorCallback(int error, const char* description)
{
    (void)fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void Function()
{
}

int main() {
    // Setup window
    (void)glfwSetErrorCallback(GlfwErrorCallback);
    if (glfwInit() != 1) { return 1; }

	// GL 4.4 + GLSL 440
	const char* glsl_version = "#version 440";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(800, 800, "tinyrt", nullptr, nullptr);
    if (window == nullptr) { return 1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

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

    auto device = rtcNewDevice(nullptr);

	ShaderProgram sp("shaders/vs.vertex", "shaders/fs.frag");
	//sp.SetInt("myint", 0);

    pcg32 rng;
    rng.seed(2);
    std::cout << "Hello, World! " << rng.nextFloat() << std::endl;

    return 0;
}
