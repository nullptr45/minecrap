#pragma once

#include <cstdint>
#include <filesystem>

struct Texture {
    Texture(const std::filesystem::path &path);

    ~Texture();

    void bind();
    void unbind();

    int32_t width, height;
    uint32_t id;
};
