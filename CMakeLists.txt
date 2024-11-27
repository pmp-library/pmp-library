cmake_minimum_required(VERSION 3.16.3)
cmake_policy(SET CMP0048 NEW)
project(
  pmp-library
  VERSION 3.0.0
  DESCRIPTION "The Polygon Mesh Processing Library"
  HOMEPAGE_URL "https://www.pmp-library.org")
cmake_policy(SET CMP0072 NEW)

option(PMP_BUILD_EXAMPLES "Build the PMP examples" ON)
option(PMP_BUILD_TESTS "Build the PMP test programs" ON)
option(PMP_BUILD_DOCS "Build the PMP documentation" ON)
option(PMP_BUILD_VIS "Build the PMP visualization tools" ON)
option(PMP_INSTALL "Install the PMP library and headers" ON)
option(PMP_STRICT_COMPILATION "Treat compiler warnings as errors" ON)
option(PMP_BUILD_REGRESSIONS "Build the PMP regression test programs" OFF)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# set output paths
set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(PMP_BUILD_VIS)
  set(GLFW_SOURCE_DIR_INTERNAL "external/glfw-3.4")
  set(GLEW_SOURCE_DIR "external/glew-2.2.0")
  set(IMGUI_SOURCE_DIR "external/imgui-1.90.4")
  set(STB_IMAGE_SOURCE_DIR "external/stb_image-2.28")
  set(STB_IMAGE_WRITE_SOURCE_DIR "external/stb_image_write-1.16")

  add_subdirectory(${STB_IMAGE_WRITE_SOURCE_DIR})
  add_subdirectory(${STB_IMAGE_SOURCE_DIR})

  # Building only the GLFW lib
  set(BUILD_SHARED_LIBS
      OFF
      CACHE BOOL "")
  set(GLFW_BUILD_EXAMPLES
      OFF
      CACHE BOOL "")
  set(GLFW_BUILD_TESTS
      OFF
      CACHE BOOL "")
  set(GLFW_BUILD_DOCS
      OFF
      CACHE BOOL "")
  set(GLFW_INSTALL
      OFF
      CACHE BOOL "")

  # disable GLFW wayland support if scanner is not found. See #207
  find_program(WAYLAND_SCANNER_EXECUTABLE NAMES wayland-scanner)
  if(NOT WAYLAND_SCANNER_EXECUTABLE)
    set(GLFW_BUILD_WAYLAND False)
  endif()

  # setup use of GLFW and GLEW
  if(NOT EMSCRIPTEN)
    add_subdirectory(${GLFW_SOURCE_DIR_INTERNAL} ${GLEW_SOURCE_DIR})

    add_definitions(-DGLEW_STATIC)
    add_library(glew STATIC ${GLEW_SOURCE_DIR}/src/glew.c
                            ${GLEW_SOURCE_DIR}/include)
    set_property(TARGET glew PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_include_directories(glew PUBLIC ${GLEW_SOURCE_DIR}/include
                                           ${GLFW_SOURCE_DIR_INTERNAL}/include)
    target_link_libraries(glew ${GLFW_LIBRARIES})
  endif()

  # setup IMGUI
  add_subdirectory(${IMGUI_SOURCE_DIR})
endif(PMP_BUILD_VIS)

if(NOT DEFINED EIGEN3_INCLUDE_DIR)
  set(EIGEN3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/external/eigen-3.4.0")
endif()
find_package(Eigen3 3.4.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src/)

# setup for code coverage testing
if(CMAKE_BUILD_TYPE STREQUAL "Debug"
   AND ENABLE_COVERAGE
   AND NOT WIN32)
  enable_testing()

  # set compiler flags
  set(CMAKE_CXX_FLAGS "-g -O0 --coverage")

  # find required tools
  find_program(LCOV lcov REQUIRED)
  find_program(GENHTML genhtml REQUIRED)

  # add coverage target
  add_custom_target(
    coverage
    # gather data
    COMMAND
      ${LCOV} --directory . --capture --exclude '*/external/*' --exclude
      '*/tests/*' --exclude '/usr/*' --exclude '/Applications/*' --exclude
      'v1/*' --output-file coverage.info
    # generate report
    COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set default compiler flags for both gcc and clang
set(COMMON_CXX_FLAGS "-pedantic -Wall -Wextra -Wshadow")
if(PMP_STRICT_COMPILATION)
  set(COMMON_CXX_FLAGS "${COMMON_CXX_FLAGS} -Werror")
endif()

if((UNIX OR APPLE) AND CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} ${COMMON_CXX_FLAGS} -Wno-deprecated-copy")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0.0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-int-in-bool-context")
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} ${COMMON_CXX_FLAGS} --system-header-prefix=Eigen")
endif()

if(WIN32)
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -D_USE_MATH_DEFINES -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS"
  )
endif()

if(MSVC)
  add_compile_options(/wd4267) # conversion from size_t
  add_compile_options(/wd4244) # conversion from double to float
  add_compile_options(/wd4305) # truncation from double to float
endif()

if(EMSCRIPTEN)
  add_compile_options(--no-heap-copy)
  add_compile_options(-fexceptions)
  add_link_options(-fexceptions)
  add_link_options(
    "SHELL:-sWASM=1 -sUSE_WEBGL2=1 -sUSE_GLFW=3 -sALLOW_MEMORY_GROWTH=1 -sALLOW_TABLE_GROWTH=1 -sSTACK_SIZE=5MB"
  )
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# make a release build by default
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type specified. Using 'Release' as default.")
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE
        STRING
        "The build type. Possible values: Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" AND NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
endif()

# choose size of the scalar type
if(PMP_SCALAR_TYPE EQUAL 64)
  message(STATUS "Using 64-bit scalar type")
  add_definitions(-DPMP_SCALAR_TYPE_64)
endif()

# choose size of the index type
if(PMP_INDEX_TYPE EQUAL 64)
  message(STATUS "Using 64-bit index type")
  add_definitions(-DPMP_INDEX_TYPE_64)
endif()

# setup clang-tidy if program found
option(WITH_CLANG_TIDY "Run clang-tidy checks" OFF)
include(clang-tidy)

# which directories to process
if(EMSCRIPTEN)
  add_subdirectory(src/pmp)
  if(PMP_BUILD_EXAMPLES)
    add_subdirectory(examples)
  endif()
else()
  add_subdirectory(src/pmp)
  if(PMP_BUILD_DOCS)
    add_subdirectory(docs)
  endif()
  if(PMP_BUILD_EXAMPLES)
    add_subdirectory(examples)
  endif()
  if(PMP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
  endif()
endif()

if(NOT EMSCRIPTEN AND PMP_INSTALL)

  # Generate package configuration files
  include(CMakePackageConfigHelpers)

  set(PMP_CONFIG_PATH "lib${LIB_SUFFIX}/cmake/pmp")

  configure_package_config_file(
    cmake/pmpConfig.cmake.in pmpConfig.cmake
    INSTALL_DESTINATION "${PMP_CONFIG_PATH}"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)

  install(
    EXPORT pmpTargets
    FILE pmpTargets.cmake
    EXPORT_LINK_INTERFACE_LIBRARIES
    DESTINATION "${PMP_CONFIG_PATH}")

  export(TARGETS pmp FILE pmpTargets.cmake)

  write_basic_package_version_file(
    pmpConfigVersion.cmake
    VERSION ${PMP_VERSION}
    COMPATIBILITY SameMajorVersion)

  install(FILES "${PROJECT_BINARY_DIR}/pmpConfig.cmake"
                "${PROJECT_BINARY_DIR}/pmpConfigVersion.cmake"
          DESTINATION "${PMP_CONFIG_PATH}")

endif()

# add uninstall target if none is defined
if(NOT TARGET uninstall AND PMP_INSTALL)

  configure_file("${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
                 cmake_uninstall.cmake IMMEDIATE @ONLY)

  add_custom_target(uninstall "${CMAKE_COMMAND}" -P
                              "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake")

endif()

# add clang-format target if program is found
include(clang-format)
