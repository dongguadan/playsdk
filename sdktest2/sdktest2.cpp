#include "dgdplaysdk.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int argc,  char* argv[])
{
	DGD_PLAY_Init();
	int ret = DGD_PLAY_CaptureFrmoFile("test_4.avi");
	if (ret != 0)
	{
	    printf("DGD_PLAY_CaptureFrmoFile fail!\r\n");
        DGD_PLAY_Cleanup();
		getchar();
		return -1;
	}

    /*FILE *pFile = fopen("test.h264", "wb+");
	 if (pFile == NULL)
	 {
		printf("fopen fail!\r\n");
        DGD_PLAY_Cleanup();
		getchar();
		return -1;
	 }*/

    int chn = DGD_PLAY_CreateImage(NULL);
	if (chn != 0)
	{
	    printf("DGD_PLAY_CreateImage fail!\n");
	    DGD_PLAY_ReleaseCapture(NULL);
        DGD_PLAY_Cleanup();
		return -1;
	}
	 
	usleep(100);
	int len = 0;
	YUVFrame *frame = NULL;
	char videobuffer[1024 * 500] = { 0 };
	while (DGD_PLAY_QueryCapture(NULL, videobuffer, &len) == 0)
	{
		printf("query frame from file success, ready decode!\n");
		if ((frame = DGD_PLAY_DecodeYUV(videobuffer, len)) != 0)
		{
		    printf("decode frame from file success, ready render!\n");
			DGD_PLAY_UpdateImage(chn, frame);
			printf("render success!\n\n");
	        //usleep(20);
		}
		else
		{
			printf("decode frame from file fail!\n");
		}
		//fwrite(videobuffer, len, 1, pFile);
		//memset(videobuffer, 0, sizeof(videobuffer));
	}

	printf("exit\r\n");
	//fclose(pFile);
	DGD_PLAY_ReleaseImage(chn);
	DGD_PLAY_ReleaseCapture(NULL);
    DGD_PLAY_Cleanup();
    getchar();
	return 0;
}

