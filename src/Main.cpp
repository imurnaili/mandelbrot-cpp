#include <iostream>
#include <GLFW/glfw3.h>

int main(int argc, char** argv) {
	std::cout << "Hello World!" << std::endl;
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	glfwMakeContextCurrent(window);
	while (!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
}
