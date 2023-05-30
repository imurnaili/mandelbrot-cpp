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
	GLFWwindow* window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
	GLint location_textureId = mandelbrotShader.getUniformLocation("textureSampler");
	GLint c_topLeftCorner = computeShader.getUniformLocation("topLeftCorner");
	GLint c_bottomRightCorner = computeShader.getUniformLocation("bottomRightCorner");

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

	std::vector<float> vertices {
		-1.0f, -1.0f,  0.0f, // bottom left
		 1.0f, -1.0f,  0.0f, // bottom right
		 1.0f,  1.0f,  0.0f, // top right
		-1.0f,  1.0f,  0.0f  // top left
	};

	std::vector<float> uvs {
		0.0f, 0.0f, // bottom left
		1.0f, 0.0f, // bottom right
		1.0f, 1.0f, // top right
		0.0f, 1.0f  // top left
	};

	std::vector<unsigned int> indices {
		0, 1, 2, // bottom right triangle
		2, 3, 0  // top left triangle
	};

	// temp texture creation code
	glm::ivec2 textureSize(640, 640);
	GLuint texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureStorage2D(texture, 1, GL_RGBA32F, textureSize.x, textureSize.y);
	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Buffers
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint indexBuffer;
	GLuint uvBuffer;
	
	glCreateVertexArrays(1, &vertexArray);

	glCreateBuffers(1, &vertexBuffer);
	glNamedBufferData(vertexBuffer, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(vertexArray, 0);
	glVertexArrayAttribBinding(vertexArray, 0, 0);
	glVertexArrayAttribFormat(vertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vertexArray, 0, vertexBuffer, 0, 3 * sizeof(float));

	glCreateBuffers(1, &uvBuffer);
	glNamedBufferData(uvBuffer, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(vertexArray, 1);
	glVertexArrayAttribBinding(vertexArray, 1, 1);
	glVertexArrayAttribFormat(vertexArray, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vertexArray, 1, uvBuffer, 0, 2 * sizeof(float));

	glCreateBuffers(1, &indexBuffer);
	glNamedBufferData(indexBuffer, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	glVertexArrayElementBuffer(vertexArray, indexBuffer);

	glBindVertexArray(vertexArray);

	while (!glfwWindowShouldClose(window)) {
		if (changed) {
			glClear(GL_COLOR_BUFFER_BIT);

			computeShader.bind();
			computeShader.setdVec2(c_topLeftCorner, windowData.topLeftCorner.x, windowData.topLeftCorner.y);
			computeShader.setdVec2(c_bottomRightCorner, windowData.bottomRightCorner.x, windowData.bottomRightCorner.y);
			glBindTextureUnit(0, texture);
			computeShader.dispatch(glm::ivec3(textureSize / 8, 1));
			computeShader.await();

			mandelbrotShader.bind();
			mandelbrotShader.setiVec1(location_textureId, 0);
			mandelbrotShader.setdVec2(location_windowSize, windowData.windowSize.x, windowData.windowSize.y);
			glBindTextureUnit(0, texture);

			// draw triangle
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			glfwSwapBuffers(window);
			changed = false;
		}
		glfwPollEvents();
	}

	// cleanup
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &vertexArray);

	glfwDestroyWindow(window);
	glfwTerminate();
}
