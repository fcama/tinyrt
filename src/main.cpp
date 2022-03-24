#include <iostream>

#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <pcg32.h>

#include <stb/stb_image_write.h>
#include <stb/stb_image.h>

#include <embree3/rtcore.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main() {

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
       return 1;

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(800, 800, "tinyrt", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Init Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    auto device = rtcNewDevice(nullptr);

    pcg32 rng;
    rng.seed(2);
    std::cout << "Hello, World! " << rng.nextFloat() << std::endl;

    return 0;
}
