//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include <surface_mesh/gl/SurfaceMeshGL.h>
#include <float.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================


SurfaceMeshGL::SurfaceMeshGL()
{
    n_triangles_ = n_edges_ = n_vertices_ = 0;


  // initialize GL buffers to zero
  vertex_array_object_    = 0;
  vertex_buffer_          = 0;
  normal_buffer_          = 0;
  edge_buffer_            = 0;
  triangle_buffer_        = 0;
}


//-----------------------------------------------------------------------------


SurfaceMeshGL::~SurfaceMeshGL()
{
  // delete OpenGL buffers
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &normal_buffer_);
  glDeleteBuffers(1, &edge_buffer_);
  glDeleteBuffers(1, &triangle_buffer_);
  glDeleteVertexArrays(1, &vertex_array_object_);
}


//-----------------------------------------------------------------------------


void SurfaceMeshGL::updateOpenGLBuffers()
{
  // are buffers already initialized?
  if (!vertex_array_object_)
    {
      glGenVertexArrays(1, &vertex_array_object_);
      glBindVertexArray(vertex_array_object_);
      glGenBuffers(1, &vertex_buffer_);
      glGenBuffers(1, &normal_buffer_);
      glGenBuffers(1, &edge_buffer_);
      glGenBuffers(1, &triangle_buffer_);
    }


  // activate VAO
  glBindVertexArray(vertex_array_object_);


  // vertices
  SurfaceMesh::VertexProperty<Point> points = vertexProperty<Point>("v:point");
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, 3*nVertices()*sizeof(float), points.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  n_vertices_ = nVertices();


  // normals
  SurfaceMesh::VertexProperty<Point> normals = vertexProperty<Point>("v:normal");
  updateVertexNormals();
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, 3*nVertices()*sizeof(float), normals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);


  // triangle indices
  std::vector<GLuint> triangle_indices;
  for (Face f : faces())
    {
      // every polygonal face has to be split into triangles
      VertexAroundFaceCirculator fvit, fvend;
      Vertex v0, v1, v2;
      fvit = fvend = vertices(f);
      v0 = *fvit; ++fvit;
      v2 = *fvit; ++fvit;
      do
        {
          v1 = v2;
          v2 = *fvit;
          triangle_indices.push_back(v0.idx());
          triangle_indices.push_back(v1.idx());
          triangle_indices.push_back(v2.idx());
        }
      while (++fvit != fvend);
    }
  n_triangles_ = triangle_indices.size()/3;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*n_triangles_*sizeof(GLuint), &triangle_indices[0], GL_STATIC_DRAW);


  // edge indices
  std::vector<GLuint> edge_indices;
  for (Edge e : edges())
    {
      Vertex v0 = vertex(e, 0);
      Vertex v1 = vertex(e, 1);

      edge_indices.push_back(v0.idx());
      edge_indices.push_back(v1.idx());
    }
  n_edges_ = edge_indices.size()/2;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*n_edges_*sizeof(GLuint), &edge_indices[0], GL_STATIC_DRAW);


  glBindVertexArray(0); }


//-----------------------------------------------------------------------------


void
SurfaceMeshGL::
drawPoints()
{
    // did we generate buffers already?
    if (!vertex_array_object_)
    {
        updateOpenGLBuffers();
    }

    // set point size
#ifndef __EMSCRIPTEN__
    glPointSize(5.0);
#endif

    // draw vertices of mesh
    glBindVertexArray(vertex_array_object_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDrawArrays(GL_POINTS, 0, n_vertices_);

    glBindVertexArray(0);
}


//-----------------------------------------------------------------------------


void
SurfaceMeshGL::
drawEdges()
{
    // did we generate buffers already?
    if (!vertex_array_object_)
    {
        updateOpenGLBuffers();
    }

    // draw edges of mesh
    glBindVertexArray(vertex_array_object_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_buffer_);
    glDrawElements(GL_LINES, 2*n_edges_, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}


//-----------------------------------------------------------------------------


void
SurfaceMeshGL::
drawFaces()
{
    // did we generate buffers already?
    if (!vertex_array_object_)
    {
        updateOpenGLBuffers();
    }

    // draw triangles (might be tessellated quads)
    glBindVertexArray(vertex_array_object_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle_buffer_);
    glDrawElements(GL_TRIANGLES, 3*n_triangles_, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}


//=============================================================================
} // namespace
//=============================================================================
