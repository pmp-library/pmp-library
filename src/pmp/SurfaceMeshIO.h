// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <string>

#include "pmp/Types.h"
#include "pmp/SurfaceMesh.h"

namespace pmp {

class SurfaceMeshIO
{
public:
    SurfaceMeshIO(const std::string& filename, const IOFlags& flags)
        : filename_(filename), flags_(flags)
    {
    }

    bool read(SurfaceMesh& mesh);

    bool write(const SurfaceMesh& mesh);

private:
    bool read_off(SurfaceMesh& mesh);
    bool read_obj(SurfaceMesh& mesh);
    bool read_stl(SurfaceMesh& mesh);
    bool read_ply(SurfaceMesh& mesh);
    bool read_pmp(SurfaceMesh& mesh);
    bool read_xyz(SurfaceMesh& mesh);
    bool read_agi(SurfaceMesh& mesh);

    bool write_off(const SurfaceMesh& mesh);
    bool write_off_binary(const SurfaceMesh& mesh);
    bool write_obj(const SurfaceMesh& mesh);
    bool write_stl(const SurfaceMesh& mesh);
    bool write_ply(const SurfaceMesh& mesh);
    bool write_pmp(const SurfaceMesh& mesh);
    bool write_xyz(const SurfaceMesh& mesh);

private:
    std::string filename_;
    IOFlags flags_;
};

} // namespace pmp
