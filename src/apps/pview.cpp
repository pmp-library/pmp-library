//=============================================================================

#include "PointSetProcessingViewer.h"

//=============================================================================

int main(int argc, char **argv)
{
    PointSetProcessingViewer window("PointSetProcessingViewer", 800, 600);

#ifdef __EMSCRIPTEN__
    window.loadPointSet("input.xyz");
#else
    if (argc == 2)
        window.loadPointSet(argv[1]);
#endif

    return window.run();
}

//=============================================================================
