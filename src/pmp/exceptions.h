// Copyright 2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include <stdexcept>

namespace pmp {

//! \addtogroup core
//! @{

//! \brief Exception indicating invalid input passed to a function.
//! \details This exception should be used to signal violation of a
//! precondition, e.g., if an algorithm expects a triangle mesh but a
//! general polygon mesh is passed instead.
class InvalidInputException : public std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

//! \brief Exception indicating failure so solve an equation system.
class SolverException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

//! \brief Exception indicating failure to allocate a new resource.
//! \details This exception signals an error resulting from an attempt to exceed
//! implementation-defined allocation limits.
class AllocationException : public std::length_error
{
    using std::length_error::length_error;
};

//! \brief Exception indicating a topological error has occurred.
class TopologyException : public std::logic_error
{
    using std::logic_error::logic_error;
};

//! \brief Exception indicating an error occurred while performing IO.
class IOException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

//! \brief Exception indicating an OpenGL error.
class GLException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

//! @}

} // namespace pmp
