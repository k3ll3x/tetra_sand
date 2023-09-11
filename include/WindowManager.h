#pragma once
#include "LAakHandler.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

//tex editor
#include "TextEditor.h"

#include <string>
#include <vector>
#include <fstream>

struct entry {
	std::string input, output;
};

class WindowManager {
public:
    // WindowManager(Environment env);
    WindowManager(LAakHandler* laak_hndl);
    // ~WindowManager();
    // ~WindowManager() = delete;
    void setup(const char* glsl_version, GLFWwindow* window);
    void start_frame();
    void main(ImVec4& clear_color);
    void matrix_vector_win(bool& show, bool& new_entry);
	void script_win(bool& show, bool& new_entry);
	void render(GLFWwindow* window, ImVec4& clear_color);
    void clean();
private:
    LAakHandler* laak_hndl;
    // GLFWwindow* window;

    float imgui_font_scale = 1.5;

	//text editor
	TextEditor editor;
	char filename_path[100] = "./scripts/tmp.lua";

    inline static const unsigned int buf_s = 255;
	char buf[buf_s];
	std::vector<entry> entries;
	int entry_idx = -1;

    std::string pre_laak(char* input);
};