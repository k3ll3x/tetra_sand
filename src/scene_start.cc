#include "scene_start.h"

void scene_start::init(){
	std::vector<Shader> shaders;
	shaders.push_back({"./shaders/simple.vert", GL_VERTEX_SHADER});
	shaders.push_back({"./shaders/simple.frag", GL_FRAGMENT_SHADER});
	setupShaders(shaders);
	bool ret = image_handler::LoadTextureFromFile("./misc/nodimension.png", &my_image_texture, &my_image_width, &my_image_height);
	IM_ASSERT(ret);
}

void scene_start::sceneWindowHandler(){
	ImGui::Begin("Start Scene");
    ImGui::Text("Hello");
	ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
	ImGui::End();
}

void scene_start::mainLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(.69, .69, .69, 1.0f);
	//nothing yet
}

