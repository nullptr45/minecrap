#include "blocks.h"

#include <fstream>
#include <nlohmann/json.hpp>

BlockRegistry::BlockRegistry(std::string path, TextureArray& textures)
{
    using json = nlohmann::json;
    std::ifstream file(path);
    json          data = json::parse(file);
    auto          blocks = data["blocks"];

    BlockID id = 0;

    for (const auto& b : blocks) {
        BlockData block{};
        block.id = id;

        std::string name = b["name"];
        block.name = name;
        block_ids_.insert_or_assign(std::string(name), id);

        const auto& tex = b["textures"];
        if (tex.contains("all")) {
            block.top = block.side = block.bottom = textures.add(tex["all"]);
        } else {
            block.top = textures.add(tex["top"]);
            block.side = textures.add(tex["side"]);
            block.bottom = textures.add(tex["bottom"]);
        }

        block.transparent = b["transparent"];

        block_data_.push_back(block);
        id++;
    }
}
