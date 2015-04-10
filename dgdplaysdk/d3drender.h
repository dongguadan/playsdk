#ifdef WIN32
#ifndef _D3DRENDER_H
#define _D3DRENDER_H

#include <atlbase.h>
#include <deque>

#include "common.h"

#include "d3dx9core.h"
#include "d3d9.h"
#include "dxerr9.h"
#include "scopelock.h"

#pragma comment(lib, "D3d9.lib")
#pragma comment(lib, "D3dx9.lib")
#pragma comment(lib, "dxerr9.lib")

#define MAX_RENDER_FRAME 6

typedef struct tagVideoFrame
{
    int                idx;
    unsigned char      *yuv;
    unsigned long long pts;
    long               w;
    long               h;
    long               stride;
    int                bIdle;
} VideoFrame;

typedef std::deque<int> VideoFrameQ;

class CD3DRender
{
public:
    CD3DRender();
    virtual ~CD3DRender();
	HRESULT     InitD3D(HWND hWnd);
	int         PutFrame(YUVFrame *frame, unsigned long srcWidth, unsigned long srcHeight, unsigned long long pts);
    int         Flush();
	HRESULT     CreateOffScreen(unsigned long w, unsigned long h);
	void        Stop();
private:
    static unsigned int WINAPI InitialThreadProc(void *pv);
    unsigned long ThreadProc();
    void  SetDstRect();
	long  Create();
	int   DoRender(VideoFrame *vFrame);
	int   Lock(char **pDst, int &iStride);
    void  UnLock();

	int   GetIdleFrame();
    int   GetFrame();
    void  ReturnFrame(int idx);

    int  DrawVideoImage(int bStretch = 1);
    void  GetRenderRect(RECT& rcRender);

	HWND                    m_hWnd;
	LPDIRECT3D9             m_pD3D;
	D3DPRESENT_PARAMETERS   m_d3dpp;
	LPDIRECT3DDEVICE9       m_pD3DDevice;
	IDirect3DSurface9       *m_pBackBuffer;
	IDirect3DSwapChain9     *m_pSwapChain;
	IDirect3DSurface9       *m_pOffscreenSurface;

	HANDLE                  m_hVideoRendererThread;
    unsigned int            m_uVideoRendererThreadID;

	unsigned long           m_dwWidth;
    unsigned long           m_dwHeight;
	int                     m_bStretch;
	HANDLE                  m_EventCanPut;
    HANDLE                  m_EventCanGet;
	RECT                    m_rcDst;
	CCriticalSection        m_csLock;
	VideoFrameQ             m_videoQ;
	VideoFrame              m_videoFrames[MAX_RENDER_FRAME];
	int                     m_bDone;
};

#endif
#endif
