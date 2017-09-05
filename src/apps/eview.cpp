//=============================================================================

#include "EdgeSetProcessingViewer.h"

//=============================================================================

int main(int argc, char **argv)
{
    EdgeSetProcessingViewer window("EdgeSetProcessingViewer", 800, 600);

#ifdef __EMSCRIPTEN__
    window.loadEdgeSet("input.knt");
#else
    if (argc == 2)
        window.loadEdgeSet(argv[1]);
#endif

    return window.run();
}

//=============================================================================
