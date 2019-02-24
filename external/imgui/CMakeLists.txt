file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)
add_library(imgui STATIC ${SOURCES} ${HEADERS})
target_compile_definitions(imgui PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GLEW)
