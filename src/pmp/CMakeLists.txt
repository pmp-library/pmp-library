file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)

if (WIN32 OR EMSCRIPTEN)
  add_library(pmp STATIC ${SOURCES} ${HEADERS})
elseif(UNIX)
  add_library(pmp SHARED ${SOURCES} ${HEADERS})
endif()

set_target_properties(pmp PROPERTIES VERSION 1.0)

install(TARGETS pmp DESTINATION lib)

install(FILES ${HEADERS} DESTINATION include/pmp/)

add_subdirectory(gl)

include(algorithms/CMakeLists.txt)
include(io/CMakeLists.txt)
