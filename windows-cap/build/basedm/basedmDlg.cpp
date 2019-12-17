
// basedmDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
CbasedmDlg* g_winhand = NULL;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CbasedmDlg �Ի���




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


// CbasedmDlg ��Ϣ�������

BOOL CbasedmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	g_winhand = this;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CbasedmDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	static int cntstart = 0;
	dbgOut("you click start %d times!\n", cntstart++);
}


void CbasedmDlg::OnBnClickedDoit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	static int cntdo = 0;
	dbgOut("you click start %d times!\n", cntdo++);
}
