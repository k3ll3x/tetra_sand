#include "scene_vertex.h"

scene_vertex::~scene_vertex()
{
	//Delete executable from memory when scene fades,.
	glDeleteProgram(shader_program);
}

void scene_vertex::init(){
	std::vector<Shader> shaders;
	shaders.push_back({"./shaders/grid.vert", GL_VERTEX_SHADER, "positions"});
	shaders.push_back({"./shaders/solid_color.frag", GL_FRAGMENT_SHADER, "colors"});
	setupShaders(shaders);
}

void scene_vertex::awake()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	//diferent sizes of points for each shader program
	glEnable(GL_PROGRAM_POINT_SIZE);
}

void scene_vertex::sleep()
{
	glClearColor(1.0f,1.0f,0.5f,1.0f);
	glDisable(GL_PROGRAM_POINT_SIZE);
}

void scene_vertex::sceneWindowHandler(){
	ImGui::Begin("uniforms");
	ImGui::SliderFloat("time multiplier", &mul, 1.0f, 10.0f, "%.3f");
	ImGui::SliderFloat("point size", &psize, 0.1f, 300.0f, "%.3f");
	ImGui::SliderFloat("width", &width, 0.1f, 20.0f, "%.3f");
	ImGui::End();
}

void scene_vertex::mainLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader_program);
	GLuint time_location = glGetUniformLocation(shader_program, "time");
	glUniform1f(time_location, time::elapsed_time().count() * mul);
	GLuint p_size = glGetUniformLocation(shader_program, "psize");
	glUniform1f(p_size, psize);
	GLuint _width = glGetUniformLocation(shader_program, "width");
	glUniform1f(_width, width);
	// glDrawArrays(GL_TRIANGLE_STRIP, 0, 100);	
	glDrawArrays(GL_POINTS, 0, 100);
	glUseProgram(0);
}

