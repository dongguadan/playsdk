#ifndef _PLAYER_H
#define _PLAYER_H

#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef WIN32
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#else
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#endif

#ifdef __cplusplus
}
#endif

#include "common.h"
#include "avdecoder.h"
#include "streambuffer.h"

#ifdef WIN32
#include <process.h>
#include "d3drender.h"
#else
#include <pthread.h>
#include "sdlrender.h"
#endif

class CPlayer
{
public:
    CPlayer();
    ~CPlayer();

    int init();
	int Open(const char *pFileName);
    int Close();
	int ReadOutAFrame(void *data, unsigned long &datalenth, unsigned long &frametype, unsigned long &EncrytSize, unsigned long long &FrameStamp);
	YUVFrame* DecodeAFrame(void *data, int len);
#ifdef WIN32
	int  InitRenderer(HWND handle);
#else
	int  InitRenderer(void *handle);
#endif
	int  RenderFrame(YUVFrame* pYUVFrame);
	int  StopRenderer();

private:
#ifdef WIN32
    static unsigned int WINAPI InitialThreadProc(void *pv);
	HANDLE            m_hVideoDecodeThread;
    unsigned int      m_uVideoDecodeThreadID;
#else
	static void * InitialThreadProc(void *pv);
    pthread_t     m_hVideoDecodeThread;
    unsigned int  m_uVideoDecodeThreadID;
#endif
    unsigned long ThreadProc();
    int WriteInAFrame(unsigned char *pBuf, unsigned long nSize, unsigned long long FrameStamp, int frametype, int EncrytSize = 0);
    CVideoDecoder *m_pDecoder;
	CStreamBuffer *m_videoBuffer;
#ifdef WIN32
	CD3DRender    *m_pRender;
	HWND           m_hDDrawWnd;
#else
	CSDLRender    *m_pRender;
#endif
	unsigned char *m_pVideoStream;
	int           m_playing;
	YUVFrame      m_frame;
	unsigned long              m_lVideoWidth;
    unsigned long              m_lVideoHeight;
};

#endif