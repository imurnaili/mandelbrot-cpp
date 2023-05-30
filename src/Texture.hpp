#pragma once

#include <glm/glm.hpp>
#include <cstdint>

class Texture {
public:
    Texture(glm::ivec2 size);
    ~Texture();

    void bind(uint8_t slot);
    glm::ivec2 getSize();

private:
    unsigned int textureId;
    glm::ivec2 size;
};
