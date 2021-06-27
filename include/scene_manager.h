#pragma once

#include <iostream>

#include "scene.h"

#include "xek.h"

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
	static void renderImgui(GLFWwindow* window, ImVec4& clear_color);

	static void error_callback(int error, const char* desc);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static std::vector<std::unique_ptr<scene>> sceneList;
	static int currentScene;

	static GLFWwindow* window;
	static int width, height;
};