#pragma once
#include <windows.h>

#define STAMPCHECK -1


class MHTimeStamp
{
public:
    MHTimeStamp() : m_stamp(0) {}

    MHTimeStamp(LONGLONG stamp) : m_stamp(stamp)
    {}

    LONGLONG increment()
    {
        return ::InterlockedIncrement64(&m_stamp);
    }

    LONGLONG stamp() const
    {
        return ::InterlockedCompareExchange64(&m_stamp, STAMPCHECK, STAMPCHECK);
    }

    bool operator == (const MHTimeStamp& right) const
    {
        return stamp() == right.stamp();
    }

    bool operator != (const MHTimeStamp& right) const
    {
        return stamp() != right.stamp();
    }

    LONGLONG operator ++ (int)
    {
        return increment();
    }

    const MHTimeStamp& operator = (const MHTimeStamp& right)
    {
        if (this == &right)
            return *this;

        InterlockedExchange64(&(m_stamp), right.stamp());
        return *this;
    }

protected:
    mutable LONGLONG volatile m_stamp;
};