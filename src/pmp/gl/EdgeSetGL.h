//=============================================================================
#pragma once
//=============================================================================

#include <pmp/gl/gl.h>
#include <pmp/gl/Shader.h>
#include <pmp/Vector.h>
#include <pmp/Matrix.h>
#include <pmp/EdgeSet.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup gl gl
//! @{

//=============================================================================

//! Class for rendering an edge set using OpenGL
class EdgeSetGL : public EdgeSet
{
public:

    //! Constructor
    EdgeSetGL();

    //! default destructor
    ~EdgeSetGL();

    //! draw the edges
    void draw(const mat4& projectionMatrix,
              const mat4& modelviewMatrix,
              const std::string drawMode);

    //! update all opengl buffers for efficient core profile rendering
    void updateOpenGLBuffers();


private:

    //! OpenGL buffers
    GLuint vertex_array_object_;
    GLuint vertex_buffer_;
    GLuint edge_buffer_;

    //! buffer sizes
    GLsizei n_vertices_;
    GLsizei n_edges_;

    //! shaders
    Shader  m_phongShader;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace
//=============================================================================
