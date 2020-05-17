// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <cstdint>

#include <stdexcept>

#include "pmp/MatVec.h"

//! \def PMP_ASSERT(x)
//! Custom assert macro that allows to silence unused variable warnings with no
//! overhead. Generates no code in release mode since if the argument to
//! sizeof() is an expression it is not evaluated. In debug mode we just fall
//! back to the default assert().
#ifdef NDEBUG
#define PMP_ASSERT(x)    \
    do                   \
    {                    \
        (void)sizeof(x); \
    } while (0)
#else
#define PMP_ASSERT(x) assert(x)
#endif

//! \def PMP_SHOW(x)
//! A simple pretty-printing debug macro. Prints the expression and its value.
#define PMP_SHOW(x) std::cerr << #x " = '" << x << "'" << std::endl;

//! The pmp-library namespace
namespace pmp {

//! \addtogroup core
//! @{

//! Scalar type
#ifdef PMP_SCALAR_TYPE_64
typedef double Scalar;
#else
typedef float Scalar;
#endif

//! Point type
typedef Vector<Scalar, 3> Point;

//! Normal type
typedef Vector<Scalar, 3> Normal;

//! Color type
typedef Vector<Scalar, 3> Color;

//! Texture coordinate type
typedef Vector<Scalar, 2> TexCoord;

// define index type to be used
#ifdef PMP_INDEX_TYPE_64
typedef std::uint_least64_t IndexType;
#define PMP_MAX_INDEX UINT_LEAST64_MAX
#else
typedef std::uint_least32_t IndexType;
#define PMP_MAX_INDEX UINT_LEAST32_MAX
#endif

//! Common IO flags for reading and writing
struct IOFlags
{
    IOFlags() {}
    bool use_binary = false;             //!< read / write binary format
    bool use_vertex_normals = false;     //!< read / write vertex normals
    bool use_vertex_colors = false;      //!< read / write vertex colors
    bool use_vertex_texcoords = false;   //!< read / write vertex texcoords
    bool use_face_normals = false;       //!< read / write face normals
    bool use_face_colors = false;        //!< read / write face colors
    bool use_halfedge_texcoords = false; //!< read / write halfedge texcoords
};

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

//! @}

//! \defgroup core core
//! \brief Core data structure and utilities.

//! \defgroup algorithms algorithms
//! \brief Geometry processing algorithms.

//! \defgroup visualization visualization
//! \brief Visualization tools using OpenGL.

} // namespace pmp
