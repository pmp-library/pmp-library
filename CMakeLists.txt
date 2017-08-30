#cmake_policy(SET CMP0048 NEW) # for project versioning

project(surface_mesh)

cmake_minimum_required(VERSION 3.0.2)

# set output paths
set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})


set(GLFW_SOURCE_DIR "external/glfw")
# set(GLM_SOURCE_DIR "external/glm")
set(GLEW_SOURCE_DIR "external/glew")

# Building only the GLFW lib
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory(${GLFW_SOURCE_DIR} ${GLEW_SOURCE_DIR})

include_directories(${GLFW_SOURCE_DIR}/include
  ${GLFW_SOURCE_DIR}/deps
  ${GLEW_SOURCE_DIR}/include)

add_definitions(-DGLEW_STATIC)
add_library(glew STATIC
  ${GLEW_SOURCE_DIR}/src/glew.c
  ${GLEW_SOURCE_DIR}/include
)
target_link_libraries(glew ${GLFW_LIBRARIES})

include(AddFileDependencies)
include_directories(${CMAKE_SOURCE_DIR}/src/)

# setup for code coverage testing
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" AND CMAKE_COMPILER_IS_GNUCXX)
  enable_testing()

  # set compiler flags
  set(CMAKE_CXX_FLAGS "-g -O0 --coverage")

  # find required tools
  find_program(GCOV gcov)
  find_program(LCOV lcov)
  find_program(GENHTML genhtml)

  # add coverage target
  add_custom_target(coverage

    # cleanup old counters
    ${LCOV} --directory . --zerocounters

    # run test command
    COMMAND make test

    # gather data, generate report
    COMMAND ${LCOV} --directory . --capture --output-file coverage.info
    COMMAND ${LCOV} --remove coverage.info 'tests/*' '/usr/*' 'googletest/*' --output-file coverage.info.cleaned
    COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info.cleaned
    COMMAND ${CMAKE_COMMAND} -E remove coverage.info coverage.info.cleaned

    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()


# set default compiler flags
if((UNIX OR APPLE) AND CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra")
endif()

if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_USE_MATH_DEFINES")
endif()


# make a release build by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()


if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
endif()

# choose size of the index type
if(SM_INDEX_TYPE EQUAL 64)
  add_definitions(-DSM_INDEX_TYPE_64)
endif()

add_subdirectory(src/surface_mesh)
add_subdirectory(src/apps)
add_subdirectory(doc)
add_subdirectory(examples)
add_subdirectory(benchmarks)

enable_testing()
add_subdirectory(tests)

set(CPACK_PACKAGE_VERSION 1.1)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES "/build/;/.git/;~$;${CPACK_SOURCE_IGNORE_FILES}")
set(CPACK_SOURCE_GENERATOR "ZIP")
include(CPack)
