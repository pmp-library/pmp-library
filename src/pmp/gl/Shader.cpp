//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================

#include <pmp/gl/Shader.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

//=============================================================================

namespace pmp {

//=============================================================================

Shader::Shader() : m_pid(0), m_vid(0), m_fid(0)
{
}

//-----------------------------------------------------------------------------

Shader::~Shader()
{
    cleanup();
}

//-----------------------------------------------------------------------------

void Shader::cleanup()
{
    if (m_pid)
        glDeleteProgram(m_pid);
    if (m_vid)
        glDeleteShader(m_vid);
    if (m_fid)
        glDeleteShader(m_fid);

    m_pid = m_vid = m_fid = 0;
}

//-----------------------------------------------------------------------------

bool Shader::source(const char* vshader, const char* fshader)
{
    // cleanup existing shaders first
    cleanup();

    // create program
    m_pid = glCreateProgram();

    // vertex shader
    m_vid = compile(vshader, GL_VERTEX_SHADER);
    if (!m_vid)
    {
        std::cerr << "Cannot compile vertex shader!\n";
        return false;
    }
    glAttachShader(m_pid, m_vid);

    // fragment shader
    m_fid = compile(fshader, GL_FRAGMENT_SHADER);
    if (!m_fid)
    {
        std::cerr << "Cannot compile fragment shader!\n";
        return false;
    }
    glAttachShader(m_pid, m_fid);

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
    m_pid = glCreateProgram();

    // vertex shader
    m_vid = loadAndCompile(vfile, GL_VERTEX_SHADER);
    if (!m_vid)
    {
        std::cerr << "Cannot compile vertex shader!\n";
        return false;
    }
    glAttachShader(m_pid, m_vid);

    // fragment shader
    m_fid = loadAndCompile(ffile, GL_FRAGMENT_SHADER);
    if (!m_fid)
    {
        std::cerr << "Cannot compile fragment shader!\n";
        return false;
    }
    glAttachShader(m_pid, m_fid);

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
    glLinkProgram(m_pid);
    GLint status;
    glGetProgramiv(m_pid, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(m_pid, GL_INFO_LOG_LENGTH, &length);

        auto* info = new GLchar[length + 1];
        glGetProgramInfoLog(m_pid, length, nullptr, info);
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

GLint Shader::loadAndCompile(const char* filename, GLenum type)
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
    if (m_pid)
        glUseProgram(m_pid);
}

//-----------------------------------------------------------------------------

void Shader::disable()
{
    glUseProgram(0);
}

//-----------------------------------------------------------------------------

void Shader::bindAttribute(const char* name, GLuint index)
{
    if (!m_pid)
        return;
    glBindAttribLocation(m_pid, index, name);
    link(); // have to re-link now!
}

//-----------------------------------------------------------------------------

void Shader::setUniform(const char* name, float value)
{
    if (!m_pid)
        return;
    int location = glGetUniformLocation(m_pid, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    }
    glUniform1f(location, value);
}

//-----------------------------------------------------------------------------

void Shader::setUniform(const char* name, int value)
{
    if (!m_pid)
        return;
    int location = glGetUniformLocation(m_pid, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    }
    glUniform1i(location, value);
}

//-----------------------------------------------------------------------------

void Shader::setUniform(const char* name, const vec3& vec)
{
    if (!m_pid)
        return;
    int location = glGetUniformLocation(m_pid, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    };
    glUniform3f(location, vec[0], vec[1], vec[2]);
}

//-----------------------------------------------------------------------------

void Shader::setUniform(const char* name, const vec4& vec)
{
    if (!m_pid)
        return;
    int location = glGetUniformLocation(m_pid, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    }
    glUniform4f(location, vec[0], vec[1], vec[2], vec[3]);
}

//-----------------------------------------------------------------------------

void Shader::setUniform(const char* name, const mat3& mat)
{
    if (!m_pid)
        return;
    int location = glGetUniformLocation(m_pid, name);
    if (location == -1)
    {
        std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    }
    glUniformMatrix3fv(location, 1, false, mat.data());
}

//-----------------------------------------------------------------------------

void Shader::setUniform(const char* name, const mat4& mat)
{
    if (!m_pid)
        return;
    int location = glGetUniformLocation(m_pid, name);
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
