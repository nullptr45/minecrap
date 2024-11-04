#include "shader.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const char *vs, const char *fs)
{
    uint32_t v = compile(read_file(vs).c_str(), GL_VERTEX_SHADER);
    uint32_t f = compile(read_file(fs).c_str(), GL_FRAGMENT_SHADER);

    id = glCreateProgram();
    glAttachShader(id, v);
    glAttachShader(id, f);
    glLinkProgram(id);
    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    char info_log[512];
    if(!success) {
        glGetProgramInfoLog(id, 512, nullptr, info_log);
        std::cerr << info_log << std::endl;
    }

    glDeleteShader(v);
    glDeleteShader(f);
}

std::string Shader::read_file(const char *path)
{
    std::string contents;
    std::ifstream file;

    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    try {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        contents = stream.str();
    } catch (std::ifstream::failure e) {
        std::cerr << "Failed to read file: " << path << std::endl;
    }

    return contents;
}

uint32_t Shader::compile(const char *contents, uint32_t type)
{
    uint32_t result;
    int success;
    char info_log[512];

    result = glCreateShader(type);
    glShaderSource(result, 1, &contents, nullptr);
    glCompileShader(result);

    glGetShaderiv(result, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(result, 512, nullptr, info_log);
        std::cerr << "Failed to compile shader: " << info_log << std::endl;
    }

    return result;
}

void Shader::bind()
{
    glUseProgram(id);
}

void Shader::set_int(const char *name, int value) const
{
    glUniform1i(glGetUniformLocation(id, name), value);
}
