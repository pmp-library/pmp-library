//=============================================================================

#include <pmp/gl/MeshViewer.h>
#include <pmp/algorithms/SurfaceSubdivision.h>

using namespace pmp;

//=============================================================================

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height, bool showgui)
        : MeshViewer(title, width, height, showgui)
    {
        setDrawMode("Hidden Line");
    }

protected:

    void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (action != GLFW_PRESS) return;
        switch (key)
        {
            case GLFW_KEY_ENTER:
            {
                if (m_mesh.isTriangleMesh())
                    SurfaceSubdivision(m_mesh).loop();
                else
                    SurfaceSubdivision(m_mesh).catmullClark();
                updateMesh();
                break;
            }

            default:
            {
                MeshViewer::keyboard(window, key, scancode, action, mods);
            }
        }
    }
};

//=============================================================================

int main(int argc, char **argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Subdivision", 800, 600);
    if (argc == 2) window.loadMesh(argv[1]);
    return window.run();
#else
    Viewer window("Subdivision", 800, 600, false);
    window.loadMesh(argc==2 ? argv[1] : "input.obj");
    return window.run();
#endif
}

//=============================================================================
