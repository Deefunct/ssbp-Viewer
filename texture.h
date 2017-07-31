//*
#ifndef TEXTURE_LOADING_H
#define TEXTURE_LOADING_H

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learn_OpenGL/shader.h>

#include <webp/decode.h>
#include <file_reader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Texture {
	~Texture() { glDeleteTextures(1, &id); }
	Texture() { }
	Texture(const char* file, bool message_on_fail = true) : file_name(file), message_on_fail(message_on_fail) { openFile(); }
	void openFile();
	std::string file_name;
	int width, height, nrChannels;
	unsigned int id = 0;
	bool message_on_fail;
	bool loaded = false;
private:
	unsigned short times_failed = 0;
	unsigned int last_slash = std::string::npos;
	std::string file = "";
	void Texture::retry();
};

#endif // TEXTURE_LOADING_H
/**/
