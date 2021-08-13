#ifndef WINDOWMGR
#define WINDOWMGR
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

//tex editor
#include "../ImGuiColorTextEdit/TextEditor.h"
#endif

#ifndef STRING
#define STRING
#include <string>
#endif

#ifndef VECTOR
#define VECTOR
#include <vector>
#endif

#ifndef FSTREAM
#define FSTREAM
#include <fstream>
#endif

#ifndef LAAKHNDR
#define LAAKHNDR
#include "LAakHandler.h"
#endif

struct entry {
	std::string input, output;
};

class window_mgr {
public:
    window_mgr(LAakHandler* laak_hndl);
    // ~window_mgr() = delete;
    void setup(const char* glsl_version, GLFWwindow* window);
    void start_frame();
    void main(ImVec4& clear_color);
    void matrix_vector_win(bool& show, bool& new_entry);
	void script_win(bool& show, bool& new_entry);
	void render(GLFWwindow* window, ImVec4& clear_color);
    void clean();
private:
    LAakHandler* laak_hndl;

    float imgui_font_scale = 1.5;

	//text editor
	TextEditor editor;
	char filename_path[100] = "./scripts/tmp.lua";

    inline static const unsigned int buf_s = 255;
	char buf[buf_s];
	std::vector<entry> entries;
	int entry_idx = -1;

    std::string pre_laak(char* input);
};