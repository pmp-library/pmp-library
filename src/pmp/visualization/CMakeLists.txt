file(GLOB SRCS ./*.cpp)
file(GLOB HDRS ./*.h)
set(HDRS ${HDRS} "../MatVec.h")

if(EMSCRIPTEN)

  add_library(pmp_vis STATIC ${SRCS} ${HDRS})
  target_link_libraries(pmp_vis stb_image imgui pmp)

else()

  find_package(OpenGL REQUIRED)

  if(OpenGL_FOUND)
    add_library(pmp_vis SHARED ${SRCS} ${HDRS})
    set_property(TARGET pmp_vis PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)
    target_link_libraries(
      pmp_vis
      pmp
      stb_image
      imgui
      glfw
      glew
      ${OPENGL_LIBRARIES})
    if(PMP_INSTALL)
      install(TARGETS pmp_vis DESTINATION lib)
      install(FILES ${HDRS} DESTINATION include/pmp/visualization)
    endif()
  endif()

endif()
