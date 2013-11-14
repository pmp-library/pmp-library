//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011-2013 by Graphics & Geometry Group, Bielefeld University
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


//== INCLUDES =================================================================

#include <surface_mesh/IO.h>

#include <clocale>


//== NAMESPACE ================================================================


namespace surface_mesh {


//== IMPLEMENTATION ===========================================================


bool read_mesh(Surface_mesh& mesh, const std::string& filename)
{
    std::setlocale(LC_NUMERIC, "C");

    // clear mesh before reading from file
    mesh.clear();

    // extract file extension
    std::string::size_type dot(filename.rfind("."));
    if (dot == std::string::npos) return false;
    std::string ext = filename.substr(dot+1, filename.length()-dot-1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "off")
    {
        return read_off(mesh, filename);
    }
    else if (ext == "obj")
    {
        return read_obj(mesh, filename);
    }
    else if (ext == "stl")
    {
        return read_stl(mesh, filename);
    }

    // we didn't find a reader module
    return false;
}


//-----------------------------------------------------------------------------


bool write_mesh(const Surface_mesh& mesh, const std::string& filename)
{
    // extract file extension
    std::string::size_type dot(filename.rfind("."));
    if (dot == std::string::npos) return false;
    std::string ext = filename.substr(dot+1, filename.length()-dot-1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);


    // extension determines reader
    if (ext == "off")
    {
        return write_off(mesh, filename);
    }
    else if(ext=="obj")
    {
        return write_obj(mesh, filename);
    }

    // we didn't find a writer module
    return false;
}


//=============================================================================
} // namespace surface_mesh
//=============================================================================
