if(WITH_BENCHMARKS)

  add_executable(performance performance.cpp)
  target_link_libraries(performance surface_mesh)

  add_custom_target(benchmarks
    COMMAND
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "")

  add_dependencies(benchmarks
    performance
    )

endif(WITH_BENCHMARKS)
