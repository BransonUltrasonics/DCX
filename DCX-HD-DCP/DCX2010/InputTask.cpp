/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/InputTask.cpp_v   1.22.1.2.1.0   17 Mar 2016 16:23:40   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/InputTask.cpp_v  $
 * 
 *    Rev 1.22.1.2.1.0   17 Mar 2016 16:23:40   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.22.1.3   17 Mar 2016 16:07:20   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.22.1.2   02 Sep 2014 06:58:38   amaurya
 * Changed for Digital Tune.
 * 
 *    Rev 1.22.1.1   14 Feb 2013 08:53:04   amaurya
 * Added Energy Special.
 * 
 *    Rev 1.22.1.0   13 Feb 2013 09:19:54   amaurya
 * Added Graphs, Energy special and rescaling special.
 * 
 *    Rev 1.23   15 Jan 2013 04:40:46   amaurya
 * Energy Special.
 * 
 *    Rev 1.21.1.0   07 Sep 2012 08:54:24   amaurya
 * Fixed resonance frequency issue of Horn Scan.
 * 
 *    Rev 1.21   16 Sep 2011 08:25:30   RatneshA
 * corrected code formatting
 * 
 *    Rev 1.20   06 Sep 2011 14:19:26   ygupta
 * Added extra security to remove the chances of getting garbage from Serial port by adding 5 character command string.
 * 
 *    Rev 1.19   01 Sep 2011 13:06:00   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.18   28 Aug 2011 11:42:24   ygupta
 * Ver1.01M
 * 
 *    Rev 1.17   16 Aug 2011 08:24:06   pdwivedi
 * code cleanup
 * 
 *    Rev 1.16   05 Aug 2011 10:32:34   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.15   02 Aug 2011 07:37:52   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.14   18 Jul 2011 09:25:26   pdwivedi
 * Added changes for serial buffer size.
 * 
 *    Rev 1.13   15 Jul 2011 09:45:50   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.12   02 May 2011 09:27:34   ygupta
 * Manufacturing Bug fixes
 * 
 *    Rev 1.11   27 Apr 2011 10:47:24   PDwivedi
 * minor bug fixes, system info page GUI updated for max serail number,Debug re enabled
 * 
 *    Rev 1.10   25 Apr 2011 08:44:16   PDwivedi
 * Changes after MAC chip addition
 * 
 *    Rev 1.9   02 Mar 2011 08:36:50   PDwivedi
 * changes due TAR size removed from sysconfig structure
 * 
 *    Rev 1.8   16 Dec 2010 08:17:40   PDwivedi
 * Added changes for Website Code Clean Up and Compressed file download from serial.
 * 
 *    Rev 1.7   14 Dec 2010 11:15:10   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.6   09 Dec 2010 07:54:26   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.5   06 Dec 2010 09:56:54   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.4   29 Nov 2010 07:47:34   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.3   22 Nov 2010 09:04:24   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.1   12 Nov 2010 10:59:00   nhahmed
 * Added Different function cases for debugging.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:42   ygupta
 * Initial revision.

******************************************************************************/

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
#include "Sysconfig.h"
extern SINT16 HornScanRawTestPhase[DCP_HORNSCANMAXPOINTS];
extern BOOLEAN writeValue;
extern BOOLEAN ReadValue;
extern MCP3208 * adc0, *adc1;
extern SPI * spi;
extern SINT16      CurrentIndex;
extern volatile uint16 * cs0;
extern volatile uint16 * cs1;
extern volatile uint8 * cs2;
extern volatile uint8 * cs3;
extern SINT32	*AnalogInValue[NUM_AIN];
extern SINT32 freqIn, amplitudeIn;
SPI * spi;
DAC * dac;
int mV[4];
struct AdcBuf;
extern char FPGAVersionLabel[FPGASWVERSION_LEN];
LogData templogdata[100];
extern SINT16 *HornScanRawPhase;
InputTask::InputTask() :
	Task("Input", 3072)
{
}

void InputTask::PrintHelp()
{
	printf("\n\n");
	printf("? - This help\n");
	printf("M - Print memory allocation\n");
	printf("T - Show task status\n");
	printf("F - ReFlash Board\n");
	printf("r - Read flash chip\n");
	printf("1 - Run tests\n");
	printf("\n\n");
}

void EraseCallback(int offset, int length)
{
	pprintf("Erase: %08X of %08X\n", offset, length);
}

void ProgramCallback(int offset, int length)
{
	if (((offset & 0xFFFF) == 0) || (offset == length))
		pprintf("Program: %08X of %08X\n", offset, length);
}

void InputTask::ProgramFlash(int intf)
{
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
	CyGOS::TftpReceive(intf, fileName, &length, data , srvr);
	pprintf("Length %d\n", length);
	if (length > 0)
	{
		pprintf("Programming Address %08X\n", __FLASH_ROM);
		pprintf("Flash %08X\n", flash);
		flash->Unlock(__FLASH_ROM, length);
		flash->Erase(__FLASH_ROM, length, EraseCallback);
		flash->Program(__FLASH_ROM, data, length, ProgramCallback);
		flash->Lock(__FLASH_ROM, length);
//		pprintf("Programmed, restarting...\n");
		DelayMs(1000);
//		CPU::Restart();
		Log::WriteEventLog(PROGRAM_FLASH, (unsigned short)(FLASHCODE_PROGARM) , 0, 0, false);
		pprintf("\n Waiting for Restart after flash program...\n ");
		CyGOS::DisableWatchDog();
		CyGOS::Stop();
		while(1);
	}
	delete data;
	delete flash;
}

void InputTask::ProgramCompressedTarRegionFromSerial()
{
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
	while(true)
	{
		if(firstByteReceived)
		{
			ch = getcharWait(500);
			if(ch == (-1))
			{
				pprintf("\n receive timeout");
				break;
			}
			else
			{
				data[length++] = ch;
				pprintf(".");
			}
		}
		else
		{
			ch = getchar();
			data[length++] = ch;
			firstByteReceived = true;
		}
	}
	pprintf("Length %d\n", length);
	if ((length > (50 * 1024)) && (length < ((int) __TAR_SIZE)))
	{
		pprintf("Programming Address %08X\n", __TAR_START);
		pprintf("Flash %08X\n", flash);
		flash->Unlock(__TAR_START, length);
		flash->Erase(__TAR_START, length, EraseCallback);
		flash->Program(__TAR_START, data, length, ProgramCallback);
		flash->Lock(__TAR_START, length);
//		pprintf("Programmed, restarting...\n");
		DelayMs(3000);
//		CPU::Restart();
		Log::WriteEventLog(PROGRAM_FLASH, (unsigned short)(FLASHTAR_PROGRAM) , 0, 0, false);
		pprintf("\n Waiting for Restart after flash program...\n ");
	     CPU::Restart();
		CyGOS::DisableWatchDog();
		CyGOS::Stop();
		while(1);
	}
	pprintf("\n invalid file length");
	delete data;
	delete flash;
   CyGOS::StartTimer(1);
   CyGOS::EnableWatchDog();
}

void InputTask::ProgramCompressedTarRegionFlash(int intf)
{
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
	for (int i = 0; i < 5; i++)
	sprintf(fileName + i * 2 + strlen("web"), "%02X", __ETHERNET_MAC[i]);
	strcat(fileName, ".cyg");

	printf("Receiving file '%s':...", fileName);

	Flash * flash = Board::GetFlash();
	ASSERT(flash);

	int length = (int) __TAR_SIZE;
	uint8 * data = new uint8[length];

	printf("Receiving file '%s':...", fileName);
	CyGOS::TftpReceive(intf, fileName, &length, data , srvr);
	pprintf("Length %d\n", length);

	if (length > 0)
	{
		pprintf("Programming Address %08X\n", __TAR_START);
		pprintf("Flash %08X\n", flash);
		flash->Unlock(__TAR_START, length);
		flash->Erase(__TAR_START, length, EraseCallback);
		flash->Program(__TAR_START, data, length, ProgramCallback);
		flash->Lock(__TAR_START, length);
//		pprintf("Programmed, restarting...\n");
		DelayMs(3000);
//		CPU::Restart();
		Log::WriteEventLog(PROGRAM_FLASH, (unsigned short)(FLASHTAR_PROGRAM) , 0, 0, false);
		pprintf("\n Waiting for Restart after flash program...\n ");
		CyGOS::DisableWatchDog();
		CyGOS::Stop();
		while(1);
	}
	delete data;
	delete flash;
}

void InputTask::Run()
{
	DelayMs(500);
	PrintHelp();
	int NumbeOfLog = 0;
	char data1[10];
	UINT32 currFreq;
	UINT32 testCyTime = 0;
	for (;;)
	{
		//Increased Security against the noise on receiver line and garbage coming.
		// Command will work As:
		//PASSx
		//Where x is the command option character and than carriage return.
		int ch[6];
		ch[0] = getchar();//P
		ch[1] = getchar();//A
		ch[2] = getchar();//S
		ch[3] = getchar();//S
		ch[4]  = getchar();//Command option
		ch[5] = getchar();//Carriage return
		if((ch[0] == 'P')&&(ch[1] == 'A')&&(ch[2] == 'S')&&(ch[3] == 'S')&&(ch[5] == '\r'))
		{
			switch (ch[4])
			{
				 case '?':
					PrintHelp();
				 break;
				 case 'R':
		           CPU::Restart();
				 break;
				 case 'T':
					CyGOS::PrintStatus(PrintTasks);
				 break;
				 case 'M':
					ShowMemory();
					break;
				 case 'F':
					 sysConfig.currNetworkConfig = configDhcpClient;
					 pprintf("ip changed");
					 CPU::Restart();
					//ProgramFlash(0);
					break;
				 case 'f':
				   // if (Board::EthCount() == 2)
					  // ProgramFlash(1);
					 pprintf("\n FPGA->st %08X %d ", &FPGA->st , &FPGA->st);
					 pprintf("\n FPGA->aui8Dummy %08X %d", FPGA->aui8Dummy , FPGA->aui8Dummy);
					 pprintf("\n FPGA->st.stRCyc %08X %d", &FPGA->st.stRCyc, &FPGA->st.stRCyc);
					 pprintf("\n FPGA->st.stRUsOff %08X %d", &FPGA->st.stRUsOff, &FPGA->st.stRUsOff);
					 pprintf("\n FPGA->st.stWCyc %08X %d", &FPGA->st.stWCyc , &FPGA->st.stWCyc);
					 pprintf("\n FPGA->st.stWInit %08X %d", &FPGA->st.stWInit, &FPGA->st.stWInit);
					 pprintf("\n FPGA->st.stWUsOff %08X %d", &FPGA->st.stWUsOff , &FPGA->st.stWUsOff);
					 pprintf("\n FPGA->st.ui32TestRegister %08X %d", &FPGA->st.ui32TestRegister, &FPGA->st.ui32TestRegister);
					 pprintf("\n FPGA->st.st.stRCyc.ui8GPI %08X %d", &FPGA->st.stRCyc.ui8GPI , &FPGA->st.stRCyc.ui8GPI);

					break;
				 case '3':
					printf("\n enter scan amp in percentage");
					memset(data1, 0, sizeof(data1));
					gets(data1);
					ParaScan.MaxAmplitude = atoi(data1);
					ParaScan.MaxCurrent =  atoi(data1);
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
					for(int i=0;i<6;i++)
					  {
					   DelayMs(50);
					   printf(" %4d",(int)ResonanceFreqs[i]);
					  }
					break;
				 case '5':
					pprintf("\n FreqOffsetInput =%d",FreqOffsetInput);
					pprintf("\n ExtAmplitudeInput =%d",ExtAmplitudeInput);
					break;
				 case '7':
					ProgramCompressedTarRegionFromSerial();
				     CPU::Restart();
					break;
				 case '8':
					memset(data1, 0, sizeof(data1));
					gets(data1);
					testCyTime = (UINT32)atoi(data1);
					pprintf("\n testCyTime = %d \n", SCALEDTIMETOWEBSITE(testCyTime));
					break;
				 case '9':
					MFCycleFlag = FALSE;
					memset(&MFcycleResults, 0 , sizeof(MFcycleResults));
					MFcycleResults.currCycleTime = 3600000;
					SonicsOffCounter = 0;
					MFcycleResults.nCycles = 18000;
					MFcycleResults.sonicsOnTime = 1800000;
					SonicsOnCounter = 0;
					MFCycleFlag = TRUE;
					//DecompressTarRegionFlash('A');
					break;
				 case 'w':
					pprintf("\n CurrentIndex=%d",CurrentIndex);
					DebugPCBs();
					break;
				 case '*':
					printf(" * ");
					printf("\n");
					pprintf(" RunningPreset.SeekAmplitude = %d (%%) \n",RunningPreset.SeekAmplitude);
					pprintf(" RunningPreset.SeekRampTime = %d\n",RunningPreset.SeekRampTime);
					pprintf(" RunningPreset.SeekTime = %d\n",RunningPreset.SeekTime);
					pprintf(" RunningPreset.SeekFrequLimitHigh = %d\n",RunningPreset.SeekFrequLimitHigh);
					pprintf(" RunningPreset.SeekFrequLimitLow = %d\n",RunningPreset.SeekFrequLimitLow);
					pprintf(" RunningPreset.SeekPhaseloopI = %d\n",RunningPreset.SeekPhaseloopI);
					pprintf(" RunningPreset.SeekPhaseloopCF = %d\n",RunningPreset.SeekPhaseloopCF);
					pprintf(" RunningPreset.SeekAmplitudeLoopC1 = %d\n",RunningPreset.SeekAmplitudeLoopC1);
					pprintf(" RunningPreset.SeekAmplitudeLoopC2 = %d\n",RunningPreset.SeekAmplitudeLoopC2);
					pprintf(" RunningPreset.WeldPhaseloopI = %d\n",RunningPreset.WeldPhaseloopI);
					pprintf(" RunningPreset.WeldPhaseloopCF = %d\n",RunningPreset.WeldPhaseloopCF);
					pprintf(" RunningPreset.WeldAmplitudeLoopC1 = %d\n",RunningPreset.WeldAmplitudeLoopC1);
					pprintf(" RunningPreset.WeldAmplitudeLoopC2 = %d\n",RunningPreset.WeldAmplitudeLoopC2);
					pprintf(" RunningPreset.WeldRampTime = %d\n",RunningPreset.WeldRampTime);
					pprintf(" RunningPreset.WeldAmplitude = %d(%%) \n",RunningPreset.WeldAmplitude);
					pprintf(" RunningPreset.WeldPhaseLimit = %d\n",RunningPreset.WeldPhaseLimit);
					pprintf(" RunningPreset.SWDip.SoftwareFlag = %d\n",RunningPreset.SWDip.SoftwareFlag);
					pprintf(" RunningPreset.MemoryFreq = %d\n",RunningPreset.MemoryFreq);
					pprintf(" RunningPreset.DigitalTune = %d\n",RunningPreset.DigitalTune);
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
					pprintf("\n CycleCounter=%d",ReadCycleCounter());
					pprintf("\n ErrorCounter=%d",ReadErrorCounter());
					pprintf("\n PowerOnSecCounter=%d",ReadPowerOnSecCounter());
					pprintf("\n SonicOnSecCounter=%d",ReadSonicOnSecCounter());
					break;
				 case 'N':
					printf("\n phase points");
					for(int i=0;i<NumScanPoints;i++)
					{
					   DelayMs(50);
					   printf("\n Phs %d,Phase %d, freq %d, Amp %d, Cur %d,",(int)HornScanRawPhase[i],(int)ScanBuffer[i].Phase,(int)ScanBuffer[i].Frequency ,(int)ScanBuffer[i].Amplitude,(int)ScanBuffer[i].Current);
					}
					break;
				 case 'p':
					 pprintf("\n Enter Power : ");
					 memset(data1, 0, sizeof(data1));
					 gets(data1);
					 PowerInput = atoi(data1);
					 pprintf("\nPower :%d ",PowerInput);
					 break;
				 case 'P':
					 pprintf("\n Read Last 5 second Weld data");
					 ReadWeldBufferLog(WeldBufferIndex);
					 break;
				 case 'Q':
					NumbeOfLog = Log::ReadEventLog(templogdata, Log::GetLatestEventEntryIndex(),100);
					pprintf("\n number of Event LOG Entry = %d",NumbeOfLog);
					pprintf("\n eventNo    logStr\n");
					for(int i=0;i < NumbeOfLog;i++)
					{
					   DelayMs(10);
					   pprintf("\n %d    %s",templogdata[i].eventNo,templogdata[i].logStr);
					}
					break;
				 case 'S':
					NumbeOfLog = Log::ReadAlarmLog(templogdata, Log::GetLatestAlarmEntryIndex(),100);
					pprintf("\n number of Alarm LOG Entry = %d",NumbeOfLog);
					pprintf("\n alarmeventNo    logStr\n");
					for(int i=0;i< NumbeOfLog;i++)
					{
					   DelayMs(10);
					   pprintf("\n %d    %s",templogdata[i].eventNo,templogdata[i].logStr);
					}
					break;
				 case 'U':
					 pprintf("\n Disabling Special.");
					 ePromData.EnergyPowerSpecial = FALSE;
					 ModifyEEPROMdata();
					 pprintf("\n Special is disabled.");
					 break;
				 case 'V':
					pprintf("\n Software version string of controller code = ");
					for (int i=0; i< SWVERSION_LEN; i++)
					   pprintf("%c", SWVersionLabel[i]);
					ReadFPGAVersion();
					break;
				 case 'X':
					pprintf("\n Please Enter Power Supply frequency : ");
					memset(data1, 0, sizeof(data1));
					gets(data1);
					currFreq = ePromData.PSfrequency;
					ePromData.PSfrequency = atoi(data1);

					pprintf("\n Please Enter Power Supply Power : ");
					memset(data1, 0, sizeof(data1));
					gets(data1);
					ePromData.PSpower = atoi(data1);
					ModifyEEPROMdata();
					if(currFreq != ePromData.PSfrequency)
					{
					   UpdateMidBandAndFFactor();
					   SetDefaultWeldPreset(TRUE);
					   SetMaxMinValues();
					   SetMinMaxScanParameter();
					}
					break;
				 case 'Y':
					pprintf("\n Power Supply Specification : ");
					pprintf("\n Power Supply frequency : %d",ePromData.PSfrequency);
					pprintf("\n Power Supply power : %d",ePromData.PSpower);
					pprintf("\n DCX Power supply Flash CRC : %d",sysConfig.DCXCRC);
					break;
				 case 'Z':
					pprintf("\n CurrentPreset.MemoryFreq = %d ,\n RunningPreset.MemoryFreq=%d , \n MemoryValue=%d , \n CurrentPreset.DigitalTune=%d , \n RunningPreset.DigitalTune=%d  ",
					   CurrentPreset.MemoryFreq,RunningPreset.MemoryFreq,MemoryValue,CurrentPreset.DigitalTune,RunningPreset.DigitalTune);
					break;
				 default:
					 pprintf("NotSupported %c",ch[4]);
					 break;
			}
		}
		else
		{
			pprintf("\nWI=%c,%c,%c,%c,%c,%c",ch[0],ch[1],ch[2],ch[3],ch[4],ch[5]);
			unsigned short UID = ((ch[0]<<8)|(ch[1] & 0x0FF));
			unsigned short data1 = ((ch[2]<<8)|(ch[3] & 0x0FF));
			unsigned short data2 = ((ch[4]<<8)|(ch[5] & 0x0FF));
			Log::WriteEventLog(BAD_SERIALFORMAT, data1 , data2, UID, true);
		}
	}
}
