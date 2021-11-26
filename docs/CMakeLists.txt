find_package(Doxygen 1.9.2)

if(DOXYGEN_FOUND)

  # prepare doxygen configuration file
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                 ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  # add docs as custom target
  add_custom_target(docs ${DOXYGEN_EXECUTABLE}
                         ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  # cleanup $build/html on "make clean"
  set_property(
    DIRECTORY
    APPEND
    PROPERTY ADDITIONAL_MAKE_CLEAN_FILES html)

endif()
