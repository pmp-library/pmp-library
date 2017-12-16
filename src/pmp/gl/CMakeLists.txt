file(GLOB SRCS ./*.cpp)
file(GLOB HDRS ./*.h)
set(HDRS ${HDRS} "../Vector.h")

if (EMSCRIPTEN)

    add_library(pmp_gl STATIC ${SRCS} ${HDRS})
    target_link_libraries(pmp_gl imgui pmp)

else()

    find_package(OpenGL REQUIRED)

    if (OpenGL_FOUND)
        add_library(pmp_gl STATIC ${SRCS} ${HDRS})
        target_link_libraries(pmp_gl pmp imgui glfw glew ${OPENGL_LIBRARIES})
        install(TARGETS pmp_gl DESTINATION lib)
    endif()

endif()
