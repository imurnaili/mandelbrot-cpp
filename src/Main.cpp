#include <iostream>
#include <iomanip>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "Shader.hpp"

void logGlError(int id) {
	GLenum err;
	while((err = glGetError()) != GL_NO_ERROR) {
		std::cout << id << " - GL Error: 0x" << std::setfill('0') << std::setw(4) << std::hex << err << std::endl;
	}
}

struct WindowData {
	int width, height;
};

int main(int argc, char** argv) {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	Shader shader(
		"#version 450\n"
            "layout(location = 0) in vec3 aPos;\n"
			"uniform vec2 windowSize;\n"
			"out vec2 fragWindowSize;\n"
            "void main() {\n"
            "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"    fragWindowSize = windowSize;\n"
            "}\0",
		"#version 450\n"
            "out vec4 FragColor;\n"
			"in vec2 fragWindowSize;\n"
            "void main() {\n"
            "    FragColor = vec4(gl_FragCoord.x / fragWindowSize.x, gl_FragCoord.y / fragWindowSize.y, 0.0f, 1.0f);\n"
            "}\0");

	WindowData windowData;
	glfwSetWindowUserPointer(window, &windowData);

	glfwGetFramebufferSize(window, &windowData.width, &windowData.height);
	GLint location_windowSize = shader.getUniformLocation("windowSize");
	shader.bind();

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->width = static_cast<float>(width);
		data->height = static_cast<float>(height);
		glViewport(0, 0, width, height);
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
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.bind();
		shader.setVec2(location_windowSize, static_cast<float>(windowData.width), static_cast<float>(windowData.height));

		glBindVertexArray(VAO);
		// tell OpenGL how to interpret vertex data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		// enable vertex attribute
		glEnableVertexAttribArray(0);
		// draw triangle
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}
