add_executable(basics basics.cpp)
target_link_libraries(basics pmp)

add_executable(io io.cpp)
target_link_libraries(io pmp)

add_executable(iterators iterators.cpp)
target_link_libraries(iterators pmp)

add_executable(barycenter barycenter.cpp)
target_link_libraries(barycenter pmp)

add_custom_target(
  examples
  COMMAND
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  COMMENT "")

add_dependencies(examples basics io iterators barycenter)
