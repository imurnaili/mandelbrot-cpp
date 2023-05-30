#pragma once

#include <glm/glm.hpp>
#include <cstdint>

class Texture {
public:
	enum class Format : uint16_t {
		RGBA32F = 0x8814
	};

	enum class Access : uint16_t {
		READ_ONLY = 0x88B8,
		WRITE_ONLY = 0x88B9,
		READ_WRITE = 0x88BA
	};

	enum class Filter : uint16_t {
		NEAREST = 0x2600,
		LINEAR = 0x2601
	};

	enum class Wrap : uint16_t {
		REPEAT = 0x2901,
		MIRRORED_REPEAT = 0x8370,
		CLAMP_TO_EDGE = 0x812F,
		CLAMP_TO_BORDER = 0x812D
	};

public:
	Texture(glm::ivec2 size, Format format, Access access, Filter filter = Filter::NEAREST, Wrap wrap = Wrap::REPEAT);
	~Texture();

	void bind(uint8_t slot);
	void resize(glm::ivec2 newSize);
	glm::ivec2 getSize();
	Format getFormat();
	Access getAccess();

private:
	unsigned int textureId;
	glm::ivec2 size;
	Format format;
	Access access;
	Filter filter;
	Wrap wrap;
};
