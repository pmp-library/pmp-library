//=============================================================================

#include "MeshProcessingViewer.h"

//=============================================================================

int main(int argc, char **argv)
{
    MeshProcessingViewer window("MeshProcessingViewer", 800, 600);

#ifdef __EMSCRIPTEN__
    window.loadMesh("input.obj");
#else
    if (argc == 2)
        window.loadMesh(argv[1]);
#endif

    return window.run();
}

//=============================================================================
