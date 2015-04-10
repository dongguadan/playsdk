// playerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "player.h"
#include "playerDlg.h"
#include "../include/dgdplaysdk.h"

#pragma comment(lib, "../debug/dgdplaysdk.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CplayerDlg �Ի���




CplayerDlg::CplayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CplayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CplayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SHOW, m_playStatic);
}

BEGIN_MESSAGE_MAP(CplayerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CplayerDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CplayerDlg ��Ϣ�������

BOOL CplayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CplayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CplayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CplayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CplayerDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
	HWND hwd = m_playStatic.m_hWnd;

    DGD_PLAY_Init();
	int idle = DGD_PLAY_CaptureFrmoFile("D:\\Projects\\dgdsdk\\sdktest2\\test_4.wmv");
	if (idle != 0)
	{
	    OutputDebugStringA("DGD_PLAY_CaptureFrmoFile fail!\n");
        DGD_PLAY_Cleanup();
		return;
	}

	int chn = DGD_PLAY_CreateImage(hwd);
	if (chn != 0)
	{
	    OutputDebugStringA("DGD_PLAY_CreateImage fail!\n");
	    DGD_PLAY_ReleaseCapture(NULL);
        DGD_PLAY_Cleanup();
		return;
	}

	int len = 0;
	YUVFrame *frame = NULL;

	char videobuffer[1024 * 500] = { 0 };
	while (DGD_PLAY_QueryCapture(idle, videobuffer, &len) == 0)
	{
		OutputDebugStringA("query frame from file success!\n");
		if ((frame = DGD_PLAY_DecodeYUV(videobuffer, len)) != 0)
		{
		    OutputDebugStringA("render frame from file success!\n");
			DGD_PLAY_UpdateImage(chn, frame);
		    Sleep(100);
		}
	}

	OutputDebugStringA("exit\n");
	DGD_PLAY_ReleaseImage(chn);
	DGD_PLAY_ReleaseCapture(NULL);
    DGD_PLAY_Cleanup();
}
