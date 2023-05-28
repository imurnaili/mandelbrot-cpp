#version 450

layout(location = 0) in vec3 aPos;
uniform vec2 windowSize;
uniform vec2 renderTopLeftCorner;
uniform vec2 renderBottomRightCorner;
out vec2 fragWindowSize;
out vec2 topLeftCorner;
out vec2 bottomRightCorner;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	fragWindowSize = windowSize;
	topLeftCorner = renderTopLeftCorner;
	bottomRightCorner = renderBottomRightCorner;
}
