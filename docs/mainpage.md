# Introduction {#mainpage}

The pmp-library is a modern C++ open-source library for digital geometry
processing. It provides a set of geometric data structures frequently used
in geometry processing tasks as well as implementations of canonical
algorithms. It has been designed and implemented with a focus on ease of
use and performance while maintaining high flexibility. In addition, it provides
mesh viewers and visualization utilities based on OpenGL&reg; as well as the
possibility to compile the library into JavaScript
using [emscripten](https://github.com/kripken/emscripten).

The [User Guide](userguide.html) describes how to
quickly [get started](quickstart.html) using the pmp-library, provides
an [overview](overview.html) of the library and its capabilities,
a [tutorial](tutorial.html) as well as guidelines
for [contributing](contributing.html) patches or new code.

The [Reference Documentation](annotated.html) provides detailed information on
the classes and functions provided by the pmp-library.

## License

The pmp-library is provided under a flexible 3-clause BSD
[license](https://github.com/pmp-library/pmp-library/blob/master/LICENSE.txt),
thereby allowing for both open-source and commercial usage.

## Acknowledgment

If you are using the pmp-library for research projects, please acknowledge its
use by referencing

    @misc{pmp-library,
      title  = {The Polygon Mesh Processing Library},
      author = {Daniel Sieger and Mario Botsch},
      note   = {http://pmp-library.github.io/pmp-library/},
      year   = {2018},
    }

For references to specific algorithms implemented in the library check out
the [bibliography](citelist.html).

## Heritage

The pmp-library evolved from the `Surface_mesh` data structure developed by
the [Bielefeld Graphics & Geometry Group](http://graphics.uni-bielefeld.de),
which in turn was originally derived from [OpenMesh](http://www.openmesh.org).
