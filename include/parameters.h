#pragma once
#include "charutil.h"
#include <map>
#include "refcounted.h"

struct Parameter
{
public:

    enum Type
    {
        eLongLong = 0
        , eFloat
        , eDouble
        , eInt
        , eUInt
        , eCharString
        , ePointer
        , eBool
        , eObject
        , eSharedPtr
        , eUnKnown
    };
    Parameter()
    {
        ::memset(&m_value, 0, sizeof(m_value));
        m_type = eUnKnown;
    }

    Parameter(const Parameter& source)
        : m_value(source.m_value)
        , m_sharedPtr(source.m_sharedPtr)
        , m_type(source.m_type)
    {

    }

    void            asLongLong(long long value) { m_value.m_longlong = value; m_type = eLongLong; }
    long long       asLongLong(void) const      { return m_value.m_longlong; }

    void            asFloat(float value)        { m_value.m_float = value; m_type = eFloat; }
    float           asFloat(void) const         { return m_value.m_float; }

    void            asDouble(double value)      { m_value.m_double = value; m_type = eDouble; }
    double          asDouble(void) const        { return m_value.m_double; }

    void            asInt(int value)            { m_value.m_int = value; m_type = eInt; }
    int             asInt(void) const           { return m_value.m_int; }

    void            asUInt(unsigned int value)  { m_value.m_uint = value; m_type = eUInt; }
    unsigned int    asUInt(void) const          { return m_value.m_uint; }

    void            asCharString(const char* value) { m_value.m_char_pointer = value; m_type = eCharString; }
    const char*     asCharString(void) const    { return m_value.m_char_pointer; }

    void            asPointer(void* value)      { m_value.m_pointer = value; m_type = ePointer; }
    void*           asPointer(void) const       { return m_value.m_pointer; }

    void            asBool(bool value)          { m_value.m_bool = value; m_type = eBool; }
    bool            asBool(void) const          { return m_value.m_bool; }

    template <typename T>
    void            asObject(T* value)          { m_value.m_pointer = (void*)(value);  m_type = eObject; }
    template <typename T>
    T*              asObject(void) const        { return (T *)(m_value.m_pointer); }

    template <typename T>
    void            asSharedPtr(MHSharedPtr<T> spObj)
    {
        m_type = eSharedPtr;
        m_sharedPtr = translateToGeneral(spObj);
    }

    template <typename T>
    MHSharedPtr<T>   asSharedPtr() const
    {
        return translateToSpecific<T>(m_sharedPtr);
    }

    Parameter& operator = (const Parameter& right)
    {
        if (this == &right)
            return *this;
        m_value = right.m_value;

        m_sharedPtr = right.m_sharedPtr;

        m_type = right.m_type;

        return *this;

    }

    const Type type() const
    {
        return m_type;
    }
protected:
    mutable union
    {
        long long       m_longlong;
        float           m_float;
        double          m_double;
        int             m_int;
        unsigned int    m_uint;
        const char*     m_char_pointer;
        void*           m_pointer;
        bool            m_bool;
    } m_value;
    mutable GeneralSharedPtr m_sharedPtr;
    Type                m_type;
};

class ParameterTable
{
public:
    typedef std::map<UCharString, Parameter> ParameterMap;

    ParameterTable() {}

    ParameterTable(const ParameterTable& source)
    {
        clear();
        m_map.insert(source.m_map.begin(), source.m_map.end());
    }

    ParameterTable& operator = (const ParameterTable& right)
    {
        if (this == &right)
            return *this;

        clear();
        m_map.insert(right.m_map.begin(), right.m_map.end());
        return *this;
    }

    Parameter& operator[](const UCharString key)
    {
        return m_map[key];
    }

    const Parameter& operator[](const UCharString key) const
    {
        return m_map[key];
    }

    bool find(const UCharString key)
    {
        return m_map.find(key) != m_map.end();
    }

    bool addParameter(const UCharString key, Parameter value)
    {
        if (find(key))
            return false;

        m_map[key] = value;
    }

    void clear()
    {
        m_map.clear();
    }

protected:
    mutable ParameterMap m_map;
};