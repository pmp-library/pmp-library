# make sure to install headers, use relative path from top-level since we're
# included from above
file(GLOB HEADERS ./algorithms/*.h)
file(GLOB SOURCES ./algorithms/*.cpp)
target_sources(surface_mesh PRIVATE "${SOURCES}" PUBLIC "${HEADERS}")
install(FILES ${HEADERS} DESTINATION include/surface_mesh/algorithms)
