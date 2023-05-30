#version 450

uniform sampler2D textureSampler;
in vec2 UV;
out vec4 FragColor;

void main() {
	//FragColor = texture(textureSampler, UV);
	FragColor = vec4(UV, 0.0f, 1.0f);
}
