#include "Shader.hpp"

Shader::Shader(ShaderType type, const char* vertexShaderSource, const char* fragmentShaderSource) : type(type) {
	if (type == ShaderType::NONE) return;

	unsigned int vertexShader;
	if (type == ShaderType::COMPUTE) vertexShader = glCreateShader(GL_COMPUTE_SHADER);
	else vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	
	unsigned int fragmentShader;
	if (type == ShaderType::RENDER) {
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
		glCompileShader(fragmentShader);
	}
	
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	if (type == ShaderType::RENDER) glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	
	glDeleteShader(vertexShader);
	if (type == ShaderType::RENDER) glDeleteShader(fragmentShader);
}

Shader::Shader(const Shader& other) : ID(other.ID), type(other.type) { }

Shader::~Shader() {
	glDeleteProgram(ID);
}

void Shader::bind() {
	glUseProgram(ID);
}

void Shader::bindTexture(Texture& texture, uint8_t slot) {
	bind();
	texture.bind(slot);
}

void Shader::setInt(GLint location, int x) {
	bind();
	glUniform1i(location, x);
}

void Shader::setUint(GLint location, unsigned int x) {
	bind();
	glUniform1ui(location, x);
}

void Shader::setVec2(GLint location, float x, float y) {
	bind();
	glUniform2f(location, x, y);
}

void Shader::setdVec2(GLint location, double x, double y) {
	bind();
	glUniform2d(location, x, y);
}

void Shader::dispatch(glm::ivec3 workgroupSize) {
	if (type != ShaderType::COMPUTE) return;
	bind();
	glDispatchCompute(workgroupSize.x, workgroupSize.y, workgroupSize.z);
}

void Shader::await() {
	if (type != ShaderType::COMPUTE) return;
	bind();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

GLint Shader::getUniformLocation(const std::string& name) {
	return glGetUniformLocation(ID, name.c_str());
}

Shader::ShaderType Shader::getType() {
	return type;
}
