// PlayStatic.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "player.h"
#include "PlayStatic.h"


// CPlayStatic �Ի���

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


// CPlayStatic ��Ϣ�������
