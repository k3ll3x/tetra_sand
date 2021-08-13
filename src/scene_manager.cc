#include "scene_manager.h"

#include "scene_start.h"
#include "scene_vertex.h"

std::vector<std::unique_ptr<scene>> scene_manager::sceneList;
int scene_manager::currentScene = -1;
GLFWwindow *scene_manager::window;
int scene_manager::width;
int scene_manager::height;

void scene_manager::error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void scene_manager::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	// std::cout << "key:\t" << key << "\t" << (char)key << "\tscancode:\t" << scancode << "\taction:\t" << action << "\tmods:\t" << mods << '\n';
	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS){
		next();
	}
	if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS){
		prev();
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS){
		if (currentScene >= 0)
			sceneList.at(currentScene)->reset();
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (key == GLFW_KEY_F11 && action == GLFW_PRESS){
		glfwMaximizeWindow(window);
	}
	if (currentScene >= 0)
		sceneList.at(currentScene)->key_callback(window, key, scancode, action, mods);
}

void scene_manager::cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
	//scene_manager::mxpos = xpos;
	//scene_manager::mypos = ypos;
	//std::cout << xpos << '\t' << ypos << '\n';
}

void scene_manager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		std::cout << "right button clicked!\n";
}

void scene_manager::start(const char *name, int w, int h)
{
	// Time init
	time::init();

	if (!glfwInit())
	{
		std::cout << "Could not create window!\n";
		cleanup();
		exit(EXIT_FAILURE);
	}

	// glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE );

	//imgui
	const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE );

	width = w;
	height = h;

	window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	if (!window)
	{
		std::cout << "Could not create window!\n";
		cleanup();
		exit(EXIT_FAILURE);
	}

	GLFWimage icon[1];
	icon[0].width = xek::width;
	icon[0].height = xek::height;
	icon[0].pixels = (unsigned char*)xek::xek_icon_w;

	glfwSetWindowIcon(window, 1, icon);

	glfwGetFramebufferSize(window, &width, &height);

	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetWindowSizeCallback(window, resize);

	// glfwSetWindowCloseCallback(window, GL_FALSE);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Error init glew\n";
		cleanup();
		exit(EXIT_FAILURE);
	}

	win_mgr.setup(glsl_version, window);

	glViewport(0, 0, width, height);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// GL version
	std::cout << glGetString(GL_VERSION) << std::endl;

	//maximize window
	glfwMaximizeWindow(window);

	// Scene init
	initialize();

	// Run main loop
	mainLoop();
}

void scene_manager::next()
{
	int nScenes = (int)sceneList.size();
	if (nScenes > 0){
		sceneList.at(currentScene)->sleep();
		currentScene = (currentScene + 1) % nScenes;
		sceneList.at(currentScene)->awake();
	}
}

void scene_manager::prev()
{
	int nScenes = (int)sceneList.size();
	if (nScenes > 0){
		sceneList.at(currentScene)->sleep();
		currentScene--;
		if (currentScene < 0)
			currentScene = (nScenes - 1);
		sceneList.at(currentScene)->awake();
	}
}

void scene_manager::initialize()
{
	// Ejemplo de como agregar escenas al proyecto
	//std::unique_ptr<scene> somescene(new scene_project);
	//sceneList.push_back(std::move(somescene));

	std::unique_ptr<scene> scene0(new scene_start);
	sceneList.push_back(std::move(scene0));

	std::unique_ptr<scene> scene1(new scene_vertex);
	sceneList.push_back(std::move(scene1));

	//vao
	GLuint vao;
	GLuint positionsVBO;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	for (auto& s : sceneList)
		s->init();

	if (sceneList.size() >= 0){
		currentScene = 0;
		sceneList.at(currentScene)->awake();
	}
}

void scene_manager::mainLoop(){
	time::tick();
	auto cls_clr = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	while (!glfwWindowShouldClose(window)){
		if (currentScene >= 0)
			sceneList.at(currentScene)->mainLoop();

		glfwPollEvents();

		win_mgr.start_frame();
		win_mgr.main(cls_clr);

		if (currentScene >= 0)
			sceneList.at(currentScene)->sceneWindowHandler();
		win_mgr.render(window, cls_clr);

		glfwSwapBuffers(window);
	}

	cleanup();
}

void scene_manager::idle(){
	//glutPostRedisplay();
}

void scene_manager::cleanup(){
	win_mgr.clean();
	glfwDestroyWindow(window);
	glfwTerminate();
	sceneList.clear();
	currentScene = -1;
}

void scene_manager::resize(GLFWwindow *window, int width, int height){
	glViewport(0, 0, width, height);
	//if (currentScene >= 0)
	//sceneList.at(currentScene)->resize(window, width, height);
}