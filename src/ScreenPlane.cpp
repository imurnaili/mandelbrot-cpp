#include "ScreenPlane.hpp"
#include <vector>

ScreenPlane::ScreenPlane(float zValue) {

	std::vector<float> vertices {
		-1.0f, -1.0f, zValue, // bottom left
		 1.0f, -1.0f, zValue, // bottom right
		 1.0f,  1.0f, zValue, // top right
		-1.0f,  1.0f, zValue  // top left
	};

	static std::vector<float> uvs {
		0.0f, 0.0f, // bottom left
		1.0f, 0.0f, // bottom right
		1.0f, 1.0f, // top right
		0.0f, 1.0f  // top left
	};

	static std::vector<unsigned int> indices {
		0, 1, 2, // bottom right triangle
		2, 3, 0  // top left triangle
	};

	glCreateVertexArrays(1, &vertexArray);
	
	glCreateBuffers(1, &vertexBuffer);
	glNamedBufferData(vertexBuffer, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(vertexArray, 0);
	glVertexArrayAttribBinding(vertexArray, 0, 0);
	glVertexArrayAttribFormat(vertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vertexArray, 0, vertexBuffer, 0, 3 * sizeof(float));

	glCreateBuffers(1, &uvBuffer);
	glNamedBufferData(uvBuffer, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(vertexArray, 1);
	glVertexArrayAttribBinding(vertexArray, 1, 1);
	glVertexArrayAttribFormat(vertexArray, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vertexArray, 1, uvBuffer, 0, 2 * sizeof(float));

	glCreateBuffers(1, &indexBuffer);
	glNamedBufferData(indexBuffer, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	glVertexArrayElementBuffer(vertexArray, indexBuffer);
}

ScreenPlane::~ScreenPlane() {
	glDeleteVertexArrays(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &indexBuffer);
}

void ScreenPlane::setShader(Shader* shader) {
	this->shader = shader;
}

void ScreenPlane::draw() {
	glBindVertexArray(vertexArray);
	shader->bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
