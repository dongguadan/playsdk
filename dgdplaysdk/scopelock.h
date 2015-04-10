#ifdef WIN32
#pragma once
#include <Windows.h>

class CCriticalSection
{
    CCriticalSection(const CCriticalSection &refCritSec);
    CCriticalSection &operator=(const CCriticalSection &refCritSec);
    CRITICAL_SECTION m_CritSec;
public:
    CCriticalSection()
    {
        InitializeCriticalSection(&m_CritSec);
    };
    ~CCriticalSection()
    {
        DeleteCriticalSection(&m_CritSec);
    };

    void Lock()
    {
        EnterCriticalSection(&m_CritSec);
    };
    void Unlock()
    {
        LeaveCriticalSection(&m_CritSec);
    };
};

class CScopeLock
{
    CScopeLock(const CScopeLock &refAutoLock);
    CScopeLock &operator=(const CScopeLock &refAutoLock);
protected:
    CCriticalSection *m_pLock;
public:
    CScopeLock(CCriticalSection *plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };
    ~CScopeLock()
    {
        m_pLock->Unlock();
    };
};
#endif
