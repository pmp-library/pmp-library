find_package(OpenGL REQUIRED)

file(GLOB SRCS ./*.cpp)
file(GLOB HDRS ./*.h)
set(HDRS ${HDRS} "../Vector.h")

if (EMSCRIPTEN)

    add_library(pmp_gl STATIC ${SRCS} ${HDRS})
    target_link_libraries(pmp_gl imgui pmp)

else()

    if (OpenGL_FOUND)
        add_library(pmp_gl STATIC ${SRCS} ${HDRS})
        target_link_libraries(pmp_gl imgui glfw ${OPENGL_LIBRARIES} pmp)
    endif()

endif()
