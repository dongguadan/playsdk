#ifndef __DGD_PLAYSDK_H__
#define __DGD_PLAYSDK_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define DGD_PLAY_API extern "C" __declspec(dllexport)
#else
#define DGD_PLAY_API extern
#endif

#ifndef WIN32
#else
#include <Windows.h>
#endif

DGD_PLAY_API int DGD_PLAY_Init();

DGD_PLAY_API int DGD_PLAY_Cleanup();

DGD_PLAY_API int DGD_PLAY_CaptureFrmoFile(const char* pFileName);

DGD_PLAY_API int DGD_PLAY_CaptureFrmoCam(const char* pFileName);

DGD_PLAY_API int DGD_PLAY_QueryCapture(int idle, char*pVideobuffer, int* len);

DGD_PLAY_API YUVFrame* DGD_PLAY_DecodeYUV(char* pVideobuffer, int len);

DGD_PLAY_API YUVFrame* DGD_PLAY_DecodeRGB(char* pVideobuffer, int len);

DGD_PLAY_API int DGD_PLAY_ReleaseCapture(int idle);

#ifdef WIN32
DGD_PLAY_API int DGD_PLAY_CreateImage(HWND handle);
#else
DGD_PLAY_API int DGD_PLAY_CreateImage(void *handle);
#endif

DGD_PLAY_API int DGD_PLAY_UpdateImage(int channel, YUVFrame *frame);

DGD_PLAY_API int DGD_PLAY_ReleaseImage(int channel);

#ifdef __cplusplus
}
#endif

#endif