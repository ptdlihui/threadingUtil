#pragma once
#include <windows.h>

class MHRefCounted
{
public:
    MHRefCounted(void);

    // For performance reasons, these should not be virtual for released code.
    // If you want virtuals for debugging, be sure to restore these lines before
    // checking this file in.
    void            AddRef(void) const;
    void            Release(void) const;
    unsigned int    RefCount(void) const;

private:
    MHRefCounted(const MHRefCounted &that);
    void            operator=   (const MHRefCounted &that);

protected:
    virtual        ~MHRefCounted(void) { }

    mutable LONG volatile m_refCount;
};


inline MHRefCounted::MHRefCounted()
    : m_refCount(0)
{
}

inline void MHRefCounted::AddRef(void) const
{
    ::InterlockedIncrement(&m_refCount);
}

inline void MHRefCounted::Release(void) const
{
    if (::InterlockedDecrement(&m_refCount) == 0)
        delete this;
}

inline unsigned int MHRefCounted::RefCount(void) const
{
    return (unsigned int)m_refCount;
}

///////////////////////////////////////////////////////////////////////////////
//
// Auto-pointer smart pointer intended for use with MHRefCounted classes.
//
template <typename T>
class MHSharedPtr
{
public:
    MHSharedPtr(void) : m_pData(NULL)         { }
    MHSharedPtr(const MHSharedPtr& that) : m_pData(that.m_pData) { if (m_pData) m_pData->AddRef(); }
    MHSharedPtr(T* pData) : m_pData(pData)        { if (m_pData) m_pData->AddRef(); }
    ~MHSharedPtr()                                               { if (m_pData) m_pData->Release(); }

    void        operator=   (const MHSharedPtr& that)     { setData(that.m_pData); }
    void        operator=   (T* pData)                  { setData(pData); }

    const T*    operator->  (void) const        { return m_pData; }
    T*          operator->  (void)              { return m_pData; }

    operator T*             (void)              { return m_pData; }
    operator const T*       (void) const        { return m_pData; }

    const T*    get(void) const        { return m_pData; }
    T*          get(void)              { return m_pData; }

private:
    void setData(T* pData)
    {
        if (pData)
            pData->AddRef();
        if (m_pData)
            m_pData->Release();
        m_pData = pData;
    }

    T*  m_pData;
};

typedef MHSharedPtr<MHRefCounted> GeneralSharedPtr;

template <typename T>
GeneralSharedPtr translateToGeneral(MHSharedPtr<T> ptr)
{
    MHRefCounted* pObj = static_cast<MHRefCounted*>(ptr.get());
    return GeneralSharedPtr(pObj);
}

template <typename T>
MHSharedPtr<T> translateToSpecific(GeneralSharedPtr ptr)
{
    T* pObj = static_cast<T*>(ptr.get());
    return MHSharedPtr<T>(pObj);
}


template <typename T>
class RefCountedWrapper : public MHRefCounted
{
public:
    RefCountedWrapper(T* pSelf) : m_pSelf(pSelf) {};
    ~RefCountedWrapper()
    {
        clear();
    }

    T* Data() { return m_pSelf; };
    const T* Data() const { return m_pSelf; }

    void clear()
    {
        if (m_pSelf)
        {
            delete m_pSelf;
            m_pSelf = nullptr;
        }
    }
protected:
    T* m_pSelf;
};


