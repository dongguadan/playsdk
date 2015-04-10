// PlayStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "player.h"
#include "PlayStatic.h"


// CPlayStatic 对话框

IMPLEMENT_DYNAMIC(CPlayStatic, CStatic)

CPlayStatic::CPlayStatic(CWnd* pParent /*=NULL*/)
	: CStatic()
{

}

CPlayStatic::~CPlayStatic()
{
}

void CPlayStatic::DoDataExchange(CDataExchange* pDX)
{
	CStatic::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlayStatic, CStatic)
END_MESSAGE_MAP()


// CPlayStatic 消息处理程序
