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

#include <pmp/types.h>
#include <pmp/properties.h>
#include <pmp/BoundingBox.h>
#include <pmp/io/IOOptions.h>

#include <map>
#include <vector>
#include <limits>
#include <numeric>

//=============================================================================

namespace pmp {

class SurfaceMeshIO;

//=============================================================================

//! \addtogroup geometry geometry
//!@{

//=============================================================================

//! A halfedge data structure for polygonal meshes.
class SurfaceMesh
{

public:
    //! \name Handle Types
    //!@{

    //! Base class for all entity handles types.
    //! \details internally it is basically an index.
    class Handle
    {
    public:
        //! constructor
        explicit Handle(IndexType idx = PMP_MAX_INDEX) : m_idx(idx) {}

        //! Get the underlying index of this handle
        int idx() const { return m_idx; }

        //! reset handle to be invalid (index=PMP_MAX_INDEX.)
        void reset() { m_idx = PMP_MAX_INDEX; }

        //! return whether the handle is valid, i.e., the index is not equal to PMP_MAX_INDEX.
        bool isValid() const { return m_idx != PMP_MAX_INDEX; }

        //! are two handles equal?
        bool operator==(const Handle& rhs) const
        {
            return m_idx == rhs.m_idx;
        }

        //! are two handles different?
        bool operator!=(const Handle& rhs) const
        {
            return m_idx != rhs.m_idx;
        }

        //! compare operator useful for sorting handles
        bool operator<(const Handle& rhs) const
        {
            return m_idx < rhs.m_idx;
        }

    private:
        friend class SurfaceMesh;
        IndexType m_idx;
    };

    //! this type represents a vertex (internally it is basically an index)
    struct Vertex : public Handle
    {
        //! default constructor (with invalid index)
        explicit Vertex(IndexType idx = PMP_MAX_INDEX) : Handle(idx){};
    };

    //! \brief this type represents a halfedge (internally it is basically an
    //! index) \sa Vertex, Edge, Face
    struct Halfedge : public Handle
    {
        //! default constructor (with invalid index)
        explicit Halfedge(IndexType idx = PMP_MAX_INDEX) : Handle(idx) {}
    };

    //! this type represents an edge (internally it is basically an index)
    //! \sa Vertex, Halfedge, Face
    struct Edge : public Handle
    {
        //! default constructor (with invalid index)
        explicit Edge(IndexType idx = PMP_MAX_INDEX) : Handle(idx) {}
    };


    //! this type represents a face (internally it is basically an index)
    //! \sa Vertex, Halfedge, Edge
    struct Face : public Handle
    {
        //! default constructor (with invalid index)
        explicit Face(IndexType idx = PMP_MAX_INDEX) : Handle(idx) {}
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
        Face m_face;             //!< face incident to halfedge
        Vertex m_vertex;         //!< vertex the halfedge points to
        Halfedge m_nextHalfedge; //!< next halfedge
        Halfedge m_prevHalfedge; //!< previous halfedge
    };

    //! This type stores the face connectivity
    //! \sa VertexConnectivity, HalfedgeConnectivity
    struct FaceConnectivity
    {
        //! a halfedge that is part of the face
        Halfedge m_halfedge;
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


    //! Face property of type T
    //! \sa VertexProperty, HalfedgeProperty, EdgeProperty
    template <class T>
    class FaceProperty : public Property<T>
    {
    public:
        //! default constructor
        explicit FaceProperty() {}
        explicit FaceProperty(Property<T> p) : Property<T>(p) {}

        //! access the data stored for face \c f
        typename Property<T>::Reference operator[](Face f)
        {
            return Property<T>::operator[](f.idx());
        }

        //! access the data stored for face \c f
        typename Property<T>::ConstReference operator[](Face f) const
        {
            return Property<T>::operator[](f.idx());
        }
    };

    //! Object property of type T
    template <class T>
    class ObjectProperty : public Property<T>
    {
    public:
        //! default constructor
        explicit ObjectProperty() {}
        explicit ObjectProperty(Property<T> p) : Property<T>(p) {}

        //! access the data stored for the object
        typename Property<T>::Reference operator[](size_t idx)
        {
            return Property<T>::operator[](idx);
        }

        //! access the data stored for the object
        typename Property<T>::ConstReference operator[](size_t idx) const
        {
            return Property<T>::operator[](idx);
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
        VertexIterator(Vertex v = Vertex(), const SurfaceMesh* m = NULL)
            : m_hnd(v), m_mesh(m)
        {
            if (m_mesh && m_mesh->garbage())
                while (m_mesh->isValid(m_hnd) && m_mesh->isDeleted(m_hnd))
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
            assert(m_mesh);
            while (m_mesh->garbage() && m_mesh->isValid(m_hnd) &&
                   m_mesh->isDeleted(m_hnd))
                ++m_hnd.m_idx;
            return *this;
        }

        //! pre-decrement iterator
        VertexIterator& operator--()
        {
            --m_hnd.m_idx;
            assert(m_mesh);
            while (m_mesh->garbage() && m_mesh->isValid(m_hnd) &&
                   m_mesh->isDeleted(m_hnd))
                --m_hnd.m_idx;
            return *this;
        }

    private:
        Vertex m_hnd;
        const SurfaceMesh* m_mesh;
    };

    //! this class iterates linearly over all halfedges
    //! \sa halfedgesBegin(), halfedgesEnd()
    //! \sa VertexIterator, EdgeIterator, FaceIterator
    class HalfedgeIterator
    {
    public:
        //! Default constructor
        HalfedgeIterator(Halfedge h = Halfedge(), const SurfaceMesh* mesh = nullptr)
            : m_hnd(h), m_mesh(mesh)
        {
            if (m_mesh && m_mesh->garbage())
                while (m_mesh->isValid(m_hnd) && m_mesh->isDeleted(m_hnd))
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
            assert(m_mesh);
            while (m_mesh->garbage() && m_mesh->isValid(m_hnd) &&
                   m_mesh->isDeleted(m_hnd))
                ++m_hnd.m_idx;
            return *this;
        }

        //! pre-decrement iterator
        HalfedgeIterator& operator--()
        {
            --m_hnd.m_idx;
            assert(m_mesh);
            while (m_mesh->garbage() && m_mesh->isValid(m_hnd) &&
                   m_mesh->isDeleted(m_hnd))
                --m_hnd.m_idx;
            return *this;
        }

    private:
        Halfedge m_hnd;
        const SurfaceMesh* m_mesh;
    };

    //! this class iterates linearly over all edges
    //! \sa edgesBegin(), edgesEnd()
    //! \sa VertexIterator, HalfedgeIterator, FaceIterator
    class EdgeIterator
    {
    public:
        //! Default constructor
        EdgeIterator(Edge e = Edge(), const SurfaceMesh* mesh = nullptr)
            : m_hnd(e), m_mesh(mesh)
        {
            if (m_mesh && m_mesh->garbage())
                while (m_mesh->isValid(m_hnd) && m_mesh->isDeleted(m_hnd))
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
            assert(m_mesh);
            while (m_mesh->garbage() && m_mesh->isValid(m_hnd) &&
                   m_mesh->isDeleted(m_hnd))
                ++m_hnd.m_idx;
            return *this;
        }

        //! pre-decrement iterator
        EdgeIterator& operator--()
        {
            --m_hnd.m_idx;
            assert(m_mesh);
            while (m_mesh->garbage() && m_mesh->isValid(m_hnd) &&
                   m_mesh->isDeleted(m_hnd))
                --m_hnd.m_idx;
            return *this;
        }

    private:
        Edge m_hnd;
        const SurfaceMesh* m_mesh;
    };

    //! this class iterates linearly over all faces
    //! \sa facesBegin(), facesEnd()
    //! \sa VertexIterator, HalfedgeIterator, EdgeIterator
    class FaceIterator
    {
    public:
        //! Default constructor
        FaceIterator(Face f = Face(), const SurfaceMesh* m = NULL)
            : m_hnd(f), m_mesh(m)
        {
            if (m_mesh && m_mesh->garbage())
                while (m_mesh->isValid(m_hnd) && m_mesh->isDeleted(m_hnd))
                    ++m_hnd.m_idx;
        }

        //! get the face the iterator refers to
        Face operator*() const { return m_hnd; }

        //! are two iterators equal?
        bool operator==(const FaceIterator& rhs) const
        {
            return (m_hnd == rhs.m_hnd);
        }

        //! are two iterators different?
        bool operator!=(const FaceIterator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment iterator
        FaceIterator& operator++()
        {
            ++m_hnd.m_idx;
            assert(m_mesh);
            while (m_mesh->garbage() && m_mesh->isValid(m_hnd) &&
                   m_mesh->isDeleted(m_hnd))
                ++m_hnd.m_idx;
            return *this;
        }

        //! pre-decrement iterator
        FaceIterator& operator--()
        {
            --m_hnd.m_idx;
            assert(m_mesh);
            while (m_mesh->garbage() && m_mesh->isValid(m_hnd) &&
                   m_mesh->isDeleted(m_hnd))
                --m_hnd.m_idx;
            return *this;
        }

    private:
        Face m_hnd;
        const SurfaceMesh* m_mesh;
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

    //! helper class for iterating through all faces using range-based
    //! for-loops. \sa faces()
    class FaceContainer
    {
    public:
        FaceContainer(FaceIterator begin, FaceIterator end)
            : m_begin(begin), m_end(end)
        {
        }
        FaceIterator begin() const { return m_begin; }
        FaceIterator end() const { return m_end; }
    private:
        FaceIterator m_begin, m_end;
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
        VertexAroundVertexCirculator(const SurfaceMesh* mesh = nullptr,
                                     Vertex v = Vertex())
            : m_mesh(mesh), m_active(true)
        {
            if (m_mesh)
                m_halfedge = m_mesh->halfedge(v);
        }

        //! are two circulators equal?
        bool operator==(const VertexAroundVertexCirculator& rhs) const
        {
            assert(m_mesh);
            return (m_active && (m_mesh == rhs.m_mesh) &&
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
            assert(m_mesh);
            m_halfedge = m_mesh->ccwRotatedHalfedge(m_halfedge);
            m_active = true;
            return *this;
        }

        //! pre-decrement (rotate clockwise)
        VertexAroundVertexCirculator& operator--()
        {
            assert(m_mesh);
            m_halfedge = m_mesh->cwRotatedHalfedge(m_halfedge);
            return *this;
        }

        //! get the vertex the circulator refers to
        Vertex operator*() const
        {
            assert(m_mesh);
            return m_mesh->toVertex(m_halfedge);
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
        const SurfaceMesh* m_mesh;
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
        HalfedgeAroundVertexCirculator(const SurfaceMesh* mesh = nullptr,
                                       Vertex v = Vertex())
            : m_mesh(mesh), m_active(true)
        {
            if (m_mesh)
                m_halfedge = m_mesh->halfedge(v);
        }

        //! are two circulators equal?
        bool operator==(const HalfedgeAroundVertexCirculator& rhs) const
        {
            assert(m_mesh);
            return (m_active && (m_mesh == rhs.m_mesh) &&
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
            assert(m_mesh);
            m_halfedge = m_mesh->ccwRotatedHalfedge(m_halfedge);
            m_active = true;
            return *this;
        }

        //! pre-decrement (rotate clockwise)
        HalfedgeAroundVertexCirculator& operator--()
        {
            assert(m_mesh);
            m_halfedge = m_mesh->cwRotatedHalfedge(m_halfedge);
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
        const SurfaceMesh* m_mesh;
        Halfedge m_halfedge;
        bool m_active; // helper for C++11 range-based for-loops
    };

    //! this class circulates through all incident faces of a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundVertexCirculator, HalfedgeAroundVertexCirculator, faces(Vertex)
    class FaceAroundVertexCirculator
    {
    public:
        //! construct with mesh and vertex (vertex should not be isolated!)
        FaceAroundVertexCirculator(const SurfaceMesh* m = NULL,
                                   Vertex v = Vertex())
            : m_mesh(m), m_active(true)
        {
            if (m_mesh)
            {
                m_halfedge = m_mesh->halfedge(v);
                if (m_halfedge.isValid() &&
                    m_mesh->isBoundary(m_halfedge))
                    operator++();
            }
        }

        //! are two circulators equal?
        bool operator==(const FaceAroundVertexCirculator& rhs) const
        {
            assert(m_mesh);
            return (m_active && (m_mesh == rhs.m_mesh) &&
                    (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const FaceAroundVertexCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        FaceAroundVertexCirculator& operator++()
        {
            assert(m_mesh && m_halfedge.isValid());
            do
            {
                m_halfedge = m_mesh->ccwRotatedHalfedge(m_halfedge);
            } while (m_mesh->isBoundary(m_halfedge));
            m_active = true;
            return *this;
        }

        //! pre-decrement (rotate clockwise)
        FaceAroundVertexCirculator& operator--()
        {
            assert(m_mesh && m_halfedge.isValid());
            do
                m_halfedge = m_mesh->cwRotatedHalfedge(m_halfedge);
            while (m_mesh->isBoundary(m_halfedge));
            return *this;
        }

        //! get the face the circulator refers to
        Face operator*() const
        {
            assert(m_mesh && m_halfedge.isValid());
            return m_mesh->face(m_halfedge);
        }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return m_halfedge.isValid(); }

        // helper for C++11 range-based for-loops
        FaceAroundVertexCirculator& begin()
        {
            m_active = !m_halfedge.isValid();
            return *this;
        }
        // helper for C++11 range-based for-loops
        FaceAroundVertexCirculator& end()
        {
            m_active = true;
            return *this;
        }

    private:
        const SurfaceMesh* m_mesh;
        Halfedge m_halfedge;
        // helper for C++11 range-based for-loops
        bool m_active;
    };

    //! this class circulates through the vertices of a face.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa HalfedgeAroundFaceCirculator, vertices(Face)
    class VertexAroundFaceCirculator
    {
    public:
        //! default constructor
        VertexAroundFaceCirculator(const SurfaceMesh* m = NULL, Face f = Face())
            : m_mesh(m), m_active(true)
        {
            if (m_mesh)
                m_halfedge = m_mesh->halfedge(f);
        }

        //! are two circulators equal?
        bool operator==(const VertexAroundFaceCirculator& rhs) const
        {
            assert(m_mesh);
            return (m_active && (m_mesh == rhs.m_mesh) &&
                    (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const VertexAroundFaceCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        VertexAroundFaceCirculator& operator++()
        {
            assert(m_mesh && m_halfedge.isValid());
            m_halfedge = m_mesh->nextHalfedge(m_halfedge);
            m_active = true;
            return *this;
        }

        //! pre-decrement (rotates clockwise)
        VertexAroundFaceCirculator& operator--()
        {
            assert(m_mesh && m_halfedge.isValid());
            m_halfedge = m_mesh->prevHalfedge(m_halfedge);
            return *this;
        }

        //! get the vertex the circulator refers to
        Vertex operator*() const
        {
            assert(m_mesh && m_halfedge.isValid());
            return m_mesh->toVertex(m_halfedge);
        }

        // helper for C++11 range-based for-loops
        VertexAroundFaceCirculator& begin()
        {
            m_active = false;
            return *this;
        }
        // helper for C++11 range-based for-loops
        VertexAroundFaceCirculator& end()
        {
            m_active = true;
            return *this;
        }

    private:
        const SurfaceMesh* m_mesh;
        Halfedge m_halfedge;
        // helper for C++11 range-based for-loops
        bool m_active;
    };

    //! this class circulates through all halfedges of a face.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundFaceCirculator, halfedges(Face)
    class HalfedgeAroundFaceCirculator
    {
    public:
        //! default constructur
        HalfedgeAroundFaceCirculator(const SurfaceMesh* m = NULL,
                                     Face f = Face())
            : m_mesh(m), m_active(true)
        {
            if (m_mesh)
                m_halfedge = m_mesh->halfedge(f);
        }

        //! are two circulators equal?
        bool operator==(const HalfedgeAroundFaceCirculator& rhs) const
        {
            assert(m_mesh);
            return (m_active && (m_mesh == rhs.m_mesh) &&
                    (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const HalfedgeAroundFaceCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        HalfedgeAroundFaceCirculator& operator++()
        {
            assert(m_mesh && m_halfedge.isValid());
            m_halfedge = m_mesh->nextHalfedge(m_halfedge);
            m_active = true;
            return *this;
        }

        //! pre-decrement (rotates clockwise)
        HalfedgeAroundFaceCirculator& operator--()
        {
            assert(m_mesh && m_halfedge.isValid());
            m_halfedge = m_mesh->prevHalfedge(m_halfedge);
            return *this;
        }

        //! get the halfedge the circulator refers to
        Halfedge operator*() const { return m_halfedge; }

        // helper for C++11 range-based for-loops
        HalfedgeAroundFaceCirculator& begin()
        {
            m_active = false;
            return *this;
        }
        // helper for C++11 range-based for-loops
        HalfedgeAroundFaceCirculator& end()
        {
            m_active = true;
            return *this;
        }

    private:
        const SurfaceMesh* m_mesh;
        Halfedge m_halfedge;
        // helper for C++11 range-based for-loops
        bool m_active;
    };

    //!@}
    //! \name Construction, destruction, assignment
    //!@{

    //! default constructor
    SurfaceMesh();

    //! destructor
    ~SurfaceMesh();

    //! copy constructor: copies \c rhs to \c *this. performs a deep copy of all
    //! properties.
    SurfaceMesh(const SurfaceMesh& rhs) { operator=(rhs); }

    //! assign \c rhs to \c *this. performs a deep copy of all properties.
    SurfaceMesh& operator=(const SurfaceMesh& rhs);

    //! assign \c rhs to \c *this. does not copy custom properties.
    SurfaceMesh& assign(const SurfaceMesh& rhs);

    //!@}
    //! \name File IO
    //!@{

    //! read mesh from file \c filename. file extension determines file type.
    //! \sa write(const std::string& filename)
    bool read(const std::string& filename,
              const IOOptions& options = IOOptions());

    //! write mesh to file \c filename. file extensions determines file type.
    //! \sa read(const std::string& filename)
    bool write(const std::string& filename,
               const IOOptions& options = IOOptions()) const;

    //!@}
    //! \name Add new elements by hand
    //!@{

    //! add a new vertex with position \c p
    Vertex addVertex(const Point& p);

    //! add a new face with vertex list \c vertices
    //! \sa addTriangle, addQuad
    Face addFace(const std::vector<Vertex>& vertices);

    //! add a new triangle connecting vertices \c v0, \c v1, \c v2
    //! \sa addFace, addQuad
    Face addTriangle(Vertex v0, Vertex v1, Vertex v2);

    //! add a new quad connecting vertices \c v0, \c v1, \c v2, \c v3
    //! \sa addTriangle, addFace
    Face addQuad(Vertex v0, Vertex v1, Vertex v2, Vertex v3);

    //!@}
    //! \name Memory Management
    //!@{

    //! returns number of (deleted and valid) vertices in the mesh
    size_t verticesSize() const { return m_vprops.size(); }

    //! returns number of (deleted and valid)halfedge in the mesh
    size_t halfedgesSize() const { return m_hprops.size(); }

    //! returns number of (deleted and valid)edges in the mesh
    size_t edgesSize() const { return m_eprops.size(); }

    //! returns number of (deleted and valid)faces in the mesh
    size_t facesSize() const { return m_fprops.size(); }

    //! returns number of vertices in the mesh
    size_t nVertices() const { return verticesSize() - m_deletedVertices; }

    //! returns number of halfedge in the mesh
    size_t nHalfedges() const { return halfedgesSize() - 2 * m_deletedEdges; }

    //! returns number of edges in the mesh
    size_t nEdges() const { return edgesSize() - m_deletedEdges; }

    //! returns number of faces in the mesh
    size_t nFaces() const { return facesSize() - m_deletedFaces; }

    //! returns true iff the mesh is empty, i.e., has no vertices
    bool isEmpty() const { return nVertices() == 0; }

    //! clear mesh: remove all vertices, edges, faces
    void clear();

    //! remove unused memory from vectors
    void freeMemory();

    //! reserve memory (mainly used in file readers)
    void reserve(size_t nvertices, size_t nedges, size_t nfaces);

    //! remove deleted elements
    void garbageCollection();

    //! returns whether vertex \c v is deleted
    //! \sa garbageCollection()
    bool isDeleted(Vertex v) const { return m_vdeleted[v]; }

    //! returns whether halfedge \c h is deleted
    //! \sa garbageCollection()
    bool isDeleted(Halfedge h) const { return m_edeleted[edge(h)]; }

    //! returns whether edge \c e is deleted
    //! \sa garbageCollection()
    bool isDeleted(Edge e) const { return m_edeleted[e]; }

    //! returns whether face \c f is deleted
    //! \sa garbageCollection()
    bool isDeleted(Face f) const { return m_fdeleted[f]; }


    //! return whether vertex \c v is valid, i.e. the index is stores
    //! it within the array bounds.
    bool isValid(Vertex v) const
    {
        return (0 <= v.idx()) && (v.idx() < (int)verticesSize());
    }

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

    //! returns whether the face \p f is valid.
    bool isValid(Face f) const
    {
        return (0 <= f.idx()) && (f.idx() < (int)facesSize());
    }

    //!@}
    //! \name Low-level connectivity
    //!@{

    //! returns an outgoing halfedge of vertex \c v.
    //! if \c v is a boundary vertex this will be a boundary halfedge.
    Halfedge halfedge(Vertex v) const { return m_vconn[v].m_halfedge; }

    //! set the outgoing halfedge of vertex \c v to \c h
    void setHalfedge(Vertex v, Halfedge h) { m_vconn[v].m_halfedge = h; }

    //! returns whether \c v is a boundary vertex
    bool isBoundary(Vertex v) const
    {
        Halfedge h(halfedge(v));
        return (!(h.isValid() && face(h).isValid()));
    }

    //! returns whether \c v is isolated, i.e., not incident to any edge
    bool isIsolated(Vertex v) const { return !halfedge(v).isValid(); }

    //! returns whether \c v is a manifold vertex (not incident to several patches)
    bool isManifold(Vertex v) const
    {
        // The vertex is non-manifold if more than one gap exists, i.e.
        // more than one outgoing boundary halfedge.
        int n(0);
        HalfedgeAroundVertexCirculator hit = halfedges(v), hend = hit;
        if (hit)
            do
            {
                if (isBoundary(*hit))
                    ++n;
            } while (++hit != hend);
        return n < 2;
    }

    //! returns the vertex the halfedge \c h points to
    inline Vertex toVertex(Halfedge h) const { return m_hconn[h].m_vertex; }

    //! returns the vertex the halfedge \c h emanates from
    inline Vertex fromVertex(Halfedge h) const
    {
        return toVertex(oppositeHalfedge(h));
    }

    //! sets the vertex the halfedge \c h points to to \c v
    inline void setVertex(Halfedge h, Vertex v) { m_hconn[h].m_vertex = v; }

    //! returns the face incident to halfedge \c h
    Face face(Halfedge h) const { return m_hconn[h].m_face; }

    //! sets the incident face to halfedge \c h to \c f
    void setFace(Halfedge h, Face f) { m_hconn[h].m_face = f; }

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

    //! returns whether h is a boundary halfege, i.e., if its face does not exist.
    inline bool isBoundary(Halfedge h) const { return !face(h).isValid(); }

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

    //! returns the face incident to the \c i'th halfedge of edge \c e. \c i has to be 0 or 1.
    Face face(Edge e, unsigned int i) const
    {
        assert(i <= 1);
        return face(halfedge(e, i));
    }

    //! returns whether \c e is a boundary edge, i.e., if one of its
    //! halfedges is a boundary halfedge.
    bool isBoundary(Edge e) const
    {
        return (isBoundary(halfedge(e, 0)) ||
                isBoundary(halfedge(e, 1)));
    }

    //! returns a halfedge of face \c f
    Halfedge halfedge(Face f) const { return m_fconn[f].m_halfedge; }

    //! sets the halfedge of face \c f to \c h
    void setHalfedge(Face f, Halfedge h) { m_fconn[f].m_halfedge = h; }

    //! returns whether \c f is a boundary face, i.e., it one of its edges is a boundary edge.
    bool isBoundary(Face f) const
    {
        Halfedge h = halfedge(f);
        Halfedge hh = h;
        do
        {
            if (isBoundary(oppositeHalfedge(h)))
                return true;
            h = nextHalfedge(h);
        } while (h != hh);
        return false;
    }

    //!@}
    //! \name Property handling
    //!@{

    //! add a object property of type \c T with name \c name and default value \c t.
    //! fails if a property named \c name exists already, since the name has to
    //! be unique. in this case it returns an invalid property
    template <class T>
    ObjectProperty<T> addObjectProperty(const std::string& name,
                                        const T t = T())
    {
        return ObjectProperty<T>(m_oprops.add<T>(name, t));
    }

    //! get the object property named \c name of type \c T. returns an invalid
    //! ObjectProperty if the property does not exist or if the type does not
    //! match.
    template <class T>
    ObjectProperty<T> getObjectProperty(const std::string& name) const
    {
        return ObjectProperty<T>(m_oprops.get<T>(name));
    }

    //! if a object property of type \c T with name \c name exists, it is
    //! returned.  otherwise this property is added (with default value \c t)
    template <class T>
    ObjectProperty<T> objectProperty(const std::string& name, const T t = T())
    {
        return ObjectProperty<T>(m_oprops.getOrAdd<T>(name, t));
    }

    //! remove the object property \c p
    template <class T>
    void removeObjectProperty(ObjectProperty<T>& p)
    {
        m_oprops.remove(p);
    }

    //! get the type_info \c T of face property named \c name. returns an
    //! typeid(void) if the property does not exist or if the type does not
    //! match.
    const std::type_info& getObjectPropertyType(const std::string& name)
    {
        return m_oprops.getType(name);
    }

    //! returns the names of all face properties
    std::vector<std::string> objectProperties() const
    {
        return m_oprops.properties();
    }

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

    //! get the type_info \c T of vertex property named \c name. returns an
    //! typeid(void) if the property does not exist or if the type does not
    //! match.
    const std::type_info& getVertexPropertyType(const std::string& name)
    {
        return m_vprops.getType(name);
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

    //! add a face property of type \c T with name \c name and default value \c
    //! t.  fails if a property named \c name exists already, since the name has
    //! to be unique.  in this case it returns an invalid property
    template <class T>
    FaceProperty<T> addFaceProperty(const std::string& name, const T t = T())
    {
        return FaceProperty<T>(m_fprops.add<T>(name, t));
    }

    //! get the face property named \c name of type \c T. returns an invalid
    //! VertexProperty if the property does not exist or if the type does not
    //! match.
    template <class T>
    FaceProperty<T> getFaceProperty(const std::string& name) const
    {
        return FaceProperty<T>(m_fprops.get<T>(name));
    }

    //! if a face property of type \c T with name \c name exists, it is
    //! returned.  otherwise this property is added (with default value \c t)
    template <class T>
    FaceProperty<T> faceProperty(const std::string& name, const T t = T())
    {
        return FaceProperty<T>(m_fprops.getOrAdd<T>(name, t));
    }

    //! remove the face property \c p
    template <class T>
    void removeFaceProperty(FaceProperty<T>& p)
    {
        m_fprops.remove(p);
    }

    //! get the type_info \c T of face property named \c name . returns an
    //! typeid(void) if the property does not exist or if the type does not
    //! match.
    const std::type_info& getFacePropertyType(const std::string& name)
    {
        return m_fprops.getType(name);
    }

    //! returns the names of all face properties
    std::vector<std::string> faceProperties() const
    {
        return m_fprops.properties();
    }

    //! prints the names of all properties
    void propertyStats() const;

    //!@}
    //! \name Iterators and circulators
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

    //! returns start iterator for faces
    FaceIterator facesBegin() const { return FaceIterator(Face(0), this); }

    //! returns end iterator for faces
    FaceIterator facesEnd() const
    {
        return FaceIterator(Face(facesSize()), this);
    }

    //! returns face container for C++11 range-based for-loops
    FaceContainer faces() const
    {
        return FaceContainer(facesBegin(), facesEnd());
    }

    //! returns circulator for faces around vertex \c v
    FaceAroundVertexCirculator faces(Vertex v) const
    {
        return FaceAroundVertexCirculator(this, v);
    }

    //! returns circulator for vertices of face \c f
    VertexAroundFaceCirculator vertices(Face f) const
    {
        return VertexAroundFaceCirculator(this, f);
    }

    //! returns circulator for halfedges of face \c f
    HalfedgeAroundFaceCirculator halfedges(Face f) const
    {
        return HalfedgeAroundFaceCirculator(this, f);
    }

    //!@}
    //! \name Navigators
    //!@{

    // forward declarations
    class VertexNavigator;
    class HalfedgeNavigator;
    class EdgeNavigator;
    class FaceNavigator;

    //! navigator object for vertices
    class VertexNavigator
    {
    public:
        //! construct from mesh and vertex
        VertexNavigator(const SurfaceMesh& mesh, Vertex v)
            : m_mesh(mesh), m_v(v)
        {
        }

        //! de-reference Vertex handle
        Vertex operator*() { return m_v; }

        //! get the position of the vertex
        const Point& position() { return m_mesh.position(m_v); }

        //! get HalfedgeNavigator object
        inline HalfedgeNavigator halfedge()
        {
            return HalfedgeNavigator(m_mesh, m_mesh.halfedge(m_v));
        }

    private:
        const SurfaceMesh& m_mesh;
        Vertex m_v;
    };

    //! navigator object for halfedges
    class HalfedgeNavigator
    {
    public:
        //! construct from SurfaceMesh \p mesh and Halfedge \p h
        HalfedgeNavigator(const SurfaceMesh& mesh, Halfedge h)
            : m_mesh(mesh), m_h(h)
        {
        }

        //! de-reference Halfedge handle
        Halfedge operator*() { return m_h; }

        //! get VertexNavigator object for the halfedge's *to* Vertex
        inline VertexNavigator toVertex()
        {
            return VertexNavigator(m_mesh, m_mesh.toVertex(m_h));
        }

        //! get VertexNavigator object for the halfedge's *from* Vertex
        inline VertexNavigator fromVertex()
        {
            return VertexNavigator(m_mesh, m_mesh.fromVertex(m_h));
        }

        //! get HalfedgeNavigator object for the *next* halfedge
        inline HalfedgeNavigator next()
        {
            return HalfedgeNavigator(m_mesh, m_mesh.nextHalfedge(m_h));
        }

        //! get HalfedgeNavigator object for the *previous* halfedge
        inline HalfedgeNavigator prev()
        {
            return HalfedgeNavigator(m_mesh, m_mesh.prevHalfedge(m_h));
        }

        //! get HalfedgeNavigator object for *opposite* halfedge
        inline HalfedgeNavigator opposite()
        {
            return HalfedgeNavigator(m_mesh, m_mesh.oppositeHalfedge(m_h));
        }

        //! get EdgeNavigator object for the corresponding Edge
        inline EdgeNavigator edge()
        {
            return EdgeNavigator(m_mesh, m_mesh.edge(m_h));
        }

        //! get FaceNavigator object for the incident Face
        inline FaceNavigator face()
        {
            return FaceNavigator(m_mesh, m_mesh.face(m_h));
        }

    private:
        const SurfaceMesh& m_mesh;
        Halfedge m_h;
    };

    //! navigator object for edges
    class EdgeNavigator
    {
    public:
        //! construct from SurfaceMesh \p mesh and Edge \p e
        EdgeNavigator(const SurfaceMesh& mesh, Edge e) : m_mesh(mesh), m_e(e) {}

        //! de-reference Edge handle
        Edge operator*() { return m_e; }

        //! get VertexNavigator object, \p i is 0 or 1
        inline VertexNavigator vertex(int i)
        {
            return VertexNavigator(m_mesh, m_mesh.vertex(m_e, i));
        }

        //! get HalfedgeNavigator object, \p i is 0 or 1
        inline HalfedgeNavigator halfedge(int i)
        {
            return HalfedgeNavigator(m_mesh, m_mesh.halfedge(m_e, i));
        }

    private:
        const SurfaceMesh& m_mesh;
        Edge m_e;
    };

    //! navigator object for faces
    class FaceNavigator
    {
    public:
        //! construct from SurfaceMesh \p mesh and Face \p f
        FaceNavigator(const SurfaceMesh& mesh, Face f) : m_mesh(mesh), m_f(f) {}

        //! de-reference Face handle
        Face operator*() { return m_f; }

        //! get HalfedgeNavigator object
        inline HalfedgeNavigator halfedge()
        {
            return HalfedgeNavigator(m_mesh, m_mesh.halfedge(m_f));
        }

    private:
        const SurfaceMesh& m_mesh;
        Face m_f;
    };

    //! create VertexNavigator object from Vertex \p v
    VertexNavigator nav(Vertex v) const { return VertexNavigator(*this, v); }

    //! create HalfedgeNavigator object from Halfedge \p h
    HalfedgeNavigator nav(Halfedge h) const
    {
        return HalfedgeNavigator(*this, h);
    }

    //! create EdgeNavigator object from Edge \p e
    EdgeNavigator nav(Edge e) const { return EdgeNavigator(*this, e); }

    //! create FaceNavigator object from Face \p f
    FaceNavigator nav(Face f) const { return FaceNavigator(*this, f); }

    //!@}
    //! \name Higher-level Topological Operations
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
    Halfedge insertVertex(Halfedge h0, Vertex v);

    //! find the halfedge from start to end
    Halfedge findHalfedge(Vertex start, Vertex end) const;

    //! find the edge (a,b)
    Edge findEdge(Vertex a, Vertex b) const;

    //! returns whether the mesh a triangle mesh. this function simply tests
    //! each face, and therefore is not very efficient.
    //! \sa trianglate(), triangulate(Face)
    bool isTriangleMesh() const;

    //! returns whether the mesh a quad mesh. this function simply tests
    //! each face, and therefore is not very efficient.
    bool isQuadMesh() const;

    //! triangulate the entire mesh, by calling triangulate(Face) for each face.
    //! \sa triangulate(Face)
    void triangulate();

    //! triangulate the face \c f.
    //! \sa triangulate()
    void triangulate(Face f);

    //! returns whether collapsing the halfedge \c v0v1 is topologically legal.
    //! \attention This function is only valid for triangle meshes.
    bool isCollapseOk(Halfedge v0v1);

    //! Collapse the halfedge \c h by moving its start vertex into its target
    //! vertex. For non-boundary halfedges this function removes one vertex, three
    //! edges, and two faces. For boundary halfedges it removes one vertex, two
    //! edges and one face.
    //! \attention This function is only valid for triangle meshes.
    //! \attention Halfedge collapses might lead to invalid faces. Call
    //! isCollapseOk(Halfedge) to be sure the collapse is legal.
    //! \attention The removed items are only marked as deleted. You have
    //! to call garbageCollection() to finally remove them.
    void collapse(Halfedge h);

    //! Split the face \c f by first adding point \c p to the mesh and then
    //! inserting edges between \c p and the vertices of \c f. For a triangle
    //! this is a standard one-to-three split.
    //! \sa split(Face, Vertex)
    Vertex split(Face f, const Point& p)
    {
        Vertex v = addVertex(p);
        split(f, v);
        return v;
    }

    //! Split the face \c f by inserting edges between \c p and the vertices
    //! of \c f. For a triangle this is a standard one-to-three split.
    //! \sa split(Face, const Point&)
    void split(Face f, Vertex v);

    //! Split the edge \c e by first adding point \c p to the mesh and then
    //! connecting it to the two vertices of the adjacent triangles that are
    //! opposite to edge \c e. Returns the halfedge pointing to \c p that is
    //! created by splitting the existing edge \c e.
    //!
    //! \attention This function is only valid for triangle meshes.
    //! \sa split(Edge, Vertex)
    Halfedge split(Edge e, const Point& p) { return split(e, addVertex(p)); }

    //! Split the edge \c e by connecting vertex \c v it to the two
    //! vertices of the adjacent triangles that are opposite to edge \c
    //! e. Returns the halfedge pointing to \c p that is created by splitting
    //! the existing edge \c e.
    //!
    //! \attention This function is only valid for triangle meshes.
    //! \sa split(Edge, Point)
    Halfedge split(Edge e, Vertex v);

    //! insert edge between the to-vertices v0 of h0 and v1 of h1.
    //! returns the new halfedge from v0 to v1.
    //! \attention h0 and h1 have to belong to the same face
    Halfedge insertEdge(Halfedge h0, Halfedge h1);

    //! Check whether flipping edge \c e is topologically
    //! \attention This function is only valid for triangle meshes.
    //! \sa flip(Edge)
    bool isFlipOk(Edge e) const;

    //! Flip the edge \c e . Removes the edge \c e and add an edge between the
    //! two vertices opposite to edge \c e of the two incident triangles.
    //! \attention This function is only valid for triangle meshes.
    //! \attention Flipping an edge may result in a non-manifold mesh, hence check
    //! for yourself whether this operation is allowed or not!
    //! \sa isFlipOk()
    void flip(Edge e);

    //! returns the valence (number of incident edges or neighboring
    //! vertices) of vertex \c v.
    size_t valence(Vertex v) const;

    //! returns the valence of face \c f (its number of vertices)
    size_t valence(Face f) const;

    //! deletes the vertex \c v from the mesh
    void deleteVertex(Vertex v);

    //! deletes the edge \c e from the mesh
    void deleteEdge(Edge e);

    //! deletes the face \c f from the mesh
    void deleteFace(Face f);

    //!@}
    //! \name Geometry-related Functions
    //!@{

    //! position of a vertex (read only)
    const Point& position(Vertex v) const { return m_vpoint[v]; }

    //! position of a vertex
    Point& position(Vertex v) { return m_vpoint[v]; }

    //! vector of point positions, re-implemented from \c GeometryObject
    std::vector<Point>& positions() { return m_vpoint.vector(); }

    //! compute the bounding box of the object
    BoundingBox bounds()
    {
        BoundingBox bb;
        for (auto p : positions())
            bb += p;
        return bb;
    }

    //! compute the length of edge \c e.
    Scalar edgeLength(Edge e) const
    {
        return norm(m_vpoint[vertex(e, 0)] - m_vpoint[vertex(e, 1)]);
    }

    //!@}

private:
    //! \name Allocate new elements
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

    //! allocate a new face, resize face properties accordingly.
    Face newFace()
    {
        if (facesSize() == PMP_MAX_INDEX - 1)
        {
            std::cerr << "newFace: cannot allocate face, max. index reached"
                      << std::endl;
            return Face();
        }

        m_fprops.pushBack();
        return Face(facesSize() - 1);
    }


    //!@}
    //! \name Helper functions
    //!@{

    //! make sure that the outgoing halfedge of vertex \c v is a boundary
    //! halfedge if \c v is a boundary vertex.
    void adjustOutgoingHalfedge(Vertex v);

    //! Helper for halfedge collapse
    void removeEdge(Halfedge h);

    //! Helper for halfedge collapse
    void removeLoop(Halfedge h);

    //! are there any deleted entities?
    inline bool garbage() const { return m_garbage; }

    //!@}

private:
    //! \name Private members
    //!@{

    friend SurfaceMeshIO;

    // property containers for each entity type and object
    PropertyContainer m_oprops;
    PropertyContainer m_vprops;
    PropertyContainer m_hprops;
    PropertyContainer m_eprops;
    PropertyContainer m_fprops;

    // point coordinates
    VertexProperty<Point> m_vpoint;

    // connectivity information
    VertexProperty<VertexConnectivity>     m_vconn;
    HalfedgeProperty<HalfedgeConnectivity> m_hconn;
    FaceProperty<FaceConnectivity>         m_fconn;

    // markers for deleted entities
    VertexProperty<bool> m_vdeleted;
    EdgeProperty<bool>   m_edeleted;
    FaceProperty<bool>   m_fdeleted;

    // numbers of deleted entities
    IndexType m_deletedVertices;
    IndexType m_deletedEdges;
    IndexType m_deletedFaces;

    // indicate garbage present
    bool m_garbage;

    // helper data for addFace()
    typedef std::pair<Halfedge, Halfedge> NextCacheEntry;
    typedef std::vector<NextCacheEntry> NextCache;
    std::vector<Vertex> m_addFaceVertices;
    std::vector<Halfedge> m_addFaceHalfedges;
    std::vector<bool> m_addFaceIsNew;
    std::vector<bool> m_addFaceNeedsAdjust;
    NextCache m_addFaceNextCache;

    //!@}
};

//! \name Output operators
//!@{

//! output a halfedge to a stream
inline std::ostream& operator<<(std::ostream& os, SurfaceMesh::Vertex v)
{
    return (os << 'v' << v.idx());
}

//! output a halfedge to a stream
inline std::ostream& operator<<(std::ostream& os, SurfaceMesh::Halfedge h)
{
    return (os << 'h' << h.idx());
}

//! output an edge to a stream
inline std::ostream& operator<<(std::ostream& os, SurfaceMesh::Edge e)
{
    return (os << 'e' << e.idx());
}

//! output a face to a stream
inline std::ostream& operator<<(std::ostream& os, SurfaceMesh::Face f)
{
    return (os << 'f' << f.idx());
}

//!@}

//=============================================================================
//!@}
//=============================================================================
} // namespace pmp
//=============================================================================
