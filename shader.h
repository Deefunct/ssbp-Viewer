#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <file_reader.h>

class Shader
{
public:
	// the program ID
    unsigned int ID;
	
    // Constructor reads and builds the shader
	Shader() {};
	void init(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use() { glUseProgram(ID); }
	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, float value1, float value2) const;
	void setVec3v(const std::string & name, const float* value, GLsizei count) const;
	void setVec4v(const std::string & name, const float* value, GLsizei count) const;
	void setMat4(const std::string& name, const float* value) const;
	void setTexture2D(const std::string& name, GLuint texture, int unit=0) {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), unit);
		glActiveTexture(GL_TEXTURE0+unit);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
};

#endif