#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include "Texture.hpp"

class Shader {
public:
	enum class ShaderType : uint8_t {
		NONE = 0,
		RENDER = 1,
		COMPUTE = 2
	};

public:
	Shader(ShaderType type, const char* vertexShaderSource, const char* fragmentShaderSource);
	Shader(const Shader& other);
	~Shader();

	void bind();
	void bindTexture(Texture& texture, uint8_t slot);
	void setiVec1(GLint location, int x);
	void setVec2(GLint location, float x, float y);
	void setdVec2(GLint location, double x, double y);

	void dispatch(glm::ivec3 workgroupSize);
	void await();

	GLint getUniformLocation(const std::string& name);
	ShaderType getType();

private:
	unsigned int ID;
	ShaderType type;
};
