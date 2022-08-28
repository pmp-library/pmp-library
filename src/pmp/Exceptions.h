// Copyright 2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <stdexcept>

namespace pmp {

//! \addtogroup core
//! @{

//! \brief Exception indicating invalid input passed to a function.
//! \details This exception should be used to signal violation of a
//! precondition, e.g., if an algorithm expects a pure triangle mesh but a
//! general polygon mesh is passed instead.
class InvalidInputException : public std::invalid_argument
{
public:
    InvalidInputException(const std::string& what) : std::invalid_argument(what)
    {
    }
};

//! \brief Exception indicating failure so solve an equation system.
class SolverException : public std::runtime_error
{
public:
    SolverException(const std::string& what) : std::runtime_error(what) {}
};

//! \brief Exception indicating failure to allocate a new resource.
//! \details This exception signals an error resulting from an attempt to exceed
//! implementation-defined allocation limits.
class AllocationException : public std::length_error
{
public:
    AllocationException(const std::string& what) : std::length_error(what) {}
};

//! \brief Exception indicating a topological error has occurred.
class TopologyException : public std::logic_error
{
public:
    TopologyException(const std::string& what) : std::logic_error(what) {}
};

//! \brief Exception indicating an error occurred while performing IO.
class IOException : public std::runtime_error
{
public:
    IOException(const std::string& what) : std::runtime_error(what) {}
};

//! @}

} // namespace pmp
