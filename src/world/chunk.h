#pragma once

#include "blocks.h"
#include <cstdint>

#include <glm/glm.hpp>
#include <vector>

struct BlockFace {
    glm::vec3 top_right;
    glm::vec3 top_left;
    glm::vec3 bottom_right;
    glm::vec3 bottom_left;
};

struct BlockVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    uint32_t  tex_layer;
};

class Chunk {
public:
    static constexpr int SIZE = 16;

    Chunk(const glm::ivec3& pos);
    ~Chunk();

    void update(const BlockRegistry& registry);
    void render() const;

    BlockID get(int x, int y, int z) const;
    void    set(int x, int y, int z, BlockID id);

    glm::ivec3 position() const { return pos_; }

private:
    bool face_visible(const BlockRegistry& registry, BlockID current_block, BlockID neighbor);
    void add_face(const BlockFace& face, TextureLayer layer);
    void generate(const BlockRegistry& registry);

private:
    BlockID blocks_[SIZE][SIZE][SIZE];

    std::vector<BlockVertex> vertices_;
    std::vector<uint32_t>    indices_;

    glm::ivec3 pos_;

    uint32_t vao_;
    uint32_t vbo_;
    uint32_t ibo_;

    bool dirty_ = true;
};
