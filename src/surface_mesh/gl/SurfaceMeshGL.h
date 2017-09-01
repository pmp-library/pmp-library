//=============================================================================
#pragma once
//=============================================================================

#include <surface_mesh/gl/gl.h>
#include <surface_mesh/Vector.h>
#include <surface_mesh/Matrix.h>
#include <surface_mesh/SurfaceMesh.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================

class SurfaceMeshGL : public SurfaceMesh
{
public:

    //! Constructor
    SurfaceMeshGL();

    //! default destructor
    ~SurfaceMeshGL();

    // draw mesh elements
    void drawPoints();
    void drawEdges();
    void drawFaces();

    //! update all opengl buffers for efficient core profile rendering
    void updateOpenGLBuffers();


private:

    void crease_normals(std::vector<Point>& vertex_normals);


private:

    // material parameters
    vec3  front_color_;
    vec3  back_color_;
    vec3  wire_color_;
    vec4  material_;
    float crease_angle_;

    //! OpenGL buffers 
    GLuint vertex_array_object_;
    GLuint vertex_buffer_;
    GLuint normal_buffer_;
    GLuint edge_buffer_;

    //! buffer sizes
    GLsizei n_vertices_;
    GLsizei n_edges_;
    GLsizei n_triangles_;
};


//=============================================================================
} // namespace
//=============================================================================
