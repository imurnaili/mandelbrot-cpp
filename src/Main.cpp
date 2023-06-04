#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <chrono>
#include "Shader.hpp"
#include "ScreenPlane.hpp"
#include "Texture.hpp"

static bool changed = true;
//TODO: 
// - add zooming by fathering the mouse position to the center of the screen and scaling the render area by the scroll offset (use glfwSetScrollCallback) 
// - add panning by fathering the mouse position to the center of the screen and translating the render area by the mouse position delta (use glfwSetCursorPosCallback)
// - add color palette (use glfwSetKeyCallback)
// - add color modes (use glfwSetKeyCallback)
// - add julia set 

struct WindowData {
	glm::dvec2 windowSize;
	glm::dvec2 prevMousePos;
	glm::dvec2 topLeftCorner;
	glm::dvec2 bottomRightCorner;
	Texture* texture;

	~WindowData() { delete texture; }
};

std::string readFileContents(const std::string& path) {
	std::ifstream fs;
	std::stringstream ss;

	fs.open(path, std::ios::in);
	ss << fs.rdbuf();
	fs.close();
	return ss.str();
}

#ifndef NDEBUG
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cout << "OpenGL '" << (type == GL_DEBUG_TYPE_ERROR ? "ERROR"
			: type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR ? "DEPRECATED"
			: type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR ? "UNDEFINED"
			: type == GL_DEBUG_TYPE_PORTABILITY ? "PORTABILITY"
			: type == GL_DEBUG_TYPE_PERFORMANCE ? "PERFORMANCE"
			: type == GL_DEBUG_TYPE_MARKER ? "MARKER"
			: type == GL_DEBUG_TYPE_PUSH_GROUP ? "PUSH_GROUP"
			: type == GL_DEBUG_TYPE_POP_GROUP ? "POP_GROUP"
			: type == GL_DEBUG_TYPE_OTHER ? "OTHER" : "-")
		<< "' (" << (severity == GL_DEBUG_SEVERITY_HIGH ? "HIGH"
			: severity == GL_DEBUG_SEVERITY_MEDIUM ? "MEDIUM"
			: severity == GL_DEBUG_SEVERITY_LOW ? "LOW"
			: severity == GL_DEBUG_SEVERITY_NOTIFICATION ? "NOTIFICATION" : "-")
		<< "): " << message << std::endl;
}
#endif

int main(int argc, char** argv) {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

#ifndef NDEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	std::string vertexShaderCode = readFileContents("shaders/texture.vert");
	std::string fragmentShaderCode = readFileContents("shaders/texture.frag");
	Shader mandelbrotShader(Shader::ShaderType::RENDER, vertexShaderCode.c_str(), fragmentShaderCode.c_str());
	std::string computeShaderCode = readFileContents("shaders/mandelbrot.comp");
	Shader computeShader(Shader::ShaderType::COMPUTE, computeShaderCode.c_str(), nullptr);

	WindowData windowData{ };
	int windowWidth, windowHeight;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	windowData.windowSize.x = windowWidth;
	windowData.windowSize.y = windowHeight;
	windowData.prevMousePos = glm::dvec2();
	windowData.topLeftCorner = glm::dvec2(-2.0f, 2.0f);
	windowData.bottomRightCorner = glm::dvec2(2.0f, -2.0f);
	windowData.texture = new Texture(glm::ivec2(windowData.windowSize), Texture::Format::RGBA32F, Texture::Access::WRITE_ONLY);

	glfwSetWindowUserPointer(window, &windowData);
	GLint location_textureId = mandelbrotShader.getUniformLocation("textureSampler");
	GLint c_topLeftCorner = computeShader.getUniformLocation("topLeftCorner");
	GLint c_bottomRightCorner = computeShader.getUniformLocation("bottomRightCorner");
	GLint c_iterations = computeShader.getUniformLocation("iterations");

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->windowSize.x = width;
		data->windowSize.y = height;
		glViewport(0, 0, width, height);
		data->texture->resize(glm::ivec2(width, height));
		double upp = (data->topLeftCorner.y - data->bottomRightCorner.y) / data->windowSize.y;
		data->bottomRightCorner.x = data->topLeftCorner.x + (data->windowSize.x * upp);
		changed = true;
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
			glm::dvec2 mousePos(xpos, ypos);
			glm::dvec2 delta = (mousePos - data->prevMousePos) * ((data->topLeftCorner.y - data->bottomRightCorner.y) / data->windowSize.y);
			data->topLeftCorner -= delta;
			data->bottomRightCorner -= delta;
			changed = true;
		}
		data->prevMousePos.x = xpos;
		data->prevMousePos.y = ypos;
	});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double, double yoffset) {
		if (yoffset == 0) return;
		WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		const double zoomFactor = 0.05f;
		glm::dvec1 stepDirection(yoffset < 0 ? -1.0f : 1.0f);
		glm::dvec2 scaleValue;
		for (int stepCount = glm::abs(yoffset); stepCount > 0; stepCount--) {
			scaleValue = ((data->topLeftCorner - data->bottomRightCorner) * zoomFactor) * stepDirection;
			data->topLeftCorner -= scaleValue;
			data->bottomRightCorner += scaleValue;
		}
		changed = true;
	});

	ScreenPlane screen = ScreenPlane(1.0f);
	screen.setShader(&mandelbrotShader);

	GLuint timerQuery;
	glGenQueries(1, &timerQuery);

	while (!glfwWindowShouldClose(window)) {
		if (changed) {
			unsigned int iterCount = 1000;
			computeShader.setUint(c_iterations, iterCount);
			computeShader.setdVec2(c_topLeftCorner, windowData.topLeftCorner.x, windowData.topLeftCorner.y);
			computeShader.setdVec2(c_bottomRightCorner, windowData.bottomRightCorner.x, windowData.bottomRightCorner.y);
			computeShader.bindTexture(*windowData.texture, 0);

#ifndef NDEBUG
			glBeginQuery(GL_TIME_ELAPSED, timerQuery);
#endif
			
			computeShader.dispatch(glm::ivec3(windowData.texture->getSize() / 8, 1));
			computeShader.await();

#ifndef NDEBUG
			glEndQuery(GL_TIME_ELAPSED);
			GLuint64 elapsedTime;
			glGetQueryObjectui64v(timerQuery, GL_QUERY_RESULT, &elapsedTime);
			std::cout << "Render time: " << elapsedTime / 1000000.0 << "ms" << std::endl;
#endif

			mandelbrotShader.setInt(location_textureId, 0);
			mandelbrotShader.bindTexture(*windowData.texture, 0);

			screen.draw();

			glfwSwapBuffers(window);
			changed = false;
		}
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}
