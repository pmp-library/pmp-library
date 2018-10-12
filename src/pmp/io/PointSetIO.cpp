//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================

#include <pmp/io/PointSetIO.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <clocale>
#include <cfloat>
#include <cstring>

//=============================================================================

namespace pmp {

//=============================================================================

bool PointSetIO::read(PointSet& ps, const std::string& filename)
{
    std::setlocale(LC_NUMERIC, "C");

    // clear mesh before reading from file
    ps.clear();

    // extract file extension
    std::string::size_type dot(filename.rfind("."));
    if (dot == std::string::npos)
        return false;
    std::string ext = filename.substr(dot + 1, filename.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "xyz")
    {
        return readXYZ(ps, filename);
    }
    else if (ext == "agi")
    {
        return readAGI(ps, filename);
    }

    // we didn't find a reader module
    return false;
}

//-----------------------------------------------------------------------------

bool PointSetIO::write(const PointSet& ps, const std::string& filename)
{
    // extract file extension
    std::string::size_type dot(filename.rfind("."));
    if (dot == std::string::npos)
        return false;
    std::string ext = filename.substr(dot + 1, filename.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "xyz")
    {
        return writeXYZ(ps, filename);
    }

    // we didn't find a writer module
    return false;
}

//-----------------------------------------------------------------------------

bool PointSetIO::readXYZ(PointSet& ps, const std::string& filename)
{
    // open file (in ASCII mode)
    FILE* in = fopen(filename.c_str(), "r");
    if (!in)
        return false;

    // add normal property
    auto vnormal = ps.vertexProperty<Normal>("v:normal");

    char line[200];
    float x, y, z;
    float nx, ny, nz;
    int n;
    PointSet::Vertex v;

    // read data
    while (in && !feof(in) && fgets(line, 200, in))
    {
        n = sscanf(line, "%f %f %f %f %f %f", &x, &y, &z, &nx, &ny, &nz);
        if (n >= 3)
        {
            v = ps.addVertex(Point(x, y, z));
            if (n >= 6)
            {
                vnormal[v] = Normal(nx, ny, nz);
            }
        }
    }

    fclose(in);
    return true;
}

//-----------------------------------------------------------------------------

bool PointSetIO::readAGI(PointSet& ps, const std::string& filename)
{
    // open file (in ASCII mode)
    FILE* in = fopen(filename.c_str(), "r");
    if (!in)
        return false;

    // add normal property
    auto normal = ps.vertexProperty<Normal>("v:normal");
    auto color = ps.vertexProperty<Color>("v:color");

    char line[200];
    float x, y, z;
    float nx, ny, nz;
    float r, g, b;
    int n;
    PointSet::Vertex v;

    // read data
    while (in && !feof(in) && fgets(line, 200, in))
    {
        n = sscanf(line, "%f %f %f %f %f %f %f %f %f", &x, &y, &z, &r, &g, &b,
                   &nx, &ny, &nz);
        if (n == 9)
        {
            v = ps.addVertex(Point(x, y, z));
            normal[v] = Normal(nx, ny, nz);
            color[v] = Color(r / 255.0, g / 255.0, b / 255.0);
        }
    }

    fclose(in);
    return true;
}

//-----------------------------------------------------------------------------

bool PointSetIO::writeXYZ(const PointSet& ps, const std::string& filename)
{
    std::ofstream ofs(filename);
    if (!ofs)
        return false;

    auto vnormal = ps.getVertexProperty<Normal>("v:normal");
    for (auto v : ps.vertices())
    {
        ofs << ps.position(v);
        ofs << " ";
        if (vnormal)
        {
            ofs << vnormal[v];
        }
        ofs << std::endl;
    }

    ofs.close();
    return true;
}

//=============================================================================
} // namespace pmp
//=============================================================================
