# Installation {#installation}

[TOC]

In this section, we describe how to configure, build, and install PMP in detail.

## System Requirements

PMP uses [CMake](http://www.cmake.org) as its build configuration system. Version 3.16.3 or greater is required. PMP requires a C++20-compliant compiler. We continuously build and test PMP with the following compilers and operating systems:

| Operating System | Compiler           |
| ---------------- | ------------------ |
| Linux            | gcc 11.4.0         |
| macOS            | AppleClang 14.0.0  |
| Windows          | Visual Studio 2022 |

We do not officially support older compiler versions.

## Dependencies

Some parts of PMP depend on the following third-party libraries:

| Library                                             | Description                       | Version     |
| --------------------------------------------------- | --------------------------------- | ----------- |
| [Eigen](http://eigen.tuxfamily.org)                 | C++ linear algebra library        | &ge; 3.4.0  |
| [OpenGL](http://opengl.org)                         | Open Graphics Library             | &ge; 3.3    |
| [GLEW](http://glew.sourceforge.net)                 | OpenGL Extension Wrangler Library | &ge; 2.1.0  |
| [GLFW](http://glfw.org)                             | Graphics Library Framework        | &ge; 3.4    |
| [ImGui](https://github.com/ocornut/imgui)           | Immediate Mode GUI                | &ge; 1.90.4 |
| [Google Test](https://github.com/google/googletest) | C++ Test Framework                | &ge; 1.13.0 |

By default, we include the corresponding libraries in our repository. Note that OpenGL and related dependencies are optional. They are only needed if you want to use the viewer classes. Google Test is optional as well and only required if you want to run the unit test suite.

## Configuration

PMP relies on [CMake](http://www.cmake.org) as its build and configuration system. `CMake` is a cross-platform build-system capable of generating different build files (so-called _generators_) for a specific platform, e.g., Makefiles for Linux/Unix, Xcode projects for macOS, and Visual Studio projects for Windows.

On the command line, change to the top-level PMP directory, create a build directory and run `cmake`:

```sh
cd pmp-library
mkdir build
cd build
cmake ..
```

The configuration procedure can be fine-tuned by specifying flags using the `-D` option of `cmake`:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/g++
```

The command above would configure `CMake` to use release mode as its build type and `/usr/bin/g++` as its C++ compiler.

Commonly used flags are shown below.

| Flag                 | Description                                        |
| -------------------- | -------------------------------------------------- |
| `CMAKE_BUILD_TYPE`   | Specify the build type, e.g. Debug or Release.     |
| `CMAKE_CXX_COMPILER` | Specify the compiler to be used.                   |
| `CMAKE_CXX_FLAGS`    | Specify additional compiler flags, e.g. `-DNDEBUG` |

For additional information on using `CMake` and customizing its configuration see the [CMake documentation](http://cmake.org/cmake/help/documentation.html).

## Building

After successful configuration, PMP can be build using the chosen build system. For a Unix-like environment the default generator is Makefiles. In order to build PMP just call

```sh
make
```

from the top-level build directory. In order to build pmp in parallel use the `-j` option of `make`:

```sh
make -j
```

The resulting library is named `libpmp.so` and located in the current working directory.

In order to build the full HTML manual and reference documentation call

```sh
make docs
```

The resulting HTML documentation can be found in the `docs/html/` sub-directory. Note: this requires [Doxygen](http://www.doxygen.nl/) to be installed. In order to generate bibliographical references you need to have [BibTex](http://www.bibtex.org/) installed.

## Installation

In order to install PMP just call

```sh
sudo make install
```

Upon installation, both the library and headers will be installed to the directory given via `CMAKE_INSTALL_PREFIX`, which defaults to `/usr/local/` on Unix-like systems. If you need to install to a custom location set the install prefix during build configuration:

```sh
cmake -DCMAKE_INSTALL_PREFIX=<your custom path> ..
```

The library can be uninstalled using

```sh
make uninstall
```

To use PMP in your own CMake-based projects simply include the library by using `find_package(pmp)` and point CMake to the directory containing PMP CMake configuration file `pmpConfig.cmake`. This can be either the PMP build directory

```sh
cmake -Dpmp_DIR=<path-to-pmp-build-directory>
```

or the installed version

```sh
cmake -Dpmp_DIR=<your custom path>/lib/cmake/pmp
```

This way, you can simply link your own target against PMP

```cmake
target_link_libraries(your_target pmp)
```

**Note:** The usage described above is currently limited to the @ref core and @ref algorithms modules of PMP. If you want to use the @ref visualization module you need to link your target against `pmp_vis` and its dependencies: `stb_image`, `imgui`, `glfw`, `glew`, as well as your platform OpenGL library.

## Build Options

### Index Type

By default, PMP uses 32-bit unsigned integers as internal index type to reference entities. However, if you need to process very large data sets this might not be sufficient. In this case, you can change the index type to be 64-bit by specifying

```sh
cmake -DPMP_INDEX_TYPE=64
```

during build configuration.

### Scalar Type

By default, PMP uses `float` as `Scalar` type. In case you require higher floating point precision you can change the `Scalar` type to `double` by specifying

```sh
cmake -DPMP_SCALAR_TYPE=64
```

during build configuration.
