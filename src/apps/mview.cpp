//=============================================================================

#include "MeshProcessingViewer.h"

//=============================================================================

int main(int argc, char **argv)
{
    MeshProcessingViewer window("MeshProcessingViewer", 800, 600);

    if (argc == 2)
        window.loadMesh(argv[1]);
#ifdef __EMSCRIPTEN__
    else
        window.loadMesh("input.off");
#endif

    return window.run();
}

//=============================================================================
