// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/SurfaceMeshIO.h"

#include "pmp/io/helpers.h"
#include "pmp/io/read_obj.h"
#include "pmp/io/read_off.h"
#include "pmp/io/read_stl.h"
#include "pmp/io/write_obj.h"
#include "pmp/io/write_off.h"
#include "pmp/io/write_stl.h"

#include <clocale>
#include <cstring>
#include <cctype>

#include <algorithm>
#include <map>
#include <fstream>
#include <limits>

namespace pmp {

void read(SurfaceMesh& mesh, const std::string& filename, const IOFlags& flags)
{
    SurfaceMeshIO reader(filename, flags);
    reader.read(mesh);
}

void write(const SurfaceMesh& mesh, const std::string& filename,
           const IOFlags& flags)
{
    SurfaceMeshIO writer(filename, flags);
    writer.write(mesh);
}

void SurfaceMeshIO::read(SurfaceMesh& mesh)
{
    std::setlocale(LC_NUMERIC, "C");

    // clear mesh before reading from file
    mesh.clear();

    // extract file extension
    std::string::size_type dot(filename_.rfind("."));
    if (dot == std::string::npos)
        throw IOException("Could not determine file extension!");
    std::string ext = filename_.substr(dot + 1, filename_.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "off")
        read_off(mesh, filename_);
    else if (ext == "obj")
        read_obj(mesh, filename_);
    else if (ext == "stl")
        read_stl(mesh, filename_);
    else
        throw IOException("Could not find reader for " + filename_);
}

void SurfaceMeshIO::write(const SurfaceMesh& mesh)
{
    // extract file extension
    std::string::size_type dot(filename_.rfind("."));
    if (dot == std::string::npos)
        throw IOException("Could not determine file extension!");
    std::string ext = filename_.substr(dot + 1, filename_.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "off")
        write_off(mesh, filename_, flags_);
    else if (ext == "obj")
        write_obj(mesh, filename_, flags_);
    else if (ext == "stl")
        write_stl(mesh, filename_, flags_);
    else
        throw IOException("Could not find writer for " + filename_);
}

} // namespace pmp
