#include "chunk.h"

#include <glad/glad.h>
#include <vector>

Chunk::Chunk(const glm::ivec3& pos) : pos_(pos)
{
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex),
                          (void*) offsetof(BlockVertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BlockVertex),
                          (void*) offsetof(BlockVertex, uv));
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(BlockVertex),
                           (void*) offsetof(BlockVertex, tex_layer));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // TODO: temporary
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            for (int z = 0; z < SIZE; z++) {
                blocks_[x][y][z] = 1;
            }
        }
    }
}

Chunk::~Chunk()
{
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ibo_);
    glDeleteVertexArrays(1, &vao_);
}

void Chunk::update(const BlockRegistry& registry)
{
    if (dirty_)
        generate(registry);
}

void Chunk::render() const
{
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
}

BlockID Chunk::get(int x, int y, int z) const
{
    if (x < 0 || y < 0 || z < 0 || x >= SIZE || y >= SIZE || z >= SIZE)
        return 0;

    return blocks_[x][y][z];
}

bool Chunk::face_visible(const BlockRegistry& registry, BlockID current_block, BlockID neighbor)
{
    const auto& a = registry.block_data(current_block);
    const auto& b = registry.block_data(neighbor);

    return !a.transparent && b.transparent;
}

void Chunk::add_face(const BlockFace& face, TextureLayer layer)
{
    uint32_t start_index = vertices_.size();

    vertices_.push_back({
        .pos = face.top_left, .uv = {0, 1},
             .tex_layer = layer
    });
    vertices_.push_back({
        .pos = face.top_right, .uv = {1, 1},
             .tex_layer = layer
    });
    vertices_.push_back({
        .pos = face.bottom_left, .uv = {0, 0},
             .tex_layer = layer
    });
    vertices_.push_back({
        .pos = face.bottom_right, .uv = {1, 0},
             .tex_layer = layer
    });

    indices_.push_back(start_index + 0);
    indices_.push_back(start_index + 2);
    indices_.push_back(start_index + 1);

    indices_.push_back(start_index + 1);
    indices_.push_back(start_index + 2);
    indices_.push_back(start_index + 3);
}

void Chunk::generate(const BlockRegistry& registry)
{
    for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < SIZE; ++y) {
            for (int z = 0; z < SIZE; ++z) {
                BlockID id = get(x, y, z);
                if (id == 0)
                    continue;

                const BlockData& block = registry.block_data(id);

                float wx = float(x + pos_.x * SIZE);
                float wy = float(y + pos_.y * SIZE);
                float wz = float(z + pos_.z * SIZE);

                // Back
                if (face_visible(registry, id, get(x, y, z + 1))) {
                    BlockFace face = {
                        .top_right = {wx + 1, wy + 1, wz + 1},
                        .top_left = {wx,     wy + 1, wz + 1},
                        .bottom_right = {wx + 1, wy,     wz + 1},
                        .bottom_left = {wx,     wy,     wz + 1}
                    };
                    add_face(face, block.side);
                }

                // Front
                if (face_visible(registry, id, get(x, y, z - 1))) {
                    BlockFace face = {
                        .top_right = {wx,     wy + 1, wz},
                        .top_left = {wx + 1, wy + 1, wz},
                        .bottom_right = {wx,     wy,     wz},
                        .bottom_left = {wx + 1, wy,     wz}
                    };
                    add_face(face, block.side);
                }

                // Top
                if (face_visible(registry, id, get(x, y + 1, z))) {
                    BlockFace face = {
                        .top_right = {wx + 1, wy + 1, wz    },
                        .top_left = {wx,     wy + 1, wz    },
                        .bottom_right = {wx + 1, wy + 1, wz + 1},
                        .bottom_left = {wx,     wy + 1, wz + 1}
                    };
                    add_face(face, block.top);
                }

                // Bottom
                if (face_visible(registry, id, get(x, y - 1, z))) {
                    BlockFace face = {
                        .top_right = {wx + 1, wy, wz + 1},
                        .top_left = {wx,     wy, wz + 1},
                        .bottom_right = {wx + 1, wy, wz    },
                        .bottom_left = {wx,     wy, wz    }
                    };
                    add_face(face, block.bottom);
                }

                // Right
                if (face_visible(registry, id, get(x + 1, y, z))) {
                    BlockFace face = {
                        .top_right = {wx + 1, wy + 1, wz    },
                        .top_left = {wx + 1, wy + 1, wz + 1},
                        .bottom_right = {wx + 1, wy,     wz    },
                        .bottom_left = {wx + 1, wy,     wz + 1}
                    };
                    add_face(face, block.side);
                }

                // Left
                if (face_visible(registry, id, get(x - 1, y, z))) {
                    BlockFace face = {
                        .top_right = {wx, wy + 1, wz + 1},
                        .top_left = {wx, wy + 1, wz    },
                        .bottom_right = {wx, wy,     wz + 1},
                        .bottom_left = {wx, wy,     wz    }
                    };
                    add_face(face, block.side);
                }
            }
        }
    }

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(BlockVertex), vertices_.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(uint32_t), indices_.data(),
                 GL_STATIC_DRAW);

    dirty_ = false;
}
