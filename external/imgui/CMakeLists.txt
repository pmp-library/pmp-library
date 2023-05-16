file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(imgui STATIC ${SOURCES} ${HEADERS})
#TODO: instead of linking glew to get the glfw includ dir, maybe just use a global variable?
target_link_libraries(imgui ${CMAKE_DL_LIBS} glew)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)
