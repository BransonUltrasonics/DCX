/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ReceiveCommandHandler.h_v   1.16   23 Jan 2015 15:09:34   ygupta  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ReceiveCommandHandler.h_v  $
 * 
 *    Rev 1.16   23 Jan 2015 15:09:34   ygupta
 * Change the logic so if Preset is chnaged but the parameter values are same then only send the values those are not same.
 * 
 *    Rev 1.15   25 Aug 2014 10:11:48   amaurya
 * Added register 142-154
 * 
 *    Rev 1.14   08 Oct 2013 06:17:32   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.13   24 Aug 2013 17:14:08   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.12   21 May 2013 12:25:58   amaurya
 * Code review fixes.
 * 
 *    Rev 1.11   26 Apr 2013 11:03:26   amaurya
 * Fixed traker issue for Ver2.0U.
 * 
 *    Rev 1.10   15 Mar 2013 10:51:48   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.9   11 Mar 2013 02:28:36   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.8   29 Jan 2013 02:22:00   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.7   27 Dec 2012 06:45:18   ygupta
 * Changes to fix Issues
 * 
 *    Rev 1.6   14 Dec 2012 05:27:54   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.5   13 Dec 2012 00:11:42   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.4   29 Nov 2012 08:46:50   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.3   24 Nov 2012 08:04:12   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.2   08 Nov 2012 01:20:12   rjamloki
 * Website changes
 * 
 *    Rev 1.1   26 Oct 2012 02:38:56   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.0   21 Sep 2012 01:43:42   rjamloki
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

#define LCDCOMTIMEOUT 5000000 //in microseconds

//If a key is pressed and signal is lost during TEST, then there should be some timeout to terminate the process.
//KEYPRESSEDTIMEOUT is used in that case.
#define KEYPRESSEDTIMEOUT 500000 //in microseconds

#define TXBUFSIZE   512
#define RXBUFSIZE   512
#define MAXDATASIZE 512

#define RECEIVECOMMHANDLER_STACKSIZE   4096
#define LCDBAUDRATE        57600
#define TOTAL_SERIALCOMM   47

////default IP Static case
#define DEFAULTIPSTATIC1  192
#define DEFAULTIPSTATIC2  168
#define DEFAULTIPSTATIC3  10
#define DEFAULTIPSTATIC4  100

#define DEFSUBNETMASK1  255
#define DEFSUBNETMASK2  255
#define DEFSUBNETMASK3  255
#define DEFSUBNETMASK4  0

////Static IP case
#define DEFGATEWAYSTATIC1  192
#define DEFGATEWAYSTATIC2  168
#define DEFGATEWAYSTATIC3  10
#define DEFGATEWAYSTATIC4  1

#define LCD_ETHIPSTATIC		0
#define LCD_ETHIPCLIENT		1
#define LCD_ETHBOOTP		2
#define LCD_ETHDEF			3

//
#define COLDSTART 2
#define RESTOREWELDPRESET 1

#define HEXBASE 16
#define DECIMALBASE 10
#define BITS_IN_A_BYTE 8
#define BITS_IN_TWO_BYTE 16
#define BITS_IN_THREE_BYTE 24
#define HIGHBYTE 0xFF00
#define LOWBYTE 0x00FF

extern UINT8 SerialRecBuff[RXBUFSIZE];
extern BOOLEAN RegUpdateCmdFlag;
extern UINT16 CurrentAlarm;

extern SINT16 TempPeakPower;
extern UINT16 LCDKeyStatus;

enum ReceiveState {
   STATE_WAIT_START,
   STATE_RECEIVE,
   STATE_WAIT_LF,
   STATE_PROCESSRESPONSE,
};

enum RecordType {
   UNUSED = '0',
   DATACMD = '1',
   POLLING = '4',
   ENDOFFILE = '9',
   RESERVED = '5',
};


//S4030004cc0d0a
struct SerialCommand
{
   UINT8 StartChar;
   UINT8 RecordType;
   UINT8 ByteCount;
   UINT16 Addr;
   UINT8 Data[MAXDATASIZE];
   UINT8 CheckSum;
   UINT8 Cr;
   UINT8 Lf;
};
struct DCXSTATUS{
	union
	{
		UINT8 StatusFlags;
		struct
		{
			UINT8  unused: 2;
			UINT8  TweakFlag: 1;
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
	CMD_NOCMD = 0X00,
	CMD_NOTUSED = 0X01,
	CMD_DIGIT_DISPALY = 0X02,
	CMD_ALPHA_DISPLAY = 0X03,
	CMD_GET_REGVALUE = 0X04,
	CMD_GET_ALLREG_CURRENTVALUE = 0X05,
	CMD_GET_ALLREG_DEFAULTVALUE = 0X06,
	CMD_SET_ALLREG_TODEFAULTVAUE = 0X07,
	CMD_GET_SYSTEMINFO = 0X08,
	CMD_GET_ALLREG_MAXVALUES = 0X09,
	CMD_GET_ALLREG_MINVALUES = 0X0A,
	//FACTORY TEST COMMANDS
	CMD_ONSEGMENTS = 80,
	CMD_OFFSEGMENTS = 81,
	CMD_FLASHCRC = 82,
	CMD_TURNON_BKLIGHT = 83,
	CMD_TURNOFF_BKLIGHT = 84,
	CMD_TURNON_GREENLED = 85,
	CMD_TURNON_YELLOWLED = 86,
	CMD_TURNON_REDLED = 87,
	CMD_TRUNOFF_LED = 88,
	CMD_SOFTWARE_VERSION = 89,
	CMD_POWERUP_TEST = 90,
	CMD_EEPROM_WRITETEST = 91,
	CMD_EEPROM_READTEST = 92,
};

enum DataReg
{
    REG_NONE = 0X0000,
    REG_SOFTWARE_VERSION =  101,
    REG_BARGRAPH_USAGE = 102,
    REG_EXAMPLITUDE = 104,
    REG_WELD_RAMPTIME =  105,
    REG_MEMSTORE = 106,
    REG_PWRUP_SEEKSCAN = 107,
    REG_SEEK_RAMPTIME =  108,
    REG_TIMEDSEEK =  109,
    REG_SEEKTIME =  110,
    REG_EXTFREQ = 111,
    REG_FREQOFFSET = 112,
    REG_CUTOFFS = 113,
    REG_LIMITS = 114,
    REG_SETDEFAULT = 115,
    REG_IP1 = 116,
    REG_IP2 = 117,
    REG_IP3 = 118,
    REG_IP4 = 119,
    REG_GATEWAY1    = 120,
    REG_GATEWAY2    = 121,
    REG_GATEWAY3    = 122,
    REG_GATEWAY4    = 123,
    REG_SUBNETMASK1 = 124,
    REG_SUBNETMASK2 = 125,
    REG_SUBNETMASK3 = 126,
    REG_SUBNETMASK4 = 127,
    REG_IPSETUP = 128,
    REG_PROFIBUS_ADDR = 129,
    REG_CONTRAST = 133,
    REG_BKLIGHT_TIMEOUT = 134,
    REG_SCROLLSIZE = 135,
    REG_POWERON_DISP = 136,
    REG_RESERVED = 137,     //REG_SEEK_BEFORE_TEST
    REG_WELDMODE = 138,
    REG_ETHIP1 = 142,
    REG_ETHIP2 = 143,
    REG_ETHIP3 = 144,
    REG_ETHIP4 = 145,
    REG_ETH_GATEWAY_1 = 146,
    REG_ETH_GATEWAY_2 = 147,
    REG_ETH_GATEWAY_3 = 148,
    REG_ETH_GATEWAY_4 = 149,
    REG_SUBNET_ETH_1 = 150,
    REG_SUBNET_ETH_2 = 151,
    REG_SUBNET_ETH_3 = 152,
    REG_SUBNET_ETH_4 = 153,
    REG_DEFNET_INFO = 154,
    REG_900PASSWORD = 200,
    REG_BLINK_ONTIME =  900,
    REG_BLINK_OFFTIME =  901,
    REG_POLLING_RATE =  902,
    REG_RESPONSE_TIMEOUT =  903,
    REG_POWERUP_DIAGNOSTIC = 904,
    REG_CODECRC_LOW = 905,
    REG_CODECRC_HIGH = 906,

    REG_AMPLITUDE = 907,
    REG_TIMEMODE_VAL = 908,
    REG_ENERGYMODE_VAL = 909,
    REG_PKPWRMODE_VAL = 910,
    REG_GNDDETECTMODE_VAL = 911
};

typedef struct SerialCmdData
{
   enum DataReg DataReg;
   void * Data;
   BOOLEAN MinMaxValue;
   UINT8 DataLen;
   void * DataLast;
}SerCmdData;

struct LCDData
{
   UINT8  AmpStepCondition;
   UINT16 WeldRampTime;    //in ms
   UINT8  MemoryStoreFlag;
   UINT8  PowerupOption;
   UINT16 SeekRampTime;    //in ms
   UINT8  TimedSeekFlag;
   UINT16 SeekTime;        //in ms
   UINT8  FreqOffsetFlag;
   SINT16 FreqOffsetValue;
   UINT8  CutOffs;
   UINT8  Limits;
   UINT8  IP1;
   UINT8  IP2;
   UINT8  IP3;
   UINT8  IP4;
   UINT8  GateWay1;
   UINT8  GateWay2;
   UINT8  GateWay3;
   UINT8  GateWay4;
   UINT8  SubNetMask1;
   UINT8  SubNetMask2;
   UINT8  SubNetMask3;
   UINT8  SubNetMask4;
   UINT8  IPSetUp;
   UINT8  ProfiBusAddr;
   UINT8  Reserved;
   UINT8  WeldMode;
	UINT8  EthernetIP1;
	UINT8  EthernetIP2;
	UINT8  EthernetIP3;
	UINT8  EthernetIP4;
	UINT8  GatewayEthIP1;
	UINT8  GatewayEthIP2;
	UINT8  GatewayEthIP3;
	UINT8  GatewayEthIP4;
	UINT8  DefNetInfo;
	UINT8  SubNetEthIP1;
	UINT8  SubNetEthIP2;
	UINT8  SubNetEthIP3;
	UINT8  SubNetEthIP4;
   UINT8  Amplitude;//in percentage
   UINT16 TimeModeValue;//in ms
   UINT32 EnergyModeValue;//in percentage
   UINT8  PKPwrModeValue;//in percentage
   UINT16 GndDetectModeValue;//in ms
};

class ReceiveCommandHandler: public Task
{
public:
	ReceiveCommandHandler();
	static ReceiveCommandHandler *thisptr;
	static LCDData LcdData;
	static BOOLEAN SeekBeforeTest;
	static DCXSTATUS DCXStatus;
	static UINT16 LCDSWVersion;
	static UINT8 LCDCodeCRCLow;
	static UINT8 LCDCodeCRCHigh;
	BOOLEAN SystemInfoReceived;
	static LCDData LcdDataLast;
	void TransmitPollingCommandReply(UINT8 Cmd, UINT16 DataReg);
protected:
	void LCDSerialMessageQueue(unsigned char * Data, unsigned int Length);
	void TransmitAsciiCommand(struct SerialCommand * Cmd);
	void AsciiToHex(void * Var, int Varsize, UINT8 * Buff);
	UINT8 xtoi(char HexNumber);
	void HexToAscii(void * Var, int VarSize, UINT8 * Buff);
	void ProcessCommand(UINT8 * Response);
	UINT8 VerifyCheckSum(UINT8 * Data, UINT8 DataLen);
	void UpdateLCDData(void);
	void ProcessRegUpdateCommand(enum DataReg DataReg, UINT32 Value);
	UINT8 PrepareDCXStatusByte(void);
	void GetDefaultParameterValues(void);
	void SetDefaultIPSetUp(void);
	void SetAllColdStartParameter(void);
	void GetMaxParameterValues(void);
	void GetMinParameterValues(void);
	BOOLEAN CheckIfChanged(int Indx);
	virtual void Run(void);
	void Tick(void);
	BOOLEAN CmdReceived;
	Serial Port;
	int Interface;
	Ethernet * NetIf;
	NetworkConfiguration CurrNWConfig;
	BOOLEAN SentSystemInfoCmd;
};
void UpdateMinMaxLCDPreset(PSUPDATESPECIAL PsUpdateSpcl = UPDATEMINMAX);
extern BOOLEAN RegMaxUpdateFlag;
extern BOOLEAN RegMinUpdateFlag;
#endif  /* RECEIVECOMMANDHANDLER_H_ */
