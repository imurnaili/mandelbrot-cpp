#include "Texture.hpp"
#include <glad/glad.h>

Texture::Texture(glm::ivec2 size) : size(size) {
	glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
	glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureStorage2D(textureId, 1, GL_RGBA32F, size.x, size.y);
	glBindImageTexture(0, textureId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureId);
}

void Texture::bind(uint8_t slot) {
    glBindTextureUnit(slot, textureId);
}

glm::ivec2 Texture::getSize() {
    return size;
}
