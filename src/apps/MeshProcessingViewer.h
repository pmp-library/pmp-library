//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/visualization/MeshViewer.h>

//=============================================================================

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
};

//=============================================================================
