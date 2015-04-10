#include "stdafx.h"
#include "avdecoder.h"

/*
static void print_error(int err)
{
   char errbuf[128];
   const char *errbuf_ptr = errbuf;
   if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
   {
        errbuf_ptr = strerror(AVUNERROR(err));
   }
   printf("%s\n",errbuf_ptr);
}
*/

CDecoder::CDecoder()
{
    m_pOutputFrame    = NULL;
	m_pFormatCtx      = NULL;
	m_pCodecCtx       = NULL;
	m_ImgConvertCtx   = NULL;
    memset(m_Filename, 0, sizeof(m_Filename));
	av_register_all();
    //avcodec_register_all();
}

CDecoder::~CDecoder()
{
	if (m_pOutputFrame != NULL)
    {
        av_free(m_pOutputFrame);
        //m_pOutputFrame = NULL;
    }

    if (m_pCodecCtx != NULL)
    {
        //avcodec_flush_buffers(m_pCodecCtx);
		avcodec_close(m_pCodecCtx);
        //av_free(m_pCodecCtx);
    }

	if (m_pFormatCtx != NULL)
	{
	    av_close_input_file(m_pFormatCtx);
	}

}

CVideoDecoder::CVideoDecoder(void)
{
	m_pOutputFrameRGB = NULL;
}

CVideoDecoder::~CVideoDecoder()
{
    if (m_pOutputFrameRGB != NULL)
    {
        av_free(m_pOutputFrameRGB);
        m_pOutputFrameRGB = NULL;
    }
}

int CVideoDecoder::Decode(char *pInputBuffer, int inputlen, int &got_output)
{
	m_Packet.data = (uint8_t *)pInputBuffer;
    m_Packet.size = inputlen;
#ifdef WIN32
	return avcodec_decode_video(m_pCodecCtx, m_pOutputFrame, &got_output, m_Packet.data, m_Packet.size);
#else
	return avcodec_decode_video2(m_pCodecCtx, m_pOutputFrame, &got_output, &m_Packet);
#endif
}

int CVideoDecoder::OpenFile(const char *pFilename)
{
	if (pFilename == NULL)
	{
		return -1;
	}
	strncpy(m_Filename, pFilename, sizeof(m_Filename));

    int error = 0;
#ifdef WIN32
	if ((error = av_open_input_file(&m_pFormatCtx, m_Filename, NULL, 0, NULL)) != 0)  
    {
		printf("av_open_input_file error\n");
        //print_error(error);
        return -1;
    }
#else
	if ((error = avformat_open_input(&m_pFormatCtx, m_Filename, NULL, NULL)) != 0)  
    {
		printf("avformat_open_input error\n");
        //print_error(error);
        return -1;
    }
#endif
	if (av_find_stream_info(m_pFormatCtx) < 0)
	{
		return -1;
	}

	int videostream = -1;
	for (int i = 0; i < m_pFormatCtx->nb_streams; i++)
	{
#ifdef WIN32
		if (m_pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) 
		{
            videostream = i;
			break;
		}
#else
	    if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
            videostream = i;
			break;
		}
#endif
	}
	
	if (videostream == -1)
	{
		return -1;
	}

	m_pCodecCtx = m_pFormatCtx->streams[videostream]->codec;

	AVCodec *pCodec;
	pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		return -1;
	}
#ifdef WIN32
	if (avcodec_open(m_pCodecCtx, pCodec) < 0)
	{
		return -1;
	}
#else
    if (avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0)
	{
		return -1;
	}
#endif
	m_Width = m_pCodecCtx->width;
	m_Height = m_pCodecCtx->height;
	m_pOutputFrame = avcodec_alloc_frame();   

	return 0;
}

int CVideoDecoder::ReadAFrame(char *pOutputBuffer, int &outputlen)
{
	if (pOutputBuffer == NULL)
	{
		return -1;
	}
	if (av_read_frame(m_pFormatCtx, &m_Packet) < 0)
	{
		return -1;
	}
	outputlen =  m_Packet.size;
	memcpy(pOutputBuffer, m_Packet.data, m_Packet.size);

	return 0;
}

int CVideoDecoder::GetYUVData(YUVFrame *pFrame)
{
	if (pFrame == NULL || m_pOutputFrame == NULL)
	{
		return -1;
	}

	unsigned long srcWidth  = m_Width;
	unsigned long srcHeight = m_Height;
	unsigned char *pDst = (unsigned char *)(pFrame->data[0]);
    unsigned char *pSrc = m_pOutputFrame->data[0];
    // Y
	//char output[32] = { 0 };
    for (unsigned long i = 0; i < srcHeight; i++)
    {
		//sprintf(output, "%d\n", i);
		//OutputDebugStringA(output);
        memcpy(pDst, pSrc, srcWidth);
        pSrc += m_pOutputFrame->linesize[0];
        pDst += srcWidth;
    }
    // U
    srcWidth = (srcWidth / 2);
    srcHeight = (srcHeight / 2);
    pDst = (unsigned char *)(pFrame->data[2]);
    pSrc = m_pOutputFrame->data[2];
    for (unsigned long i = 0; i < srcHeight; i++)
    {
        memcpy(pDst, pSrc, srcWidth);
        pSrc += m_pOutputFrame->linesize[1];
        pDst += srcWidth;
    }
    // V
	pDst = (unsigned char *)(pFrame->data[1]);
    pSrc = m_pOutputFrame->data[1];
    for (unsigned long i = 0; i < srcHeight; i++)
    {
        memcpy(pDst, pSrc, srcWidth);
        pSrc += m_pOutputFrame->linesize[2];
        pDst += srcWidth;
    }

	pFrame->linesize[0] = srcWidth * 2;
	pFrame->linesize[1] = srcWidth;
    pFrame->linesize[2] = srcWidth;

	return 0;
}
