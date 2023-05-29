#version 450

layout(location = 0) in vec3 aPos;
uniform dvec2 windowSize;
uniform dvec2 renderTopLeftCorner;
uniform dvec2 renderBottomRightCorner;
out dvec2 fragWindowSize;
out dvec2 topLeftCorner;
out dvec2 bottomRightCorner;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	fragWindowSize = windowSize;
	topLeftCorner = renderTopLeftCorner;
	bottomRightCorner = renderBottomRightCorner;
}
