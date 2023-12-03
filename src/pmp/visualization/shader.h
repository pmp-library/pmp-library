// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <vector>

#include "pmp/visualization/gl.h"
#include "pmp/mat_vec.h"

namespace pmp {

//! Class for handling shaders.
//! \ingroup visualization
class Shader
{
public:
    //! Default destructor, clears all shaders.
    ~Shader();

    //! Indicate if shader is valid.
    bool is_valid() const { return pid_ != 0; }

    //! Get source from strings, compile, and link vertex and fragment shader.
    //! \param vshader string with the address to the vertex shader
    //! \param fshader string with the address to the fragment shader
    void source(const char* vshader, const char* fshader);

    //! Load from file, compile, and link vertex and fragment shader,
    //! and optional geometry and tessellation shaders.
    //! Unused shaders should be nullptr.
    //! \param vfile string with the address to the vertex shader
    //! \param ffile string with the address to the fragment shader
    //! \param gfile filename of geometry shader
    //! \param tcfile filename of tessellation control shader
    //! \param tefile filename of tessellation evaluation shader
    void load(const char* vfile, const char* ffile, const char* gfile = nullptr,
              const char* tcfile = nullptr, const char* tefile = nullptr);

    //! Enable/bind this shader program.
    void use();

    //! Disable/unbind this shader program.
    void disable();

    //! Bind attribute to location.
    void bind_attribute(const char* name, GLuint index);

    //! Upload float uniform.
    //! \param name string of the uniform name
    //! \param value the value for the uniform
    void set_uniform(const char* name, float value);

    //! Upload int uniform.
    //! \param name string of the uniform name
    //! \param value the value for the uniform
    void set_uniform(const char* name, int value);

    //! Upload vec3 uniform.
    //! \param name string of the uniform name
    //! \param vec the value for the uniform
    void set_uniform(const char* name, const vec3& vec);

    //! Upload vec4 uniform.
    //! \param name string of the uniform name
    //! \param vec the value for the uniform
    void set_uniform(const char* name, const vec4& vec);

    //! Upload mat3 uniform.
    //! \param name string of the uniform name
    //! \param mat the value for the uniform
    void set_uniform(const char* name, const mat3& mat);

    //! Upload mat4 uniform.
    //! \param name string of the uniform name
    //! \param mat the value for the uniform
    void set_uniform(const char* name, const mat4& mat);

private:
    // deletes all shader and frees GPU shader capacities
    void cleanup();

    // load shader from file, return as string
    void load(const char* filename, std::string& source);

    // compile a vertex/fragment shader
    GLint compile(const char* source, GLenum type);

    // loads a vertex/fragment shader from a file and compiles it
    GLint load_and_compile(const char* filename, GLenum type);

    // relink: use this after setting/changing attrib location
    void link();

    // id of the linked shader program
    GLint pid_{0};

    // id of the vertex shader
    std::vector<GLint> shaders_;
};

} // namespace pmp
