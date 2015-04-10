#ifdef WIN32
#include "stdafx.h"
#endif
#include "player.h"

#define DEOCDE_DATA_LEN         (1920 * 1080 * 3 / 2)
#define VIDEO_BUF_SIZE          (1920 * 1080 * 5)

CPlayer::CPlayer()
{
    m_pDecoder    = NULL;
	m_videoBuffer = NULL;
	m_pVideoStream = NULL;
	m_pRender      = NULL;
#ifdef WIN32
	m_hDDrawWnd = 0;
#endif
	m_hVideoDecodeThread = NULL;
    m_uVideoDecodeThreadID = 0;
	m_playing = 0;
	m_lVideoWidth = 0;
	m_lVideoHeight = 0;
	int idx = 0;
	for (idx = 0; idx < 3; idx++)
	{
		m_frame.linesize[idx] = 0;
        m_frame.data[idx] = NULL;
	}
}

CPlayer::~CPlayer()
{
	if (m_hVideoDecodeThread != NULL)
    {
	    m_playing = 0;
        m_hVideoDecodeThread   = NULL;
        m_uVideoDecodeThreadID = 0;
    }
	if (m_pDecoder != NULL)
    {
        delete m_pDecoder;
        m_pDecoder = NULL;
    }
    if (m_videoBuffer != NULL)
    {
        delete m_videoBuffer;
        m_videoBuffer = NULL;
    }
	if (m_pVideoStream != NULL)
    {
        delete [] m_pVideoStream;
        m_pVideoStream = NULL;
    }
	if (m_pRender != NULL)
	{
		delete m_pRender;
		m_pRender = NULL;
	}
    int idx = 0;
	for (idx = 0; idx < 3; idx++)
	{
		m_frame.linesize[idx] = 0;
		if (m_frame.data[idx] != NULL)
		{
		    free(m_frame.data[idx]);
            m_frame.data[idx] = NULL;
		}
	}
}

int CPlayer::init()
{
	int idx = 0;
	for (idx = 0; idx < 3; idx++)
	{
		m_frame.linesize[idx] = 0;
		if (m_frame.data[idx] == NULL)
		{
		    m_frame.data[idx] = (unsigned char *)malloc(1280 * 960 * sizeof(unsigned char));
		}
	}

	if (m_pVideoStream == NULL)
	{
		m_pVideoStream = new unsigned char [DEOCDE_DATA_LEN];
	}

    if (m_videoBuffer == NULL)
	{
		m_videoBuffer = new CStreamBuffer();
	}

    if (m_videoBuffer != NULL)
    {
        m_videoBuffer->AlloceBuffer(VIDEO_BUF_SIZE);
    }
    else
    {
        return -1;
    }

    if (m_pDecoder == NULL)
	{
		m_pDecoder = new CVideoDecoder();
	}
#ifdef WIN32
	if (m_pRender == NULL)
	{
		m_pRender = new CD3DRender();
	}
#else
	if (m_pRender == NULL)
	{
		m_pRender = new CSDLRender();
	}
#endif
	return 0;
}

int CPlayer::Open(const char *pFileName)
{
	if (pFileName == NULL)
	{
		return -1;
	}
	
	if (m_pDecoder->OpenFile(pFileName) < 0)
	{
		return -1;
	}
#ifdef WIN32
	m_hVideoDecodeThread = (HANDLE)_beginthreadex(NULL, 0, InitialThreadProc, (void *)this, 0, &m_uVideoDecodeThreadID);
    if (m_hVideoDecodeThread == NULL)
    {
		return -1;
	}
#else
    pthread_create(&m_hVideoDecodeThread, NULL, InitialThreadProc, (void *)this);
#endif
    return 0;
}

int CPlayer::Close()
{
	if (m_hVideoDecodeThread != NULL)
    {
	    m_playing = 0;
        m_hVideoDecodeThread   = NULL;
        m_uVideoDecodeThreadID = 0;
    }
    return 0;
}

#ifdef WIN32
unsigned int WINAPI CPlayer::InitialThreadProc(void *pv)
{
    unsigned int retVal = 0;
    if (pv == NULL)
    {
        return -1;
    }

    CPlayer *p = (CPlayer *)pv;
    retVal = p->ThreadProc();
    return retVal;
}
#else
void * CPlayer::InitialThreadProc(void *pv)
{
    unsigned int retVal = 0;
    if (pv == NULL)
    {
        return NULL;
    }

    CPlayer *p = (CPlayer *)pv;
    retVal = p->ThreadProc();
    return NULL;
}
#endif

unsigned long CPlayer::ThreadProc()
{
    unsigned long datalenth  = 0;
    unsigned long frametype = 0;
    unsigned long EncrytSize = 0;
    unsigned int  FrameStamp = 0;
	int len = 0;
    m_playing = 1;
    printf("player ThreadProc...\r\n");
	while (m_playing == 1)
    {
		if (m_pDecoder->ReadAFrame((char *)m_pVideoStream, len) == 0)
		{
			printf("player ready write a frame\r\n");
            while (m_playing == 1 && WriteInAFrame(m_pVideoStream, len, FrameStamp, frametype, EncrytSize) != 0)
            {
                MSLEEP(10);
                continue;
            }
		}
		else
		{
			m_playing = 0;
		}
        MSLEEP(80);
	}

	return 0;
}

int CPlayer::ReadOutAFrame(void *data, unsigned long &datalenth, unsigned long &frametype, unsigned long &EncrytSize, unsigned long long &FrameStamp)
{
    if (m_videoBuffer->GetBufferFrameNum() > 0)
    {
		printf("player read a frame success\r\n");
        return m_videoBuffer->ReadOutAFrame(data, datalenth, frametype, EncrytSize, FrameStamp);
    }
    else
    {
		printf("player read a frame error\r\n");
        return -1;
    }
}

int CPlayer::WriteInAFrame(unsigned char *pBuf, unsigned long nSize, unsigned long long FrameStamp, int frametype, int EncrytSize)
{
    return m_videoBuffer->WriteInAFrame(pBuf, nSize, frametype, FrameStamp, EncrytSize, 0);
}

YUVFrame* CPlayer::DecodeAFrame(void *data, int len)
{
	if (data == 0)
	{
		return NULL;
	}
	int got_output = 0;
	//memset(&m_frame, 0, sizeof(m_frame));
	m_pDecoder->Decode((char *)data, len, got_output);
	if (got_output)
	{
		m_pDecoder->GetYUVData(&m_frame);
		return &m_frame;
	}
    return NULL;
}
#ifdef WIN32
int CPlayer::InitRenderer(HWND handle)
{
	if (handle == NULL)
    {
        return -1;
	}

    m_hDDrawWnd = (HWND)handle;
	m_pDecoder->GetResolution(m_lVideoWidth, m_lVideoHeight);
    if (m_lVideoWidth <= 0 || m_lVideoHeight <= 0)
	{
		return -1;
	}

    if (m_pRender->InitD3D(m_hDDrawWnd) < 0)
    {
        return -1;
    }
    if (m_pRender->CreateOffScreen(m_lVideoWidth, m_lVideoHeight) < 0)
    {
        return -1;
    }

    return 0;
}
#else
int CPlayer::InitRenderer(void *handle)
{
	m_pDecoder->GetResolution(m_lVideoWidth, m_lVideoHeight);
    if (m_lVideoWidth <= 0 || m_lVideoHeight <= 0)
	{
		return -1;
	}

    if (m_pRender->InitSDL(NULL) < 0)
    {
        return -1;
    }
    if (m_pRender->CreateOffScreen(m_lVideoWidth, m_lVideoHeight) < 0)
    {
        return -1;
    }

    return 0;
}
#endif

int CPlayer::RenderFrame(YUVFrame* pYUVFrame)
{
    if (pYUVFrame == NULL)
    {
        return -1;
    }

    return m_pRender->PutFrame(pYUVFrame, m_lVideoWidth, m_lVideoHeight, 0);
}

int CPlayer::StopRenderer()
{
	if (m_pRender != NULL)
	{
        m_pRender->Flush();
	    m_pRender->Stop();
		return 0;
	}
	return -1;
}
