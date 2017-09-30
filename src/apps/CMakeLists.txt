find_package(OpenGL REQUIRED)

if (EMSCRIPTEN)

    add_executable(mview mview.cpp MeshProcessingViewer.cpp MeshProcessingViewer.h)
    target_link_libraries(mview pmp_gl pmp)
    set_target_properties(mview PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/obj/suzanne.obj@input.obj")

    add_executable(subdiv subdiv.cpp)
    target_link_libraries(subdiv pmp_gl pmp)
    set_target_properties(subdiv PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/obj/suzanne.obj@input.obj")

    add_executable(remeshing remeshing.cpp)
    target_link_libraries(remeshing pmp_gl pmp)
    set_target_properties(remeshing PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/bunny.off@input.off")

else()

  if(OpenGL_FOUND)
    add_executable(mview mview.cpp MeshProcessingViewer.cpp MeshProcessingViewer.h)
    target_link_libraries(mview pmp_gl glew)

    add_executable(eview eview.cpp EdgeSetProcessingViewer.cpp EdgeSetProcessingViewer.h)
    target_link_libraries(eview pmp_gl glew)

    add_executable(pview pview.cpp PointSetProcessingViewer.cpp PointSetProcessingViewer.h)
    target_link_libraries(pview pmp_gl glew)
  endif()

endif()
