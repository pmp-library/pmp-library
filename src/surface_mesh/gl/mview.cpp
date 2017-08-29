//=============================================================================

#include <surface_mesh/gl/MeshViewer.h>

//=============================================================================

int main(int argc, char **argv)
{
    surface_mesh::MeshViewer window("MeshViewer", 800, 600);

    if (argc == 2)
        window.loadMesh(argv[1]);

    return window.run();
}

//=============================================================================
