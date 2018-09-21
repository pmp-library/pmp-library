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

#include <pmp/gl/SurfaceMeshGL.h>
#include <pmp/gl/TrackballViewer.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup gl gl
//! @{

//=============================================================================

//! Simple viewer for a SurfaceMesh
class MeshViewer : public TrackballViewer
{
public:
    //! constructor
    MeshViewer(const char* title, int width, int height, bool showgui = true);

    //! destructor
    virtual ~MeshViewer();

    //! load a mesh from file \c filename
    virtual bool loadMesh(const char* filename);

    //! load a texture from file \c filename
    bool loadTexture(const char* filename, GLint format = GL_RGB,
                     GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                     GLint magFilter = GL_LINEAR,
                     GLint wrap = GL_CLAMP_TO_EDGE);

    //! update mesh normals and all buffers for OpenGL rendering.  call this
    //! function whenever you change either the vertex positions or the
    //! triangulation of the mesh
    virtual void updateMesh();

    //! draw the scene in different draw modes
    virtual void draw(const std::string& _draw_mode) override;

    //! handle ImGUI interface
    virtual void processImGUI() override;

    //! this function handles keyboard events
    virtual void keyboard(int key, int code, int action, int mod) override;

protected:
    SurfaceMeshGL m_mesh;   //!< the mesh
    std::string m_filename; //!< the current file
    float m_creaseAngle;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
