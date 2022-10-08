// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/io.h"

#include "pmp/io/read_obj.h"
#include "pmp/io/read_off.h"
#include "pmp/io/read_pmp.h"
#include "pmp/io/read_stl.h"
#include "pmp/io/write_obj.h"
#include "pmp/io/write_off.h"
#include "pmp/io/write_pmp.h"
#include "pmp/io/write_stl.h"

namespace pmp {

void read(SurfaceMesh& mesh, const std::filesystem::path& file)
{
    // clear mesh before reading from file
    mesh.clear();

    // extension determines reader
    auto ext = file.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".obj")
        read_obj(mesh, file);
    else if (ext == ".off")
        read_off(mesh, file);
    else if (ext == ".pmp")
        read_pmp(mesh, file);
    else if (ext == ".stl")
        read_stl(mesh, file);
    else
        throw IOException("Could not find reader for " + file.string());
}

void write(const SurfaceMesh& mesh, const std::filesystem::path& file,
           const IOFlags& flags)
{
    auto ext = file.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == ".obj")
        write_obj(mesh, file, flags);
    else if (ext == ".off")
        write_off(mesh, file, flags);
    else if (ext == ".pmp")
        write_pmp(mesh, file, flags);
    else if (ext == ".stl")
        write_stl(mesh, file, flags);
    else
        throw IOException("Could not find writer for " + file.string());
}

} // namespace pmp
