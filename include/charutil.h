#pragma once
#include <crtdefs.h>

// By Dan Bernstein
inline size_t hashAChar(const char * pChar)
{
    size_t hash = 5381;
    int c = *pChar;

    while (c)
    {
        hash = (hash << 5) + hash + c - 'A';
        pChar++;
        c = *pChar;
    }

    return hash;

}

class UCharString
{
public:
    UCharString(const char* pStr) : m_value(hashAChar(pStr)) {}
    UCharString(const UCharString& src) : m_value(src.m_value) {}

    bool operator < (const UCharString& right) const
    {
        return m_value < right.m_value;
    }

    UCharString& operator = (const UCharString& right)
    {
        if (this != &right)
            m_value = right.m_value;

        return *this;
    }

    bool operator == (const UCharString& right) const
    {
        if (this == &right)
            return true;

        return m_value == right.m_value;
    }

    bool operator != (const UCharString& right) const
    {
        return !(operator == (right));
    }


protected:
    size_t m_value;
};