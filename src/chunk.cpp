#include "chunk.h"

#include <glad/glad.h>
#include <vector>

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

Chunk::Chunk(const glm::ivec3& pos) : pos_(pos)
{
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks_[x][y][z] = Block::Grass;
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

void Chunk::update()
{
    if (dirty_) generate();
}

void Chunk::draw() const
{
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);
}

Block Chunk::get_block(int x, int y, int z) const
{
    if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE)
        return Block::Air;

    return blocks_[x][y][z];
}

void Chunk::generate()
{
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;

    auto add_face = [&](Vertex* face) {
        uint32_t start_index = vertices.size();

        for (int i = 0; i < 4; i++) vertices.push_back(face[i]);

        indices.push_back(start_index + 0);
        indices.push_back(start_index + 1);
        indices.push_back(start_index + 3);

        indices.push_back(start_index + 1);
        indices.push_back(start_index + 2);
        indices.push_back(start_index + 3);
    };

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (blocks_[x][y][z] == Block::Air) continue;

                float wx = (float) x;
                float wy = (float) y;
                float wz = (float) z;

                // Front
                if (get_block(x, y, z - 1) == Block::Air) {
                    Vertex face[] = {
                        {{wx, wy + 1, wz},     {0, 1}},
                        {{wx + 1, wy + 1, wz}, {1, 1}},
                        {{wx + 1, wy, wz},     {1, 0}},
                        {{wx, wy, wz},         {0, 0}},
                    };
                    add_face(face);
                }

                // Back)
                if (get_block(x, y, z + 1) == Block::Air) {
                    Vertex face[] = {
                        {{wx + 1, wy + 1, wz + 1}, {1, 1}},
                        {{wx, wy + 1, wz + 1},     {0, 1}},
                        {{wx, wy, wz + 1},         {0, 0}},
                        {{wx + 1, wy, wz + 1},     {1, 0}},
                    };
                    add_face(face);
                }

                // Right
                if (get_block(x + 1, y, z) == Block::Air) {
                    Vertex face[] = {
                        {{wx + 1, wy + 1, wz + 1}, {1, 1}},
                        {{wx + 1, wy + 1, wz},     {0, 1}},
                        {{wx + 1, wy, wz},         {0, 0}},
                        {{wx + 1, wy, wz + 1},     {1, 0}},
                    };
                    add_face(face);
                }

                // Left
                if (get_block(x - 1, y, z) == Block::Air) {
                    Vertex face[] = {
                        {{wx, wy + 1, wz},     {1, 1}},
                        {{wx, wy + 1, wz + 1}, {0, 1}},
                        {{wx, wy, wz + 1},     {0, 0}},
                        {{wx, wy, wz},         {1, 0}},
                    };
                    add_face(face);
                }

                // Top
                if (get_block(x, y + 1, z) == Block::Air) {
                    Vertex face[] = {
                        {{wx, wy + 1, wz + 1},     {0, 1}},
                        {{wx + 1, wy + 1, wz + 1}, {1, 1}},
                        {{wx + 1, wy + 1, wz},     {1, 0}},
                        {{wx, wy + 1, wz},         {0, 0}},
                    };
                    add_face(face);
                }

                // Bottom
                if (get_block(x, y - 1, z) == Block::Air) {
                    Vertex face[] = {
                        {{wx, wy, wz},         {0, 1}},
                        {{wx + 1, wy, wz},     {1, 1}},
                        {{wx + 1, wy, wz + 1}, {1, 0}},
                        {{wx, wy, wz + 1},     {0, 0}},
                    };
                    add_face(face);
                }
            }
        }
    }

    glBindVertexArray(vao_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(),
                 GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(),
                 GL_STATIC_DRAW);

    index_count_ = indices.size();
    dirty_ = false;
}
