# Installation {#installation}

# System Requirements {#system_requirements}

pmp uses [CMake](http://www.cmake.org) as its build configuration
system. Version 3.1 or greater is required.

pmp requires a C++11-compliant compiler. For the current release pmp has
been tested to build with the following compilers:

Operating System | Compiler
-----------------|--------------------
Linux            | gcc version 4.8.4
Mac OS-X         | LLVM version 5.0
Windows          | Visual Studio 2012

# Configuration {#configuration}

pmp relies on [CMake](http://www.cmake.org) as its build and configuration
system. `CMake` is a cross-platform build-system capable of generating different
build files (so-called _generators_) for a specific platform, e.g., Makefiles
for Linux/Unix, Xcode projects for Mac OS-X and Visual Studio projects for
Windows.

On the command line change to the top-level pmp directory, create a
build directory and run `cmake`:

    $ cd pmp
    $ mkdir build
    $ cd build
    $ cmake ..

The configuration procedure can be fine-tuned by specifying flags using the `-D`
option of `cmake`:

    $ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/g++

The command above would configure `CMake` to use release mode as its build type
and `/usr/bin/g++` as its C++ compiler.

In order to compile the included examples configure `CMake` with

    $ cmake -DWITH_EXAMPLES=true ..

Commonly used flags are shown below.

Flag                 | Description
---------------------|-------------------------------------------------
`CMAKE_BUILD_TYPE`   | Specify the build type, e.g. Debug or Release.
`CMAKE_CXX_COMPILER` | Specify the compiler to be used.
`CMAKE_CXX_FLAGS`    | Specify additional compiler flags, e.g. `-DNDEBUG`

For additional information on using `CMake` and
customizing its configuration see
the [CMake documentation](http://cmake.org/cmake/help/documentation.html).


# Building pmp {#building}

After successful configuration pmp can be build using
the chosen build system. For a Unix-like environment the default
generator is Makefiles. In order to build pmp just call

    $ make

from the top-level build directory. In order to build
pmp in parallel use the `-j` option of
`make`:

    $ make -j

The resulting library is named <code>pmp.so</code> and
located in the current working directory.

In order to build the full HTML manual and reference
documentation call

    $ make doxygen

The resulting HTML documentation can be found in the `doc/` sub-directory.

# Installation {#make_installation}

In order to install pmp just call

    $ sudo make install

Upon installation, both the library and headers will be installed to the
directory given via `CMAKE_INSTALL_PREFIX`, which defaults to `/usr/local/` on
Unix-like systems. If you need to install to a custom location set the install
prefix during build configuration:

    $ cmake -DCMAKE_INSTALL_PREFIX=<your custom path> ..

# Build Options {#build_options}

By default, pmp uses 32-bit unsigned integers as internal index type to
reference entities. However, if you need to process very large data sets this
might not be sufficient. In this case, you can change the index type to be
64-bit by specifying

    $ cmake -DPMP_INDEX_TYPE=64

during build configuration.
