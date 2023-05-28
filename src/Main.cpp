#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "Shader.hpp"

bool changed = true;
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
	glm::ivec2 windowSize;
	glm::fvec2 center;
	glm::fvec2 prevMousePos;
	glm::fvec2 topLeftCorner;
	glm::fvec2 bottomRightCorner;
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
	Shader shader(vertexShaderCode.c_str(), fragmentShaderCode.c_str());

	WindowData windowData{ };
	glfwGetFramebufferSize(window, &windowData.windowSize.x, &windowData.windowSize.y);
	windowData.center = glm::fvec2();
	windowData.prevMousePos = glm::fvec2();
	windowData.topLeftCorner = glm::fvec2(-2.0f, 2.0f);
	windowData.bottomRightCorner = glm::fvec2(2.0f, -2.0f);

	glfwSetWindowUserPointer(window, &windowData);
	GLint location_windowSize = shader.getUniformLocation("windowSize");
	GLint location_topLeftCorner = shader.getUniformLocation("renderTopLeftCorner");
	GLint location_bottomRightCorner = shader.getUniformLocation("renderBottomRightCorner");
	shader.bind();

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->windowSize.x = static_cast<float>(width);
		data->windowSize.y = static_cast<float>(height);
		glViewport(0, 0, width, height);
		changed = true;
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
			glm::fvec2 mousePos(static_cast<float>(xpos), static_cast<float>(ypos));
			glm::fvec2 mousePosDelta = data->prevMousePos - mousePos;
			changed = true;
		}
		data->prevMousePos.x = static_cast<float>(xpos);
		data->prevMousePos.y = static_cast<float>(ypos);
	});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double, double yoffset) {
		WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		glm::fvec2 scaleFactor(-0.1f, 0.1f);
		data->topLeftCorner -= scaleFactor * glm::fvec1(static_cast<float>(yoffset));
		data->bottomRightCorner += scaleFactor * glm::fvec1(static_cast<float>(yoffset));
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

			shader.bind();
			shader.setVec2(location_windowSize, static_cast<float>(windowData.windowSize.x), static_cast<float>(windowData.windowSize.y));
			shader.setVec2(location_topLeftCorner, windowData.topLeftCorner.x, windowData.topLeftCorner.y);
			shader.setVec2(location_bottomRightCorner, windowData.bottomRightCorner.x, windowData.bottomRightCorner.y);

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
