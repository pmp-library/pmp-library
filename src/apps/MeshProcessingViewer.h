// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <pmp/visualization/MeshViewer.h>
#include <pmp/algorithms/SurfaceSmoothing.h>

using namespace pmp;

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
    virtual void process_imgui() override;

private:
    // smoother has to remember cotan weights, hence it global member
    SurfaceSmoothing smoother_;
};
