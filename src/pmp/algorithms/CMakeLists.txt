# make sure to install headers, use relative path from top-level since we're
# included from above
file(GLOB HEADERS ./algorithms/*.h)
file(GLOB SOURCES ./algorithms/*.cpp)

# remove smoothing files if we don't have Eigen3 support
if(NOT EIGEN3_FOUND)
  list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/./algorithms/SurfaceSmoothing.cpp")
  list(REMOVE_ITEM HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/./algorithms/SurfaceSmoothing.h")
endif()

target_sources(pmp PRIVATE "${SOURCES}" PUBLIC "${HEADERS}")
install(FILES ${HEADERS} DESTINATION include/pmp/algorithms)
