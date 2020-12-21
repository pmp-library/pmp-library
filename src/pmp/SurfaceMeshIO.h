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

    bool read_off_ascii(SurfaceMesh& mesh, FILE* in, const bool has_normals,
                        const bool has_texcoords, const bool has_colors);

    bool read_off_binary(SurfaceMesh& mesh, FILE* in, const bool has_normals,
                         const bool has_texcoords, const bool has_colors);

private:
    std::string filename_;
    IOFlags flags_;
    std::vector<std::vector<Vertex>> failed_faces_;
};

} // namespace pmp
