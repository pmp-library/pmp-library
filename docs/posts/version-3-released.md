# Version 3.0 Released {#version-3-0-released}

_Xxx xx, 2023_

\warning This is a draft

We just released version 3.0 of the Polygon Mesh Processing Library! This is a major release with several additions and API changes

- Several algorithms now work on general polygon meshes
- The algorithms API has been revamped to use a simple function-based interface
- We no longer use git submodules for handling third-party dependencies.
- Upgrade C++ standard to C++17
- ... and much more

See the [changelog](https://github.com/pmp-library/pmp-library/blob/master/CHANGELOG.md) for a full summary of changes.

## Obtaining PMP

Get your copy by cloning:

```sh
git clone https://github.com/pmp-library/pmp-library.git
```

Checkout the 3.0 release tag:

```sh
cd pmp-library && git checkout 3.0
```

Configure and build:

```sh
mkdir build && cd build && cmake .. && make
```

Run the mesh processing app

```sh
./mpview ../data/off/bunny.off
```

## Reporting Bugs or Problems

If you encounter any glitches or problems please [report the issue](https://github.com/pmp-library/pmp-library/issues) on GitHub.
