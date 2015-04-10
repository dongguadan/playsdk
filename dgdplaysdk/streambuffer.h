#ifndef _STREAMBUFFER_
#define _STREAMBUFFER_

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

#define DEFINE_FRAME_NUM 300

typedef struct  tagBufferFrameInfo
{
    unsigned long      FrameType;
    unsigned long      FrameLenth;
    unsigned long long FrameStamp;
    int                EncrytSize;
    tagBufferFrameInfo *NextFrameInfo;
} BUFFERFRAMEINFO, *pBUFFERFRAMEINFO;

class CStreamBuffer
{
public:
    CStreamBuffer();
    virtual ~CStreamBuffer();
	int AlloceBuffer(int buffersize);
	void SetMaxBufferNum(int maxbufnum) 
    {
#ifdef WIN32
        EnterCriticalSection(&m_csBuf);
#endif
        if (maxbufnum < DEFINE_FRAME_NUM)
        {
            m_nMaxBufFrameNum = maxbufnum;
        }
        else
        {
            m_nMaxBufFrameNum = DEFINE_FRAME_NUM;
        }
#ifdef WIN32
        LeaveCriticalSection(&m_csBuf);
#endif
    }
	void ClearBuffer();
    int WriteInAFrame(void *data, int datalenth, int FrameType, unsigned long long FrameStamp = 0, int EncrytSize = 0, unsigned long timeout = 0);
    int ReadOutAFrame(void *data, unsigned long &datalenth, unsigned long &FrameType, unsigned long &EncrytSize, unsigned long long &FrameStamp);
    void DeleteAFrame();
    int  GetBufferFrameNum() const;
    long GetCurrentTs() const;
    long GetBufferEmptySize() const;
    int  SetBufferTime(long bufTime)
    {
        m_lBufTime = bufTime;
        return 0;
    }
private:
	int  WriteData(void *data, int size);
    void ReadData(void *data, int size);
    unsigned char      *m_pBufferData;
    BUFFERFRAMEINFO     m_pBufferFrameInfo[DEFINE_FRAME_NUM];
    BUFFERFRAMEINFO     *m_pReadpos;
    BUFFERFRAMEINFO     *m_pWritePos;
    int                 m_nBufferSize;
    int                 m_nBufferReadPos;
    int                 m_nBufferWritePos;
    volatile int        m_nBufferEmptySize;
    volatile int        m_nBufferFrameNum;
    int                 m_nMaxBufFrameNum;
    long                m_lCurrentTs;
    int                 m_bCanRead;
    long                m_lBufTime;
#ifdef WIN32
	CRITICAL_SECTION    m_csBuf;
	HANDLE              m_EventBufferCanWrite;
    HANDLE              m_EventSignal;
#else
    pthread_mutex_t     m_csBuf;
#endif
};

#endif