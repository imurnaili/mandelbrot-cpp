#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

void main() {
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f);
	UV = vertexUV;
}
