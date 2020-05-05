// Copyright 2017-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <cstddef>

namespace pmp {

//! A simple class to retrieve memory usage information.
//! \ingroup core
class MemoryUsage
{
public:
    //! \brief Get the maximum memory size the application has used so far.
    //! \return the max. resident set size (RSS) in bytes
    static size_t max_size();

    //! \brief Get the currently used memory.
    //! \return the current resident set size (RSS) in bytes
    static size_t current_size();
};

} // namespace pmp