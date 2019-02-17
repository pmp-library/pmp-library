//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include <pmp/visualization/Shader.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

//=============================================================================

namespace pmp {

//=============================================================================

Shader::Shader() : pid_(0), vid_(0), fid_(0) {}

//-----------------------------------------------------------------------------

Shader::~Shader()
{
    cleanup();
}

//-----------------------------------------------------------------------------

void Shader::cleanup()
{
    if (pid_)
        glDeleteProgram(pid_);
    if (vid_)
        glDeleteShader(vid_);
    if (fid_)
        glDeleteShader(fid_);

    pid_ = vid_ = fid_ = 0;
}

//-----------------------------------------------------------------------------

bool Shader::source(const char* vshader, const char* fshader)
{
    // cleanup existing shaders first
    cleanup();

    // create program
    pid_ = glCreateProgram();

    // vertex shader
    vid_ = compile(vshader, GL_VERTEX_SHADER);
    if (!vid_)
    {
        std::cerr << "Cannot compile vertex shader!\n";
        return false;
    }
    glAttachShader(pid_, vid_);

    // fragment shader
    fid_ = compile(fshader, GL_FRAGMENT_SHADER);
    if (!fid_)
    {
        std::cerr << "Cannot compile fragment shader!\n";
        return false;
    }
    glAttachShader(pid_, fid_);

    // link program
    if (!link())
    {
        std::cerr << "Cannot link program!\n";
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool Shader::load(const char* vfile, const char* ffile)
{
    // cleanup existing shaders first
    cleanup();

    // create program
    pid_ = glCreateProgram();

    // vertex shader
    vid_ = load_and_compile(vfile, GL_VERTEX_SHADER);
    if (!vid_)
    {
        std::cerr << "Cannot compile vertex shader!\n";
        return false;
    }
    glAttachShader(pid_, vid_);

    // fragment shader
    fid_ = load_and_compile(ffile, GL_FRAGMENT_SHADER);
    if (!fid_)
    {
        std::cerr << "Cannot compile fragment shader!\n";
        return false;
    }
    glAttachShader(pid_, fid_);

    // link program
    if (!link())
    {
        std::cerr << "Cannot link program!\n";
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool Shader::link()
{
    glLinkProgram(pid_);
    GLint status;
    glGetProgramiv(pid_, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(pid_, GL_INFO_LOG_LENGTH, &length);

        auto* info = new GLchar[length + 1];
        glGetProgramInfoLog(pid_, length, nullptr, info);
        std::cerr << "Shader: Cannot link program:\n" << info << std::endl;
        delete[] info;

        cleanup();

        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool Shader::load(const char* filename, std::string& source)
{
    std::ifstream ifs(filename);
    if (!ifs)
    {
        std::cerr << "Shader: Cannot open file \"" << filename << "\"\n";
        return false;
    }

    std::stringstream ss;
    ss << ifs.rdbuf();
    source = ss.str();

    ifs.close();
    return true;
}

//-----------------------------------------------------------------------------

GLint Shader::compile(const char* source, GLenum type)
{
    // create shader
    GLint id = glCreateShader(type);
    if (!id)
    {
        std::cerr << "Shader: Cannot create shader object\n";
        return 0;
    }

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

        auto* info = new GLchar[length + 1];
        glGetShaderInfoLog(id, length, nullptr, info);
        std::cerr << "Shader: Cannot compile shader\n" << info << std::endl;
        delete[] info;

        glDeleteShader(id);

        return 0;
    }

    return id;
}

//-----------------------------------------------------------------------------

GLint Shader::load_and_compile(const char* filename, GLenum type)
{
    std::string source;
    if (!load(filename, source))
    {
        std::cerr << "Shader: Cannot open file \"" << filename << "\"\n";
        return 0;
    }

    return compile(source.c_str(), type);
}

//-----------------------------------------------------------------------------

void Shader::use()
{
    if (pid_)
        glUseProgram(pid_);
}

//-----------------------------------------------------------------------------

void Shader::disable()
{
    glUseProgram(0);
}

//-----------------------------------------------------------------------------

void Shader::bind_attribute(const char* name, GLuint index)
{
    if (!pid_)
        return;
    glBindAttribLocation(pid_, index, name);
    link(); // have to re-link now!
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//=============================================================================
} // namespace pmp
//=============================================================================
