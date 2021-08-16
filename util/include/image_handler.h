#pragma once

#ifndef STRING
#define STRING
#include <string>
#endif

#ifndef GRAPHICS
#define GRAPHICS
#include <GL/glew.h>
#endif

class image_handler
{
public:
	static void save_png(const std::string& filename, int width, int height, int channels, const void* data);
	static void save_jpg(const std::string& filename, int width, int height, int channels, const void* data);
	static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
};