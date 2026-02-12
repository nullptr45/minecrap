#pragma once

#include "chunk.h"

#include <glm/glm.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <unordered_map>

struct ChunkHash {
    size_t operator()(const glm::ivec2& pos) const noexcept
    {
        size_t hx = std::hash<int>()(pos.x);
        size_t hz = std::hash<int>()(pos.y);
        return hx ^ (hz << 1);
    }
};

class World {
public:
    World() = default;

    Chunk* get_chunk(const glm::ivec2& chunk_pos);

    void update(const glm::vec3& player_pos);
    void render() const;

private:
    void rebuild_queue();
    void generate_chunks();
    void unload_far_chunks();

    glm::ivec2 world_to_chunk(const glm::vec3& world_pos) const;

private:
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, ChunkHash> loaded_chunks_;

    std::queue<glm::ivec2> gen_queue_;

    glm::ivec2 player_chunk_{INT32_MAX};

    static constexpr int RENDER_DISTANCE = 6;
    static constexpr int CHUNKS_PER_FRAME = 2;
};
