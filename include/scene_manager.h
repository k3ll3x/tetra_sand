#pragma once

#ifndef IOSTREAM
#define IOSTREAM
#include <iostream>
#endif

#include "scene.h"
#include "xek.h"

//laak interpreter
#include "LuaHandler.h"

#ifndef FSTREAM
#define FSTREAM
#include <fstream>
#endif

//tex editor
#include "../ImGuiColorTextEdit/TextEditor.h"

struct entry {
	std::string input, output;
};

class scene_manager {
public:
	static void next();
	static void prev();

	static void start(const char* name, int width, int height);

private:
	static void initialize();
	static void mainLoop();
	static void idle();
	static void cleanup();
	static void resize(GLFWwindow* window, int width, int height);

	static void setupImgui(const char* glsl_version, GLFWwindow* window);
	static void startImguiFrame();
	static void imguiMain(ImVec4& clear_color);
	static void matrix_vector_win(bool& show, bool& new_entry);
	static void script_win(bool& show, bool& new_entry);
	static void renderImgui(GLFWwindow* window, ImVec4& clear_color);
	inline static float imgui_font_scale = 1.5;

	//text editor
	inline static TextEditor editor;
	inline static char filename_path[100] = "./scripts/tmp.lua";

	static void error_callback(int error, const char* desc);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	static std::vector<std::unique_ptr<scene>> sceneList;
	static int currentScene;

	static GLFWwindow* window;
	static int width, height;

	inline static LuaHandler lua_hndl = LuaHandler();
	static std::string pre_laak(char* input);
	inline static const unsigned int buf_s = 255;
	inline static char buf[buf_s];
	inline static std::vector<entry> entries;
	inline static int entry_idx = -1;
};