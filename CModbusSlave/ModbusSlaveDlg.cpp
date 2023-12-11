#include "pch.h"
#include "framework.h"
#include "ModbusSlave.h"
#include "ModbusSlaveDlg.h"
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

// 实现
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

CModbusSlaveDlg::CModbusSlaveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODBUSSLAVE_DIALOG, pParent)
	, m_light(_T(""))
	, m_ComName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CModbusSlaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LIGHT, m_light);
	DDX_Text(pDX, IDC_EDIT1, m_ComName);
}

BEGIN_MESSAGE_MAP(CModbusSlaveDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CModbusSlaveDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

CModbus	g_Modbus;
BYTE g_byAppData[512];
int g_iAppDataNum;
BYTE g_byPhyData[512];
int g_iPhyDataNum;
int	g_iRegAddr;
int	g_iRegValue;

BOOL CModbusSlaveDlg::OnInitDialog()
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

	m_light = "close";

	return TRUE;
}

void CModbusSlaveDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CModbusSlaveDlg::OnPaint()
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


HCURSOR CModbusSlaveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CModbusSlaveDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);

	DCB dcb;
	BOOL IfSuccess;
	BYTE ReceiveBuffer[512];
	DWORD dw_BytesRead = 0;
	LPCWSTR PcComPort = m_ComName;

	handleCom = CreateFile(PcComPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (handleCom == INVALID_HANDLE_VALUE)
	{
		MessageBox(TEXT("Failed to open the Com"));
	}
	else
	{
		IfSuccess = GetCommState(handleCom, &dcb);
		if (!IfSuccess)
		{
			MessageBox(TEXT("Failed to get the state"));
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
				MessageBox(TEXT("Failed to set the state"));
			}
			else
			{
				CString cstr1;
				cstr1.Format(TEXT("BaudRate:%d,ByteSize:%d,Parity:%d,StopBits:%d"), dcb.BaudRate, dcb.ByteSize, dcb.Parity, dcb.StopBits);
				MessageBox(cstr1);
				COMMTIMEOUTS Timeouts;
				Timeouts.ReadIntervalTimeout = MAXDWORD;
				Timeouts.ReadTotalTimeoutConstant = 0;
				Timeouts.ReadTotalTimeoutConstant = 0;

				IfSuccess = SetCommTimeouts(handleCom, &Timeouts);
				if (!IfSuccess)
				{
					MessageBox(TEXT("Failed to set timeouts"));
				}
				DWORD DataNum;
				BOOL light = FALSE;
				while (1)
				{
					IfSuccess = ReadFile(handleCom, ReceiveBuffer, 8, &dw_BytesRead, NULL);

					if (IfSuccess && 0 < dw_BytesRead)
					{

						if (g_Modbus.HDLC_PhyToApp(ReceiveBuffer, dw_BytesRead, g_byAppData, &g_iAppDataNum) != MODBUS_ERR)
						{
							int result = g_Modbus.AppDataAnalyse(g_byAppData, g_iAppDataNum, &g_iRegValue);
							switch (result)
							{
							case 1:
								if (light == FALSE)
								{
									m_light = "On";
									light = TRUE;
								}
								else
								{
									m_light = "Off";
									light = FALSE;
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
								DataNum = g_iPhyDataNum;
								IfSuccess = WriteFile(handleCom, g_byPhyData, g_iPhyDataNum, &DataNum, NULL);
								if (!IfSuccess)
								{
									MessageBox(TEXT("Failed to send message"));
								}
								break;
							case 3:
								g_Modbus.AppDataAnalyse(g_byAppData, g_iAppDataNum, &g_iRegValue);
								TRACE("RegValue:%d", g_iRegValue);
								cstr1.Format(_T("%d"), g_iRegValue);
								MessageBox(cstr1);
								break;
							case 5:
								if (light == FALSE)
								{
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
									DataNum = g_iPhyDataNum;
									IfSuccess = WriteFile(handleCom, g_byPhyData, g_iPhyDataNum, &DataNum, NULL);
									if (!IfSuccess)
									{
										MessageBox(TEXT("Failed to send message"));
									}
								}
								else
								{
									g_byAppData[0] = 0x07;
									g_byAppData[1] = 0x00;
									g_byAppData[2] = 0x00;
									g_byAppData[3] = 0x00;
									g_byAppData[4] = 0x01;
									g_iAppDataNum = 5;

									UpdateData(true);
									g_byAppData[1] = (m_iRegAddr & 0xff00) >> 8;
									g_byAppData[2] = (m_iRegAddr & 0x00ff);

									g_Modbus.HDLC_AppToPhy(g_byPhyData, &g_iPhyDataNum, g_byAppData, g_iAppDataNum);
									DataNum = g_iPhyDataNum;
									IfSuccess = WriteFile(handleCom, g_byPhyData, g_iPhyDataNum, &DataNum, NULL);
									if (!IfSuccess)
									{
										MessageBox(TEXT("Failed to send message"));
									}
								}
								break;
							default:
								MessageBox(TEXT("An unexpected response"));
								break;
							}
						}
						else
						{
							MessageBox(TEXT("An unexpected response"));
						}
					}
				}
			}
		}
	}
}
