// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include <pmp/viewers/mesh_viewer.h>

class MeshProcessingViewer : public pmp::MeshViewer
{
public:
    //! constructor
    MeshProcessingViewer(const char* title, int width, int height);

protected:
    //! this function handles mouse button presses
    void mouse(int button, int action, int mods) override;

    //! this function handles keyboard events
    void keyboard(int key, int code, int action, int mod) override;

    //! draw the scene in different draw modes
    void process_imgui() override;
};
