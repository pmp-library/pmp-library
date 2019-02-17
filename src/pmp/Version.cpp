//=============================================================================
// Copyright (C) 2019 The pmp-library developers
// Copyright (c) 2010 Martin Reddy. All rights reserved.
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include <pmp/Version.h>

#include <sstream>
#include <string>
#include <set>

//=============================================================================

namespace pmp {

//=============================================================================

int Version::get_major()
{
    return int(PMP_MAJOR_VERSION);
}

int Version::get_minor()
{
    return int(PMP_MINOR_VERSION);
}

int Version::get_patch()
{
    return int(PMP_PATCH_VERSION);
}

std::string Version::get_version()
{
    static std::string version("");

    if (version.empty())
    {
        // cache the version string
        std::ostringstream stream;
        stream << PMP_MAJOR_VERSION << "." << PMP_MINOR_VERSION << "."
               << PMP_PATCH_VERSION;
        version = stream.str();
    }

    return version;
}

bool Version::is_at_least(int major, int minor, int patch)
{
    if (PMP_MAJOR_VERSION < major)
        return false;
    if (PMP_MAJOR_VERSION > major)
        return true;
    if (PMP_MINOR_VERSION < minor)
        return false;
    if (PMP_MINOR_VERSION > minor)
        return true;
    if (PMP_PATCH_VERSION < patch)
        return false;
    return true;
}

bool Version::has_feature(const std::string& name)
{
    static std::set<std::string> features;

    if (features.empty())
    {
        // cache the feature list
        // features.insert("XXX");
    }

    return features.find(name) != features.end();
}

//=============================================================================
} // namespace pmp
//=============================================================================
