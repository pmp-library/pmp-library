# Introduction

[![build](https://github.com/pmp-library/pmp-library/workflows/build/badge.svg)](https://github.com/pmp-library/pmp-library/actions?query=workflow%3Abuild)
[![Coverage Status](https://coveralls.io/repos/github/pmp-library/pmp-library/badge.svg?branch=master)](https://coveralls.io/github/pmp-library/pmp-library?branch=master)

The Polygon Mesh Processing Library is a modern C++ open-source library for
processing and visualizing polygon surface meshes. Its main features are:

- An efficient and easy-to-use mesh data structure
- Canonical mesh processing algorithms
- Ready-to-use visualization tools
- Seamless cross-compilation to JavaScript ([demo](https://www.pmp-library.org/mpview.html))

## Getting Started

Clone the repository:

```sh
git clone --recursive https://github.com/pmp-library/pmp-library.git
```

Configure and build:

```sh
cd pmp-library && mkdir build && cd build && cmake .. && make
```

Run the mesh processing app:

```sh
./mpview ../external/pmp-data/off/bunny.off
```

Build your own tool:

```cpp
#include <pmp/SurfaceMesh.h>

int main(void)
{
    pmp::SurfaceMesh mesh;
    mesh.read("input.obj");
    // .. do awesome things with your mesh
    mesh.write("output.obj");
}
```

## Contributing

Contributions to the pmp-library are welcome. See the
[contributing](https://www.pmp-library.org/contributing.html) section of the
[user guide](https://www.pmp-library.org/userguide.html).

## Acknowledgment

If you are using the pmp-library for research projects, please acknowledge its
use by referencing

```tex
@misc{pmp-library,
title  = {The Polygon Mesh Processing Library},
author = {Daniel Sieger and Mario Botsch},
note   = {http://www.pmp-library.org},
year   = {2020},
}
```

## License

The pmp-library is provided under a simple and flexible MIT-style
[license](https://github.com/pmp-library/pmp-library/blob/master/LICENSE.txt),
thereby allowing for both open-source and commercial usage.
