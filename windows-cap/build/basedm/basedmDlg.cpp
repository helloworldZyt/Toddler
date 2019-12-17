
// basedmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "basedm.h"
#include "basedmDlg.h"
#include "afxdialogex.h"
#include <string>
#include <stdarg.h>
#include <string.h>

#include "bdm_hymedia.h"
#include "bdm_hymedia_commdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define USE_UNICODE

using namespace std;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
CbasedmDlg* g_winhand = NULL;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CbasedmDlg 对话框




CbasedmDlg::CbasedmDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CbasedmDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CbasedmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEBUG_OUT, CEdit_Log_View);
	DDX_Control(pDX, IDC_LOCALVIEW, CStatic_Local_View);
	DDX_Control(pDX, IDC_EDIT_INPUT0, CEdit_Input0);
	DDX_Control(pDX, IDC_EDIT_INPUT1, CEdit_Input1);
}

BEGIN_MESSAGE_MAP(CbasedmDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CbasedmDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_DOIT, &CbasedmDlg::OnBnClickedDoit)
END_MESSAGE_MAP()


// CbasedmDlg 消息处理程序

BOOL CbasedmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	g_winhand = this;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CbasedmDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CbasedmDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CbasedmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CbasedmDlg::myDbgOut(const char* s)
{
	static int maxline = 0;
	CString csOld = _T("");
	CString csNew = _T("");
	if (0==maxline)
	{
#ifdef USE_UNICODE
		CEdit_Log_View.SetWindowText(_T("first line"));
#else
		CEdit_Log_View.SetWindowText(_T("first line\n\r\n\r"));
#endif
	}
	maxline++;
	
	CEdit_Log_View.GetWindowText(csOld);
	
#ifdef USE_UNICODE
	csNew = "\n\r\n\r";
	csNew += s;
#else
	csNew.Format(_T("%s \n\r\n\r"), s);
#endif
	csOld += csNew;
	CEdit_Log_View.SetWindowText(csOld);
	return;
}

void dbgOut(const char* f, ...)
{
	char outbuf[1024];
	memset(outbuf, 0, sizeof(outbuf));
	va_list vl;
	va_start(vl, f);
	_vsnprintf(outbuf, sizeof(outbuf)-1, f, vl);
	va_end(vl);
	g_winhand->myDbgOut(outbuf);
}

void CbasedmDlg::OnBnClickedStart()
{
	// TODO: 在此添加控件通知处理程序代码
	static int cntstart = 0;
	dbgOut("you click start %d times!\n", cntstart++);
}


void CbasedmDlg::OnBnClickedDoit()
{
	// TODO: 在此添加控件通知处理程序代码
	static int cntdo = 0;
	dbgOut("you click start %d times!\n", cntdo++);
}
