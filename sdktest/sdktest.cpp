// sdktest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../include/dgdplaysdk.h"
#include <conio.h>

#pragma comment(lib, "../debug/dgdplaysdk.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	DGD_PLAY_Init();
	int idle = DGD_PLAY_CaptureFrmoFile("D:\\Projects\\dgdsdk\\sdktest2\\test_4.mp4");
	if (idle != 0)
	{
	    printf("DGD_PLAY_CaptureFrmoFile fail!\r\n");
        DGD_PLAY_Cleanup();
		getch();
		return -1;
	}

     FILE *pFile = fopen("test.h264", "wb+");
	 if (pFile == NULL)
	 {
		printf("fopen fail!\r\n");
        DGD_PLAY_Cleanup();
		getch();
		return -1;
	 }
	Sleep(100);
	int len = 0;
	char videobuffer[1024 * 500] = { 0 };
	while (DGD_PLAY_QueryCapture(idle, videobuffer, &len) == 0)
	{
		printf("query frame from file success!\r\n");
		fwrite(videobuffer, len, 1, pFile);
		memset(videobuffer, 0, sizeof(videobuffer));
	    Sleep(100);
	}

	printf("exit\r\n");
	fclose(pFile);
	DGD_PLAY_ReleaseCapture(NULL);
    DGD_PLAY_Cleanup();
    getch();
	return 0;
}

