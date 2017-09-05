find_package(OpenGL REQUIRED)

if (EMSCRIPTEN)

    add_executable(mview mview.cpp MeshProcessingViewer.cpp MeshProcessingViewer.h)
    target_link_libraries(mview surface_mesh_gl surface_mesh)
    set_target_properties(mview PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data/suzanne.obj@input.obj")

    add_executable(subdiv subdiv.cpp)
    target_link_libraries(subdiv surface_mesh_gl surface_mesh)
    set_target_properties(subdiv PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data/suzanne.obj@input.obj")

    add_executable(remeshing remeshing.cpp)
    target_link_libraries(remeshing surface_mesh_gl surface_mesh)
    set_target_properties(remeshing PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data/bunny.off@input.off")

else()

  if(OpenGL_FOUND)
    add_executable(mview mview.cpp MeshProcessingViewer.cpp MeshProcessingViewer.h)
    target_link_libraries(mview surface_mesh_gl glew)

    add_executable(eview eview.cpp EdgeSetProcessingViewer.cpp EdgeSetProcessingViewer.h)
    target_link_libraries(eview surface_mesh_gl glew)
  endif()

endif()
