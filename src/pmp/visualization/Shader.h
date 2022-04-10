// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <vector>

#include "pmp/visualization/GL.h"
#include "pmp/MatVec.h"

namespace pmp {

//! shader class for easy handling of the shader
//! \ingroup visualization
class Shader
{
public:
    //! default constructor
    Shader();

    //! default destructor
    ~Shader();

    //! is shader valid (ID != 0)
    bool is_valid() const { return pid_ != 0; }

    //! get source from strings, compile, and link vertex and fragment shader,
    //! \param vshader string with the adress to the vertex shader
    //! \param fshader string with the adress to the fragment shader
    bool source(const char* vshader, const char* fshader);

    //! load (from file), compile, and link vertex and fragment shader,
    //! and optional geometry and tessellation shaders.
    //! unused shaders should be NULL.
    //! \param vfile string with the adress to the vertex shader
    //! \param ffile string with the adress to the fragment shader
    //! \param gfile filename of geometry shader
    //! \param tcfile filename of tessellation control shader
    //! \param tefile filename of tessellation evaluation shader
    bool load(const char* vfile, const char* ffile, const char* gfile = nullptr,
              const char* tcfile = nullptr, const char* tefile = nullptr);

    //! enable/bind this shader program
    void use();

    //! disable/unbind this shader program
    void disable();

    //! bind attribute to location
    void bind_attribute(const char* name, GLuint index);

    //! upload float uniform
    //! \param name string of the uniform name
    //! \param value the value for the uniform
    void set_uniform(const char* name, float value);

    //! upload int uniform
    //! \param name string of the uniform name
    //! \param value the value for the uniform
    void set_uniform(const char* name, int value);

    //! upload vec3 uniform
    //! \param name string of the uniform name
    //! \param vec the value for the uniform
    void set_uniform(const char* name, const vec3& vec);

    //! upload vec4 uniform
    //! \param name string of the uniform name
    //! \param vec the value for the uniform
    void set_uniform(const char* name, const vec4& vec);

    //! upload mat3 uniform
    //! \param name string of the uniform name
    //! \param mat the value for the uniform
    void set_uniform(const char* name, const mat3& mat);

    //! upload mat4 uniform
    //! \param name string of the uniform name
    //! \param mat the value for the uniform
    void set_uniform(const char* name, const mat4& mat);

private:
    // deletes all shader and frees GPU shader capacities
    void cleanup();

    // load shader from file, return as string
    bool load(const char* filename, std::string& source);

    // compile a vertex/fragment shader
    GLint compile(const char* source, GLenum type);

    // loads a vertex/fragmend shader from a file and compiles it
    GLint load_and_compile(const char* filename, GLenum type);

    // relink: use this after setting/changing attrib location
    bool link();

    // id of the linked shader program
    GLint pid_{0};

    // id of the vertex shader
    std::vector<GLint> shaders_;
};

} // namespace pmp
