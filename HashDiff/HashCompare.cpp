// HashCompare.cpp : 实现文件
//

#include "stdafx.h"
#include "HashDiff.h"
#include "HashCompare.h"
#include "afxdialogex.h"
#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include <windows.h>

// CHashCompare 对话框
 extern  CStringArray  AllFiles; 
 extern bool isCount;
 SIZE_T nCounts =0;

IMPLEMENT_DYNAMIC(CHashCompare, CDialogEx)


void SplitCString(const CString& _cstr, const CString _flag, CStringArray& _resultArray)
{
	CString strSrc(_cstr);

	CStringArray& strResult = _resultArray;
	CString strLeft = _T("");

	int nPos = strSrc.Find(_flag);
	while(0 <= nPos)
	{
		strLeft = strSrc.Left(nPos);
		if (!strLeft.IsEmpty())
		{
			strResult.Add(strLeft);
		}
		strSrc = strSrc.Right(strSrc.GetLength() - nPos - 1);
		nPos = strSrc.Find(_flag);
	}

	if (!strSrc.IsEmpty()) {
		strResult.Add(strSrc);
	}
}


CHashCompare::CHashCompare(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHashCompare::IDD, pParent)
	, m_InputHash(_T(""))
	, m_radioHashGroup(0)
	, m_InputGroup(0)
	, m_LimitSizeOfFile(20)
	, m_DestGroup(0)
	, m_SelectDir(_T(""))
	, m_MinThread(10)
	, m_MaxThread(30)
	, m_FileSize(_T(""))
	, m_Text(_T(""))
{
	isStop = FALSE;
	mProgress = _T("0.00%");
	InitializeCriticalSection(&m_cs);
	pThread = NULL;
}

CHashCompare::~CHashCompare()
{
}

void CHashCompare::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STA_STATUS, m_statusRect);
	DDX_Text(pDX, IDC_EDIT_HASHS, m_InputHash);
	DDX_Radio(pDX, IDC_RADIO_MD5, m_radioHashGroup);
	DDX_Radio(pDX, IDC_RADIO_HASH, m_InputGroup);
	DDX_Text(pDX, IDC_EDIT_SIZELIMIT, m_LimitSizeOfFile);
	DDX_Radio(pDX, IDC_RADIO_ALL, m_DestGroup);
	DDX_Text(pDX, IDC_EDIT_PATH, m_SelectDir);
	DDX_Control(pDX, IDC_LIST_RESULT, m_List);
	DDX_Text(pDX, IDC_EDIT_MIN_THREAD, m_MinThread);
	DDX_Text(pDX, IDC_EDIT_MAX_THREAD, m_MaxThread);
	DDX_Text(pDX, IDC_EDIT_MAX_FILESIZE, m_FileSize);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_Text);
}


BEGIN_MESSAGE_MAP(CHashCompare, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_START, &CHashCompare::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CHashCompare::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_RADIO_IMPORT, &CHashCompare::OnImportHash)
	ON_BN_CLICKED(IDC_RADIO_DIR, &CHashCompare::OnBnClickedRadioDir)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_RESULT, &CHashCompare::OnInsertDataList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RESULT, &CHashCompare::OnShowRntryInText)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_COMMAND(ID_LISTMENU_COPYALL, &CHashCompare::OnListmenuCopyall)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_RESULT, &CHashCompare::OnPopMenu)
END_MESSAGE_MAP()


// CHashCompare 消息处理程序
void CHashCompare::ReSize(void)
{
	float fsp[2];
	POINT Newp; //获取现在对话框的大小
	CRect recta;
	GetClientRect(&recta);     //取客户区大小  
	Newp.x = recta.right - recta.left;
	Newp.y = recta.bottom - recta.top;
	fsp[0] = (float)Newp.x / old.x;
	fsp[1] = (float)Newp.y / old.y;
	CRect Rect;
	int woc;
	CPoint OldTLPoint, TLPoint; //左上角
	CPoint OldBRPoint, BRPoint; //右下角
	HWND  hwndChild = ::GetWindow(m_hWnd, GW_CHILD);  //列出所有控件  
	while (hwndChild)
	{
		woc = ::GetDlgCtrlID(hwndChild);//取得ID
		GetDlgItem(woc)->GetWindowRect(Rect);
		ScreenToClient(Rect);
		OldTLPoint = Rect.TopLeft();
		TLPoint.x = long(OldTLPoint.x*fsp[0]);
		TLPoint.y = long(OldTLPoint.y*fsp[1]);
		OldBRPoint = Rect.BottomRight();
		BRPoint.x = long(OldBRPoint.x *fsp[0]);
		BRPoint.y = long(OldBRPoint.y *fsp[1]);
		Rect.SetRect(TLPoint, BRPoint);
		GetDlgItem(woc)->MoveWindow(Rect, TRUE);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
	old = Newp;
}

BOOL CHashCompare::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect,status_rect;
	GetClientRect(&rect);     //取客户区大小  
	old.x = rect.right - rect.left;
	old.y = rect.bottom - rect.top;
	m_statusRect.GetClientRect(status_rect);
	m_status.Create(WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, status_rect, this, 3);
	int nPanel[2] = { status_rect.Width()/2,-1 };      //分割尺寸
	m_status.SetParts(2, nPanel);  //分割状态栏
	m_status.SetText(_T("Already！"), 0, 0); //第一个分栏加入"这是第一个指示器"

	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT|LVS_OWNERDATA);

	// 为列表视图控件添加三列  
	
	m_List.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 40);
	m_List.InsertColumn(1, _T("Hash"), LVCFMT_LEFT, 200);
	m_List.InsertColumn(2, _T("File"), LVCFMT_LEFT, 500);
	m_List.SetItemCountEx(m_dataList.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CHashCompare::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED) {

		ReSize();
	}
}


void CHashCompare::OnBnClickedBtnStart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	isStop = FALSE;
	nCounts = 0;
	mProgress = _T("0.00%");
	m_status.SetText(_T(""), 0, 0);
	m_status.SetText(_T(""), 0, 1);
	SIZE_T nSizeOfList = m_dataList.size();
	for(SIZE_T i=0;i<nSizeOfList;i++)
	{
		m_dataList[i].clear();
	}
	m_dataList.clear();
	
	if(m_LimitSizeOfFile<=0 || m_LimitSizeOfFile>1024)
	{
		m_status.SetText(_T("Please Set 0<FileSize(MB)<1024."), 0, 1);
		return;
	}
	if(m_DestGroup == 1)
	{
		if(m_SelectDir.IsEmpty())
		{
			m_status.SetText(_T("Please Select a Dir."), 0, 1);
			return;
		}
	}
	if(isCount){
		if(!m_InputHash.IsEmpty())
		{
			m_status.SetText(_T("Searching ..."), 0, 0);
			SplitCString(m_InputHash,_T("\r\n"),Hashs);
			m_btnStart.EnableWindow(FALSE);
			pThread = AfxBeginThread(SearchFileThread, (LPVOID)this);
			
		}else{
			m_status.SetText(_T("Please input hash."), 0, 1);
		}
		
	}else{
		m_status.SetText(_T("All Driver have't count end.Please wait a moment."), 0, 1);
	}
	
	UpdateData(FALSE);
}


void CHashCompare::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
	if(isStop == FALSE)
	{
		isStop = TRUE;
	}
}

UINT CHashCompare::SearchFileThread(LPVOID pParam) {
	UINT ret = 0;
	CHashCompare * pObj = (CHashCompare*)pParam;
	//ret = pObj->SearchFile();
	
	pObj->DoThreadPool();
	return ret;
}

UINT CHashCompare::SearchFile()
{
	SYSTEMTIME	SysTime;
	GetLocalTime(&SysTime);
	start_time = SysTime;
	SetTimer(1, 300, NULL);
	UINT ret = 0;
	SIZE_T nSizeOfAllFiles = 0;
	CString source = m_InputHash;
	CString FileName =_T("");
	SplitCString(source,_T("\r\n"),Hashs);

	nSizeOfAllFiles = AllFiles.GetSize();
	for (SIZE_T i=0;i<nSizeOfAllFiles;i++)
	{
		if(isStop == TRUE)
		{
			break;
		}
		SIZE_T nSizeOfHashs = Hashs.GetSize();
		FileName = AllFiles[i];
		
		for (SIZE_T j=0;j<nSizeOfHashs;j++)
		{
			HANDLE hFile;
			
			CString h =Hashs[j];
			h.Trim();
			hFile = CreateFile(FileName.GetBuffer(FileName.GetLength()), GENERIC_READ,
				0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == hFile)
			{
				CloseHandle(hFile);
				break;
			}
			__int64 fileSize = 0;
			DWORD lowSize, highSize;
			lowSize = GetFileSize(hFile, &highSize);

			fileSize = (__int64)highSize << 32 | lowSize;
			
			if((fileSize==0) || (fileSize > m_LimitSizeOfFile*1024*1024)){
				CloseHandle(hFile);
				break;
			}
			md5 *m = new md5(hFile,lowSize);
			char md5hash[33] = {0x0};
			lstrcpynA(md5hash,m->md5hash,33);
			delete m;
			CString theString = _T("");
			theString.Format(_T("%s"),CString(md5hash));
			
			if(theString==h) // find success
			{
				std::vector<CString>  vecTempDatda;
				size_t nCount = 0;
				nCount =m_dataList.size();
				CString id = _T("");
				id.Format(_T("%d"),++nCount);
				vecTempDatda.push_back(id);
				vecTempDatda.push_back(FileName);
				vecTempDatda.push_back(h);
				m_dataList.push_back(vecTempDatda);
				m_List.SetItemCountEx(m_dataList.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
			}
			CloseHandle(hFile);
		}
	}
	m_status.SetText(_T("Search task have already finsh!"), 0, 0);
	KillTimer(1);
	UpdateData(FALSE);
	return ret;
}
void CHashCompare::OnImportHash()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	PBYTE pMsgBuff = NULL;
	UINT uLenthMsg = 0;
	HANDLE hFile = NULL;
	DWORD dwFileSize = 0;
	CFileDialog OpenDlg(TRUE, (LPCTSTR)"txt", NULL, OFN_OVERWRITEPROMPT,
		(LPCTSTR)_T("(*.txt)|*.txt|All Files (*.*)|*.*||"));
	if (OpenDlg.DoModal() == IDCANCEL)
	{
		m_InputGroup = 0;
		UpdateData(FALSE);
		return;
	}
	CString FileName = OpenDlg.GetPathName();
	hFile = CreateFile(FileName.GetBuffer(FileName.GetLength()), GENERIC_READ,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (NULL == hFile)
	{
		UpdateData(FALSE);
		return;
	}
	dwFileSize = GetFileSize(hFile, NULL);
	uLenthMsg = dwFileSize;
	pMsgBuff = new BYTE[uLenthMsg];
	memset(pMsgBuff, 0, uLenthMsg);
	if (!ReadFile(hFile, pMsgBuff, dwFileSize, &dwFileSize, NULL))
	{
		UpdateData(FALSE);
		return;
	}
	CloseHandle(hFile);
	CString Text(pMsgBuff);
	delete pMsgBuff;
	m_InputHash = Text;
	UpdateData(FALSE);

}


void CHashCompare::OnBnClickedRadioDir()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	/*
	TCHAR szDir[MAX_PATH];
	CString Path;
	BROWSEINFO bi; 
	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = _T("Select Dir:");
	bi.iImage = 0;

	bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = (LPARAM)(LPCTSTR)Path;

	LPITEMIDLIST lp = SHBrowseForFolder(&bi);

	if (lp && SHGetPathFromIDList(lp,Path.GetBuffer(0)))
	{
		m_SelectDir = Path;

	}
	*/
	UpdateData(FALSE);	
}

void CHashCompare::OnInsertDataList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	LV_ITEM* pItem = &(pDispInfo)->item;
	int nItem = pItem->iItem;
	if (nItem < 0 || nItem >= m_dataList.size())
	{
		return;
	}
	if (pItem->mask & LVIF_TEXT)
	{
		int nSubItem = pItem->iSubItem;
		_tcscpy(pItem->pszText, m_dataList[nItem][nSubItem]);
	}
	*pResult = 0;
}


void CHashCompare::OnShowRntryInText(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;

	if (-1 != pNMListView->iItem)        // 如果iItem不是-1，就说明有列表项被选择   
	{
		// 获取被选择列表项第一个子项的文本   
		// 将选择的语言显示与编辑框中   
		CString hash = m_List.GetItemText(pNMListView->iItem, 1);
		CString FileName = m_List.GetItemText(pNMListView->iItem, 2);
		m_Text.Format(_T("%s|%s"),hash,FileName);
		UpdateData(false);
	}
	*pResult = 0;
}

void CHashCompare::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		{
	
			SYSTEMTIME	CurrentSysTime;
			GetLocalTime(&CurrentSysTime);
			COleDateTime dTimeF(start_time);
			COleDateTime dTimeS(CurrentSysTime);
			COleDateTimeSpan dTimeSpan = dTimeS - dTimeF;
			CString cost_time = dTimeSpan.Format(_T("%H:%M:%S"));
				//cost_time.Format(_T("%02d:%02d:%02d"), (int)dTimeSpan.GetHours(), (int)dTimeSpan.GetMinutes(), (int)dTimeSpan.GetSeconds());
			CString text = _T("");
			text.Format(_T("%s %s"),cost_time,mProgress);
			m_status.SetText(text, 0, 1);
		}
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void  NTAPI CHashCompare::poolThreadWork(_Inout_ PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID Context,
	_Inout_ PTP_WORK Work){
		
		CHashCompare * pObj = (CHashCompare*)Context;
		if(pObj->isStop != TRUE){
			pObj->SearchFileThreadPool();
		}
}

UINT CHashCompare::SearchFileThreadPool(){
	UINT ret = 0;
	SIZE_T nSizeOfAllFiles = 0;
	CString FileName =_T("");

	EnterCriticalSection(&m_cs);
	nCounts++;
	FileName = AllFiles[nCounts-1];
	LeaveCriticalSection(&m_cs);
	if(m_DestGroup==1)
	{
		DWORD Len = m_SelectDir.GetLength();
		CString path = FileName.Left(Len);
		if(path != m_SelectDir)
		{
			UpdateData(FALSE);
			return 0;
		}
	}
	nSizeOfAllFiles = AllFiles.GetSize();
	mProgress.Format(_T("%.3f%s"),double(nCounts)/(double)nSizeOfAllFiles*100,_T("%"));
	SIZE_T nSizeOfHashs = Hashs.GetSize();

	for (SIZE_T j=0;j<nSizeOfHashs;j++)
	{
			HANDLE hFile;
			CString h =Hashs[j];
			h.Trim();
			hFile = CreateFile(FileName.GetBuffer(FileName.GetLength()), GENERIC_READ,
				0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == hFile)
			{
				CloseHandle(hFile);
				break;
			}
			__int64 fileSize = 0;
			DWORD lowSize, highSize;
			lowSize = GetFileSize(hFile, &highSize);

			fileSize = (__int64)highSize << 32 | lowSize;

			if((fileSize==0) || (fileSize > m_LimitSizeOfFile*1024*1024)){
				CloseHandle(hFile);
				break;
			}
			
			CString SizeOfFile =_T("");
			SizeOfFile.Format(_T("%d"),lowSize);
			
			if(!m_FileSize.IsEmpty()&&SizeOfFile != m_FileSize)
			{
				CloseHandle(hFile);
				UpdateData(FALSE);
				return 0;
			}
			char hash[256] = {0x0};
			switch(m_radioHashGroup){
			case 0: {
						md5 *m = new md5(hFile,lowSize);
						lstrcpynA(hash,m->md5hash,33);
						delete m;
					}
					break;
			case 1: {
						sha1 *m = new sha1(hFile,lowSize);
						lstrcpynA(hash,m->sha1hash,41);
						delete m;
					} 
					break;
			case 2: {
						unsigned char digest[SHA256::DIGEST_SIZE];
						memset(digest, 0, SHA256::DIGEST_SIZE);
						SHA256 ctx = SHA256();
						ctx.init();
						HANDLE hFileMap = CreateFileMapping(
							hFile, // 如果这值为INVALID_HANDLE_VALUE,是合法的，上步一定测试啊  
							NULL, // 默认安全性  
							PAGE_READONLY, // 可写  
							0, // 2个32位数示1个64位数，最大文件字节数，  
							// 高字节，文件大小小于4G时，高字节永远为0  
							0,//dwFileSize, // 此为低字节，也就是最主要的参数，如果为0，取文件真实大小  
							NULL);
						if (hFileMap == NULL)
						{
							CloseHandle(hFile);
							break;
						}

						PVOID pvFile = MapViewOfFile(hFileMap,FILE_MAP_READ, 0, 0, 0);
						if (pvFile == NULL)
						{
							CloseHandle(hFile);
							break;
						}
						ctx.update((unsigned char*)pvFile, lowSize);
						ctx.final(digest);
						char buffer[2 * SHA256::DIGEST_SIZE + 1] = {0x0};
						for (int i = 0; i < SHA256::DIGEST_SIZE; i++)
							sprintf_s(buffer + i * 2, 3, "%02x", digest[i]);
						cout << "sha256('" << "input" << "'):" << std::string(buffer) << endl;
						lstrcpynA(hash,buffer,2 * SHA256::DIGEST_SIZE + 1);
					}
					break;
			default:
				break;
			}
			
			CString theString = _T("");
			theString.Format(_T("%s"),CString(hash));

			if(theString==h) // find success
			{
				EnterCriticalSection(&m_cs);
				std::vector<CString>  vecTempDatda;
				size_t nCount = 0;
				nCount =m_dataList.size();
				CString id = _T("");
				id.Format(_T("%d"),++nCount);
				vecTempDatda.push_back(id);
				vecTempDatda.push_back(h);
				vecTempDatda.push_back(FileName);
				m_dataList.push_back(vecTempDatda);
				LeaveCriticalSection(&m_cs);
				m_List.SetItemCountEx(m_dataList.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
				m_List.Invalidate();
			}
			CloseHandle(hFile);
		}
	UpdateData(FALSE);
	return ret;
}

VOID CHashCompare::DoThreadPool(){
	SYSTEMTIME	SysTime;
	GetLocalTime(&SysTime);
	start_time = SysTime;
	SetTimer(1, 300, NULL);
	//创建线程池
	PTP_POOL threadPool = CreateThreadpool(NULL);
	SetThreadpoolThreadMinimum(threadPool, m_MinThread);
	SetThreadpoolThreadMaximum(threadPool, m_MinThread);
	//初始化环境
	TP_CALLBACK_ENVIRON te;
	InitializeThreadpoolEnvironment(&te);
	SetThreadpoolCallbackPool(&te, threadPool);
	Sleep(2000);
	
	PTP_WORK pw = CreateThreadpoolWork(poolThreadWork,this,&te);
	SIZE_T nSizeOfAllFiles = AllFiles.GetSize();
	for (SIZE_T i=0;i<nSizeOfAllFiles;i++)
	{
		if(isStop == TRUE)
		{
			break;
		}
		SubmitThreadpoolWork(pw);
		Sleep(10);
	}
	WaitForThreadpoolWorkCallbacks(pw,FALSE);
	CloseThreadpoolWork(pw);

	/*清理线程池的环境变量*/
	DestroyThreadpoolEnvironment(&te);
	/*关闭线程池*/
	CloseThreadpool(threadPool);
	m_status.SetText(_T("Search task have already finsh!"), 0, 0);
	KillTimer(1);
	m_btnStart.EnableWindow(TRUE);
}

void CHashCompare::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	isStop = TRUE;
	if(pThread !=NULL)
	{
		WaitForSingleObject(pThread->m_hThread, INFINITE );
	}
	CDialogEx::OnClose();
}


void CHashCompare::OnListmenuCopyall()
{
	// TODO: 在此添加命令处理程序代码
	int Counts = m_List.GetItemCount();
	CString data = _T("");
	for (int i=0;i<Counts;i++){
		CString itemdata;
		itemdata.Format(_T("%s|%s|%s\r\n"),m_List.GetItemText(i,0),m_List.GetItemText(i,1),m_List.GetItemText(i,2));
		data+=itemdata;
	}
	pClipBuffer = NULL;
	if(OpenClipboard()){
		HGLOBAL clipbuffer;
		wchar_t* buffer;
		int buf_szie = data.GetLength();
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE,(data.GetLength()+1)*sizeof(wchar_t));
		buffer = (wchar_t*)GlobalLock(clipbuffer);
		memset(buffer,0,(buf_szie+1)*sizeof(wchar_t));
		memcpy_s(buffer,buf_szie*sizeof(wchar_t),data.GetBuffer(0),buf_szie*sizeof(wchar_t));
		data.ReleaseBuffer();
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_UNICODETEXT,clipbuffer);
		CloseClipboard();
	}
}


void CHashCompare::OnPopMenu(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	POINT pt;
	GetCursorPos(&pt);

	CMenu menu;
	menu.LoadMenu(IDR_MENU_COPY);
	CMenu * pop = menu.GetSubMenu(0);
	pop->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	*pResult = 0;
}
