file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)
add_library(imgui STATIC ${SOURCES} ${HEADERS})
