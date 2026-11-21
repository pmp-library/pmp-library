if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ansi")
endif()

add_library(stb_image_write STATIC stb_image_write.cpp stb_image_write.h)
set_target_properties(stb_image_write PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(stb_image_write PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
