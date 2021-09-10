// HashCalc.cpp : 实现文件
//

#include "stdafx.h"
#include "HashDiff.h"
#include "HashCalc.h"
#include "afxdialogex.h"
//#include "file.h"
#include "sha1.h"
#include "md5.h"
#include "sha256.h"
#include "crc32.h"
// CHashCalc 对话框

IMPLEMENT_DYNAMIC(CHashCalc, CDialogEx)

CHashCalc::CHashCalc(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHashCalc::IDD, pParent)
	, m_Path(_T(""))
	, m_md5String(_T(""))
	, m_sha1String(_T(""))
	, m_sha256String(_T(""))
	, m_crc32String(_T(""))
{

}

CHashCalc::~CHashCalc()
{
}

void CHashCalc::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, m_Path);
	DDX_Text(pDX, IDC_EDIT_MD5, m_md5String);
	DDX_Text(pDX, IDC_EDIT_SHA1, m_sha1String);
	DDX_Text(pDX, IDC_EDIT_SHA256, m_sha256String);
	DDX_Text(pDX, IDC_EDIT_CRC32, m_crc32String);
	DDX_Control(pDX, IDC_MD5_CHECK, m_md5CheckBox);
	DDX_Control(pDX, IDC_SHA1_CHECK, m_sha1CheckBox);
	DDX_Control(pDX, IDC_SHA256_CHECK, m_sha256CheckBox);
	DDX_Control(pDX, IDC_CRC32_CHECK2, m_crc32CheckBox);
	DDX_Control(pDX, IDC_HASHCALC_STATUS, m_StatusShow);
}

void CHashCalc::ReSize(void)
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
BEGIN_MESSAGE_MAP(CHashCalc, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_OPEN, &CHashCalc::OnBnClickedBtnOpen)
END_MESSAGE_MAP()


// CHashCalc 消息处理程序


BOOL CHashCalc::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	GetClientRect(&rect);     //取客户区大小  
	old.x = rect.right - rect.left;
	old.y = rect.bottom - rect.top;

	m_md5CheckBox.SetCheck(1);
	m_sha1CheckBox.SetCheck(1);
	m_sha256CheckBox.SetCheck(1);
	m_crc32CheckBox.SetCheck(1);

	CRect status_rect;
	m_StatusShow.GetClientRect(status_rect);
	m_status.Create(WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, status_rect, this, 3);

	int nPanel[2] = { status_rect.Width()/2,-1 };      //分割尺寸
	m_status.SetParts(2, nPanel);  //分割状态栏
	m_status.SetText(_T("Already！"), 0, 0); //第一个分栏加入"这是第一个指示器"
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CHashCalc::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED) {

		ReSize();
	}
}


void CHashCalc::OnBnClickedBtnOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	PBYTE pMsgBuff = NULL;
	UINT uLenthMsg = 0;
	HANDLE hFile = NULL;
	DWORD dwFileSize = 0;
	CFileDialog OpenDlg(TRUE, (LPCTSTR)"txt", NULL, OFN_OVERWRITEPROMPT,
		(LPCTSTR)_T("All Files (*.*)|*.*|(*.exe)|*.exe||"));
	if (OpenDlg.DoModal() == IDCANCEL)
	{
		return;
	}
	CString FileName = OpenDlg.GetPathName();
	m_Path = FileName;

	hFile = CreateFile(FileName.GetBuffer(FileName.GetLength()), GENERIC_READ,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (NULL == hFile)
	{
		CloseHandle(hFile);
		return;
	}
	DWORD hignFileSile=0;
	dwFileSize = GetFileSize(hFile, &hignFileSile);
	CString _ = _T("");
	_.Format(_T("FileSize:%d(B)"),dwFileSize);
	m_status.SetText(_, 0, 0);
	MEMORYSTATUS status;//定义存放内存信息的变量
	GlobalMemoryStatus(&status);//调用GlobalMemoryStatus函数获取内存信息
	SIZE_T phyPart = status.dwTotalPhys;

	if(dwFileSize > phyPart-(1024*1024*20))
	{
		CloseHandle(hFile);
		return;
	}

	if(m_md5CheckBox.GetCheck())
	{
		md5 *m = new md5(hFile,dwFileSize);
		USES_CONVERSION;
		CString str = A2T(m->md5hash);
		m_md5String = _T("");
		m_md5String.Format(_T("%s"),str);
		delete m;
	}
	if(m_sha1CheckBox.GetCheck())
	{
		sha1 *m = new sha1(hFile,dwFileSize);
		USES_CONVERSION;
		CString str = A2T(m->sha1hash);
		m_sha1String = _T("");
		m_sha1String.Format(_T("%s"),str);
		delete m;
	}

	if(m_sha256CheckBox.GetCheck())
	{
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
			UpdateData(false);
			return;
		}

		PVOID pvFile = MapViewOfFile(hFileMap,FILE_MAP_READ, 0, 0, 0);
		if (pvFile == NULL)
		{
			CloseHandle(hFile);
			UpdateData(false);
			return;
		}
		ctx.update((unsigned char*)pvFile, dwFileSize);
		ctx.final(digest);
		char buffer[2 * SHA256::DIGEST_SIZE + 1] = {0x0};
		for (int i = 0; i < SHA256::DIGEST_SIZE; i++)
			sprintf_s(buffer + i * 2, 3, "%02x", digest[i]);
		USES_CONVERSION;
		CString str = A2T(buffer);
		m_sha256String = _T("");
		m_sha256String.Format(_T("%s"),str);
		CloseHandle(hFileMap);
	}
	if(m_crc32CheckBox.GetCheck()){
		crc32 *c = new crc32(hFile,dwFileSize);
		m_crc32String.Format(_T("%x"),c->crc32Value);
		delete c;
	}

	CloseHandle(hFile);
	UpdateData(false);
}
