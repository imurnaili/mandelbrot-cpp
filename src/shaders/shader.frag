#version 450

out vec4 FragColor;
in vec2 fragWindowSize;

void main() {
	FragColor = vec4(gl_FragCoord.x / fragWindowSize.x, gl_FragCoord.y / fragWindowSize.y, 0.0f, 1.0f);
}
