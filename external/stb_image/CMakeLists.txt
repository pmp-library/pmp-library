file(GLOB_RECURSE SRCS ./*.cpp)
file(GLOB_RECURSE HDRS ./*.h)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ansi")
endif()

add_library(stb_image STATIC ${HDRS} ${SRCS})
set_target_properties(stb_image PROPERTIES POSITION_INDEPENDENT_CODE ON)

