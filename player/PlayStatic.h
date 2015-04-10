#pragma once


// CPlayStatic 对话框

class CPlayStatic : public CStatic
{
	DECLARE_DYNAMIC(CPlayStatic)

public:
	CPlayStatic(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlayStatic();

// 对话框数据
	enum { IDD = IDC_STATIC_SHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
