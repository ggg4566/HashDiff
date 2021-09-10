#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
//#include <iostream>
using namespace std;
// CSearch 对话框

class CSearch : public CDialogEx
{
	DECLARE_DYNAMIC(CSearch)

public:
	CSearch(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSearch();

// 对话框数据
	enum { IDD = IDD_DIG_SEARCH };

protected:
	POINT old;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void ReSize(void);
	
	DECLARE_MESSAGE_MAP()
public:
	CString m_ShowFile;
	CString m_SearchKey;
	CListCtrl m_ListShowFiles;
	CStatic m_StatusRect;
	CStatusBarCtrl  m_status;
	CWinThread* pThread,*pThreadSearch;
	BOOL bIsSearch;
	BOOL bExit;
	CRITICAL_SECTION m_cs;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMClickListShowFiles(NMHDR *pNMHDR, LRESULT *pResult);
	UINT realThread(LPVOID pParam);
	UINT static initThread(LPVOID pParam);
	UINT static SearchFileThread(LPVOID pParam);
	UINT SearchFile(CString key);
	UINT GetAllFiles();
	CString GetFileTime(CString);
	vector<std::vector<CString>> m_dataList;
protected:
	afx_msg LRESULT OnUpdatelist(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLvnGetdispinfoListShowFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	CButton m_ButtonQuery;
	
};
