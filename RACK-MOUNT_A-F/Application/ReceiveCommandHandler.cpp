/*
 $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/ReceiveCommandHandler.cpp_v   1.8   03 Feb 2016 00:58:58   ygupta  $
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
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/ReceiveCommandHandler.cpp_v  $
 * 
 *    Rev 1.8   03 Feb 2016 00:58:58   ygupta
 * UpdateLCDData() modified to send recalled preset no. to LCD.
 * 
 *    Rev 1.7   28 Jan 2016 01:59:06   ygupta
 * Added below given registers:
 * 1. Limit Registers(158-163) to meet requirement REQ01276(Limits can't be set through the LCD) 
 * 2. Added present number register(950) and preset screen to implement requirement REQ01277(Can't save parameter changes from the LCD into presets)
 * 
 *    Rev 1.6   24 Dec 2015 02:39:58   sesharma
 * Fix for DEF01243: UpdateLCDData() and GetDefaultParameterValues() modified to send fieldbus type information i.e. Ethernet IP or Profibus to LCD
 * 
 *    Rev 1.5   31 Jul 2015 09:53:12   RJamloki
 * FirstTime flag is set outside loop to update all register values correctly on LCD at power up.
 *
 *    Rev 1.4   24 Jul 2015 08:37:46   sesharma
 * Added function ConfigureResetLineForLCD and SendResetToLCD for all three boards.
 *
 *    Rev 1.3   08 Jul 2015 02:42:44   sesharma
 * Added register 193 for System type.
 *
 *    Rev 1.2   02 Jul 2015 10:00:18   RJamloki
 * Added comments on GPIO configuration and operation in constructor and Run function.
 *
 *    Rev 1.1   25 Jun 2015 05:47:20   rjamloki
 * Changes to send reset pulse to LCD once at power up from Run function
 *
 *    Rev 1.0   09 Jun 2015 09:10:50   tayars
 * Initial revision.
 *
 *    Rev 1.27   23 Jan 2015 15:09:30   ygupta
 * Change the logic so if Preset is chnaged but the parameter values are same then only send the values those are not same.
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
#include "ModBusSendTask.h"
#include "LED.h"
#include "VerifyPreset.h"
#include "WebSite.h"
#include "FieldBus.h"
#include "ProfiBus.h"

//#define pprintf
//#define printf

BOOLEAN OldLinkStat;
#define TIMEOUT_1SECOND  100000

 UINT8 CrashSerialRecBuff[RXBUFSIZE];
/*--------------------Global Variables-------------------------------------*/
ReceiveCommandHandler *ReceiveCommandHandler::thisptr;
UINT16 CurrentAlarm = 0;
BOOLEAN RegUpdateCmdFlag = TRUE;
BOOLEAN RegAmplitudeUpdateFlag = FALSE;
BOOLEAN RegMaxUpdateFlag = FALSE;
BOOLEAN RegMinUpdateFlag = FALSE;
UINT8 SerialRecBuff[RXBUFSIZE];
UINT8 SerialTxBuff[TXBUFSIZE];
UINT16 LCDKeyStatus = 0;

LCDData ReceiveCommandHandler::LcdData;
LCDData ReceiveCommandHandler::LcdDataLast;
DCXSTATUS ReceiveCommandHandler::DCXStatus;
UINT16 ReceiveCommandHandler::LCDSWVersion;
UINT8 ReceiveCommandHandler::LCDCodeCRCLow;
UINT8 ReceiveCommandHandler::LCDCodeCRCHigh;
BOOLEAN ReceiveCommandHandler::SeekBeforeTest = FALSE;

SINT16 TempPeakPower;
BOOLEAN LcdCommunicationLost;
extern BOOLEAN TokenAvailable; //FALSE means acquired by FieldBus/ProfiBus
extern TCPIPInterfaceConfig EthIPConfig;
BOOLEAN DebugLCD = FALSE;
unsigned char * MacAddress;
/*--------------------------------------------- ---------------------------*/
//Custom LCD Registers table.
const SerCmdData SerialCmdReg[] = {
		{ REG_EXAMPLITUDE,
			&(ReceiveCommandHandler::LcdData.AmpStepCondition), FALSE,
			sizeof(ReceiveCommandHandler::LcdData.AmpStepCondition), ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.AmpStepCondition) },
		{ REG_WELD_RAMPTIME,
         &(ReceiveCommandHandler::LcdData.WeldRampTime),
         TRUE, sizeof(ReceiveCommandHandler::LcdData.WeldRampTime), ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.WeldRampTime)},
		{ REG_MEMSTORE,
		   &(ReceiveCommandHandler::LcdData.MemoryStoreFlag), FALSE,
		   sizeof(ReceiveCommandHandler::LcdData.MemoryStoreFlag), ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.MemoryStoreFlag) },
		{ REG_PWRUP_SEEKSCAN,
		   &(ReceiveCommandHandler::LcdData.PowerupOption),
		   FALSE, sizeof(ReceiveCommandHandler::LcdData.PowerupOption),ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.PowerupOption) },
		{ REG_SEEK_RAMPTIME,
		   &(ReceiveCommandHandler::LcdData.SeekRampTime),
		   TRUE, sizeof(ReceiveCommandHandler::LcdData.SeekRampTime) ,ALLENABLE,  &(ReceiveCommandHandler::LcdDataLast.SeekRampTime)},
		{ REG_TIMEDSEEK,
		   &(ReceiveCommandHandler::LcdData.TimedSeekFlag), FALSE,
		   sizeof(ReceiveCommandHandler::LcdData.TimedSeekFlag),ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.TimedSeekFlag) },
		{ REG_SEEKTIME,
         &(ReceiveCommandHandler::LcdData.SeekTime), TRUE,
         sizeof(ReceiveCommandHandler::LcdData.SeekTime) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.SeekTime)},
		{ REG_EXTFREQ,
         &(ReceiveCommandHandler::LcdData.FreqOffsetFlag), TRUE,
         sizeof(ReceiveCommandHandler::LcdData.FreqOffsetFlag),ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.FreqOffsetFlag) },
		{ REG_FREQOFFSET,
		   &(ReceiveCommandHandler::LcdData.FreqOffsetValue),
         TRUE, sizeof(ReceiveCommandHandler::LcdData.FreqOffsetValue) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.FreqOffsetValue)},
		{ REG_CUTOFFS,
		   &(ReceiveCommandHandler::LcdData.CutOffs), TRUE,
		   sizeof(ReceiveCommandHandler::LcdData.CutOffs),ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.CutOffs) },
		{ REG_LIMITS,
         &(ReceiveCommandHandler::LcdData.Limits), TRUE,
         sizeof(ReceiveCommandHandler::LcdData.Limits),ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.Limits) },
		{ REG_IP1,
         &(ReceiveCommandHandler::LcdData.IP1), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.IP1),ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.IP1) },
		{ REG_IP2,
         &(ReceiveCommandHandler::LcdData.IP2), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.IP2) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.IP2)},
		{ REG_IP3,
         &(ReceiveCommandHandler::LcdData.IP3), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.IP3) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.IP3)},
		{ REG_IP4,
         &(ReceiveCommandHandler::LcdData.IP4), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.IP4),ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.IP4) },
		{ REG_GATEWAY1,
         &(ReceiveCommandHandler::LcdData.GateWay1), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.GateWay1) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.GateWay1)},
		{ REG_GATEWAY2,
         &(ReceiveCommandHandler::LcdData.GateWay2), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.GateWay2) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.GateWay2)},
		{ REG_GATEWAY3,
         &(ReceiveCommandHandler::LcdData.GateWay3), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.GateWay3) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.GateWay3)},
		{ REG_GATEWAY4,
         &(ReceiveCommandHandler::LcdData.GateWay4), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.GateWay4) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.GateWay4)},
		{ REG_SUBNETMASK1,
         &(ReceiveCommandHandler::LcdData.SubNetMask1), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.SubNetMask1) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.SubNetMask1)},
		{ REG_SUBNETMASK2,
		   &(ReceiveCommandHandler::LcdData.SubNetMask2), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.SubNetMask2) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.SubNetMask2)},
		{ REG_SUBNETMASK3,
		   &(ReceiveCommandHandler::LcdData.SubNetMask3), FALSE,
		   sizeof(ReceiveCommandHandler::LcdData.SubNetMask3) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.SubNetMask3)},
		{ REG_SUBNETMASK4,
		   &(ReceiveCommandHandler::LcdData.SubNetMask4), FALSE,
		   sizeof(ReceiveCommandHandler::LcdData.SubNetMask4),ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.SubNetMask4) },
		{ REG_IPSETUP,
         &(ReceiveCommandHandler::LcdData.IPSetUp), FALSE,
         sizeof(ReceiveCommandHandler::LcdData.IPSetUp) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.IPSetUp)},
		{ REG_PROFIBUS_ADDR,
         &(ReceiveCommandHandler::LcdData.ProfiBusAddr), TRUE,
         sizeof(ReceiveCommandHandler::LcdData.ProfiBusAddr),ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.ProfiBusAddr) },
		{ REG_RESERVED,
		   &(ReceiveCommandHandler::LcdData.Reserved), FALSE,
		   sizeof(ReceiveCommandHandler::LcdData.Reserved) ,ALLENABLE,  &(ReceiveCommandHandler::LcdDataLast.WeldMode)},
		{ REG_WELDMODE,
         &(ReceiveCommandHandler::LcdData.WeldMode), TRUE,
         sizeof(ReceiveCommandHandler::LcdData.WeldMode),ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.Reserved) },

		{ REG_ETHIP1,
		&(ReceiveCommandHandler::LcdData.EthernetIP1), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.EthernetIP1),DCXF, &(ReceiveCommandHandler::LcdDataLast.EthernetIP1) },
		{ REG_ETHIP2,
		&(ReceiveCommandHandler::LcdData.EthernetIP2), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.EthernetIP2),DCXF, &(ReceiveCommandHandler::LcdDataLast.EthernetIP2) },
		{ REG_ETHIP3,
		&(ReceiveCommandHandler::LcdData.EthernetIP3), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.EthernetIP3) ,DCXF, &(ReceiveCommandHandler::LcdDataLast.EthernetIP3)},
		{ REG_ETHIP4,
		&(ReceiveCommandHandler::LcdData.EthernetIP4), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.EthernetIP4),DCXF, &(ReceiveCommandHandler::LcdDataLast.EthernetIP4) },
		{ REG_ETH_GATEWAY_1,
		&(ReceiveCommandHandler::LcdData.GatewayEthIP1), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.GatewayEthIP1) ,DCXF, &(ReceiveCommandHandler::LcdDataLast.GatewayEthIP1)},
		{ REG_ETH_GATEWAY_2,
		&(ReceiveCommandHandler::LcdData.GatewayEthIP2), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.GatewayEthIP2) ,DCXF, &(ReceiveCommandHandler::LcdDataLast.GatewayEthIP2)},
		{ REG_ETH_GATEWAY_3,
		&(ReceiveCommandHandler::LcdData.GatewayEthIP3), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.GatewayEthIP3) ,DCXF, &(ReceiveCommandHandler::LcdDataLast.GatewayEthIP3)},
		{ REG_ETH_GATEWAY_4,
		&(ReceiveCommandHandler::LcdData.GatewayEthIP4), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.GatewayEthIP4) ,DCXF, &(ReceiveCommandHandler::LcdDataLast.GatewayEthIP4)},
		{ REG_SUBNET_ETH_1,
		&(ReceiveCommandHandler::LcdData.SubNetEthIP1), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.SubNetEthIP1) ,DCXF, &(ReceiveCommandHandler::LcdDataLast.SubNetEthIP1)},
		{ REG_SUBNET_ETH_2,
		&(ReceiveCommandHandler::LcdData.SubNetEthIP2), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.SubNetEthIP2) ,DCXF, &(ReceiveCommandHandler::LcdDataLast.SubNetEthIP2)},
		{ REG_SUBNET_ETH_3,
		&(ReceiveCommandHandler::LcdData.SubNetEthIP3), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.SubNetEthIP3) ,DCXF, &(ReceiveCommandHandler::LcdDataLast.SubNetEthIP3)},
		{ REG_SUBNET_ETH_4,
		&(ReceiveCommandHandler::LcdData.SubNetEthIP4), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.SubNetEthIP4) ,DCXF, &(ReceiveCommandHandler::LcdDataLast.SubNetEthIP4) },
		{ REG_DEFNET_INFO,
		&(ReceiveCommandHandler::LcdData.DefNetInfo), FALSE,
		sizeof(ReceiveCommandHandler::LcdData.DefNetInfo) ,DCXF, &(ReceiveCommandHandler::LcdDataLast.DefNetInfo)},

		{ REG_TIME_PLR,
        &(ReceiveCommandHandler::LcdData.TimePLR), TRUE,
        sizeof(ReceiveCommandHandler::LcdData.TimePLR), ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.TimePLR)},
        { REG_TIME_MLR,
		&(ReceiveCommandHandler::LcdData.TimeMLR), TRUE,
		sizeof(ReceiveCommandHandler::LcdData.TimeMLR), ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.TimeMLR)},
		{ REG_ENERGY_PLR,
		&(ReceiveCommandHandler::LcdData.EnergyPLR),
		TRUE, sizeof(ReceiveCommandHandler::LcdData.EnergyPLR), ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.EnergyPLR)},
		{ REG_ENERGY_MLR,
		&(ReceiveCommandHandler::LcdData.EnergyMLR),
		TRUE, sizeof(ReceiveCommandHandler::LcdData.EnergyMLR), ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.EnergyMLR)},
		{ REG_PEAKPOWER_PLR,
		&(ReceiveCommandHandler::LcdData.PeakPowerPLR),
		TRUE, sizeof(ReceiveCommandHandler::LcdData.PeakPowerPLR), ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.PeakPowerPLR)},
		{ REG_PEAKPOWER_MLR,
		&(ReceiveCommandHandler::LcdData.PeakPowerMLR),
		TRUE, sizeof(ReceiveCommandHandler::LcdData.PeakPowerMLR), ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.PeakPowerMLR)},

		{ REG_AMPLITUDE,
         &(ReceiveCommandHandler::LcdData.Amplitude), TRUE,
         sizeof(ReceiveCommandHandler::LcdData.Amplitude) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.Amplitude)},
		{ REG_TIMEMODE_VAL,
         &(ReceiveCommandHandler::LcdData.TimeModeValue), TRUE,
         sizeof(ReceiveCommandHandler::LcdData.TimeModeValue) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.TimeModeValue)},
		{ REG_ENERGYMODE_VAL,
		   &(ReceiveCommandHandler::LcdData.EnergyModeValue),
		   TRUE, sizeof(ReceiveCommandHandler::LcdData.EnergyModeValue) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.EnergyModeValue)},
		{ REG_PKPWRMODE_VAL,
		   &(ReceiveCommandHandler::LcdData.PKPwrModeValue),
		   TRUE, sizeof(ReceiveCommandHandler::LcdData.PKPwrModeValue) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.PKPwrModeValue)},
		{ REG_GNDDETECTMODE_VAL,
         &(ReceiveCommandHandler::LcdData.GndDetectModeValue), TRUE,
         sizeof(ReceiveCommandHandler::LcdData.GndDetectModeValue) ,ALLENABLE, &(ReceiveCommandHandler::LcdDataLast.GndDetectModeValue)},
        { REG_SYSTEMTYPE_VAL,
         &(ReceiveCommandHandler::LcdData.SystemTypeValue), TRUE,
         sizeof(ReceiveCommandHandler::LcdData.SystemTypeValue),ALLENABLE,&(ReceiveCommandHandler::LcdDataLast.SystemTypeValue)},
         { REG_PRESET_NUM,
          &(ReceiveCommandHandler::LcdData.PresetNo), TRUE,
          sizeof(ReceiveCommandHandler::LcdData.PresetNo),ALLENABLE,&(ReceiveCommandHandler::LcdDataLast.PresetNo)},
		{ REG_NONE, 0, FALSE, 0, NOTUSED , 0} };


/*  ReceiveCommandHandler()
 *
 *  Purpose:
 *    ReceiveCommandHandler Constructor.
 *    To initialize the task stack and member variables of class.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None
 */
ReceiveCommandHandler::ReceiveCommandHandler() :
	Task("RxHandler", RECEIVECOMMHANDLER_STACKSIZE), Port(1536, 1536) {
	thisptr = this;
	Interface = ETHERNET_INTERFACE_WCTOWEB;
	LcdCommunicationLost = TRUE;
	SentSystemInfoCmd = FALSE;
	SystemInfoReceived = FALSE;
	CmdReceived = FALSE;
	CurrNWConfig = configDefault;
	Port.Init(0, LCDBAUDRATE, 3, 3, FALSE);
	NetIf = 0;
	ConfigureLCDRst();
}

/*  void ReceiveCommandHandler::Run(void)
 *
 *  Purpose
 *    This function is Run function of ReceiveCommandHandler task.
 *    This function is responsible for receiving LCD command in Motorola S9 record format and process it accordingly.
 *
 *  EntryCondition:
 *    None.
 *
 *  ExitCondition:
 *    None.
 */
void ReceiveCommandHandler::Run(void) {
	//Initially Send reset pulse to LCD hardware. It will also be useful to restart the LCD if WC gets soft reset.
	SendResetToLCD();
	static enum ReceiveState RecState = STATE_WAIT_START;
	static UINT32 ChCnt = 0;
	static UINT16 LEDState = LedRed;//Red
	UINT8 Ch;
	MacAddress = CyGOS::GetMacAddress(Interface);
	NetIf = CyGOS::GetNetworkInterface(Interface);
	CurrNWConfig = CyGOS::GetNetworkConfiguration(Interface);
	OldLinkStat = NetIf->up;
	while (FOREVER) {
		Ch = 0;
		//Wait for charater to come from LCD.
		Port.Receive(this, Ch, -1);
		if (Ch == 'S')
		   RecState = STATE_WAIT_START;
		if (DebugLCD == TRUE) {
			printf("%c", Ch);
		}
		switch (RecState) {
		case STATE_WAIT_START://Start character
			if (Ch == 'S') {
				ChCnt = 0;
				SerialRecBuff[ChCnt] = Ch;
				ChCnt++;
				RecState = STATE_RECEIVE;
			}
			break;
		case STATE_RECEIVE:
			SerialRecBuff[ChCnt] = Ch;
			ChCnt++;
			if (ChCnt >= RXBUFSIZE) {
				ChCnt = 0;
				RecState = STATE_WAIT_START;
			} else if (Ch == CR)
				RecState = STATE_WAIT_LF;
			break;
		case STATE_WAIT_LF:
			if (Ch == LF) {
				SerialRecBuff[ChCnt] = Ch;
				CmdReceived = TRUE;
				memcpy(CrashSerialRecBuff,SerialRecBuff,ChCnt);
				ProcessCommand(SerialRecBuff);
				ChCnt = 0;
				RecState = STATE_WAIT_START;
				if (LEDState == LedRed) {
					LEDState = LedGreen;
					LED::Set(1, LedGreen);
				} else {
					LEDState = LedRed;
					LED::Set(1, LedRed);
				}
			}
			break;
		default:
			ChCnt = 0;
			RecState = STATE_WAIT_START;
			break;
		}
	}
}

/* UINT8 ReceiveCommandHandler::VerifyCheckSum(UINT8 * Data, UINT8 DataLen)
 *
 *  Purpose
 *    This function verify Check Sum of LCD command.This function is called by ReceiveCommandHandler::ProcessCommand().
 *
 *  EntryCondition:
 *    Data - indicates pointer to LCD command on which checksum needs to be calculated.
 *    DataLen - indicates length of data buffer on which Checksum is calculated.
 *
 *  ExitCondition:
 *    Return 0 if CRC Matched else 1.
 */
UINT8 ReceiveCommandHandler::VerifyCheckSum(UINT8 * Data, UINT8 DataLen) {
	UINT8 ReceivedCRC, CalculatedCRC = 0, CRCTemp, Indx, DataIndx = 0;
	//Calculate CRC for received Data Length.
	UINT16 Len = 0;
	Len = sizeof(CRCTemp);
	for (Indx = 0; Indx < DataLen; Indx++) {
		AsciiToHex(&CRCTemp, (int) Len, &Data[DataIndx]);
		CalculatedCRC ^= CRCTemp;
		DataIndx += (Len * 2);
	}
	AsciiToHex(&ReceivedCRC, sizeof(ReceivedCRC), &Data[DataIndx]);
	// Return 0 if CRC Matched else 1.
	return (ReceivedCRC ^ CalculatedCRC);
}

/* void ReceiveCommandHandler::ProcessCommand(UINT8 * LcdCommand)
 *
 *  Purpose
 *    This function processes polling and data command.This function is called by ReceiveCommandHandler::Run().
 *
 *  EntryCondition:
 *    LcdCommand - indicates pointer to LCD command.
 *
 *  ExitCondition:
 *    None.
 */
void ReceiveCommandHandler::ProcessCommand(UINT8 * LcdCommand) {
	UINT8 TotalBytes;
	UINT16 Addr, RegAddr;
	UINT8 BuffIndex = 1;
	UINT8 ByteCount = 0;
	UINT32 RegValue;
	UINT8 CrcByte;
	UINT8 VerifyChkSum, Cmd;
	UINT8 RegSize = 0;
	UINT8 Tempvar = 0;
	TokenAvailable = IsAccessTokenAvailable();
	if (LcdCommand[BuffIndex] == POLLING) {//reply of POLLING
		BuffIndex++;
		//Get the total byte count out of command.
		AsciiToHex(&TotalBytes, sizeof(TotalBytes), &LcdCommand[BuffIndex]);
		//Verify CheckSum for Total Received Bytes.
		VerifyChkSum = VerifyCheckSum(&LcdCommand[BuffIndex], TotalBytes);

		if (VerifyChkSum == 0) {//Checksum OK
			if (LcdCommunicationLost) {
				//We need to Get LCD information if we lost the LCD communication or
				//if this is the first time we are getting any valid command from LCD.
				LcdCommunicationLost = FALSE;
				SentSystemInfoCmd = FALSE;
			}

			BuffIndex += (sizeof(TotalBytes) * 2);//Increment for Total byte.
			//Get the Key status byte
			AsciiToHex(&Addr, sizeof(Addr), &LcdCommand[BuffIndex]);//Addr is the Key status in Polling.
			BuffIndex += (sizeof(Addr) * 2);//Increment for Key status byte.
			AsciiToHex(&Cmd, sizeof(Cmd), &LcdCommand[BuffIndex]);
			BuffIndex += (sizeof(Cmd) * 2);//Increment for Cmd byte.
			AsciiToHex(&RegAddr, sizeof(RegAddr), &LcdCommand[BuffIndex]);//RegAddr
			BuffIndex += (sizeof(RegAddr) * 2);
			switch (Addr) {
			case KB_RESET:
				if (IsManualFunctionAvailable() && (WebIOEnabled == FALSE)) {
					LCDKeyStatus = KB_RESET;
					ControlVariable |= LCDRESET;
				}
				break;
			case KB_TEST:
				if (IsManualFunctionAvailable() && (WebIOEnabled == FALSE)) {
					LCDKeyStatus = KB_TEST;
					ControlVariable |= LCDTEST;
				}
				break;
			case PRESET_RECALL:
				Tempvar = (RegAddr&0xFF);
				//Recall preset no. 1 to 32 only
				if((Tempvar > 0) && (Tempvar <= MAXIMUM_USER_PRESETS)){
					if(RecallPreset((PRESET_NUMBER)Tempvar)){
						if(AdvanceConfigData->Advrnd.Flag2)
						   Log::WriteEventLog(EVENT_PRESET_RECALL, Tempvar, COMM_IFC_LCD);
					}
				}
				RegAddr = 0; //to handle polling command reply
				RegUpdateCmdFlag = TRUE;
				break;
			default:
				LCDKeyStatus = 0;
				ControlVariable &= ~(LCDTEST | LCDRESET);
				break;
			}

			if (Cmd == CMD_NOCMD) {
				if (RegUpdateCmdFlag == TRUE) {
					//RegUpdateCmdFlag represents that something is changed either from WEBPages or from FB.
					// Update all registers on LCD side in this case.
					RegUpdateCmdFlag = FALSE;
					TransmitPollingCommandReply(CMD_GET_ALLREG_CURRENTVALUE,
							REG_NONE);
				} else if (SentSystemInfoCmd == FALSE) {
					//Get LCD S/W version and CRC of the LCD code.
					TransmitPollingCommandReply(CMD_GET_SYSTEMINFO, REG_NONE);
					SentSystemInfoCmd = TRUE;
				} else if (RegAmplitudeUpdateFlag == TRUE) {
					//Amplitude is changed from Manufacturing webpage so update it on LCD.
					TransmitPollingCommandReply(CMD_GET_REGVALUE, REG_AMPLITUDE);
					RegAmplitudeUpdateFlag = FALSE;
				} else if (RegMinUpdateFlag == TRUE) {
					//Minimum values needs to be updated on LCD. It will happen if something is changed in MIN
					//preset or in tweak min preset.
					TransmitPollingCommandReply(CMD_GET_ALLREG_MINVALUES,
							REG_NONE);
					RegMinUpdateFlag = FALSE;
				} else if (RegMaxUpdateFlag == TRUE) {
					//Maximum values needs to be updated on LCD. It will happen if something is changed in MAX
					//preset or in tweak max preset.
					TransmitPollingCommandReply(CMD_GET_ALLREG_MAXVALUES,
							REG_NONE);
					RegMaxUpdateFlag = FALSE;
				} else {// Reply the value of requested register.
					TransmitPollingCommandReply(Cmd, RegAddr);
				}
			} else {// Reply the value of requested register.
				TransmitPollingCommandReply(Cmd, RegAddr);
			}
		} else {
			printf("\n checkSUM Error");
		}
	} else if (LcdCommand[BuffIndex] == DATACMD) {//Register data update commands.
		BuffIndex++;
		//Get the total byte count out of command.
		AsciiToHex(&TotalBytes, sizeof(TotalBytes), &LcdCommand[BuffIndex]);
		//Calculate and verify checksum.
		VerifyChkSum = VerifyCheckSum(&LcdCommand[BuffIndex], TotalBytes);
		if (VerifyChkSum == 0) {//CheckSum OK
			BuffIndex += (sizeof(TotalBytes) * 2);
			while (TotalBytes > (ByteCount + 1)) {// Loop thorugh total bytes received.
				if (TotalBytes >= ByteCount + 3) {// 2 bytes Address and at least 1 byte data.
					AsciiToHex(&Addr, sizeof(Addr), &LcdCommand[BuffIndex]);
					BuffIndex += (sizeof(Addr) * 2);
					ByteCount += sizeof(Addr);
					RegSize = (Addr & 0xF000) >> 12; // Upper 4 bits are carrying Size of register.
					Addr = Addr & 0x0FFF; // Lower 12 bits has the Register number.
					//read value
					RegValue = 0;
					AsciiToHex(&RegValue, RegSize, &LcdCommand[BuffIndex]);

					BuffIndex += (RegSize * 2);
					ByteCount += RegSize;
					// convert from Bigendian to little endian format.
					// RegValue is 4 byte variable and if register size is one byte than value will be in upper 8 bits. Shift the RegValue according
					//to the size of Register.
					RegValue = RegValue >> ((sizeof(RegValue) - RegSize)
							* BITS_IN_A_BYTE);
					if (DebugLCD == TRUE) {
//						printf("RG %d RS = %d, RV = %d ", (DataReg) Addr,
//								RegSize, RegValue);
					}
					//Process the value of register.
					ProcessRegUpdateCommand((DataReg) Addr, RegValue);
				} else {
					BuffIndex += 2;// totalBytes - 1;
					ByteCount++;
				}
			}
			AsciiToHex(&CrcByte, sizeof(CrcByte), &LcdCommand[BuffIndex]);
		} else {
			printf("\n checkSUMError data cmd");
		}
	} else {
		printf(" LCD command not supported ");
	}
}

/* UINT8 ReceiveCommandHandler::PrepareDCXStatusByte(void)
 *
 *  Purpose
 *    This function prepares DCP status to be send to LCD.This function is called by ReceiveCommandHandler::TransmitPollingCommandReply().
 *
 *  EntryCondition:
 *    None.
 *
 *  ExitCondition:
 *    Return DCX Status Flag.
 */
UINT8 ReceiveCommandHandler::PrepareDCXStatusByte(void) {
	//Get DCX Status.
	DCXStatus.Status.StatusFlags = 0;
	//Alarm
	DCXStatus.Status.Flags.Alarm = UserIO::GetAlarmStatus() || AlarmIndex;
	//System Ready
	DCXStatus.Status.Flags.Ready = UserIO::GetReadyFlag();
	//Memory Reset
	DCXStatus.Status.Flags.MemReset = MemoryReset;
	//Sonics are Active or not.
	if ((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) == MB_SONICACTIVE_OUT)
		DCXStatus.Status.Flags.SonicsActive = TRUE;
	else
		DCXStatus.Status.Flags.SonicsActive = FALSE;

	if (TokenAvailable == FALSE) //Token is acquired by FieldBus/ProfiBus
		DCXStatus.Status.Flags.DispLock = TRUE;
	else {
		DCXStatus.Status.Flags.DispLock = FALSE;
		DCXStatus.Status.Flags.TweakFlag = FALSE;
		if (PasscodeConfig->PassCodeRequired == TRUE) {
			if (Sysconfig->TweakRangeEnabled == TRUE) {
				//tweaking is enabled through the web page, then if passcodes are required, the main parameter
				//(time in time mode, energy in energy mode, power in peak power mode) and amplitude can be changed from the LCD
				DCXStatus.Status.Flags.TweakFlag = TRUE;
			} else
				DCXStatus.Status.Flags.DispLock = TRUE;
		} else
			// LCD Display Lock.
			DCXStatus.Status.Flags.DispLock = (UserIO::CheckForLockOutChange()
					|| WebSite::CheckForManufacturingFlag());
	}
	return DCXStatus.Status.StatusFlags;
}



/*  void ReceiveCommandHandler::TransmitPollingCommandReply(UINT8 Cmd, UINT16 DataReg)
 *
 *  Purpose
 *    This function prepares polling command reply to be send to LCD.
 *    This function is called by ReceiveCommandHandler::ProcessCommand().
 *
 *  Entry Condition:
 *    Cmd - it indicates Polling Response Cmd.
 *    DataReg - Data Reg number.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::TransmitPollingCommandReply(UINT8 Cmd,
		UINT16 DataReg) {
	UINT16 Indx = 0;
	struct SerialCommand Command;
	UINT8 PeakPower, DCXStatusByte, DataByteCount;
	UINT16 MemoryFreq;
	UINT16 PowerInput = (UINT8) RtReceiveData.PowerOut;
	UINT8 CRCbyte = 0, indx;
	DataByteCount = 0;
	//DCX Status(Alarm/Ready/Sonics/MemoryReset/DisplayLock)
	DCXStatusByte = PrepareDCXStatusByte();
	static BOOLEAN FirstTime = TRUE;
	if ((DCXStatus.Status.Flags.SonicsActive) && (PowerInput <= 0))
		PowerInput = 1;//set min power to 1% whenever sonics are Active to display Lightning bolt on LCD.
	//Convert Peak power in percentage form
	PeakPower = TempPeakPower;
	//Convert MemoryFreq
	MemoryFreq
			= SCALE_DOWN_FOR_N_BITS(HsParameterSet[CurrHsPreset].DigitalTuneScaledUP
					+ HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP)
					/ FREQ_SCALING_FACTOR;


	Command.StartChar = STARTCHAR;
	Command.RecordType = POLLING;
	Command.ByteCount = 0;
	Command.Addr = 0;
	Command.ByteCount += sizeof(Command.Addr);
	Command.Data[DataByteCount++] = DCXStatusByte;
	Command.Data[DataByteCount++] = (UINT8) PowerInput;
	Command.Data[DataByteCount++] = PeakPower;
	Command.Data[DataByteCount++] = MemoryFreq >> BITS_IN_A_BYTE;
	Command.Data[DataByteCount++] = MemoryFreq;
	/////////////////
	Command.Data[DataByteCount++] = CurrentAlarm >> BITS_IN_A_BYTE; //Alarm Number.
	Command.Data[DataByteCount++] = CurrentAlarm;
	/////////////////
	switch (Cmd) {
	case CMD_NOCMD:
		Command.Data[DataByteCount++] = CMD_NOCMD;
		Command.Data[DataByteCount++] = 0x00;
		Command.Data[DataByteCount++] = 0x00;
		break;
	case CMD_GET_REGVALUE:
		UpdateLCDData();
		Command.Data[DataByteCount++] = Cmd;
		for (Indx = 0; SerialCmdReg[Indx].Data; Indx++) {
			if (SerialCmdReg[Indx].DataReg == DataReg) {
				//Register number is in lower 12 bits. Upper four bits is the size of register.
				Command.Data[DataByteCount++] = (((SerialCmdReg[Indx].DataReg
						>> BITS_IN_A_BYTE) & 0x0F)
						| ((SerialCmdReg[Indx].DataLen << 4) & 0xF0));
				Command.Data[DataByteCount++] = (SerialCmdReg[Indx].DataReg)
						& 0xFF;
				for (int j = 0; j < SerialCmdReg[Indx].DataLen; j++)
					Command.Data[DataByteCount++]
							= ((UINT8*) SerialCmdReg[Indx].Data)[j];
			}
		}
		break;
	case CMD_GET_ALLREG_CURRENTVALUE:
		UpdateLCDData();
		Command.Data[DataByteCount++] = Cmd;
      for (Indx = 0; SerialCmdReg[Indx].Data; Indx++) {
         //Register number is in lower 12 bits. Upper four bits is the size of register.
         if ((FirstTime == TRUE) || CheckIfChanged(Indx)) {
            if ((SerialCmdReg[Indx].RegEnable
                  & (BIT(MacchipData.CurrSystemType))) != 0) {
               Command.Data[DataByteCount++] = (((SerialCmdReg[Indx].DataReg
                     >> BITS_IN_A_BYTE) & 0x0F)
                     | ((SerialCmdReg[Indx].DataLen << 4) & 0xF0));
               Command.Data[DataByteCount++] = (SerialCmdReg[Indx].DataReg)
                     & 0xFF;
               for (int j = 0; j < SerialCmdReg[Indx].DataLen; j++) {
                  Command.Data[DataByteCount++] =
                        ((UINT8*) SerialCmdReg[Indx].DataLast)[j] =
                              ((UINT8*) SerialCmdReg[Indx].Data)[j];
               }
            }
         }
		}
        FirstTime = FALSE;
		break;
	case CMD_GET_ALLREG_DEFAULTVALUE:
		GetDefaultParameterValues();
		Command.Data[DataByteCount++] = Cmd;
		for (Indx = 0; SerialCmdReg[Indx].Data; Indx++) {
			//Register number is in lower 12 bits. Upper four bits is the size of register.
			if((SerialCmdReg[Indx].RegEnable & (BIT(MacchipData.CurrSystemType)))!=0){
			Command.Data[DataByteCount++] = (((SerialCmdReg[Indx].DataReg
					>> BITS_IN_A_BYTE) & 0x0F) | ((SerialCmdReg[Indx].DataLen
					<< 4) & 0xF0));
			Command.Data[DataByteCount++] = (SerialCmdReg[Indx].DataReg) & 0xFF;
			for (int j = 0; j < SerialCmdReg[Indx].DataLen; j++)
				Command.Data[DataByteCount++]
						= ((UINT8*) SerialCmdReg[Indx].Data)[j];
			}
		}
		break;
	case CMD_SET_ALLREG_TODEFAULTVAUE:
		if (DataReg == COLDSTART) {
			SetAllColdStartParameter();
			Log::WriteEventLog(EVENT_COLD_START, COMM_IFC_LCD);
		} else if(DataReg == RESTOREWELDPRESET){
			SetDefaultValues();//restore default to weld preset parameters
			Log::WriteEventLog(EVENT_RESTORE_DEFAULTS, COMM_IFC_LCD);
			RegUpdateCmdFlag = TRUE;
		}
		Command.Data[DataByteCount++] = CMD_NOCMD;
		Command.Data[DataByteCount++] = 0x00;
		Command.Data[DataByteCount++] = 0x00;
		break;
	case CMD_GET_SYSTEMINFO:
		Command.Data[DataByteCount++] = (UINT8) Cmd;
		for (Indx = 0; Indx < 6; Indx++)
			Command.Data[DataByteCount++] = MacAddress[Indx];
		break;
	case CMD_GET_ALLREG_MINVALUES:
		GetMinParameterValues();
		Command.Data[DataByteCount++] = Cmd;
		for (Indx = 0; SerialCmdReg[Indx].Data; Indx++) {
			//WC don't need to send the Min values of all the Registers.
		   //There are only few registers for which the MIN values can be changed
			//like WeldTime. Min Values are fixed for Registers like IPAddr or TimedSeek and can not be changed.
			if (SerialCmdReg[Indx].MinMaxValue == TRUE) {
				//Register number is in lower 12 bits. Upper four bits is the size of register.
            if ((SerialCmdReg[Indx].RegEnable
                  & (BIT(MacchipData.CurrSystemType))) != 0) {
               Command.Data[DataByteCount++] = (((SerialCmdReg[Indx].DataReg
                     >> BITS_IN_A_BYTE) & 0x0F)
                     | ((SerialCmdReg[Indx].DataLen << 4) & 0xF0));
               Command.Data[DataByteCount++] = (SerialCmdReg[Indx].DataReg)
                     & 0xFF;
               for (int j = 0; j < SerialCmdReg[Indx].DataLen; j++)
                  Command.Data[DataByteCount++] =
                        ((UINT8*) SerialCmdReg[Indx].Data)[j];
            }
			}
		}
		break;
	case CMD_GET_ALLREG_MAXVALUES:
		GetMaxParameterValues();
		Command.Data[DataByteCount++] = Cmd;
		for (Indx = 0; SerialCmdReg[Indx].Data; Indx++) {
			//WC don't need to send the MAX values of all the Registers.
		   //There are only few registers for which the MAX values can be changed
			//like WeldTime. MAX Values are fixed for Registers like IPAddr or TimedSeek and can not be changed.
         if (SerialCmdReg[Indx].MinMaxValue == TRUE) {
            //Register number is in lower 12 bits. Upper four bits is the size of register.
            if ((SerialCmdReg[Indx].RegEnable
                  & (BIT(MacchipData.CurrSystemType))) != 0) {
               Command.Data[DataByteCount++] = (((SerialCmdReg[Indx].DataReg
                     >> BITS_IN_A_BYTE) & 0x0F)
                     | ((SerialCmdReg[Indx].DataLen << 4) & 0xF0));
               Command.Data[DataByteCount++] = (SerialCmdReg[Indx].DataReg)
                     & 0xFF;
               for (int j = 0; j < SerialCmdReg[Indx].DataLen; j++)
                  Command.Data[DataByteCount++] =
                        ((UINT8*) SerialCmdReg[Indx].Data)[j];
            }
         }
		}
		break;
	default:
		Command.Data[DataByteCount++] = Cmd;
		Command.Data[DataByteCount++] = 0x00;
		Command.Data[DataByteCount++] = 0x00;
		break;
	}
	//Now Calculate CRC on Data bytes.
	for (indx = 0; indx < DataByteCount; indx++)
		CRCbyte ^= Command.Data[indx];
	Command.ByteCount += DataByteCount;
	Command.ByteCount += sizeof(Command.CheckSum);
	CRCbyte ^= Command.ByteCount;
	Command.CheckSum = CRCbyte;
	Command.Cr = CR;
	Command.Lf = LF;
	//Finally transmit the command to LCD.
	TransmitAsciiCommand(&Command);
}

/* void ReceiveCommandHandler::HexToAscii(void * Var, int VarSize, UINT8 * Buff)
 *
 *  Purpose
 *    This routine converts hex number to ascii character.This function is called by ReceiveCommandHandler::TransmitAsciiCommand().
 *
 *  Entry Condition:
 *    * Var - Pointer to buffer that contains hex values.
 *    VarSize - Size of Buffer pointed to by Var pointer.
 *    Buff - Poniter to buffer in which ASCII values are to be filled.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::HexToAscii(void * Var, int VarSize, UINT8 * Buff) {
	SINT32 Indx;
	const char * HexStr = "0123456789ABCDEF";
	int Offset = 0;
	unsigned char * VarPtr = (unsigned char *) Var;
	int Temp = 0;
	for (Indx = 0; Indx < VarSize; Indx++) {
		Buff[Offset++] = HexStr[VarPtr[Temp] / HEXBASE];
		Buff[Offset++] = HexStr[VarPtr[Temp] % HEXBASE];
		Temp++;
	}
}

/* UINT8 ReceiveCommandHandler::xtoi(char HexNumber)
 *
 *  Purpose
 *    This routine converts ascii character to hex number.This function is called by AsciiToHex() function.
 *
 *  Entry Condition:
 *    HexNumber - value to be converted to hex.
 *
 *  Exit Condition:
 *    None.
 */
UINT8 ReceiveCommandHandler::xtoi(char HexNumber) {
	//'0' to '9' 48 to 57
	// 'A' to 'F' 65 to 70
	// 'a' to 'f'  97 to 102
	UINT8 Result = 0;
	if ((HexNumber >= '0') && (HexNumber <= '9'))
		Result |= HexNumber & 0x0F;
	else if ((HexNumber >= 'A') && (HexNumber <= 'F'))
		Result |= HexNumber - 'A' + DECIMALBASE;
	else if ((HexNumber >= 'a') && (HexNumber <= 'f'))
		Result |= HexNumber - 'a' + DECIMALBASE;
	else
		Result = 0;

	return Result;
}

/* void ReceiveCommandHandler::AsciiToHex(void * Var, int Varsize, UINT8 * Buff)
 *
 *  Purpose
 *    This function converts ASCII to Hex.This function is called by VerifyCheckSum(), ProcessCommand() functions.
 *
 *  Entry Condition:
 *    * Var - Pointer to buffer that contains ASCII values.
 *    VarSize - Size of Buffer pointed to by Var pointer.
 *    Buff - Poniter to buffer in which Hex values are to be filled.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::AsciiToHex(void * Var, int Varsize, UINT8 * Buff) {
	SINT32 Indx;
	UINT8 Offset = 0;
	UINT8 * VarPtr = (UINT8 *) Var;
	for (Indx = 0; Indx < Varsize; Indx++) {
		VarPtr[Indx] = (xtoi(Buff[Offset]) << 4) | (xtoi(Buff[Offset + 1]));
		Offset += 2;
	}
}

/* void ReceiveCommandHandler::TransmitAsciiCommand(struct SerialCommand * Cmd)
 *
 *  Purpose
 *    This function transmits ASCII command to LCD.This function is called by ReceiveCommandHandler::TransmitPollingCommandReply().
 *
 *  Entry Condition:
 *    SerialCommand Cmd - It is command to be sent.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::TransmitAsciiCommand(struct SerialCommand * Cmd) {
	SINT32 CharCnt = 0;
	SINT32 DataLen = 0, Indx;
	SerialTxBuff[CharCnt++] = Cmd->StartChar;
	SerialTxBuff[CharCnt++] = Cmd->RecordType;
	HexToAscii(&Cmd->ByteCount, sizeof(Cmd->ByteCount), &SerialTxBuff[CharCnt]);
	CharCnt += (sizeof(Cmd->ByteCount) * 2);
	HexToAscii(&Cmd->Addr, sizeof(Cmd->Addr), &SerialTxBuff[CharCnt]);
	CharCnt += (sizeof(Cmd->Addr) * 2);

	DataLen = Cmd->ByteCount - sizeof(Cmd->Addr) - sizeof(Cmd->CheckSum);
	for (Indx = 0; Indx < DataLen; Indx++) {
		HexToAscii(&Cmd->Data[Indx], sizeof(UINT8), &SerialTxBuff[CharCnt]);
		CharCnt += 2;
	}

	HexToAscii(&Cmd->CheckSum, sizeof(Cmd->CheckSum), &SerialTxBuff[CharCnt]);
	CharCnt += (sizeof(Cmd->CheckSum) * 2);
	SerialTxBuff[CharCnt++] = Cmd->Cr;
	SerialTxBuff[CharCnt++] = Cmd->Lf;
	if (DebugLCD == TRUE) {
		printf("\nSend ");
	}
	for (Indx = 0; Indx < CharCnt; Indx++) {
		Port.Send(this, SerialTxBuff[Indx], -1);
		if (DebugLCD == TRUE) {
			printf("%02x", SerialTxBuff[Indx]);
		}
	}
}



/*  void ReceiveCommandHandler::UpdateLCDData(void)
 *
 *  Purpose
 *    This function updates LcdData structure from Current Presets.
 *    This function is called by ReceiveCommandHandler::TransmitPollingCommandReply().
 *
 *  Entry Condition:
 *    None.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::UpdateLCDData(void) {
	ip_addr temp;
	LcdData.SystemTypeValue = 0;
	LcdData.SystemTypeValue = MacchipData.CurrSystemType + OFFSET;
	if((MacchipData.CurrSystemType == SYSTEM_FIELDBUS) || (MacchipData.CurrSystemType == SYSTEM_FIELDBUS_HD)){
	    LcdData.SystemTypeValue |= ((FieldBusType & 0xFF) << 8);
	}

	LcdData.SeekRampTime = HsParameterSet[CurrHsPreset].ParaSeek.SeekRampTime;
	LcdData.WeldRampTime = ParameterSet[CurrPreset].DcpParam.WeldRampTime;
	LcdData.TimedSeekFlag = ParameterSet[CurrPreset].DcpParam.TimedSeekFlag;
	LcdData.SeekTime = HsParameterSet[CurrHsPreset].ParaSeek.SeekTime;
	if (CurrentCycleParam->MfCycling)
		LcdData.Amplitude = CurrentCycleParam->Amplitude;
	else
		LcdData.Amplitude = ParameterSet[CurrPreset].WcParam.Amplitude1;
	LcdData.AmpStepCondition
			= (ParameterSet[CurrPreset].WcParam.AmpStepCondition
					== AMPSTEP_COND_EXTERNALAIN);
	LcdData.FreqOffsetFlag = ParameterSet[CurrPreset].WcParam.FreqOffsetFlag;
	switch (Sysconfig->CurrNetworkConfig) {
	case configDefault:
	case configStatic:
		LcdData.IP1 = (Sysconfig->SystemIPStatic.addr >> BITS_IN_THREE_BYTE)
				& 0xff;
		LcdData.IP2 = (Sysconfig->SystemIPStatic.addr >> BITS_IN_TWO_BYTE)
				& 0xff;
		LcdData.IP3 = (Sysconfig->SystemIPStatic.addr >> BITS_IN_A_BYTE) & 0xff;
		LcdData.IP4 = (Sysconfig->SystemIPStatic.addr & 0xff);

		LcdData.SubNetMask1 = (Sysconfig->Netmask.addr >> BITS_IN_THREE_BYTE)
				& 0xff;
		LcdData.SubNetMask2 = (Sysconfig->Netmask.addr >> BITS_IN_TWO_BYTE)
				& 0xff;
		LcdData.SubNetMask3 = (Sysconfig->Netmask.addr >> BITS_IN_A_BYTE) & 0xff;
		LcdData.SubNetMask4 = (Sysconfig->Netmask.addr & 0xff);

		LcdData.GateWay1 = (Sysconfig->Gw.addr >> BITS_IN_THREE_BYTE) & 0xff;
		LcdData.GateWay2 = (Sysconfig->Gw.addr >> BITS_IN_TWO_BYTE) & 0xff;
		LcdData.GateWay3 = (Sysconfig->Gw.addr >> BITS_IN_A_BYTE) & 0xff;
		LcdData.GateWay4 = (Sysconfig->Gw.addr & 0xff);
		break;
	case configDhcpServer:
		LcdData.IP1 = (Sysconfig->SystemIP.addr >> BITS_IN_THREE_BYTE) & 0xff;
		LcdData.IP2 = (Sysconfig->SystemIP.addr >> BITS_IN_TWO_BYTE) & 0xff;
		LcdData.IP3 = (Sysconfig->SystemIP.addr >> BITS_IN_A_BYTE) & 0xff;
		LcdData.IP4 = (Sysconfig->SystemIP.addr & 0xff);

		LcdData.SubNetMask1 = (Sysconfig->DhcpConfig.netmask.addr
				>> BITS_IN_THREE_BYTE) & 0xff;
		LcdData.SubNetMask2 = (Sysconfig->DhcpConfig.netmask.addr
				>> BITS_IN_TWO_BYTE) & 0xff;
		LcdData.SubNetMask3 = (Sysconfig->DhcpConfig.netmask.addr
				>> BITS_IN_A_BYTE) & 0xff;
		LcdData.SubNetMask4 = (Sysconfig->DhcpConfig.netmask.addr & 0xff);

		LcdData.GateWay1 = (Sysconfig->DhcpConfig.gateway.addr
				>> BITS_IN_THREE_BYTE) & 0xff;
		LcdData.GateWay2 = (Sysconfig->DhcpConfig.gateway.addr
				>> BITS_IN_TWO_BYTE) & 0xff;
		LcdData.GateWay3
				= (Sysconfig->DhcpConfig.gateway.addr >> BITS_IN_A_BYTE) & 0xff;
		LcdData.GateWay4 = (Sysconfig->DhcpConfig.gateway.addr & 0xff);
		break;
	case configDhcpClient:
		temp = CyGOS::GetIPAddress(Interface);
		LcdData.IP1 = (temp.addr >> BITS_IN_THREE_BYTE) & 0xff;
		LcdData.IP2 = (temp.addr >> BITS_IN_TWO_BYTE) & 0xff;
		LcdData.IP3 = (temp.addr >> BITS_IN_A_BYTE) & 0xff;
		LcdData.IP4 = (temp.addr & 0xff);

		temp = CyGOS::GetSubnetMask(Interface);
		LcdData.SubNetMask1 = (temp.addr >> BITS_IN_THREE_BYTE) & 0xff;
		LcdData.SubNetMask2 = (temp.addr >> BITS_IN_TWO_BYTE) & 0xff;
		LcdData.SubNetMask3 = (temp.addr >> BITS_IN_A_BYTE) & 0xff;
		LcdData.SubNetMask4 = (temp.addr & 0xff);

		temp = CyGOS::GetGatewayAddress(Interface);
		LcdData.GateWay1 = (temp.addr >> BITS_IN_THREE_BYTE) & 0xff;
		LcdData.GateWay2 = (temp.addr >> BITS_IN_TWO_BYTE) & 0xff;
		LcdData.GateWay3 = (temp.addr >> BITS_IN_A_BYTE) & 0xff;
		LcdData.GateWay4 = (temp.addr & 0xff);
		break;
	default:
		break;
	}
	LcdData.IPSetUp = (UINT16) Sysconfig->CurrNetworkConfig;
	LcdData.PowerupOption = Sysconfig->PowerupOption;
	LcdData.MemoryStoreFlag = ParameterSet[CurrPreset].DcpParam.MemoryStoreFlag;
	LcdData.FreqOffsetValue = (ParameterSet[CurrPreset].WcParam.FreqOffset
			/ FREQ_SCALING_FACTOR);
	LcdData.CutOffs = ParameterSet[CurrPreset].WcParam.CutOffEnabled;
	LcdData.Limits = ParameterSet[CurrPreset].LimitsParam.ControlLimits
			& CONTROL_LIMITS_ENABLE;
	LcdData.WeldMode = ParameterSet[CurrPreset].WcParam.WeldMode;
	//Limit Registers
	LcdData.TimePLR = 0;
	LcdData.TimeMLR = 0;
	LcdData.EnergyPLR = 0;
	LcdData.EnergyMLR = 0;
	LcdData.PeakPowerPLR = 0;
	LcdData.PeakPowerMLR = 0;

	//send limit value if individual checkbox is checked.
	if(ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_TIMEPLR)
		LcdData.TimePLR = ParameterSet[CurrPreset].LimitsParam.WeldTimePLR;
	if(ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_TIMEMLR)
		LcdData.TimeMLR = ParameterSet[CurrPreset].LimitsParam.WeldTimeMLR;
	if(ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENERGYPLR)
		LcdData.EnergyPLR = ParameterSet[CurrPreset].LimitsParam.EnergyPLR;
	if(ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENERGYMLR)
		LcdData.EnergyMLR = ParameterSet[CurrPreset].LimitsParam.EnergyMLR;
	if(ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_POWERPLR)
		LcdData.PeakPowerPLR = ParameterSet[CurrPreset].LimitsParam.PowerPLR;
	if(ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_POWERMLR)
		LcdData.PeakPowerMLR = ParameterSet[CurrPreset].LimitsParam.PowerMLR;

	////////
	LcdData.TimeModeValue = ParameterSet[CurrPreset].WcParam.WeldTimeCP;
	LcdData.EnergyModeValue = ParameterSet[CurrPreset].WcParam.EnergyCP;
	LcdData.PKPwrModeValue = ParameterSet[CurrPreset].WcParam.PeakPowerCP / 10;
	LcdData.GndDetectModeValue = ParameterSet[CurrPreset].WcParam.ScrubTime;
	LcdData.PresetNo = Sysconfig->RecalledPreset;

	LcdData.Reserved = SeekBeforeTest;
	LcdData.ProfiBusAddr = Sysconfig->PbAddress;
	switch(Sysconfig->FBCurrNetworkConfig){
		case configStatic:
			LcdData.EthernetIP1 = (Sysconfig->FBSystemIP.addr>> BITS_IN_THREE_BYTE) & 0xff;
			LcdData.EthernetIP2 = (Sysconfig->FBSystemIP.addr >> BITS_IN_TWO_BYTE)& 0xff;
			LcdData.EthernetIP3 = (Sysconfig->FBSystemIP.addr >> BITS_IN_A_BYTE)& 0xff;
			LcdData.EthernetIP4 = (Sysconfig->FBSystemIP.addr & 0xff);
			LcdData.GatewayEthIP1 = (Sysconfig->FBGw.addr>> BITS_IN_THREE_BYTE) & 0xff;
			LcdData.GatewayEthIP2 = (Sysconfig->FBGw.addr>> BITS_IN_TWO_BYTE) & 0xff;
			LcdData.GatewayEthIP3 = (Sysconfig->FBGw.addr>> BITS_IN_A_BYTE) & 0xff;
			LcdData.GatewayEthIP4 = (Sysconfig->FBGw.addr & 0xff);
			LcdData.SubNetEthIP1 = (Sysconfig->FBNetmask.addr>> BITS_IN_THREE_BYTE) & 0xff;
			LcdData.SubNetEthIP2 = (Sysconfig->FBNetmask.addr>> BITS_IN_TWO_BYTE) & 0xff;
			LcdData.SubNetEthIP3 = (Sysconfig->FBNetmask.addr>> BITS_IN_A_BYTE) & 0xff;
			LcdData.SubNetEthIP4 = (Sysconfig->FBNetmask.addr & 0xff);
			LcdData.DefNetInfo = LCD_ETHIPSTATIC;
		break;
		case configBootP:
			LcdData.EthernetIP1 = (EthIPConfig.IpAddr >> BITS_IN_THREE_BYTE) & 0xff;
			LcdData.EthernetIP2 = (EthIPConfig.IpAddr >> BITS_IN_TWO_BYTE)& 0xff;
			LcdData.EthernetIP3 = (EthIPConfig.IpAddr >> BITS_IN_A_BYTE)& 0xff;
			LcdData.EthernetIP4 = (EthIPConfig.IpAddr & 0xff);
			LcdData.GatewayEthIP1 = (EthIPConfig.GW >> BITS_IN_THREE_BYTE) & 0xff;
			LcdData.GatewayEthIP2 = (EthIPConfig.GW >> BITS_IN_TWO_BYTE) & 0xff;
			LcdData.GatewayEthIP3 = (EthIPConfig.GW >> BITS_IN_A_BYTE) & 0xff;
			LcdData.GatewayEthIP4 = (EthIPConfig.GW & 0xff);
			LcdData.SubNetEthIP1 = (EthIPConfig.NM >> BITS_IN_THREE_BYTE) & 0xff;
			LcdData.SubNetEthIP2 = (EthIPConfig.NM >> BITS_IN_TWO_BYTE) & 0xff;
			LcdData.SubNetEthIP3 = (EthIPConfig.NM>> BITS_IN_A_BYTE) & 0xff;
			LcdData.SubNetEthIP4 = (EthIPConfig.NM & 0xff);
			LcdData.DefNetInfo = LCD_ETHBOOTP;
			break;
		case configDhcpClient:
			LcdData.EthernetIP1 = (EthIPConfig.IpAddr >> BITS_IN_THREE_BYTE) & 0xff;
			LcdData.EthernetIP2 = (EthIPConfig.IpAddr >> BITS_IN_TWO_BYTE)& 0xff;
			LcdData.EthernetIP3 = (EthIPConfig.IpAddr >> BITS_IN_A_BYTE)& 0xff;
			LcdData.EthernetIP4 = (EthIPConfig.IpAddr & 0xff);
			LcdData.GatewayEthIP1 = (EthIPConfig.GW >> BITS_IN_THREE_BYTE) & 0xff;
			LcdData.GatewayEthIP2 = (EthIPConfig.GW >> BITS_IN_TWO_BYTE) & 0xff;
			LcdData.GatewayEthIP3 = (EthIPConfig.GW >> BITS_IN_A_BYTE) & 0xff;
			LcdData.GatewayEthIP4 = (EthIPConfig.GW & 0xff);
			LcdData.SubNetEthIP1 = (EthIPConfig.NM >> BITS_IN_THREE_BYTE) & 0xff;
			LcdData.SubNetEthIP2 = (EthIPConfig.NM >> BITS_IN_TWO_BYTE) & 0xff;
			LcdData.SubNetEthIP3 = (EthIPConfig.NM>> BITS_IN_A_BYTE) & 0xff;
			LcdData.SubNetEthIP4 = (EthIPConfig.NM & 0xff);
			LcdData.DefNetInfo = LCD_ETHIPCLIENT;
			break;
		default:
			LcdData.DefNetInfo = LCD_ETHIPSTATIC;
		break;
	}

	if (DebugLCD == TRUE) {
		printf("\n update lcd data ");
	}

}

/* void ReceiveCommandHandler::SetDefaultIPSetUp(void)
 *
 *  Purpose
 *    This function stores default IP configuration in FRAM.This function is called by SetAllColdStartParameter(),ProcessRegUpdateCommand() functions.
 *
 *  Entry Condition:
 *    None.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::SetDefaultIPSetUp(void) {
	SetDHCPServerIPDefaults();
	SetStaticIPDefaults();
	LcdData.IP1 = (Sysconfig->SystemIPStatic.addr >> BITS_IN_THREE_BYTE) & 0xff;//Sysconfig->dhcpServerIP.;
	LcdData.IP2 = (Sysconfig->SystemIPStatic.addr >> BITS_IN_TWO_BYTE) & 0xff;
	LcdData.IP3 = (Sysconfig->SystemIPStatic.addr >> BITS_IN_A_BYTE) & 0xff;
	LcdData.IP4 = (Sysconfig->SystemIPStatic.addr & 0xff);

	LcdData.SubNetMask1 = (Sysconfig->Netmask.addr >> BITS_IN_THREE_BYTE) & 0xff;
	LcdData.SubNetMask2 = (Sysconfig->Netmask.addr >> BITS_IN_TWO_BYTE) & 0xff;
	LcdData.SubNetMask3 = (Sysconfig->Netmask.addr >> BITS_IN_A_BYTE) & 0xff;
	LcdData.SubNetMask4 = (Sysconfig->Netmask.addr & 0xff);

	LcdData.GateWay1 = (Sysconfig->Gw.addr >> BITS_IN_THREE_BYTE) & 0xff;
	LcdData.GateWay2 = (Sysconfig->Gw.addr >> BITS_IN_TWO_BYTE) & 0xff;
	LcdData.GateWay3 = (Sysconfig->Gw.addr >> BITS_IN_A_BYTE) & 0xff;
	LcdData.GateWay4 = (Sysconfig->Gw.addr & 0xff);
}

/* void ReceiveCommandHandler::SetAllColdStartParameter(void)
 *
 *  Purpose
 *    This function stores default value to Current preset in FRAM.This function is called by TransmitPollingCommandReply().
 *
 *  Entry Condition:
 *    None.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::SetAllColdStartParameter(void) {
	SetDefaultIPSetUp();
	SetColdStartPresetValues();
	Log::WriteEventLog(EVENT_COLD_START, COMM_IFC_LCD);
}



/* void ReceiveCommandHandler::GetDefaultParameterValues(void)
 *
 *  Purpose
 *    This function assigns default value to LCD.This function is
 *    called by TransmitPollingCommandReply() function.
 *
 *  Entry Condition:
 *    None.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::GetDefaultParameterValues(void)
{
	LcdData.SystemTypeValue = 0;
	LcdData.SystemTypeValue = MacchipData.CurrSystemType + OFFSET;
	if((MacchipData.CurrSystemType == SYSTEM_FIELDBUS) || (MacchipData.CurrSystemType == SYSTEM_FIELDBUS_HD)){
	    LcdData.SystemTypeValue |= ((FieldBusType & 0xFF) << 8);
	}

	LcdData.SeekRampTime
			= HsParameterSet[HsPreset_Default].ParaSeek.SeekRampTime;
	LcdData.WeldRampTime = ParameterSet[Preset_Default].DcpParam.WeldRampTime;
	LcdData.TimedSeekFlag = ParameterSet[Preset_Default].DcpParam.TimedSeekFlag;
	LcdData.SeekTime = HsParameterSet[HsPreset_Default].ParaSeek.SeekTime;
	LcdData.Amplitude = ParameterSet[Preset_Default].WcParam.Amplitude1;
	LcdData.MemoryStoreFlag
			= ParameterSet[Preset_Default].DcpParam.MemoryStoreFlag;
	LcdData.FreqOffsetValue = (ParameterSet[Preset_Default].WcParam.FreqOffset
			/ FREQ_SCALING_FACTOR);
	LcdData.CutOffs = ParameterSet[Preset_Default].WcParam.CutOffEnabled;
	LcdData.Limits = ParameterSet[Preset_Default].LimitsParam.ControlLimits
			& CONTROL_LIMITS_ENABLE;
	LcdData.WeldMode = ParameterSet[Preset_Default].WcParam.WeldMode;
	//Limit Registers
	LcdData.TimePLR = ParameterSet[Preset_Default].LimitsParam.WeldTimePLR;
	LcdData.TimeMLR = ParameterSet[Preset_Default].LimitsParam.WeldTimeMLR;
	LcdData.EnergyPLR = ParameterSet[Preset_Default].LimitsParam.EnergyPLR;
	LcdData.EnergyMLR = ParameterSet[Preset_Default].LimitsParam.EnergyMLR;
	LcdData.PeakPowerPLR = ParameterSet[Preset_Default].LimitsParam.PowerPLR;
	LcdData.PeakPowerMLR = ParameterSet[Preset_Default].LimitsParam.PowerMLR;
	////
	LcdData.TimeModeValue = ParameterSet[Preset_Default].WcParam.WeldTimeCP;
	LcdData.EnergyModeValue = ParameterSet[Preset_Default].WcParam.EnergyCP;
	LcdData.PKPwrModeValue = ParameterSet[Preset_Default].WcParam.PeakPowerCP
			/ 10;
	LcdData.GndDetectModeValue = ParameterSet[Preset_Default].WcParam.ScrubTime;
	LcdData.FreqOffsetFlag
			= ParameterSet[Preset_Default].WcParam.FreqOffsetFlag;
	LcdData.AmpStepCondition
			= ParameterSet[Preset_Default].WcParam.AmpStepCondition;
	LcdData.PowerupOption = POWERUP_SEEK;
	LcdData.IPSetUp = (UINT16) configStatic;
	LcdData.SubNetMask1 = DEFSUBNETMASK1;
	LcdData.SubNetMask2 = DEFSUBNETMASK2;
	LcdData.SubNetMask3 = DEFSUBNETMASK3;
	LcdData.SubNetMask4 = DEFSUBNETMASK4;
	LcdData.GateWay1 = DEFGATEWAYSTATIC1;
	LcdData.GateWay2 = DEFGATEWAYSTATIC2;
	LcdData.GateWay3 = DEFGATEWAYSTATIC3;
	LcdData.GateWay4 = DEFGATEWAYSTATIC4;
	LcdData.IP1 = DEFAULTIPSTATIC1;
	LcdData.IP2 = DEFAULTIPSTATIC2;
	LcdData.IP3 = DEFAULTIPSTATIC3;
	LcdData.IP4 = DEFAULTIPSTATIC4;
	LcdData.ProfiBusAddr = PB_DEFAULT_ADDR;
	LcdData.EthernetIP1 = DEFAULTIPSTATIC1;
	LcdData.EthernetIP2 = DEFAULTIPSTATIC2;
	LcdData.EthernetIP3 = DEFAULTIPSTATIC3;
	LcdData.EthernetIP4 = DEFAULTIPSTATIC4;
	LcdData.GatewayEthIP1 = DEFAULTIPSTATIC1;
	LcdData.GatewayEthIP2 = DEFAULTIPSTATIC2;
	LcdData.GatewayEthIP3 = DEFAULTIPSTATIC3;
	LcdData.GatewayEthIP4 = DEFAULTIPSTATIC4;
	LcdData.SubNetEthIP1 = DEFSUBNETMASK1;
	LcdData.SubNetEthIP2 = DEFSUBNETMASK2;
	LcdData.SubNetEthIP3 = DEFSUBNETMASK3;
	LcdData.SubNetEthIP4 = DEFSUBNETMASK4;
	LcdData.DefNetInfo = LCD_ETHIPSTATIC;
	LcdData.PresetNo = 0;
}

/* void ReceiveCommandHandler::GetMinParameterValues(void)
 *
 *  Purpose
 *    This function assigns Minimum allowed value to LCD.This function is called by TransmitPollingCommandReply() function.
 *
 *  Entry Condition:
 *    None.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::GetMinParameterValues(void) {
	UINT32 MinPSNum, MinHSPSNum;
	// Take Min values from Min Preset or from MinTweak preset based upon the Tweak flag.
	if (Sysconfig->TweakRangeEnabled) {
		MinPSNum = Preset_TweakMIN;
		MinHSPSNum = HsPreset_TweakMIN;
	} else {
		MinPSNum = Preset_MIN;
		MinHSPSNum = HsPreset_MIN;
	}
	LcdData.SeekRampTime = HsParameterSet[MinHSPSNum].ParaSeek.SeekRampTime;
	LcdData.SeekTime = HsParameterSet[MinHSPSNum].ParaSeek.SeekTime;
	LcdData.WeldRampTime = ParameterSet[MinPSNum].DcpParam.WeldRampTime;
	LcdData.Amplitude = ParameterSet[MinPSNum].WcParam.Amplitude1;
	LcdData.FreqOffsetValue = (ParameterSet[MinPSNum].WcParam.FreqOffset
			/ FREQ_SCALING_FACTOR);
	LcdData.CutOffs = ParameterSet[MinPSNum].WcParam.CutOffEnabled;
	LcdData.Limits = ParameterSet[MinPSNum].LimitsParam.ControlLimits;
	//Limit Registers
	LcdData.TimePLR = ParameterSet[MinPSNum].LimitsParam.WeldTimePLR;
	LcdData.TimeMLR = ParameterSet[MinPSNum].LimitsParam.WeldTimeMLR;
	LcdData.EnergyPLR = ParameterSet[MinPSNum].LimitsParam.EnergyPLR;
	LcdData.EnergyMLR = ParameterSet[MinPSNum].LimitsParam.EnergyMLR;
	LcdData.PeakPowerPLR = ParameterSet[MinPSNum].LimitsParam.PowerPLR;
	LcdData.PeakPowerMLR = ParameterSet[MinPSNum].LimitsParam.PowerMLR;
	////
	LcdData.TimeModeValue = ParameterSet[MinPSNum].WcParam.WeldTimeCP;
	LcdData.EnergyModeValue = ParameterSet[MinPSNum].WcParam.EnergyCP;
	LcdData.PKPwrModeValue = ParameterSet[MinPSNum].WcParam.PeakPowerCP / 10;
	LcdData.GndDetectModeValue = ParameterSet[MinPSNum].WcParam.ScrubTime;
	LcdData.WeldMode = ParameterSet[MinPSNum].WcParam.WeldMode;
	LcdData.FreqOffsetFlag
	         = ParameterSet[MinPSNum].WcParam.FreqOffsetFlag;
	LcdData.MemoryStoreFlag = ParameterSet[MinPSNum].DcpParam.MemoryStoreFlag;
	LcdData.ProfiBusAddr = PB_MIN_ADDR;
	LcdData.PresetNo = 1;
}

/* void ReceiveCommandHandler::GetDefaultParameterValues(void)
 *
 *  Purpose
 *    This function assigns maximum allowed value to LCD.This function is called by TransmitPollingCommandReply() function.
 *
 *  Entry Condition:
 *    None.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::GetMaxParameterValues(void) {
	SINT32 MaxPSNum, MaxHSPSNum;
	// Take Max values from Max Preset or from MaxTweak preset based upon the Tweak flag.
	if (Sysconfig->TweakRangeEnabled) {
		MaxPSNum = Preset_TweakMAX;
		MaxHSPSNum = HsPreset_TweakMAX;
	} else {
		MaxPSNum = Preset_MAX;
		MaxHSPSNum = HsPreset_MAX;
	}
	LcdData.SeekRampTime = HsParameterSet[MaxHSPSNum].ParaSeek.SeekRampTime;
	LcdData.SeekTime = HsParameterSet[MaxHSPSNum].ParaSeek.SeekTime;
	LcdData.WeldRampTime = ParameterSet[MaxPSNum].DcpParam.WeldRampTime;
	LcdData.Amplitude = ParameterSet[MaxPSNum].WcParam.Amplitude1;
	LcdData.FreqOffsetValue = (ParameterSet[MaxPSNum].WcParam.FreqOffset
			/ FREQ_SCALING_FACTOR);
	LcdData.CutOffs = ParameterSet[MaxPSNum].WcParam.CutOffEnabled;
	LcdData.Limits = (ParameterSet[MaxPSNum].LimitsParam.ControlLimits
			& CONTROL_LIMITS_ENABLE);
	//Limit Registers
	LcdData.TimePLR = ParameterSet[MaxPSNum].LimitsParam.WeldTimePLR;
	LcdData.TimeMLR = ParameterSet[MaxPSNum].LimitsParam.WeldTimeMLR;
	LcdData.EnergyPLR = ParameterSet[MaxPSNum].LimitsParam.EnergyPLR;
	LcdData.EnergyMLR = ParameterSet[MaxPSNum].LimitsParam.EnergyMLR;
	LcdData.PeakPowerPLR = ParameterSet[MaxPSNum].LimitsParam.PowerPLR;
	LcdData.PeakPowerMLR = ParameterSet[MaxPSNum].LimitsParam.PowerMLR;
	////
	LcdData.TimeModeValue = ParameterSet[MaxPSNum].WcParam.WeldTimeCP;
	LcdData.EnergyModeValue = ParameterSet[MaxPSNum].WcParam.EnergyCP;
	LcdData.PKPwrModeValue = ParameterSet[MaxPSNum].WcParam.PeakPowerCP / 10;
	LcdData.GndDetectModeValue = ParameterSet[MaxPSNum].WcParam.ScrubTime;
	LcdData.WeldMode = ParameterSet[MaxPSNum].WcParam.WeldMode;
	LcdData.FreqOffsetFlag
	            = ParameterSet[MaxPSNum].WcParam.FreqOffsetFlag;
	LcdData.MemoryStoreFlag = ParameterSet[MaxPSNum].DcpParam.MemoryStoreFlag;
	LcdData.ProfiBusAddr = PB_MAX_ADDR;
	LcdData.PresetNo = MAXIMUM_USER_PRESETS;
}

/* void ReceiveCommandHandler::ProcessRegUpdateCommand(enum DataReg dataReg, UINT32 value)
 *
 *  Purpose
 *    This function is responsible for updating Current preset according to data Reg number received in LCD command.This function is called by ProcessCommand() function.
 *
 *  Entry Condition:
 *    dataReg - data Reg number
 *    value - value to be updated.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::ProcessRegUpdateCommand(enum DataReg dataReg,
		UINT32 value) {
	UINT16 TempVar = 0;
	switch (dataReg) {
	case REG_SEEK_RAMPTIME:
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekRampTime, value, ParaSeekSeekRampTime, value, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_HSPS);
		break;
	case REG_WELD_RAMPTIME:
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldRampTime, value, DcpParamWeldRampTime, value, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_DCP);
		break;
	case REG_TIMEDSEEK:
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, TimedSeekFlag, value, DcpParamTimedSeekFlag, value, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_DCP);
		break;
	case REG_SEEKTIME:
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekTime, value, ParaSeekSeekTime, value, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_HSPS);
		break;
	case REG_AMPLITUDE:
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, Amplitude1, value, WcParamAmplitude1, value, COMM_IFC_LCD)
		;
		RunningPreset.WcParam.Amplitude1
				= ParameterSet[CurrPreset].WcParam.Amplitude1;
		RtSendData.CmdFlags |= MB_USEREALTIMEDATA;
		RtSendData.AmplitudeIn = RunningPreset.WcParam.Amplitude1;
		CalculateAndSavePresetCRC(SAVECRC_WC);
		break;
	case REG_SOFTWARE_VERSION:
		LCDSWVersion = value;
		break;
	case REG_IP1:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->SystemIP.addr = ((Sysconfig->SystemIP.addr & 0x00FFFFFF)
					| (value << BITS_IN_THREE_BYTE));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->SystemIPStatic.addr = ((Sysconfig->SystemIPStatic.addr
					& 0x00FFFFFF) | (value << BITS_IN_THREE_BYTE));
		break;
	case REG_IP2:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->SystemIP.addr = ((Sysconfig->SystemIP.addr & 0xFF00FFFF)
					| (value << BITS_IN_TWO_BYTE));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->SystemIPStatic.addr = ((Sysconfig->SystemIPStatic.addr
					& 0xFF00FFFF) | (value << BITS_IN_TWO_BYTE));
		break;
	case REG_IP3:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->SystemIP.addr = ((Sysconfig->SystemIP.addr & 0xFFFF00FF)
					| (value << BITS_IN_A_BYTE));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->SystemIPStatic.addr = ((Sysconfig->SystemIPStatic.addr
					& 0xFFFF00FF) | (value << BITS_IN_A_BYTE));
		break;
	case REG_IP4:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->SystemIP.addr = ((Sysconfig->SystemIP.addr & 0xFFFFFF00)
					| (value));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->SystemIPStatic.addr = ((Sysconfig->SystemIPStatic.addr
					& 0xFFFFFF00) | (value));
		break;
	case REG_IPSETUP:
		Sysconfig->CurrNetworkConfig = (NetworkConfiguration) value;
		CyGOS::WriteSystemEvent(EVENT_IPCONFIG_CHANGED, __LINE__,Sysconfig->CurrNetworkConfig, (('R' << 24) | ('e' << 16) | ('c' << 8) | ('v')), true);
		if (Sysconfig->CurrNetworkConfig == configDefault)
			SetDefaultIPSetUp();
		RegUpdateCmdFlag = TRUE;
		break;
	case REG_EXAMPLITUDE:
		if (value == TRUE) {
			CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, AmpStepCondition, AMPSTEP_COND_EXTERNALAIN, WcParamAmpStepCondition, AMPSTEP_COND_EXTERNALAIN, COMM_IFC_LCD);
		} else {
			CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, AmpStepCondition, AMPSTEP_COND_OFF, WcParamAmpStepCondition, AMPSTEP_COND_OFF, COMM_IFC_LCD);
		}
		CalculateAndSavePresetCRC(SAVECRC_WC);
		break;
	case REG_MEMSTORE:
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, MemoryStoreFlag, value, DcpParamMemoryStoreFlag, value, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_DCP);
		break;
	case REG_PWRUP_SEEKSCAN:
		Sysconfig->PowerupOption = (UINT16) value;
		break;
	case REG_CODECRC_LOW:
		LCDCodeCRCLow = value & LOWBYTE;
		break;
	case REG_CODECRC_HIGH:
		LCDCodeCRCHigh = value & LOWBYTE;
		SystemInfoReceived = TRUE;
		break;
	case REG_SUBNETMASK1:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->DhcpConfig.netmask.addr
					= ((Sysconfig->DhcpConfig.netmask.addr & 0x00FFFFFF)
							| (value << BITS_IN_THREE_BYTE));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->Netmask.addr = ((Sysconfig->Netmask.addr & 0x00FFFFFF)
					| (value << BITS_IN_THREE_BYTE));
		break;
	case REG_SUBNETMASK2:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->DhcpConfig.netmask.addr
					= ((Sysconfig->DhcpConfig.netmask.addr & 0xFF00FFFF)
							| (value << BITS_IN_TWO_BYTE));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->Netmask.addr = ((Sysconfig->Netmask.addr & 0xFF00FFFF)
					| (value << BITS_IN_TWO_BYTE));
		break;
	case REG_SUBNETMASK3:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->DhcpConfig.netmask.addr
					= ((Sysconfig->DhcpConfig.netmask.addr & 0xFFFF00FF)
							| (value << BITS_IN_A_BYTE));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->Netmask.addr = ((Sysconfig->Netmask.addr & 0xFFFF00FF)
					| (value << BITS_IN_A_BYTE));
		break;
	case REG_SUBNETMASK4:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->DhcpConfig.netmask.addr
					= ((Sysconfig->DhcpConfig.netmask.addr & 0xFFFFFF00)
							| (value));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->Netmask.addr = ((Sysconfig->Netmask.addr & 0xFFFFFF00)
					| (value));
		break;
	case REG_GATEWAY1:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->DhcpConfig.gateway.addr
					= ((Sysconfig->DhcpConfig.gateway.addr & 0x00FFFFFF)
							| (value << BITS_IN_THREE_BYTE));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->Gw.addr = ((Sysconfig->Gw.addr & 0x00FFFFFF) | (value
					<< BITS_IN_THREE_BYTE));
		break;
	case REG_GATEWAY2:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->DhcpConfig.gateway.addr
					= ((Sysconfig->DhcpConfig.gateway.addr & 0xFF00FFFF)
							| (value << BITS_IN_TWO_BYTE));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->Gw.addr = ((Sysconfig->Gw.addr & 0xFF00FFFF) | (value
					<< BITS_IN_TWO_BYTE));
		break;
	case REG_GATEWAY3:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->DhcpConfig.gateway.addr
					= ((Sysconfig->DhcpConfig.gateway.addr & 0xFFFF00FF)
							| (value << BITS_IN_A_BYTE));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->Gw.addr = ((Sysconfig->Gw.addr & 0xFFFF00FF) | (value
					<< BITS_IN_A_BYTE));
		break;
	case REG_GATEWAY4:
		if (Sysconfig->CurrNetworkConfig == configDhcpServer)
			Sysconfig->DhcpConfig.gateway.addr
					= ((Sysconfig->DhcpConfig.gateway.addr & 0xFFFFFF00)
							| (value));
		else if (Sysconfig->CurrNetworkConfig == configStatic)
			Sysconfig->Gw.addr = ((Sysconfig->Gw.addr & 0xFFFFFF00) | (value));
		break;
	case REG_EXTFREQ:
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, FreqOffsetFlag, value, WcParamFreqOffsetFlag, value, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_WC);
		break;
	case REG_FREQOFFSET:
		LcdData.FreqOffsetValue = value;
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, FreqOffset , (SINT32)(LcdData.FreqOffsetValue * FREQ_SCALING_FACTOR), WcParamFreqOffset, LcdData.FreqOffsetValue, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_WC);
		break;

	case REG_CUTOFFS:
		LcdData.CutOffs = value;
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, CutOffEnabled, value, WcParamCutOffEnabled, value, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_WC);
		break;
	case REG_LIMITS:
		LcdData.Limits = value;
		if(value)
			value = (ParameterSet[CurrPreset].LimitsParam.ControlLimits | CONTROL_LIMITS_ENABLE);
		else
			value = (ParameterSet[CurrPreset].LimitsParam.ControlLimits & ~CONTROL_LIMITS_ENABLE);
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, ControlLimits, value, LimitsParamControlLimits, value, COMM_IFC_LCD);
		CalculateAndSavePresetCRC(SAVECRC_LIMITS);
		break;
	case REG_PROFIBUS_ADDR:
			Sysconfig->PbAddress = (value & 0x000000FF);
		break;
	case REG_ETHIP1:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBSystemIP.addr = ((Sysconfig->FBSystemIP.addr
					& 0x00FFFFFF) | (value << BITS_IN_THREE_BYTE));
		break;
	case REG_ETHIP2:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBSystemIP.addr = ((Sysconfig->FBSystemIP.addr
					& 0xFF00FFFF) | (value << BITS_IN_TWO_BYTE));
		break;
	case REG_ETHIP3:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBSystemIP.addr = ((Sysconfig->FBSystemIP.addr
					& 0xFFFF00FF) | (value << BITS_IN_A_BYTE));
		break;
	case REG_ETHIP4:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBSystemIP.addr = ((Sysconfig->FBSystemIP.addr
					& 0xFFFFFF00) | (value));
		break;
	case REG_ETH_GATEWAY_1:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBGw.addr = ((Sysconfig->FBGw.addr
				& 0x00FFFFFF) | (value << BITS_IN_THREE_BYTE));
		break;
	case REG_ETH_GATEWAY_2:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBGw.addr = ((Sysconfig->FBGw.addr & 0xFF00FFFF) | (value
					<< BITS_IN_TWO_BYTE));
		break;
	case REG_ETH_GATEWAY_3:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBGw.addr = ((Sysconfig->FBGw.addr & 0xFFFF00FF) | (value
				<< BITS_IN_A_BYTE));
		break;
	case REG_ETH_GATEWAY_4:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBGw.addr = ((Sysconfig->FBGw.addr & 0xFFFFFF00) | (value));
		break;
	case REG_SUBNET_ETH_1:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
					Sysconfig->FBNetmask.addr = ((Sysconfig->FBNetmask.addr
						& 0x00FFFFFF) | (value << BITS_IN_THREE_BYTE));
		break;
	case REG_SUBNET_ETH_2:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBNetmask.addr = ((Sysconfig->FBNetmask.addr & 0xFF00FFFF) | (value
					<< BITS_IN_TWO_BYTE));
		break;
	case REG_SUBNET_ETH_3:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBNetmask.addr = ((Sysconfig->FBNetmask.addr & 0xFFFF00FF) | (value
				<< BITS_IN_A_BYTE));
		break;
	case REG_SUBNET_ETH_4:
		if (Sysconfig->FBCurrNetworkConfig == configStatic)
			Sysconfig->FBNetmask.addr = ((Sysconfig->FBNetmask.addr & 0xFFFFFF00) | (value));
		break;

	case REG_DEFNET_INFO:
		switch(value){
			case LCD_ETHIPSTATIC:
				Sysconfig->FBCurrNetworkConfig = configStatic;
			break;
			case LCD_ETHIPCLIENT:
				Sysconfig->FBCurrNetworkConfig = configDhcpClient;
			break;
			case LCD_ETHBOOTP:
				Sysconfig->FBCurrNetworkConfig = configBootP;
			break;
			case LCD_ETHDEF:
				SetFBStaticIPDefaults();
				LcdData.EthernetIP1 = (Sysconfig->FBSystemIP.addr >> BITS_IN_THREE_BYTE) & 0xff;//Sysconfig->dhcpServerIP.;
				LcdData.EthernetIP2 = (Sysconfig->FBSystemIP.addr >> BITS_IN_TWO_BYTE) & 0xff;
				LcdData.EthernetIP3 = (Sysconfig->FBSystemIP.addr >> BITS_IN_A_BYTE) & 0xff;
				LcdData.EthernetIP4 = (Sysconfig->FBSystemIP.addr & 0xff);

				LcdData.SubNetEthIP1 = (Sysconfig->FBNetmask.addr >> BITS_IN_THREE_BYTE) & 0xff;
				LcdData.SubNetEthIP2 = (Sysconfig->FBNetmask.addr >> BITS_IN_TWO_BYTE) & 0xff;
				LcdData.SubNetEthIP3 = (Sysconfig->FBNetmask.addr >> BITS_IN_A_BYTE) & 0xff;
				LcdData.SubNetEthIP4 = (Sysconfig->FBNetmask.addr & 0xff);

				LcdData.GatewayEthIP1 = (Sysconfig->FBGw.addr >> BITS_IN_THREE_BYTE) & 0xff;
				LcdData.GatewayEthIP2 = (Sysconfig->FBGw.addr >> BITS_IN_TWO_BYTE) & 0xff;
				LcdData.GatewayEthIP3 = (Sysconfig->FBGw.addr >> BITS_IN_A_BYTE) & 0xff;
				LcdData.GatewayEthIP4 = (Sysconfig->FBGw.addr & 0xff);
				LcdData.DefNetInfo = LCD_ETHIPSTATIC;
			break;
			default:
			break;
		}
		RegUpdateCmdFlag = TRUE;
		break;
	case REG_RESERVED: // = 137,
		break;

	case REG_WELDMODE:
		LcdData.WeldMode = value;
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, WeldMode, (WELD_MODE)value, WcParamWeldMode, (WELD_MODE)value, COMM_IFC_LCD);
		CalculateAndSavePresetCRC(SAVECRC_WC);
		break;
	//Limit REgisters
	case REG_TIME_PLR:
		LcdData.TimePLR = value;
		if(value){
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits | (CONTROL_LIMITS_ENABLE| CONTROL_LIMITS_TIMEPLR));
			CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, WeldTimePLR, value, LimitsParamWeldTimePLR, value, COMM_IFC_LCD);
		}
		else{
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits & ~CONTROL_LIMITS_TIMEPLR);
			//If all limits are Unchecked, Turn Off Global limit on LCD (When 114 is set from LCD but all limits are disabled
			if((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLEALL) == (CONTROL_LIMITS_ENABLE|CONTROL_LIMITS_TIMEPLR))
				TempVar = 0;
		}
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, ControlLimits, TempVar, LimitsParamControlLimits, TempVar, COMM_IFC_LCD);
		CalculateAndSavePresetCRC(SAVECRC_LIMITS);

		break;
	case REG_TIME_MLR:
		LcdData.TimeMLR = value;
		if(value){
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits | (CONTROL_LIMITS_ENABLE| CONTROL_LIMITS_TIMEMLR));
			CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, WeldTimeMLR, value, LimitsParamWeldTimeMLR, value, COMM_IFC_LCD);
		}
		else{
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits & ~CONTROL_LIMITS_TIMEMLR);
			//If all limits are Unchecked, Turn Off Global limit on LCD (When 114 is set from LCD but all limits are disabled
			if((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLEALL) == (CONTROL_LIMITS_ENABLE|CONTROL_LIMITS_TIMEMLR))
				TempVar = 0;
		}
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, ControlLimits, TempVar, LimitsParamControlLimits, TempVar, COMM_IFC_LCD);
		CalculateAndSavePresetCRC(SAVECRC_LIMITS);

		break;
	case REG_ENERGY_PLR:
		LcdData.EnergyPLR = value;
		if(value){
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits | (CONTROL_LIMITS_ENABLE|CONTROL_LIMITS_ENERGYPLR));
			CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, EnergyPLR, value, LimitsParamEnergyPLR, value, COMM_IFC_LCD);
		}
		else{
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits & ~CONTROL_LIMITS_ENERGYPLR);
			//If all limits are Unchecked, Turn Off Global limit on LCD (When 114 is set from LCD but all limits are disabled
			if((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLEALL) == (CONTROL_LIMITS_ENABLE|CONTROL_LIMITS_ENERGYPLR))
				TempVar = 0;
		}
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, ControlLimits, TempVar, LimitsParamControlLimits, TempVar, COMM_IFC_LCD);
		CalculateAndSavePresetCRC(SAVECRC_LIMITS);
		break;
	case REG_ENERGY_MLR:
		LcdData.EnergyMLR = value;
		if(value){
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits | (CONTROL_LIMITS_ENABLE|CONTROL_LIMITS_ENERGYMLR));
			CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, EnergyMLR, value, LimitsParamEnergyMLR, value, COMM_IFC_LCD);
		}
		else{
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits & ~CONTROL_LIMITS_ENERGYMLR);
			//If all limits are Unchecked, Turn Off Global limit on LCD (When 114 is set from LCD but all limits are disabled
			if((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLEALL) == (CONTROL_LIMITS_ENABLE|CONTROL_LIMITS_ENERGYMLR))
				TempVar = 0;
		}
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, ControlLimits, TempVar, LimitsParamControlLimits, TempVar, COMM_IFC_LCD);
		CalculateAndSavePresetCRC(SAVECRC_LIMITS);
		break;
	case REG_PEAKPOWER_PLR:
		LcdData.PeakPowerPLR = value;
		if(value){
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits | (CONTROL_LIMITS_ENABLE|CONTROL_LIMITS_POWERPLR));
			CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, PowerPLR, value, LimitsParamPowerPLR, value, COMM_IFC_LCD);
		}
		else{
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits & ~CONTROL_LIMITS_POWERPLR);
			//If all limits are Unchecked, Turn Off Global limit on LCD (When 114 is set from LCD but all limits are disabled
			if((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLEALL) == (CONTROL_LIMITS_ENABLE|CONTROL_LIMITS_POWERPLR))
				TempVar = 0;
		}
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, ControlLimits, TempVar, LimitsParamControlLimits, TempVar, COMM_IFC_LCD);
		CalculateAndSavePresetCRC(SAVECRC_LIMITS);
		break;
	case REG_PEAKPOWER_MLR:
		LcdData.PeakPowerMLR = value;
		if(value){
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits | (CONTROL_LIMITS_ENABLE|CONTROL_LIMITS_POWERMLR));
			CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, PowerMLR, value, LimitsParamPowerMLR, value, COMM_IFC_LCD);
		}
		else{
			TempVar = (ParameterSet[CurrPreset].LimitsParam.ControlLimits & ~CONTROL_LIMITS_POWERMLR);
			//If all limits are Unchecked, Turn Off Global limit on LCD (When 114 is set from LCD but all limits are disabled
			if((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLEALL) == (CONTROL_LIMITS_ENABLE|CONTROL_LIMITS_POWERMLR))
				TempVar = 0;
		}
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, ControlLimits, TempVar, LimitsParamControlLimits, TempVar, COMM_IFC_LCD);
		CalculateAndSavePresetCRC(SAVECRC_LIMITS);
		break;
	/////////
	case REG_TIMEMODE_VAL:
		LcdData.TimeModeValue = value;
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, WeldTimeCP, value, WcParamWeldTimeCP, value, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_WC);
		break;
	case REG_ENERGYMODE_VAL:// = 909,
		LcdData.EnergyModeValue = value;
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, EnergyCP, (UINT32)value, WcParamEnergyCP, value, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_WC);
		break;
	case REG_PKPWRMODE_VAL:// = 910,
		LcdData.PKPwrModeValue = value;
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, PeakPowerCP, (value * 10), WcParamPeakPowerCP, (value * 10), COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_WC);
		break;
	case REG_GNDDETECTMODE_VAL:
		LcdData.GndDetectModeValue = value;
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, ScrubTime, value, WcParamScrubTime, value, COMM_IFC_LCD)
		;
		CalculateAndSavePresetCRC(SAVECRC_WC);
		break;
	case REG_PRESET_NUM:
		LcdData.PresetNo = (value & 0xFF);
		if(LcdData.PresetNo > 0){
			if(SavePreset((PRESET_NUMBER)LcdData.PresetNo , (HSPRESET_NUMBER)HORNPRESETNUMBER_DEFAULT))
				Log::WriteEventLog(EVENT_PARAMETER_CHANGED, ((LcdData.PresetNo << 8) | 0xFF));
		}
		pprintf("\n Preset No = %d;  set= %d", LcdData.PresetNo,  Sysconfig->ActivePreset);
		break;
	default:
		break;
	}

	RegUpdateCmdFlag = TRUE;
	if (DebugLCD == TRUE) {
//		printf(" Reg = %d val = %d \n", dataReg, value);
	}
}

BOOLEAN ReceiveCommandHandler::CheckIfChanged(int Indx)
{
	BOOLEAN Changed = FALSE;
	for (int j = 0; (j < SerialCmdReg[Indx].DataLen) && (Changed == FALSE); j++)
	{
	  if(((UINT8*) SerialCmdReg[Indx].DataLast)[j] != ((UINT8*) SerialCmdReg[Indx].Data)[j])
		  Changed = TRUE;
	}
	return Changed;
}

void UpdateMinMaxLCDPreset(PSUPDATESPECIAL PsUpdateSpcl) {
	if ((PsUpdateSpcl & UPDATEMIN) == UPDATEMIN){
		RegMinUpdateFlag = TRUE;
	}
	if ((PsUpdateSpcl & UPDATEMAX) == UPDATEMAX){
		RegMaxUpdateFlag = TRUE;
	}
}

/* void ReceiveCommandHandler::Tick(void)
 *
 *  Purpose
 *    This is the tick routine of ReceiveCommandHandler.
 *
 *  Entry Condition:
 *    None.
 *
 *  Exit Condition:
 *    None.
 */
void ReceiveCommandHandler::Tick(void) {
	static UINT32 usCount = 0;
	static UINT32 usCountfor1Sec = 0;
	static BOOLEAN SwitchMultipleAlarmOnLCD = FALSE;
	if (AlarmIndex > 1) {
		usCountfor1Sec += usecPerTick;
		if (usCountfor1Sec > (TIMEOUT_1SECOND)) {
			usCountfor1Sec = 0;
			if (SwitchMultipleAlarmOnLCD) {
				SwitchMultipleAlarmOnLCD = FALSE;
				CurrentAlarm = AlarmsArray[1];
			} else {
				SwitchMultipleAlarmOnLCD = TRUE;
				CurrentAlarm = AlarmsArray[0];
			}
		}
	} else {
		if (AlarmIndex != 0)
			CurrentAlarm = AlarmsArray[0];
		usCountfor1Sec = 0;
		SwitchMultipleAlarmOnLCD = FALSE;
	}
	if (CmdReceived == FALSE) {
		usCount += usecPerTick;
		if (usCount >= KEYPRESSEDTIMEOUT) {
			LCDKeyStatus = 0;
			ControlVariable &= ~(LCDTEST | LCDRESET);
		}
		if (usCount >= LCDCOMTIMEOUT) {
			//log entry should be done on the reply of next polling
			LcdCommunicationLost = TRUE;
			//LCD should be given the fresh set of all Register values
			RegUpdateCmdFlag = TRUE;
			usCount = 0;
		}
	} else {
		usCount = 0;
		CmdReceived = FALSE;
	}
	if ((CurrNWConfig == configDhcpClient) && (OldLinkStat != NetIf->up)) {
		OldLinkStat = NetIf->up;
		RegUpdateCmdFlag = TRUE;
	}
}



/*  void ConfigureResetLineForLCDWC991(void)
 *
 *  Purpose:
 *    To configure CPU pin(G1) connected to LCD reset as output.
 *
 *  Entry Condition:
 * 	Called once at power up when ReceiveCommandHandle task object is created.
 *
 *  Exit Condition:
 * 	G1 pin is configured as general purpose output
*/
void ConfigureResetLineForLCDWC991(void)
{
   MCF_PAD_PAR_TIMER &= ~(MCF_PAD_PAR_TIMER_PAR_T0IN(
         MCF_PAD_PAR_TIMER_PAR_T0IN_T0IN));
   MCF_GPIO_PDDR_TIMER |= MCF_GPIO_PDDR_TIMER_PDDR_TIMER0;
   MCF_GPIO_PODR_TIMER |= MCF_GPIO_PODR_TIMER_PODR_TIMER0;
}



/*  void ConfigureResetLineForLCDWC1169(void)
 *
 *  Purpose:
 *    To configure CPU pin(G1) connected to LCD reset as output.
 *
 *  Entry Condition:
 *    Called once at power up when ReceiveCommandHandle task object is created.
 *
 *  Exit Condition:
 * 	G1 pin is configured as general purpose output
*/
void ConfigureResetLineForLCDWC1169(void)
{
   MCF_PAD_PAR_TIMER &= ~(MCF_PAD_PAR_TIMER_PAR_T0IN(
         MCF_PAD_PAR_TIMER_PAR_T0IN_T0IN));
   MCF_GPIO_PDDR_TIMER |= MCF_GPIO_PDDR_TIMER_PDDR_TIMER0;
   MCF_GPIO_PODR_TIMER |= MCF_GPIO_PODR_TIMER_PODR_TIMER0;
}



/*  void ConfigureResetLineForLCDSC1054(void)
 *
 *  Purpose:
 *    To configure CPU pin(AA16) connected to LCD reset as output
 *
 *  Entry Condition:
 * 	Called once at power up when ReceiveCommandHandle task object is created.
 *
 *  Exit Condition:
 * 	AA16 pin is configured as general purpose output.
*/
void ConfigureResetLineForLCDSC1054(void)
{
   //configure U0RTS pin as normal GPIO. This pin is connected to LCD_RESET lines.
   MCF_PAD_PAR_UART &= (~MCF_PAD_PAR_UART_PAR_U0RTS);
   //Change data direction out for reset line and drive high on reset
   MCF_GPIO_PDDR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
   MCF_GPIO_PODR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
}



/*  void SendResetToLCDWC991(void)
 *
 *  Purpose:
 *    This function drives CPU's pin G1 on WC991 board to send a reset pulse to the LCD board.
 *    It is called once from Run() function of ReceiveCommandHandler task.
 *
 *  Entry Condition:
 * 	Once at reset
 *
 *  Exit Condition:
 * 	Reset pulse sent to LCD Board to restart.
*/
void SendResetToLCDWC991(void)
{
   //Send Reset signal to LCD Board to restart.
   CyGOS::DelayMs(50);
   // Activate Reset Signal.(Active Low).
   MCF_GPIO_PODR_TIMER &= ~MCF_GPIO_PODR_TIMER_PODR_TIMER0;
   //wait for 50ms before Setting the reset signal to LCD.
   CyGOS::DelayMs(50);
   // Deactivate Reset Signal.(Active Low).
   MCF_GPIO_PODR_TIMER |= MCF_GPIO_PODR_TIMER_PODR_TIMER0;
}



/*  void SendResetToLCDWC1169(void)
 *
 *  Purpose:
 *    This function drives CPU's pin G1 on WC1169 board to send a reset pulse to the LCD board.
 *    It is called once from Run() function of ReceiveCommandHandler task.
 *
 *  Entry Condition:
 * 	Once at reset
 *
 *  Exit Condition:
 * 	Reset pulse sent to LCD Board to restart.
*/
void SendResetToLCDWC1169(void)
{
   //Send Reset signal to LCD Board to restart.
   CyGOS::DelayMs(50);
   // Activate Reset Signal.(Active Low).
   MCF_GPIO_PODR_TIMER &= ~MCF_GPIO_PODR_TIMER_PODR_TIMER0;
   //wait for 50ms before Setting the reset signal to LCD.
   CyGOS::DelayMs(50);
   // Deactivate Reset Signal.(Active Low).
   MCF_GPIO_PODR_TIMER |= MCF_GPIO_PODR_TIMER_PODR_TIMER0;
}



/*  void SendResetToLCDSC1054(void)
 *
 *  Purpose:
 *    This function drives CPU's pin AA16 on SC1054 board to send a reset pulse to the LCD board.
 *    It is called from Run() function of ReceiveCommandHandler task.
 *
 *  Entry Condition:
 * 	Once at reset
 *
 *  Exit Condition:
 * 	Reset pulse sent to LCD Board to restart.
*/
void SendResetToLCDSC1054(void)
{
   CyGOS::DelayMs(50);
   // Activate Reset Signal.(Active Low).
   MCF_GPIO_PODR_UART &= ~MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
   //wait for 50ms before Setting the reset signal to LCD.
   CyGOS::DelayMs(50);
   // Deactivate Reset Signal.(Active Low).
   MCF_GPIO_PODR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
}



