#include "window_mgr.h"

window_mgr::window_mgr(LAakHandler* laak_hndl){
	this->laak_hndl = laak_hndl;
}

// window_mgr::~window_mgr(){
// 	clean();
// }

void window_mgr::clean(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void window_mgr::setup(const char* glsl_version, GLFWwindow* window){
    // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();
	// ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//text editor
	auto lang = TextEditor::LanguageDefinition::Lua();
	editor.SetLanguageDefinition(lang);
}

void window_mgr::start_frame(){
    ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

std::string window_mgr::pre_laak(char* input){
	if(!laak_hndl->do_string(input))
		return laak_hndl->errmsg;
	if(laak_hndl->out.empty())
		return input;
	return laak_hndl->out;
}

void window_mgr::main(ImVec4& clear_color){
    static bool mv_win_show = false;
	static bool script_win_show = false;
	static bool new_entry = false;
	static bool style_ed_show = false;

	ImGui::Begin("LAak", nullptr, ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowFontScale(imgui_font_scale);

	if (ImGui::BeginMenuBar()){
		if (ImGui::BeginMenu("edit")){
			if(ImGui::MenuItem("show style editor"))
				style_ed_show = !style_ed_show;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("text editor")){
			if(ImGui::MenuItem("Show"))
				script_win_show = !script_win_show;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("entry Matrix/Vector")){
			if(ImGui::MenuItem("Show"))
				mv_win_show = !mv_win_show;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("entries")){
			if(ImGui::MenuItem("clear"))
				entries.clear();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if(mv_win_show)
		matrix_vector_win(mv_win_show, new_entry);

	if(script_win_show)
		script_win(script_win_show, new_entry);

	if(style_ed_show){
		ImGui::Begin("Style Editor", &style_ed_show);
		ImGui::ShowStyleEditor();
		ImGui::End();
	}
	
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() + 22;
	ImGui::BeginChild("entries", {0, -footer_height_to_reserve}, true, ImGuiWindowFlags_HorizontalScrollbar);

	for(int i = 0; i < entries.size(); ++i){
		ImGui::TextColored({ 0.0, 1.0, 0.3, 1.0 },">>\n");
		ImGui::SameLine();
		ImGui::TextColored({ 1.0, 1.0, 0.0, 1.0 },"%s", entries[i].output.c_str());
	}
	if(new_entry){
		ImGui::SetScrollHereY();
		new_entry = false;
	}

	ImGui::EndChild();
	ImGui::Separator();

	// ImGui::TextColored({0.0,1.0,0.0,0.7}, "%s", buf);
	ImGui::InputText(">>", buf, buf_s);

	if(ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)){
		ImGui::SameLine();
		if ((ImGui::Button("eval") || ImGui::IsKeyPressed(GLFW_KEY_ENTER))){
			if(buf[0] != '\0'){
				entries.push_back({buf, pre_laak(buf)});
				buf[0] = '\0';
				new_entry = true;
				entry_idx = entries.size()-1;
			}
			ImGui::SetKeyboardFocusHere(0);
		}

		if(ImGui::IsKeyPressed(GLFW_KEY_UP)){
			if(!entries.empty()){// && ImGui::GetActiveID() == ImGui::GetID(">>")){
				//ImGui::ClearActiveID();
				if(entry_idx == -1)
					entry_idx = entries.size()-1;
				strcpy(buf, entries[entry_idx].input.c_str());
				entry_idx = entry_idx - 1;
			}
		}
		if(ImGui::IsKeyPressed(GLFW_KEY_DOWN)){
			if(!entries.empty()){
				if(entry_idx == -1)
					entry_idx = 0;
				strcpy(buf, entries[entry_idx].input.c_str());
				entry_idx = (entry_idx + 1)%entries.size();
			}
		}
	}

	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void window_mgr::matrix_vector_win(bool& show, bool& new_entry){
    static char mv_var[20];
	static char mv_val[255];
	ImGui::Begin("Matrix/Vector Creator", &show, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::SetWindowFontScale(imgui_font_scale);
	if(ImGui::Button("Clear")){
		mv_var[0] = '\0';
		mv_val[0] = '\0';
	}
	ImGui::SameLine();
	if(ImGui::Button("Close"))
		show = false;
	ImGui::InputText("variable", mv_var, 20);
	ImGui::InputTextMultiline("Matrix\nVector", mv_val, 255);
	if(ImGui::Button("eval")){
		//eval expression := mv_var = mv_val
		char nbuf[275] = "";
		strcat(nbuf, mv_var);
		strcat(nbuf, "=");
		strcat(nbuf, mv_val);
		entries.push_back({nbuf, pre_laak(nbuf)});
		mv_var[0] = '\0';
		mv_val[0] = '\0';
		new_entry = true;
	}
	ImGui::End();
}

void window_mgr::script_win(bool& show, bool& new_entry){
    auto cpos = editor.GetCursorPosition();

	static char status[20] = "";

	static float font_size = imgui_font_scale;

	//open tmp script
	static auto open_file = [&](){
		std::ifstream f(filename_path);
		if(f.good()){
			strcpy(status, "file opened");
			std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
			editor.SetText(str);
			return true;
		}else{
			strcpy(status, "error opening file");
			return false;
		}
	};

	static bool once = true;
	if(once){
		open_file();
		once = false;
	}

	static auto save_file = [&](){
		auto text_content = editor.GetText();
		std::ofstream tmp_script;
		tmp_script.open(filename_path);
		if(tmp_script.is_open()){
			tmp_script << text_content;
			tmp_script.close();
			strcpy(status, "file saved");
			return true;
		}else{
			strcpy(status, "error opening file");
			return false;
		}
	};

	static bool show_input = false;
	static char save_or_open[5] = "";

	ImGui::Begin("LAak Text Editor", &show, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowFontScale(font_size);
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::BeginMenuBar()){
		if (ImGui::BeginMenu("File")){
			if (ImGui::MenuItem("New")){
				editor.SetText("-- new laak script...\n");
				strcpy(filename_path, "./scripts/new.lua");
			}
			if (ImGui::MenuItem("Open")){
				// open_file();
				show_input = true;
				ImGui::SetWindowFocus("Save/Open");
				strcpy(save_or_open, "open");
			}
			if (ImGui::MenuItem("Save")){
				// save_file();
				show_input = true;
				ImGui::SetWindowFocus("Save/Open");
				strcpy(save_or_open, "save");
			}
			if(ImGui::MenuItem("Run")){
				if(save_file()){
					char dofile[] = "dofile(\"";
					strcat(dofile, filename_path);
					strcat(dofile, "\")");
					entries.push_back({"", pre_laak(dofile)});
					new_entry = true;
					strcpy(status, "running...");
				}else{
					strcpy(status, "error running file");
				}
			}
			if (ImGui::MenuItem("Quit"))
				show = false;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")){
			bool ro = editor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				editor.SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
				editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
				editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
				editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
				editor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
				editor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")){
			if (ImGui::MenuItem("Dark palette"))
				editor.SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light palette"))
				editor.SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro blue palette"))
				editor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Highlight")){
			if (ImGui::MenuItem("Lua"))
				editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
			if (ImGui::MenuItem("C++"))
				editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
			if (ImGui::MenuItem("GLSL"))
				editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Font")){
			ImGui::SliderFloat("size", &font_size, imgui_font_scale, 3.0);
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
		editor.IsOverwrite() ? "Ovr" : "Ins",
		editor.CanUndo() ? "*" : " ",
		editor.GetLanguageDefinition().mName.c_str(), status);

	if(show_input){
		ImGui::Begin("Save/Open", &show_input, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(imgui_font_scale);
		if(ImGui::Button("close")){
			show_input = false;
		}
		ImGui::InputText("filename", filename_path, 100);
		ImGui::SameLine();
		if(ImGui::Button(save_or_open)){
			if(strcmp(save_or_open, "open") == 0){
				open_file();
			}else{
				save_file();
			}
			show_input = false;
		}
		ImGui::End();
	}

	editor.Render("TextEditor");
	ImGui::End();
}

void window_mgr::render(GLFWwindow* window, ImVec4& clear_color){
    // Rendering
	ImGui::Render();
	// int display_w, display_h;
	// glfwGetFramebufferSize(window, &display_w, &display_h);
	// glViewport(0, 0, display_w, display_h);
	// glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	// glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}