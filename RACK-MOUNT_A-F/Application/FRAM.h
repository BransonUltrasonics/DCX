/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FRAM.h_v   1.4   21 Mar 2016 16:44:52   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FRAM.h_v  $
 * 
 *    Rev 1.4   21 Mar 2016 16:44:52   hasanchez
 * Change the revision of FRAM included declaration of functions.
 * 
 *    Rev 1.3   03 Feb 2016 00:57:28   ygupta
 * FRAM_VERSION changed to 4.
 * 
 *    Rev 1.2   28 Jan 2016 01:53:18   ygupta
 * FRAM_VERSION changed to 3
 * 
 *    Rev 1.1   10 Aug 2015 09:40:30   RJamloki
 * Fram version changed to 2 for added variables in system configuration
 * 
 *    Rev 1.0   09 Jun 2015 09:10:34   tayars
 * Initial revision.
 * 
 *    Rev 1.2   08 Aug 2014 11:21:56   rjamloki
 * Removed InitFramPointer function.
 */
#ifndef FRAM_H_
#define FRAM_H_

#include "portable.h"
#include "DEF_DEF.h"
#include "TYP_DEF.h"
#include "SysConfig.h"
#include "LOG.h"
#include "UserIO.h"
#include "WeldResultHistory.h"
#include "preset.h"
#include "RTClock.h"
#include "PasscodeConfig.h"
#include "EthernetIP.h"

#define FRAM_VERSION						5			//to be changed with structure format
#define FRAM_START_ADDRESS					0x0b000000
#define FRAM_BOOTLOADER_RESERVED			0x10 // firsr 16 bytes are reserved for bootloader
#define FRAM_SIZE							0x20000
#define FRAM_SIZE_APP						(0x20000 - FRAM_BOOTLOADER_RESERVED)
#define FRAM_START_APP						(FRAM_START_ADDRESS + FRAM_BOOTLOADER_RESERVED)
#define INIT_VERSION						1
#define INIT_FRAM_INDEX						1

#define STATIC_ASSERT(COND, MSG) typedef char static_assertion_##MSG[(!!(COND))*2-1]
#define COMPILE_TIME_ASSERT3(X,L) STATIC_ASSERT(X,static_assertion_at_line_##L)
#define COMPILE_TIME_ASSERT2(X,L) COMPILE_TIME_ASSERT3(X,L)
#define COMPILE_TIME_ASSERT(X)    COMPILE_TIME_ASSERT2(X,__LINE__)

//Below defines are for Ver 6.0  official release code.
#define ADDR_VER6RLS_FRAMSTART			0x0b000010
#define ADDR_WC_SOFT_VERSION    		0x0b000410
#define ADDR_SYSCONFIG                  0x0b000448
#define ADDR_ALARM_CONFIG               0x0b0008f0
#define ADDR_USER_IO_CONFIG             0x0b000d24
#define ADDR_PASSCODE_CONFIG            0x0b001178
#define ADDR_DCX_PRESET                	0x0b0016e6
#define ADDR_DCX_HSPRESET               0x0b008da2
#define ADDR_MF_CYCLE_PARAM             0x0b00b516
#define ADDR_MF_CYCLE_RESULTS           0x0b00b92a
#define ADDR_COUNTERS               	0x0b00bd4a
#define ADDR_ADVCONFIG_DATA             0x0b00c15a
#define ADDR_WELDRESULT_INFO            0x0b00c590
#define ADDR_EVENT_LOG                  0x0b00c998
#define ADDR_ALARM_LOG                  0x0b0113d0
#define ADDR_LOG_INFO                	0x0b013180
#define ADDR_WELD_RESULTS               0x0b01358a
#define ADDR_STORED_FREQ                0x0b0142ea
#define ADDR_TPOINTS                	0x0b014746
#define ADDR_CRASHVALID                 0x0b01514a
#define ADDR_CRASHTASK                  0x0b01514e
#define ADDR_CRASHBUFFER                0x0b015166
#define ADDR_CRASH_CURRSTACKPTR         0x0b0191f2
#define ADDR_CRASH_CURRSTACKBEGIN       0x0b0191f6
#define ADDR_CRASH_ENDSTACKPTR          0x0b0191fa
#define ADDR_CRASH_BUFFPRINTLEN         0x0b0191fe
#define ADDR_CRASH_MMUAR                0x0b019202
#define ADDR_CRASH_FBRECVBUFF           0x0b019206
#define ADDR_CRASH_FBRECVMAILBOX        0x0b019246
#define ADDR_CRASH_SERRECVBUFF          0x0b019882
#define ADDR_CRASH_MBRECVBUFF           0x0b019a82
#define ADDR_LASTACD                    0x0b019b89
#define ADDR_INTSPEEDCONTROL            0x0b019bac
#define ADDR_QOSVALS                	0x0b019bb4
#define ADDR_DUMMY_PLACE_HOLDERRES      0x0b019bc0
#define ADDR_FRAM_INIT_DONE             0x0b01e746


//Below defines are for FRAM Version 1 code.
#define V1_INFO_START						FRAM_START_ADDRESS
#define V1_INFO_SIZE						256
#define V1_INFO_END							(V1_INFO_START + V1_INFO_SIZE - 1)

#define V1_SYSINFO_START					(V1_INFO_END + 1)
#define V1_SYSINFO_SIZE						256
#define V1_SYSINFO_END						(V1_SYSINFO_START + V1_SYSINFO_SIZE - 1)


#define V1_MFCYCRES_START					(V1_SYSINFO_END + 1)
#define V1_MFCYCRES_SIZE					128
#define V1_MFCYCRES_END						(V1_MFCYCRES_START + V1_MFCYCRES_SIZE - 1)

#define V1_ALARMCONFIG_START				(V1_MFCYCRES_END + 1)
#define V1_ALARMCONFIG_SIZE					256
#define V1_ALARMCONFIG_END					(V1_ALARMCONFIG_START + V1_ALARMCONFIG_SIZE - 1)


#define V1_USRIOCONFIG_START				(V1_ALARMCONFIG_END + 1)
#define V1_USRIOCONFIG_SIZE					128
#define V1_USRIOCONFIG_END					(V1_USRIOCONFIG_START + V1_USRIOCONFIG_SIZE - 1)


#define V1_PASSCODE_START					(V1_USRIOCONFIG_END + 1)
#define V1_PASSCODE_SIZE					512
#define V1_PASSCODE_END						(V1_PASSCODE_START + V1_PASSCODE_SIZE - 1)


#define V1_MFPARA_START						(V1_PASSCODE_END + 1)
#define V1_MFPARA_SIZE						128
#define V1_MFPARA_END						(V1_MFPARA_START + V1_MFPARA_SIZE - 1)


#define V1_PRESET_START						(V1_MFPARA_END + 1)
#define V1_PRESET_SIZE						32768
#define V1_PRESET_END						(V1_PRESET_START + V1_PRESET_SIZE - 1)

#define V1_SPARE1_START						(V1_PRESET_END + 1)
#define V1_SPARE1_SIZE						6144
#define V1_SPARE1_END						(V1_SPARE1_START + V1_SPARE1_SIZE - 1)


#define V1_HORN_PRESET_START				(V1_SPARE1_END + 1)
#define V1_HORN_PRESET_SIZE					8192
#define V1_HORN_PRESET_END					(V1_HORN_PRESET_START + V1_HORN_PRESET_SIZE - 1)

#define V1_ADVCONFIG_START					(V1_HORN_PRESET_END + 1)
#define V1_ADVCONFIG_SIZE					128
#define V1_ADVCONFIG_END					(V1_ADVCONFIG_START + V1_ADVCONFIG_SIZE - 1)

#define V1_SPARE2_START						(V1_ADVCONFIG_START + 1)
#define V1_SPARE2_SIZE						4096
#define V1_SPARE2_END						(V1_SPARE2_START + V1_SPARE2_SIZE - 1)


#define V1_WELDRES_START					(V1_SPARE2_END + 1)
#define V1_WELDRES_SIZE						4096
#define V1_WELDRES_END						(V1_WELDRES_START + V1_WELDRES_SIZE - 1)


#define V1_EVENT_START						(V1_WELDRES_END + 1)
#define V1_EVENT_SIZE						16384
#define V1_EVENT_END						(V1_EVENT_START + V1_EVENT_SIZE - 1)

#define V1_ALARM_START						(V1_EVENT_END + 1)
#define V1_ALARM_SIZE						8192
#define V1_ALARM_END						(V1_ALARM_START + V1_ALARM_SIZE - 1)


#define V1_SPARE3_START						(V1_ALARM_END + 1)
#define V1_SPARE3_SIZE						8192
#define V1_SPARE3_END						(V1_SPARE3_START + V1_SPARE3_SIZE - 1)

#define V1_ETHIP_START						 (V1_SPARE3_END + 1)
#define V1_ETHIP_SIZE						 56
#define V1_ETHIP_END						 (V1_ETHIP_START + V1_ETHIP_SIZE - 1)

#define V1_DEBUG_START						(V1_ETHIP_END + 1)
#define V1_DEBUG_SIZE						41160
#define V1_DEBUG_END						(V1_DEBUG_START + V1_DEBUG_SIZE - 1)

#if ((V1_INFO_SIZE+V1_SYSINFO_SIZE+V1_MFCYCRES_SIZE+V1_ALARMCONFIG_SIZE+V1_USRIOCONFIG_SIZE+V1_PASSCODE_SIZE \
		+V1_MFPARA_SIZE+V1_PRESET_SIZE+V1_SPARE1_SIZE+V1_HORN_PRESET_SIZE+V1_ADVCONFIG_SIZE+V1_SPARE2_SIZE+V1_WELDRES_SIZE+V1_EVENT_SIZE+V1_ALARM_SIZE+ \
		V1_SPARE3_SIZE+ V1_ETHIP_SIZE+V1_DEBUG_SIZE) != FRAM_SIZE)
#error FRAM allocation is incorrect for the physical size of FRAM !
#endif

//FRAM Segments
enum FRAM_SEG{
	FRAMINFO = 0,
	SYS_INFO ,
	MFG_RESULTS,
	ALARM_CONFIG,
	USERIO_CONFIG,
	PASSCODE_CONFIG,
	CCPARAM,
	PRESETS,
	SPARE1,
	HORN_PRESETS,
	ADV_CONFIG,
	SPARE2,
	HISTORY,
	EVENTS,
	ALARMS,
	SPARE3,
	ETHIPPARAMS,
	DEBUG,
	NUM_FRAM_SEGMENTS
};


struct SEGMENT_INFO
{
	UINT32			Addr;		// Address of where segment starts in FRAM
	UINT16			Size;		// Number of bytes allocated to this segment
	UINT32			Spare;		// Reserved for future use
};

struct FRAM_INFO {
	UINT8			BootLoaderReserved[FRAM_BOOTLOADER_RESERVED];
	UINT32			FRAMVersion;
	SINT8 			WCSoftwareVersionLabel[SWVERSION_LEN];
	SEGMENT_INFO	Seg[NUM_FRAM_SEGMENTS];
};

struct DEBUGSection
{
	TestPoints 			TPoints;
	unsigned int 		CrashValid;
	char 				TaskName[TASKNAMELEN];
	unsigned int 		CrashBuffer[4096 + 35];
	unsigned int 		CurrStackPtr;
	unsigned int 		Stackbegin;
	unsigned int 		StackEnd;
	unsigned int 		CrashBuffPrintLen;
	unsigned int 		MMUAR;
	unsigned char 		FRAMCrashFBRecvBuffer[64];
	unsigned char 		FRAMCrashMailBoxbuff[1596];
	UINT8 				FRAMCrashSerialRecBuff[512];
	UINT8 				FRAMCrashMBRcvPktbuf[MB_UDP_BUF_SIZE];
};

extern FRAM_INFO * FramInfo;
typedef void (*FramPowerUpHandler)(void);
void InitFRAM(void);
void CheckFRAMSections(void);
void FramCheck(void);
void ConvertV1FRAM_to_V2FRAM(void);
void ConvertV2FRAM_to_V3FRAM(void);
void ConvertV3FRAM_to_V4FRAM(void);
void ConvertV4FRAM_to_V5FRAM(void);
void ConvertVer6RlsFRAM_to_V1FRAM(void);
#endif /* FRAM_H_ */
