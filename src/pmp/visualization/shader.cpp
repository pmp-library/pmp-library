// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/visualization/shader.h"
#include "pmp/exceptions.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace pmp {

Shader::~Shader()
{
    cleanup();
}

void Shader::cleanup()
{
    if (pid_)
    {
        glDeleteProgram(pid_);
        pid_ = 0;
    }

    for (GLint id : shaders_)
    {
        glDeleteShader(id);
    }
    shaders_.clear();
}

void Shader::source(const char* vshader, const char* fshader)
{
    GLint id;

    // cleanup existing shaders first
    cleanup();

    // create program
    pid_ = glCreateProgram();

    // vertex shader
    id = compile(vshader, GL_VERTEX_SHADER);
    glAttachShader(pid_, id);

    // fragment shader
    id = compile(fshader, GL_FRAGMENT_SHADER);
    glAttachShader(pid_, id);

    // link program
    link();
}

void Shader::load(const char* vfile, const char* ffile, const char* gfile,
                  const char* tcfile, const char* tefile)
{
    GLint id;

    // cleanup existing shaders first
    cleanup();

    // create program
    pid_ = glCreateProgram();

    // vertex shader
    id = load_and_compile(vfile, GL_VERTEX_SHADER);
    glAttachShader(pid_, id);

    // fragment shader
    id = load_and_compile(ffile, GL_FRAGMENT_SHADER);
    glAttachShader(pid_, id);

    // tessellation control shader
    if (tcfile)
    {
        id = load_and_compile(tcfile, GL_TESS_CONTROL_SHADER);
        glAttachShader(pid_, id);
    }

    // tessellation evaluation shader
    if (tefile)
    {
        id = load_and_compile(tefile, GL_TESS_EVALUATION_SHADER);
        glAttachShader(pid_, id);
    }

    // geometry shader
    if (gfile)
    {
        id = load_and_compile(gfile, GL_GEOMETRY_SHADER);
        glAttachShader(pid_, id);
    }

    // link program
    link();
}

void Shader::link()
{
    glLinkProgram(pid_);
    GLint status;
    glGetProgramiv(pid_, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(pid_, GL_INFO_LOG_LENGTH, &length);
        auto info = std::vector<GLchar>(length + 1);
        glGetProgramInfoLog(pid_, length, nullptr, info.data());
        auto what = "Shader: Cannot link program:" + std::string(info.data());
        cleanup();
        throw GLException(what);
    }
}

void Shader::load(const char* filename, std::string& source)
{
    std::ifstream ifs(filename);
    if (!ifs)
    {
        auto what = "Shader: Cannot open file:" + std::string(filename);
        cleanup();
        throw IOException(what);
    }

    std::stringstream ss;
    ss << ifs.rdbuf();
    source = ss.str();

    ifs.close();
}

GLint Shader::compile(const char* source, GLenum type)
{
    // create shader
    GLint id = glCreateShader(type);
    if (!id)
    {
        auto what = "Shader: Cannot create shader object.\n";
        cleanup();
        throw GLException(what);
    }
    shaders_.push_back(id);

    // compile vertex shader
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    // check compile status
    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        auto info = std::vector<GLchar>(length + 1);
        glGetShaderInfoLog(id, length, nullptr, info.data());
        auto what = "Shader: Cannot compile shader:" + std::string(info.data());
        cleanup();
        throw GLException(what);
    }

    return id;
}

GLint Shader::load_and_compile(const char* filename, GLenum type)
{
    std::string source;
    load(filename, source);
    return compile(source.c_str(), type);
}

void Shader::use()
{
    if (pid_)
        glUseProgram(pid_);
}

void Shader::disable()
{
    glUseProgram(0);
}

void Shader::bind_attribute(const char* name, GLuint index)
{
    if (!pid_)
        return;
    glBindAttribLocation(pid_, index, name);
    link(); // have to re-link now!
}

void Shader::set_uniform(const char* name, float value)
{
    if (!pid_)
        return;
    int location = glGetUniformLocation(pid_, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    }
    glUniform1f(location, value);
}

void Shader::set_uniform(const char* name, int value)
{
    if (!pid_)
        return;
    int location = glGetUniformLocation(pid_, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    }
    glUniform1i(location, value);
}

void Shader::set_uniform(const char* name, const vec3& vec)
{
    if (!pid_)
        return;
    int location = glGetUniformLocation(pid_, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    };
    glUniform3f(location, vec[0], vec[1], vec[2]);
}

void Shader::set_uniform(const char* name, const vec4& vec)
{
    if (!pid_)
        return;
    int location = glGetUniformLocation(pid_, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    }
    glUniform4f(location, vec[0], vec[1], vec[2], vec[3]);
}

void Shader::set_uniform(const char* name, const mat3& mat)
{
    if (!pid_)
        return;
    int location = glGetUniformLocation(pid_, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    }
    glUniformMatrix3fv(location, 1, false, mat.data());
}

void Shader::set_uniform(const char* name, const mat4& mat)
{
    if (!pid_)
        return;
    int location = glGetUniformLocation(pid_, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    }
    glUniformMatrix4fv(location, 1, false, mat.data());
}

} // namespace pmp
