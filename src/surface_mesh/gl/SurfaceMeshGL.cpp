//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include <surface_mesh/gl/SurfaceMeshGL.h>
#include <surface_mesh/gl/phong_shader.h>
#include <float.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================


SurfaceMeshGL::SurfaceMeshGL()
{
    // initialize GL buffers to zero
    vertex_array_object_    = 0;
    vertex_buffer_          = 0;
    normal_buffer_          = 0;
    edge_buffer_            = 0;


    // initialize buffer sizes
    n_vertices_  = 0;
    n_edges_     = 0;
    n_triangles_ = 0;


    // material parameters
    front_color_  = vec3(0.4, 0.425, 0.475);
    back_color_   = vec3(0.5, 0.3, 0.3);
    wire_color_   = vec3(0,0,0);
    material_     = vec4(0.1, 1.0, 1.0, 100.0);
    crease_angle_ = 0.0;
}


//-----------------------------------------------------------------------------


SurfaceMeshGL::~SurfaceMeshGL()
{
    // delete OpenGL buffers
    glDeleteBuffers(1, &vertex_buffer_);
    glDeleteBuffers(1, &normal_buffer_);
    glDeleteBuffers(1, &edge_buffer_);
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
    }


    // activate VAO
    glBindVertexArray(vertex_array_object_);



    // produce arrays of points and normals (duplicate vertices to allow for flat shading)
    std::vector<vec3>  positionArray; positionArray.reserve(3*nFaces());
    std::vector<vec3>  normalArray; normalArray.reserve(3*nFaces());

    auto vertex_indices = addVertexProperty<size_t>("v:index");

    size_t i(0);
    for (auto f : faces())
    {
        SurfaceMesh::VertexAroundFaceCirculator fvit, fvend;
        SurfaceMesh::Vertex v0, v1, v2;
        Point p0, p1, p2, n0, n1, n2;

        fvit = fvend = vertices(f);
        v0 = *fvit; ++fvit;
        v2 = *fvit; ++fvit;
        do
        {
            v1 = v2;
            v2 = *fvit;

            p0 = position(v0);
            p1 = position(v1);
            p2 = position(v2);

            positionArray.push_back(p0);
            positionArray.push_back(p1);
            positionArray.push_back(p2);

            if (crease_angle_ < 0.1) 
            {
                n0 = n1 = n2 = computeFaceNormal(f);
            }
            else
            {
                n0 = computeVertexNormal(v0);
                n1 = computeVertexNormal(v1);
                n2 = computeVertexNormal(v2);
            }

            normalArray.push_back(n0);
            normalArray.push_back(n1);
            normalArray.push_back(n2);

            vertex_indices[v0] = i++;
            vertex_indices[v1] = i++;
            vertex_indices[v2] = i++;
        }
        while (++fvit != fvend);
    }



    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, positionArray.size()*3*sizeof(float), positionArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    n_vertices_ = positionArray.size();


    // normals
    //crease_normals(normals);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(GL_ARRAY_BUFFER, normalArray.size()*3*sizeof(float), normalArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);


    // edge indices
    std::vector<unsigned int> edgeArray;  edgeArray.reserve(nEdges());
    for (auto e : edges())
    {
        edgeArray.push_back(vertex_indices[vertex(e, 0)]);
        edgeArray.push_back(vertex_indices[vertex(e, 1)]);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeArray.size()*sizeof(unsigned int), edgeArray.data(), GL_STATIC_DRAW);
    n_edges_ = edgeArray.size();


    // unbind vertex arry
    glBindVertexArray(0); 


    // remove vertex index property again
    removeVertexProperty(vertex_indices);
}


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
    
    if (!n_vertices_) return;

    // set point size
#ifndef __EMSCRIPTEN__
    glPointSize(5.0);
#endif

    // draw vertices of mesh
    glBindVertexArray(vertex_array_object_);
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

    if (!n_edges_) return;

    // draw edges of mesh
    glBindVertexArray(vertex_array_object_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_buffer_);
    glDrawElements(GL_LINES, 2*n_edges_, GL_UNSIGNED_INT, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

    if (!n_vertices_) return;

    // draw triangles (might be tessellated polygons)
    glBindVertexArray(vertex_array_object_);
    glDrawArrays(GL_TRIANGLES, 0, n_vertices_);
    glBindVertexArray(0);
}


//=============================================================================
} // namespace
//=============================================================================
