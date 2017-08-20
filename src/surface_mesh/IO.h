//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011-2016 by Graphics & Geometry Group, Bielefeld University
// Copyright (C) 2017 Daniel Sieger
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public License
// as published by the Free Software Foundation, version 2.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================
#pragma once
//=============================================================================

#include <surface_mesh/SurfaceMesh.h>

#include <string>


//== NAMESPACE ================================================================


namespace surface_mesh {


//=============================================================================


bool readMesh(SurfaceMesh& mesh, const std::string& filename);
bool readOFF(SurfaceMesh& mesh, const std::string& filename);
bool readOBJ(SurfaceMesh& mesh, const std::string& filename);
bool readPoly(SurfaceMesh& mesh, const std::string& filename);
bool readSTL(SurfaceMesh& mesh, const std::string& filename);

bool writeMesh(const SurfaceMesh& mesh, const std::string& filename);
bool writeOFF(const SurfaceMesh& mesh, const std::string& filename);
bool writeOBJ(const SurfaceMesh& mesh, const std::string& filename);
bool writePoly(const SurfaceMesh& mesh, const std::string& filename);
bool writeSTL(const SurfaceMesh& mesh, const std::string& filename);


//=============================================================================
} // namespace surface_mesh
//=============================================================================
