#include "shader.h"

#include <cassert>
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <string>

static GLuint compile_shader(const char* path, GLenum type)
{
    GLuint s = glCreateShader(type);

    std::ifstream file{path};
    std::stringstream strm;
    strm << file.rdbuf();
    file.close();
    std::string contents = strm.str();
    const char* src      = contents.c_str();

    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    int success;
    char buf[512];
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(s, 512, nullptr, buf);
    if (!success) {
        std::cerr << "Error compiling shader: " << buf << std::endl;
        assert(false);
    }

    return s;
}

Shader::Shader(const char* vs_path, const char* fs_path)
{
    GLuint vs = compile_shader(vs_path, GL_VERTEX_SHADER);
    GLuint fs = compile_shader(fs_path, GL_FRAGMENT_SHADER);
    id        = glCreateProgram();

    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::bind() const
{
    glUseProgram(id);
}

uint32_t Shader::uniform_loc(const char* name) const
{
    return glGetUniformLocation(id, name);
}

void Shader::set_uniform(const char* name, const glm::mat4& mat)
{
    glUniformMatrix4fv(uniform_loc(name), 1, GL_FALSE, &mat[0][0]);
}
