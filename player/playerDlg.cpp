// playerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "player.h"
#include "playerDlg.h"
#include "../include/dgdplaysdk.h"

#pragma comment(lib, "../debug/dgdplaysdk.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CplayerDlg 对话框




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


// CplayerDlg 消息处理程序

BOOL CplayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CplayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CplayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CplayerDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
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
