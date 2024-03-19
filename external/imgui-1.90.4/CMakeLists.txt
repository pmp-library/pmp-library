file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(imgui STATIC ${SOURCES} ${HEADERS})
target_link_libraries(imgui ${CMAKE_DL_LIBS})
target_compile_definitions(imgui PUBLIC IMGUI_USER_CONFIG="pmp_imconfig.h")

if(NOT EMSCRIPTEN)
  target_include_directories(
    imgui
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../${GLFW_SOURCE_DIR_INTERNAL}/include")
else()
  target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)
