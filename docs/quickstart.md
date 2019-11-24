# Quickstart {#quickstart}

This section briefly describes how to get up and running using the
pmp-library. See [Installation](installation.html) for more detailed
information such as system requirements and different ways of installing and
using pmp-library.

## Building the Library

Fetch the repository:

    git clone --recursive https://github.com/pmp-library/pmp-library.git

Configure and build:

    cd pmp-library && mkdir build && cd build && cmake .. && make

Load and view a mesh:

    ./mview ../external/pmp-data/off/bunny.off

## Using the Project Template

We also provide a simple project template for writing your own algorithms and
applications using the pmp-library. It directly includes the pmp-library
repository as a git submodule. To get started, just clone the repository
recursively:

    git clone --recursive https://github.com/pmp-library/pmp-template.git

Configure and build:

    cd pmp-template && mkdir build && cd build && cmake .. && make

This will automatically build the pmp-library and its dependecies.
