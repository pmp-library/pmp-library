// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
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

    //! \brief Wrapper around add_face() to catch any topology errors.
    //! \details Failed faces are stored so they can be added later.
    //! \return A valid Face *if* it could be added, invalid Face otherwise.
    Face add_face(SurfaceMesh& mesh, const std::vector<Vertex>& vertices);

    //! \brief Add failed faces after duplicating their vertices.
    //! \pre failed_faces_ contains only valid vertex indices.
    //! \post failed faces are added to the mesh and the vector is cleared.
    void add_failed_faces(SurfaceMesh& mesh);

    //! \brief Duplicate the given set of vertices by adding their points to the mesh again.
    //! \pre All input vertices are valid and already added to the mesh.
    //! \return A vector of duplicated vertices.
    std::vector<Vertex> duplicate_vertices(
        SurfaceMesh& mesh, const std::vector<Vertex>& vertices) const;

    void read_off_ascii(SurfaceMesh& mesh, FILE* in, const bool has_normals,
                        const bool has_texcoords, const bool has_colors);

    void read_off_binary(SurfaceMesh& mesh, FILE* in, const bool has_normals,
                         const bool has_texcoords, const bool has_colors);

private:
    std::string filename_;
    IOFlags flags_;
    std::vector<std::vector<Vertex>> failed_faces_;
};

} // namespace pmp
