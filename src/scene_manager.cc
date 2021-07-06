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
	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
	{
		next();
	}
	if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
	{
		prev();
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		if (currentScene >= 0)
			sceneList.at(currentScene)->reset();
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
	{
		glfwMaximizeWindow(window);
	}
	if (currentScene >= 0)
		sceneList.at(currentScene)->key_callback(window, key, scancode, action, mods);
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

	setupImgui(glsl_version, window);

	glViewport(0, 0, width, height);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// GL version
	std::cout << glGetString(GL_VERSION) << std::endl;

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

void scene_manager::setupImgui(const char* glsl_version, GLFWwindow* window){
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void scene_manager::startImguiFrame(){
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void doSomething(char buf[]){
	std::cout << "\t\tmsg:\t" << buf << std::endl;
}

void scene_manager::imguiMain(ImVec4& clear_color){
	static char buf[255];

	ImGui::Begin("LAak");
	ImGui::Text("%s", buf);
	// ImGui::SetKeyboardFocusHere(0);
	ImGui::InputText("<<", buf, 255);

	if (ImGui::Button(">>") || ImGui::IsKeyPressed(257)){
		doSomething(buf);
		buf[0] = '\0';
		ImGui::SetKeyboardFocusHere(0);
	}

	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void scene_manager::renderImgui(GLFWwindow* window, ImVec4& clear_color){
	// Rendering
	ImGui::Render();
	// int display_w, display_h;
	// glfwGetFramebufferSize(window, &display_w, &display_h);
	// glViewport(0, 0, display_w, display_h);
	// glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	// glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void scene_manager::mainLoop(){
	time::tick();
	auto cls_clr = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	while (!glfwWindowShouldClose(window)){
		if (currentScene >= 0)
			sceneList.at(currentScene)->mainLoop();

		glfwPollEvents();

		startImguiFrame();
		imguiMain(cls_clr);
		if (currentScene >= 0)
			sceneList.at(currentScene)->sceneWindowHandler();
		renderImgui(window, cls_clr);

		glfwSwapBuffers(window);
	}

	cleanup();
}

void scene_manager::idle(){
	//glutPostRedisplay();
}

void scene_manager::cleanup(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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