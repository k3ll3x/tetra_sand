#include "scene_start.h"

void scene_start::init(){
	std::vector<Shader> shaders;
	shaders.push_back({"./shaders/simple.vert", GL_VERTEX_SHADER});
	shaders.push_back({"./shaders/simple.frag", GL_FRAGMENT_SHADER});
	setupShaders(shaders);
	bool ret = image_handler::LoadTextureFromFile("./misc/nodimension.png", &my_image_texture, &my_image_width, &my_image_height);
	IM_ASSERT(ret);
}

void scene_start::audio_window(){
	ImGui::Begin("Audio Window");
	ImGui::Text("Record Audio with default device...");
	ImGui::TextColored(scene_start::tcolor, scene_start::audio_status.c_str());
	if(ImGui::Button("record")){
		scene_start::audio_status = "Recording...";
		scene_start::tcolor = ImVec4(1.0, 0.0, 0.0, 1.0);
		scene::audio_mgr->record();
	}
	if(ImGui::Button("save recording to file")){
		scene_start::audio_status = "Saving file...";
		scene_start::tcolor = ImVec4(1.0, 0.5, 0.0, 1.0);
		scene::audio_mgr->save_raw_data("record.raw");
	}
	if(ImGui::Button("play file")){
		scene_start::audio_status = "Playing file...";
		scene_start::tcolor = ImVec4(0.0, 1.0, 0.0, 1.0);
		scene::audio_mgr->play("record.raw");
	}
	if(scene::audio_mgr->is_stream_running())
		scene_start::audio_status = "";
	ImGui::End();
}

void scene_start::sceneWindowHandler(){
	ImGui::Begin("Start Scene");
    ImGui::Text("Hello");
	ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
	ImGui::End();

	audio_window();
}

void scene_start::mainLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(.69, .69, .69, 1.0f);
	//nothing yet
}

