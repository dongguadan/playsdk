#ifdef WIN32
#include "stdafx.h"
#include "d3drender.h"

CD3DRender::CD3DRender()
{
	m_pD3D              = NULL;
    m_pD3DDevice        = NULL;
	m_pOffscreenSurface = NULL;
    m_pBackBuffer       = NULL;
    m_pSwapChain        = NULL;
	m_hWnd              = NULL;
	m_dwWidth           = 0;
    m_dwHeight          = 0;
	m_bDone             = 0;
	m_bStretch          = 0;

	m_uVideoRendererThreadID = 0;
	m_hVideoRendererThread   = NULL;

    m_EventCanPut       = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_EventCanGet       = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_videoQ.clear();
    memset(&m_rcDst, 0, sizeof(m_rcDst));
    for (int i = 0; i < MAX_RENDER_FRAME; i++)
    {
        ZeroMemory(&m_videoFrames[i], sizeof(VideoFrame));
        m_videoFrames[i].idx   = i;
        m_videoFrames[i].bIdle = 0;
    }
    ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
}

CD3DRender::~CD3DRender()
{
	/*m_d3dpp m_hWnd*/
    if (m_pSwapChain != NULL)
	{
		m_pSwapChain->Release();
		m_pSwapChain = NULL;
	}

    if (m_pBackBuffer != NULL)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}

    if (m_pOffscreenSurface != NULL)
	{
		m_pOffscreenSurface->Release();
		m_pOffscreenSurface = NULL;
	}

    if (m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	if (m_pD3D != NULL)
	{
	    m_pD3D->Release();
		m_pD3D = NULL;
	}

    CloseHandle(m_EventCanPut);
    CloseHandle(m_EventCanGet);

	for (int i = 0; i < MAX_RENDER_FRAME; i++)
    {
        if (m_videoFrames[i].yuv != NULL)
        {
            free(m_videoFrames[i].yuv);
            m_videoFrames[i].yuv = NULL;
        }
    }
}

void CD3DRender::SetDstRect()
{
    GetClientRect(m_hWnd, &m_rcDst);
}

HRESULT CD3DRender::InitD3D(HWND hWnd)
{
	if (NULL == hWnd)
    {
        return E_FAIL;
    }
    m_hWnd = hWnd;

    m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (m_pD3D == NULL)
    {
        return E_FAIL;
    }

	D3DCAPS9 d3dCaps;    
    m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps);
    bool bHardwareEnable = (d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ? true : false);
    unsigned long behaviorFlags = bHardwareEnable ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    m_d3dpp.Windowed                   = TRUE;
    m_d3dpp.BackBufferCount            = 0;
    m_d3dpp.BackBufferWidth            = 0;
    m_d3dpp.BackBufferHeight           = 0;
    m_d3dpp.Flags                      = D3DPRESENTFLAG_VIDEO;
    m_d3dpp.SwapEffect                 = D3DSWAPEFFECT_COPY;//D3DSWAPEFFECT_FLIP;
    m_d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;
    m_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
    m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    m_d3dpp.hDeviceWindow              = hWnd;
    m_d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
    m_d3dpp.MultiSampleQuality         = 0;
    if (m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
                                    D3DDEVTYPE_HAL,
                                    m_hWnd,
                                    behaviorFlags | D3DCREATE_MULTITHREADED,
                                    &m_d3dpp,
                                    &m_pD3DDevice) < 0)
    {
        return E_FAIL;
    }

    SetDstRect();

    return S_OK;
}

LRESULT CD3DRender::CreateOffScreen(unsigned long w, unsigned long h)
{
	m_dwWidth  = w;
    m_dwHeight = h;

	for (int i = 0; i < MAX_RENDER_FRAME; i++)
    {
        m_videoFrames[i].yuv = (unsigned char *)malloc(m_dwWidth * m_dwHeight * 3 / 2);
        if (m_videoFrames[i].yuv == NULL)
        {
            return -1;
        }
        m_videoFrames[i].w      = m_dwWidth;
        m_videoFrames[i].h      = m_dwHeight;
        m_videoFrames[i].stride = m_dwWidth;
    }

    if (Create() < 0)
    {
        return -1;
    }

    m_hVideoRendererThread = (HANDLE)_beginthreadex(NULL, 0, InitialThreadProc, (void *)this, 0, &m_uVideoRendererThreadID);
    if (m_hVideoRendererThread == NULL)
    {
        return -1;
    }

    return D3D_OK;
}

long CD3DRender::Create()
{
    long hr = m_pD3DDevice->CreateOffscreenPlainSurface(m_dwWidth,
                                                        m_dwHeight,
                                                        (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
                                                        D3DPOOL_DEFAULT,//
                                                        &m_pOffscreenSurface,
                                                        NULL);
    if (hr < 0)
    {
        return E_FAIL;
    }

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed               = TRUE;
    d3dpp.BackBufferCount        = 1;
    d3dpp.BackBufferWidth        = m_dwWidth;
    d3dpp.BackBufferHeight       = m_dwHeight;
    d3dpp.Flags                  = D3DPRESENTFLAG_VIDEO;
    d3dpp.MultiSampleQuality     = 0;
    d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_COPY;// D3DSWAPEFFECT_FLIP;//
    d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.hDeviceWindow          = m_hWnd;
    d3dpp.EnableAutoDepthStencil = FALSE;
    if (m_pD3DDevice->CreateAdditionalSwapChain(&d3dpp, &m_pSwapChain) < 0)
    {
        return -1;
    }

    m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	return 0;
}

void CD3DRender::Stop()
{
	m_bDone = 1;
    ::SetEvent(m_EventCanGet);
    if (m_hVideoRendererThread != NULL)
    {
        WaitForSingleObject(m_hVideoRendererThread, INFINITE);
        CloseHandle(m_hVideoRendererThread);
        m_hVideoRendererThread   = NULL;
        m_uVideoRendererThreadID = 0;
    }
}

unsigned int CD3DRender::InitialThreadProc(void *pv)
{
    CD3DRender *pThis = (CD3DRender *)pv;
    return pThis->ThreadProc();
}

unsigned long CD3DRender::ThreadProc()
{
    while (m_bDone == 0)
    {
        int idx = GetFrame();
		if (idx < 0 && m_bDone == 0)
        {
            ::SetEvent(m_EventCanPut);
            WaitForSingleObject(m_EventCanGet, 5);
            continue;
        }
		DoRender(&m_videoFrames[idx]);
		ReturnFrame(idx);
	    OutputDebugStringA("ThreadProc... render!\n");
	    Sleep(30);
	}
	return 0;
}

void CD3DRender::ReturnFrame(int idx)
{
    if (idx >= 0 && idx < MAX_RENDER_FRAME)
    {
        CScopeLock lock(&m_csLock);
        m_videoFrames[idx].bIdle = 0;
		::SetEvent(m_EventCanPut);
    }
    return;
}

int CD3DRender::GetFrame()
{
    int idx = -1;
    CScopeLock lock(&m_csLock);
    if (m_videoQ.empty() == true)
    {
        return -1;
    }
    idx = m_videoQ.front();
    m_videoQ.pop_front();
    return idx;
}

int CD3DRender::GetIdleFrame()
{
    CScopeLock lock(&m_csLock);
    for (int i = 0; i < MAX_RENDER_FRAME; i++)
    {
        if (m_videoFrames[i].bIdle == 0)
        {
            m_videoFrames[i].bIdle = 1;
            return i;
        }
    }
    return -1;
}

static void CopyFrame(YUVFrame *frame, unsigned long srcWidth, unsigned long srcHeight, unsigned char *dst, unsigned long dstStride)
{
    unsigned char *pDst = (unsigned char *)dst;
    unsigned char *pSrc = frame->data[0];
    // Y
    for (DWORD i = 0; i < srcHeight; i++)
    {
        memcpy(pDst, pSrc, srcWidth);
        pSrc += dstStride;
        pDst += dstStride;
    }
    // U
    srcWidth = (srcWidth / 2);
    srcHeight = (srcHeight / 2);
    dstStride = (dstStride / 2);
    pSrc = frame->data[2];
    for (unsigned long i = 0; i < srcHeight; i++)
    {
        memcpy(pDst, pSrc, srcWidth);
        pSrc += dstStride;
        pDst += dstStride;
    }
    // V
    pSrc = frame->data[1];
    for (unsigned long i = 0; i < srcHeight; i++)
    {
        memcpy(pDst, pSrc, srcWidth);
        pSrc += dstStride;
        pDst += dstStride;
    }
}

int CD3DRender::PutFrame(YUVFrame *frame, DWORD srcWidth, DWORD srcHeight, unsigned long long pts)
{
	::WaitForSingleObject(m_EventCanPut, INFINITE);
    int idx = GetIdleFrame();
    if (idx < 0)
    {
        return -1;
    }
    if (m_bDone == 1)
    {
        return -1;
    }

    CopyFrame(frame, srcWidth, srcHeight, m_videoFrames[idx].yuv, m_videoFrames[idx].stride);
    m_videoFrames[idx].pts = pts;
    CScopeLock lock(&m_csLock);
    m_videoQ.push_back(idx);

    ::SetEvent(m_EventCanGet);
    return 0;
}

int  CD3DRender::Flush()
{
    CScopeLock lock(&m_csLock);
    m_videoQ.clear();
    for (int i = 0; i < MAX_RENDER_FRAME; i++)
    {
        m_videoFrames[i].bIdle = 0;
    }
    ::SetEvent(m_EventCanPut);
    return 0;
}

static void Copy2Surface(VideoFrame *vFrame, unsigned char *dst, unsigned long dstStride)
{
    unsigned char *pDst = (unsigned char *)dst;
    unsigned char *pSrc = vFrame->yuv;
    unsigned long srcWidth  = vFrame->w;
    unsigned long srcHeight = vFrame->h;
    unsigned long stride    = vFrame->stride;
    // Y
    for (unsigned long i = 0; i < srcHeight; i++)
    {
        memcpy(pDst, pSrc, srcWidth);
        pSrc += vFrame->stride;
        pDst += dstStride;
    }
    // U
    srcWidth = (srcWidth / 2);
    srcHeight = (srcHeight / 2);
    dstStride = (dstStride / 2);

    pSrc = vFrame->yuv + vFrame->h * vFrame->stride;
    for (unsigned long i = 0; i < srcHeight; i++)
    {
        memcpy(pDst, pSrc, srcWidth);
        pSrc += vFrame->stride / 2;
        pDst += dstStride;
    }
    // V
    pSrc = vFrame->yuv + vFrame->h * vFrame->stride + vFrame->h * vFrame->stride / 4;
    for (unsigned long i = 0; i < srcHeight; i++)
    {
        memcpy(pDst, pSrc, srcWidth);
        pSrc += vFrame->stride / 2;
        pDst += dstStride;
    }
}

void CD3DRender::GetRenderRect(RECT& rcRender)
{
    if (m_hWnd == NULL)
    {
        return;
    }

    GetClientRect(m_hWnd, &m_rcDst);
    int ww = m_rcDst.right  - m_rcDst.left;
    int wh = m_rcDst.bottom - m_rcDst.top;

    int dw = 0;
    int dh = 0;

    if (double(wh) / double(ww) < double(m_dwHeight) / double(m_dwWidth))
    {
        dh = wh;
        dw = dh / (double(m_dwHeight) / double(m_dwWidth));
        rcRender.left   = m_rcDst.left + (ww - dw) / 2;
        rcRender.top    = m_rcDst.top;
        rcRender.right  = rcRender.left + dw;
        rcRender.bottom = m_rcDst.bottom;
    }
    else
    {
        dw = ww;
        dh = ww * (double(m_dwHeight) / double(m_dwWidth));
        rcRender.left   = m_rcDst.left;
        rcRender.top    = m_rcDst.top + (wh - dh) / 2;
        rcRender.right  = m_rcDst.right;
        rcRender.bottom = rcRender.top + dh;
    }
    return;
}

int CD3DRender::DrawVideoImage(int bStretch)
{
    HDC hDC = NULL;
    if (!::IsWindow(m_hWnd))
    {
        return -1;
    }

    RECT rcDisp = { 0 };
    GetRenderRect(rcDisp);

    if (m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0) < 0)
    {
        return -1;
    }
    if (m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer) < 0)
    {
        return -1;
    }

    m_pD3DDevice->BeginScene();
    m_pD3DDevice->StretchRect(m_pOffscreenSurface, NULL, m_pBackBuffer, NULL, D3DTEXF_LINEAR);
    m_pD3DDevice->SetRenderTarget(0, m_pBackBuffer);
    m_pD3DDevice->EndScene();

    D3DPRESENT_PARAMETERS params;
    ZeroMemory(&params, sizeof(D3DPRESENT_PARAMETERS));
    m_pSwapChain->GetPresentParameters(&params);
    if (params.SwapEffect == D3DSWAPEFFECT_FLIP)
    {
        m_pSwapChain->Present(NULL, NULL, NULL, NULL, D3DPRESENT_DONOTWAIT);
    }
    else
    {
        if (bStretch)
        {
            m_pSwapChain->Present(NULL, &m_rcDst, NULL, NULL, D3DPRESENT_DONOTWAIT);
        }
        else
        {
            m_pSwapChain->Present(NULL, &rcDisp, NULL, NULL, D3DPRESENT_DONOTWAIT);
        }
    }

    if (m_pBackBuffer != NULL)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}
    return 0;
}

int CD3DRender::DoRender(VideoFrame *vFrame)
{
    unsigned long running_time = 0;
    unsigned long diff = 0;
    void *pDisplay = NULL;
    int  dstride    = 0;
    if (Lock((char **)&pDisplay, dstride) == TRUE)
    {
        if (pDisplay != NULL)
        {
            Copy2Surface(vFrame, (unsigned char *)pDisplay, dstride);
        }

        UnLock();
		DrawVideoImage(m_bStretch);
    }

    return 1;
}

int CD3DRender::Lock(char **pDst, int &iStride)
{
    if (m_pOffscreenSurface != NULL)
    {
        D3DLOCKED_RECT Drect = {0};
        RECT           rect  = {0, 0, m_dwWidth, m_dwHeight};
        long  result = m_pOffscreenSurface->LockRect(&Drect, &rect, 0);
        if (result < 0)
        {
            return 0;
        }
        *pDst   = (char *)Drect.pBits;
        iStride = Drect.Pitch;
        return 1;
    }
    return 0;
}

void CD3DRender::UnLock()
{
    if (m_pOffscreenSurface != NULL)
    {
        m_pOffscreenSurface->UnlockRect();
    }
}
#endif
