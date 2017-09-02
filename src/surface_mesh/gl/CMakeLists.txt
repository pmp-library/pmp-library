find_package(OpenGL REQUIRED)

file(GLOB SRCS ./*.cpp)
file(GLOB HDRS ./*.h)
set(HDRS ${HDRS} "../Vector.h")

if (EMSCRIPTEN)

    add_library(surface_mesh_gl STATIC ${SRCS} ${HDRS})
    target_link_libraries(surface_mesh_gl surface_mesh)

else()

    if (OpenGL_FOUND)
        add_library(surface_mesh_gl STATIC ${SRCS} ${HDRS})
        target_link_libraries(surface_mesh_gl glfw ${OPENGL_LIBRARIES} surface_mesh)
    endif()

endif()
