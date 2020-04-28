//=============================================================================
// Copyright (C) 2011-2020 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================

#include <pmp/visualization/MeshViewer.h>

using namespace pmp;

//=============================================================================

void usage_and_exit()
{
    std::cerr << "Usage:\nmview [-g] [-t texture] <input>\n\n"
              << "Options\n"
              << " -g:  show GUI controls (toggle with 'g')\n"
              << " -t:  specify texture image (mesh has to provide texture "
                 "coordinates)\n"
              << " -m:  specify matcap image\n"
              << "\n";
    exit(1);
}

//----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    char* input = nullptr;
    char* texture = nullptr;
    char* matcap = nullptr;
    bool gui = false;

    // parse command line parameters
    for (int i = 0; i < argc; ++i)
    {
        if (std::string(argv[i]) == std::string("-g"))
        {
            gui = true;
        }
        else if (std::string(argv[i]) == std::string("-t"))
        {
            if (i + 1 < argc)
            {
                texture = argv[i + 1];
                ++i;
            }
        }
        else if (std::string(argv[i]) == std::string("-m"))
        {
            if (i + 1 < argc)
            {
                matcap = argv[i + 1];
                ++i;
            }
        }
        else if (std::string(argv[i]) == std::string("-h"))
        {
            usage_and_exit();
        }
        else
        {
            input = argv[i];
        }
    }

    // need a mesh!
    if (!input)
        usage_and_exit();

    // open window, start application
    MeshViewer viewer("MeshViewer", 800, 600, gui);
    viewer.load_mesh(input);
    if (texture)
    {
        viewer.load_texture(texture, GL_SRGB8);
    }
    else if (matcap)
    {
        viewer.load_matcap(matcap);
    }

    return viewer.run();
}

//=============================================================================
