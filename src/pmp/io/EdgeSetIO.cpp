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

#include <pmp/io/EdgeSetIO.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>

//=============================================================================

namespace pmp {

//=============================================================================

std::string getExtension(const std::string& filename)
{
    // check file extension
    std::string::size_type dot(filename.rfind("."));
    if (dot == std::string::npos)
        return std::string("");

    std::string ext = filename.substr(dot + 1, filename.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
    return ext;
}

//-----------------------------------------------------------------------------

bool EdgeSetIO::read(EdgeSet& es, const std::string& filename)
{
    auto ext = getExtension(filename);

    if (ext == "")
    {
        return false;
    }
    else if (ext == "knt")
    {
        return readKNT(es, filename);
    }

    return false;
}

//-----------------------------------------------------------------------------

bool EdgeSetIO::write(const EdgeSet& es, const std::string& filename)
{
    auto ext = getExtension(filename);

    if (ext == "")
    {
        return false;
    }
    else if (ext == "knt")
    {
        return writeKNT(es, filename);
    }

    return false;
}

//-----------------------------------------------------------------------------

bool EdgeSetIO::readKNT(EdgeSet& es, const std::string& filename)
{
    // open file
    FILE* in = fopen(filename.c_str(), "r");
    if (!in)
        return false;

    // clear edge set
    es.clear();

    char line[100], dummy[9];
    unsigned int nV, nE;
    unsigned int i, items;
    int idx_i, idx_j;
    Point p;

    // skip every line before "vertices..."
    do
    {
        if (fgets(line, 100, in) != nullptr)
        {
            items = sscanf(line, "%s", dummy);
        }
    } while (std::strncmp(dummy, "vertices", 8));

    // read #vertices, #edges
    items = sscanf(line, "%s %u %s %u", dummy, &nV, dummy, &nE);
    PMP_ASSERT(items > 0);

    // read vertices
    for (i = 0; i < nV && !feof(in); ++i)
    {
        if (fgets(line, 100, in) != nullptr)
        {
            items = sscanf(line, "%f %f %f", &p[0], &p[1], &p[2]);
            es.addVertex(p);
        }
    }

    using Vertex = EdgeSet::Vertex;

    // read edges
    for (i = 0; i < nE && !feof(in); ++i)
    {
        if (fgets(line, 100, in) != nullptr)
        {
            items = sscanf(line, "%i %i", &idx_i, &idx_j);
            es.insertEdge(Vertex(idx_i), Vertex(idx_j));
        }
    }

    fclose(in);

    return true;
}

//-----------------------------------------------------------------------------

bool EdgeSetIO::writeKNT(const EdgeSet& es, const std::string& filename)
{
    FILE* out = fopen(filename.c_str(), "w");
    if (!out)
        return false;

    // header
    fprintf(out, "%s %f\n", "time", 0.0);
    fprintf(out, "%s %zi %s %zi\n", "vertices", es.nVertices(), "edges",
            es.nEdges());

    // vertices
    for (auto v : es.vertices())
    {
        const Point p = es.position(v);
        fprintf(out, "%f %f %f\n", p[0], p[1], p[2]);
    }

    // edges
    for (auto e : es.edges())
    {
        const auto v0 = es.toVertex(es.halfedge(e, 0));
        const auto v1 = es.toVertex(es.halfedge(e, 1));
        fprintf(out, "%i %i\n", v0.idx(), v1.idx());
    }

    fclose(out);
    return true;
}

//=============================================================================
} // namespace pmp
//=============================================================================
