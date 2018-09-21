//=============================================================================
// Copyright (C) 2017-2018 The pmp-library developers
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

#include <pmp/properties.h>
#include <pmp/types.h>
#include <pmp/BoundingBox.h>

#include <map>
#include <vector>
#include <limits>
#include <numeric>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup geometry geometry
//!@{

//=============================================================================

//! Base class for geometric objects
class GeometryObject
{
public:
    //! \name Handle Types
    //!@{

    //! Base class for all entity handles types (internally it is basically
    //! an index).
    class BaseHandle
    {
    public:
        //! constructor
        explicit BaseHandle(IndexType idx = PMP_MAX_INDEX) : m_idx(idx) {}

        //! Get the underlying index of this handle
        int idx() const { return m_idx; }

        //! reset handle to be invalid (index=PMP_MAX_INDEX.)
        void reset() { m_idx = PMP_MAX_INDEX; }

        //! return whether the handle is valid, i.e., the index is not equal to PMP_MAX_INDEX.
        bool isValid() const { return m_idx != PMP_MAX_INDEX; }

        //! are two handles equal?
        bool operator==(const BaseHandle& rhs) const
        {
            return m_idx == rhs.m_idx;
        }

        //! are two handles different?
        bool operator!=(const BaseHandle& rhs) const
        {
            return m_idx != rhs.m_idx;
        }

        //! compare operator useful for sorting handles
        bool operator<(const BaseHandle& rhs) const
        {
            return m_idx < rhs.m_idx;
        }

    private:
        friend class SurfaceMesh;
        friend class PointSet;
        friend class EdgeSet;
        IndexType m_idx;
    };

    //@}
    //! \name Property Types
    //@{

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
    //! \name Construction, destruction, assignment
    //!@{

    //! default constructor
    GeometryObject() { m_oprops.pushBack(); }

    //! destructor
    virtual ~GeometryObject(){};

    //! copy constructor: copies \c rhs to \c *this. performs a deep copy of all
    //! properties.
    GeometryObject(const GeometryObject& rhs) { operator=(rhs); }

    //! assign \c rhs to \c *this. performs a deep copy of all properties.
    GeometryObject& operator=(const GeometryObject& rhs)
    {
        if (this != &rhs)
        {
            // deep copy of property containers
            m_oprops = rhs.m_oprops;
            m_garbage = rhs.m_garbage;
        }
        return *this;
    }

    //! assign \c rhs to \c *this. does not copy custom properties.
    GeometryObject& assign(const GeometryObject& rhs)
    {
        if (this != &rhs)
        {
            m_oprops.clear();
            m_oprops.resize(1);
        }
        return *this;
    }

    //!@}
    //! \name Memory Management
    //!@{

    //! clear object: remove & free all properties
    void clear()
    {
        m_oprops.clear();
        m_oprops.resize(1);
    }

    //! free all memory from object properties
    void freeMemory() { m_oprops.freeMemory(); }

    //! reserve memory (mainly used in file readers)
    void reserve() { m_oprops.reserve(1); }

    //! remove deleted elements
    void garbageCollection()
    {
        m_garbageprops.clear();
        beginGarbage();
        finalizeGarbage();
    }

    //!@}
    //! \name Property Handling
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

    //!@}
    //! \name Geometry Interface
    //!@{

    //! return a vector of points compromising the geometry of the object
    virtual std::vector<Point>& positions() = 0;

    //! compute the bounding box of the object
    BoundingBox bounds()
    {
        BoundingBox bb;
        for (auto p : positions())
            bb += p;
        return bb;
    }

    //!@}

protected:
    //! \name Garbage Collection
    //!@{
    //! Garbage collection has been split into three methods:
    //! - garbageCollection (initiating the process)
    //! - beginGarbage (overridable, remove elements)
    //! and
    //! - finalizeGarbage (overridable, resize vectors)
    //!
    //! This makes extending the mechanism easier for subclasses.
    //! Subclasses can access the count of initial vertices and
    //! the count of remaining vertices after the cleanup at the
    //! same time (during beginGarbage) and access the according
    //! vertex properties. Then in finalizeGarbage, these vectors
    //! are resized accordingly.

    //! collect information and delete vertices, edges, etc.
    virtual void beginGarbage() = 0;

    //! resize the property vectors accordingly
    virtual void finalizeGarbage() { m_garbage = false; };

    //! are there any deleted entities?
    inline bool garbage() const { return m_garbage; }

    //! sets the garbage flag to true (means garbage collection has to be run)
    inline void setGarbage() { m_garbage = true; }

    //!@}
    //! \name Protected Members
    //!@{
    PropertyContainer m_oprops; //!< store object properties
    bool m_garbage;             //!< indicate garbage present

    //! during garbage collection, e.g. current vertex count must be
    //! saved. this map temporarily saves those values and can be used from
    //! subclasses, too. will be cleared after each garbage collection
    //! run.
    std::map<std::string, int> m_garbageprops;

    //!@}
};

//=============================================================================
//!@}
//=============================================================================
} // namespace pmp
//=============================================================================
