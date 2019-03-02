find_package(Doxygen)

if(DOXYGEN_FOUND STREQUAL "NO")
  message(STATUS "Doxygen not found. Please get a copy http://www.doxygen.org")
endif()

if("${DOXYGEN_VERSION}" VERSION_LESS 1.8.3.1)
  message(STATUS "Doxygen version too old. Please get a copy http://www.doxygen.org")
endif()

# prepare doxygen configuration file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# add doxygen as custom target
add_custom_target(doxygen ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  # POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/fonts" "${CMAKE_CURRENT_BINARY_DIR}/html/fonts"
)

# add docs as custom target
add_custom_target(docs ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  # POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/fonts" "${CMAKE_CURRENT_BINARY_DIR}/html/fonts"
  COMMAND jekyll build -s "${CMAKE_CURRENT_SOURCE_DIR}/jekyll/" -d "${CMAKE_CURRENT_BINARY_DIR}/jekyll/"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/jekyll/" "${CMAKE_CURRENT_BINARY_DIR}/html/"
  )

# cleanup $build/html on "make clean"
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES html)
