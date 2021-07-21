file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)

if(EMSCRIPTEN)
  add_library(pmp STATIC ${SOURCES} ${HEADERS} $<TARGET_OBJECTS:rply>)
else()
  add_library(pmp SHARED ${SOURCES} ${HEADERS} $<TARGET_OBJECTS:rply>)
  set_property(TARGET pmp PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(pmp PUBLIC OpenMP::OpenMP_CXX)
endif()

set_target_properties(pmp PROPERTIES VERSION ${PMP_VERSION})

if(CLANG_TIDY_EXE AND FALSE) # disabled by default
  set_target_properties(pmp PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

if(NOT EMSCRIPTEN AND PMP_INSTALL)

  target_include_directories(
    pmp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
               $<INSTALL_INTERFACE:include/>)

  if((UNIX OR APPLE) AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(pmp PUBLIC "-std=c++11")
  endif()

  install(
    TARGETS pmp
    EXPORT pmpTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES
    DESTINATION include)

  install(FILES ${HEADERS} DESTINATION include/pmp/)

endif()

if(PMP_BUILD_VIS)
  add_subdirectory(visualization)
endif()

include(algorithms/CMakeLists.txt)
