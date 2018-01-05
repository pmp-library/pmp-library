//=============================================================================

#include <pmp/gl/MeshViewer.h>

//=============================================================================

int main(int argc, char **argv)
{
#ifndef __EMSCRIPTEN__
    pmp::MeshViewer window("MeshViewer", 800, 600);
    if (argc == 2)
        window.loadMesh(argv[1]);
    return window.run();
#else
    pmp::MeshViewer window("MeshViewer", 800, 600, false);
    window.loadMesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}

//=============================================================================
