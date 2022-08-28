// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <string>
#include <utility>

#include "pmp/Types.h"
#include "pmp/SurfaceMesh.h"

namespace pmp {

class SurfaceMeshIO
{
public:
    SurfaceMeshIO(std::string filename, IOFlags flags)
        : filename_(std::move(filename)), flags_(std::move(flags))
    {
    }

    void read(SurfaceMesh& mesh);

    void write(const SurfaceMesh& mesh);

private:
    void read_off(SurfaceMesh& mesh);
    void read_obj(SurfaceMesh& mesh);
    void read_stl(SurfaceMesh& mesh);
    void read_ply(SurfaceMesh& mesh);
    void read_pmp(SurfaceMesh& mesh);
    void read_xyz(SurfaceMesh& mesh);
    void read_agi(SurfaceMesh& mesh);

    void write_off(const SurfaceMesh& mesh);
    void write_off_binary(const SurfaceMesh& mesh);
    void write_obj(const SurfaceMesh& mesh);
    void write_stl(const SurfaceMesh& mesh);
    void write_ply(const SurfaceMesh& mesh);
    void write_pmp(const SurfaceMesh& mesh);
    void write_xyz(const SurfaceMesh& mesh);

    std::string filename_;
    IOFlags flags_;
};

} // namespace pmp
