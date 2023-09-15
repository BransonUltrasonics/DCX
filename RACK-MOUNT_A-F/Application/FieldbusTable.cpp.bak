/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/FieldbusTable.cpp_v   1.22.1.2   Mar 06 2017 16:13:04   EGUAJARDO  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2009        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/FieldbusTable.cpp_v  $
 * 
 *    Rev 1.22.1.2   Mar 06 2017 16:13:04   EGUAJARDO
 * Applied coding standar on commented old code line.
 * 
 *    Rev 1.22.1.1   Feb 08 2017 17:02:50   EGUAJARDO
 * Changed the size type of WeldTime to TYPE_32.
 * 
 *    Rev 1.22.1.0   07 May 2015 13:25:36   gbutron
 * Tenth of Joules feature incorporated
 * 
 *    Rev 1.23   03 Nov 2014 10:11:38   gbutron
 * It solves a problem with setting the energy to 1/10th value through the Ethernet/IP
 * 
 *    Rev 1.22   11 Aug 2014 04:58:44   rjamloki
 * Fixed Profibus param type for parameter 1505
 * 
 *    Rev 1.21   16 Apr 2014 10:40:18   amaurya
 * Added Break to case.
 * 
 *    Rev 1.20   24 Mar 2014 14:13:34   rjamloki
 * Setting Memory offset to zero on digital tune change.
 * 
 *    Rev 1.19   13 Mar 2014 15:54:20   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.18   04 Mar 2014 07:49:22   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
 * 
 *    Rev 1.17   08 Oct 2013 06:17:36   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.16   04 Sep 2013 13:11:42   rjamloki
 * Removed Test Offset
 * 
 *    Rev 1.15   24 Aug 2013 17:14:00   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.14   27 Jun 2013 05:54:16   amaurya
 * System crash fixes.
 * 
 *    Rev 1.13   14 Jun 2013 11:21:06   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.12   11 Jun 2013 09:48:32   amaurya
 * MH round I testing fixes.
 * 
 *    Rev 1.11   05 Jun 2013 03:08:36   amaurya
 * Fixed Passcode and Limits issue.
 * 
 *    Rev 1.10   03 Jun 2013 08:45:14   amaurya
 * Fixed Sort by Date/Time on History page and reset overload after horn disconnect.
 * 
 *    Rev 1.8   28 May 2013 07:42:48   amaurya
 * Fixed issues of Level II testing.
 * 
 *    Rev 1.7   22 May 2013 10:41:36   ygupta
 * Fixes for Disable Stepping in while in after burst, energy braking and scrub time.
 * 
 *    Rev 1.6   21 May 2013 12:25:50   amaurya
 * Code review fixes.
 * 
 *    Rev 1.5   06 May 2013 09:15:18   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.4   26 Apr 2013 11:02:24   amaurya
 * Fixed traker issue for Ver2.0U.
 * 
 *    Rev 1.3   22 Apr 2013 11:29:26   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.2   08 Apr 2013 08:22:34   amaurya
 * Fixed Tracker issue of Ver2.0P.
*/
#include "FieldbusTask.h"
#include "portable.h"
#include "preset.h"
#include "GLOBALS.h"
#include "ProfiBus.h"
#include "EthernetIP.h"
#include "Version.h"
#include "Alarms.h"
#include "cifXEndianess.h"
#include "FUNC.h"
#include "FieldbusTable.h"

//#define printf
//#define pprintf
static const bool EnableDebug = false;
//Fieldbus table is an array of objects of type FieldbusParameter. Every FieldbusParameter has an unique
//number(PNU) which is also entered in the table and is derived from PRAMETER_PBDCX Rev1.01.xls.
//Starting from the index 0 of global table array different parameter groups are entered. An individual group of
//parameters are entered consequently so that once the table array index of the first(base) parameter is
//known in group other table array indexes can be calculated based on PNU without parsing the table.
//below groups are there
//PNU 1 to 12 (Profubus test params)
//PNU 100 to 113(version info params)
//PNU 150 to 157(system info params)
//PNU 170 to 171(RTC Date/time)
//PNU 200 to 247(global errors)
//PNU 400 to 440(external control visualization. Related to analog IOs)
//PNU 800 to 944(global counters)
//PNU 1010 to 1105(weld parameter set)
//PNU 1210 to 1389(weld status set)
//PNU 1460 to 1504(horn parameter set)
//PNU 1610 to 1894(horn status set)

//Pre call back before parameter access
ParamAccessError ParamAccessPreCallback(UINT16 ParamNumber, void * Val, SINT32 Indx, UINT8 RW, UINT32 * Len = 0);
//Post call back after parameter access
ParamAccessError ParamAccessPostCallback(UINT16 ParamNumber, void * Val, SINT32 Indx, UINT8 RW , UINT32 * Len = 0);

SINT32 GetParameterIndexInTable(UINT16 ParamNumber);

//Fieldbus Test Values for read write parameter number 1 to 10
//Fieldbus Test Values for read
struct COMMTESTRD{
   SINT8 TestValue1Read;
   UINT8 TestValue2Read;
   SINT16 TestValue3Read;
   UINT16 TestValue4Read;
   SINT32 TestValue5Read;
   UINT32 TestValue6Read;
};

//Fieldbus Test Values for write
struct COMMTESTWR{
   SINT8 TestValue1Write;
   UINT8 TestValue2Write;
   SINT16 TestValue3Write;
   UINT16 TestValue4Write;
   SINT32 TestValue5Write;
   UINT32 TestValue6Write;
};
//Number of days in month. Array index represents month
static const UINT8 DaysinMonth[] =
{
  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
#define LEAP_YEAR(year) 	(!(year % 4))
static SINT32 DaysInMonth(UINT32  Month, UINT32 Year)
{
	return DaysinMonth[Month] + (LEAP_YEAR(Year) && (Month == 2));
}


//Test values read default
COMMTESTRD TestValuesRD = {-100, 200, -30000, 60000u, -2000000000, 4000000000u};
//Test values read/write default,min and max
COMMTESTWR TestValuesWR[3]={
							{-100, 200, -30000, 60000u, -2000000000, 4000000000u},
							{-120, 50, -32000, 5000, -2100000000, 200000000},
							{120, 250, 32000, 65000u, 2100000000, 4200000000u}
						   };
EXTCTRL ExternalControl;
UINT8 MinPsID = 0x20;//space
UINT8 MaxPsID = 0x7E;//~
//parameter extended access info. Part of FieldbusParameter structure ParamInfoExt member.
enum PARAMACCE
{
	RS = BIT(0),//standard read available
	RD = BIT(1),//default value read available
	RLL = BIT(2),//min value read available
	RLH = BIT(3),//max value read available
	WS = BIT(4),//standard value write available
	WSAT = BIT(5),//write available only with access token
	WNV = BIT(6),//non volatile write available
	//special handling params
	WPRCB = BIT(7),//call pre call back before access
	WPSCB = BIT(8),//call post call back aftr access
	RPRCB = BIT(9),//call pre call back for read
	RPSCB = BIT(10),//call post call back aftr read
	ACCPARASET = (RS | RD | RLL | RLH | WSAT),//preset parameters rights
};
//Base indexes in Fieldbus table for a group of parameters
//calculated once at power up
static UINT32 TestParamBaseTableIndex;
static UINT32 VersionBaseTableIndex;
static UINT32 SysInfoBaseTableIndex;
static UINT32 RTCBaseTableIndex;
static UINT32 ErrorInfoBaseTableIndex;
static UINT32 ExtctrlBaseTableIndex;
static UINT32 CntrInfoBaseTableIndex;
static UINT32 ParamBaseTableIndex;
static UINT32 WSBaseTableIndex;
static UINT32 HSParamBaseTableIndex;
static UINT32 HSSSParamBaseTableIndex;

//For some params writing zero means disabling the same. The range min/max should not
//be checked for those.This flags keeps track of this.
static BOOLEAN EnableLimitCheck;

//For some params if the feature is disabled the zero value should be sent on read access.
//This flags keeps track of this that the params should be read as usual from table or not.
static BOOLEAN ReadFromTable;


//RTC read/write related structures
//RTC Data and time are read/written as a single UINT32 variable where individual
//bytes represents the weekday,day, year, month for date and hour,second,minute and ten millisecond for time
union DcxDate
{
	UINT32 RTCDate;
	struct
	{
		UINT8 WeekDay;
		UINT8 Date;
		UINT8 Month;
		UINT8 Year;
	}Date;
}RTDate;
union DcxTime
{
	UINT32 RTCTime;
	struct
	{
		UINT8 TenMilliSecond;//not supported in DCX
		UINT8 Second;
		UINT8 Minute;
		UINT8 Hour;
	}Time;
}RTTime;

//Preset and status set objects can be recalled in RAM by special commands and can be edited or read
//by special index. The can be written(write valid for Weld and horn preset) as an whole by other special
//commands.
//These objects are named as Communication objects
DcxPreset CommPS;//accessed by PKW index 32. PNUs as usual.
HSPRESET  CommHSPS;//accessed by PKW index 16. PNUs as usual.
HSSTATUS CommHSSS[MAX_HS_FUNC];//accessed by PKW index 16. PNUs as usual.
WSTATUS CommWeldSS;//accessed by PKW index 32. PNUs as usual.

DcxPreset FbMaxPs , FbMinPs;
HSPRESET  FbMaxHsPs, FbMinHsPs;

extern BOOLEAN RegUpdateCmdFlag;
//FieldbusParameter structure ParamInfo member keeps PNU(bit 0 to 15), Actual data type(bit 16 to 23),
//(expected data type from profibus)(bit 24 to 31).
#define PARAMINFO(PbNum, Type, PBType) ((PbNum) | (Type << 16) | (PBType << 24))
//FieldbusParameter structure ParamInfoExt member.  Array the param belong to(see enum ArrayType)(bit 0 to 7),
//preset CRC if affected(see enum SAVECRC_CMD)(bit 8 to 15), and AccessRights (see enum PARAMACCE)(bit 16 to 31)
#define PARAMINFOEXT(Array ,CRC, AccessRights) ((Array) | ((CRC) << 8) | ((AccessRights) << 16))

//Fieldbus Parameter table
const FieldbusParameter PTable[] =
{
		{PARAMINFO(1, TYPE_S8, TYPE_8), &TestValuesRD.TestValue1Read, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(2, TYPE_8, TYPE_8), &TestValuesRD.TestValue2Read, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(3, TYPE_S16, TYPE_16), &TestValuesRD.TestValue3Read, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(4, TYPE_16, TYPE_16), &TestValuesRD.TestValue4Read, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(5, TYPE_S32, TYPE_32), &TestValuesRD.TestValue5Read, 0 , 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(6, TYPE_32, TYPE_32), &TestValuesRD.TestValue6Read,0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(7, TYPE_S8, TYPE_8), &TestValuesWR[0].TestValue1Write, &TestValuesWR[1].TestValue1Write,&TestValuesWR[2].TestValue1Write, 0, PARAMINFOEXT(ARRAYNA, 0, (RS|WS))},
		{PARAMINFO(8, TYPE_8, TYPE_8), &TestValuesWR[0].TestValue2Write, &TestValuesWR[1].TestValue2Write, &TestValuesWR[2].TestValue2Write, 0, PARAMINFOEXT(ARRAYNA, 0, (RS|WS))},
		{PARAMINFO(9, TYPE_S16, TYPE_16), &TestValuesWR[0].TestValue3Write, &TestValuesWR[1].TestValue3Write, &TestValuesWR[2].TestValue3Write, 0, PARAMINFOEXT(ARRAYNA, 0, (RS|WS))},
		{PARAMINFO(10, TYPE_16, TYPE_16), &TestValuesWR[0].TestValue4Write, &TestValuesWR[1].TestValue4Write, &TestValuesWR[2].TestValue4Write, 0, PARAMINFOEXT(ARRAYNA, 0, (RS|WS))},
		{PARAMINFO(11, TYPE_S32, TYPE_32), &TestValuesWR[0].TestValue5Write, &TestValuesWR[1].TestValue5Write, &TestValuesWR[2].TestValue5Write, 0, PARAMINFOEXT(ARRAYNA, 0, (RS|WS))},
		{PARAMINFO(12, TYPE_32, TYPE_32), &TestValuesWR[0].TestValue6Write, &TestValuesWR[1].TestValue6Write, &TestValuesWR[2].TestValue6Write, 0, PARAMINFOEXT(ARRAYNA, 0, (RS|WS))},

		// Version Information
		{PARAMINFO(100, TYPE_32, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},//DCP HW version
		{PARAMINFO(101, TYPE_32, TYPE_32), PowerUpData.FPGAVersionLabel, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS | RPRCB)},
		{PARAMINFO(102, TYPE_32, TYPE_32), (void*)(PowerUpData.BootLoaderVersionLabel + 2), 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS | RPRCB)},
		{PARAMINFO(103, TYPE_32, TYPE_32), (void*)(PowerUpData.SWVersionLabel + 3), 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS | RPRCB)},
		//dummy
		{PARAMINFO(104, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(105, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(106, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(107, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(108, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(109, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		{PARAMINFO(110, TYPE_32, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},//WC HW version
		//dummy
		{PARAMINFO(111, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(112, TYPE_32, TYPE_32), (void*)(WCBootLoaderVersionLabel + 2), 0,	0, 0, PARAMINFOEXT(ARRAYNA, 0, RS | RPRCB)},
		{PARAMINFO(113, TYPE_32, TYPE_32), (void*)(SWVersionLabel + 3), 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS | RPRCB)},
		// System Information
		{PARAMINFO(150, TYPE_32, TYPE_16), &MacchipData.Freq, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(151, TYPE_32, TYPE_16), &MacchipData.Power, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		//dummy
		{PARAMINFO(152, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(153, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(154, TYPE_A_8, TYPE_A_8), &MacchipData.SerialNo, 0, 0, 0, PARAMINFOEXT(ARRAYSN, 0, RS | RPRCB)},
		{PARAMINFO(155, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(156, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(157, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		// RealTime Clock
		{PARAMINFO(170, TYPE_32, TYPE_32), &RTDate.RTCDate, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS | RPRCB | WPSCB)},
		{PARAMINFO(171, TYPE_32, TYPE_32), &RTTime.RTCTime, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, (WSAT | RS | RPRCB | WPSCB))},
		// Error Information
		{PARAMINFO(200, TYPE_32, TYPE_32), &FBAlarms[0], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(201, TYPE_32, TYPE_32), &FBAlarms[1], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(202, TYPE_32, TYPE_32), &FBAlarms[2], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(203, TYPE_32, TYPE_32), &FBAlarms[3], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(204, TYPE_32, TYPE_32), &FBAlarms[4], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(205, TYPE_32, TYPE_32), &FBAlarms[5], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(206, TYPE_32, TYPE_32), &FBAlarms[6], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(207, TYPE_32, TYPE_32), &FBAlarms[7], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(208, TYPE_32, TYPE_32), &FBAlarms[8], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(209, TYPE_32, TYPE_32), &FBAlarms[9], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(210, TYPE_32, TYPE_32), &FBAlarms[10], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(211, TYPE_32, TYPE_32), &FBAlarms[11], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(212, TYPE_32, TYPE_32), &FBAlarms[12], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(213, TYPE_32, TYPE_32), &FBAlarms[13], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(214, TYPE_32, TYPE_32), &FBAlarms[14], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(215, TYPE_32, TYPE_32), &FBAlarms[15], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(216, TYPE_32, TYPE_32), &FBAlarms[16], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(217, TYPE_32, TYPE_32), &FBAlarms[17], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(218, TYPE_32, TYPE_32), &FBAlarms[18], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(219, TYPE_32, TYPE_32), &FBAlarms[19], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(220, TYPE_32, TYPE_32), &FBAlarms[20], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(221, TYPE_32, TYPE_32), &FBAlarms[21], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(222, TYPE_32, TYPE_32), &FBAlarms[22], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(223, TYPE_32, TYPE_32), &FBAlarms[23], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(224, TYPE_32, TYPE_32), &FBAlarms[24], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(225, TYPE_32, TYPE_32), &FBAlarms[25], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(226, TYPE_32, TYPE_32), &FBAlarms[26], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(227, TYPE_32, TYPE_32), &FBAlarms[27], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(228, TYPE_32, TYPE_32), &FBAlarms[28], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(229, TYPE_32, TYPE_32), &FBAlarms[29], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(230, TYPE_32, TYPE_32), &FBAlarms[30], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(231, TYPE_32, TYPE_32), &FBAlarms[31], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(232, TYPE_32, TYPE_32), &FBAlarms[32], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(233, TYPE_32, TYPE_32), &FBAlarms[33], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(234, TYPE_32, TYPE_32), &FBAlarms[34], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(235, TYPE_32, TYPE_32), &FBAlarms[35], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(236, TYPE_32, TYPE_32), &FBAlarms[36], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(237, TYPE_32, TYPE_32), &FBAlarms[37], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(238, TYPE_32, TYPE_32), &FBAlarms[38], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(239, TYPE_32, TYPE_32), &FBAlarms[39], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(240, TYPE_32, TYPE_32), &FBAlarms[40], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(241, TYPE_32, TYPE_32), &FBAlarms[41], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(242, TYPE_32, TYPE_32), &FBAlarms[42], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(243, TYPE_32, TYPE_32), &FBAlarms[43], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(244, TYPE_32, TYPE_32), &FBAlarms[44], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(245, TYPE_32, TYPE_32), &FBAlarms[45], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(246, TYPE_32, TYPE_32), &FBAlarms[46], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(247, TYPE_32, TYPE_32), &FBAlarms[47], 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},

		// External Control - Visualization @ Communication Channel
		{PARAMINFO(400, TYPE_S16, TYPE_16), &ExternalControl.AmplitudeSet, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		//dummy
		{PARAMINFO(401, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(402, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(403, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(404, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(405, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(406, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(407, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(408, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(409, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(410, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(411, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(412, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(413, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(414, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(415, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(416, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(417, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(418, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(419, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		{PARAMINFO(420, TYPE_S16, TYPE_16), &ExternalControl.Amplitude, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(421, TYPE_S16, TYPE_16), &ExternalControl.Current,0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(422, TYPE_S16, TYPE_16), &ExternalControl.Power, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(423, TYPE_S16, TYPE_16), &ExternalControl.Phase, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		//dummy
		{PARAMINFO(424, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(425, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		{PARAMINFO(426, TYPE_S16, TYPE_16), &ExternalControl.PSV, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(427, TYPE_16, TYPE_16), &ExternalControl.Frequency, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(428, TYPE_S16, TYPE_16), &ExternalControl.Temperature, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		//dummy
		{PARAMINFO(429, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(430, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(431, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(432, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(433, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(434, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(435, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(436, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(437, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(438, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(439, TYPE_S16, TYPE_16), &ExternalControl.ExtFrequencyOffset, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, (RS | WSAT | WPRCB))},
		//TODO:Min max
		{PARAMINFO(440, TYPE_S16, TYPE_16), &ExternalControl.ExtAmplitudeSet, 0,0, 0, PARAMINFOEXT(ARRAYNA, 0, (RS | WSAT | WPRCB))},

		// Counter
		{PARAMINFO(860, TYPE_32, TYPE_32), &MFcycleResults->CurrCycleTime, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, (RS | RPRCB))},
		{PARAMINFO(861, TYPE_32, TYPE_32), &MFcycleResults->SonicsOnTime, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, (RS | RPRCB))},
		//dummy
		{PARAMINFO(862, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(863, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(864, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(865, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(866, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(867, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(868, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(869, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(870, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(871, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(872, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(873, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(874, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(875, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		{PARAMINFO(876, TYPE_32, TYPE_32), &MFcycleResults->CyclesCount, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(877, TYPE_32, TYPE_32), &MFcycleResults->AlarmsCount, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		//dummy
		{PARAMINFO(878, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(879, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(880, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(881, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(882, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(883, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(884, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(885, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(886, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(887, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(888, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(889, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(890, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(891, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(892, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(893, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(894, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(895, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(896, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(897, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(898, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(899, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		{PARAMINFO(900, TYPE_32, TYPE_32), &SysCounters->LifeTimeCycleCount, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		{PARAMINFO(901, TYPE_32, TYPE_32), &SysCounters->GeneralAlarmCount, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, RS)},
		//dummy
		{PARAMINFO(902, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(903, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(904, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(905, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(906, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(907, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(908, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(909, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(910, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(911, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(912, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(913, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(914, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(915, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(916, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(917, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(918, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(919, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(920, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(921, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(922, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(923, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(924, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(925, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(926, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(927, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(928, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(929, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(930, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(931, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(932, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(933, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(934, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(935, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(936, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(937, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(938, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(939, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(940, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(941, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(942, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(943, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(944, TYPE_A_32, TYPE_A_32), &ParameterSet[CurrPreset].InfoParam.WeldCount, 0, 0, 0, PARAMINFOEXT(ARRAYPS, 0, RS)},

		//Start Weld Parameter/
		{PARAMINFO(1010, TYPE_A_8, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.PresetID[0],
		&MinPsID, &MaxPsID, &ParameterSet[Preset_Default].InfoParam.PresetID[0], PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1011, TYPE_A_8, TYPE_A_8), (UINT8*)&ParameterSet[CurrPreset].InfoParam.PresetID[0] + 1,
		&MinPsID, &MaxPsID,	(UINT8*)&ParameterSet[Preset_Default].InfoParam.PresetID[0] + 1, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1012, TYPE_A_8, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.PresetID[1],
		&MinPsID, &MaxPsID,	&ParameterSet[Preset_Default].InfoParam.PresetID[1], PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1013, TYPE_A_8, TYPE_A_8), (UINT8*)&ParameterSet[CurrPreset].InfoParam.PresetID[1] + 1,
		&MinPsID, &MaxPsID,	(UINT8*)&ParameterSet[Preset_Default].InfoParam.PresetID[1] + 1, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1014, TYPE_A_8, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.PresetID[2],
		&MinPsID, &MaxPsID, &ParameterSet[Preset_Default].InfoParam.PresetID[2], PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1015, TYPE_A_8, TYPE_A_8), (UINT8*)&ParameterSet[CurrPreset].InfoParam.PresetID[2] + 1,
		&MinPsID , &MaxPsID,(UINT8*)&ParameterSet[Preset_Default].InfoParam.PresetID[2] + 1, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1016, TYPE_A_8, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.PresetID[3],
		&MinPsID , &MaxPsID, &ParameterSet[Preset_Default].InfoParam.PresetID[3], PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1017, TYPE_A_8, TYPE_A_8), (UINT8*)&ParameterSet[CurrPreset].InfoParam.PresetID[3] + 1,
		&MinPsID, &MaxPsID,	(UINT8*)&ParameterSet[Preset_Default].InfoParam.PresetID[3] + 1, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1018, TYPE_A_8, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.PresetID[4],
		&MinPsID, &MaxPsID, &ParameterSet[Preset_Default].InfoParam.PresetID[4], PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1019, TYPE_A_8, TYPE_A_8), (UINT8*)&ParameterSet[CurrPreset].InfoParam.PresetID[4] + 1,
		&MinPsID, &MaxPsID ,(UINT8*)&ParameterSet[Preset_Default].InfoParam.PresetID[4] + 1, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1020, TYPE_A_8, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.PresetID[5],
		&MinPsID , &MaxPsID, &ParameterSet[Preset_Default].InfoParam.PresetID[5], PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1021, TYPE_A_8, TYPE_A_8), (UINT8*)&ParameterSet[CurrPreset].InfoParam.PresetID[5] + 1,
		&MinPsID, &MaxPsID, (UINT8*)&ParameterSet[Preset_Default].InfoParam.PresetID[5] + 1, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1022, TYPE_A_8, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.PresetID[6],
		&MinPsID, &MaxPsID, &ParameterSet[Preset_Default].InfoParam.PresetID[6], PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1023, TYPE_A_8, TYPE_A_8), (UINT8*)&ParameterSet[CurrPreset].InfoParam.PresetID[6] + 1,
		&MinPsID, &MaxPsID, (UINT8*)&ParameterSet[Preset_Default].InfoParam.PresetID[6] + 1, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1024, TYPE_A_8, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.PresetID[7],
		&MinPsID, &MaxPsID,	&ParameterSet[Preset_Default].InfoParam.PresetID[7], PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1025, TYPE_A_8, TYPE_A_8), (UINT8*)&ParameterSet[CurrPreset].InfoParam.PresetID[7] + 1,
		&MinPsID, &MaxPsID,	(UINT8*)&ParameterSet[Preset_Default].InfoParam.PresetID[7] + 1, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1026, TYPE_A_8, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.PresetID[8],
		&MinPsID, &MaxPsID,	&ParameterSet[Preset_Default].InfoParam.PresetID[8], PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1027, TYPE_A_8, TYPE_A_8), (UINT8*)&ParameterSet[CurrPreset].InfoParam.PresetID[8] + 1,
		&MinPsID, &MaxPsID,	(UINT8*)&ParameterSet[Preset_Default].InfoParam.PresetID[8] + 1, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1028, TYPE_A_8, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.PresetID[9],
		&MinPsID, &MaxPsID,	&ParameterSet[Preset_Default].InfoParam.PresetID[9], PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1029, TYPE_A_8, TYPE_A_8), (UINT8*)&ParameterSet[CurrPreset].InfoParam.PresetID[9] + 1,
		&MinPsID, &MaxPsID,	(UINT8*)&ParameterSet[Preset_Default].InfoParam.PresetID[9] + 1, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		//dummy
		{PARAMINFO(1030, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1031, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1032, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1033, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1034, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1035, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1036, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1037, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1038, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1039, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1040, TYPE_16, TYPE_A_8), &ParameterSet[CurrPreset].InfoParam.HornPresetNumber,
		&FbMinPs.InfoParam.HornPresetNumber, &FbMaxPs.InfoParam.HornPresetNumber,
		&ParameterSet[Preset_Default].InfoParam.HornPresetNumber, PARAMINFOEXT(ARRAYPS, SAVECRC_INFO, ACCPARASET)},
		{PARAMINFO(1041, TYPE_32, TYPE_A_8), &ParameterSet[CurrPreset].WcParam.WeldMode,
		&FbMinPs.WcParam.WeldMode, &FbMaxPs.WcParam.WeldMode,
		&ParameterSet[Preset_Default].WcParam.WeldMode, PARAMINFOEXT(ARRAYPS, SAVECRC_WC , ACCPARASET)},
		//dummy
		{PARAMINFO(1042, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1043, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1044, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1045, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1046, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1047, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1048, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1049, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1050, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1051, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1052, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1053, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1054, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1055, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1056, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1057, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1058, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1059, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1060, TYPE_32, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.WeldMode,
		&FbMinPs.WcParam.WeldMode, &FbMaxPs.WcParam.WeldMode,
		&ParameterSet[Preset_Default].WcParam.WeldMode, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1061, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.WeldTimeCP,
		&FbMinPs.WcParam.WeldTimeCP, &FbMaxPs.WcParam.WeldTimeCP,
		&ParameterSet[Preset_Default].WcParam.WeldTimeCP, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1062, TYPE_32, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.EnergyCP,
		&FbMinPs.WcParam.EnergyCP, &FbMaxPs.WcParam.EnergyCP,
		&ParameterSet[Preset_Default].WcParam.EnergyCP, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET | WPRCB)},
		{PARAMINFO(1063, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.PeakPowerCP,
		&FbMinPs.WcParam.PeakPowerCP, &FbMaxPs.WcParam.PeakPowerCP,
		&ParameterSet[Preset_Default].WcParam.PeakPowerCP, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET | RPRCB | WPRCB)},
		{PARAMINFO(1064, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.ScrubTime,
		&FbMinPs.WcParam.ScrubTime, &FbMaxPs.WcParam.ScrubTime,
		&ParameterSet[Preset_Default].WcParam.ScrubTime, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET /*| RPRCB | WPRCB | WPSCB*/)},
		{PARAMINFO(1065, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.Amplitude1,
		&FbMinPs.WcParam.Amplitude1, &FbMaxPs.WcParam.Amplitude1,
		&ParameterSet[Preset_Default].WcParam.Amplitude1, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1066, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.Amplitude2,
		&FbMinPs.WcParam.Amplitude2, &FbMaxPs.WcParam.Amplitude2,
		&ParameterSet[Preset_Default].WcParam.Amplitude2, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1067, TYPE_32, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.AmpStepCondition,
		&FbMinPs.WcParam.AmpStepCondition, &FbMaxPs.WcParam.AmpStepCondition,
		&ParameterSet[Preset_Default].WcParam.AmpStepCondition, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1068, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.AmpStepTime,
		&FbMinPs.WcParam.AmpStepTime, &FbMaxPs.WcParam.AmpStepTime,
		&ParameterSet[Preset_Default].WcParam.AmpStepTime, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1069, TYPE_32, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.AmpStepEnergy,
		&FbMinPs.WcParam.AmpStepEnergy, &FbMaxPs.WcParam.AmpStepEnergy,
		&ParameterSet[Preset_Default].WcParam.AmpStepEnergy, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET | WPRCB )},
		{PARAMINFO(1070, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.AmpStepPower,
		&FbMinPs.WcParam.AmpStepPower, &FbMaxPs.WcParam.AmpStepPower,
		&ParameterSet[Preset_Default].WcParam.AmpStepPower, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1071, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].DcpParam.WeldRampTime,
		&FbMinPs.DcpParam.WeldRampTime, &FbMaxPs.DcpParam.WeldRampTime,
		&ParameterSet[Preset_Default].DcpParam.WeldRampTime, PARAMINFOEXT(ARRAYPS, SAVECRC_DCP, ACCPARASET)},
		{PARAMINFO(0, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].DcpParam.WeldProfileRampTime,
		&FbMinPs.DcpParam.WeldProfileRampTime, &FbMaxPs.DcpParam.WeldProfileRampTime,
		&ParameterSet[Preset_Default].DcpParam.WeldProfileRampTime, PARAMINFOEXT(ARRAYPS, SAVECRC_DCP, ACCPARASET)},
		{PARAMINFO(1073, TYPE_8, TYPE_A_32), &ParameterSet[CurrPreset].DcpParam.MemoryStoreFlag,
		&FbMinPs.DcpParam.MemoryStoreFlag, &FbMaxPs.DcpParam.MemoryStoreFlag,
		&ParameterSet[Preset_Default].DcpParam.MemoryStoreFlag, PARAMINFOEXT(ARRAYPS, SAVECRC_DCP, ACCPARASET)},
		{PARAMINFO(1074, TYPE_S32, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.FreqOffset,
		&FbMinPs.WcParam.FreqOffset, &FbMaxPs.WcParam.FreqOffset,
		&ParameterSet[Preset_Default].WcParam.FreqOffset, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET | RPRCB | WPRCB)},
		{PARAMINFO(1075, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.HoldTime,
		&FbMinPs.WcParam.HoldTime, &FbMaxPs.WcParam.HoldTime,
		&ParameterSet[Preset_Default].WcParam.HoldTime, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET /*| RPRCB | WPRCB | WPSCB*/)},
		{PARAMINFO(1076, TYPE_8, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.EBFlag,
		&FbMinPs.WcParam.EBFlag, &FbMaxPs.WcParam.EBFlag,
		&ParameterSet[Preset_Default].WcParam.EBFlag, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1077, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.EBAmp,
		&FbMinPs.WcParam.EBAmp, &FbMaxPs.WcParam.EBAmp,
		&ParameterSet[Preset_Default].WcParam.EBAmp, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1078, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.EBTime,
		&FbMinPs.WcParam.EBTime, &FbMaxPs.WcParam.EBTime,
		&ParameterSet[Preset_Default].WcParam.EBTime, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1079, TYPE_8, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.ABFlag,
		&FbMinPs.WcParam.ABFlag, &FbMaxPs.WcParam.ABFlag,
		&ParameterSet[Preset_Default].WcParam.ABFlag, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1080, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.ABAmp,
		&FbMinPs.WcParam.ABAmp, &FbMaxPs.WcParam.ABAmp,
		&ParameterSet[Preset_Default].WcParam.ABAmp, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1081, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.ABTime,
		&FbMinPs.WcParam.ABTime, &FbMaxPs.WcParam.ABTime,
		&ParameterSet[Preset_Default].WcParam.ABTime, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(1082, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.ABDelay,
		&FbMinPs.WcParam.ABDelay, &FbMaxPs.WcParam.ABDelay,
		&ParameterSet[Preset_Default].WcParam.ABDelay, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},//dummy 1083
		{PARAMINFO(1084, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.ScrubAmp,
		&FbMinPs.WcParam.ScrubAmp, &FbMaxPs.WcParam.ScrubAmp,
		&ParameterSet[Preset_Default].WcParam.ScrubAmp, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},////dummy 1085
		{PARAMINFO(1086, TYPE_32, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.TimeCutOff,
		&FbMinPs.WcParam.TimeCutOff, &FbMaxPs.WcParam.TimeCutOff,
		&ParameterSet[Preset_Default].WcParam.TimeCutOff, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1087, TYPE_32, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.EnergyCutoff,
		&FbMinPs.WcParam.EnergyCutoff, &FbMaxPs.WcParam.EnergyCutoff,
		&ParameterSet[Preset_Default].WcParam.EnergyCutoff, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1088, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.PeakPowerCutoff,
		&FbMinPs.WcParam.PeakPowerCutoff, &FbMaxPs.WcParam.PeakPowerCutoff,
		&ParameterSet[Preset_Default].WcParam.PeakPowerCutoff, PARAMINFOEXT(ARRAYPS, SAVECRC_WC, ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1089, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].LimitsParam.WeldTimeMLR,
		&FbMinPs.LimitsParam.WeldTimeMLR, &FbMaxPs.LimitsParam.WeldTimeMLR,
		&ParameterSet[Preset_Default].LimitsParam.WeldTimeMLR, PARAMINFOEXT(ARRAYPS, SAVECRC_LIMITS, ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1090, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].LimitsParam.WeldTimePLR,
		&FbMinPs.LimitsParam.WeldTimePLR, &FbMaxPs.LimitsParam.WeldTimePLR,
		&ParameterSet[Preset_Default].LimitsParam.WeldTimePLR, PARAMINFOEXT(ARRAYPS, SAVECRC_LIMITS, ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1091, TYPE_32, TYPE_A_32), &ParameterSet[CurrPreset].LimitsParam.EnergyMLR,
		&FbMinPs.LimitsParam.EnergyMLR, &FbMaxPs.LimitsParam.EnergyMLR,
		&ParameterSet[Preset_Default].LimitsParam.EnergyMLR, PARAMINFOEXT(ARRAYPS, SAVECRC_LIMITS, ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1092, TYPE_32, TYPE_A_32), &ParameterSet[CurrPreset].LimitsParam.EnergyPLR,
		&FbMinPs.LimitsParam.EnergyPLR, &FbMaxPs.LimitsParam.EnergyPLR,
		&ParameterSet[Preset_Default].LimitsParam.EnergyPLR, PARAMINFOEXT(ARRAYPS, SAVECRC_LIMITS, ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1093, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].LimitsParam.PowerMLR,
		&FbMinPs.LimitsParam.PowerMLR, &FbMaxPs.LimitsParam.PowerMLR,
		&ParameterSet[Preset_Default].LimitsParam.PowerMLR, PARAMINFOEXT(ARRAYPS, SAVECRC_LIMITS, ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1094, TYPE_16, TYPE_A_32), &ParameterSet[CurrPreset].LimitsParam.PowerPLR,
		&FbMinPs.LimitsParam.PowerPLR, &FbMaxPs.LimitsParam.PowerPLR,
		&ParameterSet[Preset_Default].LimitsParam.PowerPLR, PARAMINFOEXT(ARRAYPS, SAVECRC_LIMITS, ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1095, TYPE_A_S32, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.WeldFreqLowCutoff,
		&FbMinPs.WcParam.WeldFreqLowCutoff, &FbMaxPs.WcParam.WeldFreqLowCutoff,
		&ParameterSet[Preset_Default].WcParam.WeldFreqLowCutoff, PARAMINFOEXT(ARRAYPS, (SAVECRC_DCP | SAVECRC_WC), ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1096, TYPE_A_S32, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.WeldFreqHighCutoff,
		&FbMinPs.WcParam.WeldFreqHighCutoff, &FbMaxPs.WcParam.WeldFreqHighCutoff,
		&ParameterSet[Preset_Default].WcParam.WeldFreqHighCutoff, PARAMINFOEXT(ARRAYPS, (SAVECRC_DCP | SAVECRC_WC), ACCPARASET| RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1097, TYPE_A_8, TYPE_A_32), &ParameterSet[CurrPreset].WcParam.GrdDetectFlagCP,
		&FbMinPs.WcParam.GrdDetectFlagCP, &FbMaxPs.WcParam.GrdDetectFlagCP,
		&ParameterSet[Preset_Default].WcParam.GrdDetectFlagCP, PARAMINFOEXT(0, SAVECRC_WC, ACCPARASET)},//TODO:Ground detect error??
		{PARAMINFO(1098, TYPE_S32, TYPE_A_32), &ParameterSet[CurrPreset].ActParam.CustomAIn1Cutoff,
		&FbMinPs.ActParam.CustomAIn1Cutoff, &FbMaxPs.ActParam.CustomAIn1Cutoff,
		&ParameterSet[Preset_Default].ActParam.CustomAIn1Cutoff, PARAMINFOEXT(ARRAYPS, SAVECRC_ACT, ACCPARASET | RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1099, TYPE_S32, TYPE_A_32), &ParameterSet[CurrPreset].ActParam.CustomAIn2Cutoff,
		&FbMinPs.ActParam.CustomAIn2Cutoff, &FbMaxPs.ActParam.CustomAIn2Cutoff,
		&ParameterSet[Preset_Default].ActParam.CustomAIn2Cutoff, PARAMINFOEXT(ARRAYPS, SAVECRC_ACT, ACCPARASET | RPRCB | WPRCB | WPSCB)},
		{PARAMINFO(1100, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1101, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1102, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1103, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1104, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1105, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//*************** End Weld Parameter ***************/
		//start Status parameter
		{PARAMINFO(1210, TYPE_A_8, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[0],
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1211, TYPE_A_8, TYPE_A_8), (UINT8*)&WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[0] + 1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1212, TYPE_A_8, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[1],
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1213, TYPE_A_8, TYPE_A_8), (UINT8*)&WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[1] + 1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1214, TYPE_A_8, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[2],
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1215, TYPE_A_8, TYPE_A_8), (UINT8*)&WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[2] + 1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1216, TYPE_A_8, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[3],
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1217, TYPE_A_8, TYPE_A_8), (UINT8*)&WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[3] + 1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1218, TYPE_A_8, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[4],
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1219, TYPE_A_8, TYPE_A_8), (UINT8*)&WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[4] + 1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1220, TYPE_A_8, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[5],
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1221, TYPE_A_8, TYPE_A_8), (UINT8*)&WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[5] + 1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1222, TYPE_A_8, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[6],
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1223, TYPE_A_8, TYPE_A_8), (UINT8*)&WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[6] + 1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1224, TYPE_A_8, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[7],
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1225, TYPE_A_8, TYPE_A_8), (UINT8*)&WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[7] + 1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1226, TYPE_A_8, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[8],
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1227, TYPE_A_8, TYPE_A_8), (UINT8*)&WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[8] + 1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1228, TYPE_A_8, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[9],
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1229, TYPE_A_8, TYPE_A_8), (UINT8*)&WeldStatusSet[CurrPreset].DcxPs.InfoParam.PresetID[9] + 1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		//dummy
		{PARAMINFO(1230, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1231, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1232, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1233, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1234, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1235, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1236, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1237, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1238, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1239, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1240, TYPE_A_16, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.InfoParam.HornPresetNumber,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1241, TYPE_A_32, TYPE_A_8), &WeldStatusSet[CurrPreset].DcxPs.WcParam.WeldMode,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(FB_PARAM_PEAKPOWERRAW, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.PeakPower,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS | RPRCB)},
		//dummy
		{PARAMINFO(1243, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1244, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1245, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1246, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1247, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1248, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1249, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1250, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1251, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1252, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1253, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1254, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1255, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1256, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1257, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1258, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1259, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		{PARAMINFO(1260, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.WeldMode,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1261, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.WeldTimeCP,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1262, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.EnergyCP,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1263, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.PeakPowerCP,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1264, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.ScrubTime,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1265, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.Amplitude1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1266, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.Amplitude2,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1267, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.AmpStepCondition,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1268, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.AmpStepTime,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1269, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.AmpStepEnergy,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1270, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.AmpStepPower,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1271, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.DcpParam.WeldRampTime,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(0, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.DcpParam.WeldProfileRampTime,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1273, TYPE_8, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.DcpParam.MemoryStoreFlag,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1274, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.FreqOffset,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1275, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.HoldTime,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1276, TYPE_8, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.EBFlag,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1277, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.EBAmp,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1278, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.EBTime,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1279, TYPE_8, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.ABFlag,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1280, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.ABAmp,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1281, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.ABTime,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1282, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.ABDelay,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1283, TYPE_8, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.ScrubTimeFlag,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1284, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.ScrubAmp,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1285, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.ScrubTime,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1286, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.TimeCutOff,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1287, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.EnergyCutoff,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1288, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.PeakPowerCutoff,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1289, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.LimitsParam.WeldTimeMLR,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1290, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.LimitsParam.WeldTimePLR,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1291, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.LimitsParam.EnergyMLR,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1292, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.LimitsParam.EnergyPLR,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1293, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.LimitsParam.PowerMLR,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1294, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.LimitsParam.PowerPLR,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1295, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.WeldFreqLowCutoff,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1296, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.WeldFreqHighCutoff,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1297, TYPE_8, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.WcParam.GrdDetectFlagCP,
		0, 0, 0, PARAMINFOEXT(0, 0, RS)},//TODO:Ground detect error??
		{PARAMINFO(1298, TYPE_S32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.ActParam.CustomAIn1Cutoff, 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1299, TYPE_S32, TYPE_A_32), &WeldStatusSet[CurrPreset].DcxPs.ActParam.CustomAIn2Cutoff, 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1300, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1301, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1302, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1303, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1304, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1305, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		// WELD DATA

		{PARAMINFO(1306, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.RTCDate.Date, 0, 0,
		0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1307, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.RTCTime.Time, 0, 0,
		0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1308, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.CycleCounter, 0, 0,
		0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		// Error Information
		{PARAMINFO(1309, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[0], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1310, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[1], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1311, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[2], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1312, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[3], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1313, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[4], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1314, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[5], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1315, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[6], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1316, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[7], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1317, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[8], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1318, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[9], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1319, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[10], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1320, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[11], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1321, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[12], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1322, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[13], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1323, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[14], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1324, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[15], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1325, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[16], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1326, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[17], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1327, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[18], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1328, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[19], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1329, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[20], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1330, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[21], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1331, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[22], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1332, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[23], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1333, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[24], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1334, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[25], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1335, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[26], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1336, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[27], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1337, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[28], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1338, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[29], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1339, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[30], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1340, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[31], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1341, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[32], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1342, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[33], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1343, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[34], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1344, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[35], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1345, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[36], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1346, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[37], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1347, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[38], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1348, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[39], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1349, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[40], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1350, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[41], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1351, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[42], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1352, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[43], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1353, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[44], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1354, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[45], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1355, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[46], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1356, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.FBAlarms[47], 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1357, TYPE_A_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.ErrorCodeFB, 0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},

		{PARAMINFO(1358, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1359, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//status values
		//{PARAMINFO(1360, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.WeldTime,				//16 bits did not allow to read the weld time
		{PARAMINFO(1360, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.WeldTime,				//for more than one minute
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1361, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.HoldTime,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1362, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.WeldEnergy,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1363, TYPE_8, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.PeakPower,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1364, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.AvgPower,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1365, TYPE_8, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.AvgAmplitude1,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1366, TYPE_8, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.AvgAmplitude2,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1367, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.RecalledResonanceFrequencyAbs,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1368, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.WeldStartFrequency,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1369, TYPE_16, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.WeldStopFrequency,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1370, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.StoredFrequencyAbs,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1371, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.ResFreqOK,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1372, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.EndAmplitudeSet,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1373, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.EndAmplitude,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1374, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.ErrorPsv,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1375, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.EndPower,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1376, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.EndCurrent,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1377, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.EndPhase,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1378, TYPE_32, TYPE_A_32), &WeldStatusSet[CurrPreset].Wstat.EndTemperature,
		0, 0, 0, PARAMINFOEXT(ARRAYSS, 0, RS)},
		{PARAMINFO(1379, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1380, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1381, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1382, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1383, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1384, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1385, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1386, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1387, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1388, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1389, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//End Weld Status Parameters

		//Stack parameters
		//dummy
		//******** Stack Parameter - Function 0(Seek) *******/
		{PARAMINFO(1460, TYPE_16, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaSeek.SeekTime,
		&FbMinHsPs.ParaSeek.SeekTime, &FbMaxHsPs.ParaSeek.SeekTime,
		&HsParameterSet[HsPreset_Default].ParaSeek.SeekTime, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1461, TYPE_16, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaSeek.SeekAmp,
		&FbMinHsPs.ParaSeek.SeekAmp, &FbMaxHsPs.ParaSeek.SeekAmp,
		&HsParameterSet[HsPreset_Default].ParaSeek.SeekAmp, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1462, TYPE_16, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaSeek.SeekRampTime,
		&FbMinHsPs.ParaSeek.SeekRampTime, &FbMaxHsPs.ParaSeek.SeekRampTime,
		&HsParameterSet[HsPreset_Default].ParaSeek.SeekRampTime, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaSeek.SeekAmpLoopC1,
		&FbMinHsPs.ParaSeek.SeekAmpLoopC1, &FbMaxHsPs.ParaSeek.SeekAmpLoopC1,
		&HsParameterSet[HsPreset_Default].ParaSeek.SeekAmpLoopC2, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaSeek.SeekAmpLoopC2,
		&FbMinHsPs.ParaSeek.SeekAmpLoopC2, &FbMaxHsPs.ParaSeek.SeekAmpLoopC2,
		&HsParameterSet[HsPreset_Default].ParaSeek.SeekAmpLoopC2, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1465, TYPE_S32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaSeek.FreqOffset,
		&FbMinHsPs.ParaSeek.FreqOffset, &FbMaxHsPs.ParaSeek.FreqOffset,
		&HsParameterSet[HsPreset_Default].ParaSeek.FreqOffset, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET | RPRCB | WPRCB)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaSeek.SeekPhaseLoopI,
		&FbMinHsPs.ParaSeek.SeekPhaseLoopI, &FbMaxHsPs.ParaSeek.SeekPhaseLoopI,
		&HsParameterSet[HsPreset_Default].ParaSeek.SeekPhaseLoopI, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1467, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaSeek.SeekFreqLow,
		&FbMinHsPs.ParaSeek.SeekFreqLow, &FbMaxHsPs.ParaSeek.SeekFreqLow,
		&HsParameterSet[HsPreset_Default].ParaSeek.SeekFreqLow, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET | RPRCB | WPRCB)},
		{PARAMINFO(1468, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaSeek.SeekFreqHigh,
		&FbMinHsPs.ParaSeek.SeekFreqHigh, &FbMaxHsPs.ParaSeek.SeekFreqHigh,
		&HsParameterSet[HsPreset_Default].ParaSeek.SeekFreqHigh, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET | RPRCB | WPRCB)},
		{PARAMINFO(1469, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1470, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1471, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1472, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1473, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1474, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//********Stack Parameter - Function 1(Test)*******/
		{PARAMINFO(1475, TYPE_16, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.Amplitude1,
		&FbMinHsPs.ParaTest.Amplitude1, &FbMaxHsPs.ParaTest.Amplitude1,
		&HsParameterSet[HsPreset_Default].ParaTest.Amplitude1, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1476, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.AmpStepCondition,
		&FbMinHsPs.ParaTest.AmpStepCondition, &FbMaxHsPs.ParaTest.AmpStepCondition,
		&HsParameterSet[HsPreset_Default].ParaTest.AmpStepCondition, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1477, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.AmplitudeProfileTime,
		&FbMinHsPs.ParaTest.AmplitudeProfileTime, &FbMaxHsPs.ParaTest.AmplitudeProfileTime,
		&HsParameterSet[HsPreset_Default].ParaTest.AmplitudeProfileTime, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1478, TYPE_16, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.Amplitude2,
		&FbMinHsPs.ParaTest.Amplitude2, &FbMaxHsPs.ParaTest.Amplitude2,
		&HsParameterSet[HsPreset_Default].ParaTest.Amplitude2, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1479, TYPE_16, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.AmplitudeStartRampTime,
		&FbMinHsPs.ParaTest.AmplitudeStartRampTime, &FbMaxHsPs.ParaTest.AmplitudeStartRampTime,
		&HsParameterSet[HsPreset_Default].ParaTest.AmplitudeStartRampTime, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(0, TYPE_16, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.AmplitudeProfileRampTime,
		&FbMinHsPs.ParaTest.AmplitudeProfileRampTime, &FbMaxHsPs.ParaTest.AmplitudeProfileRampTime,
		&HsParameterSet[HsPreset_Default].ParaTest.AmplitudeProfileRampTime, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.AmplitudeLoopC1,
		&FbMinHsPs.ParaTest.AmplitudeLoopC1, &FbMaxHsPs.ParaTest.AmplitudeLoopC1,
		&HsParameterSet[HsPreset_Default].ParaTest.AmplitudeLoopC1, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.AmplitudeLoopC2,
		&FbMinHsPs.ParaTest.AmplitudeLoopC2, &FbMaxHsPs.ParaTest.AmplitudeLoopC2,
		&HsParameterSet[HsPreset_Default].ParaTest.AmplitudeLoopC2, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(0, TYPE_S32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.FreqOffset,
		&FbMinHsPs.ParaTest.FreqOffset, &FbMaxHsPs.ParaTest.FreqOffset,
		&HsParameterSet[HsPreset_Default].ParaTest.FreqOffset, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET | RPRCB | WPRCB)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.PhaseLoopI,
		&FbMinHsPs.ParaTest.PhaseLoopI, &FbMaxHsPs.ParaTest.PhaseLoopI,
		&HsParameterSet[HsPreset_Default].ParaTest.PhaseLoopI, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1485, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.TimeErrorHigh,
		&FbMinHsPs.ParaTest.TimeErrorHigh, &FbMaxHsPs.ParaTest.TimeErrorHigh,
		&HsParameterSet[HsPreset_Default].ParaTest.TimeErrorHigh, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1486, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.WeldFreqLow,
		&FbMinHsPs.ParaTest.WeldFreqLow, &FbMaxHsPs.ParaTest.WeldFreqLow,
		&HsParameterSet[HsPreset_Default].ParaTest.WeldFreqLow, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET | RPRCB | WPRCB)},
		{PARAMINFO(1487, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaTest.WeldFreqHigh,
		&FbMinHsPs.ParaTest.WeldFreqHigh, &FbMaxHsPs.ParaTest.WeldFreqHigh,
		&HsParameterSet[HsPreset_Default].ParaTest.WeldFreqHigh, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET | RPRCB | WPRCB)},
		{PARAMINFO(1488, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1489, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//********Stack Parameter - Function 2(Scan)*******/

		{PARAMINFO(1490, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaScan.FrequencyStart,
		&FbMinHsPs.ParaScan.FrequencyStart, &FbMaxHsPs.ParaScan.FrequencyStart,
		&HsParameterSet[HsPreset_Default].ParaScan.FrequencyStart, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET | RPRCB | WPRCB)},
		{PARAMINFO(1491, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaScan.FrequencyStop,
		&FbMinHsPs.ParaScan.FrequencyStop, &FbMaxHsPs.ParaScan.FrequencyStop,
		&HsParameterSet[HsPreset_Default].ParaScan.FrequencyStop, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET | RPRCB | WPRCB)},
		{PARAMINFO(1492, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaScan.FrequencyStep,
		&FbMinHsPs.ParaScan.FrequencyStep, &FbMaxHsPs.ParaScan.FrequencyStep,
		&HsParameterSet[HsPreset_Default].ParaScan.FrequencyStep, PARAMINFOEXT(0, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1493, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaScan.TimeDelay,
		&FbMinHsPs.ParaScan.TimeDelay, &FbMaxHsPs.ParaScan.TimeDelay,
		&HsParameterSet[HsPreset_Default].ParaScan.TimeDelay, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1494, TYPE_16, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaScan.MaxAmplitude,
		&FbMinHsPs.ParaScan.MaxAmplitude, &FbMaxHsPs.ParaScan.MaxAmplitude,
		&HsParameterSet[HsPreset_Default].ParaScan.MaxAmplitude, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1495, TYPE_16, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaScan.MaxCurrent,
		&FbMinHsPs.ParaScan.MaxCurrent, &FbMaxHsPs.ParaScan.MaxCurrent,
		&HsParameterSet[HsPreset_Default].ParaScan.MaxCurrent, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1496, TYPE_32, TYPE_A_32), &HsParameterSet[CurrHsPreset].ParaScan.TimeErrorHigh,
		&FbMinHsPs.ParaScan.TimeErrorHigh, &FbMaxHsPs.ParaScan.TimeErrorHigh,
		&HsParameterSet[HsPreset_Default].ParaScan.TimeErrorHigh, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET)},
		{PARAMINFO(1497, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1498, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1499, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1500, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1501, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1502, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1503, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1504, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//Common Stack parameters
		{PARAMINFO(1505, TYPE_A_S32, TYPE_A_32), &HsParameterSet[CurrHsPreset].DigitalTuneScaledUP,
		&FbMinHsPs.DigitalTuneScaledUP, &FbMaxHsPs.DigitalTuneScaledUP,
		&HsParameterSet[HsPreset_Default].DigitalTuneScaledUP, PARAMINFOEXT(ARRAYHSPS, SAVECRC_HSPS, ACCPARASET | RPRCB | WPRCB | WPSCB)},
		/*//Stack Parameter - Function 3
		{PARAMINFO(1505, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1506, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1507, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1508, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1509, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1510, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1511, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1512, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1513, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1514, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1515, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1516, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1517, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1518, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1519, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		//Stack Parameter - Function 4

		{PARAMINFO(1520, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1521, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1522, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1523, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1524, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1525, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1526, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1527, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1528, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1529, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1530, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1531, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1532, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1533, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1534, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},

		//Stack Parameter - Function 5
		{PARAMINFO(1535, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1536, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1537, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1538, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1539, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1540, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1541, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1542, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1543, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1544, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1545, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1546, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1547, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1548, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1549, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		//Stack Parameter - Function 6

		{PARAMINFO(1550, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1551, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1552, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1553, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1554, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1555, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1556, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1557, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1558, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1559, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1560, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1561, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1562, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1563, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1564, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},

		//Stack Parameter - Function 7
		{PARAMINFO(1565, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1566, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1567, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1568, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1569, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1570, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1571, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1572, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1573, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1574, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1575, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1576, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1577, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1578, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		{PARAMINFO(1579, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSPS, 0, 0)},
		//End Stack Parameter*/
		//
		//Start Stack Status Parameter
		//******** start Stack Status Parameter - Function 0(Seek) Copy of param set//
		{PARAMINFO(1610, TYPE_16, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsPreset.ParaSeek.SeekTime,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1611, TYPE_16, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsPreset.ParaSeek.SeekAmp,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1612, TYPE_16, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsPreset.ParaSeek.SeekRampTime,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsPreset.ParaSeek.SeekAmpLoopC1,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsPreset.ParaSeek.SeekAmpLoopC2,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1615, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsPreset.ParaSeek.FreqOffset,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsPreset.ParaSeek.SeekPhaseLoopI,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1617, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsPreset.ParaSeek.SeekFreqLow,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1618, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsPreset.ParaSeek.SeekFreqHigh,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1619, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1620, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1621, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1622, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1623, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1624, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//RTC
		{PARAMINFO(1625, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].RTCDate.Date,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1626, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].RTCTime.Time,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1627, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1628, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1629, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//ERROR REASON
		{PARAMINFO(1630, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[0],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1631, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[1],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1632, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[2],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1633, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[3],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1634, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[4],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1635, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[5],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1636, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[6],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1637, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[7],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1638, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[8],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1639, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[9],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1640, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[10],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1641, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[11],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1642, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[12],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1643, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[13],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1644, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[14],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1645, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[15],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1646, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[16],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1647, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[17],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1648, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[18],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1649, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[19],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1650, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[20],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1651, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[21],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1652, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[22],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1653, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[23],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1654, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[24],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1655, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[25],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1656, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[26],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1657, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[27],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1658, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[28],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1659, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[29],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1660, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[30],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1661, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[31],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1662, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[32],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1663, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[33],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1664, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[34],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1665, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[35],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1666, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[36],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1667, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[37],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1668, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[38],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1669, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[39],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1670, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[40],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1671, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[41],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1672, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[42],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1673, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[43],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1674, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[44],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1675, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[45],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1676, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[46],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1677, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].FBAlarms[47],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1678, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].ErrorCodeFB,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1679, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1680, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.Time,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1681, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.AvgAmplitude,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1682, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.RecalledMidBandFreqAbs,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1683, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.SeekStartFrequencyAbs,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1684, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.SeekStopFrequencyAbs,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1685, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.SeekMemoryFrequencyAbs,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1686, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.ResFreqOK,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1687, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.EndSetAmplitude,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1688, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.EndAmplitude,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1689, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.EndPwm,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1690, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.EndPower,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1691, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.EndCurrent,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1692, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.EndPhase,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1693, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][0].HsStatus.SeekStatus.EndTemperature,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1694, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//******** End Stack Status Parameter - Function 0(Seek)*******/
		//dummy
		{PARAMINFO(1695, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1696, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1697, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1698, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1699, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1700, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1701, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1702, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1703, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1704, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1705, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1706, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1707, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1708, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1709, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		//********Start Stack Status Parameter - Function 1(Test)*****/
		{PARAMINFO(1710, TYPE_16, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.Amplitude1,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1711, TYPE_16, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.AmpStepCondition,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1712, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.AmplitudeProfileTime,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1713, TYPE_16, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.Amplitude2,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1714, TYPE_16, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.AmplitudeStartRampTime,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(0, TYPE_16, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.AmplitudeProfileRampTime,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.AmplitudeLoopC1,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.AmplitudeLoopC2,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.FreqOffset,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(0, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.PhaseLoopI,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1720, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.TimeErrorHigh,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1721, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.WeldFreqLow,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1722, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsPreset.ParaTest.WeldFreqHigh,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1723, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1724, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//RTC
		{PARAMINFO(1725, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].RTCDate.Date,
				0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1726, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].RTCTime.Time,
				0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		//ERROR REASON
		{PARAMINFO(1727, TYPE_VOID, TYPE_VOID), 0,	0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1728, TYPE_VOID, TYPE_VOID), 0,	0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1729, TYPE_VOID, TYPE_VOID), 0,	0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		{PARAMINFO(1730, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[0],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1731, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[1],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1732, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[2],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1733, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[3],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1734, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[4],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1735, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[5],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1736, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[6],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1737, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[7],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1738, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[8],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1739, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[9],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1740, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[10],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1741, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[11],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1742, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[12],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1743, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[13],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1744, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[14],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1745, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[15],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1746, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[16],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1747, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[17],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1748, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[18],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1749, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[19],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1750, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[20],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1751, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[21],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1752, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[22],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1753, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[23],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1754, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[24],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1755, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[25],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1756, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[26],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1757, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[27],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1758, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[28],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1759, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[29],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1760, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[30],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1761, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[31],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1762, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[32],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1763, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[33],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1764, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[34],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1765, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[35],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1766, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[36],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1767, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[37],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1768, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[38],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1769, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[39],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1770, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[40],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1771, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[41],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1772, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[42],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1773, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[43],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1774, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[44],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1775, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[45],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1776, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[46],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1777, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].FBAlarms[47],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1778, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].ErrorCodeFB,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1779, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		{PARAMINFO(1780, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.Time,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1781, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.AvgAmplitude1,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1782, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.AvgAmplitude2,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1783, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.RecalledResFreqAbs,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1784, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.ResFreqOK,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1785, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.TestStartFrequencyAbs,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1786, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.TestStopFrequencyAbs,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1787, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.EndAmplitudeSet,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1788, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.EndAmplitude,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1789, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.EndPwm,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},

		{PARAMINFO(1790, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.EndPower,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},

		{PARAMINFO(1791, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.EndCurrent,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1792, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.EndPhase,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1793, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][1].HsStatus.TestStatus.EndTemperature,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},

		{PARAMINFO(1794, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][HFTEST].HsStatus.TestStatus.PeakPower,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},

		//********End Stack Status Parameter - Function 1(Test)*****/
		//dummy
		{PARAMINFO(FB_PARAM_TESTPEAKPOWERRAW, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][HFTEST].HsStatus.TestStatus.PeakPower,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS | RPRCB)},

		{PARAMINFO(1796, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1797, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1798, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1799, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1800, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1801, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1802, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1803, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1804, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1805, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1806, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1807, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1808, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1809, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		//********Start Stack Status Parameter - Function 2(Scan)*******/
		{PARAMINFO(1810, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsPreset.ParaScan.FrequencyStart,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1811, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsPreset.ParaScan.FrequencyStop,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1812, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsPreset.ParaScan.FrequencyStep,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1813, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsPreset.ParaScan.TimeDelay,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1814, TYPE_16, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsPreset.ParaScan.MaxAmplitude,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1815, TYPE_16, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsPreset.ParaScan.MaxCurrent,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1816, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsPreset.ParaScan.TimeErrorHigh,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1817, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1818, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1819, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1820, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1821, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1822, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1823, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1824, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		//RTC
		{PARAMINFO(1825, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].RTCDate.Date,
				0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1826, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].RTCTime.Time,
				0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		//ERROR REASON
		{PARAMINFO(1827, TYPE_VOID, TYPE_VOID), 0,	0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1828, TYPE_VOID, TYPE_VOID), 0,	0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1829, TYPE_VOID, TYPE_VOID), 0,	0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		{PARAMINFO(1830, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[0],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1831, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[1],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1832, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[2],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1833, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[3],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1834, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[4],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1835, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[5],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1836, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[6],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1837, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[7],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1838, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[8],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1839, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[9],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1840, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[10],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1841, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[11],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1842, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[12],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1843, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[13],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1844, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[14],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1845, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[15],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1846, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[16],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1847, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[17],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1848, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[18],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1849, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[19],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1850, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[20],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1851, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[21],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1852, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[22],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1853, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[23],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1854, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[24],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1855, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[25],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1856, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[26],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1857, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[27],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1858, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[28],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1859, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[29],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1860, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[30],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1861, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[31],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1862, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[32],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1863, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[33],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1864, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[34],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1865, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[35],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1866, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[36],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1867, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[37],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1868, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[38],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1869, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[39],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1870, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[40],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1871, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[41],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1872, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[42],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1873, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[43],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1874, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[44],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1875, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[45],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1876, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[46],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1877, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].FBAlarms[47],
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1878, TYPE_A_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].ErrorCodeFB,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1879, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},

		{PARAMINFO(1880, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsStatus.ScanStatus.Time,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1881, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsStatus.ScanStatus.ScanStartFrequency,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1882, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsStatus.ScanStatus.ScanStopFrequency,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1883, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsStatus.ScanStatus.EndAmplitude,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1884, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsStatus.ScanStatus.EndPwm,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1885, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsStatus.ScanStatus.EndPower,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1886, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsStatus.ScanStatus.EndCurrent,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1887, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsStatus.ScanStatus.EndPhase,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1888, TYPE_32, TYPE_A_32), &HsStatusSet[CurrHsPreset][2].HsStatus.ScanStatus.EndTemperature,
		0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, RS)},
		{PARAMINFO(1889, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1890, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1891, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1892, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1893, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		{PARAMINFO(1894, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(0, 0, 0)},
		//End Stack Status Parameter - Function 2(Scan)/
		/*//dummy
		{PARAMINFO(1895, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1896, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1897, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1898, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1899, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1900, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1901, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1902, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1903, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1904, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1905, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1906, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1907, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1908, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1909, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//Start Stack Status Parameter - Function 3
		{PARAMINFO(1910, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1911, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1912, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1913, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1914, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1915, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1916, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1917, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1918, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1919, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1920, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1921, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1922, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1923, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1924, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1925, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1926, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1927, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1928, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1929, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//dummy
		{PARAMINFO(1930, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1931, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1932, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1933, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1934, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(1935, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1936, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1937, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1938, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1939, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1940, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1941, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1942, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1943, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1944, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1945, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1946, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1947, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1948, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1949, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//missing
		{PARAMINFO(1950, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//dummy
		{PARAMINFO(1951, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1952, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1953, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1954, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(1955, TYPE_32, TYPE_A_32), &RTCDate, 0, 0,
		0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1956, TYPE_32, TYPE_A_32), &RTCTime, 0, 0,
		0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//dummy
		{PARAMINFO(1957, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1958, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1959, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(1960, TYPE_VOID, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1961, TYPE_VOID, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//missing
		{PARAMINFO(1962, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1963, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1964, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1965, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//dummy
		{PARAMINFO(1966, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1967, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1968, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1969, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//missing
		{PARAMINFO(1970, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1971, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1972, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1973, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1974, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1975, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//dummy
		{PARAMINFO(1976, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1977, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1978, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1979, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(1980, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1981, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1982, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1983, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1984, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1985, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1986, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1987, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1988, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1989, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1990, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1991, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1992, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1993, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(1994, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//End Stack Status Parameter - Function 3
		//dummy
		{PARAMINFO(1995, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1996, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1997, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1998, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(1999, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2000, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2001, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2002, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2003, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2004, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2005, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2006, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2007, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2008, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2009, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//Start Stack Status Parameter - Function 4
		{PARAMINFO(2010, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2011, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2012, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2013, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2014, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2015, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2016, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2017, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2018, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2019, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2020, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2021, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2022, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2023, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2024, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2025, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2026, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2027, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2028, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2029, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//dummy
		{PARAMINFO(2030, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2031, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2032, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2033, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2034, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2035, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2036, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2037, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2038, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2039, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2040, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2041, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2042, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2043, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2044, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2045, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2046, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2047, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2048, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2049, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//missing
		{PARAMINFO(2050, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//dummy
		{PARAMINFO(2051, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2052, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2053, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2054, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2055, TYPE_32, TYPE_A_32), &RTCDate, 0, 0,
		0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2056, TYPE_32, TYPE_A_32), &RTCTime, 0, 0,
		0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//dummy
		{PARAMINFO(2057, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2058, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2059, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2060, TYPE_VOID, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2061, TYPE_VOID, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//missing
		{PARAMINFO(2062, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2063, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2064, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2065, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//dummy
		{PARAMINFO(2066, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2067, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2068, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2069, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//missing
		{PARAMINFO(2070, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2071, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2072, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2073, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2074, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2075, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//dummy
		{PARAMINFO(2076, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2077, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2078, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2079, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2080, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2081, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2082, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2083, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2084, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2085, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2086, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2087, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2088, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2089, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2090, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2091, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2092, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2093, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2094, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//End Stack Status Parameter - Function 4
		//dummy
		{PARAMINFO(2095, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2096, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2097, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2098, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2099, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2100, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2101, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2102, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2103, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2104, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2105, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2106, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2107, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2108, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2109, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//Start Stack Status Parameter - Function 5
		{PARAMINFO(2110, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2111, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2112, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2113, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2114, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2115, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2116, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2117, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2118, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2119, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2120, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2121, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2122, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2123, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2124, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2125, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2126, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2127, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2128, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2129, TYPE_VOID, TYPE_A_8), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//dummy
		{PARAMINFO(2130, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2131, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2132, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2133, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2134, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2135, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2136, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2137, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2138, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2139, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2140, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2141, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2142, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2143, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2144, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2145, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2146, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2147, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2148, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2149, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//missing
		{PARAMINFO(2150, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//dummy
		{PARAMINFO(2151, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2152, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2153, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2154, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2155, TYPE_32, TYPE_A_32), &RTCDate, 0, 0,
		0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2156, TYPE_32, TYPE_A_32), &RTCTime, 0, 0,
		0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//dummy
		{PARAMINFO(2157, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2158, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2159, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2160, TYPE_VOID, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2161, TYPE_VOID, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//missing
		{PARAMINFO(2162, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2163, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2164, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2165, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//dummy
		{PARAMINFO(2166, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2167, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2168, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2169, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//missing
		{PARAMINFO(2170, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2171, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2172, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2173, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2174, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2175, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//dummy
		{PARAMINFO(2176, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2177, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2178, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2179, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2180, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2181, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2182, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2183, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2184, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2185, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2186, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2187, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2188, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2189, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2190, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2191, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2192, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2193, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2194, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		// End Stack Status Parameter - Function 5
		//dummy
		{PARAMINFO(2195, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2196, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2197, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2198, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2199, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2200, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2201, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2202, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2203, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2204, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2205, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2206, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2207, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2208, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2209, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//Start Stack Status Parameter - Function 6
		{PARAMINFO(2210, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2211, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2212, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2213, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2214, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2215, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2216, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2217, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2218, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2219, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2220, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2221, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2222, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2223, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2224, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2225, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2226, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2227, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2228, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2229, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//dummy
		{PARAMINFO(2230, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2231, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2232, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2233, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2234, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2235, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2236, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2237, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2238, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2239, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2240, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2241, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2242, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2243, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2244, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2245, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2246, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2247, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2248, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2249, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//missing
		{PARAMINFO(2250, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//dummy
		{PARAMINFO(2251, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2252, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2253, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2254, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2255, TYPE_32, TYPE_A_32), &RTCDate, 0, 0,
		0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2256, TYPE_32, TYPE_A_32), &RTCTime, 0, 0,
		0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//dummy
		{PARAMINFO(2257, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2258, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2259, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2260, TYPE_VOID, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2261, TYPE_VOID, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//missing
		{PARAMINFO(2262, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2263, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2264, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2265, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//dummy
		{PARAMINFO(2266, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2267, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2268, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2269, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//missing
		{PARAMINFO(2270, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2271, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2272, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2273, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2274, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2275, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//dummy
		{PARAMINFO(2276, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2277, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2278, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2279, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2280, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2281, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2282, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2283, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2284, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2285, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2286, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2287, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2288, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2289, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2290, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2291, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2292, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2293, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2294, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//End Stack Status Parameter - Function 6
		//dummy
		{PARAMINFO(2295, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2296, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2297, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2298, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2299, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2300, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2301, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2302, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2303, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2304, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2305, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2306, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2307, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2308, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2309, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		// Start Stack Status Parameter - Function 7
		{PARAMINFO(2310, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2311, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2312, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2313, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2314, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2315, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2316, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2317, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2318, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2319, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2320, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2321, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2322, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2323, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2324, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2325, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2326, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2327, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2328, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2329, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//dummy
		{PARAMINFO(2330, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2331, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2332, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2333, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2334, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2335, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2336, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2337, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2338, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2339, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2340, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2341, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2342, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2343, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2344, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2345, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2346, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2347, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2348, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2349, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//missing
		{PARAMINFO(2350, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//dummy
		{PARAMINFO(2351, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2352, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2353, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2354, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2355, TYPE_32, TYPE_A_32), &RTCDate, 0, 0,
		0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2356, TYPE_32, TYPE_A_32), &RTCTime, 0, 0,
		0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//dummy
		{PARAMINFO(2357, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2358, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2359, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2360, TYPE_VOID, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2361, TYPE_VOID, TYPE_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//missing
		{PARAMINFO(2362, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2363, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2364, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2365, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//dummy
		{PARAMINFO(2366, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2367, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2368, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2369, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		//missing
		{PARAMINFO(2370, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2371, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2372, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2373, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2374, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2375, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		//dummy
		{PARAMINFO(2376, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2377, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2378, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},
		{PARAMINFO(2379, TYPE_VOID, TYPE_VOID), 0, 0, 0, 0, PARAMINFOEXT(ARRAYNA, 0, 0)},

		{PARAMINFO(2380, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2381, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2382, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2383, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2384, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2385, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2386, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2387, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2388, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2389, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2390, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2391, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2392, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2393, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		{PARAMINFO(2394, TYPE_VOID, TYPE_A_32), 0, 0, 0, 0, PARAMINFOEXT(ARRAYHSSS, 0, 0)},
		//End Stack Status Parameter - Function 7 */
	};

//Defines to read/write values to/from table
#define PARANUM(Info) ((UINT16)(Info & 0xFFFF))
#define PARATYPE(Info) ((ParaType)((Info >> 16) & 0xFF))
#define PARATYPEPB(Info) ((ParaType)((Info >> 24) & 0xFF))
#define ARRAYTYPE(InfoExt) ((ArrayType)(InfoExt & 0xFF))
#define CRCCMD(InfoExt) ((SAVECRC_CMD)((InfoExt >> 8) & 0xFF))
#define ACCRIGHTS(InfoExt) ((UINT16)((InfoExt >> 16) & 0xFFFF))
#define VAL8(X)   (*((UINT8*)X))
#define VAL16(X)  (*((UINT16*)X))
#define VAL32(X)  (*((UINT32*)X))
#define VALS8(X)  (*((SINT8*)X))
#define VALS16(X) (*((SINT16*)X))
#define VALS32(X) (*((SINT32*)X))

/*  void InitBaseIndexes(void)
 *
 *   Purpose:
 *    This function is called at powerup by Fieldbus Class constructor. It parses the global PTable
 *    and finds the table index of first parameter entry for different group of parameters
 *    (Test, SysInfor preset etc.).
 *
 *   Entry condition:
 *    None
 *
 *   Exit condition:
 *   None
 */
void InitBaseIndexes(void)
{
	UINT32 Indx;
	for(Indx = 0; Indx < (sizeof(PTable)/sizeof(PTable[0])) ; Indx++){
		switch(PARANUM(PTable[Indx].ParamInfo)){
			case TEST_PARA_BASE: 
				TestParamBaseTableIndex = Indx;
				break;
			case VERSIONINFO_BASE: 
				VersionBaseTableIndex = Indx;
				break;
			case SYSINFO_BASE: 
				SysInfoBaseTableIndex = Indx;
				break;
			case RTC_DATE: 
				RTCBaseTableIndex = Indx;
				break;
			case ERROR_INFO_BASE: 
				ErrorInfoBaseTableIndex = Indx;
				break;
			case EXTCTRL_INFO_BASE: 
				ExtctrlBaseTableIndex = Indx;
				break;
			case CNTR_INFO_BASE: 
				CntrInfoBaseTableIndex = Indx;
				break;
			case PARA_SET_BASE: 
				ParamBaseTableIndex = Indx;
				break;
			case WS_SET_BASE: 
				WSBaseTableIndex = Indx;
				break;
			case HSPARA_SET_BASE: 
				HSParamBaseTableIndex = Indx;
				break;
			case HSSS_SET_BASE: 
				HSSSParamBaseTableIndex = Indx;
				break;
			default :
				break;
		}
	}
}


/*  void SetMinMaxParamForFbTable()
 *
 *   Purpose:
 *    This function is called at powerup by Fieldbus Class constructor and whenever TweakRangeEnabled
 *    in changed in sysconfig. Also called when min,max,tweakmin,tweakmax parameters are changed.
 *
 *   Entry condition:
 *    None
 *
 *   Exit condition:
 *   None
 */
void SetMinMaxParamForFbTable(PSUPDATESPECIAL PsUpdateSpcl)
{
	SetupMinMaxPresetIndexes();
	if((PsUpdateSpcl & UPDATEMIN) == UPDATEMIN){
		FbMinPs = ParameterSet[MinPSNum];
		FbMinHsPs = HsParameterSet[MinHSPSNum];
	}
	if((PsUpdateSpcl & UPDATEMAX) == UPDATEMAX){
		FbMaxPs = ParameterSet[MaxPSNum];
		FbMaxHsPs = HsParameterSet[MaxHSPSNum];
	}
}


SINT32 GetParameterIndexInTable(UINT16 ParamNumber)
{
   SINT32 ParamIndexInTable;
   if((ParamNumber >= (PARA_SET_BASE)) && ((ParamNumber <= (PARA_SET_LAST))))
      ParamIndexInTable = (ParamNumber - PARA_SET_BASE) + ParamBaseTableIndex;
    else if((ParamNumber >= (WS_SET_BASE)) && ((ParamNumber <= (WS_SET_LAST))))
      ParamIndexInTable = (ParamNumber - WS_SET_BASE) + WSBaseTableIndex;
    else if((ParamNumber >= (HSPARA_SET_BASE)) && ((ParamNumber <= (HSPARA_SET_LAST))))
      ParamIndexInTable = (ParamNumber - HSPARA_SET_BASE) + HSParamBaseTableIndex;
    else if((ParamNumber >= (HSSS_SET_BASE)) && ((ParamNumber <= (HSSS_SET_LAST))))
      ParamIndexInTable = (ParamNumber - HSSS_SET_BASE) + HSSSParamBaseTableIndex;
    else if((ParamNumber >= (SYSINFO_BASE)) && ((ParamNumber <= (SYSINFO_LAST))))
      ParamIndexInTable = (ParamNumber - SYSINFO_BASE) + SysInfoBaseTableIndex;
    else if((ParamNumber >= (VERSIONINFO_BASE)) && ((ParamNumber <= (VERSIONINFO_LAST))))
      ParamIndexInTable = (ParamNumber - VERSIONINFO_BASE) + VersionBaseTableIndex;
    else if((ParamNumber >= (TEST_PARA_BASE)) && ((ParamNumber <= (TEST_PARA_LAST))))
      ParamIndexInTable = (ParamNumber - TEST_PARA_BASE) + TestParamBaseTableIndex;
    else if((ParamNumber >= (RTC_DATE)) && ((ParamNumber <= (RTC_TIME))))
      ParamIndexInTable = (ParamNumber - RTC_DATE) + RTCBaseTableIndex;
    else if((ParamNumber >= (ERROR_INFO_BASE)) && ((ParamNumber <= (ERROR_INFO_LAST))))
      ParamIndexInTable = (ParamNumber - ERROR_INFO_BASE) + ErrorInfoBaseTableIndex;
    else if((ParamNumber >= (EXTCTRL_INFO_BASE)) && ((ParamNumber <= (EXTCTRL_INFO_LAST))))
      ParamIndexInTable = (ParamNumber - EXTCTRL_INFO_BASE) + ExtctrlBaseTableIndex;
    else if((ParamNumber >= (CNTR_INFO_BASE)) && ((ParamNumber <= (CNTR_INFO_LAST))))
       ParamIndexInTable = (ParamNumber - CNTR_INFO_BASE) + CntrInfoBaseTableIndex;
    else
       ParamIndexInTable = -1;

   return ParamIndexInTable;
}

/*  ParamAccessError WriteParam(ParaType Type, ParamSpecialAccess SepcAcc, BOOLEAN TokenAvailable,void * Val, UINT32 ArrayIndex, UINT16 ParamNumber)
 *
 *  Purpose:
 *    To validate the type, Index , special access, rights , min, max limit of parameter and to modify the
 *    parameter value. This function is called by HandleValue function if a parameter is accessed for write from PKW.
 *
 *  Entry condition:
 *    Type: Represents width 8,16 or 32 bit and array or non array parameter.
 *    SpecAcc: Special access.
 *    TokenAvailable: Token currently avaialbale or not to fieldbus status.
 *    Val: The pointer to the value to be written.
 *    ArrayIndex: Index of array the parameter belogs to.
 *    ParamNumber: The unique parameter number.
 *
 *  Exit condition:
 *    Returns appropriate error code in case of error else returns VAL_ERR_NO_ERR.
 */
ParamAccessError WriteParam(ParaType Type, ParamSpecialAccess SepcAcc, BOOLEAN TokenAvailable,void * Val,
		UINT32 ArrayIndex, UINT16 ParamNumber)
{
   SINT32 ParamIndexInTable = 0;
   void * VarPtr = 0, * MinvalPtr = 0, *MaxvalPtr = 0;
   UINT32 TempVal = (UINT32)VAL32(Val), TempVal8, TempVal16;
   UINT16 Access = 0;
   ParamAccessError RetVal = VAL_ERR_NO_ERR;
   EnableLimitCheck = TRUE;
   ParamIndexInTable = GetParameterIndexInTable(ParamNumber);
   if(ParamIndexInTable < 0)
	  RetVal = VAL_ERR_INVALID_VAL_NBR;
   if (RetVal == VAL_ERR_NO_ERR){
	   if(SepcAcc == SPEC_ACC_ERROR)
		   RetVal = VAL_ERR_SPEC_ACCESS_MISMATCH;
	   else if((UINT32)ParamIndexInTable >= (sizeof(PTable)/sizeof(PTable[0])))//additional check
			RetVal = VAL_ERR_INVALID_VAL_NBR;
	   else if(PARANUM(PTable[ParamIndexInTable].ParamInfo) != ParamNumber)
			 RetVal = VAL_ERR_INVALID_VAL_NBR;
	   else if(PARATYPEPB(PTable[ParamIndexInTable].ParamInfo) != Type)
			 RetVal = VAL_ERR_TYPE_MISMATCH;
	   else{
		   Access = ACCRIGHTS(PTable[ParamIndexInTable].ParamInfoExt);
		   if(((Access & WSAT) == WSAT) && (TokenAvailable == FALSE))
			   RetVal = VAL_ERR_NO_ACCESS_COMM_IFC_TOKEN;
		}
   }
   if(RetVal == VAL_ERR_NO_ERR){
      switch (ARRAYTYPE(PTable[ParamIndexInTable].ParamInfoExt)){
         case ARRAYPS :
             if(ArrayIndex > MAXIMUM_USER_PRESETS)
                RetVal = VAL_ERR_INDEX_MISMATCH;
             else if(ArrayIndex == MAXIMUM_USER_PRESETS){
                VarPtr = (UINT8*)((UINT8*)&CommPS + ((UINT8*)PTable[ParamIndexInTable].ParamVal - (UINT8*)&ParameterSet[CurrPreset]));
//                if(EnableDebug) pprintf(" VarPtr %08X %08X %08X %08X %08X", VarPtr, (UINT8*)&CommPS,(UINT8*)&ParameterSet[CurrPreset],
//                      (UINT8*)PTable[ParamIndexInTable].ParamVal,
//                      ((UINT8*)PTable[ParamIndexInTable].ParamVal - (UINT8*)&ParameterSet[CurrPreset]));
             }
             else
                VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (ArrayIndex * sizeof(DcxPreset));
             break;
         case ARRAYHSPS :
              if(ArrayIndex > MAXIMUM_USER_HSPRESETS)
                 RetVal = VAL_ERR_INDEX_MISMATCH;
              else if(ArrayIndex == MAXIMUM_USER_HSPRESETS)
                 VarPtr =  (UINT8*)((UINT8*)&CommHSPS + ((UINT8*)PTable[ParamIndexInTable].ParamVal - (UINT8*)&HsParameterSet[CurrHsPreset]));
              else
                 VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (ArrayIndex * sizeof(HSPRESET));
              break;
         case ARRAYSS :
         case ARRAYHSSS :
         case ARRAYSN:
              RetVal = VAL_ERR_NOT_WRITEABLE;
           	break;
         case ARRAYNA:
        	 if(ArrayIndex > 0)
        	 	 RetVal = VAL_ERR_INDEX_MISMATCH;
        	 else
        		 VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal;
//             if(EnableDebug) pprintf("\n Non array write access");
            break;
         default:
            RetVal = VAL_ERR_INVALID_VAL_NBR;
            break;
      }
   }
   if(((Access & WPRCB) == WPRCB) && (RetVal == VAL_ERR_NO_ERR))
  		RetVal = ParamAccessPreCallback(ParamNumber, &TempVal, ArrayIndex, ACC_WRITE);

   if((RetVal == VAL_ERR_NO_ERR) && (EnableLimitCheck == TRUE)){
	   MinvalPtr = PTable[ParamIndexInTable].MinVal;
	   MaxvalPtr = PTable[ParamIndexInTable].MaxVal;
       switch(PARATYPE(PTable[ParamIndexInTable].ParamInfo))
       {
          case TYPE_8:
          case TYPE_A_8:
             TempVal8 = (TempVal & 0xFF);
             if(EnableDebug) pprintf("\n 8bit %d %d %d", VAL8(MinvalPtr), VAL8(MaxvalPtr), TempVal8);
             if((MinvalPtr) && (TempVal8 < VAL8(MinvalPtr)))
                RetVal = VAL_ERR_LIMIT_LOW;
             else if((MaxvalPtr) && (TempVal8 > VAL8(MaxvalPtr)))
                RetVal = VAL_ERR_LIMIT_HIGH;
             else
                VAL8(VarPtr) = TempVal8;
             break;
          case TYPE_16:
          case TYPE_A_16:
             TempVal16 = (TempVal & 0xFFFF);
//             if(EnableDebug) pprintf("\n 16bit %d %d %d", VAL16(MinvalPtr), VAL16(MaxvalPtr), TempVal16);
             if((MinvalPtr) && (TempVal16 < VAL16(MinvalPtr)))
                RetVal = VAL_ERR_LIMIT_LOW;
             else if((MaxvalPtr) && (TempVal16 > VAL16(MaxvalPtr)))
                RetVal = VAL_ERR_LIMIT_HIGH;
             else
                VAL16(VarPtr) = TempVal16;
             break;
          case TYPE_32:
          case TYPE_A_32:
//             if(EnableDebug) pprintf("\n 32bit %d min %x mx %x", TempVal, MinvalPtr, MaxvalPtr);
             if((MinvalPtr) && (TempVal < VAL32(MinvalPtr)))
                RetVal = VAL_ERR_LIMIT_LOW;
             else if((MaxvalPtr) && (TempVal > VAL32(MaxvalPtr)))
                RetVal = VAL_ERR_LIMIT_HIGH;
             else
                VAL32(VarPtr) = TempVal;
             break;
          case TYPE_S8:
          case TYPE_A_S8:
              TempVal8 = (TempVal & 0xFF);
              if(EnableDebug) pprintf("\n 8bit %d %d %d", VALS8(MinvalPtr), VALS8(MaxvalPtr), (SINT8)TempVal8);
              if((MinvalPtr) && ((SINT8)TempVal8 < VALS8(MinvalPtr)))
                 RetVal = VAL_ERR_LIMIT_LOW;
              else if((MaxvalPtr) && ((SINT8)TempVal8 > VALS8(MaxvalPtr)))
                 RetVal = VAL_ERR_LIMIT_HIGH;
              else
                 VALS8(VarPtr) = (SINT8)TempVal8;
              break;
           case TYPE_S16:
           case TYPE_A_S16:
              TempVal16 = (TempVal & 0xFFFF);
              if(EnableDebug) pprintf("\n 16bit %d %d %d", VALS16(MinvalPtr), VALS16(MaxvalPtr), (SINT16)TempVal16);
              if((MinvalPtr) && ((SINT16)TempVal16 < VALS16(MinvalPtr)))
                 RetVal = VAL_ERR_LIMIT_LOW;
              else if((MaxvalPtr) && ((SINT16)TempVal16 > VALS16(MaxvalPtr)))
                 RetVal = VAL_ERR_LIMIT_HIGH;
              else
                 VALS16(VarPtr) = (SINT16)TempVal16;
              break;
           case TYPE_S32:
           case TYPE_A_S32:
              if(EnableDebug) pprintf("\n 32bit %d %d %d min %x mx %x", VALS32(MinvalPtr), VALS32(MaxvalPtr), (SINT32)TempVal, MinvalPtr, MaxvalPtr);
              if((MinvalPtr) && ((SINT32)TempVal < VALS32(MinvalPtr)))
                 RetVal = VAL_ERR_LIMIT_LOW;
              else if((MaxvalPtr) && ((SINT32)TempVal > VALS32(MaxvalPtr)))
                 RetVal = VAL_ERR_LIMIT_HIGH;
              else
                 VALS32(VarPtr) = (SINT32)TempVal;
              break;
          default:
             RetVal = VAL_ERR_TYPE_MISMATCH;
       }
   }
   if(EnableDebug) pprintf("\n f=%s acc=%04X ret=%d apeacc=%d", __FUNCTION__, Access, RetVal, SepcAcc);
   if(((Access & WPSCB) == WPSCB) &&(RetVal == VAL_ERR_NO_ERR))
		RetVal =  ParamAccessPostCallback(ParamNumber, &TempVal, ArrayIndex, ACC_WRITE);
   if (RetVal == VAL_ERR_NO_ERR) {
      if ((ARRAYTYPE(PTable[ParamIndexInTable].ParamInfoExt) == ARRAYPS) && (ArrayIndex < MAXIMUM_USER_PRESETS)) {
         ParameterSet[ArrayIndex].InfoParam.PresetStatus = NOTVERIFIED;
         if (ArrayIndex == CurrPreset)
            RegUpdateCmdFlag = TRUE;
         CalculateAndSavePresetCRC((SAVECRC_CMD) (CRCCMD(PTable[ParamIndexInTable].ParamInfoExt) | SAVECRC_INFO),
               PRESET_NUMBER(ArrayIndex));
         Log::WriteEventLog(EVENT_PARAMETER_CHANGED, ParamNumber, (VAL32(Val)));
      }
      else if ((ARRAYTYPE(PTable[ParamIndexInTable].ParamInfoExt) == ARRAYHSPS)
            && (ArrayIndex < MAXIMUM_USER_HSPRESETS)) {
         if (ArrayIndex == CurrHsPreset)
            RegUpdateCmdFlag = TRUE;
         CalculateAndSavePresetCRC(CRCCMD(PTable[ParamIndexInTable].ParamInfoExt), (PRESET_NUMBER) 0, HSPRESET_NUMBER(
               ArrayIndex));
         Log::WriteEventLog(EVENT_PARAMETER_CHANGED, ParamNumber, (VAL32(Val)));
      }
   }

   if(RetVal != VAL_ERR_NO_ERR){
//      if(EnableDebug) pprintf("\n write error %d ", RetVal);
   }
   return  RetVal;
}

/*  ParamAccessError ReadParam(ParaType Type, ParamSpecialAccess SepcAcc, void * Val, UINT32 ArrayIndex, UINT16 ParamNumber)
 *
 *  Purpose:
 *     To validate the type, Index, special access of parameter and to read the
 *     parameter value. This function is called by HandleValue function if a parameter is accessed for read from PKW.
 *
 *  Entry condition:
 *     Type: Represents width 8,16 or 32 bit and array or non array parameter.
 *     SpecAcc: Special access.
 *     Val: The pointer where read value to be written.
 *     ArrayIndex: Index of array the parameter belongs to.
 *     ParamNumber: The unique parameter number.
 *
 *  Exit condition:
 *     Returns appropriate error code in case of error else returns VAL_ERR_NO_ERR.
 */
ParamAccessError ReadParam(ParaType Type, ParamSpecialAccess SepcAcc, void * Val, UINT32 ArrayIndex, UINT16 ParamNumber)
{
//   if(EnableDebug) pprintf("\n %s %d %d %d", __FUNCTION__, Type, ArrayIndex, ParamNumber);
   SINT32 ParamIndexInTable = 0;
   UINT32 TempVal;
   void * VarPtr = 0;
   ParamAccessError RetVal = VAL_ERR_NO_ERR;
   ReadFromTable = TRUE;
   UINT16 Access = 0;

   ParamIndexInTable = GetParameterIndexInTable(ParamNumber);
   if(ParamIndexInTable < 0)
     RetVal = VAL_ERR_INVALID_VAL_NBR;

   if (RetVal == VAL_ERR_NO_ERR){
	   if(SepcAcc == SPEC_ACC_ERROR)
		   RetVal = VAL_ERR_SPEC_ACCESS_MISMATCH;
	   else if((UINT32)ParamIndexInTable >= (sizeof(PTable)/sizeof(PTable[0])))//additional check
		   RetVal = VAL_ERR_INVALID_VAL_NBR;
	   else if(PARANUM(PTable[ParamIndexInTable].ParamInfo) != ParamNumber)
			 RetVal =  VAL_ERR_INVALID_VAL_NBR;
	   else if(PARATYPEPB(PTable[ParamIndexInTable].ParamInfo) != Type)
			 RetVal = VAL_ERR_TYPE_MISMATCH;
	   else
		   Access = ACCRIGHTS(PTable[ParamIndexInTable].ParamInfoExt);
   }

 //  if(EnableDebug) pprintf("\n ParamBaseTableIndex %d %d %d %d", ParamBaseTableIndex, ParamIndexInTable, PARANUM(PTable[ParamIndexInTable].ParamInfo), PARATYPEPB(PTable[ParamIndexInTable].ParamInfo));

   if(RetVal == VAL_ERR_NO_ERR){
	     if(SepcAcc != SPEC_ACC_STANDARD){
	    	 switch(SepcAcc){
				 case SPEC_ACC_READ_DEFAULT:
					 if((Access & RD) == RD)
						 VarPtr = (UINT8*)PTable[ParamIndexInTable].DfltVal;
					 else
						 RetVal = VAL_ERR_SPEC_ACCESS_MISMATCH;
					 break;
				 case SPEC_ACC_READ_LIMIT_LOW:
					 if((Access & RLL) == RLL)
						 VarPtr = (UINT8*)PTable[ParamIndexInTable].MinVal;
					 else
						 RetVal = VAL_ERR_SPEC_ACCESS_MISMATCH;
					 break;
				 case SPEC_ACC_READ_LIMIT_HIGH:
					 if((Access & RLH) == RLH)
						 VarPtr = (UINT8*)PTable[ParamIndexInTable].MaxVal;
					 else
						 RetVal = VAL_ERR_SPEC_ACCESS_MISMATCH;
					 break;
				 default:
					 RetVal = VAL_ERR_SPEC_ACCESS_MISMATCH;
					 break;
	    	 }
	     }
	     else{
			 switch (ARRAYTYPE(PTable[ParamIndexInTable].ParamInfoExt))
			 {
				case ARRAYPS :
					 if(ArrayIndex > MAXIMUM_USER_PRESETS)
						RetVal = VAL_ERR_INDEX_MISMATCH;
					 else if(ArrayIndex == MAXIMUM_USER_PRESETS){
						VarPtr = (UINT8*)((UINT8*)&CommPS + ((UINT8*)PTable[ParamIndexInTable].ParamVal - (UINT8*)&ParameterSet[CurrPreset]));
//						if(EnableDebug) pprintf(" VarPtr %08X", VarPtr);
					 }
					 else
						VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (ArrayIndex * sizeof(DcxPreset));
					 break;
				case ARRAYHSPS :
					 if(ArrayIndex > MAXIMUM_USER_HSPRESETS)
						RetVal = VAL_ERR_INDEX_MISMATCH;
					 else if(ArrayIndex == MAXIMUM_USER_HSPRESETS)
						VarPtr =  (UINT8*)((UINT8*)&CommHSPS + ((UINT8*)PTable[ParamIndexInTable].ParamVal - (UINT8*)&HsParameterSet[CurrHsPreset]));
					 else
						VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (ArrayIndex * sizeof(HSPRESET));
					 break;
				case ARRAYSS :
					 if(ArrayIndex > MAXIMUM_USER_PRESETS)
						RetVal = VAL_ERR_INDEX_MISMATCH;
					 else if(ArrayIndex == MAXIMUM_USER_PRESETS)
						VarPtr = (UINT8*)((UINT8*)&CommWeldSS + ((UINT8*)PTable[ParamIndexInTable].ParamVal - (UINT8*)&WeldStatusSet[CurrPreset]));
					 else
						VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (ArrayIndex * sizeof(WSTATUS));
					 break;
				case ARRAYHSSS :
					 if(ArrayIndex > MAXIMUM_USER_HSPRESETS)
						RetVal = VAL_ERR_INDEX_MISMATCH;
					 else if(ArrayIndex == MAXIMUM_USER_HSPRESETS)//TODO:
						VarPtr = (UINT8*)((UINT8*)&CommHSSS + ((UINT8*)PTable[ParamIndexInTable].ParamVal - (UINT8*)&HsStatusSet[CurrHsPreset]));
					 else
						 VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (ArrayIndex * sizeof(HSSTATUS) * MAX_HS_FUNC);
					 break;
				case ARRAYSN:
					if(ArrayIndex > SERIALNUMBER_LEN)
						RetVal = VAL_ERR_INDEX_MISMATCH;
					else
						VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (ArrayIndex * sizeof(UINT8));
					break;
				case ARRAYNA:
//					 if(EnableDebug) pprintf("Non array read access");
					 if(ArrayIndex > 0)
						 RetVal = VAL_ERR_INDEX_MISMATCH;
					 else
						 VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal;
					 break;
				default:
				   RetVal = VAL_ERR_INVALID_VAL_NBR;
				   break;
			 }
	     }
     }

   if(EnableDebug) pprintf("\n %s %04X %d %d", __FUNCTION__, Access, RetVal, SepcAcc);
   if((RetVal == VAL_ERR_NO_ERR) && ((Access & RPRCB) == RPRCB) && (SepcAcc == SPEC_ACC_STANDARD))
   		RetVal = ParamAccessPreCallback(ParamNumber, Val, ArrayIndex, ACC_READ);

   if((RetVal == VAL_ERR_NO_ERR) && (ReadFromTable == TRUE)){
		  switch(PARATYPE(PTable[ParamIndexInTable].ParamInfo)){
			 case TYPE_8:
			 case TYPE_A_8:
				TempVal = (UINT32)VAL8(VarPtr);
				VAL32(Val) = TempVal;
				break;
			 case TYPE_16:
			 case TYPE_A_16:
				TempVal = (UINT32)VAL16(VarPtr);
				VAL32(Val) = TempVal;
				break;
			 case TYPE_32:
			 case TYPE_A_32:
				VAL32(Val) = VAL32(VarPtr);
				break;
			 case TYPE_S8:
			 case TYPE_A_S8:
				TempVal = (UINT32)VALS8(VarPtr);
				VAL32(Val) = TempVal;
				break;
			 case TYPE_S16:
			 case TYPE_A_S16:
				TempVal = (UINT32)VALS16(VarPtr);
				VAL32(Val) = TempVal;
				break;
			 case TYPE_S32:
			 case TYPE_A_S32:
				VAL32(Val) = (UINT32)VALS32(VarPtr);
				break;
			 default:
				RetVal = VAL_ERR_TYPE_MISMATCH;
		 }
   }
   if((RetVal == VAL_ERR_NO_ERR) && ((Access & RPSCB) == RPSCB))
    	RetVal =  ParamAccessPostCallback(ParamNumber, Val, ArrayIndex, ACC_READ);

    if(RetVal != VAL_ERR_NO_ERR){
//       if(EnableDebug) pprintf("\n read error %d ", RetVal);
    }
    return  RetVal;
}

/*Purpose: To validate the type, Index , special access, rights , min, max limit of parameter and to modify the parameter value. This function is called by HandleValueEthIP function if a parameter is accessed for write from SET_ATTRIBUTE_SINGLE Service as part of explicit mesaaging..
	Entry Condition: TokenAvailable- Token currently avaialbale or not to fieldbus status.
			Val- The pointer to the value to be written.
			Len- Pointer to the length of Val in bytes.
			InstanceNum- Index of array the parameter belogs to.
			Attribute- The unique parameter number.
	Exit Condition: Returns appropriate error code in case of error else returns EthIPStausCode success.
*/
EthIPStausCode WriteParamEthIP(BOOLEAN TokenAvailable, void * Val, UINT32 * Len, UINT32 InstanceNum, UINT32 Attribute)
{
//	  if(EnableDebug) pprintf("%s %d %d", __FUNCTION__,  InstanceNum, Attribute);
	EthIPStausCode RetVal;
	SINT32 ParamIndexInTable = 0;
	ParamAccessError AccErr;
	void * VarPtr = 0, *MinvalPtr = 0, *MaxvalPtr = 0;
	UINT32 TempVal = LE32_TO_HOST((UINT32) VAL32(Val)), TempVal8, TempVal16;
	UINT16 Access = 0;
	EnableLimitCheck = TRUE;
	RetVal.GSC = STAT_SUCCESS;
	RetVal.ESC = EXTSTAT_SUCCESS;

   ParamIndexInTable = GetParameterIndexInTable((UINT16)Attribute);
   if(ParamIndexInTable < 0)
      RetVal.GSC = STAT_INVALID_ATTRIBUTE;

	if (RetVal.GSC == STAT_SUCCESS) {
		if ((UINT32)ParamIndexInTable >= (sizeof(PTable) / sizeof(PTable[0])))//additional check
			RetVal.GSC = STAT_INVALID_ATTRIBUTE;
		else if (PARANUM(PTable[ParamIndexInTable].ParamInfo) != Attribute)
			RetVal.GSC = STAT_INVALID_ATTRIBUTE;
		else {
			Access = ACCRIGHTS(PTable[ParamIndexInTable].ParamInfoExt);
			if (((Access & WSAT) == WSAT) && (TokenAvailable == FALSE))
				RetVal.GSC = STAT_PRIVILEGE_VIOLATION;
		}
	}
	if (RetVal.GSC == STAT_SUCCESS) {
		switch (ARRAYTYPE(PTable[ParamIndexInTable].ParamInfoExt)) {
		case ARRAYPS:
			if (InstanceNum > MAXIMUM_USER_PRESETS)
				RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
			else
				VarPtr = (UINT8*) PTable[ParamIndexInTable].ParamVal + (InstanceNum * sizeof(DcxPreset));
			break;
		case ARRAYHSPS:
			if (InstanceNum > MAXIMUM_USER_HSPRESETS)
				RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
			else
				VarPtr = (UINT8*) PTable[ParamIndexInTable].ParamVal + (InstanceNum * sizeof(HSPRESET));
			break;
		case ARRAYSS:
		case ARRAYHSSS:
		case ARRAYSN:
			RetVal.GSC = STAT_ATTRIBUTE_NOT_SETTABLE;
			break;
		case ARRAYNA:
			if (InstanceNum > 0)
				RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
			else
				VarPtr = (UINT8*) PTable[ParamIndexInTable].ParamVal;
			if(EnableDebug) pprintf("\n Non array write access");
			break;
		default:
			RetVal.GSC = STAT_INVALID_ATTRIBUTE;
			break;
		}
	}
	if (((Access & WPRCB) == WPRCB) && (RetVal.GSC == STAT_SUCCESS)){
	   AccErr = ParamAccessPreCallback(Attribute, &TempVal, InstanceNum, ACC_WRITE, Len);
	   if(AccErr == VAL_ERR_NOT_WRITEABLE)
		  RetVal.GSC = STAT_ATTRIBUTE_NOT_SETTABLE;
	   else if(AccErr != VAL_ERR_NO_ERR)
	      RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
	}

	if ((RetVal.GSC == STAT_SUCCESS) && (EnableLimitCheck == TRUE)) {
	   if(*Len != ETHIP_PARAM_DATALEN)
	        RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
	   else{
         MinvalPtr = PTable[ParamIndexInTable].MinVal;
         MaxvalPtr = PTable[ParamIndexInTable].MaxVal;
         switch (PARATYPE(PTable[ParamIndexInTable].ParamInfo)) {
         case TYPE_8:
         case TYPE_A_8:
            TempVal8 = (TempVal & 0xFF);
            if ((MinvalPtr) && (TempVal8 < VAL8(MinvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else if ((MaxvalPtr) && (TempVal8 > VAL8(MaxvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else
               VAL8(VarPtr) = TempVal8;
            break;
         case TYPE_16:
         case TYPE_A_16:
            TempVal16 = (TempVal & 0xFFFF);
            if ((MinvalPtr) && (TempVal16 < VAL16(MinvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else if ((MaxvalPtr) && (TempVal16 > VAL16(MaxvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else
               VAL16(VarPtr) = TempVal16;
            break;
         case TYPE_32:
         case TYPE_A_32:
            if ((MinvalPtr) && (TempVal < VAL32(MinvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else if ((MaxvalPtr) && (TempVal > VAL32(MaxvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else
               VAL32(VarPtr) = TempVal;
            break;
         case TYPE_S8:
         case TYPE_A_S8:
            TempVal8 = (TempVal & 0xFF);
            if ((MinvalPtr) && ((SINT8) TempVal8 < VALS8(MinvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else if ((MaxvalPtr) && ((SINT8) TempVal8 > VALS8(MaxvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else
               VALS8(VarPtr) = (SINT8) TempVal8;
            break;
         case TYPE_S16:
         case TYPE_A_S16:
            TempVal16 = (TempVal & 0xFFFF);
            if ((MinvalPtr) && ((SINT16) TempVal16 < VALS16(MinvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else if ((MaxvalPtr) && ((SINT16) TempVal16 > VALS16(MaxvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else
               VALS16(VarPtr) = (SINT16) TempVal16;
            break;
         case TYPE_S32:
         case TYPE_A_S32:
            if ((MinvalPtr) && ((SINT32) TempVal < VALS32(MinvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else if ((MaxvalPtr) && ((SINT32) TempVal > VALS32(MaxvalPtr)))
               RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
            else
               VALS32(VarPtr) = (SINT32) TempVal;
            break;
         default:
            RetVal.GSC = STAT_VENDOR_SPECIFIC_ERROR;
            RetVal.ESC = EXTSTAT_VAL_ERR_MISC;
         }
	   }
	}
	if (((Access & WPSCB) == WPSCB) && (RetVal.GSC == STAT_SUCCESS)){
	   AccErr = ParamAccessPostCallback(Attribute, &TempVal, InstanceNum, ACC_WRITE, Len);
	   if(AccErr != VAL_ERR_NO_ERR)
	      RetVal.GSC = STAT_INVALID_PARAMETER_VALUE;
	}

	if (RetVal.GSC == STAT_SUCCESS) {
		if ((ARRAYTYPE(PTable[ParamIndexInTable].ParamInfoExt) == ARRAYPS) && (InstanceNum < MAXIMUM_USER_PRESETS)) {
			ParameterSet[InstanceNum].InfoParam.PresetStatus = NOTVERIFIED;
			CalculateAndSavePresetCRC((SAVECRC_CMD) (CRCCMD(PTable[ParamIndexInTable].ParamInfoExt) | SAVECRC_INFO),
			      PRESET_NUMBER(InstanceNum));
			if (InstanceNum == CurrPreset)
			    RegUpdateCmdFlag = TRUE;
			Log::WriteEventLog(EVENT_PARAMETER_CHANGED, Attribute, LE32_TO_HOST((UINT32) VAL32(Val)));
		} else if ((ARRAYTYPE(PTable[ParamIndexInTable].ParamInfoExt) == ARRAYHSPS) && (InstanceNum
		      < MAXIMUM_USER_HSPRESETS)){
			CalculateAndSavePresetCRC(CRCCMD(PTable[ParamIndexInTable].ParamInfoExt), (PRESET_NUMBER) 0, HSPRESET_NUMBER(
			      InstanceNum));
			if (InstanceNum == CurrHsPreset)
			    RegUpdateCmdFlag = TRUE;
			Log::WriteEventLog(EVENT_PARAMETER_CHANGED, Attribute, LE32_TO_HOST((UINT32) VAL32(Val)));
		}
	}

	if (RetVal.GSC != STAT_SUCCESS) {
		if(EnableDebug) pprintf("\n write error %d ", RetVal);
	}
	return RetVal;
}

EthIPStausCode ReadParamEthIP(void * Val, UINT32 * Len , UINT32 InstanceNum, UINT32 Attribute)
{
   SINT32 ParamIndexInTable = 0;
   UINT32 TempVal;
   void * VarPtr = 0;
   EthIPStausCode RetVal;
   RetVal.GSC = STAT_SUCCESS;
   RetVal.ESC = EXTSTAT_SUCCESS;
   ParamAccessError AccErr;
   ReadFromTable = TRUE;
   UINT16 Access = 0;

   ParamIndexInTable = GetParameterIndexInTable((UINT16)Attribute);
   if(ParamIndexInTable < 0)
	  RetVal.GSC = STAT_INVALID_ATTRIBUTE;

   if (RetVal.GSC == STAT_SUCCESS){
	   if((UINT32)ParamIndexInTable >= (sizeof(PTable)/sizeof(PTable[0])))//additional check
	   	RetVal.GSC = STAT_INVALID_ATTRIBUTE;
	   else if(PARANUM(PTable[ParamIndexInTable].ParamInfo) != Attribute)
	   	RetVal.GSC = STAT_INVALID_ATTRIBUTE;
	   else
		   Access = ACCRIGHTS(PTable[ParamIndexInTable].ParamInfoExt);
   }
   if(RetVal.GSC == STAT_SUCCESS){
	     	 switch (ARRAYTYPE(PTable[ParamIndexInTable].ParamInfoExt))
			 {
				case ARRAYPS :
					 if(InstanceNum > MAXIMUM_USER_PRESETS)
						 RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
					 else
						VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (InstanceNum * sizeof(DcxPreset));
					 break;
				case ARRAYHSPS :
					 if(InstanceNum > MAXIMUM_USER_HSPRESETS)
						 RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
					 else
						VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (InstanceNum * sizeof(HSPRESET));
					 break;
				case ARRAYSS :
					 if(InstanceNum > MAXIMUM_USER_PRESETS)
						 RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
					 else
						VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (InstanceNum * sizeof(WSTATUS));
					 break;
				case ARRAYHSSS :
					 if(InstanceNum > MAXIMUM_USER_HSPRESETS)
						 RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
					 else
						 VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (InstanceNum * sizeof(HSSTATUS) * MAX_HS_FUNC);
					 break;
				case ARRAYSN:
					if(InstanceNum > 19)
						RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
					else
						VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal + (InstanceNum * sizeof(UINT8));
					break;
				case ARRAYNA:
					 if(InstanceNum > 0)
						 RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
					 else
						 VarPtr = (UINT8*)PTable[ParamIndexInTable].ParamVal;
					 break;
				default:
					RetVal.GSC = STAT_INVALID_ATTRIBUTE;
				   break;
			 }
     }

   *Len = ETHIP_PARAM_DATALEN;
   if((RetVal.GSC == STAT_SUCCESS) && ((Access & RPRCB) == RPRCB)){
      AccErr = ParamAccessPreCallback(Attribute, Val, InstanceNum, ACC_READ, Len);
      if(AccErr != VAL_ERR_NO_ERR)
         RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
   }

   if((RetVal.GSC == STAT_SUCCESS) && (ReadFromTable == TRUE)){
		  switch(PARATYPE(PTable[ParamIndexInTable].ParamInfo)){
			 case TYPE_8:
			 case TYPE_A_8:
				TempVal = (UINT32)VAL8(VarPtr);
				VAL32(Val) = HOST_TO_LE32(TempVal);
				break;
			 case TYPE_16:
			 case TYPE_A_16:
				TempVal = (UINT32)VAL16(VarPtr);
				VAL32(Val) = HOST_TO_LE32(TempVal);
				break;
			 case TYPE_32:
			 case TYPE_A_32:
				VAL32(Val) = HOST_TO_LE32(VAL32(VarPtr));
				break;
			 case TYPE_S8:
			 case TYPE_A_S8:
				TempVal = (UINT32)VALS8(VarPtr);
				VAL32(Val) = HOST_TO_LE32(TempVal);
				break;
			 case TYPE_S16:
			 case TYPE_A_S16:
				TempVal = (UINT32)VALS16(VarPtr);
				VAL32(Val) = HOST_TO_LE32(TempVal);
				break;
			 case TYPE_S32:
			 case TYPE_A_S32:
				VAL32(Val) = HOST_TO_LE32((UINT32)VALS32(VarPtr));
				break;
			 default:
				RetVal.GSC = STAT_VENDOR_SPECIFIC_ERROR;
				RetVal.ESC = EXTSTAT_VAL_ERR_MISC;
		 }
   }
   if(((SINT32)RetVal.GSC == (SINT32)VAL_ERR_NO_ERR) && ((Access & RPSCB) == RPSCB)){
    	AccErr = ParamAccessPostCallback(Attribute, Val, InstanceNum, ACC_READ);
    	if(AccErr != VAL_ERR_NO_ERR)
    	       RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
   }

    if(RetVal.GSC != STAT_SUCCESS){
       *Len = 0;
       if(EnableDebug) pprintf("\n read error %d ", RetVal.GSC);
    }
    return  RetVal;
}

/* ParamAccessError ParamAccessPreCallback(UINT16 ParamNumber, void * Val, SINT32 Indx, UINT8 RW)
 *
 *  Purpose:
 *    This function is called by ReadParam and WriteParam functions if the parameter access
 *    requires special handling before access.
 *
 *  Entry condition:
 *   ParamNumber:Unique parameter number for which the pre call back is called.
 *   Val: Pointer to parameter value.
 *   Indx: Array index for the parameters belongs to an array.
 *   RW:To differentiate between read and write pre call back.
 *
 *  Exit condition:
 *   VAL_ERR_NO_ERR if no error else the error code.
 */
ParamAccessError ParamAccessPreCallback(UINT16 ParamNumber, void * Val, SINT32 Indx, UINT8 RW, UINT32 * Len)
{
//   if(EnableDebug) printf("\n %s %d %d %d ", __FUNCTION__, VAL32(Val), Indx, RW);
   ParamAccessError Err = VAL_ERR_NO_ERR;
   UINT32 TempVal;
   UINT32 referenceForRounding = 10000;
   UINT32 remainderForRoundingUp = 5;
   UINT32 divisorOfReference = 10;
   UINT32 roundingUpOffset = 10;
   SINT32 TempSVal;
   static RTCTime TM = {0,0,0,0,0,0,0};
   DcxPreset * PSPtr = 0;
   HSPRESET * HspsPtr = 0;
   
   if((ParamNumber >= PARA_SET_BASE) && (ParamNumber <= PARA_SET_LAST)){
      if(Indx == MAXIMUM_USER_PRESETS)
         PSPtr = &CommPS;
      else
         PSPtr = &ParameterSet[Indx];
   }
   else if((ParamNumber >= HSPARA_SET_BASE) && (ParamNumber <= HSPARA_SET_LAST)){
        if(Indx == MAXIMUM_USER_HSPRESETS)
           HspsPtr = &CommHSPS;
        else
           HspsPtr = &HsParameterSet[Indx];
    }
   //Date time should be read one after other
   switch(ParamNumber){
   case FB_PARAM_FPGAVERSIONLABEL://Fpga version level
      if((RW == ACC_READ) && (FieldBusType == HW_ASSEMBLY_ETHERNETIP)){
         strcpy((SINT8*)Val , PowerUpData.FPGAVersionLabel);
         *Len = strlen(PowerUpData.FPGAVersionLabel);
         ReadFromTable = FALSE;
      }
      break;
   case FB_PARAM_BOOTLOADERVERSIONLABEL://DCP Bootloader version
      if((RW == ACC_READ) && (FieldBusType == HW_ASSEMBLY_ETHERNETIP)){
            strcpy((SINT8*)Val , PowerUpData.BootLoaderVersionLabel);
            *Len = strlen(PowerUpData.BootLoaderVersionLabel);
            //*Len = 0;
            ReadFromTable = FALSE;
       }
      break;
   case FB_PARAM_SWVERSIONLABEL://DCP Software version
      if((RW == ACC_READ) && (FieldBusType == HW_ASSEMBLY_ETHERNETIP)){
         strcpy((SINT8*)Val , PowerUpData.SWVersionLabel);
         *Len = strlen(PowerUpData.SWVersionLabel);
         ReadFromTable = FALSE;
      }
      break;
   case FB_PARAM_WCBOOTLOADERVERSIONLABEL://WC Bootloader version
      if((RW == ACC_READ) && (FieldBusType == HW_ASSEMBLY_ETHERNETIP)){
         strcpy((SINT8*)Val , WCBootLoaderVersionLabel);
         *Len = strlen(WCBootLoaderVersionLabel);
         ReadFromTable = FALSE;
      }
      break;
   case FB_PARAM_WCSWVERSIONLABEL://WC Bootloader version
       if((RW == ACC_READ) && (FieldBusType == HW_ASSEMBLY_ETHERNETIP)){
          strcpy((SINT8*)Val , SWVersionLabel);
          *Len = strlen(SWVersionLabel);
          ReadFromTable = FALSE;
       }
       break;
   case FB_PARAM_SERIALNO:
      if((RW == ACC_READ) && (FieldBusType == HW_ASSEMBLY_ETHERNETIP)){
         strcpy((SINT8*)Val , (SINT8*)MacchipData.SerialNo);
         *Len = strlen((SINT8*)MacchipData.SerialNo);
         ReadFromTable = FALSE;
      }
      break;
   case RTC_DATE:
      if(RW == ACC_READ){
         if(RTClock::GetCurrentTime(&TM)){
            RTDate.Date.Date = TM.Day;
            RTDate.Date.WeekDay = TM.Weekdays;
            RTDate.Date.Month = TM.Century_Month;
            RTDate.Date.Year = TM.Years;
         }
         else
            Err = VAL_ERR_MISC;
      }
      break;
   case RTC_TIME:
      if(RW == ACC_READ){
         RTTime.Time.Hour = TM.Hour;
         RTTime.Time.Minute = TM.Minute;
         RTTime.Time.Second = TM.Second;
         RTTime.Time.TenMilliSecond = 0;
      }
      break;
      
//*************************************************************************************     
//*************************************************************************************
//*************************************************************************************
// ATTRIBUTE ID = 1062
// Name = Energy      
      
   case FB_PARAM_ENERGYCP://Energy CP special handling for rounding the value
       if(RW == ACC_WRITE){
             TempVal = VAL32(Val);
             if(TempVal > referenceForRounding){
                  if((TempVal % divisorOfReference) >= remainderForRoundingUp){
                      TempVal += (roundingUpOffset - (TempVal % divisorOfReference));
                  }
                  else{
                    TempVal -= (TempVal % divisorOfReference);
                  }
                  VAL32(Val) = TempVal;
             }
       }
       break;
//*************************************************************************************     
//*************************************************************************************
//*************************************************************************************

   case FB_PARAM_PEAKPOWERCP://Peak power CP special handling
      if (RW == ACC_READ) {
            ReadFromTable = FALSE;
            TempVal = (PSPtr->WcParam.PeakPowerCP / 10);
            if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
               TempVal = HOST_TO_LE32(TempVal);
            VAL32(Val) = TempVal;
      }
      else if (RW == ACC_WRITE){
         TempVal = VAL32(Val);
         TempVal = TempVal * 10;
         VAL32(Val) = TempVal;
      }
      break;
   case FB_PARAM_GNDDTIME://ground detect time
//      if(RW == ACC_READ){
//         if(PSPtr->WcParam.ScrubTimeFlag == FALSE){
//            ReadFromTable = FALSE;
//            VAL32(Val) =  (UINT32)0;
//         }
//      }
//      else if(RW == ACC_WRITE){
//         if(VAL32(Val) == 0){
//            EnableLimitCheck = FALSE;
//            PSPtr->WcParam.ScrubTimeFlag = FALSE;
//         }
//      }
      break;
   case FB_PARAM_PEAKPOWERRAW: //Peak Power raw weld
      if(RW == ACC_READ){
    	 ReadFromTable = FALSE;
		 TempSVal = HOST_TO_LE32(WeldStatusSet[CurrPreset].Wstat.PeakPower * MacchipData.Power / 100);
		 VAL32(Val) = TempSVal;
      }
      break;
   case FB_PARAM_TESTPEAKPOWERRAW: //Peak Power raw test
      if(RW == ACC_READ){
     	 UINT16 HsPsNum = ParameterSet[Sysconfig->ActivePreset].InfoParam.HornPresetNumber;
    	 ReadFromTable = FALSE;
		 TempSVal = HOST_TO_LE32(HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.PeakPower * MacchipData.Power / 100);
		 VAL32(Val) = TempSVal;
      }
      break;
   case FB_PARAM_FREQOFFSET:
		if (RW == ACC_READ) {
			ReadFromTable = FALSE;
			TempSVal = (PSPtr->WcParam.FreqOffset / FREQ_SCALING_FACTOR);
			if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
				TempSVal = HOST_TO_LE32(TempSVal);
			VALS32(Val) = TempSVal;
		}
		else if (RW == ACC_WRITE){
		  TempSVal = VALS32(Val);
		  TempSVal = TempSVal * FREQ_SCALING_FACTOR;
		  VALS32(Val) = TempSVal;
		}
	  break;
   case FB_PARAM_SEEKFREQOFFSET://seek offset
		if (RW == ACC_READ) {
			ReadFromTable = FALSE;
			TempSVal = (HspsPtr->ParaSeek.FreqOffset / FREQ_SCALING_FACTOR);
			if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
				TempSVal = HOST_TO_LE32(TempSVal);
			VALS32(Val) = TempSVal;
		}
		else if (RW == ACC_WRITE){
		  TempSVal = VALS32(Val);
		  TempSVal = TempSVal * FREQ_SCALING_FACTOR;
		  VALS32(Val) = TempSVal;
		}
	   break;
   case FB_PARAM_TESTFREQOFFSET://test offset
		if (RW == ACC_READ) {
			ReadFromTable = FALSE;
			TempSVal = (HspsPtr->ParaTest.FreqOffset / FREQ_SCALING_FACTOR);
			if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
				TempSVal = HOST_TO_LE32(TempSVal);
			VALS32(Val) = TempSVal;
		}
		else if (RW == ACC_WRITE){
		  TempSVal = VALS32(Val);
		  TempSVal = TempSVal * FREQ_SCALING_FACTOR;
		  VALS32(Val) = TempSVal;
		}
	   break;
   case FB_PARAM_HOLDTIMEFLAG://Hold Time
      break;
   case FB_PARAM_TIMEERRORHIGH://Time Error High (CutOff)
      if(RW == ACC_READ){
         if((PSPtr->WcParam.CutOffEnabled == FALSE) || (PSPtr->WcParam.TimeCutOffFlag == FALSE)){
            ReadFromTable = FALSE;
            VAL32(Val) =  (UINT32)0;
         }
      }
      else if(RW == ACC_WRITE){
         if(VAL32(Val) == 0){
            EnableLimitCheck = FALSE;
            PSPtr->WcParam.TimeCutOffFlag = FALSE;
         }
      }
      break;
      
      
//*************************************************************************************     
//*************************************************************************************

// ATTRIBUTE ID = 1069
// Name = Amplitude Profile Energy
// Step@Energy Requires Special Handling for Rounding Before Access
      
   case FB_PARAM_AMPLITUDEPROFILEENERGY:          
       if(RW == ACC_WRITE){
             TempVal = VAL32(Val);
             if(TempVal > referenceForRounding){
                  if((TempVal % divisorOfReference) >= remainderForRoundingUp){
                     TempVal += (roundingUpOffset - (TempVal % divisorOfReference));
                  }
                  else{
                     TempVal -= (TempVal % divisorOfReference);
                  }
                  VAL32(Val) =  TempVal;
             }
        }
	   break;
//*************************************************************************************     
//*************************************************************************************
    
      
//*************************************************************************************
// ATTRIBUTE ID = 1087
// Name = Energy Error High (CutOff)      
// Energy Error High (CutOff)  Requires Special Handling for Rounding Before Access  

   case FB_PARAM_ENERGYERRORHIGH:
      if(RW == ACC_READ){
         if((PSPtr->WcParam.CutOffEnabled == FALSE) || (PSPtr->WcParam.EnergyCutoffFlag == FALSE)){
            ReadFromTable = FALSE;
            VAL32(Val) =  (UINT32)0;
         }
      }
      else if(RW == ACC_WRITE){
         TempVal = VAL32(Val);
         if(TempVal > referenceForRounding){
              if((TempVal % divisorOfReference) >= remainderForRoundingUp){
                 TempVal += (roundingUpOffset - (TempVal % divisorOfReference));
              }
              else{
                 TempVal -= (TempVal % divisorOfReference);
              }
              VAL32(Val) = TempVal;
         }
         else if(TempVal == 0){
            EnableLimitCheck = FALSE;
            PSPtr->WcParam.EnergyCutoffFlag = FALSE;
         }
      }
      break;
//*************************************************************************************     
//*************************************************************************************

   
   
   case FB_PARAM_PEAKPOWERERRORHIGH://Peak Power Error High (CutOff)
      if(RW == ACC_READ){
         if((PSPtr->WcParam.CutOffEnabled == FALSE) || (PSPtr->WcParam.PeakPowerCutoffFlag == FALSE)){
            ReadFromTable = FALSE;
            VAL32(Val) =  (UINT32)0;
         }
      }
      else if(RW == ACC_WRITE){
         if(VAL32(Val) == 0){
            EnableLimitCheck = FALSE;
            PSPtr->WcParam.PeakPowerCutoffFlag = FALSE;
         }
      }
      break;
   case FB_PARAM_TIMEERRORWINDOWLOW://Time Error Window Low
      if(RW == ACC_READ){
         if(((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLE) != CONTROL_LIMITS_ENABLE)
            || ((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_TIMEMLR) != CONTROL_LIMITS_TIMEMLR)){
            VAL32(Val) =  (UINT32)0;
            ReadFromTable = FALSE;
         }
      }
      else if(RW == ACC_WRITE){
         if(VAL32(Val) == 0){
            EnableLimitCheck = FALSE;
            PSPtr->LimitsParam.ControlLimits &= ~CONTROL_LIMITS_TIMEMLR;
         }
      }
      break;
   case FB_PARAM_TIMEERRORWINDOWHIGH://Time Error Window High
      if(RW == ACC_READ){
         if(((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLE) != CONTROL_LIMITS_ENABLE)
            || ((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_TIMEPLR) != CONTROL_LIMITS_TIMEPLR)){
            ReadFromTable = FALSE;
            VAL32(Val) =  (UINT32)0;
         }
      }
      else if(RW == ACC_WRITE){
         if(VAL32(Val) == 0){
            EnableLimitCheck = FALSE;
            PSPtr->LimitsParam.ControlLimits &= ~CONTROL_LIMITS_TIMEPLR;
         }
      }
      break;
      
// **********************************************************************************************************  
// **********************************************************************************************************  

// ATTRIBUTE ID = 1091      
// Name = -ENERGY LIMIT      
// - Energy Limit Requires Special Handling for Rounding Before Access
      
   case FB_PARAM_ENERGYERRORWINDOWLOW://Energy Error Window Low 
      if(RW == ACC_READ){
         if(((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLE) != CONTROL_LIMITS_ENABLE)
            || ((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_ENERGYMLR) != CONTROL_LIMITS_ENERGYMLR)){
            ReadFromTable = FALSE;
            VAL32(Val) =  (UINT32)0;
         }
      }
      else if(RW == ACC_WRITE){
         TempVal = VAL32(Val);
         if(TempVal > referenceForRounding){
              if((TempVal % divisorOfReference) >= remainderForRoundingUp){
                 TempVal += (roundingUpOffset - (TempVal % divisorOfReference));
              }
              else{
                 TempVal -= (TempVal % divisorOfReference);
              }
              VAL32(Val) = TempVal;
         }
         else if(TempVal == 0){
            EnableLimitCheck = FALSE;
            PSPtr->LimitsParam.ControlLimits &= ~CONTROL_LIMITS_ENERGYMLR;
         }
      }
      break;

      
// **********************************************************************************************************  
// **********************************************************************************************************  
// ATTRIBUTE ID = 1092
// Name = + ENERGY LIMIT
// + ENERGY LIMIT Requires Special Handling for Rounding Before Access
      
   case FB_PARAM_ENERGYERRORWINDOWHIGH://Energy Error Window High   
      if(RW == ACC_READ){
         if(((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLE) != CONTROL_LIMITS_ENABLE)
            || ((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_ENERGYPLR) != CONTROL_LIMITS_ENERGYPLR)){
            ReadFromTable = FALSE;
            VAL32(Val) =  (UINT32)0;
         }
      }
      else if(RW == ACC_WRITE){
         TempVal = VAL32(Val);
         if(TempVal > referenceForRounding){
              if((TempVal % divisorOfReference) >= remainderForRoundingUp){
                 TempVal += (roundingUpOffset - (TempVal % divisorOfReference));
              }
              else{
                 TempVal -= (TempVal % divisorOfReference);
              }
              VAL32(Val) = TempVal;
         }
         else if(TempVal == 0){
            EnableLimitCheck = FALSE;
            PSPtr->LimitsParam.ControlLimits &= ~CONTROL_LIMITS_ENERGYPLR;
         }
      }
      break;
      
// **********************************************************************************************************   
// **********************************************************************************************************  
   
   case FB_PARAM_PEAKPOWERERRORWINDOWLOW://Peak Power Error Window Low
      if(RW == ACC_READ){
         if(((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLE) != CONTROL_LIMITS_ENABLE)
            || ((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_POWERMLR) != CONTROL_LIMITS_POWERMLR)){
            ReadFromTable = FALSE;
            VAL32(Val) =  (UINT32)0;
         }
      }
      else if(RW == ACC_WRITE){
         if(VAL32(Val) == 0){
            EnableLimitCheck = FALSE;
            PSPtr->LimitsParam.ControlLimits &= ~CONTROL_LIMITS_POWERMLR;
         }
      }
      break;
   case FB_PARAM_PEAKPOWERERRORWINDOWHIGH://Peak Power Error Window High
      if(RW == ACC_READ){
         if(((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLE) != CONTROL_LIMITS_ENABLE)
            || ((PSPtr->LimitsParam.ControlLimits & CONTROL_LIMITS_POWERPLR) != CONTROL_LIMITS_POWERPLR)){
            ReadFromTable = FALSE;
            VAL32(Val) =  (UINT32)0;
         }
      }
      else if(RW == ACC_WRITE){
         if(VAL32(Val) == 0){
            EnableLimitCheck = FALSE;
            PSPtr->LimitsParam.ControlLimits &= ~CONTROL_LIMITS_POWERPLR;
         }
      }
      break;
   case FB_PARAM_FREQUENCYERRORLOW://Frequency Error Low (Cutoff) relative
      if(RW == ACC_READ){
         ReadFromTable = FALSE;
         if((PSPtr->WcParam.CutOffEnabled == FALSE) || (PSPtr->WcParam.WeldFreqLowCutOffFlag == FALSE)){
            VAL32(Val) =  (UINT32)0;
         }
         else{
            VAL32(Val) = (UINT32)(PSPtr->WcParam.WeldFreqLowCutoff / FREQ_SCALING_FACTOR);
            if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
               VAL32(Val) = HOST_TO_LE32(VAL32(Val));
         }
      }
      else if(RW == ACC_WRITE){
         if(VAL32(Val) == 0){
            EnableLimitCheck = FALSE;
            PSPtr->WcParam.WeldFreqLowCutOffFlag = FALSE;
         }
         else{
            TempVal = VAL32(Val);
            TempVal = (TempVal * FREQ_SCALING_FACTOR);
            VAL32(Val) = TempVal;
         }
      }
      break;
   case FB_PARAM_FREQUENCYERRORHIGH://   Frequency Error High (Cutoff) relative
      if(RW == ACC_READ){
         ReadFromTable = FALSE;
         if((PSPtr->WcParam.CutOffEnabled == FALSE) || (PSPtr->WcParam.WeldFreqHighCutOffFlag == FALSE)){
            VAL32(Val) =  (UINT32)0;
         }
         else{
            VAL32(Val) = (UINT32)((PSPtr->WcParam.WeldFreqHighCutoff) / FREQ_SCALING_FACTOR);
            if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
                 VAL32(Val) = HOST_TO_LE32(VAL32(Val));
         }
      }
      else if(RW == ACC_WRITE){
         if(VAL32(Val) == 0){
            EnableLimitCheck = FALSE;
            PSPtr->WcParam.WeldFreqHighCutOffFlag = FALSE;
         }
         else{
            TempVal = VAL32(Val);
            TempVal = (TempVal * FREQ_SCALING_FACTOR);
            VAL32(Val) = TempVal;
         }
      }
      break;
   case FB_PARAM_CUSTOMAIN1CUTOFFFLAG:
	  if(RW == ACC_READ){
		 if((PSPtr->WcParam.CutOffEnabled == FALSE) || (PSPtr->ActParam.CustomAIn1CutoffFlag == FALSE)
				 || (CustomInput1Configured == FALSE)){
			ReadFromTable = FALSE;
			VAL32(Val) =  (UINT32)0;
		 }
	   }
	   else if(RW == ACC_WRITE){
		   if(CustomInput1Configured == FALSE)
			   Err = VAL_ERR_NOT_WRITEABLE;
		   else if(VAL32(Val) == 0)
			   PSPtr->ActParam.CustomAIn1CutoffFlag = FALSE;
	   }
	   break;
   case FB_PARAM_CUSTOMAIN2CUTOFFFLAG:
	   if(RW == ACC_READ){
		 if((PSPtr->WcParam.CutOffEnabled == FALSE) || (PSPtr->ActParam.CustomAIn2CutoffFlag == FALSE)
				 || (CustomInput2Configured == FALSE)){
			ReadFromTable = FALSE;
			VAL32(Val) =  (UINT32)0;
		 }
	   }
	   else if(RW == ACC_WRITE){
		   if(CustomInput2Configured == FALSE)
			   Err = VAL_ERR_NOT_WRITEABLE;
		   else if(VAL32(Val) == 0)
			   PSPtr->ActParam.CustomAIn2CutoffFlag = FALSE;
	   }
	   break;
   case FB_PARAM_FBFREQOFFSET:
      if(RW == ACC_WRITE){
         TempSVal = (SINT32)VALS32(Val);
         //verify the frequency specific range
         if (TempSVal > (FbMaxPs.WcParam.FreqOffset / FREQ_SCALING_FACTOR))
            Err = VAL_ERR_LIMIT_HIGH;
         else if(TempSVal < (FbMinPs.WcParam.FreqOffset / FREQ_SCALING_FACTOR))
            Err = VAL_ERR_LIMIT_LOW;
         else
            ExternalControl.ExtFrequencyOffset = (SINT16)TempSVal;

         EnableLimitCheck = FALSE;
      }
      break;
   case FB_PARAM_EXTAMPLITUDESET:
      if(RW == ACC_WRITE){
         TempSVal = (SINT32)VALS32(Val);
         if (TempSVal < AMP_IN_MIN)
            Err = VAL_ERR_LIMIT_LOW;
          else if (TempSVal > AMP_IN_MAX)
            Err = VAL_ERR_LIMIT_HIGH;
          else
             ExternalControl.ExtAmplitudeSet = (SINT16)TempSVal;

         EnableLimitCheck = FALSE;
      }
      break;
   case FB_PARAM_SEEKFREQLOW:
      if(RW == ACC_READ){
          ReadFromTable = FALSE;
          VAL32(Val) = (UINT32)((HspsPtr->ParaSeek.SeekFreqLow ) / FREQ_SCALING_FACTOR);
          if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
             VAL32(Val) = HOST_TO_LE32(VAL32(Val));
       }
       else if(RW == ACC_WRITE){
             TempVal = VAL32(Val);
             TempVal = (TempVal * FREQ_SCALING_FACTOR);
             VAL32(Val) = TempVal;
       }
      break;
   case FB_PARAM_SEEKFREQHIGH:
      if(RW == ACC_READ){
          ReadFromTable = FALSE;
          VAL32(Val) = (UINT32)((HspsPtr->ParaSeek.SeekFreqHigh) / FREQ_SCALING_FACTOR);
          if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
             VAL32(Val) = HOST_TO_LE32(VAL32(Val));
       }
       else if(RW == ACC_WRITE){
             TempVal = VAL32(Val);
             TempVal = (TempVal * FREQ_SCALING_FACTOR);
             VAL32(Val) = TempVal;
       }
      break;
   case FB_PARAM_WELDFREQLOW:
      if(RW == ACC_READ){
          ReadFromTable = FALSE;
          VAL32(Val) = (UINT32)((HspsPtr->ParaTest.WeldFreqLow ) / FREQ_SCALING_FACTOR);
          if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
             VAL32(Val) = HOST_TO_LE32(VAL32(Val));
       }
       else if(RW == ACC_WRITE){
             TempVal = VAL32(Val);
             TempVal = (TempVal * FREQ_SCALING_FACTOR);
             VAL32(Val) = TempVal;
       }
      break;
   case FB_PARAM_WELDFREQHIGH:
      if(RW == ACC_READ){
          ReadFromTable = FALSE;
          VAL32(Val) = (UINT32)((HspsPtr->ParaTest.WeldFreqHigh) / FREQ_SCALING_FACTOR);
          if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
             VAL32(Val) = HOST_TO_LE32(VAL32(Val));
       }
       else if(RW == ACC_WRITE){
             TempVal = VAL32(Val);
             TempVal = (TempVal * FREQ_SCALING_FACTOR);
             VAL32(Val) = TempVal;
       }
      break;
   case FB_PARAM_FREQUENCYSTART:
      if(RW == ACC_READ){
          ReadFromTable = FALSE;
          VAL32(Val) = (UINT32)(HspsPtr->ParaScan.FrequencyStart / SCANFREQFACTOR);
          if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
             VAL32(Val) = HOST_TO_LE32(VAL32(Val));
       }
       else if(RW == ACC_WRITE){
             TempVal = VAL32(Val);
             TempVal = (TempVal * SCANFREQFACTOR);
             VAL32(Val) = TempVal;
       }
      break;
   case FB_PARAM_FREQUENCYSTOP:
      if(RW == ACC_READ){
          ReadFromTable = FALSE;
          VAL32(Val) = (UINT32)(HspsPtr->ParaScan.FrequencyStop / SCANFREQFACTOR);
          if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
             VAL32(Val) = HOST_TO_LE32(VAL32(Val));
       }
       else if(RW == ACC_WRITE){
             TempVal = VAL32(Val);
             TempVal = (TempVal * SCANFREQFACTOR);
             VAL32(Val) = TempVal;
       }
      break;
   case FB_PARAM_CURRCYCLETIME:
	   if(RW == ACC_READ){
			ReadFromTable = FALSE;
			VAL32(Val) = (UINT32)((MFcycleResults->CurrCycleTime * WCTIME) / 3600000000u);
			if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
			   VAL32(Val) = HOST_TO_LE32(VAL32(Val));
		}
	   break;
   case FB_PARAM_SONICSONTIME:
	   if(RW == ACC_READ){
			ReadFromTable = FALSE;
			VAL32(Val) = (UINT32)(MFcycleResults->SonicsOnTime / 3600000);
			if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
			   VAL32(Val) = HOST_TO_LE32(VAL32(Val));
		 }
	   break;
   case FB_PARAM_DIGITALTUNE:
		if(RW == ACC_READ){
			ReadFromTable = FALSE;
			TempSVal = SCALE_DOWN_FOR_N_BITS(HspsPtr->DigitalTuneScaledUP) / FREQ_SCALING_FACTOR;
			if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
				TempSVal = HOST_TO_LE32(TempSVal);
			VALS32(Val) = TempSVal;
		}
		else if (RW == ACC_WRITE){
			TempSVal = VALS32(Val);
			TempSVal = SCALE_UP_FOR_N_BITS(TempSVal) * FREQ_SCALING_FACTOR;
			VALS32(Val) = TempSVal;
		}
	   break;
   default:
//      if(EnableDebug) pprintf(" Read/write pre cb not available");
      Err = VAL_ERR_INVALID_VAL_NBR;
   };

   return Err;
}

/* ParamAccessError ParamAccessPostCallback(UINT16 ParamNumber, void * Val, SINT32 Indx, UINT8 RW)
 *
 *  Purpose:
 *    This function is called by ReadParam and WriteParam functions if the parameter access
 *    requires special handling after access.
 *
 *  Entry condition:
 *    ParamNumber:Unique parameter number for which the pre call back is called.
 *    Val: Pointer to parameter value.
 *    Indx: Array index for the parameters belongs to an array.
 *    RW:To differentiate between read and write pre call back.
 *
 *  Exit condition:
 *    VAL_ERR_NO_ERR if no error else the error code.
 */
ParamAccessError ParamAccessPostCallback(UINT16 ParamNumber, void * Val, SINT32 Indx, UINT8 RW, UINT32 * Len)
{
   static RTCTime TM = {0,0,0,0,0,0,0};
   ParamAccessError Err = VAL_ERR_NO_ERR;
   DcxPreset * PSPtr = 0;
   HSPRESET * HspsPtr = 0;
   Len = 0;
   //Date time should be written one after other
   if((ParamNumber >= PARA_SET_BASE) && (ParamNumber <= PARA_SET_LAST)){
      if(Indx == MAXIMUM_USER_PRESETS)
         PSPtr = &CommPS;
      else
         PSPtr = &ParameterSet[Indx];
   }
   else if((ParamNumber >= HSPARA_SET_BASE) && (ParamNumber <= HSPARA_SET_LAST)){
         if(Indx == MAXIMUM_USER_HSPRESETS)
            HspsPtr = &CommHSPS;
         else
            HspsPtr = &HsParameterSet[Indx];
   }
   switch(ParamNumber){
   case RTC_DATE:
      if(RW == ACC_WRITE) {
         if((RTDate.Date.Year > 99)|| (RTDate.Date.Month < 1)|| (RTDate.Date.Month > 12)
          || (RTDate.Date.WeekDay > 6) || (RTDate.Date.Date < 1)
          || (RTDate.Date.Date > DaysInMonth(RTDate.Date.Month, RTDate.Date.Year)))
            Err = VAL_ERR_LIMIT_LOW;
      }
   break;
   case RTC_TIME:
      if(RW == ACC_WRITE) {
    	  if((RTDate.Date.Year > 99)|| (RTDate.Date.Month < 1)|| (RTDate.Date.Month > 12)
    	            || (RTDate.Date.WeekDay > 6) || (RTDate.Date.Date < 1)
    	            || (RTDate.Date.Date > DaysInMonth(RTDate.Date.Month, RTDate.Date.Year)))
            Err = VAL_ERR_LIMIT_LOW;
         else if((RTTime.Time.Hour > 23)|| (RTTime.Time.Minute > 59)|| (RTTime.Time.Second > 59)||
               (RTTime.Time.TenMilliSecond > 99))
            Err = VAL_ERR_LIMIT_HIGH;
         else
         {
            TM.Century_Month = RTDate.Date.Month;
            TM.Day = RTDate.Date.Date;
            TM.Weekdays = RTDate.Date.WeekDay;
            TM.Years = RTDate.Date.Year;
            TM.Minute = RTTime.Time.Minute;
            TM.Second = RTTime.Time.Second;
            TM.Hour = RTTime.Time.Hour;
            if(RTClock::SetCurrentTime(&TM) == FALSE)
               Err = VAL_ERR_MISC;
         }
      }
      break;
   case FB_PARAM_SCRUBTIMEFLAG:
//      if((RW == ACC_WRITE) && VAL32(Val) > 0){
//            PSPtr->WcParam.ScrubTimeFlag = TRUE;
//      }
      break;
   case FB_PARAM_HOLDTIMEFLAG://   Hold Time
//      if((RW == ACC_WRITE) && VAL32(Val) > 0){
//         PSPtr->WcParam.HoldTimeFlag = TRUE;
//      }
      break;
   case FB_PARAM_TIMEERRORHIGH://   Time Error High (CutOff)
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->WcParam.TimeCutOffFlag = TRUE;
         PSPtr->WcParam.CutOffEnabled = TRUE;
      }
      break;
   case FB_PARAM_ENERGYERRORHIGH://   Energy Error High (CutOff)
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->WcParam.EnergyCutoffFlag = TRUE;
         PSPtr->WcParam.CutOffEnabled = TRUE;
      }
      break;
   case FB_PARAM_PEAKPOWERERRORHIGH://   Peak Power Error High (CutOff)
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->WcParam.PeakPowerCutoffFlag = TRUE;
         PSPtr->WcParam.CutOffEnabled = TRUE;
      }
      break;
   case FB_PARAM_TIMEERRORWINDOWLOW://   Time Error Window Low
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->LimitsParam.ControlLimits |= (CONTROL_LIMITS_TIMEMLR | CONTROL_LIMITS_ENABLE);
      }
      break;
   case FB_PARAM_TIMEERRORWINDOWHIGH://   Time Error Window High
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->LimitsParam.ControlLimits |= (CONTROL_LIMITS_TIMEPLR | CONTROL_LIMITS_ENABLE);
      }
      break;
   case FB_PARAM_ENERGYERRORWINDOWLOW://   Energy Error Window Low
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->LimitsParam.ControlLimits |= (CONTROL_LIMITS_ENERGYMLR | CONTROL_LIMITS_ENABLE);
      }
      break;
   case FB_PARAM_ENERGYERRORWINDOWHIGH://   Energy Error Window High
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->LimitsParam.ControlLimits |= (CONTROL_LIMITS_ENERGYPLR | CONTROL_LIMITS_ENABLE);
      }
      break;
   case FB_PARAM_PEAKPOWERERRORWINDOWLOW://   Peak Power Error Window Low
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->LimitsParam.ControlLimits |= (CONTROL_LIMITS_POWERMLR | CONTROL_LIMITS_ENABLE);
      }
      break;
   case FB_PARAM_PEAKPOWERERRORWINDOWHIGH://   Peak Power Error Window High
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->LimitsParam.ControlLimits |= (CONTROL_LIMITS_POWERPLR | CONTROL_LIMITS_ENABLE);
      }
      break;
   case FB_PARAM_FREQUENCYERRORLOW://   Frequency Error Low (Cutoff)
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->WcParam.WeldFreqLowCutOffFlag = TRUE;
         PSPtr->WcParam.CutOffEnabled = TRUE;
      }
      break;
   case FB_PARAM_FREQUENCYERRORHIGH://   Frequency Error High (Cutoff)
      if((RW == ACC_WRITE) && VAL32(Val) > 0){
         PSPtr->WcParam.WeldFreqHighCutOffFlag = TRUE;
         PSPtr->WcParam.CutOffEnabled = TRUE;
      }
      break;
   case FB_PARAM_CUSTOMAIN1CUTOFFFLAG:
	  if((RW == ACC_WRITE) && VAL32(Val) > 0){
		 PSPtr->ActParam.CustomAIn1CutoffFlag = TRUE;
		 PSPtr->WcParam.CutOffEnabled = TRUE;
	  }
	   break;
   case FB_PARAM_CUSTOMAIN2CUTOFFFLAG:
	  if((RW == ACC_WRITE) && VAL32(Val) > 0){
		 PSPtr->ActParam.CustomAIn2CutoffFlag = TRUE;
		 PSPtr->WcParam.CutOffEnabled = TRUE;
	  }
	   break;
   case FB_PARAM_DIGITALTUNE:
   		if (RW == ACC_WRITE) {
   			HspsPtr->MemoryOffsetScaledUP = 0;
   		}
   		break;
   default:
//      if(EnableDebug) pprintf(" Read/write pre cb not available");
      Err = VAL_ERR_INVALID_VAL_NBR;
   };
   return Err;
}


