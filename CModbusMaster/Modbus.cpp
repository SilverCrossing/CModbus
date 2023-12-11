#include "Modbus.h"
#include "crc.h"
#include "pch.h"

#define MODBUS_SLAVE_ADDR	1

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CModbus::CModbus()
{

}

CModbus::~CModbus()
{

}

int CModbus::HDLC_PhyToApp(const unsigned char* pucPhyData, int iPhyDataNum, unsigned char* pucAppData, int* piAppDataNum)
{

	WORD wCrcCode = CRC(pucPhyData, iPhyDataNum);
	if (0 != wCrcCode)
	{
		AfxMessageBox(TEXT("CRC wrong"));
		return MODBUS_ERR;
	}

	BYTE byAddr = *pucPhyData;
	if (MODBUS_SLAVE_ADDR != byAddr)
	{
		return MODBUS_ERR;
	}

	int i;
	*piAppDataNum = iPhyDataNum - 3;
	for (i = 0; i < (*piAppDataNum); i++)
	{
		*(pucAppData + i) = *(pucPhyData + 1 + i);
	}
	return MODBUS_OK;
}

int CModbus::HDLC_AppToPhy(unsigned char* pucPhyData, int* piPhyDataNum, const unsigned char* pucAppData, int iAppDataNum)
{
	int iPhyDataNum = 0;
	BYTE byAddr = *pucPhyData;
	*(pucPhyData + iPhyDataNum) = MODBUS_SLAVE_ADDR;
	iPhyDataNum++;

	int i;
	for (i = 0; i < iAppDataNum; i++)
	{
		*(pucPhyData + iPhyDataNum) = *pucAppData;
		iPhyDataNum++;
		pucAppData++;
	}


	WORD wCrcCode = CRC(pucPhyData, iPhyDataNum);

	*(pucPhyData + iPhyDataNum) = (wCrcCode & 0xff00) >> 8;
	iPhyDataNum++;
	*(pucPhyData + iPhyDataNum) = (wCrcCode & 0x00ff);
	iPhyDataNum++;

	*piPhyDataNum = iPhyDataNum;

	return MODBUS_OK;
}

int CModbus::AppDataAnalyse(unsigned char* pucAppData, int iAppDataNum, int* piRegDataValue)
{
	int ret;
	BYTE byFuncCode;
	BYTE byRegNum;
	WORD wRegValue;
	byFuncCode = *pucAppData;
	switch (byFuncCode)
	{
	case 0x01:
		return 1;
		break;
	case 0x03:
		byRegNum = (*(pucAppData + 1)) / 2;
		if (1 != byRegNum)
		{
			ret = MODBUS_ERR;
		}
		else
		{
			wRegValue = ((*(pucAppData + 2)) << 8) + (*(pucAppData + 3));
			(*piRegDataValue) = wRegValue;
			CString cstr1;
			cstr1.Format(TEXT("RegValue:%d"), wRegValue);
			AfxMessageBox(cstr1);
		}
		ret = 3;
		break;
	case 0x05:
		ret = 5;
		break;
	case 0x07:
		ret = 7;
		break;
	default:
		AfxMessageBox(TEXT("An unexpected response"));
		ret = MODBUS_ERR;
	}

	return  ret;
}

