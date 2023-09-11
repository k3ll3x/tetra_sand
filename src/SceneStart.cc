#include "SceneStart.h"

SceneStart::~SceneStart(){
	glDeleteProgram(shader_program);
}

void SceneStart::init(){
	primitiveType = GL_POINTS;

	ifile shvertex;
	shvertex.read("shaders/grid.vert");
	// std::string vertex = shvertex.get_contents();
	auto vertex = shvertex.get_contents();

	ifile shfragment;
	shfragment.read("shaders/solid_color.frag");
	// std::string fragment = shfragment.get_contents();
	auto fragment = shfragment.get_contents();

	std::vector<Shader> shaders;
	shaders.push_back({"positions", vertex, GL_VERTEX_SHADER});
	shaders.push_back({"colors", fragment, GL_FRAGMENT_SHADER});
	setupShaders(shaders);

	bool ret = image_handler::LoadTextureFromFile("./misc/nodimension.png", &my_image_texture, &my_image_width, &my_image_height);
	IM_ASSERT(ret);
}

// void SceneStart::sleep(){
// 	glClearColor(0.2f,1.0f,0.5f,1.0f);
// 	glDisable(GL_PROGRAM_POINT_SIZE);
// }

// void SceneStart::awake(){
// 	glClearColor(0.42f,0.2f,0.33f,1.0f);
// 	//diferent sizes of points for each shader program
// 	glEnable(GL_PROGRAM_POINT_SIZE);
// }

void SceneStart::window_handler(){
	ImGui::Begin("Start Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Hello");
	ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
	ImGui::End();

	ImGui::Begin("background color", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::ColorEdit3("clear color", (float*)&clear_color);
	ImGui::End();

	ImGui::Begin("uniforms");
	ImGui::SliderFloat("time multiplier", &mul, 1.0f, 10.0f, "%.3f");
	ImGui::SliderFloat("point size", &psize, 0.1f, 300.0f, "%.3f");
	ImGui::SliderFloat("width", &width, 0.1f, 20.0f, "%.3f");
	ImGui::End();
}

void SceneStart::mainLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader_program);
	GLuint time_location = glGetUniformLocation(shader_program, "time");
	glUniform1f(time_location, time::elapsed_time().count() * mul);
	GLuint p_size = glGetUniformLocation(shader_program, "psize");
	glUniform1f(p_size, psize);
	GLuint _width = glGetUniformLocation(shader_program, "width");
	glUniform1f(_width, width);
	glDrawArrays(primitiveType, 0, 666);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glUseProgram(0);
}

