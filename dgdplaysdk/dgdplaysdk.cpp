// dgdplaysdk.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "dgdplaysdk.h"
#include "player.h"

#define MAX_VIDEOPLAYER_NUM 1

typedef struct tagTFCPlayHandle
{
    CPlayer          *vp;
} TFCPlayHandle;

typedef struct tagTFCPlayCtrl
{
    TFCPlayHandle    ph[MAX_VIDEOPLAYER_NUM];
} TFCPlayCtrl;

TFCPlayCtrl gPlayCtrl = { 0 };

DGD_PLAY_API int DGD_PLAY_Init()
{
	int ret = 0;
    CPlayer *vp = new CPlayer();
	if (vp != NULL)
	{
		gPlayCtrl.ph[0].vp = vp;
		vp->init();
	}
	else
	{
		ret = -1; 
	}
	return ret;
}

DGD_PLAY_API int DGD_PLAY_Cleanup()
{
	int ret = 0;
	if (gPlayCtrl.ph[0].vp != NULL)
	{
		delete gPlayCtrl.ph[0].vp;
		gPlayCtrl.ph[0].vp = NULL;
	}
	return ret;
}

int DGD_PLAY_CaptureFrmoFile(const char* pFileName)
{
	if (pFileName == NULL)
	{
		return -1;
	}
	int idle = 0;
	CPlayer *vp = NULL;
	vp = gPlayCtrl.ph[idle].vp;
	if (vp != NULL)
	{
		if (vp->Open(pFileName) < 0)
		{
			idle = -1;
		}
	}
	else
	{
		idle = -1;
	}
	return idle;
}

int DGD_PLAY_CaptureFrmoCam(const char* pFileName)
{
	int ret = 0;
	return ret;
}

int DGD_PLAY_QueryCapture(int idle, char* pVideobuffer, int* len)
{
	if (pVideobuffer == NULL || idle < 0)
	{
		return -1;
	}
	int ret = 0;
    unsigned long data_length = 0;
    unsigned long frame_type = 0;
    unsigned long encry_tSize = 0;
    unsigned long long frame_stamp = 0;
    CPlayer *vp = NULL;
	vp = gPlayCtrl.ph[idle].vp;
	if (vp != NULL)
	{
		ret = vp->ReadOutAFrame(pVideobuffer, data_length, frame_type, encry_tSize, frame_stamp);
		if (ret == 0)
		{
			*len = data_length;
		}
	}
	return ret;
}

YUVFrame* DGD_PLAY_DecodeYUV(char* pVideobuffer, int len)
{
	if (pVideobuffer == NULL || len < 0)
	{
		return NULL;
	}
	int ret = 0;
	CPlayer *vp = NULL;
	vp = gPlayCtrl.ph[0].vp;
	YUVFrame* pFrametmp = NULL;
	if (vp != NULL)
	{
		pFrametmp = vp->DecodeAFrame(pVideobuffer, len);
		if (pFrametmp == NULL)
		{
            return NULL;
		};
	}
	return pFrametmp;
}

YUVFrame* DGD_PLAY_DecodeRGB(char* pVideobuffer, int len)
{
	return NULL;
}

int DGD_PLAY_ReleaseCapture(int idle)
{
	if (idle < 0)
	{
		return -1;
	}
	int ret = -1;
	CPlayer *vp = NULL;
	vp = gPlayCtrl.ph[idle].vp;
	if (vp != NULL)
	{
		ret = vp->Close();
	}
	return ret;
}

#ifdef WIN32 
int DGD_PLAY_CreateImage(HWND handle)
{
	if (handle == NULL)
	{
		return -1;
	}
	int ret = 0;
	CPlayer *vp = NULL;
	vp = gPlayCtrl.ph[0].vp;
	if (vp != NULL)
	{
		ret = vp->InitRenderer(handle);
	}
	return ret;
}
#else
int DGD_PLAY_CreateImage(void *handle)
{
	if (handle == NULL)
	{

	}
	int ret = 0;
	CPlayer *vp = NULL;
	vp = gPlayCtrl.ph[0].vp;
	if (vp != NULL)
	{
		ret = vp->InitRenderer(handle);
	}
	return ret;
}
#endif

int DGD_PLAY_UpdateImage(int channel, YUVFrame *frame)
{
	if (channel < 0 || frame == NULL)
	{
		return -1;
	}

	int chn = channel;
    CPlayer *vp = gPlayCtrl.ph[chn].vp;
	return vp->RenderFrame(frame);
}

int DGD_PLAY_ReleaseImage(int channel)
{
	if (channel < 0)
	{
		return -1;
	}
	int chn = channel;
    CPlayer *vp = gPlayCtrl.ph[chn].vp;
	return vp->StopRenderer();
}

