#if !defined(AFX_MODBUS_H__7B47CB13_9D10_40F0_9AC5_BB0ACFA7C691__INCLUDED_)
#define AFX_MODBUS_H__7B47CB13_9D10_40F0_9AC5_BB0ACFA7C691__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#define MODBUS_OK	0
#define MODBUS_ERR	-1

class CModbus
{
public:
	int HDLC_PhyToApp(const unsigned char* pucPhyData, int iPhyDataNum, unsigned char* pucAppData, int* piAppDataNum);
	int HDLC_AppToPhy(unsigned char* pucPhyData, int* piPhyDataNum, const unsigned char* pucAppData, int iAppDataNum);
	int AppDataAnalyse(unsigned char* pucAppData, int iAppDataNum, int* piRegDataValue);
	CModbus();
	virtual ~CModbus();

};

#endif
