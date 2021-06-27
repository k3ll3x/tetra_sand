#pragma once

#include "scene.h"
#include <iostream>

class scene_vertex : public scene {
public:
	float mul = 1.0;
	float psize = 10.0;
	float width = 10.0f;

	//destructors
	~scene_vertex();

	void init();
	void awake();
	void sleep();
	void reset() {}
	void mainLoop();
	void resize(GLFWwindow* window, int width, int height){}
	void error_callback(int error, const char* desc){}
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){}
	void sceneWindowHandler();
};