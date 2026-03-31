if(EMSCRIPTEN)
  add_executable(polygonal main.cpp polygonal.cpp lasso_drawable.cpp)
  target_link_libraries(polygonal pmp pmp_viewers stb_image stb_image_write
                        imgui)
  set_target_properties(
    polygonal
    PROPERTIES
      LINK_FLAGS
      "-sEXPORTED_FUNCTIONS=_main,_load_mesh -sEXPORTED_RUNTIME_METHODS=ccall --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/shell.html --preload-file ${CMAKE_BINARY_DIR}/_deps/pmp-library-src/data/off/bunny_adaptive.off@input.off"
      LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shell.html)
  file(GLOB FAVICONS ${CMAKE_SOURCE_DIR}/data/favicons/*.*)
  add_custom_command(
    TARGET polygonal
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy
      ${CMAKE_SOURCE_DIR}/docs/_static/polygonal-dark.svg ${FAVICONS}
      ${PROJECT_BINARY_DIR})
else()
  find_package(OpenGL REQUIRED)
  add_executable(polygonal main.cpp polygonal.cpp lasso_drawable.cpp)
  target_link_libraries(polygonal pmp_viewers pmp imgui glfw
                        ${OPENGL_LIBRARIES})
endif()
