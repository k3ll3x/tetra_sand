#pragma once

#include "scene.h"

class scene_start : public scene {
	void audio_window();
	std::string audio_status = "";
	ImVec4 tcolor = ImVec4(1.0, 0.0, 0.0, 1.0);
	ImVec4 clear_color = ImVec4(0.69, 0.69, 0.69, 1.0);
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
	void sceneWindowHandler();
};