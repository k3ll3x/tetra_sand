#pragma once

#include "scene.h"
#include "image_handler.h"

class scene_start : public scene {
	int my_image_width = 0;
	int my_image_height = 0;
	GLuint my_image_texture = 0;
public:
	//destructors
	~scene_start(){}

	void init();
	void awake(){}
	void sleep(){}
	void reset(){}
	void mainLoop();
	void resize(GLFWwindow* window, int width, int height){}
	void error_callback(int error, const char* desc){}
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){}
	void setupImgui(const char* glsl_version){}
	void startImguiFrame(){}
	void renderImgui(GLFWwindow* window, ImVec4& clear_color){}
	void sceneWindowHandler();
};