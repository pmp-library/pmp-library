//=============================================================================

#include <pmp/gl/MeshViewer.h>
#include <pmp/algorithms/SurfaceRemeshing.h>

using namespace pmp;

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
                // adaptive remeshing
                auto bb = m_mesh.bounds().size();
                SurfaceRemeshing(m_mesh).adaptiveRemeshing(
                        0.001 * bb,  // min length
                        1.0 * bb,    // max length
                        0.001 * bb); // approx. error
                updateMesh();
                break;
            }

            case GLFW_KEY_U:
            {
                Scalar l(0);
                for (auto eit : m_mesh.edges())
                    l += distance(m_mesh.position(m_mesh.vertex(eit, 0)),
                                  m_mesh.position(m_mesh.vertex(eit, 1)));
                l /= (Scalar)m_mesh.nEdges();
                SurfaceRemeshing(m_mesh).uniformRemeshing(l);
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
    Viewer window("Remeshing", 800, 600);
    if (argc == 2)
        window.loadMesh(argv[1]);
    else
        window.loadMesh("input.off");
    return window.run();
#endif
}

//=============================================================================
