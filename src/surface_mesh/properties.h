//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011-2013 by Graphics & Geometry Group, Bielefeld University
// Copyright (C) 2017 Daniel Sieger
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
#pragma once
//=============================================================================

#include <algorithm>
#include <string>
#include <typeinfo>
#include <vector>
#include <cassert>
#include <iostream>

//== NAMESPACE ================================================================

namespace surface_mesh {

//== CLASS DEFINITION =========================================================

class BasePropertyArray
{
public:
    //! Default constructor
    BasePropertyArray(const std::string& name) : m_name(name) {}

    //! Destructor.
    virtual ~BasePropertyArray() {}

    //! Reserve memory for n elements.
    virtual void reserve(size_t n) = 0;

    //! Resize storage to hold n elements.
    virtual void resize(size_t n) = 0;

    //! Free unused memory.
    virtual void freeMemory() = 0;

    //! Extend the number of elements by one.
    virtual void pushBack() = 0;

    //! Let two elements swap their storage place.
    virtual void swap(size_t i0, size_t i1) = 0;

    //! Return a deep copy of self.
    virtual BasePropertyArray* clone() const = 0;

    //! Return the type_info of the property
    virtual const std::type_info& type() = 0;

    //! Return the name of the property
    const std::string& name() const { return m_name; }

protected:
    std::string m_name;
};

//== CLASS DEFINITION =========================================================

template <class T>
class PropertyArray : public BasePropertyArray
{
public:
    typedef T                                    ValueType;
    typedef std::vector<ValueType>               VectorType;
    typedef typename VectorType::reference       Reference;
    typedef typename VectorType::const_reference ConstReference;

    PropertyArray(const std::string& name, T t = T())
        : BasePropertyArray(name), m_value(t)
    {
    }

public: // virtual interface of BasePropertyArray
    virtual void reserve(size_t n) { m_data.reserve(n); }

    virtual void resize(size_t n) { m_data.resize(n, m_value); }

    virtual void pushBack() { m_data.push_back(m_value); }

    virtual void freeMemory() { VectorType(m_data).swap(m_data); }

    virtual void swap(size_t i0, size_t i1)
    {
        T d(m_data[i0]);
        m_data[i0] = m_data[i1];
        m_data[i1] = d;
    }

    virtual BasePropertyArray* clone() const
    {
        PropertyArray<T>* p = new PropertyArray<T>(m_name, m_value);
        p->m_data           = m_data;
        return p;
    }

    virtual const std::type_info& type() { return typeid(T); }

public:
    //! Get pointer to array (does not work for T==bool)
    const T* data() const { return &m_data[0]; }

    //! Get Reference to the underlying vector
    std::vector<T>& vector() { return m_data; }

    //! Access the i'th element. No range check is performed!
    Reference operator[](int idx)
    {
        assert(size_t(idx) < m_data.size());
        return m_data[idx];
    }

    //! Const access to the i'th element. No range check is performed!
    ConstReference operator[](int idx) const
    {
        assert(size_t(idx) < m_data.size());
        return m_data[idx];
    }

private:
    VectorType m_data;
    ValueType  m_value;
};

// specialization for bool properties
template <>
inline const bool* PropertyArray<bool>::data() const
{
    assert(false);
    return NULL;
}

//== CLASS DEFINITION =========================================================

template <class T>
class Property
{
public:
    typedef typename PropertyArray<T>::Reference      Reference;
    typedef typename PropertyArray<T>::ConstReference ConstReference;

    friend class PropertyContainer;
    friend class SurfaceMesh;
    friend class PointSet;
    friend class EdgeSet;

public:
    Property(PropertyArray<T>* p = NULL) : m_parray(p) {}

    void reset() { m_parray = NULL; }

    operator bool() const { return m_parray != NULL; }

    Reference operator[](int i)
    {
        assert(m_parray != NULL);
        return (*m_parray)[i];
    }

    ConstReference operator[](int i) const
    {
        assert(m_parray != NULL);
        return (*m_parray)[i];
    }

    const T* data() const
    {
        assert(m_parray != NULL);
        return m_parray->data();
    }

    std::vector<T>& vector()
    {
        assert(m_parray != NULL);
        return m_parray->vector();
    }

private:
    PropertyArray<T>& array()
    {
        assert(m_parray != NULL);
        return *m_parray;
    }

    const PropertyArray<T>& array() const
    {
        assert(m_parray != NULL);
        return *m_parray;
    }

private:
    PropertyArray<T>* m_parray;
};

//== CLASS DEFINITION =========================================================

class PropertyContainer
{
public:
    // default constructor
    PropertyContainer() : m_size(0) {}

    // destructor (deletes all property arrays)
    virtual ~PropertyContainer() { clear(); }

    // copy constructor: performs deep copy of property arrays
    PropertyContainer(const PropertyContainer& rhs) { operator=(rhs); }

    // assignment: performs deep copy of property arrays
    PropertyContainer& operator=(const PropertyContainer& rhs)
    {
        if (this != &rhs)
        {
            clear();
            m_parrays.resize(rhs.n_properties());
            m_size = rhs.size();
            for (unsigned int i = 0; i < m_parrays.size(); ++i)
                m_parrays[i]    = rhs.m_parrays[i]->clone();
        }
        return *this;
    }

    // returns the current size of the property arrays
    size_t size() const { return m_size; }

    // returns the number of property arrays
    size_t n_properties() const { return m_parrays.size(); }

    // returns a vector of all property names
    std::vector<std::string> properties() const
    {
        std::vector<std::string> names;
        for (unsigned int i = 0; i < m_parrays.size(); ++i)
            names.push_back(m_parrays[i]->name());
        return names;
    }

    // add a property with name \c name and default value \c t
    template <class T>
    Property<T> add(const std::string& name, const T t = T())
    {
        // if a property with this name already exists, return an invalid property
        for (unsigned int i = 0; i < m_parrays.size(); ++i)
        {
            if (m_parrays[i]->name() == name)
            {
                std::cerr << "[PropertyContainer] A property with name \""
                          << name
                          << "\" already exists. Returning invalid property.\n";
                return Property<T>();
            }
        }

        // otherwise add the property
        PropertyArray<T>* p = new PropertyArray<T>(name, t);
        p->resize(m_size);
        m_parrays.push_back(p);
        return Property<T>(p);
    }

    // get a property by its name. returns invalid property if it does not exist.
    template <class T>
    Property<T> get(const std::string& name) const
    {
        for (unsigned int i = 0; i < m_parrays.size(); ++i)
            if (m_parrays[i]->name() == name)
                return Property<T>(
                    dynamic_cast<PropertyArray<T>*>(m_parrays[i]));
        return Property<T>();
    }

    // returns a property if it exists, otherwise it creates it first.
    template <class T>
    Property<T> getOrAdd(const std::string& name, const T t = T())
    {
        Property<T> p = get<T>(name);
        if (!p)
            p = add<T>(name, t);
        return p;
    }

    // get the type of property by its name. returns typeid(void) if it does not exist.
    const std::type_info& getType(const std::string& name)
    {
        for (unsigned int i = 0; i < m_parrays.size(); ++i)
            if (m_parrays[i]->name() == name)
                return m_parrays[i]->type();
        return typeid(void);
    }

    // delete a property
    template <class T>
    void remove(Property<T>& h)
    {
        std::vector<BasePropertyArray *>::iterator it  = m_parrays.begin(),
                                                   end = m_parrays.end();
        for (; it != end; ++it)
        {
            if (*it == h.m_parray)
            {
                delete *it;
                m_parrays.erase(it);
                h.reset();
                break;
            }
        }
    }

    // delete all properties
    void clear()
    {
        for (unsigned int i = 0; i < m_parrays.size(); ++i)
            delete m_parrays[i];
        m_parrays.clear();
        m_size = 0;
    }

    // reserve memory for n entries in all arrays
    void reserve(size_t n) const
    {
        for (unsigned int i = 0; i < m_parrays.size(); ++i)
            m_parrays[i]->reserve(n);
    }

    // resize all arrays to size n
    void resize(size_t n)
    {
        for (unsigned int i = 0; i < m_parrays.size(); ++i)
            m_parrays[i]->resize(n);
        m_size = n;
    }

    // free unused space in all arrays
    void freeMemory() const
    {
        for (unsigned int i = 0; i < m_parrays.size(); ++i)
            m_parrays[i]->freeMemory();
    }

    // add a new element to each vector
    void pushBack()
    {
        for (unsigned int i = 0; i < m_parrays.size(); ++i)
            m_parrays[i]->pushBack();
        ++m_size;
    }

    // swap elements i0 and i1 in all arrays
    void swap(size_t i0, size_t i1) const
    {
        for (unsigned int i = 0; i < m_parrays.size(); ++i)
            m_parrays[i]->swap(i0, i1);
    }

private:
    std::vector<BasePropertyArray*> m_parrays;
    size_t                          m_size;
};

//=============================================================================
} // namespace surface_mesh
//=============================================================================
