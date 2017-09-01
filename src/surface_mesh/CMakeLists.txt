file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)

if(UNIX)
  add_library(surface_mesh SHARED ${SOURCES} ${HEADERS})
elseif(WIN32)
  add_library(surface_mesh STATIC ${SOURCES} ${HEADERS})
endif()

set_target_properties(surface_mesh PROPERTIES VERSION 1.0)

install(TARGETS surface_mesh DESTINATION lib)

install(FILES ${HEADERS} DESTINATION include/surface_mesh/)

add_subdirectory(gl)

include(algorithms/CMakeLists.txt)
include(io/CMakeLists.txt)