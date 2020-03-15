//=============================================================================
// Copyright (C) 2019 The pmp-library developers
// Copyright (c) 2010 Martin Reddy. All rights reserved.
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================
#pragma once
//=============================================================================

#include <string>

#define PMP_MAJOR_VERSION 1
#define PMP_MINOR_VERSION 2
#define PMP_PATCH_VERSION 0

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup core core
//!@{

//! API version information
class Version
{
public:
    //! return the major version number
    static int get_major();

    //! return the minor version number
    static int get_minor();

    //! return the patch version number
    static int get_patch();

    //! return the current API version number as a string
    static std::string get_version();

    //! return true if the current version >= (major, minor, patch)
    static bool is_at_least(int major, int minor, int patch);

    //! return true if the named feature is available in this version
    static bool has_feature(const std::string &name);
};

//=============================================================================
//!@}
//=============================================================================
} // namespace pmp
//=============================================================================
