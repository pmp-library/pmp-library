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
#pragma once
//=============================================================================

#include <pmp/gl/gl.h>
#include <pmp/MatVec.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup gl
//! @{

//=============================================================================

//! shader class for easy handling of the shader
class Shader
{
public:
    //! default constructor
    Shader();

    //! default destructor
    ~Shader();

    //! is shader valid (ID != 0)
    bool isValid() const { return m_pid != 0; }

    //! get source from strings, compile, and link vertex and fragment shader,
    //! \param vshader string with the adress to the vertex shader
    //! \param fshader string with the adress to the fragment shader
    bool source(const char* vshader, const char* fshader);

    //! load (from file), compile, and link vertex and fragment shader,
    //! \param vfile string with the adress to the vertex shader
    //! \param ffile string with the adress to the fragment shader
    bool load(const char* vfile, const char* ffile);

    //! enable/bind this shader program
    void use();

    //! disable/unbind this shader program
    void disable();

    //! bind attribute to location
    void bindAttribute(const char* name, GLuint index);

    //! upload float uniform
    //! \param name string of the uniform name
    //! \param value the value for the uniform
    void setUniform(const char* name, float value);

    //! upload int uniform
    //! \param name string of the uniform name
    //! \param value the value for the uniform
    void setUniform(const char* name, int value);

    //! upload vec3 uniform
    //! \param name string of the uniform name
    //! \param vec the value for the uniform
    void setUniform(const char* name, const vec3& vec);

    //! upload vec4 uniform
    //! \param name string of the uniform name
    //! \param vec the value for the uniform
    void setUniform(const char* name, const vec4& vec);

    //! upload mat3 uniform
    //! \param name string of the uniform name
    //! \param mat the value for the uniform

    void setUniform(const char* name, const mat3& mat);
    //! upload mat4 uniform
    //! \param name string of the uniform name
    //! \param mat the value for the uniform
    void setUniform(const char* name, const mat4& mat);

private:
    //! deletes all shader and frees GPU shader capacities
    void cleanup();

    //! load shader from file, return as string
    bool load(const char* filename, std::string& source);

    //! compile a vertex/fragmend shader
    //! \param shader source
    //! \param type the type of the shader (vertex, fragment)
    GLint compile(const char* source, GLenum type);

    //! loads a vertex/fragmend shader from a file and compiles it
    //! \param filename the location and name of the shader
    //! \param type the type of the shader (vertex, geometry, fragment)
    GLint loadAndCompile(const char* filename, GLenum type);

    //! relink: use this after setting/changing attrib location
    bool link();

private:
    //! id of the linked shader program
    GLint m_pid;

    //! id of the vertex shader
    GLint m_vid;

    //! id of the fragmend shader
    GLint m_fid;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
