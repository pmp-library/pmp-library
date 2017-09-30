if(WITH_EXAMPLES)

  add_executable(SurfaceMeshBasics SurfaceMeshBasics.cpp)
  target_link_libraries(SurfaceMeshBasics pmp)

  add_executable(SurfaceMeshIO SurfaceMeshIO.cpp)
  target_link_libraries(SurfaceMeshIO pmp)

  add_executable(SurfaceMeshIterators SurfaceMeshIterators.cpp)
  target_link_libraries(SurfaceMeshIterators pmp)

  add_executable(SurfaceMeshBarycenter SurfaceMeshBarycenter.cpp)
  target_link_libraries(SurfaceMeshBarycenter pmp)

  add_custom_target(examples
    COMMAND
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "")

  add_dependencies(examples
    SurfaceMeshBasics
    SurfaceMeshIO
    SurfaceMeshIterators
    SurfaceMeshBarycenter
    )

endif(WITH_EXAMPLES)
