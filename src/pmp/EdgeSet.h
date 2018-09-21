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

#include <pmp/PointSet.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup geometry geometry
//!@{

//=============================================================================

//! A halfedge data structure for representing a set of edges.
class EdgeSet : public PointSet
{
public:
    //! \name Handle Types
    //!@{

    //! \brief this type represents a halfedge (internally it is basically an
    //! index) \sa Vertex, Edge, Face
    struct Halfedge : public BaseHandle
    {
        //! default constructor (with invalid index)
        explicit Halfedge(IndexType idx = PMP_MAX_INDEX) : BaseHandle(idx) {}
    };

    //! this type represents an edge (internally it is basically an index)
    //! \sa Vertex, Halfedge, Face
    struct Edge : public BaseHandle
    {
        //! default constructor (with invalid index)
        explicit Edge(IndexType idx = PMP_MAX_INDEX) : BaseHandle(idx) {}
    };

    //!@}
    //! \name Connectivity Types
    //!@{

    //! This type stores the vertex connectivity
    struct VertexConnectivity
    {
        //! an outgoing halfedge per vertex (it will be a boundary halfedge
        //! for boundary vertices)
        Halfedge m_halfedge;
    };

    //! This type stores the halfedge connectivity
    struct HalfedgeConnectivity
    {
        Vertex m_vertex;         //!< vertex the halfedge points to
        Halfedge m_nextHalfedge; //!< next halfedge
        Halfedge m_prevHalfedge; //!< previous halfedge
    };

    //!@}
    //! \name Property Types
    //!@{

    //! Halfedge property of type T
    template <class T>
    class HalfedgeProperty : public Property<T>
    {
    public:
        //! default constructor
        explicit HalfedgeProperty() {}
        explicit HalfedgeProperty(Property<T> p) : Property<T>(p) {}

        //! access the data stored for halfedge \c h
        typename Property<T>::Reference operator[](Halfedge h)
        {
            return Property<T>::operator[](h.idx());
        }

        //! access the data stored for halfedge \c h
        typename Property<T>::ConstReference operator[](Halfedge h) const
        {
            return Property<T>::operator[](h.idx());
        }
    };

    //! Edge property of type T
    template <class T>
    class EdgeProperty : public Property<T>
    {
    public:
        //! default constructor
        explicit EdgeProperty() {}
        explicit EdgeProperty(Property<T> p) : Property<T>(p) {}

        //! access the data stored for edge \c e
        typename Property<T>::Reference operator[](Edge e)
        {
            return Property<T>::operator[](e.idx());
        }

        //! access the data stored for edge \c e
        typename Property<T>::ConstReference operator[](Edge e) const
        {
            return Property<T>::operator[](e.idx());
        }
    };

    //!@}
    //! \name Iterator Types
    //!@{

    //! this class iterates linearly over all halfedges
    //! \sa halfedgesBegin(), halfedgesEnd()
    //! \sa VertexIterator, EdgeIterator, FaceIterator
    class HalfedgeIterator
    {
    public:
        //! Default constructor
        HalfedgeIterator(Halfedge h = Halfedge(), const EdgeSet* es = nullptr)
            : m_hnd(h), m_edges(es)
        {
            if (m_edges && m_edges->garbage())
                while (m_edges->isValid(m_hnd) && m_edges->isDeleted(m_hnd))
                    ++m_hnd.m_idx;
        }

        //! get the halfedge the iterator refers to
        Halfedge operator*() const { return m_hnd; }

        //! are two iterators equal?
        bool operator==(const HalfedgeIterator& rhs) const
        {
            return (m_hnd == rhs.m_hnd);
        }

        //! are two iterators different?
        bool operator!=(const HalfedgeIterator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment iterator
        HalfedgeIterator& operator++()
        {
            ++m_hnd.m_idx;
            assert(m_edges);
            while (m_edges->garbage() && m_edges->isValid(m_hnd) &&
                   m_edges->isDeleted(m_hnd))
                ++m_hnd.m_idx;
            return *this;
        }

        //! pre-decrement iterator
        HalfedgeIterator& operator--()
        {
            --m_hnd.m_idx;
            assert(m_edges);
            while (m_edges->garbage() && m_edges->isValid(m_hnd) &&
                   m_edges->isDeleted(m_hnd))
                --m_hnd.m_idx;
            return *this;
        }

    private:
        Halfedge m_hnd;
        const EdgeSet* m_edges;
    };

    //! this class iterates linearly over all edges
    //! \sa edgesBegin(), edgesEnd()
    //! \sa VertexIterator, HalfedgeIterator, FaceIterator
    class EdgeIterator
    {
    public:
        //! Default constructor
        EdgeIterator(Edge e = Edge(), const EdgeSet* es = nullptr)
            : m_hnd(e), m_edges(es)
        {
            if (m_edges && m_edges->garbage())
                while (m_edges->isValid(m_hnd) && m_edges->isDeleted(m_hnd))
                    ++m_hnd.m_idx;
        }

        //! get the edge the iterator refers to
        Edge operator*() const { return m_hnd; }

        //! are two iterators equal?
        bool operator==(const EdgeIterator& rhs) const
        {
            return (m_hnd == rhs.m_hnd);
        }

        //! are two iterators different?
        bool operator!=(const EdgeIterator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment iterator
        EdgeIterator& operator++()
        {
            ++m_hnd.m_idx;
            assert(m_edges);
            while (m_edges->garbage() && m_edges->isValid(m_hnd) &&
                   m_edges->isDeleted(m_hnd))
                ++m_hnd.m_idx;
            return *this;
        }

        //! pre-decrement iterator
        EdgeIterator& operator--()
        {
            --m_hnd.m_idx;
            assert(m_edges);
            while (m_edges->garbage() && m_edges->isValid(m_hnd) &&
                   m_edges->isDeleted(m_hnd))
                --m_hnd.m_idx;
            return *this;
        }

    private:
        Edge m_hnd;
        const EdgeSet* m_edges;
    };

    //!@}
    //! \name Container Types
    //!@{

    //! helper class for iterating through all halfedges using range-based
    //! for-loops. \sa halfedges()
    class HalfedgeContainer
    {
    public:
        HalfedgeContainer(HalfedgeIterator begin, HalfedgeIterator end)
            : m_begin(begin), m_end(end)
        {
        }
        HalfedgeIterator begin() const { return m_begin; }
        HalfedgeIterator end() const { return m_end; }
    private:
        HalfedgeIterator m_begin, m_end;
    };

    //! helper class for iterating through all edges using range-based
    //! for-loops. \sa edges()
    class EdgeContainer
    {
    public:
        EdgeContainer(EdgeIterator begin, EdgeIterator end)
            : m_begin(begin), m_end(end)
        {
        }
        EdgeIterator begin() const { return m_begin; }
        EdgeIterator end() const { return m_end; }
    private:
        EdgeIterator m_begin, m_end;
    };

    //!@}
    //! \name Circulator Types
    //!@{

    //! this class circulates through all one-ring neighbors of a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa HalfedgeAroundVertexCirculator, vertices(Vertex)
    class VertexAroundVertexCirculator
    {
    public:
        //! default constructor
        VertexAroundVertexCirculator(const EdgeSet* es = nullptr,
                                     Vertex v = Vertex())
            : m_edges(es), m_active(true)
        {
            if (m_edges)
                m_halfedge = m_edges->halfedge(v);
        }

        //! are two circulators equal?
        bool operator==(const VertexAroundVertexCirculator& rhs) const
        {
            assert(m_edges);
            return (m_active && (m_edges == rhs.m_edges) &&
                    (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const VertexAroundVertexCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotate couter-clockwise)
        VertexAroundVertexCirculator& operator++()
        {
            assert(m_edges);
            m_halfedge = m_edges->ccwRotatedHalfedge(m_halfedge);
            m_active = true;
            return *this;
        }

        //! pre-decrement (rotate clockwise)
        VertexAroundVertexCirculator& operator--()
        {
            assert(m_edges);
            m_halfedge = m_edges->cwRotatedHalfedge(m_halfedge);
            return *this;
        }

        //! get the vertex the circulator refers to
        Vertex operator*() const
        {
            assert(m_edges);
            return m_edges->toVertex(m_halfedge);
        }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return m_halfedge.isValid(); }

        //! return current halfedge
        Halfedge halfedge() const { return m_halfedge; }

        // helper for C++11 range-based for-loops
        VertexAroundVertexCirculator& begin()
        {
            m_active = !m_halfedge.isValid();
            return *this;
        }
        // helper for C++11 range-based for-loops
        VertexAroundVertexCirculator& end()
        {
            m_active = true;
            return *this;
        }

    private:
        const EdgeSet* m_edges;
        Halfedge m_halfedge;
        bool m_active; // helper for C++11 range-based for-loops
    };

    //! this class circulates through all outgoing halfedges of a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundVertexCirculator, halfedges(Vertex)
    class HalfedgeAroundVertexCirculator
    {
    public:
        //! default constructor
        HalfedgeAroundVertexCirculator(const EdgeSet* es = nullptr,
                                       Vertex v = Vertex())
            : m_edges(es), m_active(true)
        {
            if (m_edges)
                m_halfedge = m_edges->halfedge(v);
        }

        //! are two circulators equal?
        bool operator==(const HalfedgeAroundVertexCirculator& rhs) const
        {
            assert(m_edges);
            return (m_active && (m_edges == rhs.m_edges) &&
                    (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const HalfedgeAroundVertexCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotate couter-clockwise)
        HalfedgeAroundVertexCirculator& operator++()
        {
            assert(m_edges);
            m_halfedge = m_edges->ccwRotatedHalfedge(m_halfedge);
            m_active = true;
            return *this;
        }

        //! pre-decrement (rotate clockwise)
        HalfedgeAroundVertexCirculator& operator--()
        {
            assert(m_edges);
            m_halfedge = m_edges->cwRotatedHalfedge(m_halfedge);
            return *this;
        }

        //! get the halfedge the circulator refers to
        Halfedge operator*() const { return m_halfedge; }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return m_halfedge.isValid(); }

        // helper for C++11 range-based for-loops
        HalfedgeAroundVertexCirculator& begin()
        {
            m_active = !m_halfedge.isValid();
            return *this;
        }
        // helper for C++11 range-based for-loops
        HalfedgeAroundVertexCirculator& end()
        {
            m_active = true;
            return *this;
        }

    private:
        const EdgeSet* m_edges;
        Halfedge m_halfedge;
        bool m_active; // helper for C++11 range-based for-loops
    };

    //!@}
    //! \name Construction, destruction, assignment
    //!@{

    //! default constructor
    EdgeSet();

    //! destructor (is virtual, since we inherit from GeometryObject)
    virtual ~EdgeSet();

    //! copy constructor: copies \c rhs to \c *this. performs a deep copy of all properties.
    EdgeSet(const EdgeSet& rhs) : PointSet(rhs) { operator=(rhs); }

    //! assign \c rhs to \c *this. performs a deep copy of all properties.
    EdgeSet& operator=(const EdgeSet& rhs);

    //! assign \c rhs to \c *this. does not copy custom properties.
    EdgeSet& assign(const EdgeSet& rhs);

    //!@}
    //! \name File IO
    //!@{

    //! read edge set from file \c filename. file extension determines file type.
    //! \sa write(const std::string& filename)
    bool read(const std::string& filename,
              const IOOptions& options = IOOptions()) override;

    //! write edge set to file \c filename. file extensions determines file type.
    //! \sa read(const std::string& filename)
    bool write(const std::string& filename,
               const IOOptions& options = IOOptions()) const override;

    //!@}
    //! \name Memory Management
    //!@{

    //! returns number of (deleted and valid)halfedge in the mesh
    size_t halfedgesSize() const { return m_hprops.size(); }

    //! returns number of (deleted and valid)edges in the mesh
    size_t edgesSize() const { return m_eprops.size(); }

    //! returns number of halfedge in the mesh
    size_t nHalfedges() const { return halfedgesSize() - 2 * m_deletedEdges; }

    //! returns number of edges in the mesh
    size_t nEdges() const { return edgesSize() - m_deletedEdges; }

    //! clear mesh: remove all vertices, edges, faces
    virtual void clear() override;

    //! remove unused memory from vectors
    virtual void freeMemory() override;

    //! reserve memory (mainly used in file readers)
    void reserve(size_t nvertices, size_t nedges);

    // tell the compiler that we're overloading the superclass' method here
    using PointSet::isDeleted;

    //! returns whether halfedge \c h is deleted
    //! \sa garbageCollection()
    bool isDeleted(Halfedge h) const { return m_edeleted[edge(h)]; }

    //! returns whether edge \c e is deleted
    //! \sa garbageCollection()
    bool isDeleted(Edge e) const { return m_edeleted[e]; }

    // tell compiler that we want to use the superclass method isValid and
    // overload it
    using PointSet::isValid;

    //! return whether halfedge \c h is valid, i.e. the index is stores it
    //! within the array bounds.
    bool isValid(Halfedge h) const
    {
        return (0 <= h.idx()) && (h.idx() < (int)halfedgesSize());
    }

    //! return whether edge \c e is valid, i.e. the index is stores it within the array bounds.
    bool isValid(Edge e) const
    {
        return (0 <= e.idx()) && (e.idx() < (int)edgesSize());
    }

    //!@}
    //! \name Low-level Connectivity
    //!@{

    //! returns an outgoing halfedge of vertex \c v.
    //! if \c v is a boundary vertex this will be a boundary halfedge.
    Halfedge halfedge(Vertex v) const { return m_vconn[v].m_halfedge; }

    //! set the outgoing halfedge of vertex \c v to \c h
    void setHalfedge(Vertex v, Halfedge h) { m_vconn[v].m_halfedge = h; }

    //! returns whether \c v is a boundary vertex
    bool isSegmentBoundary(Vertex v) const
    {
        Halfedge h(halfedge(v));
        Halfedge opposite(oppositeHalfedge(h));

        return !h.isValid() || nextHalfedge(opposite) == h;
    }

    //! returns whether \c v is isolated, i.e., not incident to any edge
    bool isIsolated(Vertex v) const { return !halfedge(v).isValid(); }

    //! returns whether \c v is a 1-manifold vertex, i.e., it has exactly two
    //! incident edges.
    bool isOneManifold(Vertex v) const { return valence(v) == 2; }

    //! returns the vertex the halfedge \c h points to
    inline Vertex toVertex(Halfedge h) const { return m_hconn[h].m_vertex; }

    //! returns the vertex the halfedge \c h emanates from
    inline Vertex fromVertex(Halfedge h) const
    {
        return toVertex(oppositeHalfedge(h));
    }

    //! sets the vertex the halfedge \c h points to to \c v
    inline void setVertex(Halfedge h, Vertex v) { m_hconn[h].m_vertex = v; }

    //! returns the next halfedge within the incident face
    inline Halfedge nextHalfedge(Halfedge h) const
    {
        return m_hconn[h].m_nextHalfedge;
    }

    //! sets the next halfedge of \c h within the face to \c nh
    inline void setNextHalfedge(Halfedge h, Halfedge nh)
    {
        m_hconn[h].m_nextHalfedge = nh;
        m_hconn[nh].m_prevHalfedge = h;
    }

    //! sets the previous halfedge of \c h and the next halfedge of \c ph to \c nh
    inline void setPrevHalfedge(Halfedge h, Halfedge ph)
    {
        m_hconn[h].m_prevHalfedge = ph;
        m_hconn[ph].m_nextHalfedge = h;
    }

    //! returns the previous halfedge within the incident face
    inline Halfedge prevHalfedge(Halfedge h) const
    {
        return m_hconn[h].m_prevHalfedge;
    }

    //! returns the opposite halfedge of \c h
    inline Halfedge oppositeHalfedge(Halfedge h) const
    {
        return Halfedge((h.idx() & 1) ? h.idx() - 1 : h.idx() + 1);
    }

    //! returns the halfedge that is rotated counter-clockwise around the
    //! start vertex of \c h. it is the opposite halfedge of the previous
    //! halfedge of \c h.
    inline Halfedge ccwRotatedHalfedge(Halfedge h) const
    {
        return oppositeHalfedge(prevHalfedge(h));
    }

    //! returns the halfedge that is rotated clockwise around the start
    //! vertex of \c h. it is the next halfedge of the opposite halfedge of
    //! \c h.
    inline Halfedge cwRotatedHalfedge(Halfedge h) const
    {
        return nextHalfedge(oppositeHalfedge(h));
    }

    //! return the edge that contains halfedge \c h as one of its two
    //! halfedges.
    inline Edge edge(Halfedge h) const { return Edge(h.idx() >> 1); }

    //! returns whether h is a boundary halfedge
    bool isSegmentBoundary(Halfedge h) const
    {
        Halfedge next = nextHalfedge(h);
        return !next.isValid() || oppositeHalfedge(next) == h;
    }

    //! returns the \c i'th halfedge of edge \c e. \c i has to be 0 or 1.
    inline Halfedge halfedge(Edge e, unsigned int i) const
    {
        assert(i <= 1);
        return Halfedge((e.idx() << 1) + i);
    }

    //! returns the \c i'th vertex of edge \c e. \c i has to be 0 or 1.
    inline Vertex vertex(Edge e, unsigned int i) const
    {
        assert(i <= 1);
        return toVertex(halfedge(e, i));
    }

    //! returns whether \c e is a boundary edge, i.e., if one of its
    //! halfedges is a boundary halfedge.
    bool isSegmentBoundary(Edge e) const
    {
        return (isSegmentBoundary(halfedge(e, 0)) ||
                isSegmentBoundary(halfedge(e, 1)));
    }

    //!@}
    //! \name Property Handling
    //!@{

    //! get the type_info \c T of vertex property named \c name. returns an
    //! typeid(void) if the property does not exist or if the type does not
    //! match.
    const std::type_info& getVertexPropertyType(const std::string& name)
    {
        return m_vprops.getType(name);
    }

    //! add a halfedge property of type \c T with name \c name and default
    //! value \c t.  fails if a property named \c name exists already,
    //! since the name has to be unique. in this case it returns an
    //! invalid property.
    template <class T>
    HalfedgeProperty<T> addHalfedgeProperty(const std::string& name,
                                            const T t = T())
    {
        return HalfedgeProperty<T>(m_hprops.add<T>(name, t));
    }

    //! add a edge property of type \c T with name \c name and default
    //! value \c t.  fails if a property named \c name exists already,
    //! since the name has to be unique.  in this case it returns an
    //! invalid property.
    template <class T>
    EdgeProperty<T> addEdgeProperty(const std::string& name, const T t = T())
    {
        return EdgeProperty<T>(m_eprops.add<T>(name, t));
    }

    //! get the halfedge property named \c name of type \c T. returns an
    //! invalid VertexProperty if the property does not exist or if the
    //! type does not match.
    template <class T>
    HalfedgeProperty<T> getHalfedgeProperty(const std::string& name) const
    {
        return HalfedgeProperty<T>(m_hprops.get<T>(name));
    }

    //! get the edge property named \c name of type \c T. returns an
    //! invalid VertexProperty if the property does not exist or if the
    //! type does not match.
    template <class T>
    EdgeProperty<T> getEdgeProperty(const std::string& name) const
    {
        return EdgeProperty<T>(m_eprops.get<T>(name));
    }

    //! if a halfedge property of type \c T with name \c name exists, it is
    //! returned.  otherwise this property is added (with default value \c
    //! t)
    template <class T>
    HalfedgeProperty<T> halfedgeProperty(const std::string& name,
                                         const T t = T())
    {
        return HalfedgeProperty<T>(m_hprops.getOrAdd<T>(name, t));
    }

    //! if an edge property of type \c T with name \c name exists, it is
    //! returned.  otherwise this property is added (with default value \c
    //! t)
    template <class T>
    EdgeProperty<T> edgeProperty(const std::string& name, const T t = T())
    {
        return EdgeProperty<T>(m_eprops.getOrAdd<T>(name, t));
    }

    //! remove the halfedge property \c p
    template <class T>
    void removeHalfedgeProperty(HalfedgeProperty<T>& p)
    {
        m_hprops.remove(p);
    }

    //! remove the edge property \c p
    template <class T>
    void removeEdgeProperty(EdgeProperty<T>& p)
    {
        m_eprops.remove(p);
    }

    //! get the type_info \c T of halfedge property named \c name. returns an
    //! typeid(void) if the property does not exist or if the type does not
    //! match.
    const std::type_info& getHalfedgePropertyType(const std::string& name)
    {
        return m_hprops.getType(name);
    }

    //! get the type_info \c T of edge property named \c name. returns an
    //! typeid(void) if the property does not exist or if the type does not
    //! match.
    const std::type_info& getEdgePropertyType(const std::string& name)
    {
        return m_eprops.getType(name);
    }

    //! returns the names of all vertex properties
    std::vector<std::string> vertexProperties() const
    {
        return m_vprops.properties();
    }

    //! returns the names of all halfedge properties
    std::vector<std::string> halfedgeProperties() const
    {
        return m_hprops.properties();
    }

    //! returns the names of all edge properties
    std::vector<std::string> edgeProperties() const
    {
        return m_eprops.properties();
    }

    //! prints the names of all properties
    virtual void propertyStats() const override;

    //!@}
    //! \name Iterators & Circulators
    //!@{

    //! returns vertex container for C++11 range-based for-loops
    using PointSet::vertices;

    //! returns start iterator for halfedges
    HalfedgeIterator halfedgesBegin() const
    {
        return HalfedgeIterator(Halfedge(0), this);
    }

    //! returns end iterator for halfedges
    HalfedgeIterator halfedgesEnd() const
    {
        return HalfedgeIterator(Halfedge(halfedgesSize()), this);
    }

    //! returns halfedge container for C++11 range-based for-loops
    HalfedgeContainer halfedges() const
    {
        return HalfedgeContainer(halfedgesBegin(), halfedgesEnd());
    }

    //! returns start iterator for edges
    EdgeIterator edgesBegin() const { return EdgeIterator(Edge(0), this); }

    //! returns end iterator for edges
    EdgeIterator edgesEnd() const
    {
        return EdgeIterator(Edge(edgesSize()), this);
    }

    //! returns edge container for C++11 range-based for-loops
    EdgeContainer edges() const
    {
        return EdgeContainer(edgesBegin(), edgesEnd());
    }

    //! returns circulator for vertices around vertex \c v
    VertexAroundVertexCirculator vertices(Vertex v) const
    {
        return VertexAroundVertexCirculator(this, v);
    }

    //! returns circulator for outgoing halfedges around vertex \c v
    HalfedgeAroundVertexCirculator halfedges(Vertex v) const
    {
        return HalfedgeAroundVertexCirculator(this, v);
    }

    //!@}
    //! \name Higher-level Operations
    //!@{

    //! Subdivide the edge \c e = (v0,v1) by splitting it into the two edge
    //! (v0,p) and (p,v1). Note that this function does not introduce any
    //! other edge or faces. It simply splits the edge. Returns halfedge that
    //! points to \c p.
    //! \sa insertVertex(Edge, Vertex)
    //! \sa insertVertex(Halfedge, Vertex)
    Halfedge insertVertex(Edge e, const Point& p)
    {
        return insertVertex(halfedge(e, 0), addVertex(p));
    }

    //! Subdivide the edge \c e = (v0,v1) by splitting it into the two edge
    //! (v0,v) and (v,v1). Note that this function does not introduce any
    //! other edge or faces. It simply splits the edge. Returns halfedge
    //! that points to \c p. \sa insertVertex(Edge, Point) \sa
    //! insertVertex(Halfedge, Vertex)
    Halfedge insertVertex(Edge e, Vertex v)
    {
        return insertVertex(halfedge(e, 0), v);
    }

    //! Subdivide the edge \c e = (v0,v1) by splitting it into the two edge
    //! (v0,v) and (v,v1). Note that this function does not introduce any
    //! other edge or faces. It simply splits the edge. Returns halfedge
    //! that points to \c p.  \sa insertVertex(Edge, Point) \sa
    //! insertVertex(Edge, Vertex)
    virtual Halfedge insertVertex(Halfedge h0, Vertex v);

    //! insert edge between v0 and v1
    //! returns the new halfedge from v0 to v1
    Halfedge insertEdge(Vertex v0, Vertex v1);

    //! returns the valence (number of incident edges or neighboring
    //! vertices) of vertex \c v.
    size_t valence(Vertex v) const;

    //! find the halfedge from start to end
    Halfedge findHalfedge(Vertex start, Vertex end) const;

    //! find the edge (a,b)
    Edge findEdge(Vertex a, Vertex b) const;

    //! deletes the vertex \c v from the mesh
    virtual void deleteVertex(Vertex v) override;

    //! deletes the edge \c e from the mesh
    virtual void deleteEdge(Edge e);

    //!@}
    //! \name Geometry-related Functions
    //!@{

    //! compute the length of edge \c e.
    Scalar edgeLength(Edge e) const;

    //!@}

protected:
    //! \name Garbage Collection
    //!@{

    //! initialize garbage collection
    virtual void beginGarbage() override;

    //! finalize garbage collection
    virtual void finalizeGarbage() override;

    //!@}

    //! \name Entity Allocation
    //!@{

    //! allocate a new edge, resize edge and halfedge properties accordingly.
    Halfedge newEdge(Vertex start, Vertex end)
    {
        assert(start != end);

        if (halfedgesSize() == PMP_MAX_INDEX - 1)
        {
            std::cerr << "newEdge: cannot allocate edge, max. index reached"
                      << std::endl;
            return Halfedge();
        }

        m_eprops.pushBack();
        m_hprops.pushBack();
        m_hprops.pushBack();

        Halfedge h0(halfedgesSize() - 2);
        Halfedge h1(halfedgesSize() - 1);

        setVertex(h0, end);
        setVertex(h1, start);

        return h0;
    }

    //!@}

protected:
    //! \name Protected Members
    //!@{

    // property containers for each entity type
    PropertyContainer m_hprops; //!< halfedge property container
    PropertyContainer m_eprops; //!< edge property container

    // connectivity information for vertices and halfedges
    VertexProperty<VertexConnectivity> m_vconn;     //!< vertex connectivity
    HalfedgeProperty<HalfedgeConnectivity> m_hconn; //!< halfedge connectivity

    EdgeProperty<bool> m_edeleted; //!< mark deleted edges

    size_t m_deletedEdges; //!< number of deleted edges

    //!@}
};

//! \name Output operators
//!@{

//! output a halfedge to a stream
inline std::ostream& operator<<(std::ostream& os, EdgeSet::Halfedge h)
{
    return (os << 'h' << h.idx());
}

//! output an edge to a stream
inline std::ostream& operator<<(std::ostream& os, EdgeSet::Edge e)
{
    return (os << 'e' << e.idx());
}
//!@}

//=============================================================================
//!@}
//=============================================================================
} // namespace pmp
//=============================================================================
