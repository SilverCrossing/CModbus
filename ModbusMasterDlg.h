
#pragma once


class CModbusMasterDlg : public CDialogEx
{
public:
	CModbusMasterDlg(CWnd* pParent = nullptr);


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODBUSMASTER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);


protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	int m_openTimes;
	HANDLE handleCom;
	int	m_iRegAddr;
	int	m_iRegValue;
	void (*m_OnDataReceivedHandler)(const unsigned char* pData, int dataLength);
	void (*m_OnDataSentHandler)(void);
public:
	afx_msg void OnBnClickedButton1();
private:
	CString m_ComName;
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
