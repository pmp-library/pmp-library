//=============================================================================
// Copyright (C) 2017-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup io io
//! @{

//=============================================================================

//! Common IO options for reading and writing
class IOOptions
{
public:
    //! Default constructor with all options set to false.
    IOOptions(bool binary = false, bool normals = false, bool colors = false,
              bool texcoords = false)
        : binary_(binary),
          normals_(normals),
          colors_(colors),
          texcoords_(texcoords){};

    //! Read / write binary files
    bool do_binary() { return binary_; };

    //! Read / write normals
    bool do_normals() { return normals_; };

    //! Read / write colors
    bool do_colors() { return colors_; };

    //! Read / write texcoords
    bool do_texcoords() { return texcoords_; };

private:
    bool binary_;
    bool normals_;
    bool colors_;
    bool texcoords_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
