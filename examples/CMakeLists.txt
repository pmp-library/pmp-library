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

# Function to build Emscripten examples with optional preload files
function(add_emscripten_example NAME SOURCE_FILES)
  add_executable(${NAME} ${SOURCE_FILES} data/shell.html)
  target_link_libraries(${NAME} pmp_viewers)

  # Build preload flags from relative file paths
  set(PRELOAD_FLAGS "")
  foreach(PRELOAD_FILE IN LISTS ARGN)
    # Extract file extension to determine output filename
    string(REGEX MATCH "\\.[^.]*$" FILE_EXT "${PRELOAD_FILE}")
    if(FILE_EXT STREQUAL ".obj")
      set(OUTPUT_FILE "input.obj")
    else()
      set(OUTPUT_FILE "input.off")
    endif()
    string(
      APPEND PRELOAD_FLAGS
      " --preload-file ${PROJECT_SOURCE_DIR}/${PRELOAD_FILE}@${OUTPUT_FILE}")
  endforeach()

  set_target_properties(
    ${NAME}
    PROPERTIES
      LINK_FLAGS
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html${PRELOAD_FLAGS}"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/shell.html)
endfunction()

# example apps
if(EMSCRIPTEN AND PMP_BUILD_VIEWERS)

  add_emscripten_example(mview mview.cpp)
  add_emscripten_example(curvature curvature.cpp "data/off/bunny.off")
  add_emscripten_example(smoothing smoothing.cpp "data/off/fandisk.off")
  add_emscripten_example(fairing fairing.cpp "data/off/hemisphere.off")
  add_emscripten_example(
    mpview "mpview.cpp;mesh_processing_viewer.cpp;mesh_processing_viewer.h"
    "data/off/bunny.off")
  add_emscripten_example(subdivision subdivision.cpp "data/obj/suzanne.obj")
  add_emscripten_example(remeshing remeshing.cpp "data/off/bunny.off")
  add_emscripten_example(decimation decimation.cpp "data/off/fandisk.off")
  add_emscripten_example(parameterization parameterization.cpp
                         "data/off/hemisphere.off")
  add_emscripten_example(hole_filling hole_filling.cpp
                         "data/off/hole_capsule.off")

else()

  find_package(OpenGL)

  # build mconvert only on unix / OS-X
  if(NOT WIN32)
    add_executable(mconvert mconvert.cpp)
    target_link_libraries(mconvert pmp)
  endif()

  if(OpenGL_FOUND AND PMP_BUILD_VIEWERS)
    add_executable(mview mview.cpp)
    target_link_libraries(mview pmp_viewers)

    add_executable(curvature curvature.cpp)
    target_link_libraries(curvature pmp_viewers)

    add_executable(subdivision subdivision.cpp)
    target_link_libraries(subdivision pmp_viewers)

    add_executable(smoothing smoothing.cpp)
    target_link_libraries(smoothing pmp_viewers)

    add_executable(fairing fairing.cpp)
    target_link_libraries(fairing pmp_viewers)

    add_executable(parameterization parameterization.cpp)
    target_link_libraries(parameterization pmp_viewers)

    add_executable(decimation decimation.cpp)
    target_link_libraries(decimation pmp_viewers)

    add_executable(remeshing remeshing.cpp)
    target_link_libraries(remeshing pmp_viewers)

    add_executable(hole_filling hole_filling.cpp)
    target_link_libraries(hole_filling pmp_viewers)

    add_executable(mpview mpview.cpp mesh_processing_viewer.cpp
                          mesh_processing_viewer.h)
    target_link_libraries(mpview pmp_viewers)
  endif()

endif()
