// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/io.h"

#include "pmp/io/read_obj.h"
#include "pmp/io/read_off.h"
#include "pmp/io/read_stl.h"
#include "pmp/io/write_obj.h"
#include "pmp/io/write_off.h"
#include "pmp/io/write_stl.h"

#include <clocale>

namespace pmp {

void read(SurfaceMesh& mesh, const std::string& filename)
{
    std::setlocale(LC_NUMERIC, "C");

    // clear mesh before reading from file
    mesh.clear();

    // extract file extension
    std::string::size_type dot(filename.rfind("."));
    if (dot == std::string::npos)
        throw IOException("Could not determine file extension!");
    std::string ext = filename.substr(dot + 1, filename.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "off")
        read_off(mesh, filename);
    else if (ext == "obj")
        read_obj(mesh, filename);
    else if (ext == "stl")
        read_stl(mesh, filename);
    else
        throw IOException("Could not find reader for " + filename);
}

void write(const SurfaceMesh& mesh, const std::string& filename,
           const IOFlags& flags)
{
    // extract file extension
    std::string::size_type dot(filename.rfind("."));
    if (dot == std::string::npos)
        throw IOException("Could not determine file extension!");
    std::string ext = filename.substr(dot + 1, filename.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "off")
        write_off(mesh, filename, flags);
    else if (ext == "obj")
        write_obj(mesh, filename, flags);
    else if (ext == "stl")
        write_stl(mesh, filename, flags);
    else
        throw IOException("Could not find writer for " + filename);
}

} // namespace pmp
