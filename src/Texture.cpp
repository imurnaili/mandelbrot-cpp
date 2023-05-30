#include "Texture.hpp"
#include <glad/glad.h>

Texture::Texture(glm::ivec2 size, Format format, Access access, Filter filter, Wrap wrap)
	: size(size), format(format), access(access), filter(filter), wrap(wrap) {
	
	glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
	glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, static_cast<uint16_t>(filter));
	glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, static_cast<uint16_t>(filter));
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, static_cast<uint16_t>(wrap));
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, static_cast<uint16_t>(wrap));
	glTextureStorage2D(textureId, 1, static_cast<uint16_t>(format), size.x, size.y);
}

Texture::~Texture() {
	glDeleteTextures(1, &textureId);
}

void Texture::bind(uint8_t slot) {
	glBindImageTexture(slot, textureId, 0, GL_FALSE, 0, static_cast<uint16_t>(access), static_cast<uint16_t>(format));
	glBindTextureUnit(slot, textureId);
}

void Texture::resize(glm::ivec2 newSize) {
	if (newSize == size) return;

	unsigned int newTextureId;
	glCreateTextures(GL_TEXTURE_2D, 1, &newTextureId);
	glTextureParameteri(newTextureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(newTextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(newTextureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(newTextureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureStorage2D(newTextureId, 1, static_cast<uint16_t>(format), newSize.x, newSize.y);
	glBindImageTexture(0, newTextureId, 0, GL_FALSE, 0, static_cast<uint16_t>(access), static_cast<uint16_t>(format));

	glDeleteTextures(1, &textureId);

	textureId = newTextureId;
	size = newSize;
}

glm::ivec2 Texture::getSize() {
	return size;
}

Texture::Format Texture::getFormat() {
	return format;
}

Texture::Access Texture::getAccess() {
	return access;
}
