#pragma once


// CPlayStatic �Ի���

class CPlayStatic : public CStatic
{
	DECLARE_DYNAMIC(CPlayStatic)

public:
	CPlayStatic(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPlayStatic();

// �Ի�������
	enum { IDD = IDC_STATIC_SHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
