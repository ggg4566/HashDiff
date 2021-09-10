
// HashDiffDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "HashCalc.h"
#include "HashCompare.h"
#include "Search.h"
#include "ReadMe.h"
// CHashDiffDlg 对话框
class CHashDiffDlg : public CDialogEx
{
// 构造
public:
	CHashDiffDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HASHDIFF_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	POINT old;
	CTabCtrl m_TabView;
	CDialog* pDialog[4];
	int m_CurSelTab;
	CSearch m_SearchDlg;
	CHashCompare m_HashCompareDlg;
	CHashCalc m_HashCalcDlg;
	CReadMe m_ReadMeDlg;
	CRect m_rect;
	afx_msg void OnTcnSelchangeTabView(NMHDR *pNMHDR, LRESULT *pResult);
	void ReSize(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
