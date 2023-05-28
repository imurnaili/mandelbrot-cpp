#include "Shader.hpp"

Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource) {
	// load and compile vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// load and compile fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// link shaders
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	// delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::bind() {
	glUseProgram(ID);
}

void Shader::setVec2(GLint location, float x, float y) {
	glUniform2f(location, x, y);
}

GLint Shader::getUniformLocation(const std::string& name) {
	return glGetUniformLocation(ID, name.c_str());
}

Shader::~Shader() {
	glDeleteProgram(ID);
}

Shader::Shader(const Shader& other) {
	ID = other.ID;
}
