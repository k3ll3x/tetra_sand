#pragma once
#include <GL/glew.h>

#include "WindowManager.h"

#include "ifile.h"
#include "image_handler.h"

#include "ttime.h"

#include <vec2.h>

#include <iostream>

#include <memory>

struct Shader {
    std::string name;
    std::string text;
    int type;
};

class Scene {
public:	
	~Scene(){
        glDeleteProgram(shader_program);
    }
	virtual void init() = 0;
	virtual void awake() = 0;
	virtual void sleep() = 0;
	virtual void reset() = 0;
	virtual void mainLoop() = 0;
	virtual void resize(int width, int height){};
	virtual void key_callback(int key, int scancode, int action, int mods){};
    virtual void cursor_position_callback(double xpos, double ypos){};
    virtual void mouse_button_callback(int button, int action, int mods){};
	virtual void window_handler(){}

    GLuint shader_program;
    GLint primitiveType;
    static inline std::vector<GLenum> primitiveTypes = {GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY };

    GLFWwindow* window;

    cgmath::vec2 get_window_size(){
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return cgmath::vec2(width, height);
    }

	template <typename V>
    static void loadVertex(std::vector<V>& positions, int i, GLuint vao[], GLuint positionsVBO[]) {
        //glGenVertexArrays(vaoSize, &vao[i]);
        glGenVertexArrays(1, &vao[i]);
        glBindVertexArray(vao[i]);
        //glGenBuffers(vaoSize, &positionsVBO[i]);
        glGenBuffers(1, &positionsVBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, positionsVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, positions.dim(), GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, positionsVBO[i]);
        glBindVertexArray(vao[i]);
    }

    void setupShaders(std::vector<Shader>& shaders){
		GLuint shader[shaders.size()];
		for(int i = 0; i < shaders.size(); i++){
		    std::vector<GLchar> log;
			//load shader and compile
			const GLchar* source = (const GLchar*)shaders[i].text.c_str();
			shader[i] = glCreateShader(shaders[i].type);
			glShaderSource(shader[i], 1, &source, nullptr);
			glCompileShader(shader[i]);

			//check for errors
            GLint compiled;
            GLint log_length;
			glGetShaderiv(shader[i], GL_COMPILE_STATUS, &compiled);
			if (compiled != GL_TRUE) {
				glGetShaderiv(shader[i], GL_INFO_LOG_LENGTH, &log_length);
				log.resize(log_length);
				glGetShaderInfoLog(shader[i], log_length, &log_length, &log[0]);
				for (int j = 0; j < log.size(); j++) {
					std::cout << log[j];
				}
			}
            std::cout << "Shader " << shaders[i].name << " processed!\n";
		}

		//manager
		shader_program = glCreateProgram();
		for(int i = 0; i < shaders.size(); i++){
			glAttachShader(shader_program, shader[i]);
			glBindAttribLocation(shader_program, i, shaders[i].name.c_str());
		}
		glLinkProgram(shader_program);
		for(int i = 0; i < shaders.size(); i++){
			glDeleteShader(shader[i]);
		}
	}
};