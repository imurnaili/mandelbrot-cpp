#pragma once

#include "Shader.hpp"

class ScreenPlane {
public:
	ScreenPlane(float zValue);
	~ScreenPlane();

	void setShader(Shader* shader);
	void draw();

private:
	unsigned int vertexArray;
	unsigned int vertexBuffer;
	unsigned int uvBuffer;
	unsigned int indexBuffer;
	Shader* shader;
};
