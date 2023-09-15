/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/ReceiveCommandHandler.h_v   1.11   25 Jul 2016 12:42:10   GButron  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/ReceiveCommandHandler.h_v  $
 * 
 *    Rev 1.11   25 Jul 2016 12:42:10   GButron
 * Rename functions prototypes to WritelnStrFT() and WritelnCharFT()
 * 
 *    Rev 1.10   06 Jun 2016 15:19:50   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.9   28 Sep 2015 06:46:56   RJamloki
 * Fix for IP, Subnet and gateway registers randomly getting set to wrong value
 * 
 *    Rev 1.8   25 Jun 2015 07:06:54   amaurya
 * Added register to switch between Time-Impulse and Maintained mode.
 * 
 *    Rev 1.7   09 Apr 2015 05:17:48   sesharma
 * All references to Impulse Start Register removed.
 * 
 *    Rev 1.6   19 Mar 2015 07:00:42   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.5   26 Feb 2015 08:29:30   sesharma
 * Unused bits removed.
 * 
 *    Rev 1.4   18 Feb 2015 00:57:54   SaPawar
 * alarm bit addition to dcpstatus for peakpowerlowlimit handling at lcd side
 * 
 *    Rev 1.3   13 Jan 2015 06:01:50   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.2   17 Sep 2014 08:59:02   amaurya
 * Moved defines to globals.h
 * 
 *    Rev 1.1   03 Sep 2014 07:41:26   amaurya
 * Removed Register 103.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:28   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:02   amaurya
 * Initial revision.
 */

#ifndef RECEIVECOMMANDHANDLER_H_
#define RECEIVECOMMANDHANDLER_H_

#include "Task.h"
#include "Serial.h"
#include "typ_def.h"
#include "Serial.h"
#include "CyGOS.h"

#define STARTCHAR                                     'S'
#define CR                                           0x0D
#define LF                                           0x0A
#define KB_RESET                                     0x10
#define KB_TEST                                      0x20
#define KB_FN                                        0x40
#define KB_UP                                        0x04
#define KB_DOWN                                      0x08
#define LCDCOMTIMEOUT                            10000000 //in microseconds
#define KEYPRESSEDTIMEOUT                          500000 //in microseconds
#define TXBUFSIZE                                     512
#define RXBUFSIZE                                     512
#define MAXDATASIZE                                   512
#define RECEIVECOMMHANDLER_STACKSIZE                 4096
#define LCDBAUDRATE                                 57600
#define TOTAL_SERIALCOMM                               41
//default IP DHCP server
#define DEFAULTIP1                                    100
#define DEFAULTIP2                                    100
#define DEFAULTIP3                                      1
#define DEFAULTIP4                                    101
//default IP Static case
#define DEFAULTIPSTATIC1                              192
#define DEFAULTIPSTATIC2                              168
#define DEFAULTIPSTATIC3                               10
#define DEFAULTIPSTATIC4                              100
#define DEFSUBNETMASK1                                255
#define DEFSUBNETMASK2                                255
#define DEFSUBNETMASK3                                255
#define DEFSUBNETMASK4                                  0
//DHCP server case
#define DEFGATEWAY1                                     0
#define DEFGATEWAY2                                     0
#define DEFGATEWAY3                                     0
#define DEFGATEWAY4                                     0
//Static IP case
#define DEFGATEWAYSTATIC1                             192
#define DEFGATEWAYSTATIC2                             168
#define DEFGATEWAYSTATIC3                              10
#define DEFGATEWAYSTATIC4                               1
//
#define COLDSTART                                       2
#define RESTOREWELDPRESET                               1
#define FT_PROMPT_SIZE                                  4
#define FT_KEY_ENTER                                   13
#define FT_KEY_BACKSPACE                                8
#define FIRSTLETTEROFTHEALPHABETLOWERCASE              97
#define LASTLETTEROFTHEALPHABETLOWERCASE              122
#define UPERCASECONVERSION                             32

enum ReceiveState {
	STATE_WAIT_START, STATE_RECEIVE, STATE_WAIT_LF, STATE_PROCESSRESPONSE,
};

enum RecordType {
	unused = '0', dataCmd = '1', polling = '4', endofFile = '9', reserved = '5',
};

//S4030004cc0d0a
struct SerialCommand {
	UINT8 startChar;
	UINT8 recordType;
	UINT8 byteCount;
	UINT16 addr;
	UINT8 data[MAXDATASIZE];
	UINT8 checkSum;
	UINT8 cr;
	UINT8 lf;
};
struct DCPSTATUS {
	union {
		UINT8 StatusFlags;
		struct {
			UINT8 unused :3;
			UINT8 DispLock :1;
			UINT8 SonicsActive :1;
			UINT8 MemReset :1;
			UINT8 Ready :1;
			UINT8 Alarm :1; //0 Off, 1-On
		} Flags;
	} Status;
};

enum PollingResponseCmd {
	cmdNocmd = 0x00,
	cmdNotUsed = 0x01,
	cmdDigitDispaly = 0x02,
	cmdAlphaDisplay = 0x03,
	cmdGetRegValue = 0x04,
	cmdGetAllRegCurrentValue = 0x05,
	cmdGetAllRegDefaultValue = 0x06,
	cmdSetAllRegToDefaultVaue = 0x07,
	cmdGetSystemInfo = 0x08,
	cmdGetAllRegMaxValues = 0x09,
	cmdGetAllRegMinValues = 0x0A,
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

enum DataReg {
	regUnused = 0x0000,
	regSoftwareVersion = 101,
	regBarGraphUsage = 102,
	regExAmplitude = 104,
	regWeldRampTime = 105,
	regMemStore = 106,
	regPwrUpSeekScan = 107,
	regSeekRampTime = 108,
	regTimedSeek = 109,
	regSeekTime = 110,
	regExtFreq = 111,
	regFreqOffset = 112,
	regcutoffs = 113,
	reglimits = 114,
	regSetDefault = 115,
	regIP1 = 116,
	regIP2 = 117,
	regIP3 = 118,
	regIP4 = 119,
	regGateWay1 = 120,
	regGateWay2 = 121,
	regGateWay3 = 122,
	regGateWay4 = 123,
	regSubNetMask1 = 124,
	regSubNetMask2 = 125,
	regSubNetMask3 = 126,
	regSubNetMask4 = 127,
	regIPSetUp = 128,
	regFbaddress1 = 129,
	regFbaddress2 = 130,
	regFbaddress3 = 131,
	regFbaddress4 = 132,
	regTimeMaintained = 133,
	regBKLightTimeout = 134,
	regScrollSize = 135,
	regPoweronDisp = 136,
	regSIG = 137,
	regWeldMode = 138,
	regMACaddress1 = 139,
	regMACaddress2 = 140,
	regMACaddress3 = 141,
	regPowerWindowLimitLow = 155,
	regPowerWindowLimitHigh = 156,
	regMemoryClear = 157,
	reg900Password = 200,
	//not sent to DCP
	regBlinkONTime = 900,
	regBlinkOFFTime = 901,
	regPollingRate = 902,
	regResponseTimeOut = 903,
	regPowerUpDiagnostic = 904,
	regCodeCRCLow = 905,
	regCodeCRCHigh = 906,
	regAmplitude = 907,
	regTimeModeVal = 908,
	regEneregyModeVal = 909,
	regPKPowerModeVal = 910,
	regGndDtctModeVal = 911,
	regReadOnly = 912,
	regSystemType = 913
};

typedef struct SerialCmdData {
	enum DataReg dataReg;
	void * data;
	UINT8 dataLen;
	UINT8 RegEnable;
	BOOLEAN MinMaxValue;
} SerCmdData;

struct LCDData {
	UINT8 ExAmplitude;
	UINT16 weldRampTime;
	UINT8 MemStoreAtWeldEnd;
	UINT8 PwrUpSeek;
	UINT16 seekRampTime;
	UINT8 TimedSeek;
	UINT16 seekTime;
	UINT8 ExtFreq;
	SINT16 FreqOffsetValue;
	UINT8 CutOffs;
	UINT8 Limits;
	UINT8 IP1;
	UINT8 IP2;
	UINT8 IP3;
	UINT8 IP4;
	UINT8 GateWay1;
	UINT8 GateWay2;
	UINT8 GateWay3;
	UINT8 GateWay4;
	UINT8 SubNetMask1;
	UINT8 SubNetMask2;
	UINT8 SubNetMask3;
	UINT8 SubNetMask4;
	UINT8 IPSetUp;
	UINT8 FieldBusAddr1; //ask
	UINT8 FieldBusAddr2; //ask
	UINT8 FieldBusAddr3; //ask
	UINT8 FieldBusAddr4; //ask
	UINT8 TimeMaintained;
	UINT8 WeldMode;										//Rem
	UINT8 Amplitude;										//tbc in percentage
	UINT16 TimeModeValue;										//in ms
	UINT32 EnergyModeValue;										//in percentage
	UINT8 PKPwrModeValue;										//in percentage
	UINT16 GndDetectModeValue;										//in ms
	UINT8 SystemType;
	UINT8 SIG;
	UINT8 PowerWindowLimitLow;
	UINT8 PowerWindowLimitHigh;
	UINT8 MemoryClear;
};
extern UINT8 SerialRecBuff[RXBUFSIZE];
extern BOOLEAN RegUpdateCmdFlag;

class ReceiveCommandHandler: public Task {
public:
	ReceiveCommandHandler(int intf);
	virtual ~ReceiveCommandHandler();
	static ReceiveCommandHandler *thisptr;
	static LCDData lcdData;
	static DCPSTATUS DCPStatus;
	static UINT16 LCDSWVersion;
	static UINT8 LCDCodeCRCLow;
	static UINT8 LCDCodeCRCHigh;
	static BOOLEAN SystemInfoReceivedFlag;
protected:
	void LCDSerialMessageQueue(unsigned char * data, unsigned int length);
	void TransmitAsciiCommand(struct SerialCommand cmd);
	void AsciiToHex(void * var, SINT32 varsize, UINT8 * buff);
	UINT8 xtoi(UINT8 hexNumber);
	void HexToAscii(void * var, SINT32 varSize, UINT8 * buff);
	void TransmitPollingCommandReply(UINT8 Cmd, UINT16 dataReg);
	void ProcessCommand(UINT8 * response);
	UINT8 VerifyCheckSum(UINT8 * data, UINT8 dataLen);
	void UpdateLCDData();
	void ProcessRegUpdateCommand(enum DataReg dataReg, UINT32 value);
	UINT8 PrepareDCPStatusByte();
	void GetDefaultParameterValues();
	void SetDefaultIPSetUp();
	void SetDefaultParameter();
	void GetMaxParameterValues(void);
	void GetMinParameterValues(void);
protected:
	virtual void Run();
	void Tick();
	BOOLEAN CmdReceivedFlag;
	BOOLEAN lcdCommunicationLost;
	Serial port;
	int interface;
private:
	UINT16 RowFT;
	UINT16 ColFT;
	STR PromptFTStr[FT_PROMPT_SIZE];
	void ClrscrFT(void);
	void GotoXYFT(SINT32 Row,SINT32 Column);
	void WritelnStrFT(STR *ValuePtr);
	void WritelnCharFT(UINT8 Character);
};
#endif  /* RECEIVECOMMANDHANDLER_H_ */
