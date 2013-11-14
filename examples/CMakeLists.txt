if(WITH_EXAMPLES)

  add_executable(surface_mesh_basics surface_mesh_basics.cpp)
  target_link_libraries(surface_mesh_basics surface_mesh)

  add_executable(surface_mesh_io surface_mesh_io.cpp)
  target_link_libraries(surface_mesh_io surface_mesh)

  add_executable(surface_mesh_iterators surface_mesh_iterators.cpp)
  target_link_libraries(surface_mesh_iterators surface_mesh)

  add_executable(surface_mesh_barycenter surface_mesh_barycenter.cpp)
  target_link_libraries(surface_mesh_barycenter surface_mesh)

  add_custom_target(examples
    COMMAND
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "")

  add_dependencies(examples
    surface_mesh_basics
    surface_mesh_io
    surface_mesh_iterators
    surface_mesh_barycenter
    )

endif(WITH_EXAMPLES)



