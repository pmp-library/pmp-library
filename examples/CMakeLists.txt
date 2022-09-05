add_executable(basics basics.cpp)
target_link_libraries(basics pmp)

add_executable(io io.cpp)
target_link_libraries(io pmp)

add_executable(iterators iterators.cpp)
target_link_libraries(iterators pmp)

add_executable(barycenter barycenter.cpp)
target_link_libraries(barycenter pmp)

add_custom_target(
  examples
  COMMAND
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  COMMENT "")

add_dependencies(examples basics io iterators barycenter)

# example apps
if(EMSCRIPTEN AND PMP_BUILD_VIS)

  add_executable(mview mview.cpp data/shell.html)
  target_link_libraries(mview pmp_vis)
  set_target_properties(
    mview PROPERTIES LINK_FLAGS
                     "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html")

  add_executable(curvature curvature.cpp data/shell.html)
  target_link_libraries(curvature pmp_vis)
  set_target_properties(
    curvature
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/bunny.off@input.off"
  )

  add_executable(smoothing smoothing.cpp data/shell.html)
  target_link_libraries(smoothing pmp_vis)
  set_target_properties(
    smoothing
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/fandisk.off@input.off"
  )

  add_executable(fairing fairing.cpp data/shell.html)
  target_link_libraries(fairing pmp_vis)
  set_target_properties(
    fairing
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/hemisphere.off@input.off"
  )

  add_executable(mpview mpview.cpp MeshProcessingViewer.cpp
                        MeshProcessingViewer.h data/shell.html)
  target_link_libraries(mpview pmp_vis)
  set_target_properties(
    mpview
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/bunny.off@input.off"
  )

  add_executable(subdivision subdivision.cpp data/shell.html)
  target_link_libraries(subdivision pmp_vis)
  set_target_properties(
    subdivision
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/obj/suzanne.obj@input.obj"
  )

  add_executable(remeshing remeshing.cpp data/shell.html)
  target_link_libraries(remeshing pmp_vis)
  set_target_properties(
    remeshing
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/bunny.off@input.off"
  )

  add_executable(decimation decimation.cpp data/shell.html)
  target_link_libraries(decimation pmp_vis)
  set_target_properties(
    decimation
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/fandisk.off@input.off"
  )

  add_executable(parameterization parameterization.cpp data/shell.html)
  target_link_libraries(parameterization pmp_vis)
  set_target_properties(
    parameterization
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/external/pmp-data/off/hemisphere.off@input.off"
  )

else()

  find_package(OpenGL)

  # build mconvert only on unix / OS-X
  if(NOT WIN32)
    add_executable(mconvert mconvert.cpp)
    target_link_libraries(mconvert pmp)
  endif()

  if(OpenGL_FOUND AND PMP_BUILD_VIS)
    add_executable(mview mview.cpp)
    target_link_libraries(mview pmp_vis)

    add_executable(curvature curvature.cpp)
    target_link_libraries(curvature pmp_vis)

    add_executable(subdivision subdivision.cpp)
    target_link_libraries(subdivision pmp_vis)

    add_executable(smoothing smoothing.cpp)
    target_link_libraries(smoothing pmp_vis)

    add_executable(fairing fairing.cpp)
    target_link_libraries(fairing pmp_vis)

    add_executable(parameterization parameterization.cpp)
    target_link_libraries(parameterization pmp_vis)

    add_executable(decimation decimation.cpp)
    target_link_libraries(decimation pmp_vis)

    add_executable(remeshing remeshing.cpp)
    target_link_libraries(remeshing pmp_vis)

    add_executable(mpview mpview.cpp MeshProcessingViewer.cpp
                          MeshProcessingViewer.h)
    target_link_libraries(mpview pmp_vis)

    add_executable(deformation_transfer deformation_transfer.cpp)
    target_link_libraries(deformation_transfer pmp_vis)

  endif()

endif()
