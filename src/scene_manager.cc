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

	setupImgui(glsl_version, window);

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

void scene_manager::setupImgui(const char* glsl_version, GLFWwindow* window){
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

void scene_manager::startImguiFrame(){
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

std::string scene_manager::pre_laak(char* input){
	if(!lua_hndl.do_string(input))
		return lua_hndl.errmsg;
	if(lua_hndl.out.empty())
		return input;
	return lua_hndl.out;
}

void scene_manager::matrix_vector_win(bool& show, bool& new_entry){
	static char mv_var[20];
	static char mv_val[255];
	ImGui::Begin("Matrix/Vector Creator", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
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

void scene_manager::script_win(bool& show, bool& new_entry){
	auto cpos = editor.GetCursorPosition();

	static char status[20] = "";

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

	ImGui::Begin("LAak Text Editor", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowFontScale(imgui_font_scale);
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
		ImGui::EndMenuBar();
	}

	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
		editor.IsOverwrite() ? "Ovr" : "Ins",
		editor.CanUndo() ? "*" : " ",
		editor.GetLanguageDefinition().mName.c_str(), status);

	if(show_input){
		ImGui::Begin("Save/Open", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
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

void scene_manager::imguiMain(ImVec4& clear_color){
	static bool mv_win_show = false;
	static bool script_win_show = false;
	static bool new_entry = false;

	ImGui::Begin("LAak");
	ImGui::SetWindowFontScale(imgui_font_scale);

	if(ImGui::Button("entry Matrix/Vector"))
		mv_win_show = !mv_win_show;

	ImGui::SameLine();
	if(ImGui::Button("text editor"))
		script_win_show = !script_win_show;

	ImGui::SameLine();
	if(ImGui::Button("clear entries"))
		entries.clear();

	if(mv_win_show)
		matrix_vector_win(mv_win_show, new_entry);
	if(script_win_show)
		script_win(script_win_show, new_entry);

	ImGui::BeginChild("Entries", {0, 100}, true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
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

	ImGui::TextColored({0.0,1.0,0.0,0.7}, "%s", buf);
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