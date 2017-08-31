find_package(OpenGL REQUIRED)

if(OpenGL_FOUND)

file(GLOB SRCS ./*.cpp)
file(GLOB HDRS ./*.h)

set(HDRS ${HDRS} "../Vector.h")

add_library(surface_mesh_gl STATIC ${SRCS} ${HDRS})
target_link_libraries(surface_mesh_gl glfw ${OPENGL_LIBRARIES} surface_mesh)

endif()
