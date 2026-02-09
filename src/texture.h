#pragma once

#include <cstdint>

class Texture {
public:
    Texture(const char* path);
    ~Texture();

    void bind() const;

private:
    uint32_t id_;
};
