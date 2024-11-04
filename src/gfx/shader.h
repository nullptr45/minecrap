#pragma once

#include <cstdint>
#include <string>

class Shader {
public:
    Shader(const char *vs, const char *fs);

    void bind();
    void unbind();

    void set_int(const char *name, int value) const;
    void set_uint(const char *name, uint32_t value) const;
    void set_float(const char *name, float value) const;

private:
    uint32_t id;

    std::string read_file(const char *path);
    uint32_t compile(const char *contents, uint32_t type);
};
