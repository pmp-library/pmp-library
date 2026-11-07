add_library(predicates OBJECT predicates.c predicates.h)

# hide symbols and dynamic linking support
set_target_properties(
  predicates
  PROPERTIES POSITION_INDEPENDENT_CODE ON
             C_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN ON)

# enforce strict floating-point behavior across platforms
if(MSVC)
  target_compile_options(predicates PRIVATE /fp:strict)
  target_compile_definitions(predicates PRIVATE random=rand)
else()
  # make sure fast math is disabled
  target_compile_options(predicates PRIVATE -fno-fast-math)

  # prevent 80-bit float register spilling differences
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|x86_64|AMD64")
    target_compile_options(predicates PRIVATE -ffloat-store)
  endif()
endif()

# silence deprecation warnings
target_compile_options(
  predicates
  PRIVATE
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:AppleClang>>:-Wno-deprecated
    -Wno-deprecated-non-prototype
    -Wno-old-style-definition
    -Wno-missing-prototypes>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4505> # unreferenced local functions
)
