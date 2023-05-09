#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

int main(int argc, char** argv) {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// shader boiler plate
		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"void main() {\n"
			"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";
		const char* fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main() {\n"
			"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
			"}\0";
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
		unsigned int shaderProgram;
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		// delete shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		// create vertex buffer
		float vertices[] = {
			-0.5f, -0.5f, 0.0f, // left
			0.5f, -0.5f, 0.0f, // right
			0.0f, 0.5f, 0.0f // top
		};
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// create vertex array object
		unsigned int VAO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		// configure vertex attributes
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// draw triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// swap buffers and poll events

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
}
