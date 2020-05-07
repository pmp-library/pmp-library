<img src="docs/images/pmp-logo-text.png" alt="logo" width="750px"/>

[![Build Status](https://travis-ci.org/pmp-library/pmp-library.svg?branch=master)](https://travis-ci.org/pmp-library/pmp-library)
[![Build status](https://ci.appveyor.com/api/projects/status/48ipcc9q7u09adn5?branch=master&svg=true)](https://ci.appveyor.com/project/dsieger/pmp-library)
[![Coverage Status](https://coveralls.io/repos/github/pmp-library/pmp-library/badge.svg?branch=master)](https://coveralls.io/github/pmp-library/pmp-library?branch=master)

The Polygon Mesh Processing Library is a modern C++ open-source library for
digital geometry processing. It provides an efficient and easy to use mesh data
structure, implementations of canonical geometry processing algorithms, as well
as ready-to-use visualization tools for rapid prototyping.
See the [home page](http://www.pmp-library.org) for more information.

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

## Usage

```cpp
#include <pmp/SurfaceMesh.h>

int main(void)
{
    pmp::SurfaceMesh mesh;
    mesh.read("input.obj");
    // .. do something with your mesh
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

The pmp-library is provided under a simple and flexible
MIT-style
[license](https://github.com/pmp-library/pmp-library/blob/master/LICENSE.txt),
thereby allowing for both open-source and commercial usage.
