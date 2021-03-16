# make sure to install headers, use relative path from top-level since we're
# included from above
file(GLOB HEADERS ./algorithms/*.h)
file(GLOB SOURCES ./algorithms/*.cpp)

if(PMP_INSTALL)
  install(FILES ${HEADERS} DESTINATION include/pmp/algorithms)
endif()

target_sources(pmp PRIVATE "${SOURCES}" "${HEADERS}")
