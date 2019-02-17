file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)

if(NOT (UNIX AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")))
  find_package(OpenMP)
  if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    if(UNIX OR APPLE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse")
    endif()
    if(MSVC)
      add_definitions(/arch:SSE)
    endif(MSVC)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_INIT} -fopenmp")
  endif()
endif()

if (WIN32 OR EMSCRIPTEN)
  add_library(pmp STATIC ${SOURCES} ${HEADERS} $<TARGET_OBJECTS:rply>)
elseif(UNIX)
  add_library(pmp SHARED ${SOURCES} ${HEADERS} $<TARGET_OBJECTS:rply>)
endif()

set_target_properties(pmp PROPERTIES VERSION ${PMP_VERSION})

# check for recent cmake version
if(${CMAKE_VERSION} VERSION_GREATER "3.6.0")
  if(CLANG_TIDY_EXE AND FALSE) # disabled by default
    set_target_properties(
      pmp PROPERTIES
      CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
      )
  endif()
endif()


if(NOT EMSCRIPTEN)

  target_include_directories(pmp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:include/>)

  if((UNIX OR APPLE) AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(pmp PUBLIC "-std=c++11")
  endif()

  install(TARGETS pmp EXPORT pmpTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

  install(FILES ${HEADERS} DESTINATION include/pmp/)

endif()

add_subdirectory(visualization)

include(algorithms/CMakeLists.txt)
