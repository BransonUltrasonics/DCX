/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ReceiveCommandHandler.h_v   1.9.1.4.2.0   17 Mar 2016 16:23:44   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ReceiveCommandHandler.h_v  $
 * 
 *    Rev 1.9.1.4.2.0   17 Mar 2016 16:23:44   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.9.1.4.1.0   17 Mar 2016 16:07:22   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.9.1.4   16 Sep 2011 08:08:56   RatneshA
 * changes regarding reg 115 option 1 and 2
 * 
 *    Rev 1.9.1.3   16 Aug 2011 08:26:44   pdwivedi
 * code cleanup
 * 
 *    Rev 1.9.1.2   05 Aug 2011 10:33:52   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.9.1.1   15 Jul 2011 09:47:40   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.9.1.0   26 Apr 2011 13:13:10   PDwivedi
 * System box in Sytem Information page now displays LCD code version and CRC
 * 
 *    Rev 1.9   25 Apr 2011 06:33:24   PDwivedi
 * Restore default should not modify the frquency,power and serial number
 * 
 *    Rev 1.8   21 Apr 2011 11:08:16   PDwivedi
 * modified Ver1.0T for tracker issues
 * 
 *    Rev 1.7   19 Apr 2011 10:48:38   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.6   14 Apr 2011 06:23:00   PDwivedi
 * Added changes for IP Setup issue and added comments.
 * 
 *    Rev 1.5   02 Mar 2011 08:22:38   PDwivedi
 * Added support for Level2.
 * 
 *    Rev 1.4   14 Jan 2011 07:55:44   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.3   07 Dec 2010 08:58:20   PDwivedi
 * Added changes for REV Board C , code clean up.
 * 
 *    Rev 1.2   06 Dec 2010 09:56:56   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.1   29 Nov 2010 07:47:38   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:46   ygupta
 * Initial revision.

******************************************************************************/

#ifndef RECEIVECOMMANDHANDLER_H_
#define RECEIVECOMMANDHANDLER_H_

#include "Task.h"
#include "Serial.h"
#include "typ_def.h"
#include "Serial.h"
#include "CyGOS.h"

#define STARTCHAR 'S'
#define CR  0x0D
#define LF  0x0A

#define KB_RESET     0x10
#define KB_TEST      0x20
#define KB_FN        0x40
#define KB_UP        0x04
#define KB_DOWN      0x08

#define LCDCOMTIMEOUT 10000000 //in microseconds
#define KEYPRESSEDTIMEOUT 500000 //in microseconds

#define TXBUFSIZE   512
#define RXBUFSIZE   512
#define MAXDATASIZE 512

#define RECEIVECOMMHANDLER_STACKSIZE   4096
#define LCDBAUDRATE        57600
#define TOTAL_SERIALCOMM   24
//default IP DHCP server
#define DEFAULTIP1  100
#define DEFAULTIP2  100
#define DEFAULTIP3  1
#define DEFAULTIP4  101

//default IP Static case
#define DEFAULTIPSTATIC1  192
#define DEFAULTIPSTATIC2  168
#define DEFAULTIPSTATIC3  10
#define DEFAULTIPSTATIC4  100


#define DEFSUBNETMASK1  255
#define DEFSUBNETMASK2  255
#define DEFSUBNETMASK3  255
#define DEFSUBNETMASK4  0

//DHCP server case
#define DEFGATEWAY1  0
#define DEFGATEWAY2  0
#define DEFGATEWAY3  0
#define DEFGATEWAY4  0

//Static IP case
#define DEFGATEWAYSTATIC1  192
#define DEFGATEWAYSTATIC2  168
#define DEFGATEWAYSTATIC3  10
#define DEFGATEWAYSTATIC4  1

//
#define COLDSTART 2
#define RESTOREWELDPRESET 1

extern UINT8 serialRecBuff[RXBUFSIZE];
extern BOOLEAN regUpdateCmdFlag;

enum ReceiveState {
 STATE_WAIT_START,
 STATE_RECEIVE,
 STATE_WAIT_LF,
 STATE_PROCESSRESPONSE,
 };

enum RecordType {
           unused = '0',
           dataCmd = '1',
           polling = '4',
           endofFile = '9',
           reserved = '5',
};


//S4030004cc0d0a
struct SerialCommand
{
  UINT8 startChar;
  UINT8 recordType;
  UINT8 byteCount;
  UINT16 addr;
  UINT8 data[MAXDATASIZE];
  UINT8 checkSum;
  UINT8 cr;
  UINT8 lf;
};
struct DCPSTATUS{
	union
	{
		UINT8 StatusFlags;
		struct
		{
			UINT8  unused: 3;
			UINT8  DispLock:1;
			UINT8  SonicsActive:1;
			UINT8  MemReset: 1;
			UINT8  Ready: 1;
			UINT8  Alarm: 1;//0 Off, 1-On
		}Flags;
	}Status;
};

enum PollingResponseCmd
{
	cmdNocmd = 0x00,
	cmdNotUsed = 0x01,
	cmdDigitDispaly = 0x02,
	cmdAlphaDisplay = 0x03,
	cmdGetRegValue = 0x04,
	cmdGetAllRegCurrentValue = 0x05,
	cmdGetAllRegDefaultValue = 0x06,
	cmdSetAllRegToDefaultVaue = 0x07,
	cmdGetSystemInfo = 0x08,
	//Factory Test Commands
	cmdOnSegments = 80,
   cmdOffSegments = 81,
   cmdFlashCRC = 82,
   cmdTurnONBkLight = 83,
   cmdTurnOFFBkLight = 84,
   cmdTurnONGreenLED = 85,
   cmdTurnONYellowLED = 86,    
   cmdTurnONRedLED = 87,
   cmdTrunOFFLED = 88,   
   cmdSoftwareVersion = 89,
   cmdPowerupTest = 90, 
};

enum DataReg
{
    regUnused = 0x0000,
    regSoftwareVersion =  101,
    regBarGraphUsage = 102,
    regAlarmResetReq = 103,
    regExAmplitude = 104,
    regWeldRampTime =  105,
    regMemStore = 106,
    regPwrUpSeekScan = 107,
    regSeekRampTime =  108,
    regTimedSeek =  109,
    regSeekTime =  110,
    regExtFreq = 111,
    regReserved1 = 113,
    regReserved2 = 114,
    regSetDefault = 115,
    regIP1 = 116,
    regIP2 = 117,
    regIP3 = 118,
    regIP4 = 119,
    regGateWay1    = 120,
    regGateWay2    = 121,
    regGateWay3    = 122,
    regGateWay4    = 123,
    regSubNetMask1 = 124,
    regSubNetMask2 = 125,
    regSubNetMask3 = 126,
    regSubNetMask4 = 127,
    regIPSetUp = 128,
    regAmplitude =  129,
    reg900Password = 200,
    //not sent to DCP
    regBlinkONTime =  900,
    regBlinkOFFTime =  901,
    regPollingRate =  902,
    regResponseTimeOut =  903,
    regPowerUpDiagnostic = 904,
    regCodeCRCLow = 905,
    regCodeCRCHigh = 906,
    regBKLightTimeout = 907,

};

typedef struct SerialCmdData
{
	enum DataReg dataReg;
	void  * data;
	UINT8 dataLen;
} SerCmdData;

struct LCDData
{
    UINT16 seekRampTime;
    UINT16 weldRampTime;
    UINT16 seekTime;
    UINT16 Amplitude;//in percentage
    UINT16 TimedSeek;
    UINT16 ExAmplitude;
    UINT16 IP1; 
    UINT16 IP2; 
    UINT16 IP3; 
    UINT16 IP4;
    UINT16 PwrUpSeek;
    UINT16 MemStoreAtWeldEnd;
    UINT16 SubNetMask1;
    UINT16 SubNetMask2;
    UINT16 SubNetMask3;
    UINT16 SubNetMask4;
    UINT16 GateWay1;
    UINT16 GateWay2;
    UINT16 GateWay3;
    UINT16 GateWay4;
    UINT16 ExtFreq;
    UINT16 AlarmResetReq;
    UINT16 IPSetUp;
};

 
class ReceiveCommandHandler: public Task
{
public:
	ReceiveCommandHandler(int intf);
	static ReceiveCommandHandler *thisptr;
	static LCDData lcdData;
	static DCPSTATUS DCPStatus;
	static UINT16 LCDSWVersion;
	static UINT8 LCDCodeCRCLow;
	static UINT8 LCDCodeCRCHigh;
protected:
	void LCDSerialMessageQueue(unsigned char * data, unsigned int length);
	void TransmitAsciiCommand(struct SerialCommand cmd);
	void AsciiToHex(void * var, int varsize, UINT8 * buff);
	UINT8 xtoi(char hexNumber);
	void HexToAscii(void * var, int varSize, UINT8 * buff);
	void TransmitPollingCommandReply(UINT8 Cmd, UINT16 dataReg);
	void ProcessCommand(UINT8 * response);
	UINT8 VerifyCheckSum(UINT8 * data, UINT8 dataLen);
	void UpdateLCDData();
	void ProcessRegUpdateCommand(enum DataReg dataReg, UINT16 value);
	UINT8 PrepareDCPStatusByte();
	void GetDefaultParameterValues();
	void SetDefaultIPSetUp();
	void SetDefaultParameter();
protected:
	virtual void Run();
	void Tick();
	BOOLEAN cmdReceived;
	BOOLEAN lcdCommunicationLost;
	Serial port;
	int interface;
	bool systemInfoReceived;
};

#endif  /* RECEIVECOMMANDHANDLER_H_ */
