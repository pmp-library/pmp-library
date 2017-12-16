# make sure to install headers, use relative path from top-level since we're
# included from above
file(GLOB HEADERS ./io/*.h)
file(GLOB SOURCES ./io/*.cpp)

install(FILES ${HEADERS} DESTINATION include/pmp/io)

target_sources(pmp PRIVATE "${SOURCES}" "${HEADERS}")