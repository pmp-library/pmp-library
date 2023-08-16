file(GLOB SRCS ./*.cpp)
file(GLOB HDRS ./*.h)
set(HDRS ${HDRS} "../mat_vec.h")

if(EMSCRIPTEN)

  add_library(pmp_vis STATIC ${SRCS} ${HDRS})
  target_include_directories(
    pmp_vis PRIVATE ${PROJECT_SOURCE_DIR}/${STB_IMAGE_WRITE_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/${STB_IMAGE_SOURCE_DIR})
  target_link_libraries(pmp_vis stb_image stb_image_write imgui pmp)

else()

  find_package(OpenGL REQUIRED)

  if(OpenGL_FOUND)
    add_library(pmp_vis STATIC ${SRCS} ${HDRS})
    target_include_directories(
      pmp_vis PRIVATE ${PROJECT_SOURCE_DIR}/${STB_IMAGE_WRITE_SOURCE_DIR}
                      ${PROJECT_SOURCE_DIR}/${STB_IMAGE_SOURCE_DIR})
    target_link_libraries(
      pmp_vis
      pmp
      stb_image
      stb_image_write
      imgui
      glfw
      glew
      ${OPENGL_LIBRARIES})
    if(PMP_INSTALL)
      install(TARGETS pmp_vis DESTINATION lib)
      install(FILES ${HDRS} DESTINATION include/pmp/visualization)
    endif()

    if(WITH_CLANG_TIDY)
      set_target_properties(pmp_vis PROPERTIES CXX_CLANG_TIDY
                                               "${CLANG_TIDY_COMMAND}")
    endif()
  endif()

endif()
