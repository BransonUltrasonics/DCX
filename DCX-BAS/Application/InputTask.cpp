/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Wupload
C/Application/InputTask.cpp_v   1.4   10 Sep 2012 03:11:38   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/InputTask.cpp_v  $
 * 
 *    Rev 1.45   23 Jan 2015 15:11:46   ygupta
 * Added case to print the number of Missed MB pkts.
 * 
 *    Rev 1.44   13 Mar 2014 15:54:42   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.43   06 Nov 2013 06:20:30   rjamloki
 * Going to preready on user input configuration change to prevent sonics.
 * 
 *    Rev 1.42   15 Oct 2013 07:42:10   rjamloki
 * Tracker Fix, JSON Fix, Link Status debouncing configurable from Adv R&D page.
 * 
 *    Rev 1.41   08 Oct 2013 07:52:26   rjamloki
 * DPRAM wait state changes after power up.
 * 
 *    Rev 1.40   08 Oct 2013 06:17:36   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.39   24 Sep 2013 06:28:40   rjamloki
 * Watch dog removed.
 * 
 *    Rev 1.38   20 Sep 2013 07:00:12   rjamloki
 * Added changes to build bootloader with same CyGOS.
 * 
 *    Rev 1.37   17 Sep 2013 04:14:00   rjamloki
 * Added Task Manager again and reduced time for WatchDogTimer.
 * 
 *    Rev 1.36   14 Sep 2013 07:30:36   rjamloki
 * WatchDog Cant be disabled.
 * 
 *    Rev 1.34   10 Sep 2013 03:46:38   ygupta
 * WC revision 6.A.
 * 
 *    Rev 1.33   03 Sep 2013 12:44:46   ygupta
 * Moved to tip from 1.31.1.1
 * 
 *    Rev 1.31.1.1   26 Aug 2013 09:02:58   amaurya
 * Fixed issue of load preset at start and tracker issue.
 * 
 *    Rev 1.31.1.0   24 Aug 2013 17:14:02   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.31   02 Aug 2013 10:55:46   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.29   10 Jul 2013 07:50:44   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.28   08 Jul 2013 08:53:10   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.27   27 Jun 2013 05:55:20   amaurya
 * System crash fixes.
 * 
 *    Rev 1.26   31 May 2013 10:37:14   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.25   21 May 2013 12:25:52   amaurya
 * Code review fixes.
 * 
 *    Rev 1.24   07 May 2013 10:13:10   amaurya
 * Code review fixed.
 * 
 *    Rev 1.23   06 May 2013 09:16:18   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.22   01 May 2013 05:46:56   ygupta
 * Fixed compilation issue in 3.0A
 * 
 *    Rev 1.21   15 Mar 2013 10:49:26   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.20   11 Mar 2013 02:25:48   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.19   18 Feb 2013 03:06:48   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.18   29 Jan 2013 02:19:50   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.17   16 Jan 2013 06:26:24   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.16   11 Jan 2013 02:51:16   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.15   03 Jan 2013 07:18:14   ygupta
 * Modbus Comm problem Resolved
 * 
 *    Rev 1.14   27 Dec 2012 06:43:54   ygupta
 * Changes to fix Issues
 * 
 *    Rev 1.13   14 Dec 2012 05:26:26   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.12   13 Dec 2012 00:09:44   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.11   29 Nov 2012 08:44:40   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.10   24 Nov 2012 08:01:22   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.9   09 Nov 2012 00:34:28   rjamloki
 * State Machine and website changes
 * 
 *    Rev 1.8   08 Nov 2012 01:19:14   rjamloki
 * Website changes
 * 
 *    Rev 1.6   26 Oct 2012 02:35:28   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.4   10 Sep 2012 03:11:38   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.3   29 Aug 2012 13:27:52   rjamloki
 * WC Modbus checkin and related changes
 *
 */

#include "InputTask.h"
#include "CyGOS.h"
#include "stdio.h"
#include "assert.h"
#include "string.h"
#include "Board.h"
#include "CPU.h"
#include "DebugNew.h"
#include "Flash.h"
#include "errno.h"
#include "Socket.h"
#include "io.h"
#include "TLV5604.h"
#include "MCP3208.h"
#include "Serial.h"
#include "WCTask.h"
#include "GLOBALS.h"
#include "FUNC.h"
#include "NRTModbusTask.h"
#include "Version.h"
#include "LOG.h"
#include "SysConfig.h"
#include "TCP.h"
#include "UploadFirmWareTask.h"
#include "ModBusRcvTask.h"	//required only to print RTC
#include "WebSite.h"
#include "StateMachineDrv.h"
#include "LED.h"
#include "RTOS.h"
#include "StateFunctions.h"
#include "TaskManager.h"
#include "BDMTask.h"
#include "ArchDefs.h"
#include "DcxManagerHandler.h"
#include "FEC.h"

#define printf pprintf
 extern void DebugPCBs();
// bool Dumb = FALSE;
void BdmTest();
SR se;
#define COMMAND_OPTION_SIZE 6
extern const struct DebugStateStruct DBGS[];
extern UINT32 HornScanRawFrequency[];
extern BOOLEAN DebugLCD, TokenAvailable;

extern SINT16 * HornScanRawPhase;
extern void PrintStateName(void);
BOOLEAN TestReset = FALSE;
#define SUCCESS_FLASHCODE_PROGARM  (SINT16)1
#define SUCCESS_FLASHTAR_PROGRAM   (SINT16)2

bool PrintHighestTaskLoad = FALSE;
bool PrintCurrentTaskLoad = FALSE;
//int currentTime11 = 0;
extern UINT32 TotalMissedPkt;
extern UINT32 LastTID;

InputTask::InputTask():
	Task("Input", 3072)
{
}

void InputTask::PrintHelp()
{
	printf("\n\n");
	 printf("\n\n");
	 printf("'?' - This help\n");
	 printf("'a' - FRAM ITPs\n");
	 printf("'C' - Current WC state\n");
	 printf("'F' - ReFlash Board\n");
	 printf("'G' - Setting FAN_EN line at logic 1\n");
	 printf("'L' - Debug LCD\n");
	 printf("'N' - Configure as DHCP Client\n");
	 printf("'O' - Configure as DHCP Server\n");
	 printf("'R' - CPU Restart\n");
	 printf("'S' - Show scan data\n");
	 printf("'W' - Show Weld Result\n");
	 printf("'Z' - Debug PCBs\n");
	 printf("'g' - Program Compressed Tar Region From TFTP\n");
	 printf("'2' - Show DCP timing control parameters\n");
	 printf("'5' - Cold Start\n");
	 printf("'6' - Print DCP Parameter Set\n");
	 printf("'7' - Print Weld Buffer\n");
	 printf("'8' - Program Compressed Tar Region From Serial\n");
	 printf("\n\n");
	printf("\n\n");
}


void EraseCallback(int offset, int length)
{
	pprintf("Erase: %08X of %08X\n", offset, length);
	 CyGOS::KickWatchDog();
}


void ProgramCallback(int offset, int length)
{
	 CyGOS::KickWatchDog();
	if(((offset & 0xFFFF) == 0) || (offset == length))
		pprintf("Program: %08X of %08X\n", offset, length);
}

void InputTask::ProgramBootFlash(int intf)
{
	char fileName[20];
	int a, b, c, d;
	char * ipPtr;
	ip_addr srvr;
	Flash * flash;
	UINT8 * data;
	int length;
	pprintf("\n enter tftp server ip address");
	memset(fileName, 0, sizeof(fileName));
	gets(fileName);
	ipPtr = fileName;
	a = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	b = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	c = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	d = atoi(ipPtr);
	srvr.addr = MakeIP(a, b, c, d);
	strcpy(fileName, "BootLoad_WC.bin");
	flash = Board::GetFlash();
	ASSERT(flash);
//	CyGOS::DisableWatchDog();
	CyGOS::StopTimer(1);
	length = (int) __FLASH_SIZE;
	data = new uint8[length];
	printf("Receiving file '%s':...", fileName);
	CyGOS::KickWatchDog();
	CyGOS::TftpReceive(intf, fileName, &length, data , srvr);
	pprintf("Length %d\n", length);
	if (length > 0){
		CyGOS::Stop();

		pprintf("Programming Address %08X\n", 0);
		pprintf("Flash %08X\n", flash);
		 CyGOS::KickWatchDog();
		flash -> Unlock(0, length);
		flash -> Erase(0, length, EraseCallback);
		flash -> Program(0, data, length, ProgramCallback);
		flash -> Lock(0, length);
		 CyGOS::KickWatchDog();
		DelayMs(500);
		CPU::Restart();
		while(1);
	}
	delete data;
	delete flash;
}

void InputTask::ProgramFlash(int intf)
{
	char fileName[20];
	char * ipPtr;
	ip_addr srvr;
	int a, b, c, d, length;
	pprintf("\n enter tftp server ip address");
	memset(fileName, 0, sizeof(fileName));
	gets(fileName);
	ipPtr = fileName;
	a = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	b = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	c = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	d = atoi(ipPtr);

	srvr.addr = MakeIP(a, b, c, d);
	strcpy(fileName, "App_WC.bin");
	Flash * flash = Board::GetFlash();
	ASSERT(flash);
//	CyGOS::DisableWatchDog();
	CyGOS::StopTimer(1);

	length = (int) __FLASH_SIZE;
	UINT8 * data = new uint8[length];
	printf("Receiving file '%s':...", fileName);
	CyGOS::TftpReceive(intf, fileName, &length, data , srvr);
	pprintf("Length %d\n", length);
	if (length > 0)
	{
	   CyGOS::Stop();
	 //  RTOS::Stop();
		pprintf("Programming Address %08X\n", __FLASH_ROM);
		pprintf("Flash %08X\n", flash);
		 CyGOS::KickWatchDog();
		flash -> Unlock(__FLASH_ROM, length);
		flash -> Erase(__FLASH_ROM, length, EraseCallback);
		flash -> Program(__FLASH_ROM, data, length, ProgramCallback);
		flash -> Lock(__FLASH_ROM, length);
		 CyGOS::KickWatchDog();
		DelayMs(300);
		 CyGOS::KickWatchDog();
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, SUCCESS_FLASHCODE_PROGARM, 0);
		pprintf("\n Waiting for Restart after flash program...\n ");
		CyGOS::Restart();
		while(1);
	}
	delete data;
	delete flash;
   CyGOS::StartTimer(1);
}


void TestCrash(int ab)
{
	ab = ab + 5;
	int bc = ab--;
	bc++;
	ab++;
	pprintf("\n %d", bc + ab);
}

void InputTask::ProgramCompressedTarRegionFromSerial()
{
	char fileName[20];
	int  ch;
	uint8 * data;
    Flash * flash;
	int  length = (int) __TAR_SIZE;
	bool firstByteReceived = false,Loop = TRUE;
	strcpy(fileName, "web.cyg");
	pprintf(" tar Size %08X Tar Start %08X", length, __TAR_START);
	printf("Download compressed TAR image '%s':...", fileName);
	//CyGOS::DisableWatchDog();
	CyGOS::StopTimer(1);
	data = new uint8[length];
	flash = Board::GetFlash();
	length = 0;
	ASSERT(flash);
	while(Loop){
		if(firstByteReceived){
			ch = getcharWait(500);
			if(ch == (-1)){
				pprintf("\n receive timeout");
				Loop = FALSE;
			}
			else{
				data[length++] = ch;
				pprintf(".");
			}
		}
		else{
			ch = getchar();
			data[length++] = ch;
			firstByteReceived = true;
		}
	}//while(true)

	pprintf("Length %d\n", length);
	if((length > (50 * 1024)) && (length < ((int) __TAR_SIZE))){
	   CyGOS::Stop();
		pprintf("Programming Address %08X\n", __TAR_START);
		pprintf("Flash %08X\n", flash);
		flash -> Unlock(__TAR_START, length);
		flash -> Erase(__TAR_START, length, EraseCallback);
		flash -> Program(__TAR_START, data, length, ProgramCallback);
		flash -> Lock(__TAR_START, length);
		DelayMs(300);
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, SUCCESS_FLASHTAR_PROGRAM, 0);
		pprintf("\n Waiting for Restart after flash program...\n ");
		CyGOS::Restart();
		while(1);
	}//if ((length > (50 * 1024)) && (length < ((int) __TAR_SIZE)))
	pprintf("\n invalid file length");
	delete data;
	delete flash;
   CyGOS::StartTimer(1);
}

void InputTask::ProgramCompressedTarRegionFlash(int intf)
{
	char fileName[20];
	int a, b, c, d;
	ip_addr srvr;
	char * ipPtr;
	Flash * flash;
	uint8 * data;
	int length;
	memset(fileName, 0, sizeof(fileName));
	pprintf("\n Enter tftp server ip address");
	gets(fileName);
	ipPtr = fileName;
	a = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	b = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	c = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	d = atoi(ipPtr);
	srvr.addr = MakeIP(a, b, c, d);
	strcpy(fileName, "web.cyg");

	printf("Receiving file '%s':...", fileName);
	flash = Board::GetFlash();
	ASSERT(flash);
	length = (int) __TAR_SIZE;
	data = new uint8[length];
//	CyGOS::DisableWatchDog();
	CyGOS::StopTimer(1);
	 CyGOS::KickWatchDog();
	printf("Receiving file '%s':...", fileName);
	CyGOS::TftpReceive(intf, fileName, &length, data , srvr);
	pprintf("Length = %d\n", length);
	if (length > 0){
	   CyGOS::Stop();
	   CyGOS::KickWatchDog();
		pprintf("Programming Address %08X\n", __TAR_START);
		pprintf("Flash %08X\n", flash);
		flash -> Unlock(__TAR_START, length);
		flash -> Erase(__TAR_START, length, EraseCallback);
		flash -> Program(__TAR_START, data, length, ProgramCallback);
		flash -> Lock(__TAR_START, length);
		 CyGOS::KickWatchDog();
		DelayMs(300);
		 CyGOS::KickWatchDog();
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, SUCCESS_FLASHTAR_PROGRAM, 0);
		CyGOS::Restart();
		pprintf("\n Waiting for Restart after flash program...\n ");
		while(1);
	}
	delete data;
	delete flash;
   CyGOS::StartTimer(1);
}

void PrintDcpParameterSet()
{
	pprintf("\n ParaScan.FrequencyStart = %d", DcpParameterSet.HsParam.ParaScan.FrequencyStart);
	pprintf("\n ParaScan.FrequencyStep = %d", DcpParameterSet.HsParam.ParaScan.FrequencyStep);
	pprintf("\n ParaScan.FrequencyStop = %d", DcpParameterSet.HsParam.ParaScan.FrequencyStop);
	pprintf("\n ParaScan.MaxAmplitude = %d", DcpParameterSet.HsParam.ParaScan.MaxAmplitude);
	pprintf("\n ParaScan.MaxCurrent = %d", DcpParameterSet.HsParam.ParaScan.MaxCurrent);
	pprintf("\n ParaScan.TimeDelay = %d", DcpParameterSet.HsParam.ParaScan.TimeDelay);
	pprintf("\n ParaScan.TimeErrorHigh = %d", DcpParameterSet.HsParam.ParaScan.TimeErrorHigh);

	pprintf("\n ParaSeek.FreqOffset = %d", DcpParameterSet.HsParam.ParaSeek.FreqOffset);
	pprintf("\n ParaSeek.SeekAmp = %d", DcpParameterSet.HsParam.ParaSeek.SeekAmp);
	pprintf("\n ParaSeek.SeekAmpLoopC1 = %d", DcpParameterSet.HsParam.ParaSeek.SeekAmpLoopC1);
	pprintf("\n ParaSeek.SeekAmpLoopC2 = %d", DcpParameterSet.HsParam.ParaSeek.SeekAmpLoopC2);
	pprintf("\n ParaSeek.SeekFreqHigh = %d", DcpParameterSet.HsParam.ParaSeek.SeekFreqHigh);
	pprintf("\n ParaSeek.SeekFreqLow = %d", DcpParameterSet.HsParam.ParaSeek.SeekFreqLow);
	pprintf("\n ParaSeek.SeekPhaseLoopCF = %d", DcpParameterSet.HsParam.ParaSeek.SeekPhaseLoopCF);
	pprintf("\n ParaSeek.SeekPhaseLoopI = %d", DcpParameterSet.HsParam.ParaSeek.SeekPhaseLoopI);
	pprintf("\n ParaSeek.SeekRampTime = %d", DcpParameterSet.HsParam.ParaSeek.SeekRampTime);
	pprintf("\n ParaSeek.SeekTime = %d", DcpParameterSet.HsParam.ParaSeek.SeekTime);

	pprintf("\n ParaTest.AmpStepCondition = %d", DcpParameterSet.HsParam.ParaTest.AmpStepCondition);
	pprintf("\n ParaTest.Amplitude1 = %d", DcpParameterSet.HsParam.ParaTest.Amplitude1);
	pprintf("\n ParaTest.Amplitude2 = %d", DcpParameterSet.HsParam.ParaTest.Amplitude2);
	pprintf("\n ParaTest.AmplitudeLoopC1 = %d", DcpParameterSet.HsParam.ParaTest.AmplitudeLoopC1);
	pprintf("\n ParaTest.AmplitudeLoopC2 = %d", DcpParameterSet.HsParam.ParaTest.AmplitudeLoopC2);
	pprintf("\n ParaTest.AmplitudeProfileRampTime = %d", DcpParameterSet.HsParam.ParaTest.AmplitudeProfileRampTime);
	pprintf("\n ParaTest.AmplitudeProfileTime = %d", DcpParameterSet.HsParam.ParaTest.AmplitudeProfileTime);
	pprintf("\n ParaTest.AmplitudeStartRampTime = %d", DcpParameterSet.HsParam.ParaTest.AmplitudeStartRampTime);
	pprintf("\n ParaTest.FreqOffset = %d", DcpParameterSet.HsParam.ParaTest.FreqOffset);
	pprintf("\n ParaTest.PhaseLoopCF = %d", DcpParameterSet.HsParam.ParaTest.PhaseLoopCF);
	pprintf("\n ParaTest.PhaseLoopI = %d", DcpParameterSet.HsParam.ParaTest.PhaseLoopI);
	pprintf("\n ParaTest.TimeErrorHigh = %d", DcpParameterSet.HsParam.ParaTest.TimeErrorHigh);
	pprintf("\n ParaTest.WeldFreqHigh = %d", DcpParameterSet.HsParam.ParaTest.WeldFreqHigh);
	pprintf("\n ParaTest.WeldFreqLow = %d", DcpParameterSet.HsParam.ParaTest.WeldFreqLow);

	pprintf("\n WeldParam.FreqWindowSize = %d", DcpParameterSet.WeldParam.FreqWindowSize);
	pprintf("\n WeldParam.MemoryStoreFlag = %d", DcpParameterSet.WeldParam.MemoryStoreFlag);
	pprintf("\n WeldParam.TimedSeekFlag = %d", DcpParameterSet.WeldParam.TimedSeekFlag);
	pprintf("\n WeldParam.UseTrueValues = %d", DcpParameterSet.WeldParam.UseTrueValues);
	pprintf("\n WeldParam.WeldAmplitudeLoopC1 = %d", DcpParameterSet.WeldParam.WeldAmplitudeLoopC1);
//	pprintf("\n WeldParam.WeldAmplitudeLoopC1New = %d", DcpParameterSet.WeldParam.WeldAmplitudeLoopC1New);
	pprintf("\n WeldParam.WeldAmplitudeLoopC2 = %d", DcpParameterSet.WeldParam.WeldAmplitudeLoopC2);
//	pprintf("\n WeldParam.WeldAmplitudeLoopC2New = %d", DcpParameterSet.WeldParam.WeldAmplitudeLoopC2New);
	pprintf("\n WeldParam.WeldFreqHigh = %d", DcpParameterSet.WeldParam.WeldFreqHigh);
	pprintf("\n WeldParam.WeldFreqLow = %d", DcpParameterSet.WeldParam.WeldFreqLow);
	pprintf("\n WeldParam.WeldPhaseLimit = %d", DcpParameterSet.WeldParam.WeldPhaseLimit);
	pprintf("\n WeldParam.WeldPhaseLimitTime = %d", DcpParameterSet.WeldParam.WeldPhaseLimitTime);
	pprintf("\n WeldParam.WeldPhaseLoopCF = %d", DcpParameterSet.WeldParam.WeldPhaseLoopCF);
	pprintf("\n WeldParam.WeldPhaseLoopI = %d", DcpParameterSet.WeldParam.WeldPhaseLoopI);
//	pprintf("\n WeldParam.WeldPhaseloopCFNew = %d", DcpParameterSet.WeldParam.WeldPhaseloopCFNew);
//	pprintf("\n WeldParam.WeldPhaseloopINew = %d", DcpParameterSet.WeldParam.WeldPhaseloopINew);

	pprintf("\n WeldParam.WeldPhaseLoopP = %d", DcpParameterSet.WeldParam.WeldPhaseLoopP);
	pprintf("\n WeldParam.WeldProfileRampTime = %d", DcpParameterSet.WeldParam.WeldProfileRampTime);
	pprintf("\n WeldParam.WeldRampTime = %d", DcpParameterSet.WeldParam.WeldRampTime);
	pprintf("\n WeldParam.Amplitude1 = %d", DcpParameterSet.Amplitude1);
	pprintf("\n WeldParam.Amplitude2 = %d", DcpParameterSet.Amplitude2);

	pprintf("\n WeldParam.EnergyCutoff = %d", DcpParameterSet.EnergyCutoff);
	pprintf("\n WeldParam.PeakPwrCutoff = %d", DcpParameterSet.PeakPwrCutoff);
	pprintf("\n WeldParam.TimeCutoff = %d", DcpParameterSet.TimeCutoff);

	pprintf("\n WeldParam.StepAtEnergy = %d", DcpParameterSet.StepAtEnergy);
	pprintf("\n WeldParam.StepAtPower = %d", DcpParameterSet.StepAtPower);
	pprintf("\n WeldParam.StepAtTime = %d", DcpParameterSet.StepAtTime);
}

void PrintCurrentParameterSet()
{
	pprintf("\n ParaScan.FrequencyStart = %d", HsParameterSet[CurrHsPreset].ParaScan.FrequencyStart);
	pprintf("\n ParaScan.FrequencyStep = %d", HsParameterSet[CurrHsPreset].ParaScan.FrequencyStep);
	pprintf("\n ParaScan.FrequencyStop = %d", HsParameterSet[CurrHsPreset].ParaScan.FrequencyStop);
	pprintf("\n ParaScan.MaxAmplitude = %d", HsParameterSet[CurrHsPreset].ParaScan.MaxAmplitude);
	pprintf("\n ParaScan.MaxCurrent = %d", HsParameterSet[CurrHsPreset].ParaScan.MaxCurrent);
	pprintf("\n ParaScan.TimeDelay = %d", HsParameterSet[CurrHsPreset].ParaScan.TimeDelay);
	pprintf("\n ParaScan.TimeErrorHigh = %d", HsParameterSet[CurrHsPreset].ParaScan.TimeErrorHigh);

	pprintf("\n ParaSeek.FreqOffset = %d", HsParameterSet[CurrHsPreset].ParaSeek.FreqOffset);
	pprintf("\n ParaSeek.SeekAmp = %d", HsParameterSet[CurrHsPreset].ParaSeek.SeekAmp);
	pprintf("\n ParaSeek.SeekAmpLoopC1 = %d", HsParameterSet[CurrHsPreset].ParaSeek.SeekAmpLoopC1);
	pprintf("\n ParaSeek.SeekAmpLoopC2 = %d", HsParameterSet[CurrHsPreset].ParaSeek.SeekAmpLoopC2);
	pprintf("\n ParaSeek.SeekFreqHigh = %d", HsParameterSet[CurrHsPreset].ParaSeek.SeekFreqHigh);
	pprintf("\n ParaSeek.SeekFreqLow = %d", HsParameterSet[CurrHsPreset].ParaSeek.SeekFreqLow);
	pprintf("\n ParaSeek.SeekPhaseLoopCF = %d", HsParameterSet[CurrHsPreset].ParaSeek.SeekPhaseLoopCF);
	pprintf("\n ParaSeek.SeekPhaseLoopI = %d", HsParameterSet[CurrHsPreset].ParaSeek.SeekPhaseLoopI);
	pprintf("\n ParaSeek.SeekRampTime = %d", HsParameterSet[CurrHsPreset].ParaSeek.SeekRampTime);
	pprintf("\n ParaSeek.SeekTime = %d", HsParameterSet[CurrHsPreset].ParaSeek.SeekTime);

	pprintf("\n ParaTest.AmpStepCondition = %d", HsParameterSet[CurrHsPreset].ParaTest.AmpStepCondition);
	pprintf("\n ParaTest.Amplitude1 = %d", HsParameterSet[CurrHsPreset].ParaTest.Amplitude1);
	pprintf("\n ParaTest.Amplitude2 = %d", HsParameterSet[CurrHsPreset].ParaTest.Amplitude2);
	pprintf("\n ParaTest.AmplitudeLoopC1 = %d", HsParameterSet[CurrHsPreset].ParaTest.AmplitudeLoopC1);
	pprintf("\n ParaTest.AmplitudeLoopC2 = %d", HsParameterSet[CurrHsPreset].ParaTest.AmplitudeLoopC2);
	pprintf("\n ParaTest.AmplitudeProfileRampTime = %d", HsParameterSet[CurrHsPreset].ParaTest.AmplitudeProfileRampTime);
	pprintf("\n ParaTest.AmplitudeProfileTime = %d", HsParameterSet[CurrHsPreset].ParaTest.AmplitudeProfileTime);
	pprintf("\n ParaTest.AmplitudeStartRampTime = %d", HsParameterSet[CurrHsPreset].ParaTest.AmplitudeStartRampTime);
	pprintf("\n ParaTest.FreqOffset = %d", HsParameterSet[CurrHsPreset].ParaTest.FreqOffset);
	pprintf("\n ParaTest.PhaseLoopCF = %d", HsParameterSet[CurrHsPreset].ParaTest.PhaseLoopCF);
	pprintf("\n ParaTest.PhaseLoopI = %d", HsParameterSet[CurrHsPreset].ParaTest.PhaseLoopI);
	pprintf("\n ParaTest.TimeErrorHigh = %d", HsParameterSet[CurrHsPreset].ParaTest.TimeErrorHigh);
	pprintf("\n ParaTest.WeldFreqHigh = %d", HsParameterSet[CurrHsPreset].ParaTest.WeldFreqHigh);
	pprintf("\n ParaTest.WeldFreqLow = %d", HsParameterSet[CurrHsPreset].ParaTest.WeldFreqLow);

	pprintf("\n WeldParam.FreqWindowSize = %d", ParameterSet[CurrPreset].DcpParam.FreqWindowSize);
	pprintf("\n WeldParam.MemoryStoreFlag = %d", ParameterSet[CurrPreset].DcpParam.MemoryStoreFlag);
	pprintf("\n WeldParam.TimedSeekFlag = %d", ParameterSet[CurrPreset].DcpParam.TimedSeekFlag);
	pprintf("\n WeldParam.UseTrueValues = %d", ParameterSet[CurrPreset].DcpParam.UseTrueValues);
	pprintf("\n WeldParam.WeldAmplitudeLoopC1 = %d", ParameterSet[CurrPreset].DcpParam.WeldAmplitudeLoopC1);
	pprintf("\n WeldParam.WeldAmplitudeLoopC2 = %d", ParameterSet[CurrPreset].DcpParam.WeldAmplitudeLoopC2);
	pprintf("\n WeldParam.WeldFreqHigh = %d", ParameterSet[CurrPreset].DcpParam.WeldFreqHigh);
	pprintf("\n WeldParam.WeldFreqLow = %d", ParameterSet[CurrPreset].DcpParam.WeldFreqLow);
	pprintf("\n WeldParam.WeldPhaseLimit = %d", ParameterSet[CurrPreset].DcpParam.WeldPhaseLimit);
	pprintf("\n WeldParam.WeldPhaseLimitTime = %d", ParameterSet[CurrPreset].DcpParam.WeldPhaseLimitTime);
	pprintf("\n WeldParam.WeldPhaseLoopCF = %d", ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopCF);
	pprintf("\n WeldParam.WeldPhaseLoopI = %d", ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopI);
	pprintf("\n WeldParam.WeldPhaseLoopP = %d", ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopP);
	pprintf("\n WeldParam.WeldProfileRampTime = %d", ParameterSet[CurrPreset].DcpParam.WeldProfileRampTime);
	pprintf("\n WeldParam.WeldRampTime = %d", ParameterSet[CurrPreset].DcpParam.WeldRampTime);

	pprintf("Weld Ramp Time %d %d %d", ParameterSet[CurrPreset].DcpParam.WeldRampTime,
	ParameterSet[Preset_Default].DcpParam.WeldRampTime, RunningPreset.DcpParam.WeldRampTime);
	pprintf("\n FreqOffset=%d %d %d %d", ParameterSet[CurrPreset].WcParam.FreqOffset, ParameterSet[Preset_Default].WcParam.FreqOffset, ParameterSet[Preset_MIN].WcParam.FreqOffset, ParameterSet[Preset_MAX].WcParam.FreqOffset);
}

void InputTask::Run(void)
{
	DelayMs(500);
	UINT16 OutData = 0;
	UINT16 OutBitsPosFANamdLED[3];
	Fifo<BOOLEAN> StatReply(1, "stat");
    SINT32 Ch[COMMAND_OPTION_SIZE];
	SINT32 T1;
	SINT8 DataIn[20];
//	long  * TCR1;
	SINT32 i = 0;
	long * StkBegin;
//	long *TCR1;
	SR ssr;
	Task * TaskPtr;
	SINT8 *Data = (SINT8*)&i;
	NRTcmd = (NrtQ){CMD_GETWELDRESULT, READ_REQ, &StatReply};
	PrintHelp();
	for (;;){
      //Increased resistant against the noise on receiver line and garbage coming.
      // Command will work As:
      //PASSx
     //Where x is the command option character and than carriage return.
      Ch[0] = getchar();//P
      Ch[1] = getchar();//A
      Ch[2] = getchar();//S
      Ch[3] = getchar();//S
      Ch[4] = getchar();//Command option
      Ch[5] = getchar();//Carriage return
//      CyGOS::DisableWatchDog();
      if((Ch[0] == 'P')&&(Ch[1] == 'A')&&(Ch[2] == 'S')&&(Ch[3] == 'S')&&(Ch[5] == '\r')){
         switch (Ch[4]){
         case 'y':
            if(TokenAvailable == TRUE){
               TokenAvailable = FALSE;
               pprintf("\n Token Not Available ");
            }
            else{
               TokenAvailable = TRUE;
               pprintf("\n Token Available ");
            }

            break;
			case '?':
				PrintHelp();
			break;
			case 'T':
				CyGOS::PrintStatus(PrintTasksWithStack);
			break;
			case 'M':
				ssr = EnterCritical();
				ShowMemory();
				ExitCritical(ssr);
			break;
			case 'F':
				ProgramFlash(1);
			break;
			case 'B':
				ProgramBootFlash(1);
				break;
			case 't':
				PrintCurrentTaskLoad = !PrintCurrentTaskLoad;
				pprintf("\n PrintCurrentTaskLoad %d",PrintCurrentTaskLoad);
			break;
			case 'C':
				PrintHighestTaskLoad = !PrintHighestTaskLoad;
				pprintf("\n PrintHighestTaskLoad %d",PrintHighestTaskLoad);
			break;
			case '4':
				pprintf("\n Total Missed MB Pkt %d, Last TID %d ",TotalMissedPkt, LastTID);
			break;
			case '5':
			   LogData2 = LOG_CLEARED_BY_COLD_START_VIA_SERIAL_PORT;
			   ColdStart();
			   pprintf("\n ColdStart");
			break;
			case '6':
				PrintDcpParameterSet();
				pprintf("\n");
				pprintf("\n");
				PrintCurrentParameterSet();
			break;
			case '7':
				if(WeldBufferIndex > 0){
				   pprintf("\n Time   Frequency   PWMValue NominalAmplitude ActualAmplitude Current  Power Phase");
	               for(i = 0; i < WeldBufferIndex; i++){
	                  pprintf("\n %d", WeldBuffer[i].Time);
	                  pprintf("   %d", WeldBuffer[i].Frequency);
	                  pprintf("   %d",WeldBuffer[i].PWMValue);
	                  pprintf("   %d",WeldBuffer[i].NominalAmplitude);
	                  pprintf("   %d",WeldBuffer[i].ActualAmplitude);
	                  pprintf("   %d",WeldBuffer[i].Current);
	                  pprintf("   %d",WeldBuffer[i].Power);
	                  pprintf("   %d",WeldBuffer[i].Phase);
	               }
               }//if(WeldBufferIndex > 0)
			break;
			case 'W':
				pprintf("\n CycleCounter 	   = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].CycleCounter);
				pprintf("\n TS.Year            = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Year);
				pprintf("\n TS.Month           = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Month );
				pprintf("\n TS.Day             = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Day);
				pprintf("\n TS.Hour            = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Hour);
				pprintf("\n TS.Minute          = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Minute );
				pprintf("\n TS.Second          = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Second );
				pprintf("\n WeldTime           = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].WeldTime);
				pprintf("\n WeldEnergy         = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].WeldEnergy);
				pprintf("\n PeakPower          = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].PeakPower);
				pprintf("\n CustomAIn1 		   = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].CustomAIn1 );
				pprintf("\n Amplitude1 		   = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].Amplitude1);
				pprintf("\n Amplitude2 		   = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].Amplitude2 );
				pprintf("\n PresetNumber 	   = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].PresetNumber );
				pprintf("\n WeldStartFrequency = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].WeldStartFrequency);
				pprintf("\n WeldStopFrequency  = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].WeldStopFrequency );
				pprintf("\n FrequencyChange    = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].FrequencyChange);
				pprintf("\n ErrorPwm           = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].ErrorPwm);
				pprintf("\n ErrorReason        = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].ErrorReason);
				pprintf("\n AvgApm1        = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].AvgAmplitude1);
				pprintf("\n AvgApm2        = %d", WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].AvgAmplitude2);
			break;
			case '8':
			   ProgramCompressedTarRegionFromSerial();
			break;
			case 'g':
			   ProgramCompressedTarRegionFlash(1);
			break;
			case 'R':
			CPU::Restart();
			break;
			case 'S':
//				if(NumScanPoints > 0){
//				   for(i = 0; i <= NumScanPoints; i++){
//					  pprintf("\n status = %d", ScanBuffer[i].Index);
//					  pprintf("  Freq = %d", ScanBuffer[i].Frequency);
//					  pprintf("  RFreq = %d", HornScanRawFrequency[i]);
//					  pprintf("  pwm = %d", ScanBuffer[i].PWMValue);
//					  pprintf("  Amp = %d", ScanBuffer[i].Amplitude);
//					  pprintf("  Current = %d", ScanBuffer[i].Current);
//					  pprintf("  Power = %d", ScanBuffer[i].Power);
//					  pprintf("  phase = %d", ScanBuffer[i].Phase);
//				   }
//				}
				//Generate WatchDog while interrupts are disabled.
				ssr = EnterCritical();
				pprintf("\n Generate WatchDog while interrupts are disabled. ");
				while(true);
				ExitCritical(ssr);
			break;
			case 'D':
				pprintf("\n RAM TPs Loaded from FRAM on PowerUp");
				for(i = 0; i < 300; i++){
					pprintf("\n Indx %d TTP %u", i , TPointsRam.TTP[i]);
				}
				for(i = 0; i < 300; i++){
					pprintf("\n Indx %d ITP %u", i , TPointsRam.ITP[i]);
				}

				if(DebugSection->CrashValid == 0xAA22EF55){
					pprintf("\n%s", TaskName);
					pprintf("\nStack Begin:%08X", Stackbegin);
					pprintf("\nStack End:%08X", StackEnd);
					pprintf("\nCurr Stack:%08X", CurrStackPtr);
					pprintf("\nMMUAR:%08X", MMUAR);
					pprintf("\nStack Content:");
					for(i = 0; i < (SINT32)CrashBuffPrintLen; i++){
						if(i < 4131)
							pprintf("%08X ", CrashBufferRam[i]);
					}
				}
				else{
					pprintf("\n No Crash Info:");
				}
    		     break;
			case 'r':
				//Try disabling the watchdog.
				//Generate WatchDog while interrupts are disabled .
//				ssr = EnterCritical();
//				 CyGOS::DisableWatchDog();
//				pprintf("\n enerate WatchDog while interrupts are enabled. ");
//				currentTime11 = Board::GetProfilerTimerCounter(
//								PROFILER_TIMER_INSTANCE);
//				while(true);
//				ExitCritical(ssr);
//				ssr = EnterCritical();
//				for(i = 0; i < 64; i++){
//					TaskPtr = RTOS::GetTask(i);
//					if(TaskPtr){
//						TCR1 = TaskPtr->GetStatckBegin();
//						pprintf("\n %s %08X %08X %08X", TaskPtr ->GetName(), TaskPtr , TCR1[TaskPtr->Tstacksize - 1], TCR1[TaskPtr->Tstacksize - 2]);
//					}
//				}
//				ExitCritical(ssr);
//				printf("\n writing weld result object");
//				Weld_Result = new WeldResult;
//				memset(Weld_Result , 0 , sizeof(WeldResult));
//
//				Obj_WeldResult = new JSONWeldResult();
//				Obj_WeldResult->DataPtr = Weld_Data;
//			    JsonDataList.AddTail(Obj_WeldResult);
			    break;
			case 'p':
				ssr = EnterCritical();
				FEC::fecs[0]->PrintBDStatus();
				FEC::fecs[1]->PrintBDStatus();
				ExitCritical(ssr);
//				TCR1 = (long *)TestCrash;
//				TCR2 = (long *)ProgramCallback;
//				funcsize = TCR2 - TCR1;
//				for(INDEX = 0; INDEX < funcsize ; INDEX++)
//					pprintf("\n %08X", *TCR1++);
//
//				TCR1 = (long *)TestCrash;
//				for(INDEX = 0; INDEX < funcsize ; INDEX++)
//					*TCR1++ = 0xDEADBABE;
//
//				TCR1 = (long *)TestCrash;
//				for(INDEX = 0; INDEX < funcsize ; INDEX++)
//					pprintf("\n %08X", *TCR1++);
//				TestCrash(7);
//				ASSERT(0);
//				printf("\n writing preset object");
//				Preset = new DcxPreset;
//				memsetD(Preset , 0 , sizeof(DcxPreset));
//
//				Obj_Preset = new JSONPreset();
//				Obj_Preset->DataPtr = Preset;
//	     	    JsonDataList.AddTail(Obj_Preset);
			    break;
			case 'Y':
				ssr = EnterCritical();
				pprintf("\n FRAM TTPs");
				for(i = 0; i < 300; i++){
					if(DebugSection->TPoints.TTP[i])
					pprintf("\n Indx %d TTP %u", i , DebugSection->TPoints.TTP[i]);
				}
				ExitCritical(ssr);
				break;
			case 'a':
				pprintf("\n FRAM ITPs");
				for(i = 0; i < 300; i++){
					if(DebugSection->TPoints.ITP[i])
					pprintf("\n Indx %d ITP %u", i , DebugSection->TPoints.ITP[i]);
				}
				break;
			case 'd':
				if(tcpip)
					pprintf("\n tcp ip Q Cnt %d", tcpip->GetQCount());
			    break;
			case 'A':

            pprintf("\n EnterWait state");
            gets(DataIn);
//          TCR1 = (long*)xtoi(DataIn);
            CPU::SetupChipSelect(CHIPSELCT_PIN2, atoi(DataIn), DATABUSWIDTH_DPRAM_CS2, (void *)HILSCHERDPRAM_BASEADDRESS);
            pprintf("\n DPRAM Wait state set to %d ",atoi(DataIn));

//				pprintf("\n Enter hex Address");
//				gets(DataIn);
//				TCR1 = (long*)xtoi(DataIn);
//				pprintf("\n Address = %08X %08x", TCR1,*TCR1);
//				pprintf("\n Data Read Success");
//				*TCR1 = 0xDEADBABE;
//				pprintf("\n Data Write Success %8X",*TCR1);
				//extern bool KickWD;
				//KickWD = FALSE;
//	    		printf("\n writing Alarm object");
//	    		Alarm = new const char*[2];
//	    		Alarm[0] = "0x0b01";
//	    		Alarm[1] = "0x0b02" ;
//
//
//	    		Obj_Alarm = new JSONAlarm();
//	    		Obj_Alarm->DataPtr = Alarm;
//			    JsonDataList.AddTail(Obj_Alarm);

			    break;
			case 'G':
				pprintf("\n Setting FAN_EN line at logic 1");
				OutData = 0;
				OutBitsPosFANamdLED[0] = BIT(7);
				OutBitsPosFANamdLED[1] =  BIT(6);
				OutBitsPosFANamdLED[2] = /*Red LED*/ BIT(5)/*Green LED*/;
				OutData |= (OutBitsPosFANamdLED[FANEN] | OutBitsPosFANamdLED[REDLED] | OutBitsPosFANamdLED[GREENLED]);
				DIGITAL_IO_FANANDLEDADDR = OutData;
			break;
			case 'H':
				ssr = EnterCritical();
				pprintf("\n FRAM TTPs");
				for(i = 0; i < 300; i++)
					DebugSection->TPoints.TTP[i] = 0;


				pprintf("\n FRAM ITPs");
				for(i = 0; i < 300; i++)
					DebugSection->TPoints.ITP[i] = 0;
				ExitCritical(ssr);
			break;
			case 'N':
			   Sysconfig->CurrNetworkConfig = configDhcpClient;
			   CyGOS::WriteSystemEvent(EVENT_IPCONFIG_CHANGED, __LINE__,Sysconfig->CurrNetworkConfig, (('I' << 24) | ('n' << 16) | ('p' << 8) | ('u')), true);
			   CyGOS::Restart();
			   break;
			case 'O':
				Sysconfig->CurrNetworkConfig = configDhcpServer;
				CyGOS::WriteSystemEvent(EVENT_IPCONFIG_CHANGED, __LINE__,Sysconfig->CurrNetworkConfig, (('I' << 24) | ('n' << 16) | ('p' << 8) | ('u')), true);
				CyGOS::Restart();
            break;
			case 'Z':
//				pprintf("\n calling DebugPCBs \n");
//				DebugPCBs();
//				pprintf("\n called it");

				memcpy((UINT8 *)FRAM_BASEADDRESS, Data, 4);
				break;
			case 'L':
				if(DebugLCD == FALSE)
					DebugLCD = TRUE;
				else
					DebugLCD = FALSE;
				break;
			case 'b':
				pprintf("\nEnter Priority\n");
				gets(DataIn);
				T1 = atoi(DataIn);
				ssr = EnterCritical();
				if((T1 >= 0) && (T1 < 64)){
					TaskPtr = RTOS::GetTask(T1);
				    if(TaskPtr){
						pprintf("\n Task %s\n", TaskPtr->GetName());
						StkBegin = TaskPtr->GetStatckBegin();
						for(i = 0; i < TaskPtr->Tstacksize; i++){
							pprintf("%08X ", StkBegin[i]);
							if((i & 0xFF) == 0xFF)
							   CyGOS::KickWatchDog();
						}
				    }
				}
				ExitCritical(ssr);
				break;
			case '2':
				pprintf("\n DCPTIME=%d CLTIME=%d MBTIME=%d SMTIME=%d ADC Resolution %d MAXDCPADCRAWVALUE %d BlindTime %d", DCPTimeControlParam.DCPTime, DCPTimeControlParam.ControlLoopTime, DCPTimeControlParam.ModBusCommTime,
											DCPTimeControlParam.StateMachineTime, DCPTimeControlParam.AdcResolution, MAXDCPADCRAWVALUE, DCPTimeControlParam.BlindTime);
			break;
			default:
				pprintf("\nNotSupported %c", Ch[4]);
			break;
		}//switch (ch)
      }
      else{
            pprintf("\nNoise detected=%c,%c,%c,%c,%c,%c",Ch[0],Ch[1],Ch[2],Ch[3],Ch[4],Ch[5]);
            Log::WriteEventLog(EVENT_INVALID_SERIAL_INPUT);
      }
   }//for(;;)
}

BOOLEAN TestCheckForReset(void)
{
	return TestReset;
}

void InputTask::Tick()
{
	//WC Board specific
	/*
	CPU Pin  Part of LED  Color when high   Port MCF_GPIO_PPDSDR_ATAH Bit
	V21			LED12		Green 		  MCF_GPIO_PPDSDR_ATAH_PPDSDR_ATAH0
	V20			LED12		Red			  MCF_GPIO_PPDSDR_ATAH_PPDSDR_ATAH1
	V19			LED5		Red			  MCF_GPIO_PPDSDR_ATAH_PPDSDR_ATAH2
	W22			LED5		Green		  MCF_GPIO_PPDSDR_ATAH_PPDSDR_ATAH3
	*/
	static int usCount = 0;
	static BOOLEAN LED12 = TRUE;
	usCount += usecPerTick;
	if(usCount > (100000)){
		if(LED12 == TRUE){

			LED12 = FALSE;
			LED::Set(2, LedRed);
		}
		else{
			LED12 = TRUE;
			LED::Set(2, LedGreen);
		}

		usCount -= 100000;
	}
//	CyGOS::KickWatchDog();
}
