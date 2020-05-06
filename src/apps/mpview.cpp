// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "MeshProcessingViewer.h"

int main(int argc, char **argv)
{
    MeshProcessingViewer window("MeshProcessingViewer", 800, 600);

    if (argc == 2)
        window.load_mesh(argv[1]);
#ifdef __EMSCRIPTEN__
    else
        window.load_mesh("input.off");
#endif

    return window.run();
}
