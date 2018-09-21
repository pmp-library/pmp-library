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

#include <pmp/EdgeSet.h>
#include <pmp/io/IOOptions.h>

//=============================================================================

namespace pmp {

class SurfaceMeshIO;

//=============================================================================

//! \addtogroup geometry geometry
//!@{

//=============================================================================

//! A halfedge data structure for polygonal meshes.
class SurfaceMesh : public EdgeSet
{

public:
    //! \name Topology Types
    //!@{

    //! this type represents a face (internally it is basically an index)
    //! \sa Vertex, Halfedge, Edge
    struct Face : public BaseHandle
    {
        //! default constructor (with invalid index)
        explicit Face(IndexType idx = PMP_MAX_INDEX) : BaseHandle(idx) {}
    };

    //!@}
    //! \name Connectivity Types
    //!@{

    //! This type stores the halfedge connectivity
    //! \sa VertexConnectivity, FaceConnectivity
    struct HalfedgeFaceConnectivity
    {
        //! face incident to halfedge
        Face m_face;
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

    //!@}
    //! \name Iterator Types
    //!@{

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
                    m_mesh->isSurfaceBoundary(m_halfedge))
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
            } while (m_mesh->isSurfaceBoundary(m_halfedge));
            m_active = true;
            return *this;
        }

        //! pre-decrement (rotate clockwise)
        FaceAroundVertexCirculator& operator--()
        {
            assert(m_mesh && m_halfedge.isValid());
            do
                m_halfedge = m_mesh->cwRotatedHalfedge(m_halfedge);
            while (m_mesh->isSurfaceBoundary(m_halfedge));
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
    virtual ~SurfaceMesh();

    //! copy constructor: copies \c rhs to \c *this. performs a deep copy of all
    //! properties.
    SurfaceMesh(const SurfaceMesh& rhs) : EdgeSet(rhs) { operator=(rhs); }

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
              const IOOptions& options = IOOptions()) override;

    //! write mesh to file \c filename. file extensions determines file type.
    //! \sa read(const std::string& filename)
    bool write(const std::string& filename,
               const IOOptions& options = IOOptions()) const override;

    //!@}
    //! \name Add new elements by hand
    //!@{

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

    //! returns number of (deleted and valid)faces in the mesh
    size_t facesSize() const { return m_fprops.size(); }

    //! returns number of faces in the mesh
    size_t nFaces() const { return facesSize() - m_deletedFaces; }

    //! clear mesh: remove all vertices, edges, faces
    void clear() override;

    //! remove unused memory from vectors
    void freeMemory() override;

    //! reserve memory (mainly used in file readers)
    void reserve(size_t nvertices, size_t nedges, size_t nfaces);

    // tell the compiler that we're overloading the superclass' method here
    using PointSet::isDeleted;
    using EdgeSet::isDeleted;

    //! returns whether face \c f is deleted
    //! \sa garbageCollection()
    bool isDeleted(Face f) const { return m_fdeleted[f]; }

    // tell compiler that we want to use the superclass method isValid and
    // overload it
    using PointSet::isValid;
    using EdgeSet::isValid;

    //! returns whether the face \p f is valid.
    bool isValid(Face f) const
    {
        return (0 <= f.idx()) && (f.idx() < (int)facesSize());
    }

    //!@}
    //! \name Low-level connectivity
    //!@{

    using EdgeSet::halfedge;
    using EdgeSet::halfedges;

    //! returns whether \c e is a boundary edge, i.e., if one of its
    //! halfedges is a boundary halfedge.
    bool isSurfaceBoundary(Edge e) const
    {
        return (isSurfaceBoundary(halfedge(e, 0)) ||
                isSurfaceBoundary(halfedge(e, 1)));
    }

    //! returns whether \c v is a boundary vertex
    bool isSurfaceBoundary(Vertex v) const
    {
        Halfedge h(halfedge(v));
        return (!(h.isValid() && face(h).isValid()));
    }

    //! returns whether \c v is a manifold vertex (not incident to several patches)
    bool isTwoManifold(Vertex v) const
    {
        // The vertex is non-manifold if more than one gap exists, i.e.
        // more than one outgoing boundary halfedge.
        int n(0);
        HalfedgeAroundVertexCirculator hit = halfedges(v), hend = hit;
        if (hit)
            do
            {
                if (isSurfaceBoundary(*hit))
                    ++n;
            } while (++hit != hend);
        return n < 2;
    }

    //! returns the face incident to halfedge \c h
    Face face(Halfedge h) const { return m_hfconn[h].m_face; }

    //! sets the incident face to halfedge \c h to \c f
    void setFace(Halfedge h, Face f) { m_hfconn[h].m_face = f; }

    //! returns whether h is a boundary halfege, i.e., if its face does not exist.
    bool isSurfaceBoundary(Halfedge h) const { return !face(h).isValid(); }

    //! returns the face incident to the \c i'th halfedge of edge \c e. \c i has to be 0 or 1.
    Face face(Edge e, unsigned int i) const
    {
        assert(i <= 1);
        return face(halfedge(e, i));
    }

    //! returns a halfedge of face \c f
    Halfedge halfedge(Face f) const { return m_fconn[f].m_halfedge; }

    using EdgeSet::setHalfedge;

    //! sets the halfedge of face \c f to \c h
    void setHalfedge(Face f, Halfedge h) { m_fconn[f].m_halfedge = h; }

    //! returns whether \c f is a boundary face, i.e., it one of its edges is a boundary edge.
    bool isSurfaceBoundary(Face f) const
    {
        Halfedge h = halfedge(f);
        Halfedge hh = h;
        do
        {
            if (isSurfaceBoundary(oppositeHalfedge(h)))
                return true;
            h = nextHalfedge(h);
        } while (h != hh);
        return false;
    }

    //!@}
    //! \name Property handling
    //!@{

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
    virtual void propertyStats() const override;

    //!@}
    //! \name Iterators and circulators
    //!@{

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

    // Tell the compiler that we're overloading superclass method.
    using PointSet::vertices;
    using EdgeSet::vertices;

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

    using EdgeSet::insertVertex;

    //! Subdivide the edge \c e = (v0,v1) by splitting it into the two edge
    //! (v0,v) and (v,v1). Note that this function does not introduce any other
    //! edge or faces. It simply splits the edge. Returns halfedge that points to \c p.
    //! \sa insertVertex(Edge, Point)
    //! \sa insertVertex(Edge, Vertex)
    virtual Halfedge insertVertex(Halfedge h0, Vertex v) override;

    //! insert edge between the to-vertices v0 of h0 and v1 of h1.
    //! returns the new halfedge from v0 to v1.
    //! \attention h0 and h1 have to belong to the same face
    Halfedge insertEdge(Halfedge h0, Halfedge h1);

    using EdgeSet::insertEdge;

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

    using EdgeSet::valence;

    //! returns the valence of face \c f (its number of vertices)
    size_t valence(Face f) const;

    //! deletes the vertex \c v from the mesh
    virtual void deleteVertex(Vertex v) override;

    //! deletes the edge \c e from the mesh
    virtual void deleteEdge(Edge e) override;

    //! deletes the face \c f from the mesh
    void deleteFace(Face f);

    //!@}

protected:
    //! \name Garbage Collection
    //!@{

    //! initialize garbage collection
    virtual void beginGarbage() override;

    //! finalize garbage collection
    virtual void finalizeGarbage() override;

    //!@}

private:
    //! \name Allocate new elements
    //!@{

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

    //!@}

private:
    //! \name Private members
    //!@{

    friend SurfaceMeshIO;
    //friend bool readPoly(SurfaceMesh& mesh, const std::string& filename);

    PropertyContainer m_fprops;

    HalfedgeProperty<HalfedgeFaceConnectivity> m_hfconn;
    FaceProperty<FaceConnectivity> m_fconn;

    FaceProperty<bool> m_fdeleted;

    size_t m_deletedFaces;

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
