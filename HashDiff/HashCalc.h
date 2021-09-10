#pragma once
#include "afxwin.h"


// CHashCalc 对话框

class CHashCalc : public CDialogEx
{
	DECLARE_DYNAMIC(CHashCalc)

public:
	CHashCalc(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHashCalc();

// 对话框数据
	enum { IDD = IDD_DIG_HASH_CALC };

protected:
	POINT old;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void ReSize(void);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnOpen();
	CString m_Path;
	CString m_md5String;
	CString m_sha1String;
	CString m_sha256String;
	CString m_crc32String;
	CButton m_md5CheckBox;
	CButton m_sha1CheckBox;
	CButton m_sha256CheckBox;
	CButton m_crc32CheckBox;
	CStatusBarCtrl  m_status;
	CStatic m_StatusShow;
};
