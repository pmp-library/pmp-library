//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011-2016 by Graphics & Geometry Group, Bielefeld University
// Copyright (C) 2017 Daniel Sieger
// All rights reserved.
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
#pragma once
//=============================================================================

#include <surface_mesh/GeometryObject.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================

//! \addtogroup geometry geometry
//!@{

//=============================================================================

//! \brief A data structure for point sets.
//! \details Represents a point set with normals.
class PointSet : public GeometryObject
{
public:
    //! \name Handle Types
    //!@{

    //! this type represents a vertex (internally it is basically an index)
    struct Vertex : public BaseHandle
    {
        //! default constructor (with invalid index)
        explicit Vertex(int idx = -1) : BaseHandle(idx){};
    };

    //!@}
    //! \name Property Types
    //!@{

    //! Vertex property of type T
    template <class T>
    class VertexProperty : public Property<T>
    {
    public:
        //! default constructor
        explicit VertexProperty() {}
        explicit VertexProperty(Property<T> p) : Property<T>(p) {}

        //! access the data stored for vertex \c v
        typename Property<T>::Reference operator[](Vertex v)
        {
            return Property<T>::operator[](v.idx());
        }

        //! access the data stored for vertex \c v
        typename Property<T>::ConstReference operator[](Vertex v) const
        {
            return Property<T>::operator[](v.idx());
        }
    };

    //!@}
    //! \name Iterator Types
    //!@{

    //! An iterator class to iterate linearly over all vertices
    class VertexIterator
    {
    public:
        //! Default constructor
        VertexIterator(Vertex v = Vertex(), const PointSet* m = NULL)
            : m_hnd(v), m_ps(m)
        {
            if (m_ps && m_ps->garbage())
                while (m_ps->isValid(m_hnd) && m_ps->isDeleted(m_hnd))
                    ++m_hnd.m_idx;
        }

        //! get the vertex the iterator refers to
        Vertex operator*() const { return m_hnd; }

        //! are two iterators equal?
        bool operator==(const VertexIterator& rhs) const
        {
            return (m_hnd == rhs.m_hnd);
        }

        //! are two iterators different?
        bool operator!=(const VertexIterator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment iterator
        VertexIterator& operator++()
        {
            ++m_hnd.m_idx;
            assert(m_ps);
            while (m_ps->garbage() && m_ps->isValid(m_hnd) &&
                   m_ps->isDeleted(m_hnd))
                ++m_hnd.m_idx;
            return *this;
        }

        //! pre-decrement iterator
        VertexIterator& operator--()
        {
            --m_hnd.m_idx;
            assert(m_ps);
            while (m_ps->garbage() && m_ps->isValid(m_hnd) &&
                   m_ps->isDeleted(m_hnd))
                --m_hnd.m_idx;
            return *this;
        }

    private:
        Vertex          m_hnd;
        const PointSet* m_ps;
    };

    //!@}
    //! \name Container Types
    //!@{

    //! helper class for iterating through all vertices using range-based
    //! for-loops.
    class VertexContainer
    {
    public:
        VertexContainer(VertexIterator begin, VertexIterator end)
            : m_begin(begin), m_end(end)
        {
        }
        VertexIterator begin() const { return m_begin; }
        VertexIterator end() const { return m_end; }
    private:
        VertexIterator m_begin, m_end;
    };

    //!@}
    //! \name Construction, destruction, assignment
    //!@{

    //! default constructor
    PointSet();

    //! destructor (is virtual, since we inherit from GeometryObject)
    virtual ~PointSet();

    //! copy constructor: copies \c rhs to \c *this. performs a deep copy
    //! of all properties.
    PointSet(const PointSet& rhs) : GeometryObject(rhs) { operator=(rhs); }

    //! assign \c rhs to \c *this. performs a deep copy of all properties.
    PointSet& operator=(const PointSet& rhs);

    //! assign \c rhs to \c *this. does not copy custom properties.
    PointSet& assign(const PointSet& rhs);

    //!@}
    //! \name File IO
    //!@{

    //! read mesh from file \c filename. file extension determines file type.
    virtual bool read(const std::string& filename);

    //! write mesh to file \c filename. file extensions determines file type.
    virtual bool write(const std::string& filename) const;

    //!@}
    //! \name Add new elements by hand
    //!@{

    //! add a new vertex with position \c p
    virtual Vertex addVertex(const Point& p);

    //!@}
    //! \name Memory Management
    //!@{

    //! returns number of (deleted and valid) vertices in the point set
    unsigned int verticesSize() const { return (unsigned int)m_vprops.size(); }

    //! returns number of vertices in the point set
    unsigned int nVertices() const
    {
        return verticesSize() - m_deletedVertices;
    }

    //! returns true iff the point set is empty, i.e., has no vertices
    unsigned int empty() const { return nVertices() == 0; }

    //! clear mesh: remove all vertices
    virtual void clear();

    //! remove unused memory from vectors
    virtual void freeMemory();

    //! collect information and delete vertices, edges, etc.
    virtual void beginGarbage();

    //! resize the property vectors accordingly
    virtual void finalizeGarbage();

    //! returns whether vertex \c v is deleted
    //! \sa garbageCollection()
    bool isDeleted(Vertex v) const { return m_vdeleted[v]; }

    //! return whether vertex \c v is valid, i.e. the index is stores
    //! it within the array bounds.
    bool isValid(Vertex v) const
    {
        return (0 <= v.idx()) && (v.idx() < (int)verticesSize());
    }

    //! reserve memory (mainly used in file readers)
    void reserve(unsigned int nvertices);

    //!@}
    //! \name Property handling
    //!@{

    //! add a vertex property of type \c T with name \c name and default
    //! value \c t. fails if a property named \c name exists already,
    //! since the name has to be unique. in this case it returns an
    //! invalid property
    template <class T>
    VertexProperty<T> addVertexProperty(const std::string& name,
                                          const T            t = T())
    {
        return VertexProperty<T>(m_vprops.add<T>(name, t));
    }

    //! get the vertex property named \c name of type \c T. returns an
    //! invalid VertexProperty if the property does not exist or if the
    //! type does not match.
    template <class T>
    VertexProperty<T> getVertexProperty(const std::string& name) const
    {
        return VertexProperty<T>(m_vprops.get<T>(name));
    }

    //! if a vertex property of type \c T with name \c name exists, it is
    //! returned. otherwise this property is added (with default value \c
    //! t)
    template <class T>
    VertexProperty<T> vertexProperty(const std::string& name, const T t = T())
    {
        return VertexProperty<T>(m_vprops.getOrAdd<T>(name, t));
    }

    //! remove the vertex property \c p
    template <class T>
    void removeVertexProperty(VertexProperty<T>& p)
    {
        m_vprops.remove(p);
    }

    //! get the type_info \c T of vertex property named \c name. returns an
    //! typeid(void) if the property does not exist or if the type does not
    //! match.
    const std::type_info& getVertexPropertyType(const std::string& name)
    {
        return m_vprops.getType(name);
    }

    //! returns the names of all vertex properties
    std::vector<std::string> vertexProperties() const
    {
        return m_vprops.properties();
    }

    //! prints the names of all properties
    virtual void propertyStats() const;

    //!@}
    //! \name Iterators
    //!@{

    //! returns start iterator for vertices
    VertexIterator verticesBegin() const
    {
        return VertexIterator(Vertex(0), this);
    }

    //! returns end iterator for vertices
    VertexIterator verticesEnd() const
    {
        return VertexIterator(Vertex(verticesSize()), this);
    }

    //! returns vertex container for C++11 range-based for-loops
    VertexContainer vertices() const
    {
        return VertexContainer(verticesBegin(), verticesEnd());
    }

    //!@}
    //! \name Higher-level Topological Operations
    //!@{

    //! deletes the vertex \c v from the mesh
    virtual void deleteVertex(Vertex v);

    //!@}
    //! \name Geometry-related Functions
    //!@{

    //! position of a vertex (read only)
    const Point& position(Vertex v) const { return m_vpoint[v]; }

    //! position of a vertex
    Point& position(Vertex v) { return m_vpoint[v]; }

    //! vector of point positions, re-implemented from \c GeometryObject
    std::vector<Point>& pointVector() { return m_vpoint.vector(); }

    //! normal for a vertex
    const Normal& normal(Vertex v) const { return m_vnormal[v]; }

    //! vector of vertex normals
    std::vector<Normal>& normals() { return m_vnormal.vector(); }

    //! check if the point set has normals
    bool hasNormals() const { return m_vnormal; }

    //!@}

protected:
    //! \name Protected Members
    //!@{

    PropertyContainer      m_vprops;  //!< property container for vertices
    VertexProperty<Point>  m_vpoint;  //!< point coordinates
    VertexProperty<Normal> m_vnormal; //!< normal vectors

    //!@}

private:
    //! \name Private Members
    //!@{

    //! allocate a new vertex, resize vertex properties accordingly.
    Vertex newVertex()
    {
        m_vprops.pushBack();
        return Vertex(verticesSize() - 1);
    }

private:
    VertexProperty<bool>   m_vdeleted; //!< for marking deleted vertices
    unsigned int           m_deletedVertices; //!< number of deleted vertices

    //!@}
};

//! \name Output Operators
//!@{

//! output a vertex to a stream
inline std::ostream& operator<<(std::ostream& os, PointSet::Vertex v)
{
    return (os << 'v' << v.idx());
}

//!@}

//=============================================================================
//!@}
//=============================================================================
} // namespace surface_mesh
//=============================================================================
