//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011-2017 The pmp-library developers
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

#include <pmp/PointSet.h>
#include <pmp/io/PointSetIO.h>

//=============================================================================

namespace pmp {

//=============================================================================

PointSet::PointSet() : GeometryObject()
{
    // allocate standard properties
    // same list is used in operator=() and assign()
    m_vpoint = addVertexProperty<Point>("v:point");
    m_vdeleted = addVertexProperty<bool>("v:deleted", false);

    m_deletedVertices = 0;
    m_garbage = false;
}

//-----------------------------------------------------------------------------

PointSet::~PointSet() = default;

//-----------------------------------------------------------------------------

PointSet& PointSet::operator=(const PointSet& rhs)
{
    GeometryObject::operator=(rhs);

    if (this != &rhs)
    {
        // deep copy of property containers
        m_vprops = rhs.m_vprops;

        // property handles contain pointers, have to be reassigned
        m_vpoint = vertexProperty<Point>("v:point");
        m_vdeleted = vertexProperty<bool>("v:deleted");

        // how many elements are deleted?
        m_deletedVertices = rhs.m_deletedVertices;
        m_garbage = rhs.m_garbage;
    }

    return *this;
}

//-----------------------------------------------------------------------------

PointSet& PointSet::assign(const PointSet& rhs)
{
    GeometryObject::operator=(rhs);

    if (this != &rhs)
    {
        // clear properties
        m_vprops.clear();

        // allocate standard properties
        m_vpoint = addVertexProperty<Point>("v:point");
        m_vdeleted = addVertexProperty<bool>("v:deleted", false);

        // copy properties from other point set
        m_vpoint.array() = rhs.m_vpoint.array();
        m_vdeleted.array() = rhs.m_vdeleted.array();

        // resize (needed by property containers)
        m_vprops.resize(rhs.verticesSize());

        // how many elements are deleted?
        m_deletedVertices = rhs.m_deletedVertices;
        m_garbage = rhs.m_garbage;
    }

    return *this;
}

//-----------------------------------------------------------------------------

bool PointSet::read(const std::string& filename, const IOOptions& options)
{
    PointSetIO reader(options);
    return reader.read(*this, filename);
}

//-----------------------------------------------------------------------------

bool PointSet::write(const std::string& filename,
                     const IOOptions& options) const
{
    PointSetIO writer(options);
    return writer.write(*this, filename);
}

//-----------------------------------------------------------------------------

void PointSet::clear()
{
    m_vprops.resize(0);

    freeMemory();

    m_deletedVertices = 0;
    m_garbage = false;

    GeometryObject::clear();
}

//-----------------------------------------------------------------------------

void PointSet::freeMemory()
{
    m_vprops.freeMemory();
    GeometryObject::freeMemory();
}

//-----------------------------------------------------------------------------

void PointSet::reserve(size_t nVertices)
{
    GeometryObject::reserve();
    m_vprops.reserve(nVertices);
}

//-----------------------------------------------------------------------------

void PointSet::propertyStats() const
{
    std::vector<std::string> props;

    std::cout << "point properties:\n";
    props = vertexProperties();
    for (const auto& prop : props)
        std::cout << "\t" << prop << std::endl;
}

//-----------------------------------------------------------------------------

PointSet::Vertex PointSet::addVertex(const Point& p)
{
    Vertex v = newVertex();
    if (v.isValid())
        m_vpoint[v] = p;
    return v;
}

//-----------------------------------------------------------------------------

void PointSet::deleteVertex(Vertex v)
{
    if (m_vdeleted[v])
        return;

    // mark v as deleted
    m_vdeleted[v] = true;
    m_deletedVertices++;

    setGarbage();
}

//-----------------------------------------------------------------------------

void PointSet::beginGarbage()
{
    int nV(verticesSize());

    // setup handle mapping
    VertexProperty<Vertex> vmap =
        addVertexProperty<Vertex>("v:garbage-collection");
    for (int i = 0; i < nV; ++i)
        vmap[Vertex(i)] = Vertex(i);

    // remove deleted vertices
    if (nV > 0)
    {
        int i0 = 0;
        int i1 = nV - 1;

        while (true)
        {
            // find first deleted and last un-deleted
            while (!isDeleted(Vertex(i0)) && i0 < i1)
                ++i0;
            while (isDeleted(Vertex(i1)) && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;

            // swap
            m_vprops.swap(i0, i1);
        };

        // remember new size
        nV = isDeleted(Vertex(i0)) ? i0 : i0 + 1;
    }

    m_garbageprops["nV"] = nV;
}

//-----------------------------------------------------------------------------

void PointSet::finalizeGarbage()
{
    VertexProperty<Vertex> vmap =
        getVertexProperty<Vertex>("v:garbage-collection");

    // remove handle maps
    removeVertexProperty(vmap);

    // finally resize arrays
    m_vprops.resize(m_garbageprops["nV"]);
    m_vprops.freeMemory();

    m_deletedVertices = 0;

    GeometryObject::finalizeGarbage();
}

//=============================================================================
} // namespace pmp
//=============================================================================
