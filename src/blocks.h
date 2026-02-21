#pragma once

#include "texture.h"

#include <cstdint>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

using BlockID = uint8_t;

struct BlockData {
    std::string name;

    TextureLayer top;
    TextureLayer side;
    TextureLayer bottom;

    BlockID id;

    bool transparent = false;
};

class BlockRegistry {
public:
    BlockRegistry(std::string path, TextureArray& textures);
    ~BlockRegistry() = default;

    const BlockData& block_data(BlockID id) const { return block_data_[id]; }

    BlockID id(std::string name) const { return block_ids_.at(name); }

    const std::vector<BlockData>& blocks() const { return block_data_; }

private:
    std::vector<BlockData>                   block_data_;
    std::unordered_map<std::string, BlockID> block_ids_;
};
