#ifndef _COMMON_
#define _COMMON_

#ifdef WIN32
#define MSLEEP(s) Sleep(s)
#else
#define MSLEEP(s) usleep(s * 1000)
#endif

typedef struct tagYUVFrame
{
    unsigned char    *data[3];
	int              linesize[3];
} YUVFrame;

#endif