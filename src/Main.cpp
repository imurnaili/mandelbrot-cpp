#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.hpp"
#include "ScreenPlane.hpp"

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
};

std::string readFileContents(const std::string& path) {
	std::ifstream fs;
	std::stringstream ss;

	fs.open(path, std::ios::in);
	ss << fs.rdbuf();
	fs.close();
	return ss.str();
}

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

int main(int argc, char** argv) {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(640, 640, "Mandelbrot", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	std::string vertexShaderCode = readFileContents("shaders/mandelbrot.vert");
	std::string fragmentShaderCode = readFileContents("shaders/mandelbrot.frag");
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

	glfwSetWindowUserPointer(window, &windowData);
	GLint location_windowSize = mandelbrotShader.getUniformLocation("windowSize");
	GLint location_topLeftCorner = mandelbrotShader.getUniformLocation("topLeftCorner");
	GLint location_bottomRightCorner = mandelbrotShader.getUniformLocation("bottomRightCorner");

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->windowSize.x = width;
		data->windowSize.y = height;
		glViewport(0, 0, width, height);
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

	while (!glfwWindowShouldClose(window)) {
		if (changed) {
			glClear(GL_COLOR_BUFFER_BIT);

			mandelbrotShader.setdVec2(location_windowSize, windowData.windowSize.x, windowData.windowSize.y);
			mandelbrotShader.setdVec2(location_topLeftCorner, windowData.topLeftCorner.x, windowData.topLeftCorner.y);
			mandelbrotShader.setdVec2(location_bottomRightCorner, windowData.bottomRightCorner.x, windowData.bottomRightCorner.y);
			screen.draw();

			glfwSwapBuffers(window);
			changed = false;
		}
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}
