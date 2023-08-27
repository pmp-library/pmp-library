# Introduction

[![build](https://github.com/pmp-library/pmp-library/workflows/build/badge.svg)](https://github.com/pmp-library/pmp-library/actions?query=workflow%3Abuild)
[![Coverage Status](https://coveralls.io/repos/github/pmp-library/pmp-library/badge.svg?branch=master)](https://coveralls.io/github/pmp-library/pmp-library?branch=main)
[![Latest Release](https://img.shields.io/github/v/release/pmp-library/pmp-library?sort=semver)](https://github.com/pmp-library/pmp-library/releases/latest)

The Polygon Mesh Processing Library is a modern C++ open-source library for processing and visualizing polygon surface meshes. Its main features are:

- An efficient and easy-to-use mesh data structure
- Standard algorithms such as decimation, remeshing, subdivision, or smoothing
- Ready-to-use visualization tools
- Seamless cross-compilation to JavaScript ([demo](https://www.pmp-library.org/demos/mpview.html))

## Get Started

Clone the repository:

```sh
git clone https://github.com/pmp-library/pmp-library.git
```

Configure and build:

```sh
cd pmp-library && mkdir build && cd build && cmake .. && make
```

Run the mesh processing app:

```sh
./mpview ../data/off/bunny.off
```

Build your own tool:

```cpp
#include <pmp/surface_mesh.h>

int main(void)
{
    pmp::SurfaceMesh mesh;
    pmp::read(mesh,"input.obj");
    // .. do awesome things with your mesh
    pmp::write(mesh,"output.obj");
}
```

## Read the Docs

The [user guide](https://www.pmp-library.org/guide.html) contains all you need to get started using PMP, including a [tutorial](https://www.pmp-library.org/tutorial.html) covering mesh processing basics.

## Contribute

Contributions to PMP are welcome! There are many ways you can help: Report any [issues](https://github.com/pmp-library/pmp-library/issues) you find, help to improve the documentation, join our [discussions](https://github.com/pmp-library/pmp-library/discussions) forum, or [contribute](https://www.pmp-library.org/contributing.html) new code.

## Acknowledge

If you are using PMP for research projects, please acknowledge its use by referencing

```tex
@misc{pmp-library,
title  = {The Polygon Mesh Processing Library},
author = {Daniel Sieger and Mario Botsch},
note   = {http://www.pmp-library.org},
year   = {2019},
}
```

We acknowledge that PMP evolved from our previous work on [Surface_mesh](http://dx.doi.org/10.1007/978-3-642-24734-7_29) and [OpenMesh](https://pub.uni-bielefeld.de/record/1961694).

## License

PMP is provided under a simple and flexible MIT-style [license](https://github.com/pmp-library/pmp-library/blob/master/LICENSE.txt) allowing for both open-source and commercial usage.
