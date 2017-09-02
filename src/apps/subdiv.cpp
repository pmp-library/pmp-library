//=============================================================================

#include <surface_mesh/gl/MeshViewer.h>
#include <surface_mesh/algorithms/SurfaceSubdivision.h>

using namespace surface_mesh;

//=============================================================================

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height)
        : MeshViewer(title,width, height)
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
#ifdef __EMSCRIPTEN__
    Viewer window("Subdivision", 800, 600);
    window.loadMesh("input.obj");
    return window.run();
#endif
}

//=============================================================================
