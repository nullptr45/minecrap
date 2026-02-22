#include "world.h"

World::World(const BlockRegistry& blocks) : blocks_(blocks) {}

glm::ivec2 World::world_to_chunk(const glm::vec3& world_pos) const
{
    return {static_cast<int>(std::floor(world_pos.x / Chunk::SIZE)),
            static_cast<int>(std::floor(world_pos.z / Chunk::SIZE))};
}

Chunk* World::get_chunk(const glm::ivec2& chunk_pos)
{
    auto it = loaded_chunks_.find(chunk_pos);
    if (it == loaded_chunks_.end())
        return nullptr;

    return it->second.get();
}

void World::update(const glm::vec3& player_pos)
{
    glm::ivec2 current_chunk = world_to_chunk(player_pos);

    if (!glm::all(glm::equal(current_chunk, player_chunk_))) {
        player_chunk_ = current_chunk;
        rebuild_queue();
    }

    generate_chunks();
    unload_far_chunks();

    for (const auto& [pos, chunk] : loaded_chunks_)
        chunk->update(blocks_);
}

void World::rebuild_queue()
{
    std::vector<glm::ivec2> desired;

    for (int dx = -RENDER_DISTANCE; dx <= RENDER_DISTANCE; ++dx)
        for (int dz = -RENDER_DISTANCE; dz <= RENDER_DISTANCE; ++dz) {
            if (dx * dx + dz * dz > RENDER_DISTANCE * RENDER_DISTANCE)
                continue;

            glm::ivec2 coord = player_chunk_ + glm::ivec2(dx, dz);

            if (loaded_chunks_.find(coord) == loaded_chunks_.end())
                desired.push_back(coord);
        }

    // Sort by distance so nearest loads first
    std::sort(desired.begin(), desired.end(), [this](const glm::ivec2& a, const glm::ivec2& b) {
        int dx1 = a.x - player_chunk_.x;
        int dz1 = a.y - player_chunk_.y;
        int dx2 = b.x - player_chunk_.x;
        int dz2 = b.y - player_chunk_.y;

        int dist_a = dx1 * dx1 + dz1 * dz1;
        int dist_b = dx2 * dx2 + dz2 * dz2;

        return dist_a < dist_b;
    });

    for (const auto& coord : desired)
        gen_queue_.push(coord);
}

void World::generate_chunks()
{
    for (int i = 0; i < CHUNKS_PER_FRAME && !gen_queue_.empty(); ++i) {
        glm::ivec2 coord = gen_queue_.front();
        gen_queue_.pop();

        if (loaded_chunks_.find(coord) != loaded_chunks_.end())
            continue;

        loaded_chunks_.emplace(coord, std::make_unique<Chunk>(glm::ivec3(coord.x, 0, coord.y)));
    }
}

void World::unload_far_chunks()
{
    for (auto it = loaded_chunks_.begin(); it != loaded_chunks_.end();) {
        int dx = it->first.x - player_chunk_.x;
        int dz = it->first.y - player_chunk_.y;

        if (dx * dx + dz * dz > RENDER_DISTANCE * RENDER_DISTANCE)
            it = loaded_chunks_.erase(it);
        else
            ++it;
    }
}

void World::render() const
{
    for (const auto& [pos, chunk] : loaded_chunks_)
        chunk->render();
}
