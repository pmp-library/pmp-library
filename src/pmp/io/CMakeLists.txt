# make sure to install headers, use relative path from top-level since we're
# included from above
file(GLOB HEADERS ./io/*.h)
file(GLOB SOURCES ./io/*.cpp)

if(PMP_INSTALL)
  install(FILES ${HEADERS} DESTINATION include/pmp/io)
endif()

target_sources(pmp PRIVATE "${SOURCES}" "${HEADERS}")
