file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)

if(EMSCRIPTEN)
  add_library(pmp STATIC ${SOURCES} ${HEADERS})
else()
  add_library(pmp ${SOURCES} ${HEADERS})
  set_property(TARGET pmp PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

target_compile_features(pmp PUBLIC cxx_std_20)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(pmp PUBLIC OpenMP::OpenMP_CXX)
endif()

set_target_properties(pmp PROPERTIES VERSION ${PROJECT_VERSION})

if(WITH_CLANG_TIDY)
  set_target_properties(pmp PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

if(NOT EMSCRIPTEN AND PMP_INSTALL)

  target_include_directories(
    pmp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
               $<INSTALL_INTERFACE:include/>)
  target_include_directories(pmp SYSTEM
                             PUBLIC $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIR}>)

  install(
    TARGETS pmp
    EXPORT pmpTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES
    DESTINATION include)

  install(FILES ${HEADERS} DESTINATION include/pmp/)
else()
  target_include_directories(pmp PUBLIC ${EIGEN3_INCLUDE_DIR}
                                        ${CMAKE_CURRENT_SOURCE_DIR}/../)
endif()

if(PMP_BUILD_VIEWERS)
  add_subdirectory(viewers)
endif()

include(algorithms/CMakeLists.txt)
include(io/CMakeLists.txt)

if(CMAKE_C_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION
                                         GREATER_EQUAL 15)
  set_source_files_properties(
    algorithms/decimation.cpp
    PROPERTIES COMPILE_FLAGS "-Wno-array-bounds -Wno-stringop-overflow")
endif()
