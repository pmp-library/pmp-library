file(GLOB_RECURSE SRCS ./*.cpp)
file(GLOB_RECURSE HDRS ./*.h)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ansi")
endif()

add_library(stb_image STATIC ${HDRS} ${SRCS})
set_target_properties(stb_image PROPERTIES POSITION_INDEPENDENT_CODE ON)

# disable deprecation warnings until https://github.com/nothings/stb/issues/1446
# is fixed
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  target_compile_options(stb_image PRIVATE "-Wno-deprecated-declarations")
endif()
