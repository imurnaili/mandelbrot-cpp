#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
	Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
	void bind();
	void setVec2(GLint location, float x, float y);
	void setdVec2(GLint location, double x, double y);
	GLint getUniformLocation(const std::string& name);
	//destructors
	~Shader();
	//copy
	Shader(const Shader& other);
private:
	unsigned int ID;
};
