
// basedmDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "basedm.h"
#include "basedmDlg.h"
#include "afxdialogex.h"
#include "bdm_crypto.h"
#include <string>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string GBToUTF8(const char* str)
{
	std::string result;
	WCHAR *strSrc;
	char *szRes;

	//�����ʱ�����Ĵ�С
	int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i+1];
	MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc, i);

	//�����ʱ�����Ĵ�С
	i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new char[i+1];
	int j=WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete []strSrc;
	delete []szRes;

	return result;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

CbasedmDlg* g_wnd = NULL;

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
	DDX_Control(pDX, IDC_LOCALVIEW, CStatic_local_wnd);
	DDX_Control(pDX, IDC_LOGVIEW, CStatic_Debug_out);
	DDX_Control(pDX, IDC_CRYPTO_KEY, CEdit_crypto_key);
}

BEGIN_MESSAGE_MAP(CbasedmDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CbasedmDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_DECRYPTO, &CbasedmDlg::OnBnClickedDecrypto)
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
	g_wnd = this;

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

void CbasedmDlg::myDbgOut(int val, const char* info, int infolen)
{
	CString oldTxt;
	CString cstrResult;
	static int ncnt = 0;
	if (6 < ncnt++)
	{
		ncnt = 0;
		CStatic_Debug_out.SetWindowTextA(_T(""));
	}
	cstrResult.Format(_T("val:%d,info:%s len:%d\r\n"),val, info, infolen);
	CStatic_Debug_out.GetWindowText(oldTxt);
	oldTxt += cstrResult;
	CStatic_Debug_out.SetWindowText(oldTxt);
	return;
}

void dbg_out(int val, const char* info, int infolen)
{
	return g_wnd->myDbgOut(val, info, infolen);
}
void progress_crypto(double val)
{
	char buf[1024] = {0};
	sprintf(buf, "%f", val);
	g_wnd->myDbgOut(0, buf, 0);
}
void CbasedmDlg::OnBnClickedStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strFilePath;
	CString strCrK;
	unsigned char kbuf[16] = "";

	GetDlgItemText(IDC_CRYPTO_KEY, strCrK);
	string cryptokey = strCrK.GetBuffer();
	cryptokey = GBToUTF8(cryptokey.c_str());

	if (16 > cryptokey.length())
	{
		myDbgOut(-1, "key len error!", cryptokey.length());
		return;
	}
	
	GetDlgItemText(IDC_SOURCE_FILE, strFilePath);
	std::string strPathAndFile = strFilePath.GetBuffer();
	strPathAndFile = GBToUTF8(strPathAndFile.c_str());
	int nPos = strPathAndFile.rfind("\\");
	std::string strFile = strPathAndFile.substr(nPos+1, strPathAndFile.size());

	myDbgOut(0, strPathAndFile.c_str(), strPathAndFile.length());
	myDbgOut(0, strFile.c_str(), strFile.length());

	bdm_set_cb(dbg_out, progress_crypto);
	memcpy(kbuf, cryptokey.c_str(), 16);
	bdm_c_file(strPathAndFile.c_str(), kbuf, 16);
	return;
}


void CbasedmDlg::OnBnClickedDecrypto()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strFilePath;
	CString strCrK;
	unsigned char kbuf[16] = "";

	GetDlgItemText(IDC_CRYPTO_KEY, strCrK);
	string cryptokey = strCrK.GetBuffer();
	cryptokey = GBToUTF8(cryptokey.c_str());

	if (16 > cryptokey.length())
	{
		myDbgOut(-1, "key len error!", cryptokey.length());
		return;
	}

	GetDlgItemText(IDC_SOURCE_FILE, strFilePath);
	std::string strPathAndFile = strFilePath.GetBuffer();
	strPathAndFile = GBToUTF8(strPathAndFile.c_str());
	int nPos = strPathAndFile.rfind("\\");
	std::string strFile = strPathAndFile.substr(nPos+1, strPathAndFile.size());

	bdm_set_cb(dbg_out, progress_crypto);
	memcpy(kbuf, cryptokey.c_str(), 16);
	bdm_d_file(strPathAndFile.c_str(), kbuf, 16);
}
