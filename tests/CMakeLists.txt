set(GOOGLE_TEST_ROOT external/googletest/googletest CACHE STRING "Google Test root")

include_directories(
  ${PROJECT_SOURCE_DIR}/${GOOGLE_TEST_ROOT}
  ${PROJECT_SOURCE_DIR}/${GOOGLE_TEST_ROOT}/include
  )

set(GOOGLE_TEST_SOURCES
  ${PROJECT_SOURCE_DIR}/${GOOGLE_TEST_ROOT}/src/gtest-all.cc
  ${PROJECT_SOURCE_DIR}/${GOOGLE_TEST_ROOT}/src/gtest_main.cc
  )

add_library(googletest ${GOOGLE_TEST_SOURCES})

# set output path so that coverage testing works
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# copy test data
file(COPY data DESTINATION  ${CMAKE_CURRENT_BINARY_DIR})

# silence gtest sign comparison warnings
if((UNIX OR APPLE) AND CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
endif()

# build test runnger
add_executable(gtest_runner
  test_geometry_object.cpp
  test_point_set.cpp
  test_edge_set.cpp
  test_surface_mesh.cpp)

if(UNIX OR APPLE)
  target_link_libraries(gtest_runner surface_mesh googletest pthread)
elseif(WIN32)
  target_link_libraries(gtest_runner surface_mesh googletest)
endif()

add_dependencies(gtest_runner googletest)

# add runner as test
add_test(gtest_runner ${CMAKE_CURRENT_BINARY_DIR}/gtest_runner)
