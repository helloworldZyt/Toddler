
// basedmDlg.h : ͷ�ļ�
//

#pragma once


// CbasedmDlg �Ի���
class CbasedmDlg : public CDialogEx
{
// ����
public:
	CbasedmDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BASEDM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
