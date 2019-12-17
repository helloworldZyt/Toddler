
// basedmDlg.h : 头文件
//

#pragma once


// CbasedmDlg 对话框
class CbasedmDlg : public CDialogEx
{
// 构造
public:
	CbasedmDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BASEDM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedDecrypto();

public:
	void myDbgOut0(const char* s);

	//CStatic CStatic_Debug_out;
	CStatic CStatic_local_wnd;
	CEdit   CEdit_crypto_key;
	CEdit	CEdit_DbgOut;
	afx_msg void OnBnClickedStitch();
	afx_msg void OnBnClickedSliceFile();
	afx_msg void OnBnClickedClrlog();
};
