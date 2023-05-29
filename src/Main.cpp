#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "Shader.hpp"

static bool changed = true;
//TODO: 
// - add zooming by fathering the mouse position to the center of the screen and scaling the render area by the scroll offset (use glfwSetScrollCallback) 
// - add panning by fathering the mouse position to the center of the screen and translating the render area by the mouse position delta (use glfwSetCursorPosCallback)
// - add color palette (use glfwSetKeyCallback)
// - add color modes (use glfwSetKeyCallback)
// - add julia set 

void logGlError(int id) {
	GLenum err;
	while((err = glGetError()) != GL_NO_ERROR) {
		std::cout << id << " - GL Error: 0x" << std::setfill('0') << std::setw(4) << std::hex << err << std::endl;
	}
}

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

int main(int argc, char** argv) {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	std::string vertexShaderCode = readFileContents("shaders/mandelbrot.vert");
	std::string fragmentShaderCode = readFileContents("shaders/mandelbrot.frag");
	Shader mandelbrotShader(vertexShaderCode.c_str(), fragmentShaderCode.c_str());

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
	GLint location_topLeftCorner = mandelbrotShader.getUniformLocation("renderTopLeftCorner");
	GLint location_bottomRightCorner = mandelbrotShader.getUniformLocation("renderBottomRightCorner");
	mandelbrotShader.bind();

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

	// cover screen with two triangles
	float vertices[] = {
		// first triangle
		-1.0f, -1.0f,  0.0f, // bottom left
		 1.0f, -1.0f,  0.0f, // bottom right
		 1.0f,  1.0f,  0.0f, // top right
		// second triangle
		-1.0f,  1.0f,  0.0f, // top left
		 1.0f,  1.0f,  0.0f, // top right
		-1.0f, -1.0f,  0.0f  // bottom left
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// copy vertices into buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// create vertex array object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	while (!glfwWindowShouldClose(window)) {
		if (changed) {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			mandelbrotShader.bind();
			mandelbrotShader.setdVec2(location_windowSize, windowData.windowSize.x, windowData.windowSize.y);
			mandelbrotShader.setdVec2(location_topLeftCorner, windowData.topLeftCorner.x, windowData.topLeftCorner.y);
			mandelbrotShader.setdVec2(location_bottomRightCorner, windowData.bottomRightCorner.x, windowData.bottomRightCorner.y);

			glBindVertexArray(VAO);
			// tell OpenGL how to interpret vertex data
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			// enable vertex attribute
			glEnableVertexAttribArray(0);
			// draw triangle
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glfwSwapBuffers(window);
			changed = false;
		}
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}
