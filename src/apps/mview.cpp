//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================

#include <pmp/visualization/MeshViewer.h>
#include <unistd.h>

using namespace pmp;

//=============================================================================

void usage_and_exit()
{
    std::cerr << "Usage:\nmview [-g] [-t texture] <input>\n\nOptions\n"
              << " -g:  show GUI controls (toggle with 'g')\n"
              << " -t:  specify texture image (mesh has to provide texture coordinates)\n"
              << "\n";
    exit(1);
}

//----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    char* input   = nullptr;
    char* texture = nullptr;
    bool gui      = false;

    // parse command line parameters
    int c;
    while ((c = getopt(argc, argv, "gt:")) != -1)
    {
        switch (c)
        {
            case 'g':
                gui = true;
                break;

            case 't':
                texture = optarg;
                break;

            default:
                usage_and_exit();
        }
    }
   
    // get input mesh filename
    if (optind < argc)
        input = argv[optind];
   
    // need a mesh!
    if (!input) usage_and_exit();

    // open window, start application
    MeshViewer viewer("MeshViewer", 800, 600, gui);
    viewer.load_mesh(input);
    if (texture)
        viewer.load_texture(texture, GL_SRGB8);
    return viewer.run();
}

//=============================================================================
