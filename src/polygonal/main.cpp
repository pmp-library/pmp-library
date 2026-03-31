// Copyright 2011-2025 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "polygonal.h"

int main(int argc, char** argv)
{
    pmp::Polygonal window("Polygonal", 1200, 800);

#ifndef __EMSCRIPTEN__
    if (argc == 2)
        window.load_mesh(argv[1]);
    return window.run();
#else
    return window.run();
#endif
}
