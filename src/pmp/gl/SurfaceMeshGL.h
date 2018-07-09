//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
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
#pragma once
//=============================================================================

#include <pmp/gl/gl.h>
#include <pmp/gl/Shader.h>
#include <pmp/MatVec.h>
#include <pmp/SurfaceMesh.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup gl gl
//! @{

//=============================================================================

//! Class for rendering surface meshes using OpenGL
class SurfaceMeshGL : public SurfaceMesh
{
public:
    //! Constructor
    SurfaceMeshGL();

    //! default destructor
    ~SurfaceMeshGL();

    //! get front color
    const vec3& frontColor() const { return m_frontColor; }
    //! set front color
    void setFrontColor(const vec3& color) { m_frontColor = color; }

    //! get back color
    const vec3& backColor() const { return m_backColor; }
    //! set back color
    void setBackColor(const vec3& color) { m_backColor = color; }

    //! get ambient reflection coefficient
    float ambient() const { return m_ambient; }
    //! set ambient reflection coefficient
    void setAmbient(float a) { m_ambient = a; }

    //! get diffuse reflection coefficient
    float diffuse() const { return m_diffuse; }
    //! set diffuse reflection coefficient
    void setDiffuse(float d) { m_diffuse = d; }

    //! get specular reflection coefficient
    float specular() const { return m_specular; }
    //! set specular reflection coefficient
    void setSpecular(float s) { m_specular = s; }

    //! get specular shininess coefficient
    float shininess() const { return m_shininess; }
    //! set specular shininess coefficient
    void setShininess(float s) { m_shininess = s; }

    //! get crease angle (in degrees) for visualization of sharp edges
    Scalar creaseAngle() const { return m_creaseAngle; }
    //! set crease angle (in degrees) for visualization of sharp edges
    void setCreaseAngle(Scalar ca);

    //! draw the mesh
    void draw(const mat4& projectionMatrix, const mat4& modelviewMatrix,
              const std::string drawMode);

    //! update all opengl buffers for efficient core profile rendering
    void updateOpenGLBuffers();

    //! use color map to visualize scalar fields
    void useColdWarmTexture();

    //! setup checkerboard texture
    void useCheckerboardTexture();

    //! load texture from file
    //! \param filename the location and name of the texture
    //! \param format internal format (GL_RGB, GL_RGBA, GL_SRGB8, etc.)
    //! \param minFilter interpolation filter for minification
    //! \param magFilter interpolation filter for magnification
    //! \param wrap texture coordinates wrap preference
    bool loadTexture(const char* filename,
                     GLint format=GL_RGB,
                     GLint minFilter=GL_LINEAR_MIPMAP_LINEAR,
                     GLint magFilter=GL_LINEAR,
                     GLint wrap=GL_CLAMP_TO_EDGE);

private:

    //! OpenGL buffers
    GLuint m_vertexArrayObject;
    GLuint m_vertexBuffer;
    GLuint m_normalBuffer;
    GLuint m_texCoordBuffer;
    GLuint m_edgeBuffer;
    GLuint m_featureBuffer;

    //! buffer sizes
    GLsizei m_nVertices;
    GLsizei m_nEdges;
    GLsizei m_nTriangles;
    GLsizei m_nFeatures;

    //! shaders
    Shader m_phongShader;

    //! material properties
    vec3  m_frontColor, m_backColor;
    float m_ambient, m_diffuse, m_specular, m_shininess;
    bool  m_srgb;
    float m_creaseAngle;

    //! 1D texture for scalar field rendering
    GLuint m_texture;
    enum
    {
        ColdWarmTexture,
        CheckerboardTexture,
        OtherTexture
    } m_textureMode;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace
//=============================================================================
