find_package(OpenGL REQUIRED)

if(OpenGL_FOUND)

file(GLOB_RECURSE SRCS ./*.cpp)
file(GLOB_RECURSE HDRS ./*.h)

set(HDRS ${HDRS} "../Vector.h")

add_library(surface_mesh_gl STATIC ${SRCS} ${HDRS})
target_link_libraries(surface_mesh_gl glfw ${OPENGL_LIBRARIES} surface_mesh)

add_executable(mview mview.cpp)
target_link_libraries(mview glfw ${OPENGL_LIBRARIES} surface_mesh_gl)

endif()