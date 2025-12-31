file(GLOB SOURCES ./python/*.cpp)
file(GLOB HEADERS ./python/*.h)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(pypmp ./python/pypmp.cpp)
install(TARGETS ${HEADERS} pypmp DESTINATION include/pmp/python)

target_sources(pmp PRIVATE "${SOURCES}" "${HEADERS}")