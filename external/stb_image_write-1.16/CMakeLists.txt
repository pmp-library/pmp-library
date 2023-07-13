if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ansi")
endif()

add_library(stb_image_write STATIC stb_image_write.cpp stb_image_write.h)
set_target_properties(stb_image_write PROPERTIES POSITION_INDEPENDENT_CODE ON)

# disable deprecation warnings until https://github.com/nothings/stb/issues/1446
# is fixed
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  target_compile_options(stb_image_write PRIVATE "-Wno-deprecated-declarations")
endif()
