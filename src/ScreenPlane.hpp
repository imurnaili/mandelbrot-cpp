#pragma once

#include "Shader.hpp"

class ScreenPlane {
public:
	ScreenPlane();
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
