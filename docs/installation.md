# Installation {#installation}

In this section, we describe how to configure, build, and install the
pmp-library in detail.

## System Requirements

The pmp-library uses [CMake](http://www.cmake.org) as its build configuration
system. Version 3.0.2 or greater is required. The pmp-library requires a
C++11-compliant compiler. We continuously build and test the pmp-library
with the following compilers and operating systems:

| Operating System | Compiler                 |
| ---------------- | ------------------------ |
| Linux            | gcc 4.8.4, clang 3.9.0   |
| Mac OS-X         | AppleClang 8.1.0         |
| Windows          | Visual Studio 2015, 2017 |

## Dependencies

Some parts of the pmp-library depends on the following third-party libraries:

| Library                                             | Description                       | Version    |
| --------------------------------------------------- | --------------------------------- | ---------- |
| [Eigen](http://eigen.tuxfamily.org)                 | C++ linear algebra library        | &ge; 3.3.7 |
| [OpenGL](http://opengl.org)                         | Open Graphics Library             | &ge; 3.3   |
| [GLEW](http://glew.sourceforge.net)                 | OpenGL Extension Wrangler Library | &ge; 2.1.0 |
| [GLFW](http://glfw.org)                             | Graphics Library Framework        | &ge; 3.3   |
| [ImGui](https://github.com/ocornut/imgui)           | Immediate Mode GUI                | &ge; 1.70  |
| [Google Test](https://github.com/google/googletest) | C++ Test Framework                | &ge; 1.8.0 |

By default, we include the corresponding libraries using git submodules. Note
that OpenGL and related dependencies are optional. They are only needed if you
want to use the viewer classes. Google Test is optional as well and only
required if you want to run the unit test suite.

## Configuration

The pmp-library relies on [CMake](http://www.cmake.org) as its build and
configuration system. `CMake` is a cross-platform build-system capable of
generating different build files (so-called _generators_) for a specific
platform, e.g., Makefiles for Linux/Unix, Xcode projects for Mac OS-X and Visual
Studio projects for Windows.

On the command line change to the top-level pmp-library directory, create a
build directory and run `cmake`:

    cd pmp
    mkdir build
    cd build
    cmake ..

The configuration procedure can be fine-tuned by specifying flags using the `-D`
option of `cmake`:

    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/g++

The command above would configure `CMake` to use release mode as its build type
and `/usr/bin/g++` as its C++ compiler.

In order to compile the included examples configure `CMake` with

    cmake -DWITH_EXAMPLES=true ..

Commonly used flags are shown below.

| Flag                 | Description                                        |
| -------------------- | -------------------------------------------------- |
| `CMAKE_BUILD_TYPE`   | Specify the build type, e.g. Debug or Release.     |
| `CMAKE_CXX_COMPILER` | Specify the compiler to be used.                   |
| `CMAKE_CXX_FLAGS`    | Specify additional compiler flags, e.g. `-DNDEBUG` |

For additional information on using `CMake` and
customizing its configuration see
the [CMake documentation](http://cmake.org/cmake/help/documentation.html).

## Building

After successful configuration pmp-library can be build using the chosen build
system. For a Unix-like environment the default generator is Makefiles. In order
to build pmp-library just call

    make

from the top-level build directory. In order to build pmp in parallel use the
`-j` option of `make`:

    make -j

The resulting library is named `libpmp.so` and
located in the current working directory.

In order to build the full HTML manual and reference documentation call

    make doxygen

The resulting HTML documentation can be found in the `docs/html/` sub-directory.
Note: this requires [Doxygen](http://www.doxygen.nl/) to be installed. In order
to generate proper bibliographical references please install
[BibTex](http://www.bibtex.org/) as well.

## Installation

In order to install pmp-library just call

    sudo make install

Upon installation, both the library and headers will be installed to the
directory given via `CMAKE_INSTALL_PREFIX`, which defaults to `/usr/local/` on
Unix-like systems. If you need to install to a custom location set the install
prefix during build configuration:

    cmake -DCMAKE_INSTALL_PREFIX=<your custom path> ..

The library can be uninstalled using

    make uninstall

To use the pmp-library in your own CMake-based projects simply include the
library by using `find_package(pmp)` and point CMake to the directory containing
the pmp-library CMake configuration file `pmpConfig.cmake`. This can be either
the pmp-library build directory

    cmake -Dpmp_DIR=<path-to-pmp-build-directory>

or the installed version

    cmake -Dpmp_DIR=<your custom path>/lib/cmake/pmp

This way, you can simply link your own target against pmp-library

    target_link_libraries(your_target pmp)

**Note:** The usage described above is currently limited to the @ref core and
@ref algorithms modules of the pmp-library. If you want to use the @ref
visualization module you need to link your target against `pmp_vis` and its
dependencies: `stb_image`, `imgui`, `glfw`, `glew`, as well as your platform
OpenGL library. In this case, however, we recommend to use the [project
template](https://github.com/pmp-library/pmp-template). See also
[Quickstart](quickstart.html).

## Build Options

### Index Type

By default, the pmp-libray uses 32-bit unsigned integers as internal index type
to reference entities. However, if you need to process very large data sets this
might not be sufficient. In this case, you can change the index type to be
64-bit by specifying

    cmake -DPMP_INDEX_TYPE=64

during build configuration.

### Scalar Type

By default, the pmp-library uses `float` as `Scalar` type. In case you require
higher floating point precision you can change the `Scalar` type to `double` by
specifying

    cmake -DPMP_SCALAR_TYPE=64

during build configuration.

## Building Bundled JavaScript Applications

In order to build the JavaScript applications
using [emscripten](https://github.com/kripken/emscripten), download the SDK
from <https://github.com/kripken/emscripten> and follow the installation
instructions.

Next, source the environment setup script:

    source <path_to_install_dir>/emsdk_env.sh

Create a build directory, run cmake, build, enjoy:

    mkdir jsbuild
    cd jsbuild
    emconfigure cmake ..
    make
    <your-browser> mview.html
