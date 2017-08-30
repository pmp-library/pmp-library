
# make sure to install headers, use relative path from top-level since we're
# included from above
file(GLOB HEADERS ./algorithms/*.h)
install(FILES ${HEADERS} DESTINATION include/surface_mesh/algorithms)

target_sources(surface_mesh
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/SurfaceSubdivider.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FeatureDetection.cpp"
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}/SurfaceSubdivider.h"
  "${CMAKE_CURRENT_LIST_DIR}/FeatureDetection.h"
  )
