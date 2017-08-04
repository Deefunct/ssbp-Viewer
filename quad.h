#ifndef QUAD_MESH_H
#define QUAD_MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learn_OpenGL/shader.h>
#include <string>

struct Quad {
	Quad() { }
	~Quad() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	Shader shader;
	GLuint VAO=0, VBO=0, EBO=0;
	void init(const std::string& vertexShader, const std::string& fragmentShader) {
		shader.init(vertexShader.c_str(), fragmentShader.c_str());
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// bind Vertex Array Object
		glBindVertexArray(VAO);
		// copy our vertex array in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// copy our index array in an element buffer for OpenGL to use
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// then set the vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	virtual void draw() {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	float vertices[32] = {
		// positions			// colors				// texture coords
		 1.0f,  1.0f, 0.0f,		1.0f, 1.0f, 0.25f,		1.0f, 1.0f,  // top right
		 1.0f, -1.0f, 0.0f,		0.25f, 1.0f, 1.0f,		1.0f, 0.0f,  // bottom right
		-1.0f, -1.0f, 0.0f,		1.0f, 0.25f, 1.0f,		0.0f, 0.0f,  // bottom left
		-1.0f,  1.0f, 0.0f,		0.5f, 0.5f, 0.5f,		0.0f, 1.0f   // top left 
	};
	unsigned int indices[6] = {  // note that we start from 0!
		0, 2, 1,  // first Triangle
		0, 3, 2   // second Triangle
	};
};
#endif //QUAD_MESH_H