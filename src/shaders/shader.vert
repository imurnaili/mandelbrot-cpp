#version 450

layout(location = 0) in vec3 aPos;
uniform vec2 windowSize;
out vec2 fragWindowSize;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	fragWindowSize = windowSize;
}
