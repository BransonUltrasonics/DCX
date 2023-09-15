/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ReceiveCommandHandler.cpp_v   1.17.1.1   05 Jun 2014 11:04:10   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ReceiveCommandHandler.cpp_v  $
 * 
 *    Rev 1.17.1.1   05 Jun 2014 11:04:10   amaurya
 * Removed extra ;
 * 
 *    Rev 1.17.1.0   30 Apr 2014 06:19:18   amaurya
 * Added change for Digital Tune
 * 
 *    Rev 1.17   15 Jan 2013 04:41:42   amaurya
 * Energy Special.
 * 
 *    Rev 1.13.1.12   16 Sep 2011 08:08:54   RatneshA
 * changes regarding reg 115 option 1 and 2
 * 
 *    Rev 1.13.1.11   01 Sep 2011 13:06:10   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.13.1.10   28 Aug 2011 11:42:26   ygupta
 * Ver1.01M
 * 
 *    Rev 1.13.1.9   16 Aug 2011 08:26:24   pdwivedi
 * code cleanup
 * 
 *    Rev 1.13.1.8   08 Aug 2011 05:47:16   pdwivedi
 * Added changes for  Private IP addresses.
 * 
 *    Rev 1.13.1.7   05 Aug 2011 10:33:36   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.13.1.6   02 Aug 2011 07:38:56   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.13.1.5   18 Jul 2011 09:25:52   pdwivedi
 * Added changes for serial buffer size.
 * 
 *    Rev 1.13.1.4   15 Jul 2011 09:47:28   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.13.1.3   21 Jun 2011 11:07:20   PDwivedi
 * Fixes after mongle horde round1
 * 
 *    Rev 1.13.1.2   02 May 2011 09:28:00   ygupta
 * Manufacturing Bug fixes
 * 
 *    Rev 1.13.1.1   27 Apr 2011 10:47:52   PDwivedi
 * minor bug fixes, system info page GUI updated for max serail number,Debug re enabled
 * 
 *    Rev 1.13.1.0   26 Apr 2011 13:12:56   PDwivedi
 * System box in Sytem Information page now displays LCD code version and CRC
 * 
 *    Rev 1.13   21 Apr 2011 11:08:00   PDwivedi
 * modified Ver1.0T for tracker issues
 * 
 *    Rev 1.12   20 Apr 2011 11:38:10   ygupta
 * Added changes for tracker issues.
 * 
 *    Rev 1.11   19 Apr 2011 10:48:24   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.10   14 Apr 2011 06:22:46   PDwivedi
 * Added changes for IP Setup issue and added comments.
 * 
 *    Rev 1.9   04 Apr 2011 11:03:00   PDwivedi
 * Display lock during manufacturing cycle
 * 
 *    Rev 1.8   02 Mar 2011 08:22:38   PDwivedi
 * Added support for Level2.
 * 
 *    Rev 1.7   14 Jan 2011 07:55:44   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.6   14 Dec 2010 11:15:12   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.5   09 Dec 2010 07:54:26   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.4   07 Dec 2010 08:58:20   PDwivedi
 * Added changes for REV Board C , code clean up.
 * 
 *    Rev 1.3   06 Dec 2010 09:56:56   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.2   29 Nov 2010 07:47:38   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.1   22 Nov 2010 09:04:24   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:46   ygupta
 * Initial revision.

******************************************************************************/

#include "ReceiveCommandHandler.h"
#include "stdio.h"
#include "Globals.h"
#include "Terminal.h"
#include "UserIO.h"
#include "Func.h"
#include "LOG.h"
#include "Preset.h"
#include "SysConfig.h"
#include "LED.h"

//#define pprintf
//#define printf

/*--------------------Global Variables-------------------------------------*/
ReceiveCommandHandler *ReceiveCommandHandler::thisptr;
BOOLEAN regUpdateCmdFlag = TRUE;
BOOLEAN regAmplitudeUpdateFlag = FALSE;
UINT8 serialRecBuff[RXBUFSIZE];
UINT8 serialBuff[TXBUFSIZE];
UINT16 LCDKeyStatus = 0;
LCDData ReceiveCommandHandler::lcdData;
DCPSTATUS ReceiveCommandHandler::DCPStatus;
UINT16 ReceiveCommandHandler::LCDSWVersion;
UINT8 ReceiveCommandHandler::LCDCodeCRCLow;
UINT8 ReceiveCommandHandler::LCDCodeCRCHigh;
/*--------------------------------------------- ---------------------------*/

 //For testing only.
SerCmdData  SerialCmdReg[TOTAL_SERIALCOMM] =
{
		{regAlarmResetReq	, &(ReceiveCommandHandler::lcdData.AlarmResetReq)    ,   2 },
		{regExAmplitude	, &(ReceiveCommandHandler::lcdData.ExAmplitude)      ,   2 },
		{regWeldRampTime  , &(ReceiveCommandHandler::lcdData.weldRampTime)     ,   2 },
		{regMemStore	   , &(ReceiveCommandHandler::lcdData.MemStoreAtWeldEnd),   2 },
		{regPwrUpSeekScan , &(ReceiveCommandHandler::lcdData.PwrUpSeek)        ,   2 },
		{regSeekRampTime  , &(ReceiveCommandHandler::lcdData.seekRampTime)     ,   2 },
		{regTimedSeek     , &(ReceiveCommandHandler::lcdData.TimedSeek)        ,   2 },
		{regSeekTime	   , &(ReceiveCommandHandler::lcdData.seekTime)		     ,   2 },
		{regExtFreq       , &(ReceiveCommandHandler::lcdData.ExtFreq)          ,   2 },
//		{regAutoSonics    , &(ReceiveCommandHandler::lcdData.AutoSonics)       ,   2 },
		//Reserved1
		//Reserved2
		{regIP1	  		   , &(ReceiveCommandHandler::lcdData.IP1)              ,   2 },
		{regIP2	         , &(ReceiveCommandHandler::lcdData.IP2)              ,   2 },
		{regIP3	         , &(ReceiveCommandHandler::lcdData.IP3)              ,   2 },
		{regIP4	         , &(ReceiveCommandHandler::lcdData.IP4)              ,   2 },
		{regGateWay1	   , &(ReceiveCommandHandler::lcdData.GateWay1)         ,   2 },
		{regGateWay2	   , &(ReceiveCommandHandler::lcdData.GateWay2)         ,   2 },
		{regGateWay3	   , &(ReceiveCommandHandler::lcdData.GateWay3)         ,   2 },
		{regGateWay4	   , &(ReceiveCommandHandler::lcdData.GateWay4)         ,   2 },
		{regSubNetMask1	, &(ReceiveCommandHandler::lcdData.SubNetMask1)      ,   2 },
		{regSubNetMask2	, &(ReceiveCommandHandler::lcdData.SubNetMask2)      ,   2 },
		{regSubNetMask3	, &(ReceiveCommandHandler::lcdData.SubNetMask3)      ,   2 },
		{regSubNetMask4	, &(ReceiveCommandHandler::lcdData.SubNetMask4)      ,   2 },
		{regIPSetUp	   ,  &(ReceiveCommandHandler::lcdData.IPSetUp)        	,   2 },
		{regAmplitude	, &(ReceiveCommandHandler::lcdData.Amplitude)        ,   2 },
		{regUnused        , 0                                                  ,   0 },
};

/*-------------------------- External variables --------------------------*/
extern UINT32 TempPeakPower;
/*--------------------------------------------- ---------------------------*/

ReceiveCommandHandler::ReceiveCommandHandler(int intf) :
	Task("RxHandler", RECEIVECOMMHANDLER_STACKSIZE), port(
			1536, 1536)
{
	thisptr = this;
	interface = intf;
	cmdReceived = FALSE;
	lcdCommunicationLost = TRUE;
	systemInfoReceived = FALSE;
}

void ReceiveCommandHandler::Run()
/****************************************************************************/
/* void Run ()                                                              */
/* Description:-												 								 */
/* This function is responsible for receiving LCD command in Motorola S9    */
/* record format and process it accordingly.                                */
/****************************************************************************/
{
	DelayMs(1000);
	port.Init(0 , LCDBAUDRATE, 3, 3, FALSE);
	printf("\n Starting ReceiveCommandHandler...\n");

	static enum ReceiveState recState = STATE_WAIT_START;
	static int chCnt = 0;
	static UINT16 LEDState = REDLED_STATE;//Red
	LED::Set(2,LEDState);
	while(FOREVER)
	{
		unsigned char ch2[2];
		ch2[1] = 0;
		char ch;
		port.Receive(this , ch2[0], -1);
		ch = ch2[0];

		switch(recState)
		{
			case STATE_WAIT_START:
			if(ch == 'S')
			{
				chCnt = 0;
				serialRecBuff[chCnt] = ch;
				chCnt++;
				recState = STATE_RECEIVE;
			}
			break;
			case STATE_RECEIVE:
				serialRecBuff[chCnt] = ch;
				chCnt++;
				if(chCnt >= RXBUFSIZE)
				{
				chCnt = 0;
				recState = STATE_WAIT_START;
				}
				else if(ch == CR)
					recState = STATE_WAIT_LF;
			break;
			case STATE_WAIT_LF:
				if(ch == LF)
				{
					serialRecBuff[chCnt] = ch;
					cmdReceived = TRUE;
					ProcessCommand(serialRecBuff);
					chCnt = 0;
					recState = STATE_WAIT_START;
					LED::Set(2,LEDState);
					if(LEDState == REDLED_STATE)
						LEDState = GREENLED_STATE;
					else
						LEDState = REDLED_STATE;
				}
			break;
			default:
				chCnt = 0;
				recState = STATE_WAIT_START;
		}
	}
}

UINT8 ReceiveCommandHandler::VerifyCheckSum(UINT8 * data, UINT8 dataLen)
/****************************************************************************/
/* void VerifyCheckSum ()                                                   */
/* Description:-												 								 */
/* This function verify Check Sum of LCD command.                           */
/* Input parameters :                                                       */
/* data : indicates pointer to LCD command on which checksum needs to be    */
/* calculated.                                                              */
/* dataLen : indicates length of data buffer on which Checksum is calculated*/
/****************************************************************************/
{
	UINT8 receivedCRC, calculatedCRC = 0, CRCTemp, indx, dataIndx = 0;
	for(indx = 0; indx < dataLen; indx++)
	{
		AsciiToHex(&CRCTemp, sizeof(CRCTemp), &data[dataIndx]);
		calculatedCRC ^= CRCTemp;
		dataIndx += (sizeof(CRCTemp) * 2);
	}
	AsciiToHex(&receivedCRC, sizeof(receivedCRC), &data[dataIndx]);

	return (receivedCRC ^ calculatedCRC);
}

void ReceiveCommandHandler::ProcessCommand(UINT8 * LcdCommand)
/****************************************************************************/
/* void ProcessCommand ()                                                   */
/* Description:-												 								 */
/* This function processes polling and data command .                       */
/* Input parameters :                                                       */
/* LcdCommand : indicates pointer to LCD command.                           */
/****************************************************************************/
{
	UINT8 totalBytes;
	UINT16 addr,RegAddr;
	UINT8 buffIndex = 1;
	UINT8 byteCount = 0;
	UINT16 regValue;
	UINT8 crcByte;
	UINT8 VerifyChkSum,Cmd;
	static UINT8 updateCounter = 0;

	if(LcdCommand[buffIndex] == polling)//every reply is the reply of POLLING
	{
		buffIndex++;
		AsciiToHex(&totalBytes, sizeof(totalBytes), &LcdCommand[buffIndex]);
		VerifyChkSum = VerifyCheckSum(&LcdCommand[buffIndex], totalBytes);
		if(VerifyChkSum == 0){
		buffIndex += (sizeof(totalBytes) * 2);
		AsciiToHex(&addr, sizeof(addr), &LcdCommand[buffIndex]);//Addr is the Key status in Polling.
		buffIndex += (sizeof(addr) * 2);
		switch (addr)
		{
			case KB_RESET:
				LCDKeyStatus = KB_RESET;
			break;
			case KB_TEST:
				LCDKeyStatus = KB_TEST;
			break;
			default:
				LCDKeyStatus = 0;
			break;
		}
		AsciiToHex(&Cmd, sizeof(Cmd), &LcdCommand[buffIndex]);
		buffIndex += (sizeof(Cmd) * 2);
		AsciiToHex(&RegAddr, sizeof(RegAddr), &LcdCommand[buffIndex]);//Addr is the Key status in Polling.
		buffIndex += (sizeof(RegAddr) * 2);
		if(lcdCommunicationLost)
		{
			//Do the Log entry in the very first reply to LCD polling command
			//at powerup as well as whenever communication reestablished after
			//LCDCOMTIMEOUT period. Timeout is detected in Tick() Function.
			//Log::WriteEventLog(LCD_BOARD_DETECTED, 0, 0);
			lcdCommunicationLost = FALSE;
			systemInfoReceived = FALSE;
		}
		if((Cmd == cmdNocmd) && (regUpdateCmdFlag == TRUE))
		{
			updateCounter++;
			if(updateCounter > 2)
			{
				updateCounter = 0;
				regUpdateCmdFlag = FALSE;
			}
			TransmitPollingCommandReply(cmdGetAllRegCurrentValue,0);
			sysConfig.CurrSystemType = STD;
		}
		else if((Cmd == cmdNocmd) && (systemInfoReceived == FALSE))
		{
			TransmitPollingCommandReply(cmdGetSystemInfo, 0);
			systemInfoReceived = TRUE;
		}
		else if((Cmd == cmdNocmd) && (regAmplitudeUpdateFlag == TRUE))
		{
			TransmitPollingCommandReply(cmdGetRegValue, regAmplitude);
			regAmplitudeUpdateFlag = FALSE;
		}
		else
			TransmitPollingCommandReply(Cmd, RegAddr);
		}
		else
		{
		}
	}
	else if (LcdCommand[buffIndex] == dataCmd)
	{
		buffIndex++;
		AsciiToHex(&totalBytes, sizeof(totalBytes), &LcdCommand[buffIndex]);
		VerifyChkSum = VerifyCheckSum(&LcdCommand[buffIndex], totalBytes);
		if(VerifyChkSum == 0)
		{
			buffIndex += (sizeof(totalBytes) * 2);
			while(totalBytes > (byteCount + 1))
			{
				if(totalBytes >= byteCount + 5 )
				{
					AsciiToHex(&addr, sizeof(addr), &LcdCommand[buffIndex]);
					buffIndex += (sizeof(addr) * 2);
					byteCount += sizeof(addr);
					//read value
					AsciiToHex(&regValue, sizeof(regValue), &LcdCommand[buffIndex]);
					buffIndex += (sizeof(regValue) * 2);
					byteCount += sizeof(regValue);
					ProcessRegUpdateCommand((DataReg) addr, regValue);
				}
				else
				{
					buffIndex += 2;// totalBytes - 1;
					byteCount++;
				}
			}
			AsciiToHex(&crcByte, sizeof(crcByte), &LcdCommand[buffIndex]);
		}
		else
			pprintf("\n checksum error data cmd");
	}
	else
		pprintf(" LCD command not supported ");
}

UINT8 ReceiveCommandHandler:: PrepareDCPStatusByte()
/****************************************************************************/
/* void PrepareDCPStatusByte ()                                             */
/* Description:-												 								 */
/* This function prepares DCP status to be send to LCD.                     */
/****************************************************************************/
{
	UINT8 Status;
	//Get DCP Status.
	DCPStatus.Status.StatusFlags = 0;
	DCPStatus.Status.Flags.Alarm = UserIO::GetGenAlarmFlag();
	DCPStatus.Status.Flags.Ready = UserIO::GetReadyFlag();
	DCPStatus.Status.Flags.MemReset = MemoryReset;
	DCPStatus.Status.Flags.SonicsActive = ((OutputStatusToFPGA & SONIC_ON_OUT)? TRUE:FALSE);
	DCPStatus.Status.Flags.DispLock = (UserIO::CheckForLockOutChange() || WebSite::CheckForManufacturingFlag() || MFCycleFlag);
	Status = DCPStatus.Status.StatusFlags;
	return Status;
}

void ReceiveCommandHandler:: TransmitPollingCommandReply(UINT8 Cmd, UINT16 dataReg)
/****************************************************************************/
/* void TransmitPollingCommandReply ()                                      */
/* Description:-																				 */
/* This function prepares polling command reply to be send to LCD.          */
/* Input parameters :                                                       */
/* Cmd : it indicates Polling Response Cmd.                                 */
/* dataReg :data reg number.                                                */
/****************************************************************************/
{
	struct SerialCommand cmd;
	UINT8 currPower, peakPower,DCPStatusByte,DataByteCount;
	UINT16 MemoryFreq;

	DataByteCount = 0;
	DCPStatusByte = PrepareDCPStatusByte();

	//Convert current power input in percentage form
	currPower = (PowerInput * 100) / ADC11BITVALUE;

	if((DCPStatus.Status.Flags.SonicsActive) && (currPower <= 0))
		currPower = 1;//set min power to 1% whenever sonics are

	//Convert Peak power in percentage form
	peakPower = (TempPeakPower * 100) / ADC11BITVALUE;

	//Convert MemoryFreq
	MemoryFreq = (((CurrentPreset.MemoryFreq + CurrentPreset.DigitalTune)* F_Factor)/1000);
	UINT8 CRCbyte = 0, indx,indexer;
//	UINT16 Regaddr;
	cmd.startChar = STARTCHAR;
	cmd.recordType = polling;
	cmd.byteCount = 0;
	cmd.addr = 0;
	CRCbyte ^= ((cmd.addr & 0xFF00) >> 8);
	CRCbyte ^= (cmd.addr & 0x00FF);
	cmd.byteCount += sizeof(cmd.addr);
	cmd.data[DataByteCount++] = DCPStatusByte;
	cmd.data[DataByteCount++] = currPower;
	cmd.data[DataByteCount++] = peakPower;
	cmd.data[DataByteCount++] = MemoryFreq >> 8;
	cmd.data[DataByteCount++] = MemoryFreq;
	
	switch(Cmd)
	{
		case cmdNocmd:
			cmd.data[DataByteCount++] = cmdNocmd;
			cmd.data[DataByteCount++] = 0x00;
			cmd.data[DataByteCount++] = 0x00;
		break;
		case cmdGetRegValue:
			UpdateLCDData();
			indexer = 0;
			cmd.data[DataByteCount++]= Cmd;
			for(int i =0; SerialCmdReg[i].data; i++)
			{
				if(SerialCmdReg[i].dataReg == dataReg)
				{
					cmd.data[DataByteCount++] = (SerialCmdReg[i].dataReg >> 8) & 0xFF;
					cmd.data[DataByteCount++] = (SerialCmdReg[i].dataReg) & 0xFF;
					for(int j =0; j < SerialCmdReg[i].dataLen; j++)
						cmd.data[DataByteCount++] = ((UINT8*)SerialCmdReg[i].data)[j];
				}
			}
		break;
		case cmdGetAllRegCurrentValue:
			UpdateLCDData();		
			indexer = 0;
			cmd.data[DataByteCount++]= Cmd;
			for(int i =0; SerialCmdReg[i].data; i++)
			{
				cmd.data[DataByteCount++] = (SerialCmdReg[i].dataReg >> 8) & 0xFF;
				cmd.data[DataByteCount++] = (SerialCmdReg[i].dataReg) & 0xFF;
				for(int j =0; j < SerialCmdReg[i].dataLen; j++)
					cmd.data[DataByteCount++] = ((UINT8*)SerialCmdReg[i].data)[j];
			}
		break;
		case cmdGetAllRegDefaultValue:
			GetDefaultParameterValues();		
			indexer = 0;
			cmd.data[DataByteCount++]= Cmd;
			for(int i =0; SerialCmdReg[i].data; i++)
			{
				cmd.data[DataByteCount++] = (SerialCmdReg[i].dataReg >> 8) & 0xFF;
				cmd.data[DataByteCount++] = (SerialCmdReg[i].dataReg) & 0xFF;
				for(int j =0; j < SerialCmdReg[i].dataLen; j++)
					cmd.data[DataByteCount++] = ((UINT8*)SerialCmdReg[i].data)[j];
			}
		break;
		case cmdSetAllRegToDefaultVaue:
			if(dataReg == COLDSTART)
				SetDefaultParameter();
			else
				SetDefaultWeldPreset(FALSE);//restore default to weld preset parameters
			cmd.data[DataByteCount++] = cmdNocmd;
			cmd.data[DataByteCount++] = 0x00;
			cmd.data[DataByteCount++] = 0x00;
		break;
		case cmdGetSystemInfo:
			cmd.data[DataByteCount++] = (UINT8)Cmd;
			//3 bytes optional data
			cmd.data[DataByteCount++] = 0x00;
			cmd.data[DataByteCount++] = 0x00;
			cmd.data[DataByteCount++] = 0x00;
			break;
	}
	for(indx = 0; indx < DataByteCount; indx++)
		CRCbyte ^= cmd.data[indx];
	cmd.byteCount += DataByteCount;
	
	cmd.byteCount +=  sizeof(cmd.checkSum);
	CRCbyte ^= cmd.byteCount;
	//CRC on bytecount, addr and data
	//byte count includes the CRC byte so decrease the data length to be passed to CRC routine
	cmd.checkSum = CRCbyte;

	cmd.cr = CR;
	cmd.lf = LF;

	TransmitAsciiCommand(cmd);
}

void ReceiveCommandHandler::HexToAscii(void * var, int varSize, UINT8 * buff)
/****************************************************************************/
/* void HexToAscii ()                                                       */
/* Description:-																				 */
/* routine converts  hex number to ascii chracter                           */
/****************************************************************************/
{
	int i;
	const char * hexStr  = "0123456789ABCDEF";
	int offset = 0;
	unsigned char * varPtr = (unsigned char *)var;
	int temp = 0;
	//TODO: Check endianness issue
	for(i = 0; i < varSize; i++)
	{
		buff[offset++] = hexStr[varPtr[temp] / 16];
		buff[offset++] = hexStr[varPtr[temp] % 16];
		temp++;
	}
}

UINT8 ReceiveCommandHandler::xtoi(char hexNumber)
/****************************************************************************/
/* void xtoi ()                                                             */
/* Description:-																				 */
/* routine converts ascii chracter to hex number.                           */
/****************************************************************************/
{
	//'0' to '9' 48 to 57
	// 'A' to 'F' 65 to 70
	// 'a' to 'f'  97 to 102
	UINT8 result = 0;
	if ((hexNumber >= '0') && (hexNumber <= '9'))
		result |= hexNumber & 0x0F;
	else if ((hexNumber >= 'A') && (hexNumber <= 'F'))
		result |= hexNumber - 'A' + 10;
	else if ((hexNumber >= 'a') && (hexNumber <= 'f'))
		result |= hexNumber - 'a' + 10;
	else
		result = 0;

	return result;
}

void ReceiveCommandHandler::AsciiToHex(void * var, int varsize, UINT8 * buff)
/****************************************************************************/
/* void AsciiToHex ()                                                       */
/* Description:-																				 */
/* This function converts ASCII to Hex.                                    */
/****************************************************************************/
{
	int i;
	UINT8 offset = 0;
	UINT8 * varPtr = (UINT8 *)var;
	for(i = 0; i < varsize;i++)
	{
		varPtr[i] = (xtoi(buff[offset]) << 4) |  (xtoi(buff[offset + 1]));
		offset += 2;
	}
}

void ReceiveCommandHandler::TransmitAsciiCommand(struct SerialCommand cmd)
/****************************************************************************/
/* void TransmitAsciiCommand ()                                             */
/* Description:-																				 */
/* This function transmits ASCII command to LCD.                            */
/* Input parameters:                                                        */
/* SerialCommand cmd : it is command to be send.                            */
/****************************************************************************/
{
	int charCnt = 0;
	int dataLen = 0, i;
	serialBuff[charCnt++] = cmd.startChar;
	serialBuff[charCnt++] = cmd.recordType;
	HexToAscii(&cmd.byteCount, sizeof(cmd.byteCount), &serialBuff[charCnt]);
	charCnt += (sizeof(cmd.byteCount) * 2);
	HexToAscii(&cmd.addr, sizeof(cmd.addr), &serialBuff[charCnt]);
	charCnt += (sizeof(cmd.addr) * 2);

	dataLen = cmd.byteCount - sizeof(cmd.addr) - sizeof(cmd.checkSum);
	for(i = 0; i < dataLen; i++)
	{
		HexToAscii(&cmd.data[i], sizeof(UINT8), &serialBuff[charCnt]);
		charCnt += 2;
	}

	HexToAscii(&cmd.checkSum, sizeof(cmd.checkSum), &serialBuff[charCnt]);
	charCnt += (sizeof(cmd.checkSum) * 2);
	serialBuff[charCnt++] = cmd.cr;
	serialBuff[charCnt++] = cmd.lf;

	for(i = 0; i < charCnt; i++)
		port.Send(this, serialBuff[i], -1);
}

void ReceiveCommandHandler::UpdateLCDData()
/****************************************************************************/
/* void UpdateLCDData ()                                                    */
/* Description:-																				 */
/* This function updates lcdData structure from Current Preset.             */
/****************************************************************************/
{
	ip_addr temp;
	lcdData.seekRampTime = CONVMICRO_MILLI(CurrentPreset.SeekRampTime);
	lcdData.weldRampTime = CONVMICRO_MILLI(CurrentPreset.WeldRampTime);
	lcdData.TimedSeek = CurrentPreset.SWDip.Flags.TimedSeek;
	lcdData.seekTime = CONVMICRO_MILLI(CurrentPreset.SeekTime);
	if(MFCycleFlag)
		lcdData.Amplitude = CurrentCycleParam.Amplitude;
	else
		lcdData.Amplitude = CurrentPreset.WeldAmplitude ;

	lcdData.ExAmplitude = CurrentPreset.SWDip.Flags.ExtAmplitude;
	lcdData.ExtFreq = CurrentPreset.SWDip.Flags.ExtFrequency;
	lcdData.AlarmResetReq = CurrentPreset.AlarmResetReq;
	switch(sysConfig.currNetworkConfig)
	{
		case configDefault:
		case configStatic:
			lcdData.IP1 = (sysConfig.systemIPStatic.addr >> 24)& 0xff;
			lcdData.IP2 = (sysConfig.systemIPStatic.addr >> 16)& 0xff;
			lcdData.IP3 = (sysConfig.systemIPStatic.addr >> 8) & 0xff;
			lcdData.IP4 = (sysConfig.systemIPStatic.addr & 0xff);

			lcdData.SubNetMask1 = (sysConfig.netmask.addr >> 24 ) & 0xff;
			lcdData.SubNetMask2 = (sysConfig.netmask.addr >> 16)& 0xff;
			lcdData.SubNetMask3 = (sysConfig.netmask.addr >> 8)& 0xff;
			lcdData.SubNetMask4 = (sysConfig.netmask.addr & 0xff);

			lcdData.GateWay1 = (sysConfig.gw.addr >> 24) & 0xff;
			lcdData.GateWay2 = (sysConfig.gw.addr >> 16) & 0xff;
			lcdData.GateWay3 = (sysConfig.gw.addr >> 8) & 0xff;
			lcdData.GateWay4 = (sysConfig.gw.addr & 0xff);
		break;
		case configDhcpServer:
			lcdData.IP1 = (sysConfig.systemIP.addr >> 24)& 0xff;
			lcdData.IP2 = (sysConfig.systemIP.addr >> 16)& 0xff;
			lcdData.IP3 = (sysConfig.systemIP.addr >> 8) & 0xff;
			lcdData.IP4 = (sysConfig.systemIP.addr & 0xff);

			lcdData.SubNetMask1 = (sysConfig.dhcpConfig.netmask.addr >> 24 ) & 0xff;
			lcdData.SubNetMask2 = (sysConfig.dhcpConfig.netmask.addr >> 16)& 0xff;
			lcdData.SubNetMask3 = (sysConfig.dhcpConfig.netmask.addr >> 8)& 0xff;
			lcdData.SubNetMask4 = (sysConfig.dhcpConfig.netmask.addr & 0xff);

			lcdData.GateWay1 = (sysConfig.dhcpConfig.gateway.addr >> 24) & 0xff;
			lcdData.GateWay2 = (sysConfig.dhcpConfig.gateway.addr >> 16) & 0xff;
			lcdData.GateWay3 = (sysConfig.dhcpConfig.gateway.addr >> 8) & 0xff;
			lcdData.GateWay4 = (sysConfig.dhcpConfig.gateway.addr & 0xff);
		break;
		case configDhcpClient:
			temp = CyGOS::GetIPAddress(interface);
			lcdData.IP1 = (temp.addr >> 24)& 0xff;
			lcdData.IP2 = (temp.addr >> 16)& 0xff;
			lcdData.IP3 = (temp.addr >> 8) & 0xff;
			lcdData.IP4 = (temp.addr & 0xff);

			temp = CyGOS::GetSubnetMask(interface);
			lcdData.SubNetMask1 = (temp.addr >> 24)& 0xff;
			lcdData.SubNetMask2 = (temp.addr >> 16)& 0xff;
			lcdData.SubNetMask3 = (temp.addr >> 8)& 0xff;
			lcdData.SubNetMask4 = (temp.addr & 0xff);

			temp = CyGOS::GetGatewayAddress(interface);
			lcdData.GateWay1 = (temp.addr >> 24) & 0xff;
			lcdData.GateWay2 = (temp.addr >> 16) & 0xff;
			lcdData.GateWay3 = (temp.addr >> 8) & 0xff;
			lcdData.GateWay4 = (temp.addr & 0xff);
		break;
	}
	pprintf("\n update lcd data ");
	lcdData.IPSetUp = (UINT16)sysConfig.currNetworkConfig;

	lcdData.PwrUpSeek = CurrentPreset.SWDip.Flags.PowerUpOptions;
	lcdData.MemStoreAtWeldEnd = CurrentPreset.SWDip.Flags.EndOfWeldStore;
}


void ReceiveCommandHandler::SetDefaultIPSetUp()
/****************************************************************************/
/* void SetDefaultIPSetUp ()                                                */
/* Description:-															*/
/* This function stores default IP configuration in FRAM.                   */
/****************************************************************************/
{
	SetDHCPServerIPDefaults();
	SetStaticIPDefaults();
	lcdData.IP1 = (sysConfig.systemIPStatic.addr >> 24)& 0xff;//sysConfig.dhcpServerIP.;
	lcdData.IP2 = (sysConfig.systemIPStatic.addr >> 16)& 0xff;
	lcdData.IP3 = (sysConfig.systemIPStatic.addr >> 8)& 0xff;
	lcdData.IP4 = (sysConfig.systemIPStatic.addr & 0xff);


	lcdData.SubNetMask1 = (sysConfig.netmask.addr >> 24 ) & 0xff;
	lcdData.SubNetMask2 = (sysConfig.netmask.addr >> 16)& 0xff;
	lcdData.SubNetMask3 = (sysConfig.netmask.addr >> 8)& 0xff;
	lcdData.SubNetMask4 = (sysConfig.netmask.addr & 0xff);

	lcdData.GateWay1 = (sysConfig.gw.addr >> 24) & 0xff;
	lcdData.GateWay2 = (sysConfig.gw.addr >> 16) & 0xff;
	lcdData.GateWay3 = (sysConfig.gw.addr >> 8) & 0xff;
	lcdData.GateWay4 = (sysConfig.gw.addr & 0xff);
	if(regUpdateCmdFlag == FALSE)
		regUpdateCmdFlag = TRUE;
}

void ReceiveCommandHandler::SetDefaultParameter()
/****************************************************************************/
/* void SetDefaultParameter ()                                              */
/* Description:-															*/
/* This function stores default value of LCD preset in FRAM.                */
/****************************************************************************/
{
	SetDefaultIPSetUp();
	ColdStart();
	SavePreset(0);
}

void ReceiveCommandHandler::GetDefaultParameterValues()
/****************************************************************************/
/* void GetDefaultParameterValues ()                                        */
/* Description:-																				 */
/* This function assigns default value to LCD.                              */
/****************************************************************************/
{
	lcdData.seekRampTime = SEEKRAMPTIME_DEFAULT;
	lcdData.weldRampTime = WELDRAMPTIME_DEFAULT;
	lcdData.TimedSeek = 0;
	lcdData.seekTime = SEEKTIME_DEFAULT;
	lcdData.Amplitude = 10;
	lcdData.ExAmplitude = 0;
	lcdData.IP1 = DEFAULTIPSTATIC1;//sysConfig.dhcpServerIP.;
	lcdData.IP2 = DEFAULTIPSTATIC2;
	lcdData.IP3 = DEFAULTIPSTATIC3;
	lcdData.IP4 = DEFAULTIPSTATIC4;
	lcdData.PwrUpSeek = 1;
	lcdData.MemStoreAtWeldEnd = 1;
	//lcdData.AutoSonics = 0;
	lcdData.SubNetMask1 = DEFSUBNETMASK1;
	lcdData.SubNetMask2 = DEFSUBNETMASK2;
	lcdData.SubNetMask3 = DEFSUBNETMASK3;
	lcdData.SubNetMask4 = DEFSUBNETMASK4;
	lcdData.GateWay1 = DEFGATEWAYSTATIC1;
	lcdData.GateWay2 = DEFGATEWAYSTATIC2;
	lcdData.GateWay3 = DEFGATEWAYSTATIC3;
	lcdData.GateWay4 = DEFGATEWAYSTATIC4;
	lcdData.ExtFreq = 0;
	lcdData.AlarmResetReq = FALSE;
	lcdData.IPSetUp = (UINT16)configStatic;
}

void ReceiveCommandHandler::ProcessRegUpdateCommand(enum DataReg dataReg, UINT16 value)
/****************************************************************************/
/* void ProcessRegUpdateCommand ()                                          */
/* Description:-																				 */
/* This function is responsible for updating Current preset according to    */
/* data reg number received in LCD command.                                 */
/****************************************************************************/
{
	bool PresetUpdate = FALSE;
	switch(dataReg)
	{
		case regSeekRampTime:
			PresetUpdate = TRUE;
			CurrentPreset.SeekRampTime = CONV_MILLI_MICROSEC(value);
		break;
		case regWeldRampTime:
			PresetUpdate = TRUE;
			CurrentPreset.WeldRampTime = CONV_MILLI_MICROSEC(value);
		break;
		case regTimedSeek:
			PresetUpdate = TRUE;
			CurrentPreset.SWDip.Flags.TimedSeek = value;
		break;
		case regSeekTime:
			PresetUpdate = TRUE;
			CurrentPreset.SeekTime = CONV_MILLI_MICROSEC(value);
		break;
		case regAmplitude:
		PresetUpdate = TRUE;
			CurrentPreset.WeldAmplitude = value;
			RunningPreset.WeldAmplitude = CurrentPreset.WeldAmplitude;
		break;
		case regSoftwareVersion:
			LCDSWVersion = value;
		break;
		case regIP1:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.systemIP.addr = ((sysConfig.systemIP.addr & 0x00FFFFFF)|( value << 24));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.systemIPStatic.addr = ((sysConfig.systemIPStatic.addr & 0x00FFFFFF)|( value << 24));
		break;
		case regIP2:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.systemIP.addr = ((sysConfig.systemIP.addr & 0xFF00FFFF)|( value << 16));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.systemIPStatic.addr = ((sysConfig.systemIPStatic.addr & 0xFF00FFFF)|( value << 16));
		break;
		case regIP3:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.systemIP.addr = ((sysConfig.systemIP.addr & 0xFFFF00FF)|( value << 8));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.systemIPStatic.addr = ((sysConfig.systemIPStatic.addr & 0xFFFF00FF)|( value << 8));
		break;
		case regIP4:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.systemIP.addr = ((sysConfig.systemIP.addr & 0xFFFFFF00)|( value ));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.systemIPStatic.addr = ((sysConfig.systemIPStatic.addr & 0xFFFFFF00)|( value ));
		break;
		case regIPSetUp:
			sysConfig.currNetworkConfig = (NetworkConfiguration)value;
			if(sysConfig.currNetworkConfig == configDefault)
				SetDefaultIPSetUp();
			regUpdateCmdFlag = TRUE;
			break;
		case regExAmplitude:
			PresetUpdate = TRUE;
			CurrentPreset.SWDip.Flags.ExtAmplitude = value;
		break;
		case regMemStore:
			PresetUpdate = TRUE;
			CurrentPreset.SWDip.Flags.EndOfWeldStore = value;
		break;
		case regPwrUpSeekScan:
			PresetUpdate = TRUE;
			CurrentPreset.SWDip.Flags.PowerUpOptions = value;
		break;
		case regCodeCRCLow:
			LCDCodeCRCLow = value & 0x00FF;
		break;
		case regCodeCRCHigh:
			LCDCodeCRCHigh = value & 0x00FF;
		break;
		case regSubNetMask1:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.netmask.addr = ((sysConfig.dhcpConfig.netmask.addr & 0x00FFFFFF)|( value << 24));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.netmask.addr = ((sysConfig.netmask.addr & 0x00FFFFFF)|( value << 24));
		break;
		case regSubNetMask2:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.netmask.addr = ((sysConfig.dhcpConfig.netmask.addr & 0xFF00FFFF)|( value << 16));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.netmask.addr = ((sysConfig.netmask.addr & 0xFF00FFFF)|( value << 16));
		break;
		case regSubNetMask3:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.netmask.addr = ((sysConfig.dhcpConfig.netmask.addr & 0xFFFF00FF)|( value << 8));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.netmask.addr = ((sysConfig.netmask.addr & 0xFFFF00FF)|( value << 8));
		break;
		case regSubNetMask4:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.netmask.addr = ((sysConfig.dhcpConfig.netmask.addr & 0xFFFFFF00)|( value ));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.netmask.addr = ((sysConfig.netmask.addr & 0xFFFFFF00)|( value ));
		break;
		case regGateWay1:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.gateway.addr = ((sysConfig.dhcpConfig.gateway.addr & 0x00FFFFFF)|( value << 24));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.gw.addr = ((sysConfig.gw.addr & 0x00FFFFFF)|( value << 24));
		break;
		case regGateWay2:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.gateway.addr = ((sysConfig.dhcpConfig.gateway.addr & 0xFF00FFFF)|( value << 16));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.gw.addr = ((sysConfig.gw.addr & 0xFF00FFFF)|( value << 16));
		break;
		case regGateWay3:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.gateway.addr = ((sysConfig.dhcpConfig.gateway.addr & 0xFFFF00FF)|( value << 8));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.gw.addr = ((sysConfig.gw.addr & 0xFFFF00FF)|( value << 8));
		break;
		case regGateWay4:
			if(sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.gateway.addr = ((sysConfig.dhcpConfig.gateway.addr & 0xFFFFFF00)|( value ));
			else if(sysConfig.currNetworkConfig == configStatic)
				sysConfig.gw.addr = ((sysConfig.gw.addr & 0xFFFFFF00)|( value ));
		break;
		case regAlarmResetReq:
			PresetUpdate = TRUE;
			CurrentPreset.AlarmResetReq = value;
		break;
		case regExtFreq:
			PresetUpdate = TRUE;
			CurrentPreset.SWDip.Flags.ExtFrequency = value;
		break;
		default:
			break;
	}
	if(PresetUpdate)
		SavePreset(0);

	pprintf(" reg = %d val = %d", dataReg , value);
	pprintf("\n");
}

void ReceiveCommandHandler::Tick()
{
	static int usCount = 0;
	if(cmdReceived == FALSE)
	{
		usCount += usecPerTick;
		if(usCount >= KEYPRESSEDTIMEOUT)
		{
			LCDKeyStatus = 0;
		}
		if(usCount >= LCDCOMTIMEOUT)
		{
			//log entry should be done on the reply of next polling
			lcdCommunicationLost = TRUE;
			//LCD should be given the fresh set of all register values
			regUpdateCmdFlag = TRUE;
			usCount = 0;
		}
	}
	else
	{
		usCount = 0;
		cmdReceived = FALSE;
	}
}



