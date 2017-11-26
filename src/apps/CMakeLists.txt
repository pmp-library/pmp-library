
if (EMSCRIPTEN)

    add_executable(mview mview.cpp data/shell.html)
    target_link_libraries(mview pmp_gl)
    set_target_properties(mview PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/bunny.off@input.off")

    add_executable(curview curview.cpp data/shell.html)
    target_link_libraries(curview pmp_gl)
    set_target_properties(curview PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/bunny.off@input.off")

    add_executable(mpview mpview.cpp MeshProcessingViewer.cpp MeshProcessingViewer.h data/shell.html)
    target_link_libraries(mpview pmp_gl)
    set_target_properties(mpview PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/bunny.off@input.off")

    add_executable(subdiv subdiv.cpp data/shell.html)
    target_link_libraries(subdiv pmp_gl)
    set_target_properties(subdiv PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/obj/suzanne.obj@input.obj")

    add_executable(remeshing remeshing.cpp data/shell.html)
    target_link_libraries(remeshing pmp_gl)
    set_target_properties(remeshing PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/bunny.off@input.off")

else()

    find_package(OpenGL)

    # build mconvert only on unix / OS-X
    if(NOT WIN32)
      add_executable(mconvert mconvert.cpp)
      target_link_libraries(mconvert pmp)
    endif()

    if(OpenGL_FOUND)
        add_executable(mview mview.cpp)
        target_link_libraries(mview pmp_gl)

        add_executable(curview curview.cpp)
        target_link_libraries(curview pmp_gl)

        add_executable(remeshing remeshing.cpp)
        target_link_libraries(remeshing pmp_gl)

        add_executable(mpview mpview.cpp MeshProcessingViewer.cpp MeshProcessingViewer.h)
        target_link_libraries(mpview pmp_gl)

        add_executable(eview eview.cpp EdgeSetProcessingViewer.cpp EdgeSetProcessingViewer.h)
        target_link_libraries(eview pmp_gl)

        add_executable(pview pview.cpp PointSetProcessingViewer.cpp PointSetProcessingViewer.h)
        target_link_libraries(pview pmp_gl)
    endif()

endif()
