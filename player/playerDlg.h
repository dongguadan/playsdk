// playerDlg.h : ͷ�ļ�
//

#pragma once

#include "PlayStatic.h"
// CplayerDlg �Ի���
class CplayerDlg : public CDialog
{
// ����
public:
	CplayerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CPlayStatic m_playStatic;
	afx_msg void OnBnClickedOk();
};
