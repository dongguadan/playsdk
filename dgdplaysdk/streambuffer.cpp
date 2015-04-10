#include "stdafx.h"
#include <string.h>
#include "common.h"
#include "streambuffer.h"

#ifndef WIN32
#include <unistd.h>
#endif

CStreamBuffer::CStreamBuffer()
{
	memset(m_pBufferFrameInfo, 0, sizeof(m_pBufferFrameInfo));
    m_pBufferData      = NULL;
    m_pReadpos         = NULL;
    m_pWritePos        = NULL;
    m_nBufferReadPos   = 0;
    m_nBufferWritePos  = 0;
    m_nBufferFrameNum  = 0;
    m_lCurrentTs       = 0;
    m_nMaxBufFrameNum  = DEFINE_FRAME_NUM;
    m_nBufferSize      = 0;
    m_nBufferEmptySize = 0;
    m_bCanRead         = 0;
    m_lBufTime         = 0;
#ifdef WIN32
    InitializeCriticalSection(&m_csBuf);
    m_EventBufferCanWrite = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_EventSignal         = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
	pthread_mutex_init(&m_csBuf, NULL);
#endif
}

CStreamBuffer::~CStreamBuffer()
{
	if (m_pBufferData != NULL)
    {
        delete [] m_pBufferData;
        m_pBufferData = NULL;
    }
#ifdef WIN32
    DeleteCriticalSection(&m_csBuf);
    CloseHandle(m_EventBufferCanWrite);
    CloseHandle(m_EventSignal);
#else
    pthread_mutex_destroy(&m_csBuf);
#endif
}

int CStreamBuffer::AlloceBuffer(int buffersize)
{
    if (m_pBufferData != NULL)
    {
        delete [] m_pBufferData;
        m_pBufferData = NULL;
    }

    m_pBufferData = (unsigned char *) new unsigned char[buffersize];

    if (m_pBufferData == NULL)
    {
        return -1;
    }
    m_nBufferSize      = buffersize;
    m_nBufferEmptySize = buffersize;

    for (int i = 0; i < DEFINE_FRAME_NUM - 1; i++)
    {
        m_pBufferFrameInfo[i].NextFrameInfo = &m_pBufferFrameInfo[i + 1];
    }
    m_pBufferFrameInfo[DEFINE_FRAME_NUM - 1].NextFrameInfo = &m_pBufferFrameInfo[0];
    m_pReadpos = m_pWritePos = &m_pBufferFrameInfo[0];
    return 0;
}

void CStreamBuffer::ClearBuffer()
{
#ifdef WIN32
    EnterCriticalSection(&m_csBuf);
#else
    pthread_mutex_lock(&m_csBuf);
#endif
    m_nBufferReadPos   = 0;
    m_nBufferWritePos  = 0;
    m_nBufferFrameNum  = 0;
    m_nBufferEmptySize = m_nBufferSize;
    m_pWritePos = m_pReadpos = &m_pBufferFrameInfo[0];
    memset(m_pBufferData, 0, m_nBufferSize);
#ifdef WIN32
    LeaveCriticalSection(&m_csBuf);
    ::SetEvent(m_EventBufferCanWrite);
#else
    pthread_mutex_unlock(&m_csBuf);
#endif
}

int CStreamBuffer::WriteInAFrame(void *data, int datalenth, int FrameType, unsigned long long FrameStamp, int EncrytSize, unsigned long timeout)
{
    while (m_nBufferFrameNum >= m_nMaxBufFrameNum || m_nBufferEmptySize < datalenth)
    {
#ifdef WIN32
        HANDLE wait[2] = { m_EventBufferCanWrite, m_EventSignal };
        DWORD ret = WaitForMultipleObjects(2, wait, FALSE, timeout);
        if (ret == WAIT_OBJECT_0 + 1 || ret == WAIT_TIMEOUT)
        {
            return -1;
        }
#else
		MSLEEP(1);
#endif
    }
#ifdef WIN32
    EnterCriticalSection(&m_csBuf);
#else
    pthread_mutex_lock(&m_csBuf);
#endif

    WriteData((unsigned char *)data, datalenth);

    m_pWritePos->FrameStamp = FrameStamp;
    m_pWritePos->FrameType  = FrameType;
    m_pWritePos->EncrytSize = EncrytSize;

    m_pWritePos->FrameLenth = datalenth;
    m_pWritePos = m_pWritePos->NextFrameInfo;
    m_nBufferFrameNum++;

    // 
    if (m_bCanRead == 0 && FrameStamp >= m_lBufTime)
    {
        m_bCanRead = 1;
    }
#ifdef WIN32
    LeaveCriticalSection(&m_csBuf);
#else
    pthread_mutex_unlock(&m_csBuf);
#endif
    return 0;
}

int CStreamBuffer::ReadOutAFrame(void *data, unsigned long &datalenth, unsigned long &FrameType, unsigned long & EncrytSize, unsigned long long &FrameStamp)
{
#ifdef WIN32
    EnterCriticalSection(&m_csBuf);
#else
    pthread_mutex_lock(&m_csBuf);
#endif
    if (m_bCanRead == 0 || m_nBufferFrameNum < 1)
    {
        datalenth = 0;
        FrameType = 0;
#ifdef WIN32
        LeaveCriticalSection(&m_csBuf);
        ::SetEvent(m_EventBufferCanWrite);
#else
        pthread_mutex_unlock(&m_csBuf);
#endif
        return -1;
    }

    if (m_pReadpos == m_pWritePos)
    {
        m_nBufferReadPos   = 0;
        m_nBufferWritePos  = 0;
        m_nBufferEmptySize = m_nBufferSize;
        m_nBufferFrameNum  = 0;
        m_pWritePos  = m_pReadpos = &m_pBufferFrameInfo[0];
        datalenth = 0;
        FrameType = 0;
#ifdef WIN32
        LeaveCriticalSection(&m_csBuf);
        ::SetEvent(m_EventBufferCanWrite);
#else
        pthread_mutex_unlock(&m_csBuf);
#endif
        return -1;
    }

    m_lCurrentTs = (long)m_pReadpos->FrameStamp;
    datalenth   = m_pReadpos->FrameLenth;
    FrameType   = m_pReadpos->FrameType;
    EncrytSize  = m_pReadpos->EncrytSize;
    FrameStamp  = m_pReadpos->FrameStamp;
    ReadData(data , m_pReadpos->FrameLenth);
    m_pReadpos = m_pReadpos->NextFrameInfo;

    m_nBufferFrameNum--;
#ifdef WIN32
    LeaveCriticalSection(&m_csBuf);
    ::SetEvent(m_EventBufferCanWrite);
#else
    pthread_mutex_unlock(&m_csBuf);
#endif
    return 0;
}

int CStreamBuffer::WriteData(void *data, int size)
{
    m_nBufferEmptySize -= size;
    if (size <= (m_nBufferSize - m_nBufferWritePos))
    {
        memcpy(m_pBufferData + m_nBufferWritePos, data, size);
        m_nBufferWritePos += size;
    }
    else
    {
        int tempsize = size - (m_nBufferSize - m_nBufferWritePos);
        memcpy(m_pBufferData + m_nBufferWritePos, data, m_nBufferSize - m_nBufferWritePos);
        memcpy(m_pBufferData, (unsigned char *)data + m_nBufferSize - m_nBufferWritePos, tempsize);
        m_nBufferWritePos = tempsize;
    }
    return 1;
}

void CStreamBuffer::DeleteAFrame()
{
#ifdef WIN32
    EnterCriticalSection(&m_csBuf);
#else
    pthread_mutex_lock(&m_csBuf);
#endif
    int size = m_pReadpos->FrameLenth;
    m_nBufferReadPos += size;
    m_nBufferReadPos %= m_nBufferSize;
    m_nBufferEmptySize += size;
    m_pReadpos->NextFrameInfo = m_pReadpos;
    m_nBufferFrameNum--;
#ifdef WIN32
    LeaveCriticalSection(&m_csBuf);
    ::SetEvent(m_EventBufferCanWrite);
#else
    pthread_mutex_unlock(&m_csBuf);
#endif
}

void CStreamBuffer::ReadData(void *data, int size)
{
    if (size <= (m_nBufferSize - m_nBufferReadPos))
    {
        memcpy(data, m_pBufferData + m_nBufferReadPos, size);
        m_nBufferReadPos += size;
        m_nBufferReadPos %= m_nBufferSize;
    }
    else
    {
        int temp1 = m_nBufferSize - m_nBufferReadPos;
        memcpy(data, m_pBufferData + m_nBufferReadPos, temp1);
        m_nBufferReadPos += size;
        m_nBufferReadPos %= m_nBufferSize;
        memcpy((unsigned char *)data + temp1, m_pBufferData, m_nBufferReadPos);
    }
    m_nBufferEmptySize += size;
}

int CStreamBuffer::GetBufferFrameNum() const
{
    return m_nBufferFrameNum;
}

long CStreamBuffer::GetCurrentTs() const
{
    return m_lCurrentTs / 1000;
}

long CStreamBuffer::GetBufferEmptySize() const
{
    return m_nBufferEmptySize;
}
