#include "pch.h"
#include "framework.h"
#include "ModbusMaster.h"
#include "ModbusMasterDlg.h"
#include "afxdialogex.h"
#include "crc.h"
#include "Modbus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CModbusMasterDlg::CModbusMasterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODBUSMASTER_DIALOG, pParent)
	, m_openTimes(0)
	, m_ComName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CModbusMasterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_OPENTIMES, m_openTimes);
	DDX_Text(pDX, IDC_EDIT1, m_ComName);
}

BEGIN_MESSAGE_MAP(CModbusMasterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CModbusMasterDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CModbusMasterDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CModbusMasterDlg::OnBnClickedButton3)
END_MESSAGE_MAP()

CModbus	g_Modbus;
BYTE g_byAppData[512];
int g_iAppDataNum;
BYTE g_byPhyData[512];
int g_iPhyDataNum;
int	g_iRegAddr;
int	g_iRegValue;

BOOL CModbusMasterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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


	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_openTimes = 0;
	m_iRegAddr = 0;
	m_iRegValue = 0;

	return TRUE;
}

void CModbusMasterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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


void CModbusMasterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


HCURSOR CModbusMasterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CModbusMasterDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);

	DCB dcb;
	BOOL IfSuccess;
	LPCWSTR PcComPort = m_ComName;

	handleCom = CreateFile(PcComPort, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (handleCom == INVALID_HANDLE_VALUE)
	{
		MessageBox(TEXT("Failed to open"));
	}
	else
	{
		IfSuccess = GetCommState(handleCom, &dcb);
		if (!IfSuccess)
		{
			MessageBox(TEXT("Failed to get status"));
		}
		else
		{
			dcb.BaudRate = CBR_9600;
			dcb.ByteSize = 8;
			dcb.Parity = NOPARITY;
			dcb.StopBits = ONESTOPBIT;

			IfSuccess = SetCommState(handleCom, &dcb);

			if (!IfSuccess)
			{
				MessageBox(TEXT("Failed to setup"));
			}
			else
			{
				CString cstr1;
				cstr1.Format(TEXT("BaudRate:%d，ByteSize:%d，Parity:%d，StopBits:%d"), dcb.BaudRate, dcb.ByteSize, dcb.Parity, dcb.StopBits);
				MessageBox(cstr1);
			}
			g_byAppData[0] = 0x01;
			g_byAppData[1] = 0x00;
			g_byAppData[2] = 0x00;
			g_byAppData[3] = 0x00;
			g_byAppData[4] = 0x01;
			g_iAppDataNum = 5;

			UpdateData(true);
			g_byAppData[1] = (m_iRegAddr & 0xff00) >> 8;
			g_byAppData[2] = (m_iRegAddr & 0x00ff);

			g_Modbus.HDLC_AppToPhy(g_byPhyData, &g_iPhyDataNum, g_byAppData, g_iAppDataNum);

			DWORD DataNum = g_iPhyDataNum;
			IfSuccess = WriteFile(handleCom, g_byPhyData, g_iPhyDataNum, &DataNum, NULL);
			if (!IfSuccess)
			{
				MessageBox(TEXT("Failed to write"));
			}

			BYTE Data[512];
			DWORD dataLength = 0;
			SetFilePointer(handleCom, 0, 0, FILE_BEGIN);
			IfSuccess = ReadFile(handleCom, Data, 8, &dataLength, NULL);
			CString cstr1;
			int i = 0;
			if (g_Modbus.HDLC_PhyToApp(Data, dataLength, g_byAppData, &g_iAppDataNum) != MODBUS_ERR)
			{
				int result = g_Modbus.AppDataAnalyse(g_byAppData, g_iAppDataNum, &g_iRegValue);
				if (result == 1)
				{
					if (i % 2 == 0)
					{
						i++;
						m_openTimes = m_openTimes + 1;
					}
				}
				else
				{
					MessageBox(TEXT("An unexpected response"));
				}
			}
		}
	}
	CloseHandle(handleCom);
}


void CModbusMasterDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);

	DCB dcb;
	BOOL IfSuccess;
	LPCWSTR PcComPort = m_ComName;

	handleCom = CreateFile(PcComPort, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (handleCom == INVALID_HANDLE_VALUE)
	{
		MessageBox(TEXT("Failed to open"));
	}
	else
	{
		IfSuccess = GetCommState(handleCom, &dcb);
		if (!IfSuccess)
		{
			MessageBox(TEXT("Failed to get status"));
		}
		else
		{
			dcb.BaudRate = CBR_9600;
			dcb.ByteSize = 8;
			dcb.Parity = NOPARITY;
			dcb.StopBits = ONESTOPBIT;

			IfSuccess = SetCommState(handleCom, &dcb);

			if (!IfSuccess)
			{
				MessageBox(TEXT("Failed to setup"));
			}
			else
			{
				CString cstr1;
				cstr1.Format(TEXT("BaudRate:%d，ByteSize:%d，Parity:%d，StopBits:%d"), dcb.BaudRate, dcb.ByteSize, dcb.Parity, dcb.StopBits);
				MessageBox(cstr1);
			}
			g_byAppData[0] = 0x03;
			g_byAppData[1] = 0x00;
			g_byAppData[2] = 0x00;
			g_byAppData[3] = 0x00;
			g_byAppData[4] = 0x01;
			g_iAppDataNum = 5;

			UpdateData(true);
			g_byAppData[1] = (m_iRegAddr & 0xff00) >> 8;
			g_byAppData[2] = (m_iRegAddr & 0x00ff);

			g_Modbus.HDLC_AppToPhy(g_byPhyData, &g_iPhyDataNum, g_byAppData, g_iAppDataNum);

			DWORD DataNum = g_iPhyDataNum;
			IfSuccess = WriteFile(handleCom, g_byPhyData, g_iPhyDataNum, &DataNum, NULL);
			if (!IfSuccess)
			{
				MessageBox(TEXT("Failed to write"));
			}

			BYTE Data[512];
			DWORD dataLength = 0;
			SetFilePointer(handleCom, 0, 0, FILE_BEGIN);
			IfSuccess = ReadFile(handleCom, Data, 8, &dataLength, NULL);
			CString cstr1;
			int i = 0;
			if (g_Modbus.HDLC_PhyToApp(Data, dataLength, g_byAppData, &g_iAppDataNum) != MODBUS_ERR)
			{
				int result = g_Modbus.AppDataAnalyse(g_byAppData, g_iAppDataNum, &g_iRegValue);
				if (result == 3)
				{
					g_Modbus.AppDataAnalyse(g_byAppData, g_iAppDataNum, &g_iRegValue);
					cstr1.Format(TEXT("RegValue:%d "), g_iRegValue);
					MessageBox(cstr1);
				}
				else
				{
					MessageBox(TEXT("An unexpected response"));
				}
			}
		}
	}
	CloseHandle(handleCom);
}


void CModbusMasterDlg::OnBnClickedButton3()
{
	UpdateData(TRUE);

	DCB dcb;
	BOOL IfSuccess;
	LPCWSTR PcComPort = m_ComName;

	handleCom = CreateFile(PcComPort, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (handleCom == INVALID_HANDLE_VALUE)
	{
		MessageBox(TEXT("Failed to open"));
	}
	else
	{
		IfSuccess = GetCommState(handleCom, &dcb);
		if (!IfSuccess)
		{
			MessageBox(TEXT("Failed to get status"));
		}
		else
		{
			dcb.BaudRate = CBR_9600;
			dcb.ByteSize = 8;
			dcb.Parity = NOPARITY;
			dcb.StopBits = ONESTOPBIT;

			IfSuccess = SetCommState(handleCom, &dcb);

			if (!IfSuccess)
			{
				MessageBox(TEXT("Failed to setup"));
			}
			else
			{
				CString cstr1;
				cstr1.Format(TEXT("BaudRate:%d，ByteSize:%d，Parity:%d，StopBits:%d"), dcb.BaudRate, dcb.ByteSize, dcb.Parity, dcb.StopBits);
				MessageBox(cstr1);
			}
			g_byAppData[0] = 0x05;
			g_byAppData[1] = 0x00;
			g_byAppData[2] = 0x00;
			g_byAppData[3] = 0x00;
			g_byAppData[4] = 0x01;
			g_iAppDataNum = 5;

			UpdateData(true);
			g_byAppData[1] = (m_iRegAddr & 0xff00) >> 8;
			g_byAppData[2] = (m_iRegAddr & 0x00ff);

			g_Modbus.HDLC_AppToPhy(g_byPhyData, &g_iPhyDataNum, g_byAppData, g_iAppDataNum);

			DWORD DataNum = g_iPhyDataNum;
			IfSuccess = WriteFile(handleCom, g_byPhyData, g_iPhyDataNum, &DataNum, NULL);
			if (!IfSuccess)
			{
				MessageBox(TEXT("Failed to write"));
			}

			BYTE Data[512];
			DWORD dataLength = 0;
			SetFilePointer(handleCom, 0, 0, FILE_BEGIN);
			IfSuccess = ReadFile(handleCom, Data, 8, &dataLength, NULL);
			CString cstr1;
			int i = 0;
			if (g_Modbus.HDLC_PhyToApp(Data, dataLength, g_byAppData, &g_iAppDataNum) != MODBUS_ERR)
			{
				int result = g_Modbus.AppDataAnalyse(g_byAppData, g_iAppDataNum, &g_iRegValue);
				if (result == 1)
				{
					MessageBox(TEXT("The light is changed"));
				}
				else if (result == 3)
				{
					cstr1.Format(TEXT("RegValue:%d"), g_iRegValue);
					MessageBox(cstr1);
				}
				else if (result == 5)
				{
					MessageBox(TEXT("The light is opened"));
				}
				else if (result == 7)
				{
					MessageBox(TEXT("The light is closed"));
				}
				else
				{
					MessageBox(TEXT("An unexpected response"));
				}
			}
		}
	}
	CloseHandle(handleCom);
}
