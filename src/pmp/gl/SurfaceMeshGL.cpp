//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
// All rights reserved.
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

#include <pmp/gl/SurfaceMeshGL.h>
#include <pmp/gl/phong_shader.h>
#include <pmp/algorithms/SurfaceNormals.h>

#include "cold_warm_texture.h"
#include <stb_image.h>
#include <cfloat>

//=============================================================================

namespace pmp {

//=============================================================================

SurfaceMeshGL::SurfaceMeshGL()
{
    // initialize GL buffers to zero
    m_vertexArrayObject = 0;
    m_vertexBuffer = 0;
    m_normalBuffer = 0;
    m_texCoordBuffer = 0;
    m_edgeBuffer = 0;
    m_featureBuffer = 0;

    // initialize buffer sizes
    m_nVertices = 0;
    m_nEdges = 0;
    m_nTriangles = 0;
    m_nFeatures = 0;

    // material parameters
    m_frontColor = vec3(0.6, 0.6, 0.6);
    m_backColor = vec3(0.5, 0.0, 0.0);
    m_ambient = 0.1;
    m_diffuse = 0.8;
    m_specular = 0.6;
    m_shininess = 100.0;
    m_srgb = false;
    m_creaseAngle = 70.0;

    // initialize texture
    m_texture = 0;
    m_textureMode = OtherTexture;
}

//-----------------------------------------------------------------------------

SurfaceMeshGL::~SurfaceMeshGL()
{
    // delete OpenGL buffers
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteBuffers(1, &m_normalBuffer);
    glDeleteBuffers(1, &m_texCoordBuffer);
    glDeleteBuffers(1, &m_edgeBuffer);
    glDeleteBuffers(1, &m_featureBuffer);
    glDeleteVertexArrays(1, &m_vertexArrayObject);
    glDeleteTextures(1, &m_texture);
}

//-----------------------------------------------------------------------------

//void SurfaceMeshGL::useTexture(GLuint texID)
//{
//glDeleteTextures(1, &m_texture);
//m_texture     = texID;
//m_textureMode = OtherTexture;
//}

//-----------------------------------------------------------------------------

bool SurfaceMeshGL::loadTexture(const char* filename, GLint format,
                                GLint minFilter, GLint magFilter, GLint wrap)
{
    // load with stb_image
    int width, height, nComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* img =
        stbi_load(filename, &width, &height, &nComponents, 3); // enforce RGB
    if (!img)
        return false;

    // delete old texture
    glDeleteTextures(1, &m_texture);

    // setup new texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // upload texture data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, img);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    // compute mipmaps
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // use SRGB rendering?
    m_srgb = (format == GL_SRGB8);

    // free memory
    stbi_image_free(img);

    m_textureMode = OtherTexture;
    return true;
}

//-----------------------------------------------------------------------------

void SurfaceMeshGL::useColdWarmTexture()
{
    if (m_textureMode != ColdWarmTexture)
    {
        // delete old texture
        glDeleteTextures(1, &m_texture);

        // setup new texture
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 1, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, cold_warm_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_srgb = false;
        m_textureMode = ColdWarmTexture;
    }
}

//-----------------------------------------------------------------------------

void SurfaceMeshGL::useCheckerboardTexture()
{
    if (m_textureMode != CheckerboardTexture)
    {
        // delete old texture
        glDeleteTextures(1, &m_texture);

        // generate checkerboard-like image
        const unsigned int res = 512;
        auto* tex = new GLubyte[res * res * 3];
        GLubyte* tp = tex;
        for (unsigned int x = 0; x < res; ++x)
        {
            for (unsigned int y = 0; y < res; ++y)
            {
                if (((x & 0x20) == 0) ^ ((y & 0x20) == 0))
                {
                    *(tp++) = 42;
                    *(tp++) = 157;
                    *(tp++) = 223;
                }
                else
                {
                    *(tp++) = 255;
                    *(tp++) = 255;
                    *(tp++) = 255;
                }
            }
        }

        // generate texture
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, res, res, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // clean up
        delete[] tex;

        m_srgb = false;
        m_textureMode = CheckerboardTexture;
    }
}

//-----------------------------------------------------------------------------

void SurfaceMeshGL::setCreaseAngle(Scalar ca)
{
    if (ca != m_creaseAngle)
    {
        m_creaseAngle = std::max(Scalar(0), std::min(Scalar(180), ca));
        updateOpenGLBuffers();
    }
}

//-----------------------------------------------------------------------------

void SurfaceMeshGL::updateOpenGLBuffers()
{
    // are buffers already initialized?
    if (!m_vertexArrayObject)
    {
        glGenVertexArrays(1, &m_vertexArrayObject);
        glBindVertexArray(m_vertexArrayObject);
        glGenBuffers(1, &m_vertexBuffer);
        glGenBuffers(1, &m_normalBuffer);
        glGenBuffers(1, &m_texCoordBuffer);
        glGenBuffers(1, &m_edgeBuffer);
        glGenBuffers(1, &m_featureBuffer);
    }

    // activate VAO
    glBindVertexArray(m_vertexArrayObject);

    // get vertex properties
    auto vpos = getVertexProperty<Point>("v:point");
    auto vtex = getVertexProperty<TexCoord>("v:tex");
    auto htex = getHalfedgeProperty<TexCoord>("h:tex");

    // produce arrays of points, normals, and texcoords
    // (duplicate vertices to allow for flat shading)
    std::vector<vec3> positionArray;
    positionArray.reserve(3 * nFaces());
    std::vector<vec3> normalArray;
    normalArray.reserve(3 * nFaces());
    std::vector<vec2> texArray;
    texArray.reserve(3 * nFaces());

    // data per face (for all corners)
    std::vector<Halfedge> cornerHalfedges;
    std::vector<Vertex> cornerVertices;
    std::vector<vec3> cornerNormals;

    // convert from degrees to radians
    const Scalar creaseAngle = m_creaseAngle / 180.0 * M_PI;

    auto vertex_indices = addVertexProperty<size_t>("v:index");
    size_t vidx(0);

    // loop over all faces
    for (auto f : faces())
    {
        // collect corner positions and normals
        cornerHalfedges.clear();
        cornerVertices.clear();
        cornerNormals.clear();
        for (auto h : halfedges(f))
        {
            cornerHalfedges.push_back(h);
            cornerVertices.push_back(toVertex(h));
            cornerNormals.push_back(
                SurfaceNormals::computeCornerNormal(*this, h, creaseAngle));
        }
        assert(cornerVertices.size() >= 3);

        // tessellate face into triangles
        int i0, i1, i2, nc = cornerVertices.size();
        for (i0 = 0, i1 = 1, i2 = 2; i2 < nc; ++i1, ++i2)
        {
            positionArray.push_back(vpos[cornerVertices[i0]]);
            positionArray.push_back(vpos[cornerVertices[i1]]);
            positionArray.push_back(vpos[cornerVertices[i2]]);

            normalArray.push_back(cornerNormals[i0]);
            normalArray.push_back(cornerNormals[i1]);
            normalArray.push_back(cornerNormals[i2]);

            if (htex)
            {
                texArray.push_back(htex[cornerHalfedges[i0]]);
                texArray.push_back(htex[cornerHalfedges[i1]]);
                texArray.push_back(htex[cornerHalfedges[i2]]);
            }
            else if (vtex)
            {
                texArray.push_back(vtex[cornerVertices[i0]]);
                texArray.push_back(vtex[cornerVertices[i1]]);
                texArray.push_back(vtex[cornerVertices[i2]]);
            }

            vertex_indices[cornerVertices[i0]] = vidx++;
            vertex_indices[cornerVertices[i1]] = vidx++;
            vertex_indices[cornerVertices[i2]] = vidx++;
        }
    }

    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, positionArray.size() * 3 * sizeof(float),
                 positionArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    m_nVertices = positionArray.size();

    // normals
    glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normalArray.size() * 3 * sizeof(float),
                 normalArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // texture coordinates
    if (!texArray.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, texArray.size() * 2 * sizeof(float),
                     texArray.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
    }

    // edge indices
    std::vector<unsigned int> edgeArray;
    edgeArray.reserve(nEdges());
    for (auto e : edges())
    {
        edgeArray.push_back(vertex_indices[vertex(e, 0)]);
        edgeArray.push_back(vertex_indices[vertex(e, 1)]);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_edgeBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 edgeArray.size() * sizeof(unsigned int), edgeArray.data(),
                 GL_STATIC_DRAW);
    m_nEdges = edgeArray.size();

    // feature edges
    auto efeature = getEdgeProperty<bool>("e:feature");
    if (efeature)
    {
        std::vector<unsigned int> features;

        for (auto e : edges())
        {
            if (efeature[e])
            {
                features.push_back(vertex_indices[vertex(e, 0)]);
                features.push_back(vertex_indices[vertex(e, 1)]);
            }
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_featureBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     features.size() * sizeof(unsigned int), features.data(),
                     GL_STATIC_DRAW);
        m_nFeatures = features.size();
    }
    else
    {
        m_nFeatures = 0;
    }

    // unbind vertex arry
    glBindVertexArray(0);

    // remove vertex index property again
    removeVertexProperty(vertex_indices);
}

//-----------------------------------------------------------------------------

void SurfaceMeshGL::draw(const mat4& projectionMatrix,
                         const mat4& modelviewMatrix,
                         const std::string drawMode)
{
    // did we generate buffers already?
    if (!m_vertexArrayObject)
    {
        updateOpenGLBuffers();
    }

    // load shader?
    if (!m_phongShader.isValid())
    {
        if (!m_phongShader.source(phong_vshader, phong_fshader))
            exit(1);
    }

    // we need some texture, otherwise WebGL complains
    if (!m_texture)
    {
        useColdWarmTexture();
    }

    // empty mesh?
    if (isEmpty())
        return;

    // setup matrices
    mat4 mv_matrix = modelviewMatrix;
    mat4 mvp_matrix = projectionMatrix * modelviewMatrix;
    mat3 n_matrix = inverse(transpose(linearPart(mv_matrix)));

    // setup shader
    m_phongShader.use();
    m_phongShader.setUniform("modelview_projection_matrix", mvp_matrix);
    m_phongShader.setUniform("modelview_matrix", mv_matrix);
    m_phongShader.setUniform("normal_matrix", n_matrix);
    m_phongShader.setUniform("point_size", 5.0f);
    m_phongShader.setUniform("light1", vec3(1.0, 1.0, 1.0));
    m_phongShader.setUniform("light2", vec3(-1.0, 1.0, 1.0));
    m_phongShader.setUniform("front_color", m_frontColor);
    m_phongShader.setUniform("back_color", m_backColor);
    m_phongShader.setUniform("ambient", m_ambient);
    m_phongShader.setUniform("diffuse", m_diffuse);
    m_phongShader.setUniform("specular", m_specular);
    m_phongShader.setUniform("shininess", m_shininess);
    m_phongShader.setUniform("use_lighting", true);
    m_phongShader.setUniform("use_texture", false);
    m_phongShader.setUniform("use_srgb", false);
    m_phongShader.setUniform("show_texture_layout", false);

    glBindVertexArray(m_vertexArrayObject);

    if (drawMode == "Points")
    {
        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, m_nVertices);
    }

    else if (drawMode == "Hidden Line")
    {
        // draw faces
        glDepthRange(0.01, 1.0);
        glDrawArrays(GL_TRIANGLES, 0, m_nVertices);

        // overlay edges
        glDepthRange(0.0, 1.0);
        glDepthFunc(GL_LEQUAL);
        m_phongShader.setUniform("front_color", vec3(0.1, 0.1, 0.1));
        m_phongShader.setUniform("back_color", vec3(0.1, 0.1, 0.1));
        m_phongShader.setUniform("use_lighting", false);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_edgeBuffer);
        glDrawElements(GL_LINES, m_nEdges, GL_UNSIGNED_INT, nullptr);
        glDepthFunc(GL_LESS);
    }

    else if (drawMode == "Smooth Shading")
    {
        glDrawArrays(GL_TRIANGLES, 0, m_nVertices);
    }

    else if (drawMode == "Texture")
    {
        m_phongShader.setUniform("front_color", vec3(0.9, 0.9, 0.9));
        m_phongShader.setUniform("back_color", vec3(0.3, 0.3, 0.3));
        m_phongShader.setUniform("use_texture", true);
        m_phongShader.setUniform("use_srgb", m_srgb);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glDrawArrays(GL_TRIANGLES, 0, m_nVertices);
    }

    else if (drawMode == "Texture Layout")
    {
        m_phongShader.setUniform("show_texture_layout", true);
        m_phongShader.setUniform("use_lighting", false);

        // draw faces
        m_phongShader.setUniform("front_color", vec3(0.8, 0.8, 0.8));
        m_phongShader.setUniform("back_color", vec3(0.9, 0.0, 0.0));
        glDepthRange(0.01, 1.0);
        glDrawArrays(GL_TRIANGLES, 0, m_nVertices);

        // overlay edges
        glDepthRange(0.0, 1.0);
        glDepthFunc(GL_LEQUAL);
        m_phongShader.setUniform("front_color", vec3(0.1, 0.1, 0.1));
        m_phongShader.setUniform("back_color", vec3(0.1, 0.1, 0.1));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_edgeBuffer);
        glDrawElements(GL_LINES, m_nEdges, GL_UNSIGNED_INT, nullptr);
        glDepthFunc(GL_LESS);
    }

    // draw feature edges
    if (m_nFeatures)
    {
        m_phongShader.setUniform("front_color", vec3(0, 1, 0));
        m_phongShader.setUniform("back_color", vec3(0, 1, 0));
        m_phongShader.setUniform("use_lighting", false);
        glDepthRange(0.0, 1.0);
        glDepthFunc(GL_LEQUAL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_featureBuffer);
        glDrawElements(GL_LINES, m_nFeatures, GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDepthFunc(GL_LESS);
    }

    glBindVertexArray(0);
    glCheckError();
}

//=============================================================================
} // namespace
//=============================================================================
