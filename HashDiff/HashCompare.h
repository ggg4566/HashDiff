#pragma once
#include "afxwin.h"
#include <vector>
#include "afxcmn.h"


// CHashCompare 对话框

class STRUCT_DATA{
public:
	LPVOID * pObj;
	CString Value;
	STRUCT_DATA(LPVOID * pObj,CString Value){
		this->pObj = pObj;
		this->Value = Value;
	}
};

class CHashCompare : public CDialogEx
{
	DECLARE_DYNAMIC(CHashCompare)

public:
	CHashCompare(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHashCompare();

// 对话框数据
	enum { IDD = IDD_DIG_HASH_DIFF };

protected:
	POINT old;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void ReSize(void);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnImportHash();
	afx_msg void OnBnClickedRadioDir();
	afx_msg void OnInsertDataList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnShowRntryInText(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	UINT static SearchFileThread(LPVOID pParam);
	UINT SearchFile();

	void static NTAPI poolThreadWork(_Inout_ PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID Context,
		_Inout_ PTP_WORK Work);
	UINT SearchFileThreadPool();
	VOID DoThreadPool();
	CRITICAL_SECTION m_cs;
	CStatusBarCtrl  m_status;
	CStatic m_statusRect;
	CString m_InputHash;
	int m_radioHashGroup;
	int m_InputGroup;
	int m_LimitSizeOfFile;
	int m_DestGroup;
	
	CString m_SelectDir;
	CStringArray Hashs;
	CWinThread* pThread;
	CString mProgress;
	std::vector<std::vector<CString>> m_dataList;
	CListCtrl m_List;
	SYSTEMTIME start_time;
	BOOL isStop;
	afx_msg void OnClose();
	int m_MinThread;
	int m_MaxThread;
	CString m_FileSize;
	char* pClipBuffer;
	afx_msg void OnListmenuCopyall();
	afx_msg void OnPopMenu(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_btnStart;
	CString m_Text;
};
