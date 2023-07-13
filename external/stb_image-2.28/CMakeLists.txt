if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ansi")
endif()

add_library(stb_image STATIC stb_image.cpp stb_image.h)
set_target_properties(stb_image PROPERTIES POSITION_INDEPENDENT_CODE ON)
