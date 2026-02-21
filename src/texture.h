#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

class Texture {
public:
    Texture(std::string_view path);
    ~Texture();

    void bind(uint32_t slot = 0) const;

    int width() const { return width_; }
    int height() const { return height_; }

private:
    uint32_t id_;

    int width_;
    int height_;
};

using TextureLayer = uint16_t;

class TextureArray {
public:
    TextureArray(uint32_t max_textures, uint32_t texture_size);
    ~TextureArray();

    TextureLayer add(const std::string& path);
    void         bind(uint32_t slot = 0) const;

    uint32_t count() const { return texture_count_; }

private:
    uint32_t id_;

    uint32_t     texture_size_;
    uint32_t     max_textures_;
    TextureLayer texture_count_;

    std::unordered_map<std::string, TextureLayer> lookup_;
};
