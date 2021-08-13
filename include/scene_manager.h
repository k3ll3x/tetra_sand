#pragma once

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

	static void error_callback(int error, const char* desc);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	static std::vector<std::unique_ptr<scene>> sceneList;
	static int currentScene;

	static GLFWwindow* window;
	static int width, height;

	inline static LAakHandler laak_hndl = LAakHandler();
	inline static window_mgr win_mgr = window_mgr(&laak_hndl);
};