---
layout: post
title: Version 1.0 Released!
---

We are proud to announce the first official release of the Polygon Mesh
Processing Library! Highlights include:

- A simple and efficient mesh data structure for storing and processing
  polygonal surface meshes
- Canonical geometry processing algorithms such as simplification, remeshing,
  subdivision, smoothing, or curvature computation
- Ready-to-use visualization tools to build your own mesh processing
  applications
- Compilation into JavaScript to build browser-based applications
  ([demo](https://www.pmp-library.org/mpview.html))

See the
[changelog](https://github.com/pmp-library/pmp-library/blob/master/CHANGELOG.md)
for a high-level summary of changes.

Get your own copy by cloning:

    $ git clone --recursive https://github.com/pmp-library/pmp-library.git

Checkout the release tag:

    $ cd pmp-library && git checkout 1.0.0

Configure and build:

    $ mkdir build && cd build && cmake .. && make

Run the mesh processing app

    $ ./mpview ../external/pmp-data/off/bunny.off

If you encounter any glitches or problems
please [report the issue](https://github.com/pmp-library/pmp-library/issues) on
our GitHub issue tracker.
