# make sure to install headers, use relative path from top-level since we're
# included from above
file(GLOB HEADERS ./algorithms/*.h)
file(GLOB SOURCES ./algorithms/*.cpp)

install(FILES ${HEADERS} DESTINATION include/pmp/algorithms)

target_sources(pmp PRIVATE "${SOURCES}" PUBLIC "${HEADERS}")
#target_include_directories(pmp PUBLIC
  #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  #$<INSTALL_INTERFACE:include/pmp/algorithms>)
