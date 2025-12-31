cmake_minimum_required(VERSION 3.15)
project(pypmp)

# --- Prerequisites ---
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

# --- Source files ---
set(PYPMP_SRC 
    src/module.cpp
    src/matrices.cpp
    src/surface_mesh.cpp
)

set(ALGORITHMS_SRC
    src/algorithms/module.cpp
    src/algorithms/algorithms.cpp
)

# --- Pybind11 modules ---
pybind11_add_module(pypmp ${PYPMP_SRC})
pybind11_add_module(algorithms ${ALGORITHMS_SRC})
target_include_directories(pypmp PRIVATE 
    ${CMAKE_SOURCE_DIR}/../..
    ${CMAKE_SOURCE_DIR}/include
)
target_include_directories(algorithms PRIVATE 
    ${CMAKE_SOURCE_DIR}/../..
    ${CMAKE_SOURCE_DIR}/include
)

# --- Module linkage ---
target_link_directories(pypmp PRIVATE 
    ${CMAKE_SOURCE_DIR}/../../../build
)
target_link_libraries(pypmp PRIVATE pmp)
target_link_directories(algorithms PRIVATE 
    ${CMAKE_SOURCE_DIR}/../../../build
)
target_link_libraries(algorithms PRIVATE pmp)

# Dynamic loader of pypmp dynamic library
set_target_properties(pypmp algorithms PROPERTIES
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/../../../build"
    INSTALL_RPATH "${CMAKE_SOURCE_DIR}/../../../build"
)

# --- Install module ---
# Module gets installed to the Python folder with `make install`
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "Install path prefix" FORCE)
endif()

install(TARGETS pypmp DESTINATION pypmp)
install(TARGETS algorithms DESTINATION pypmp/algorithms)
