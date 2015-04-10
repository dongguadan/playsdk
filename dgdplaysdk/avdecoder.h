#ifndef _AVDECODER_
#define _AVDECODER_

#include "common.h"

#include<iostream>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif
#ifdef WIN32
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#else
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#endif

#ifdef __cplusplus
}
#endif

class CDecoder
{
public:
    CDecoder(void);
    virtual ~CDecoder(void);
	virtual int Decode(char *pData, int datalength, int &got_output) = 0;
protected:
	char               m_Filename[128];
	AVFormatContext   *m_pFormatCtx;
    AVCodecContext    *m_pCodecCtx;
    AVPacket           m_Packet;
    struct SwsContext *m_ImgConvertCtx;
    AVFrame           *m_pOutputFrame;
};

class CVideoDecoder : public CDecoder
{
public:
    CVideoDecoder(void);
    virtual ~CVideoDecoder();
	virtual int Decode(char *pInputBuffer, int inputlen, int &got_output);
	int GetYUVData(YUVFrame *pFrame);
	int OpenFile(const char *pFilename);
	int ReadAFrame(char *pOutputBuffer, int &outputlen);
	int GetResolution(unsigned long &w, unsigned long &h)
	{
		w = m_Width;
		h = m_Height;
		return 0;
	}
private:
    AVFrame           *m_pOutputFrameRGB;
    unsigned long      m_Width;
    unsigned long      m_Height;
};

#endif