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

  //! OpenGL vertex array buffer
  GLuint vertex_array_object_;

  //! OpenGL buffer object for vertex positions
  GLuint vertex_buffer_;

  //! OpenGL buffer object for vertex normals
  GLuint normal_buffer_;

  //! OpenGL buffer object for triangle indices
  GLuint triangle_buffer_;

  //! OpenGL buffer object for edge indices
  GLuint edge_buffer_;

  //! number of vertices
  unsigned int n_vertices_;

  //! number of edges
  unsigned int n_edges_;

  //! number of triangles (might be tessellated polygons!)
  unsigned int n_triangles_;
};


//=============================================================================
} // namespace
//=============================================================================
