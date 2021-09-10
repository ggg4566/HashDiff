// ReadMe.cpp : 实现文件
//

#include "stdafx.h"
#include "HashDiff.h"
#include "ReadMe.h"
#include "afxdialogex.h"


// CReadMe 对话框

IMPLEMENT_DYNAMIC(CReadMe, CDialogEx)

CReadMe::CReadMe(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReadMe::IDD, pParent)
{

}

CReadMe::~CReadMe()
{
}

void CReadMe::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SHOW, m_StaticText);
}


BEGIN_MESSAGE_MAP(CReadMe, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CReadMe 消息处理程序

BOOL CReadMe::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rect;
	GetClientRect(&rect);     //取客户区大小  
	old.x = rect.right - rect.left;
	old.y = rect.bottom - rect.top;

	m_bkbrush.CreateSolidBrush(RGB(255,255,255)); 
	m_stafont.CreatePointFont(150,_T("楷体"));
	m_StaticText.SetFont(&m_stafont);
	CString szText = _T("https://github.com/ggg4566/HashDiff\r\n")
		_T("author:flystart of ms509\r\n")
		_T("email:root@flystart.org\r\n\r\n")
		_T("1.全盘搜索请以管理员权限运行\r\n")
		_T("2.通过样本 hash检索文件，可以限制特定文件大小以及特定目录\r\n")
		_T("3.文件遍历使用和everything相同的方法，仅支持 NTFS格式磁盘");
	m_StaticText.SetWindowText(szText);
	return TRUE; 
}

void CReadMe::ReSize(void)
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


void CReadMe::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED) {

		ReSize();
	}
}

HBRUSH CReadMe::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	HBRUSH org_hbr = hbr;
	//TODO:  在此更改 DC 的任何特性
	switch (nCtlColor)
	{

	case CTLCOLOR_STATIC:
		switch (pWnd->GetDlgCtrlID())//对某一个特定控件进行判断  
		{
			// first CEdit control ID  
		case IDC_STATIC_SHOW:   
			{
				pDC->SetTextColor(RGB(255,255,255)); // change the text color  
				pDC->SetBkMode(TRANSPARENT);
				hbr=(HBRUSH)GetStockObject(NULL_BRUSH);
			}
			break;
			// second CEdit control ID  
		default:
			hbr = org_hbr;
		}
		break;
	default:
		hbr = org_hbr;
	}
	
	return hbr;
}


void CReadMe::OnPaint()
{
	
		CPaintDC dc(this); // device context for painting
		// TODO: 在此处添加消息处理程序代码
		// 不为绘图消息调用 CDialogEx::OnPaint()
		CRect rect;
		GetClientRect(rect);
		//dc.FillSolidRect(rect,RGB(78,143,246)); //设置背景色
		dc.FillSolidRect(rect,RGB(97, 120, 140));
}