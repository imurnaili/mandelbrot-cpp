#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
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
	GLint location_topLeftCorner = mandelbrotShader.getUniformLocation("topLeftCorner");
	GLint location_bottomRightCorner = mandelbrotShader.getUniformLocation("bottomRightCorner");
	GLint location_textureId = mandelbrotShader.getUniformLocation("textureSampler");
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

	float uvs[] = {
		// first triangle
		0.0f, 0.0f, // bottom left
		1.0f, 0.0f, // bottom right
		1.0f, 1.0f, // top right
		// second triangle
		1.0f, 0.0f, // top left
		1.0f, 1.0f, // top right
		0.0f, 0.0f  // bottom left
	};

	// temp texture creation code
	glm::ivec2 textureSize(640, 640);
	std::vector<uint8_t> texData(textureSize.x * textureSize.y * 3);
	for (int i = 0; i < texData.size(); i += 3) {
		texData[i  ] = i % 2 == 0 ? 0xff : 0x00;
		texData[i+1] = i % 2 == 0 ? 0x00 : 0xff;
		texData[i+2] = 0x00;
	}
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize.x, textureSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, texData.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		if (changed) {
			glClear(GL_COLOR_BUFFER_BIT);

			mandelbrotShader.bind();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(location_textureId, 0);
			mandelbrotShader.setdVec2(location_windowSize, windowData.windowSize.x, windowData.windowSize.y);
			mandelbrotShader.setdVec2(location_topLeftCorner, windowData.topLeftCorner.x, windowData.topLeftCorner.y);
			mandelbrotShader.setdVec2(location_bottomRightCorner, windowData.bottomRightCorner.x, windowData.bottomRightCorner.y);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));

			// draw triangle
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			glfwSwapBuffers(window);
			changed = false;
		}
		glfwPollEvents();
	}

	// cleanup
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &vertexArray);

	glfwDestroyWindow(window);
	glfwTerminate();
}
