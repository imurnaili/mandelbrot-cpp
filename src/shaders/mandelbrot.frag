#version 450

uniform sampler2D textureSampler;
in vec2 UV;
out vec3 color;

void main() {
	color = texture(textureSampler, UV).rgb;
}
