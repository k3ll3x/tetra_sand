#include "Scene.h"

class SceneStart : public Scene {
	float mul = 1.0;
	float psize = 10.0;
	float width = 10.0f;

	ImVec4 tcolor = ImVec4(1.0, 0.0, 0.0, 1.0);
	ImVec4 clear_color = ImVec4(0.69, 0.69, 0.69, 1.0);
	int my_image_width = 0;
	int my_image_height = 0;
	GLuint my_image_texture = 0;

	// MatrixXd V, F;
public:
	//destructors
	~SceneStart();

	void init();
	void awake(){}
	void sleep(){}
	void reset(){}
	void mainLoop();
	void resize(int width, int height){}
	void error_callback(int error, const char* desc){}
	void key_callback(int key, int scancode, int action, int mods){}
	void window_handler();
};