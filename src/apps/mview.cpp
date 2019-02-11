//=============================================================================

#include <pmp/gl/MeshViewer.h>

//=============================================================================

int main(int argc, char **argv)
{
#ifndef __EMSCRIPTEN__
    pmp::MeshViewer viewer("MeshViewer", 800, 600);
    if (argc > 1)
        viewer.load_mesh(argv[1]);
    if (argc > 2)
        viewer.load_texture(argv[2], GL_SRGB8);
    return viewer.run();
#else
    pmp::MeshViewer viewer("MeshViewer", 800, 600, false);
    if (argc > 1)
        viewer.load_mesh(argv[1]);
    else
        viewer.load_mesh("input.off");
    if (argc > 2)
        viewer.load_texture(argv[2], GL_SRGB8);
    return viewer.run();
#endif
}

//=============================================================================
