//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
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

#include <pmp/GeometryObject.h>
#include <pmp/io/IOOptions.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup geometry geometry
//!@{

//=============================================================================

//! \brief A data structure for point sets.
//! \details Additional properties such as normals or colors can be attached.
class PointSet : public GeometryObject
{
public:
    //! \name Handle Types
    //!@{

    //! this type represents a vertex (internally it is basically an index)
    struct Vertex : public BaseHandle
    {
        //! default constructor (with invalid index)
        explicit Vertex(IndexType idx = PMP_MAX_INDEX) : BaseHandle(idx){};
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
        Vertex m_hnd;
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

    //! read point set from file \c filename. file extension determines file type.
    //! \sa write(const std::string& filename)
    virtual bool read(const std::string& filename,
                      const IOOptions& options = IOOptions());

    //! write point set to file \c filename. file extensions determines file type.
    //! \sa read(const std::string& filename)
    virtual bool write(const std::string& filename,
                       const IOOptions& options = IOOptions()) const;

    //!@}
    //! \name Add new elements by hand
    //!@{

    //! add a new vertex with position \c p
    virtual Vertex addVertex(const Point& p);

    //!@}
    //! \name Memory Management
    //!@{

    //! returns number of (deleted and valid) vertices in the point set
    size_t verticesSize() const { return m_vprops.size(); }

    //! returns number of vertices in the point set
    size_t nVertices() const { return verticesSize() - m_deletedVertices; }

    //! returns true iff the point set is empty, i.e., has no vertices
    bool isEmpty() const { return nVertices() == 0; }

    //! clear mesh: remove all vertices
    virtual void clear();

    //! remove unused memory from vectors
    virtual void freeMemory();

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
    void reserve(size_t nVertices);

    //!@}
    //! \name Property handling
    //!@{

    //! add a vertex property of type \c T with name \c name and default
    //! value \c t. fails if a property named \c name exists already,
    //! since the name has to be unique. in this case it returns an
    //! invalid property
    template <class T>
    VertexProperty<T> addVertexProperty(const std::string& name,
                                        const T t = T())
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
    std::vector<Point>& positions() { return m_vpoint.vector(); }

    //!@}

protected:
    //! \name Protected Functions
    //!@{

    //! initialize garbage collection
    virtual void beginGarbage();

    //! resize the property vectors accordingly
    virtual void finalizeGarbage();

    //!@}

protected:
    //! \name Protected Members
    //!@{

    PropertyContainer m_vprops;      //!< property container for vertices
    VertexProperty<Point> m_vpoint;  //!< point coordinates
    VertexProperty<bool> m_vdeleted; //!< for marking deleted vertices
    size_t m_deletedVertices;        //!< number of deleted vertices

    //!@}

private:
    //! \name Private Members
    //!@{

    //! allocate a new vertex, resize vertex properties accordingly.
    Vertex newVertex()
    {
        if (verticesSize() == PMP_MAX_INDEX - 1)
        {
            std::cerr << "newVertex: cannot allocate vertex, max. index reached"
                      << std::endl;
            return Vertex();
        }
        m_vprops.pushBack();
        return Vertex(verticesSize() - 1);
    }

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
} // namespace pmp
//=============================================================================
