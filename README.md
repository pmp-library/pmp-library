<img src="docs/images/pmp-logo-text.png" alt="logo" width="750px"/>

[![Build Status](https://travis-ci.org/pmp-library/pmp-library.svg?branch=master)](https://travis-ci.org/pmp-library/pmp-library)
[![Build status](https://ci.appveyor.com/api/projects/status/48ipcc9q7u09adn5?branch=master&svg=true)](https://ci.appveyor.com/project/dsieger/pmp-library)
[![Coverage Status](https://coveralls.io/repos/github/pmp-library/pmp-library/badge.svg?branch=master)](https://coveralls.io/github/pmp-library/pmp-library?branch=master)

The pmp-library is a modern C++ open-source library for digital geometry
processing. See the [home page](http://www.pmp-library.org) for details.

## Quickstart

Fetch the repository:

    $ git clone --recursive https://github.com/pmp-library/pmp-library.git

Configure and build:

    $ cd pmp-library && mkdir build && cd build && cmake .. && make

Run the mesh processing app:

    $ ./mpview ../external/pmp-data/off/bunny.off
