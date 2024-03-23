set(GOOGLE_TEST_ROOT
    external/googletest-1.13.0/googletest
    CACHE STRING "Google Test root")

include_directories(${PROJECT_SOURCE_DIR}/${GOOGLE_TEST_ROOT}
                    ${PROJECT_SOURCE_DIR}/${GOOGLE_TEST_ROOT}/include)

set(GOOGLE_TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/${GOOGLE_TEST_ROOT}/src/gtest-all.cc
    ${PROJECT_SOURCE_DIR}/${GOOGLE_TEST_ROOT}/src/gtest_main.cc)

add_library(googletest STATIC ${GOOGLE_TEST_SOURCES})

# set output path so that coverage testing works
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# copy test data
file(COPY ${PROJECT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# source files
file(GLOB SOURCES ./*.cpp)

# build test runner
add_executable(gtest_runner ${SOURCES})

if(UNIX OR APPLE)
  target_link_libraries(gtest_runner pmp googletest pthread)
elseif(WIN32)
  target_link_libraries(gtest_runner pmp googletest)
endif()

add_dependencies(gtest_runner googletest)

# copy dlls on windows
if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET gtest_runner
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pmp>
            $<TARGET_FILE_DIR:gtest_runner>
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:googletest>
            $<TARGET_FILE_DIR:gtest_runner>)
endif()

# automatic test discovery
include(GoogleTest)
gtest_discover_tests(gtest_runner)

