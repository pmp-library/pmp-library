find_package(OpenGL REQUIRED)

if(OpenGL_FOUND)

add_executable(mview mview.cpp MeshProcessingViewer.cpp MeshProcessingViewer.h)
target_link_libraries(mview surface_mesh_gl)

endif()