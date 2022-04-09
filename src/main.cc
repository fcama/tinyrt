#include "shader_program.h"
#include "render_context.h"
#include "window_manager.h"
#include "gui.h"


int main() {
	int width = 900;
	int height = 900;
	int components = 3;

	RenderContext context(width, height, components);
	WindowManager window_manager(context);
	Gui gui(context, window_manager);

	ShaderProgram sp("shaders/vs.vert", "shaders/fs.frag");
	sp.use();
	sp.setInt("screenTexture", 0);

	while (!window_manager.shouldClose()) {

		window_manager.processInput(gui.getFrameTime());

		context.traceScene();

		sp.setUInt("frame", context.accumulation_frames);

		window_manager.renderFrame();
		gui.showDebugView();
		glfwSwapBuffers(window_manager.window_);

		++context.accumulation_frames;
	}


	return 0;
}