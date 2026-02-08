#pragma once

#include <glm/glm.hpp>

#include <cstdint>

class Shader {
public:
    Shader(const char* vs_path, const char* fs_path);
    ~Shader();

    void bind() const;

    uint32_t uniform_loc(const char* name) const;

    void set_uniform(const char* name, const glm::mat4& mat);

private:
    uint32_t id;
};
