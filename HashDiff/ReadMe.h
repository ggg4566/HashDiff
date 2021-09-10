#pragma once
#include "afxwin.h"


// CReadMe 对话框

class CReadMe : public CDialogEx
{
	DECLARE_DYNAMIC(CReadMe)

public:
	CReadMe(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CReadMe();

// 对话框数据
	enum { IDD = IDD_DIG_README };

protected:
	POINT old;
	CBrush m_bkbrush;
	CFont m_stafont;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void ReSize(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	CStatic m_StaticText;
};
