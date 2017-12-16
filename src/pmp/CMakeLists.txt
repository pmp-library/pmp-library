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
    set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS_INIT} -fopenmp)
  endif()
endif()

if (WIN32 OR EMSCRIPTEN)
  add_library(pmp STATIC ${SOURCES} ${HEADERS})
elseif(UNIX)
  add_library(pmp SHARED ${SOURCES} ${HEADERS})
endif()

set_target_properties(pmp PROPERTIES VERSION ${PMP_VERSION})

install(TARGETS pmp DESTINATION lib)

install(FILES ${HEADERS} DESTINATION include/pmp/)

add_subdirectory(gl)

include(algorithms/CMakeLists.txt)
include(io/CMakeLists.txt)
