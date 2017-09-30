find_package(OpenGL REQUIRED)

file(GLOB SRCS ./*.cpp)
file(GLOB HDRS ./*.h)
set(HDRS ${HDRS} "../Vector.h")

if (EMSCRIPTEN)

    add_library(pmp_gl STATIC ${SRCS} ${HDRS})
    target_link_libraries(pmp_gl pmp)

else()

    if (OpenGL_FOUND)
        add_library(pmp_gl STATIC ${SRCS} ${HDRS})
        target_link_libraries(pmp_gl glfw ${OPENGL_LIBRARIES} pmp)
    endif()

endif()
