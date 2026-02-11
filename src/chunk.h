#pragma once

#include <cstdint>

#include <glm/glm.hpp>

enum class Block { Air, Grass };

constexpr int CHUNK_SIZE = 16;

class Chunk {
public:
    Chunk(const glm::ivec3& pos);
    ~Chunk();

    void update();
    void draw() const;

    Block get_block(int x, int y, int z) const;

private:
    void generate();

private:
    Block blocks_[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    glm::ivec3 pos_;

    uint32_t vao_;
    uint32_t vbo_;
    uint32_t ibo_;
    uint32_t index_count_;

    bool dirty_ = true;
};
