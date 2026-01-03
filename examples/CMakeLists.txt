add_executable(basics basics.cpp)
target_link_libraries(basics pmp)

add_executable(io io.cpp)
target_link_libraries(io pmp)

add_executable(iterators iterators.cpp)
target_link_libraries(iterators pmp)

add_executable(barycenter barycenter.cpp)
target_link_libraries(barycenter pmp)

add_executable(properties properties.cpp)
target_link_libraries(properties pmp)

add_executable(eigen eigen.cpp)
target_link_libraries(eigen pmp)

add_custom_target(
  examples
  COMMAND
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  COMMENT "")

add_dependencies(examples basics io iterators barycenter properties)

# example apps
if(EMSCRIPTEN AND PMP_BUILD_VIS)

  add_executable(mview mview.cpp data/shell.html)
  target_link_libraries(mview pmp_vis)
  set_target_properties(
    mview
    PROPERTIES LINK_FLAGS
               "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html"
               LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)

  add_executable(curvature curvature.cpp data/shell.html)
  target_link_libraries(curvature pmp_vis)
  set_target_properties(
    curvature
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data/off/bunny.off@input.off"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)

  add_executable(smoothing smoothing.cpp data/shell.html)
  target_link_libraries(smoothing pmp_vis)
  set_target_properties(
    smoothing
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data/off/fandisk.off@input.off"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)

  add_executable(fairing fairing.cpp data/shell.html)
  target_link_libraries(fairing pmp_vis)
  set_target_properties(
    fairing
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data/off/hemisphere.off@input.off"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)

  add_executable(mpview mpview.cpp mesh_processing_viewer.cpp
                        mesh_processing_viewer.h data/shell.html)
  target_link_libraries(mpview pmp_vis)
  set_target_properties(
    mpview
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data/off/bunny.off@input.off"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)

  add_executable(subdivision subdivision.cpp data/shell.html)
  target_link_libraries(subdivision pmp_vis)
  set_target_properties(
    subdivision
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data/obj/suzanne.obj@input.obj"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)

  add_executable(remeshing remeshing.cpp data/shell.html)
  target_link_libraries(remeshing pmp_vis)
  set_target_properties(
    remeshing
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data/off/bunny.off@input.off"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)

  add_executable(decimation decimation.cpp data/shell.html)
  target_link_libraries(decimation pmp_vis)
  set_target_properties(
    decimation
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data/off/fandisk.off@input.off"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)

  add_executable(parameterization parameterization.cpp data/shell.html)
  target_link_libraries(parameterization pmp_vis)
  set_target_properties(
    parameterization
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data/off/hemisphere.off@input.off"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)

  add_executable(hole_filling hole_filling.cpp data/shell.html)
  target_link_libraries(hole_filling pmp_vis)
  set_target_properties(
    hole_filling
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data/off/hole_capsule.off@input.off"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)

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

    add_executable(hole_filling hole_filling.cpp)
    target_link_libraries(hole_filling pmp_vis)

    add_executable(mpview mpview.cpp mesh_processing_viewer.cpp
                          mesh_processing_viewer.h)
    target_link_libraries(mpview pmp_vis)
  endif()

endif()
