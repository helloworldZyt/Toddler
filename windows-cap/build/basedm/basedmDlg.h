
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
	afx_msg void OnBnClickedDoit();

	void myDbgOut(const char* s);
public:
	CStatic CStatic_Local_View;
	CEdit CEdit_Log_View;
	CEdit   CEdit_Input0;
	CEdit   CEdit_Input1;
};
