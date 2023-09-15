/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/InputTask.cpp_v   1.6   06 Jun 2016 14:57:14   GButron  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/InputTask.cpp_v  $
 * 
 *    Rev 1.6   06 Jun 2016 14:57:14   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.5   04 Jun 2015 07:40:26   sesharma
 * Type casting for Data1 & Data2 fields of event log changed to SINT32 when writing an event. Removed unused variables.
 * 
 *    Rev 1.4   19 Mar 2015 06:51:54   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.3   28 Jan 2015 06:08:26   sesharma
 * Type Casting done to Remove Compiler Warnings.
 * 
 *    Rev 1.2   27 Nov 2014 09:13:46   sesharma
 * Webpages and SIG related changes 
 * 
 *    Rev 1.1   17 Sep 2014 08:57:14   amaurya
 * Added support for completebin, appbin upload from website and code review fix
 * 
 *    Rev 1.0   18 Aug 2014 09:17:24   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:00   amaurya
 * Initial revision.
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
#include "DupsTask.h"
#include "GLOBALS.h"
#include "FUNC.h"
#include "UserIO.h"
#include "StateMachineDrv.h"
#include "Version.h"
#include "LOG.h"
#include "SysConfig.h"
#include "TCP.h"

extern BOOLEAN DebugLCDFlag;
extern SINT16 CurrentIndex;
extern SINT32 FreqOutDacVal;
SPI * spi;
DAC * dac;
int mV[4];
struct AdcBuf;
LogData templogdata[100];
extern SINT16 *HornScanRawPhase;
InputTask::InputTask() :
		Task("Input", 3072) {
}

void InputTask::PrintHelp() {
	printf("\n\n");
	printf("? - This help\n");
	printf("M - Print memory allocation\n");
	printf("T - Show task status\n");
	printf("F - ReFlash Board\n");
	printf("r - Read flash chip\n");
	printf("1 - Run tests\n");
	printf("\n\n");
}

void EraseCallback(int offset, int length) {
	pprintf("Erase: %08X of %08X\n", offset, length);
}

void ProgramCallback(int offset, int length) {
	if (((offset & 0xFFFF) == 0) || (offset == length))
		pprintf("Program: %08X of %08X\n", offset, length);
}

void InputTask::ProgramFlash(int intf) {
	char fileName[20];

	pprintf("\n enter tftp server ip address");

	memset(fileName, 0, sizeof(fileName));
	gets(fileName);
	ip_addr srvr;
	char * ipPtr = fileName;
	int a, b, c, d;
	a = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	b = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	c = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	d = atoi(ipPtr);

	srvr.addr = MakeIP(a, b, c, d);

	for (int i = 0; i < 5; i++)
		sprintf(fileName + i * 2, "%02X", __ETHERNET_MAC[i]);
	strcat(fileName, ".bin");

	Flash * flash = Board::GetFlash();
	ASSERT(flash);

	int length = (int) __FLASH_SIZE;
	uint8 * data = new uint8[length];

	printf("Receiving file '%s':...", fileName);
	CyGOS::TftpReceive(intf, fileName, &length, data, srvr);
	pprintf("Length %d\n", length);
	if (length > 0) {
		pprintf("Programming Address %08X\n", __FLASH_ROM);
		pprintf("Flash %08X\n", flash);
		flash->Unlock(__FLASH_ROM, length);
		flash->Erase(__FLASH_ROM, length, EraseCallback);
		flash->Program(__FLASH_ROM, data, length, ProgramCallback);
		flash->Lock(__FLASH_ROM, length);
//		pprintf("Programmed, restarting...\n");
		DelayMs(1000);
//		CPU::Restart();
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (SINT32) (FLASHCODE_PROGARM), 0,
		      0, false);
		pprintf("\n Waiting for Restart after flash program...\n ");
		CyGOS::DisableWatchDog();
		CyGOS::Stop();
		while (1)
			;
	}
	delete data;
	delete flash;
}

void InputTask::ProgramCompressedTarRegionFromSerial() {
	char fileName[20];
	int ch;
	int length = (int) __TAR_SIZE;
	bool firstByteReceived = false;
	strcpy(fileName, "web");
	for (int i = 0; i < 5; i++)
		sprintf(fileName + i * 2 + strlen("web"), "%02X", __ETHERNET_MAC[i]);
	strcat(fileName, ".cyg");
	pprintf(" tar Size %08X Tar Start %08X", length, __TAR_START);
	printf("Download compressed TAR image '%s':...", fileName);
	CyGOS::DisableWatchDog();
	CyGOS::StopTimer(1);
	uint8 * data = new uint8[length];
	Flash * flash = Board::GetFlash();
	length = 0;
	ASSERT(flash);
	while (true) {
		if (firstByteReceived) {
			ch = getcharWait(500);
			if (ch == (-1)) {
				pprintf("\n receive timeout");
				break;
			} else {
				data[length++] = ch;
				pprintf(".");
			}
		} else {
			ch = getchar();
			data[length++] = ch;
			firstByteReceived = true;
		}
	}
	pprintf("Length %d\n", length);
	if ((length > (50 * 1024)) && (length < ((int) __TAR_SIZE))) {
		pprintf("Programming Address %08X\n", __TAR_START);
		pprintf("Flash %08X\n", flash);
		flash->Unlock(__TAR_START, length);
		flash->Erase(__TAR_START, length, EraseCallback);
		flash->Program(__TAR_START, data, length, ProgramCallback);
		flash->Lock(__TAR_START, length);
//		pprintf("Programmed, restarting...\n");
		DelayMs(3000);
//		CPU::Restart();
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (SINT32) (FLASHTAR_PROGRAM), 0,
		      0, false);
		pprintf("\n Waiting for Restart after flash program...\n ");
		CyGOS::DisableWatchDog();
		CyGOS::Stop();
		while (1)
			;
	}
	pprintf("\n invalid file length");
	delete data;
	delete flash;
	CyGOS::StartTimer(1);
	CyGOS::EnableWatchDog();
}

void InputTask::ProgramCompressedTarRegionFlash(int intf) {
	char fileName[20];
	pprintf("\n enter tftp server ip address");

	memset(fileName, 0, sizeof(fileName));
	gets(fileName);
	ip_addr srvr;
	char * ipPtr = fileName;
	int a, b, c, d;
	a = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	b = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	c = atoi(ipPtr);
	ipPtr = strchr(ipPtr, '.') + 1;
	d = atoi(ipPtr);

	srvr.addr = MakeIP(a, b, c, d);

	strcpy(fileName, "web");
	for (SINT32 i = 0; i < 5; i++)
		sprintf(fileName + i * 2 + strlen("web"), "%02X", __ETHERNET_MAC[i]);
	strcat(fileName, ".cyg");

	printf("Receiving file '%s':...", fileName);

	Flash * flash = Board::GetFlash();
	ASSERT(flash);

	int length = (int) __TAR_SIZE;
	UINT8 * data = new UINT8[length];

	printf("Receiving file '%s':...", fileName);
	CyGOS::TftpReceive(intf, fileName, &length, data, srvr);
	pprintf("Length %d\n", length);

	if (length > 0) {
		pprintf("Programming Address %08X\n", __TAR_START);
		pprintf("Flash %08X\n", flash);
		flash->Unlock(__TAR_START, length);
		flash->Erase(__TAR_START, length, EraseCallback);
		flash->Program(__TAR_START, data, length, ProgramCallback);
		flash->Lock(__TAR_START, length);
//		pprintf("Programmed, restarting...\n");
		DelayMs(3000);
//		CPU::Restart();
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (SINT32) (FLASHTAR_PROGRAM), 0,
		      0, false);
		pprintf("\n Waiting for Restart after flash program...\n ");
		CyGOS::DisableWatchDog();
		CyGOS::Stop();
		while (1)
			;
	}
	delete data;
	delete flash;
}

void InputTask::Run() {
	DelayMs(500);
	PrintHelp();
	SINT32 NumbeOfLog = 0;
	char data1[10];
	UINT32 currFreq;
	UINT32 testCyTime = 0;
	for (;;) {
		//Increased Security against the noise on receiver line and garbage coming.
		// Command will work As:
		//PASSx
		//Where x is the command option character and than carriage return.
		int ch[6];
		ch[0] = getchar();		//P
		ch[1] = getchar();		//A
		ch[2] = getchar();		//S
		ch[3] = getchar();		//S
		ch[4] = getchar();		//Command option
		ch[5] = getchar();		//Carriage return
		if ((ch[0] == 'P') && (ch[1] == 'A') && (ch[2] == 'S') && (ch[3] == 'S')
		      && (ch[5] == '\r')) {
			switch (ch[4]) {
			case '?':
				PrintHelp();
				break;
			case 'R':
				CPU::Restart();
				break;
			case 'T':
				CyGOS::PrintStatus(PrintTasks);
				break;
			case 'L':
				printf("\r\n LCD");
				if (DebugLCDFlag == FALSE)
					DebugLCDFlag = TRUE;
				else
					DebugLCDFlag = FALSE;
				break;
			case 'M':
				ShowMemory();
				break;
			case 'F':
				printf("\r\n size of int %d", (int) sizeof(int));
				printf("\r\n size of short %d", (int) sizeof(short));
				printf("\r\n size of unsigned int %d", (int) sizeof(unsigned int));
				printf("\r\n size of char %d", (int) sizeof(char));
				printf("\r\n size of uint8 %d", (int) sizeof(uint8));
				printf("\r\n size of unsigned LONG int %d",
				      (int) sizeof(unsigned long int));
				printf("\r\n size of unsigned short int %d",
				      (int) sizeof(unsigned short int));
				break;
			case 'f':
				sysConfig.currNetworkConfig = configDhcpClient;
				printf("\r\n Client");
				break;
			case '3':
				printf("\n enter scan amp in percentage");
				memset(data1, 0, sizeof(data1));
				gets(data1);
				ParaScan.MaxAmplitude = atoi(data1);
				ParaScan.MaxCurrent = atoi(data1);
				ParaScan.FrequencyStart = 194500;
				ParaScan.FrequencyStop = 204500;
				ParaScan.FrequencyStep = 10;
				ParaScan.TimeDelay = 10;
				SaveScanParamCRC();
				Reset_Scanbuffer();
				ScanInitiate = TRUE;
				break;
			case '4':
				printf("\n Scan Result = ");
				for (UINT8 i = 0; i < 6; i++) {
					DelayMs(50);
					printf(" %4d", (SINT32) ResonanceFreqs[i]);
				}
				break;
			case '5':
				pprintf("\n FreqOffsetInput =%d", FreqOffsetInput);
				pprintf("\n ExtAmplitudeInput =%d", ExtAmplitudeInput);
				pprintf("\n FreqOutDacVal %d DDSfreq %d", FreqOutDacVal,
				      DDSFrequency);
				break;
			case '7':
				ProgramCompressedTarRegionFromSerial();
				break;
			case '8':
				memset(data1, 0, sizeof(data1));
				gets(data1);
				testCyTime = (UINT32) atoi(data1);
				pprintf("\n testCyTime = %d \n", SCALEDTIMETOWEBSITE(testCyTime));
				break;
			case '9':
				MFCycleFlag = FALSE;
				memset(&MFcycleResults, 0, sizeof(MFcycleResults));
				MFcycleResults.currCycleTime = 3600000;
				SonicsOffCounter = 0;
				MFcycleResults.nCycles = 18000;
				MFcycleResults.sonicsOnTime = 1800000;
				SonicsOnCounter = 0;
				MFCycleFlag = TRUE;
				//DecompressTarRegionFlash('A');
				break;
			case 'w':
				pprintf("\n CurrentIndex=%d", CurrentIndex);
				DebugPCBs();
				break;
			case '*':
				printf(" * ");
				printf("\n");
				pprintf(" RunningPreset.SeekAmplitude = %d (%%) \n",
				      RunningPreset.SeekAmplitude);
				pprintf(" RunningPreset.SeekRampTime = %d\n",
				      RunningPreset.SeekRampTime);
				pprintf(" RunningPreset.SeekTime = %d\n", RunningPreset.SeekTime);
				pprintf(" RunningPreset.SeekFrequLimitHigh = %d\n",
				      RunningPreset.SeekFrequLimitHigh);
				pprintf(" RunningPreset.SeekFrequLimitLow = %d\n",
				      RunningPreset.SeekFrequLimitLow);
				pprintf(" RunningPreset.SeekPhaseloopI = %d\n",
				      RunningPreset.SeekPhaseloopI);
				pprintf(" RunningPreset.SeekPhaseloopCF = %d\n",
				      RunningPreset.SeekPhaseloopCF);
				pprintf(" RunningPreset.SeekAmplitudeLoopC1 = %d\n",
				      RunningPreset.SeekAmplitudeLoopC1);
				pprintf(" RunningPreset.SeekAmplitudeLoopC2 = %d\n",
				      RunningPreset.SeekAmplitudeLoopC2);
				pprintf(" RunningPreset.WeldPhaseloopI = %d\n",
				      RunningPreset.WeldPhaseloopI);
				pprintf(" RunningPreset.WeldPhaseloopCF = %d\n",
				      RunningPreset.WeldPhaseloopCF);
				pprintf(" RunningPreset.WeldAmplitudeLoopC1 = %d\n",
				      RunningPreset.WeldAmplitudeLoopC1);
				pprintf(" RunningPreset.WeldAmplitudeLoopC2 = %d\n",
				      RunningPreset.WeldAmplitudeLoopC2);
				pprintf(" RunningPreset.WeldRampTime = %d\n",
				      RunningPreset.WeldRampTime);
				pprintf(" RunningPreset.WeldAmplitude = %d(%%) \n",
				      RunningPreset.WeldAmplitude);
				pprintf(" RunningPreset.WeldPhaseLimit = %d\n",
				      RunningPreset.WeldPhaseLimit);
				pprintf(" RunningPreset.SWDip.SoftwareFlag = %d\n",
				      RunningPreset.SWDip.SoftwareFlag);
				pprintf(" RunningPreset.MemoryFreq = %d\n",
				      RunningPreset.MemoryFreq);
				pprintf(" RunningPreset.DigitalTune = %d\n",
				      RunningPreset.DigitalTune);
				pprintf(" RunningPreset.Weldmode = %d\n", RunningPreset.WeldMode);
				pprintf("RunningPreset.EnergyCP = %d\n", RunningPreset.EnergyCP);
				break;
			case 'C':
				pprintf("\n cold start");
				ColdStart();
				pprintf("\n Please power cycle the system");
				break;
			case 'G':
				pprintf("\n Clear software flag");
				CurrentPreset.SWDip.SoftwareFlag = 0;
				SavePreset(0);
				break;
			case 'H':
				pprintf("\n Enter 1 to PowerUpSeek ");
				pprintf("\n Enter 2 to PowerUpScan ");
				pprintf("\n Enter 4 to TimedSeek ");
				pprintf("\n Enter 8 to  AutoSonics ");
				pprintf("\n Enter 16 to  ExtFrequency");
				pprintf("\n Enter 32 to  ExtAmplitude");
				pprintf("\n Enter 64 to  EndOfWeldStore");
				pprintf("\n enter software flag = ");
				memset(data1, 0, sizeof(data1));
				gets(data1);
				CurrentPreset.SWDip.SoftwareFlag |= atoi(data1);
				SavePreset(0);
				break;
			case 'I':
				pprintf("\n CycleCounter=%d", ReadCycleCounter());
				pprintf("\n ErrorCounter=%d", ReadErrorCounter());
				pprintf("\n PowerOnSecCounter=%d", ReadPowerOnSecCounter());
				pprintf("\n SonicOnSecCounter=%d", ReadSonicOnSecCounter());
				break;
			case 'N':
				printf("\n phase points");
				for (UINT16 i = 0; i < NumScanPoints; i++) {
					DelayMs(50);
					printf("\n Phs %d,Phase %d, freq %d, Amp %d, Cur %d,",
					      (SINT16) HornScanRawPhase[i], (SINT16) ScanBuffer[i].Phase,
					      (SINT32) ScanBuffer[i].Frequency,
					      (SINT16) ScanBuffer[i].Amplitude,
					      (SINT16) ScanBuffer[i].Current);
				}
				break;
			case 'p':
				pprintf("\n Enter Power : ");
				memset(data1, 0, sizeof(data1));
				gets(data1);
				PowerInput = atoi(data1);
				pprintf("\nPower :%d ", PowerInput);
				break;
			case 'P':
				pprintf("\n Read Last 5 second Weld data");
				ReadWeldBufferLog(WeldBufferIndex);
				break;
			case 'Q':
				NumbeOfLog = Log::ReadEventLog(templogdata,
				      Log::GetLatestEventEntryIndex(), 100);
				pprintf("\n number of Event LOG Entry = %d", NumbeOfLog);
				pprintf("\n eventNo    logStr\n");
				for (SINT32 i = 0; i < NumbeOfLog; i++) {
					DelayMs(10);
					pprintf("\n %d    %s", templogdata[i].eventNo,
					      templogdata[i].logStr);
				}
				break;
			case 'S':
				NumbeOfLog = Log::ReadAlarmLog(templogdata,
				      Log::GetLatestAlarmEntryIndex(), 100);
				pprintf("\n number of Alarm LOG Entry = %d", NumbeOfLog);
				pprintf("\n alarmeventNo    logStr\n");
				for (SINT32 i = 0; i < NumbeOfLog; i++) {
					DelayMs(10);
					pprintf("\n %d    %s", templogdata[i].eventNo,
					      templogdata[i].logStr);
				}
				break;
			case 'U':
				pprintf("\n Disabling Special.");
				EPromData.EnergyPowerSpecial = FALSE;
				ModifyEEPROMdata();
				pprintf("\n Special is disabled.");
				break;
			case 'v':
				pprintf(
				      "\n Phs = %d\t Amp = %d,\t Curr = %d,\t Pwr = %d,\t Temp = %d",
				      PhaseInput, ActualAmplitude, CurrentInput, PowerInput,
				      InputTemperature);
				break;
			case 'V':
				pprintf("\n Software version string of controller code = ");
				for (UINT8 i = 0; i < SWVERSION_LEN; i++)
					pprintf("%c", SWVersionLabel[i]);
				ReadFPGAVersion();
				break;
			case 'X':
				pprintf("\n Please Enter Power Supply frequency : ");
				memset(data1, 0, sizeof(data1));
				gets(data1);
				currFreq = EPromData.PSfrequency;
				EPromData.PSfrequency = atoi(data1);

				pprintf("\n Please Enter Power Supply Power : ");
				memset(data1, 0, sizeof(data1));
				gets(data1);
				EPromData.PSpower = atoi(data1);
				ModifyEEPROMdata();
				if (currFreq != EPromData.PSfrequency) {
					UpdateMidBandAndFFactor();
					SetDefaultWeldPreset();
					SetMaxMinValues();
					SetMinMaxScanParameter();
				}
				break;
			case 'Y':
				pprintf("\n Power Supply Specification : ");
				pprintf("\n Power Supply frequency : %d", EPromData.PSfrequency);
				pprintf("\n Power Supply power : %d", EPromData.PSpower);
				pprintf("\n DCX Power supply Flash CRC : %d", sysConfig.DCXCRC);
				break;
			case 'Z':
				pprintf(
				      "\n CurrentPreset.MemoryFreq = %d ,\n RunningPreset.MemoryFreq=%d , \n MemoryValue=%d , \n CurrentPreset.DigitalTune=%d , \n RunningPreset.DigitalTune=%d  ",
				      CurrentPreset.MemoryFreq, RunningPreset.MemoryFreq,
				      MemoryValue, CurrentPreset.DigitalTune,
				      RunningPreset.DigitalTune);
				break;
			default:
				pprintf("NotSupported %c", ch[4]);
				break;
			}
		} else {
			pprintf("\nWI=%c,%c,%c,%c,%c,%c", ch[0], ch[1], ch[2], ch[3], ch[4],
			      ch[5]);
			UINT32 UID = ((ch[0] << 8) | (ch[1] & 0x0FF));
			SINT32 data1 = ((ch[2] << 8) | (ch[3] & 0x0FF));
			SINT32 data2 = ((ch[4] << 8) | (ch[5] & 0x0FF));
			Log::WriteEventLog(BAD_SERIALFORMAT, data1, data2, UID, true);
		}
	}
}
