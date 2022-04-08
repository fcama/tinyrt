//
// Created by dante on 08/04/22.
//

#ifndef TINYRT_SRC_GUI_H_
#define TINYRT_SRC_GUI_H_

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "window_manager.h"
#include "render_context.h"

class Gui {
 public:
	Gui() = delete;
	Gui(RenderContext &render_context, WindowManager &window_manager);

	~Gui();

	void showDebugView();
	float getFrameTime();

	WindowManager *window_manager_;
	RenderContext *render_context_;
};

#endif //TINYRT_SRC_GUI_H_
