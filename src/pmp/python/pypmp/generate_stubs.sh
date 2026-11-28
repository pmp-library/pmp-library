#!/bin/bash

export PYTHONPATH=$PYTHONPATH:$(pwd):$(pwd)/io:$(pwd)/algorithms

# pypmp stubs
pybind11-stubgen pypmp -o .

# io stubs
cd io
pybind11-stubgen pmp_io -o .
cd ..

# algorithms stubs
cd algorithms
pybind11-stubgen algorithms -o .