#include "texture.h"

#include <cassert>
#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

#include <stdexcept>

Texture::Texture(std::string_view path)
{
    int w, h, c;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(path.data(), &w, &h, &c, 4);

    if (!data)
        throw std::runtime_error("Failed to load texture");

    width_ = w;
    height_ = h;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id_);
}

void Texture::bind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id_);
}

TextureArray::TextureArray(uint32_t max_textures, uint32_t texture_size) :
    max_textures_(max_textures), texture_count_(0), texture_size_(texture_size)
{
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id_);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Allocate full capacity once
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, texture_size_, texture_size_, max_textures_, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

TextureArray::~TextureArray()
{
    glDeleteTextures(1, &id_);
}

TextureLayer TextureArray::add(const std::string& path)
{
    if (path == "none")
        return 0;

    auto it = lookup_.find(path);
    if (it != lookup_.end())
        return it->second;

    assert(texture_count_ < max_textures_ && "TextureArray surpassed max texture count");

    glBindTexture(GL_TEXTURE_2D_ARRAY, id_);

    int w, h, c;
    stbi_set_flip_vertically_on_load(1);
    std::string full_path = "assets/textures/" + path;
    uint8_t*    data = stbi_load(full_path.c_str(), &w, &h, &c, 4);

    std::cout << full_path << std::endl;
    assert(data);

    TextureLayer layer = texture_count_++;

    glBindTexture(GL_TEXTURE_2D_ARRAY, id_);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, texture_size_, texture_size_, 1, GL_RGBA,
                    GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    stbi_image_free(data);

    lookup_[std::string(path)] = layer;

    std::cout << "Loaded texture layer " << layer << ": " << path << "\n";

    return layer;
}

void TextureArray::bind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id_);
}
