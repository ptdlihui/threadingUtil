#pragma once
#include "windows.h"

#define UNLOCKED 0
#define LOCKED 1
#define LOCKCHECK 2
#define INVALIDLOCK 3

template <typename T>
class LockedContainer
{
public:
    LockedContainer() : m_lockedFlag(UNLOCKED)
    {}
    LockedContainer(const T& content)
        : m_content(content)
        , m_lockedFlag(UNLOCKED)
    {}

    T& checkout()
    {
        return m_content;
    }

    bool isLocked()
    {
        return !!(::InterlockedCompareExchange64(&m_lockedFlag, INVALIDLOCK, LOCKCHECK));
    }

    bool Lock(bool bWait = true)
    {
        if (bWait)
        {
            while (!lockImp()) {};
            return true;
        }

        return lockImp();
    }

    bool UnLock(bool bWait = true)
    {
        if (bWait)
        {
            while (!unlockImp()) {};
            return true;
        }

        return unlockImp();
    }


    bool updateContent(const T& content, bool bWait = false)
    {
        if (Lock(bWait) == false)
            return false;

        m_content = content;
        UnLock();
        return true;
    }

protected:
    bool lockImp()
    {
        return ::InterlockedCompareExchange64(&m_lockedFlag, LOCKED, UNLOCKED) == UNLOCKED;
    }

    bool unlockImp()
    {
        return ::InterlockedCompareExchange64(&m_lockedFlag, UNLOCKED, LOCKED) == LOCKED;
    }
    
protected:
    T m_content;
    volatile LONGLONG m_lockedFlag;
};