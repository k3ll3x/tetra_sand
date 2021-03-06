#pragma once

#ifndef GRAPHICS
#define GRAPHICS
#include <GL/glew.h>
#endif

#include "window_mgr.h"

#include "ifile.h"
#include "image_handler.h"

#include "ttime.h"

#include "audio.h"

#ifndef IOSTREAM
#define IOSTREAM
#include <iostream>
#endif

#include <memory>

struct Shader {
	std::string file;
	int type;
	std::string name;
};

class scene {
public:
	inline static auto audio_mgr = std::make_unique<audio>();
	GLuint shader_program;
	
	virtual ~scene() {
		audio_mgr.release();
	}

	virtual void init() = 0;
	virtual void awake() = 0;
	virtual void sleep() = 0;
	virtual void reset() = 0;
	virtual void mainLoop() = 0;
	virtual void resize(GLFWwindow* window, int width, int height) = 0;
	virtual void error_callback(int error, const char* desc) = 0;
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;

	virtual void sceneWindowHandler() = 0;

	virtual void setupShaders(std::vector<Shader>& shaders){
		ifile shader_file[shaders.size()];
		std::string shader_source[shaders.size()];
		GLchar* shader_source_c[shaders.size()];
		GLuint shader[shaders.size()];
		GLint shader_compiled[shaders.size()];
		GLint log_length;
		std::vector<GLchar> log;
		for(int i = 0; i < shaders.size(); i++){
			//load shader and compile
			shader_file[i].read(shaders[i].file);
			shader_source[i] = shader_file[i].get_contents();
			shader_source_c[i] = (GLchar*)shader_source[i].c_str();
			shader[i] = glCreateShader(shaders[i].type);
			glShaderSource(shader[i], 1, &shader_source_c[i], nullptr);
			glCompileShader(shader[i]);

			//check for errors
			glGetShaderiv(shader[i], GL_COMPILE_STATUS, &shader_compiled[i]);
			if (shader_compiled[i] != GL_TRUE) {
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
		}
		for(int i = 0; i < shaders.size(); i++){
			glBindAttribLocation(shader_program, i, shaders[i].name.c_str());
		}
		glLinkProgram(shader_program);
		for(int i = 0; i < shaders.size(); i++){
			glDeleteShader(shader[i]);
		}
	}
};