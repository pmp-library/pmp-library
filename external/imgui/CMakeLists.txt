file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(imgui STATIC ${SOURCES} ${HEADERS})
# test
target_link_libraries(imgui ${CMAKE_DL_LIBS})
set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)