/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/ReceiveCommandHandler.cpp_v   1.21   Jan 11 2017 15:30:54   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/ReceiveCommandHandler.cpp_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Fixed issue Amplitude setting cannot be updated from LCD interface
 *
 *    Rev 1.21   Jan 11 2017 15:30:54   EGUAJARDO
 * Fixed issue related to register 115 about restoring default values.
 * 
 *    Rev 1.20   15 Sep 2016 16:56:04   EGUAJARDO
 * Added the condition to set lcdData.Weldmode to be continuous mode if Rack S is the system type.
 * 
 *    Rev 1.19   25 Jul 2016 15:10:56   GButron
 * Coding standard applied after code review
 * 
 *    Rev 1.18   06 Jun 2016 15:17:22   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.17   30 Oct 2015 08:52:40   sesharma
 * Changed for Handheld mode
 * 
 *    Rev 1.16   28 Sep 2015 06:46:52   RJamloki
 * Fix for IP, Subnet and gateway registers randomly getting set to wrong value
 * 
 *    Rev 1.15   25 Jun 2015 07:06:44   amaurya
 * Changed for logging Event on change of weld parameter and added register to switch between Time-Impulse and Maintained mode.
 * 
 *    Rev 1.14   13 May 2015 08:43:44   sesharma
 * Register 157(Memory clear) enabled for DC-STD; Reg 111 (External Offset) removed from DC-STD. If window low limit is disabled, window low limit segment does not blink on LCD.
 * 
 *    Rev 1.13   09 Apr 2015 05:16:40   sesharma
 * All references to Impulse Start removed. Window low limit segment will blink only when respective window low limit is enabled from webpage.
 * 
 *    Rev 1.12   19 Mar 2015 06:59:54   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.11   26 Feb 2015 08:28:48   sesharma
 * Changes done to handle window low limit alarm when it should be raised irrespective of its state i.e. enabled/disabled.
 * 
 *    Rev 1.10   18 Feb 2015 00:56:36   SaPawar
 * alarm bit addition to dcpstatus for peakpowerlowlimit handling at lcd side
 * 
 *    Rev 1.9   22 Jan 2015 05:02:14   sesharma
 * Done Corrections to set Window Limits from LCD.
 * 
 *    Rev 1.8   15 Jan 2015 01:53:28   sesharma
 * Latching Alarm is Enabled by Default & bug fixing for restore defaults from LCD.
 * 
 *    Rev 1.7   13 Jan 2015 06:01:36   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.6   27 Nov 2014 09:14:46   sesharma
 * Webpages and SIG related changes 
 * 
 *    Rev 1.5   19 Nov 2014 11:55:16   ewomack
 * Added back code to reset LCD board on powerup as pullup on reset line of LCD is not sufficient.
 * 
 *    Rev 1.4   07 Nov 2014 16:16:40   ewomack
 * Stop resetting LCD board on powerup.  Send error reason to LCD.
 * 
 *    Rev 1.3   16 Oct 2014 03:48:50   sesharma
 * Tracker Issues Resolved
 * 
 *    Rev 1.2   17 Sep 2014 08:57:58   amaurya
 * Added changes to impulse start register and code review
 * 
 *    Rev 1.1   03 Sep 2014 07:40:00   amaurya
 * Added SendLCDAdata() for LCD Code Download from website.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:28   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:02   amaurya
 * Initial revision.
 */

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
#include "WebSite.h"
#include "UtilsFT.h"
#include "CommandsFT.h"

#define CLEARSTRING                                 "\033[2J\033[2J\r"
#define CURSORHOMEPOSITION                          "\033[%d;%dH"
#define SIG_UNAVAILABLE 				                0xFF
#define SIZECHARACTERECEIVED                        2
#define CHARACTERECEIVEDELEMENTONE                  1
#define PROMPTFTELEMENTONE                          1
#define PROMPTFTELEMENTTWO                          2
#define PROMPTFTELEMENTTHREE                        3
#define COLUMNFTDECREASEQUANTITY                    1
#define RECEIVETIMEOUT                             -1
#define SENDTIMEOUT                                -1

/*--------------------Global Variables-------------------------------------*/
ReceiveCommandHandler *ReceiveCommandHandler::thisptr;
BOOLEAN RegUpdateCmdFlag = TRUE;
BOOLEAN regAmplitudeUpdateFlag = FALSE;
BOOLEAN RegMaxMinUpdateFlag = FALSE;
UINT8 SerialRecBuff[RXBUFSIZE] = {0};
UINT8 serialBuff[TXBUFSIZE];
UINT16 LCDKeyStatus = 0;
LCDData ReceiveCommandHandler::lcdData;
DCPSTATUS ReceiveCommandHandler::DCPStatus;
UINT16 ReceiveCommandHandler::LCDSWVersion;
UINT8 ReceiveCommandHandler::LCDCodeCRCLow;
UINT8 ReceiveCommandHandler::LCDCodeCRCHigh;
BOOLEAN ReceiveCommandHandler::SystemInfoReceivedFlag = FALSE;
UINT8 * MacAddress;
BOOLEAN DebugLCDFlag = FALSE;
extern BOOLEAN OldGenAlarm;
BOOLEAN WinLimitHigh = TRUE;
BOOLEAN WinLimitLow = TRUE;
/*-------------------------- External variables --------------------------*/
extern UINT32 TempPeakPower;
extern Parameter MinParameters;
extern Parameter MaxParameters;
SerCmdData SerialCmdReg[] = {
	{ regExAmplitude,&(ReceiveCommandHandler::lcdData.ExAmplitude), 1,RMBASIC_S_STD, FALSE },
	{ regWeldRampTime,&(ReceiveCommandHandler::lcdData.weldRampTime), 2,ALLENABLE, TRUE },
	{ regMemStore,&(ReceiveCommandHandler::lcdData.MemStoreAtWeldEnd), 1, ALLENABLE,FALSE },
	{ regPwrUpSeekScan, &(ReceiveCommandHandler::lcdData.PwrUpSeek),1, ALLENABLE, FALSE },
	{ regSeekRampTime,&(ReceiveCommandHandler::lcdData.seekRampTime), 2,ALLENABLE, TRUE },
	{ regTimedSeek,&(ReceiveCommandHandler::lcdData.TimedSeek), 1, ALLENABLE,FALSE },
	{ regSeekTime, &(ReceiveCommandHandler::lcdData.seekTime), 2,ALLENABLE,TRUE },
	{ regExtFreq, &(ReceiveCommandHandler::lcdData.ExtFreq), 1,RMBASIC_S, TRUE },
	{ regFreqOffset,&(ReceiveCommandHandler::lcdData.FreqOffsetValue), 2,ALLENABLE,TRUE },
	{ regcutoffs, &(ReceiveCommandHandler::lcdData.CutOffs), 1,DCXA_F_PC, TRUE },
	{ reglimits, &(ReceiveCommandHandler::lcdData.Limits), 1,DCXA_F_PC,TRUE },
	{ regIP1, &(ReceiveCommandHandler::lcdData.IP1), 1,ALLENABLE, FALSE },
	{ regIP2, &(ReceiveCommandHandler::lcdData.IP2), 1,ALLENABLE, FALSE },
	{ regIP3, &(ReceiveCommandHandler::lcdData.IP3), 1,ALLENABLE, FALSE },
	{ regIP4, &(ReceiveCommandHandler::lcdData.IP4), 1,ALLENABLE, FALSE },
	{ regGateWay1, &(ReceiveCommandHandler::lcdData.GateWay1),1,ALLENABLE, FALSE },
	{ regGateWay2,&(ReceiveCommandHandler::lcdData.GateWay2), 1, ALLENABLE,FALSE },
	{ regGateWay3, &(ReceiveCommandHandler::lcdData.GateWay3), 1,ALLENABLE, FALSE },
	{ regGateWay4, &(ReceiveCommandHandler::lcdData.GateWay4),1, ALLENABLE,FALSE },
	{ regSubNetMask1, &(ReceiveCommandHandler::lcdData.SubNetMask1),1,ALLENABLE, FALSE },
	{ regSubNetMask2,&(ReceiveCommandHandler::lcdData.SubNetMask2), 1,ALLENABLE, FALSE },
	{ regSubNetMask3,&(ReceiveCommandHandler::lcdData.SubNetMask3), 1,ALLENABLE, FALSE },
	{ regSubNetMask4,&(ReceiveCommandHandler::lcdData.SubNetMask4), 1,ALLENABLE, FALSE },
	{ regIPSetUp,&(ReceiveCommandHandler::lcdData.IPSetUp), 1,ALLENABLE, FALSE },
	{ regFbaddress1,&(ReceiveCommandHandler::lcdData.FieldBusAddr1), 1,DCXF_PC, TRUE },
	{ regFbaddress2,&(ReceiveCommandHandler::lcdData.FieldBusAddr2), 1, DCXF_PC,TRUE },
	{ regFbaddress3, &(ReceiveCommandHandler::lcdData.FieldBusAddr3),1,DCXF_PC, TRUE },
	{ regFbaddress4,&(ReceiveCommandHandler::lcdData.FieldBusAddr4), 1, DCXF_PC,TRUE },
	{ regTimeMaintained, &(ReceiveCommandHandler::lcdData.TimeMaintained), 1, RACKMOUNT_STD, FALSE },
	{ regSIG, &(ReceiveCommandHandler::lcdData.SIG), 1,ALLENABLE, FALSE },
	{ regWeldMode,&(ReceiveCommandHandler::lcdData.WeldMode), 1,ALLENABLE, TRUE },
	{ regPowerWindowLimitLow,&(ReceiveCommandHandler::lcdData.PowerWindowLimitLow), 1,RACKMOUNT_STD, FALSE },
	{ regPowerWindowLimitHigh,&(ReceiveCommandHandler::lcdData.PowerWindowLimitHigh), 1,RACKMOUNT_STD, FALSE },
	{ regMemoryClear,&(ReceiveCommandHandler::lcdData.MemoryClear), 1,ALLENABLE, FALSE },
	{ regAmplitude,&(ReceiveCommandHandler::lcdData.Amplitude), 1, ALLENABLE,TRUE },
	{ regTimeModeVal, &(ReceiveCommandHandler::lcdData.TimeModeValue),2,RM_STD_A_F_PC, TRUE },
	{ regEneregyModeVal,&(ReceiveCommandHandler::lcdData.EnergyModeValue), 4,RM_STD_A_F_PC,TRUE },
	{ regPKPowerModeVal,&(ReceiveCommandHandler::lcdData.PKPwrModeValue), 1,DCXA_F_PC, TRUE },
	{ regGndDtctModeVal,&(ReceiveCommandHandler::lcdData.GndDetectModeValue), 2,DCXA_F_PC,TRUE },
	{ regSystemType, &(ReceiveCommandHandler::lcdData.SystemType), 1,ALLENABLE,FALSE },
	{ regUnused, 0, 0, NOTUSED, FALSE },
};

ReceiveCommandHandler::ReceiveCommandHandler(int intf):
		  Task("RxHandler", RECEIVECOMMHANDLER_STACKSIZE), port(1536, 1536)
/****************************************************************************/
/* Description:-												 								 */
/* This is the constructor for ReceiveCommandhandler. Initialising the baud */
/* rate,57600 configuring U0RTS pin for GPIO.                                 */
/****************************************************************************/
{
	thisptr = this;
	interface = intf;
	CmdReceivedFlag = FALSE;
	lcdCommunicationLost = TRUE;
	SystemInfoReceivedFlag = FALSE;
	port.Init(0, LCDBAUDRATE, 3, 3, FALSE);
	MCF_PAD_PAR_UART &= 0xF3;
	MCF_GPIO_PDDR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
	MCF_GPIO_PODR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
   RowFT=0;
   ColFT=0;
}


ReceiveCommandHandler::~ReceiveCommandHandler() {
}


void ReceiveCommandHandler::Run()
/****************************************************************************/
/* void Run ()                                                              */
/* Description:-												 								 */
/* Pulsing the configured GPIO pins low and then high to drive the lcd.		 */
/* Responsible for receiving LCD command in Motorola S9 record format and   */
/* process it accordingly.                                						 */
/****************************************************************************/
{
	DelayMs(50);
	MCF_GPIO_PODR_UART &= ~MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
	DelayMs(50);
	MCF_GPIO_PODR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
	printf("\n Starting ReceiveCommandHandler...\n");
	static enum ReceiveState RecState = STATE_WAIT_START;
	static SINT32 CharacterCtr = 0;
	static UINT16 LEDState = REDLED_STATE; //Red
	LED::Set(2, LEDState);
	MacAddress = CyGOS::GetMacAddress(interface);
	//
	//******************************//
	// LCD Factory Commands Section //
	//******************************//
	BOOLEAN CommandOkFlag = FALSE;
	STR InputValueFTStr[COMMAND_INPUT_SIZE] = {0};
	STR OutputValueFTStr[COMMAND_OUTPUT_SIZE] = {0};
	STR PrintStr[COMMAND_INPUT_SIZE] = {0};
	memset(InputValueFTStr,0,COMMAND_INPUT_SIZE);
	memset(OutputValueFTStr,0,COMMAND_OUTPUT_SIZE);
	memset(PrintStr,0,COMMAND_INPUT_SIZE);
	UINT16 LenghtFT = COMMAND_INPUT_SIZE;
	UINT8 CountCtr = 0;
   UINT8 CharacterReceived[SIZECHARACTERECEIVED] = {0};
   UINT8 Character = 0;

	ClrscrFT();
	ColFT=FT_PROMPT_SIZE;
	RowFT++;
	PromptFTStr[0]='F';
	PromptFTStr[PROMPTFTELEMENTONE]='T';
	PromptFTStr[PROMPTFTELEMENTTWO]='>';
	PromptFTStr[PROMPTFTELEMENTTHREE]='\0';
	WritelnStrFT(PromptFTStr);
	GotoXYFT(RowFT,ColFT);

	while (FOREVER) {
	   CharacterReceived[0] = 0;
	   CharacterReceived[1] = 0;
	   port.Receive(this, CharacterReceived[0], RECEIVETIMEOUT);
	   Character = CharacterReceived[0];
	   //******************************//
	   // LCD Factory Commands Section //
	   //******************************//
	   if(CommandsFT::Status==CommandsFT::FT_ON) {
	      if(Character>=FIRSTLETTEROFTHEALPHABETLOWERCASE && Character<=LASTLETTEROFTHEALPHABETLOWERCASE)
			   Character = Character-UPERCASECONVERSION; //Upper Case
			   switch (Character) {
			      case FT_KEY_ENTER:
			 	   if(!UtilsFT::IsEmpty(InputValueFTStr)) {
			 	      if(strcmp(InputValueFTStr,"CR")==0){ //clear the screen
			 	     	   ClrscrFT();
			 	      }
			 	      else if(strcmp(InputValueFTStr,"?")==0) { //list commands
			 	     	   UtilsFT::Clean(OutputValueFTStr);
			 	     	   strcpy(OutputValueFTStr,"CR Clear Screen");
					 	   RowFT++;
					 	   ColFT=0;
					 	   WritelnCharFT('\n');
					 	   GotoXYFT(RowFT,ColFT);
					 	   sprintf(PrintStr,"%s\n",OutputValueFTStr);
					 	   WritelnStrFT(PrintStr);
			 	     	   for(CountCtr=0;CountCtr<FT_ARRAY_SIZE;CountCtr++) {
			 	     		   UtilsFT::Clean(InputValueFTStr);
			 	     		   UtilsFT::Clean(OutputValueFTStr);
			 	     		   sprintf(InputValueFTStr,"HE %d",CountCtr);
			 	     		   CommandOkFlag=CommandsFT::Execute(InputValueFTStr,OutputValueFTStr);
			 	     	      if(!UtilsFT::IsEmpty(OutputValueFTStr)){
							      RowFT++;
							 	   ColFT=0;
							 	   WritelnCharFT('\n');
							 	   sprintf(PrintStr,"%s\n",OutputValueFTStr);
							 	   WritelnStrFT(PrintStr);
			 	     	      }
			 	     	   }
			 	      }
			 	      else{ //execute ft cmd
					      RowFT++;
					 	   ColFT=0;
					 	   WritelnCharFT('\n');
					 	   GotoXYFT(RowFT,ColFT);
			 	     	   UtilsFT::Clean(OutputValueFTStr);
			 	     	   CommandOkFlag=CommandsFT::Execute(InputValueFTStr,OutputValueFTStr);
					 	   sprintf(PrintStr,"%s\n",OutputValueFTStr);
					 	   WritelnStrFT(PrintStr);
			 	      }
			 	   }
			 	   RowFT++;
			 	   ColFT=0;
			 	   WritelnCharFT('\n');
			 	   GotoXYFT(RowFT,ColFT);
			 	   sprintf(PrintStr,"%s\n",PromptFTStr);
			 	   WritelnStrFT(PrintStr);
			 	   ColFT=FT_PROMPT_SIZE;
			 	   GotoXYFT(RowFT,ColFT);
			 	   memset(InputValueFTStr,0,COMMAND_INPUT_SIZE);
			 	   break;
			 	   case FT_KEY_BACKSPACE:
			 	   if(ColFT<=FT_PROMPT_SIZE) {
			 	  	   ColFT=FT_PROMPT_SIZE;
			 	   }
			 	   else{
			 	  	   ColFT--;
			 	   }
			 	   Character=' ';
			 	   GotoXYFT(RowFT,ColFT);
			 	   WritelnCharFT(Character);
			 	   Character='\0';
			 	   InputValueFTStr[ColFT-FT_PROMPT_SIZE]=Character;
			 	   GotoXYFT(RowFT,ColFT);
			 	   break;
			 	   default:
			 	   if(ColFT>=LenghtFT-COLUMNFTDECREASEQUANTITY) {
			 	      Character=' ';
			 	      InputValueFTStr[ColFT-FT_PROMPT_SIZE]=Character;
			 	      WritelnCharFT(Character);
			 	      ColFT=LenghtFT-COLUMNFTDECREASEQUANTITY;
			 	   }
			 	   else{
			 	      InputValueFTStr[ColFT-FT_PROMPT_SIZE]=Character;
			 	     WritelnCharFT(Character);
			 	      ColFT++;
			      }
			 	   GotoXYFT(RowFT,ColFT);
		      }
	   }
	   else if(CommandsFT::Status==CommandsFT::FT_OFF) {
	      if (DebugLCDFlag == TRUE) {
			   printf("%c", Character);
		   }
		   //
		   switch (RecState) {
		   case STATE_WAIT_START:
		   if (Character == 'S') {
			   CharacterCtr = 0;
			   SerialRecBuff[CharacterCtr] = Character;
			   CharacterCtr++;
			   RecState = STATE_RECEIVE;
		   }
		   break;
		   case STATE_RECEIVE:
		   	SerialRecBuff[CharacterCtr] = Character;
		   CharacterCtr++;
		   if (CharacterCtr >= RXBUFSIZE) {
			   CharacterCtr = 0;
			   RecState = STATE_WAIT_START;
		   }
		   else if (Character == CR)
		   	RecState = STATE_WAIT_LF;
		   break;
		   case STATE_WAIT_LF:
		   if (Character == LF) {
		   	SerialRecBuff[CharacterCtr] = Character;
			   CmdReceivedFlag = TRUE;
			   ProcessCommand(SerialRecBuff);
			   CharacterCtr = 0;
			   RecState = STATE_WAIT_START;
			   LED::Set(2, LEDState);
			   if (LEDState == REDLED_STATE)
				   LEDState = GREENLED_STATE;
			   else
				   LEDState = REDLED_STATE;
			   }
			   break;
		   default:
			   CharacterCtr = 0;
			   RecState = STATE_WAIT_START;
		   }
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
	for (indx = 0; indx < dataLen; indx++) {
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
	UINT16 addr, RegAddr;
	UINT8 buffIndex = 1;
	UINT8 byteCount = 0;
	UINT32 RegValue = 0;
	UINT8 crcByte;
	UINT8 VerifyChkSum, Cmd;
	static UINT8 updateCounter = 0;
	UINT8 RegSize = 0;

	if (LcdCommand[buffIndex] == polling) {	//every reply is the reply of POLLING
		buffIndex++;
		AsciiToHex(&totalBytes, sizeof(totalBytes), &LcdCommand[buffIndex]);
		VerifyChkSum = VerifyCheckSum(&LcdCommand[buffIndex], totalBytes);
		if (VerifyChkSum == 0) {
			buffIndex += (sizeof(totalBytes) * 2);
			AsciiToHex(&addr, sizeof(addr), &LcdCommand[buffIndex]);	//Addr is the Key status in Polling.
			buffIndex += (sizeof(addr) * 2);
			switch (addr) {
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
			AsciiToHex(&RegAddr, sizeof(RegAddr), &LcdCommand[buffIndex]);	//Addr is the Key status in Polling.
			buffIndex += (sizeof(RegAddr) * 2);
			if (lcdCommunicationLost) {
				//Do the Log entry in the very first reply to LCD polling command
				//at powerup as well as whenever communication reestablished after
				//LCDCOMTIMEOUT period. Timeout is detected in Tick() Function.
				//Log::WriteEventLog(LCD_BOARD_DETECTED, 0, 0);
				lcdCommunicationLost = FALSE;
				SystemInfoReceivedFlag = FALSE;
			}
			if ((Cmd == cmdNocmd) && (RegUpdateCmdFlag == TRUE)) {
				updateCounter++;
				if (updateCounter > 2) {
					updateCounter = 0;
					RegUpdateCmdFlag = FALSE;
				}
				TransmitPollingCommandReply(cmdGetAllRegCurrentValue, 0);
				//sysConfig.CurrSystemType = STD;
			}
			else if ((Cmd == cmdNocmd) && (SystemInfoReceivedFlag == FALSE)) {
				TransmitPollingCommandReply(cmdGetSystemInfo, 0);
				SystemInfoReceivedFlag = TRUE;
			}
			else if ((Cmd == cmdNocmd) && (regAmplitudeUpdateFlag == TRUE)) {
				TransmitPollingCommandReply(cmdGetRegValue, regAmplitude);
				regAmplitudeUpdateFlag = FALSE;
			}
			else if ((Cmd == cmdNocmd) && (RegMaxMinUpdateFlag == TRUE)) {
				TransmitPollingCommandReply(cmdGetAllRegMaxValues, 0);
				RegMaxMinUpdateFlag = FALSE;
			}
			else {
				TransmitPollingCommandReply(Cmd, RegAddr);
				if(DebugLCDFlag == TRUE) printf("\n Cmd %d \n", Cmd);
			}
		}
	}
	else if (LcdCommand[buffIndex] == dataCmd) {
		buffIndex++;
		AsciiToHex(&totalBytes, sizeof(totalBytes), &LcdCommand[buffIndex]);
		VerifyChkSum = VerifyCheckSum(&LcdCommand[buffIndex], totalBytes);
		if (VerifyChkSum == 0) {
			buffIndex += (sizeof(totalBytes) * 2);
			while (totalBytes > (byteCount + 1)) {
				if (totalBytes >= byteCount + 3) {
					AsciiToHex(&addr, sizeof(addr), &LcdCommand[buffIndex]);
					buffIndex += (sizeof(addr) * 2);
					byteCount += sizeof(addr);
					RegSize = (addr & 0xF000) >> 12; // Upper 4 bits are carrying Size of register.
					addr = addr & 0x0FFF; // Lower 12 bits has the Register number.
					//read value
					RegValue = 0;
					AsciiToHex(&RegValue, RegSize, &LcdCommand[buffIndex]);
					buffIndex += (RegSize * 2);
					byteCount += RegSize;
					// convert from Bigendian to little endian format.
					// RegValue is 4 byte variable and if register size is one byte than value will be in upper 8 bits. Shift the RegValue according
					//to the size of Register.
					RegValue = RegValue >> ((sizeof(RegValue) - RegSize) * 8);
					if (DebugLCDFlag == TRUE) {
					 printf("\nRG %d RS = %d, RV = %d ", (DataReg) addr,RegSize, RegValue);
					 }
					ProcessRegUpdateCommand((DataReg) addr, RegValue);
				} else {
					buffIndex += 2;					// totalBytes - 1;
					byteCount++;
				}
			}
			AsciiToHex(&crcByte, sizeof(crcByte), &LcdCommand[buffIndex]);
		} else
			pprintf("\n checksum error data cmd");
	} //else
	  //pprintf(" LCD command not supported ");
}

UINT8 ReceiveCommandHandler::PrepareDCPStatusByte()
/****************************************************************************/
/* void PrepareDCPStatusByte ()                                             */
/* Description:-												 								 */
/* This function prepares DCP status to be send to LCD.                     */
/****************************************************************************/
{
	UINT8 Status;
	//Get DCP Status.
	DCPStatus.Status.StatusFlags = 0;
	DCPStatus.Status.Flags.Alarm =
	      (!WebSite::CheckForDiagnosticFlag()) ? (UserIO::GetGenAlarmFlag()
	            || WindowLimitAlarm) :
	            OldGenAlarm;
	DCPStatus.Status.Flags.Ready = UserIO::GetReadyFlag();
	DCPStatus.Status.Flags.MemReset = MemoryReset;
	DCPStatus.Status.Flags.SonicsActive = (
	      (OutputStatusToFPGA & SONIC_ON_OUT) ? TRUE : FALSE);
	DCPStatus.Status.Flags.DispLock = (UserIO::CheckForLockOutChange()
	      || WebSite::CheckForManufacturingFlag() || MFCycleFlag);
	Status = DCPStatus.Status.StatusFlags;
	return Status;
}

void ReceiveCommandHandler::TransmitPollingCommandReply(UINT8 Cmd,
      UINT16 dataReg)
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
	UINT8 currPower, peakPower, DCPStatusByte, DataByteCount;
	UINT16 MemoryFreq;
	SINT32 TempErrorReason = 0;
	UINT8 CRCbyte = 0, indx, indexer;

	DataByteCount = 0;
	DCPStatusByte = PrepareDCPStatusByte();

	//Convert current power input in percentage form
	currPower = (PowerInput * 100) / ADC11BITVALUE;

	if ((DCPStatus.Status.Flags.SonicsActive) && (currPower <= 0))
		currPower = 1;					//set min power to 1% whenever sonics are

	//Convert Peak power in percentage form
	peakPower = (TempPeakPower * 100) / ADC11BITVALUE;

	//Convert MemoryFreq
	MemoryFreq = (((CurrentPreset.MemoryFreq + CurrentPreset.DigitalTune)
	      * F_Factor) / 1000);

	TempErrorReason = ErrorReason;

	cmd.startChar = STARTCHAR;
	cmd.recordType = polling;
	cmd.byteCount = 0;
	cmd.addr = 0;
	cmd.byteCount += sizeof(cmd.addr);
	cmd.data[DataByteCount++] = DCPStatusByte;
	cmd.data[DataByteCount++] = currPower;
	cmd.data[DataByteCount++] = peakPower;
	cmd.data[DataByteCount++] = MemoryFreq >> 8;
	cmd.data[DataByteCount++] = MemoryFreq;
	cmd.data[DataByteCount++] = (TempErrorReason >> 8) & 0xFF;
	cmd.data[DataByteCount++] = TempErrorReason & 0xFF;


	switch (Cmd) {
	case cmdNocmd:
		cmd.data[DataByteCount++] = cmdNocmd;
		cmd.data[DataByteCount++] = 0x00;
		cmd.data[DataByteCount++] = 0x00;
		break;
	case cmdGetRegValue:
		UpdateLCDData();
		indexer = 0;
		cmd.data[DataByteCount++] = Cmd;
		for (SINT32 Indx = 0; SerialCmdReg[Indx].data; Indx++) {
			if (SerialCmdReg[Indx].dataReg == dataReg) {
				//Register number is in lower 12 bits. Upper four bits is the size of register.
				cmd.data[DataByteCount++] = (((SerialCmdReg[Indx].dataReg >> 8)
				      & 0x0F) | ((SerialCmdReg[Indx].dataLen << 4) & 0xF0));
				cmd.data[DataByteCount++] = (SerialCmdReg[Indx].dataReg) & 0xFF;
				for (UINT8 j = 0; j < SerialCmdReg[Indx].dataLen; j++)
					cmd.data[DataByteCount++] =
					      ((UINT8*) SerialCmdReg[Indx].data)[j];
			}
		}
		break;
	case cmdGetAllRegCurrentValue:
		UpdateLCDData();
		indexer = 0;
		cmd.data[DataByteCount++] = Cmd;
		for (SINT32 Indx = 0; SerialCmdReg[Indx].data; Indx++) {
			if ((SerialCmdReg[Indx].RegEnable & (BIT(EPromData.Systemtype)))
			      != 0) {
				cmd.data[DataByteCount++] = (((SerialCmdReg[Indx].dataReg >> 8)
				      & 0x0F) | ((SerialCmdReg[Indx].dataLen << 4) & 0xF0));
				cmd.data[DataByteCount++] = (SerialCmdReg[Indx].dataReg) & 0xFF;
				for (UINT8 j = 0; j < SerialCmdReg[Indx].dataLen; j++)
					cmd.data[DataByteCount++] =
					      ((UINT8*) SerialCmdReg[Indx].data)[j];
			}
		}
		break;
	case cmdGetAllRegDefaultValue:
		GetDefaultParameterValues();
		indexer = 0;
		cmd.data[DataByteCount++] = Cmd;
		for (SINT32 Indx = 0; SerialCmdReg[Indx].data; Indx++) {
			//Register number is in lower 12 bits. Upper four bits is the size of register.
			if ((SerialCmdReg[Indx].RegEnable & (BIT(EPromData.Systemtype)))
			      != 0) {
				cmd.data[DataByteCount++] = (((SerialCmdReg[Indx].dataReg >> 8)
				      & 0x0F) | ((SerialCmdReg[Indx].dataLen << 4) & 0xF0));
				cmd.data[DataByteCount++] = (SerialCmdReg[Indx].dataReg) & 0xFF;
				for (UINT8 j = 0; j < SerialCmdReg[Indx].dataLen; j++)
					cmd.data[DataByteCount++] =
					      ((UINT8*) SerialCmdReg[Indx].data)[j];
			}
		}
		break;
	case cmdSetAllRegToDefaultVaue:
		if (dataReg == COLDSTART)
			SetDefaultParameter();
		else if(dataReg == RESTOREWELDPRESET){
			SetDefaultWeldPreset();
			Log::WriteEventLog(EVENT_RESTORE_DEFAULTS,  1, 0, 0, FALSE);
		}
		cmd.data[DataByteCount++] = cmdNocmd;
		cmd.data[DataByteCount++] = 0x00;
		cmd.data[DataByteCount++] = 0x00;
		break;
	case cmdGetSystemInfo:
		//printf("\n SysytemInfo");
		cmd.data[DataByteCount++] = (UINT8) Cmd;
		//3 bytes optional data
		for (UINT8 Indx = 0; Indx < 6; Indx++)
			cmd.data[DataByteCount++] = MacAddress[Indx];
		break;
	case cmdGetAllRegMinValues:
		GetMinParameterValues();
		cmd.data[DataByteCount++] = Cmd;
		for (SINT32 Indx = 0; SerialCmdReg[Indx].data; Indx++) {
			//DCP don't need to send the Min values of all the Registers. There are only few registers for which the MIN values can be changed
			//like WeldTime. Min Values are fixed for Registers like IPAddr or TimedSeek and can not be changed.
			if ((SerialCmdReg[Indx].MinMaxValue)) {
				if ((SerialCmdReg[Indx].RegEnable & (BIT(EPromData.Systemtype)))
				      != 0) {
					//Register number is in lower 12 bits. Upper four bits is the size of register.
					cmd.data[DataByteCount++] = (((SerialCmdReg[Indx].dataReg >> 8)
					      & 0x0F) | ((SerialCmdReg[Indx].dataLen << 4) & 0xF0));
					cmd.data[DataByteCount++] = (SerialCmdReg[Indx].dataReg) & 0xFF;
					for (UINT8 j = 0; j < SerialCmdReg[Indx].dataLen; j++)
						cmd.data[DataByteCount++] =
						      ((UINT8*) SerialCmdReg[Indx].data)[j];
				}
			}
		}
		break;
	case cmdGetAllRegMaxValues:
		GetMaxParameterValues();
		cmd.data[DataByteCount++] = Cmd;
		for (SINT32 Indx = 0; SerialCmdReg[Indx].data; Indx++) {
			//DCP don't need to send the MAX values of all the Registers. There are only few registers for which the MAX values can be changed
			//like WeldTime. MAX Values are fixed for Registers like IPAddr or TimedSeek and can not be changed.
			if (SerialCmdReg[Indx].MinMaxValue == TRUE) {
				if ((SerialCmdReg[Indx].RegEnable & (BIT(EPromData.Systemtype)))
				      != 0) {
					//Register number is in lower 12 bits. Upper four bits is the size of register.
					cmd.data[DataByteCount++] = (((SerialCmdReg[Indx].dataReg >> 8)
					      & 0x0F) | ((SerialCmdReg[Indx].dataLen << 4) & 0xF0));
					cmd.data[DataByteCount++] = (SerialCmdReg[Indx].dataReg) & 0xFF;
					for (UINT8 j = 0; j < SerialCmdReg[Indx].dataLen; j++)
						cmd.data[DataByteCount++] =
						      ((UINT8*) SerialCmdReg[Indx].data)[j];
				}
			}
		}
		break;
	default:
		cmd.data[DataByteCount++] = Cmd;
		cmd.data[DataByteCount++] = 0x00;
		cmd.data[DataByteCount++] = 0x00;
		break;
	}
	for (indx = 0; indx < DataByteCount; indx++)
		CRCbyte ^= cmd.data[indx];
	cmd.byteCount += DataByteCount;
	cmd.byteCount += sizeof(cmd.checkSum);
	CRCbyte ^= cmd.byteCount;
	//CRC on bytecount, addr and data
	//byte count includes the CRC byte so decrease the data length to be passed to CRC routine
	cmd.checkSum = CRCbyte;

	cmd.cr = CR;
	cmd.lf = LF;

	TransmitAsciiCommand(cmd);
}

void ReceiveCommandHandler::HexToAscii(void * var, SINT32 varSize, UINT8 * buff)
/****************************************************************************/
/* void HexToAscii ()                                                       */
/* Description:-																				 */
/* routine converts  hex number to ascii chracter                           */
/****************************************************************************/
{
	SINT32 i;
	const char * hexStr = "0123456789ABCDEF";
	SINT32 offset = 0;
	UINT8 * varPtr = (UINT8 *) var;
	SINT32 temp = 0;
	for (i = 0; i < varSize; i++) {
		buff[offset++] = hexStr[varPtr[temp] / 16];
		buff[offset++] = hexStr[varPtr[temp] % 16];
		temp++;
	}
}

UINT8 ReceiveCommandHandler::xtoi(UINT8 hexNumber)
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

void ReceiveCommandHandler::AsciiToHex(void * var, SINT32 varsize, UINT8 * buff)
/****************************************************************************/
/* void AsciiToHex ()                                                       */
/* Description:-																				 */
/* This function converts ASCII to Hex.                                    */
/****************************************************************************/
{
	SINT32 i;
	UINT8 offset = 0;
	UINT8 * varPtr = (UINT8 *) var;
	for (i = 0; i < varsize; i++) {
		varPtr[i] = (xtoi(buff[offset]) << 4) | (xtoi(buff[offset + 1]));
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
	SINT32 charCnt = 0;
	SINT32 dataLen = 0, i;
	serialBuff[charCnt++] = cmd.startChar;
	serialBuff[charCnt++] = cmd.recordType;
	HexToAscii(&cmd.byteCount, sizeof(cmd.byteCount), &serialBuff[charCnt]);
	charCnt += (sizeof(cmd.byteCount) * 2);
	HexToAscii(&cmd.addr, sizeof(cmd.addr), &serialBuff[charCnt]);
	charCnt += (sizeof(cmd.addr) * 2);

	dataLen = cmd.byteCount - sizeof(cmd.addr) - sizeof(cmd.checkSum);
	for (i = 0; i < dataLen; i++) {
		HexToAscii(&cmd.data[i], sizeof(UINT8), &serialBuff[charCnt]);
		charCnt += 2;
	}

	HexToAscii(&cmd.checkSum, sizeof(cmd.checkSum), &serialBuff[charCnt]);
	charCnt += (sizeof(cmd.checkSum) * 2);
	serialBuff[charCnt++] = cmd.cr;
	serialBuff[charCnt++] = cmd.lf;

	if (DebugLCDFlag == TRUE)
		printf("\nData");
	for (i = 0; i < charCnt; i++) {
		port.Send(this, serialBuff[i], -1);
		if (DebugLCDFlag == TRUE) {
			printf("%c", serialBuff[i]);
		}
	}
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
	if (MFCycleFlag)
		lcdData.Amplitude = CurrentCycleParam.Amplitude;
	else
		lcdData.Amplitude = CurrentPreset.WeldAmplitude;

	lcdData.ExAmplitude = CurrentPreset.SWDip.Flags.ExtAmplitude;
	lcdData.ExtFreq = CurrentPreset.SWDip.Flags.ExtFrequency;
	lcdData.FreqOffsetValue = CurrentPreset.FreqOffset;
	lcdData.CutOffs = CurrentPreset.CutOffs;
	lcdData.Limits = CurrentPreset.Limits;
	switch (sysConfig.currNetworkConfig) {
	case configDefault:
	case configStatic:
		lcdData.IP1 = (sysConfig.systemIPStatic.addr >> 24) & 0xff;
		lcdData.IP2 = (sysConfig.systemIPStatic.addr >> 16) & 0xff;
		lcdData.IP3 = (sysConfig.systemIPStatic.addr >> 8) & 0xff;
		lcdData.IP4 = (sysConfig.systemIPStatic.addr & 0xff);

		lcdData.SubNetMask1 = (sysConfig.netmask.addr >> 24) & 0xff;
		lcdData.SubNetMask2 = (sysConfig.netmask.addr >> 16) & 0xff;
		lcdData.SubNetMask3 = (sysConfig.netmask.addr >> 8) & 0xff;
		lcdData.SubNetMask4 = (sysConfig.netmask.addr & 0xff);

		lcdData.GateWay1 = (sysConfig.gw.addr >> 24) & 0xff;
		lcdData.GateWay2 = (sysConfig.gw.addr >> 16) & 0xff;
		lcdData.GateWay3 = (sysConfig.gw.addr >> 8) & 0xff;
		lcdData.GateWay4 = (sysConfig.gw.addr & 0xff);
		break;
	case configDhcpServer:
		lcdData.IP1 = (sysConfig.systemIP.addr >> 24) & 0xff;
		lcdData.IP2 = (sysConfig.systemIP.addr >> 16) & 0xff;
		lcdData.IP3 = (sysConfig.systemIP.addr >> 8) & 0xff;
		lcdData.IP4 = (sysConfig.systemIP.addr & 0xff);

		lcdData.SubNetMask1 = (sysConfig.dhcpConfig.netmask.addr >> 24) & 0xff;
		lcdData.SubNetMask2 = (sysConfig.dhcpConfig.netmask.addr >> 16) & 0xff;
		lcdData.SubNetMask3 = (sysConfig.dhcpConfig.netmask.addr >> 8) & 0xff;
		lcdData.SubNetMask4 = (sysConfig.dhcpConfig.netmask.addr & 0xff);

		lcdData.GateWay1 = (sysConfig.dhcpConfig.gateway.addr >> 24) & 0xff;
		lcdData.GateWay2 = (sysConfig.dhcpConfig.gateway.addr >> 16) & 0xff;
		lcdData.GateWay3 = (sysConfig.dhcpConfig.gateway.addr >> 8) & 0xff;
		lcdData.GateWay4 = (sysConfig.dhcpConfig.gateway.addr & 0xff);
		break;
	case configDhcpClient:
		temp = CyGOS::GetIPAddress(interface);
		lcdData.IP1 = (temp.addr >> 24) & 0xff;
		lcdData.IP2 = (temp.addr >> 16) & 0xff;
		lcdData.IP3 = (temp.addr >> 8) & 0xff;
		lcdData.IP4 = (temp.addr & 0xff);

		temp = CyGOS::GetSubnetMask(interface);
		lcdData.SubNetMask1 = (temp.addr >> 24) & 0xff;
		lcdData.SubNetMask2 = (temp.addr >> 16) & 0xff;
		lcdData.SubNetMask3 = (temp.addr >> 8) & 0xff;
		lcdData.SubNetMask4 = (temp.addr & 0xff);

		temp = CyGOS::GetGatewayAddress(interface);
		lcdData.GateWay1 = (temp.addr >> 24) & 0xff;
		lcdData.GateWay2 = (temp.addr >> 16) & 0xff;
		lcdData.GateWay3 = (temp.addr >> 8) & 0xff;
		lcdData.GateWay4 = (temp.addr & 0xff);
		break;
	default:
		break;
	}
	lcdData.IPSetUp = (UINT16) sysConfig.currNetworkConfig;
	lcdData.FieldBusAddr1 = NOTUSED;
	lcdData.FieldBusAddr2 = NOTUSED;
	lcdData.FieldBusAddr3 = NOTUSED;
	lcdData.FieldBusAddr4 = NOTUSED;
	// If Time-Impuse / Maintained Mode then set TimeMaintainedMode flag else don't care.
	if(TIMEMAINTAINEDMODE == CurrentPreset.WeldMode){
		lcdData.WeldMode = TIMEMODE;
		lcdData.TimeMaintained = TRUE;
	}
	else if(TIMEMODE == CurrentPreset.WeldMode) {
		lcdData.WeldMode = CurrentPreset.WeldMode;
		lcdData.TimeMaintained = FALSE;
	}
	else if(BIT(EPromData.Systemtype) == RACKMOUNT_S){		//	Sends Continuous mode to LCD even if the current weld mode
			lcdData.WeldMode = CONTINUOUSMODE;							//is Energy only if the special Energy software is enabled.
			}
	else
		lcdData.WeldMode = CurrentPreset.WeldMode;
	lcdData.TimeModeValue = CurrentPreset.WeldTime;
	lcdData.EnergyModeValue = CurrentPreset.EnergyCP;
	lcdData.PKPwrModeValue = CurrentPreset.PeakPowerCP;
	lcdData.GndDetectModeValue = CurrentPreset.ScrubTime;
	lcdData.PwrUpSeek = CurrentPreset.SWDip.Flags.PowerUpOptions;
	lcdData.MemStoreAtWeldEnd = CurrentPreset.SWDip.Flags.EndOfWeldStore;
	lcdData.SystemType = EPromData.Systemtype;
	lcdData.MemoryClear = CurrentPreset.SWDip.Flags.ClearMemAtRst;
	if (EPromData.SIGSpecial) {
		lcdData.SIG = CurrentPreset.SIG;
	} else {
		lcdData.SIG = SIG_UNAVAILABLE;
	}
	lcdData.PowerWindowLimitHigh = 0;
	lcdData.PowerWindowLimitLow = 0;
	if (CurrentPreset.SWDip.Flags.PowerLimitOn) {
		if (CurrentPreset.SWDip.Flags.PlusLimitCheck) {
			lcdData.PowerWindowLimitHigh = CurrentPreset.PowerWindowLimitHigh;
		}
		if (CurrentPreset.SWDip.Flags.MinusLimitCheck) {
			lcdData.PowerWindowLimitLow = CurrentPreset.PowerWindowLimitLow;
		}
	}
	WinLimitLow = lcdData.PowerWindowLimitLow > 0 ? TRUE : FALSE;
	WinLimitHigh = lcdData.PowerWindowLimitHigh > 0 ? TRUE : FALSE;

	if (DebugLCDFlag == TRUE)
		pprintf("\n update lcd data ");
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
	lcdData.IP1 = (sysConfig.systemIPStatic.addr >> 24) & 0xff;	//sysConfig.dhcpServerIP.;
	lcdData.IP2 = (sysConfig.systemIPStatic.addr >> 16) & 0xff;
	lcdData.IP3 = (sysConfig.systemIPStatic.addr >> 8) & 0xff;
	lcdData.IP4 = (sysConfig.systemIPStatic.addr & 0xff);

	lcdData.SubNetMask1 = (sysConfig.netmask.addr >> 24) & 0xff;
	lcdData.SubNetMask2 = (sysConfig.netmask.addr >> 16) & 0xff;
	lcdData.SubNetMask3 = (sysConfig.netmask.addr >> 8) & 0xff;
	lcdData.SubNetMask4 = (sysConfig.netmask.addr & 0xff);

	lcdData.GateWay1 = (sysConfig.gw.addr >> 24) & 0xff;
	lcdData.GateWay2 = (sysConfig.gw.addr >> 16) & 0xff;
	lcdData.GateWay3 = (sysConfig.gw.addr >> 8) & 0xff;
	lcdData.GateWay4 = (sysConfig.gw.addr & 0xff);
	if (RegUpdateCmdFlag == FALSE)
		RegUpdateCmdFlag = TRUE;
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
	lcdData.ExAmplitude = OFF;
	lcdData.weldRampTime = WELDRAMPTIME_DEFAULT;
	lcdData.MemStoreAtWeldEnd = ON;
	lcdData.PwrUpSeek = POWERUP_SEEK_DEF;
	lcdData.seekRampTime = SEEKRAMPTIME_DEFAULT;
	lcdData.TimedSeek = OFF;
	lcdData.seekTime = SEEKTIME_DEFAULT;
	lcdData.ExtFreq = OFF;
	lcdData.FreqOffsetValue = FREQ_OFFSET_DEF;
	lcdData.CutOffs = NOTUSED;
	lcdData.Limits = NOTUSED;
	lcdData.IP1 = DEFAULTIPSTATIC1;	//sysConfig.dhcpServerIP.;
	lcdData.IP2 = DEFAULTIPSTATIC2;
	lcdData.IP3 = DEFAULTIPSTATIC3;
	lcdData.IP4 = DEFAULTIPSTATIC4;
	lcdData.GateWay1 = DEFGATEWAYSTATIC1;
	lcdData.GateWay2 = DEFGATEWAYSTATIC2;
	lcdData.GateWay3 = DEFGATEWAYSTATIC3;
	lcdData.GateWay4 = DEFGATEWAYSTATIC4;
	//lcdData.AutoSonics = 0;
	lcdData.SubNetMask1 = DEFSUBNETMASK1;
	lcdData.SubNetMask2 = DEFSUBNETMASK2;
	lcdData.SubNetMask3 = DEFSUBNETMASK3;
	lcdData.SubNetMask4 = DEFSUBNETMASK4;
	lcdData.IPSetUp = (UINT16) configStatic;
	lcdData.FieldBusAddr1 = NOTUSED;
	lcdData.FieldBusAddr2 = NOTUSED;
	lcdData.FieldBusAddr3 = NOTUSED;
	lcdData.FieldBusAddr4 = NOTUSED;
	lcdData.TimeMaintained = FALSE;
	if (EPromData.SIGSpecial) {
		lcdData.SIG = CurrentPreset.SIG;
	} else {
		lcdData.SIG = SIG_UNAVAILABLE;
	}
	lcdData.WeldMode = CONTINUOUSMODE;                                //tbc
	lcdData.PowerWindowLimitLow = DEFPOWERLIMITLOW;
	lcdData.PowerWindowLimitHigh = DEFPOWERLIMITHIGH;
	lcdData.MemoryClear = DEFAULT_MEM_CLR_RST;
	lcdData.Amplitude = AMP1_DEFAULT;
	lcdData.TimeModeValue = TIMEMODE_DEFAULT;
	lcdData.EnergyModeValue = ENERGYCP_DEFAULT;
	lcdData.PKPwrModeValue = PEAKPOWER_DEFAULT;
	lcdData.GndDetectModeValue = NOTUSED;
	lcdData.SystemType = EPromData.Systemtype;                             // tbc
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

	lcdData.weldRampTime = CONVMICRO_MILLI(MinParameters.WeldRampTime);
	lcdData.seekRampTime = CONVMICRO_MILLI(MinParameters.SeekRampTime);
	lcdData.seekTime = CONVMICRO_MILLI(MinParameters.SeekTime);
	lcdData.ExtFreq = OFF;
	lcdData.FreqOffsetValue = MinParameters.FreqOffset;
	lcdData.CutOffs = NOTUSED;
	lcdData.Limits = NOTUSED;
	lcdData.FieldBusAddr1 = NOTUSED;
	lcdData.FieldBusAddr2 = NOTUSED;
	lcdData.FieldBusAddr3 = NOTUSED;
	lcdData.FieldBusAddr4 = NOTUSED;
	lcdData.WeldMode = MinParameters.WeldMode;
	lcdData.Amplitude = MinParameters.WeldAmplitude;
	lcdData.TimeModeValue = MinParameters.WeldTime;
	lcdData.EnergyModeValue = MinParameters.EnergyCP;
	lcdData.PKPwrModeValue = MinParameters.PeakPowerCP;
	lcdData.GndDetectModeValue = MinParameters.ScrubTime;

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

	lcdData.weldRampTime = CONVMICRO_MILLI(MaxParameters.WeldRampTime);
	lcdData.seekRampTime = CONVMICRO_MILLI(MaxParameters.SeekRampTime);
	lcdData.seekTime = CONVMICRO_MILLI(MaxParameters.SeekTime);
	lcdData.ExtFreq = ON;
	lcdData.FreqOffsetValue = MaxParameters.FreqOffset;
	lcdData.CutOffs = NOTUSED;
	lcdData.Limits = NOTUSED;
	lcdData.FieldBusAddr1 = NOTUSED;
	lcdData.FieldBusAddr2 = NOTUSED;
	lcdData.FieldBusAddr3 = NOTUSED;
	lcdData.FieldBusAddr4 = NOTUSED;
	lcdData.WeldMode = MaxParameters.WeldMode;
	lcdData.Amplitude = MaxParameters.WeldAmplitude;
	lcdData.TimeModeValue = MaxParameters.WeldTime;
	lcdData.EnergyModeValue = MaxParameters.EnergyCP;
	lcdData.PKPwrModeValue = MaxParameters.PeakPowerCP;
	lcdData.GndDetectModeValue = MaxParameters.ScrubTime;
	//printf("\n Update Max Values");

}

void ReceiveCommandHandler::ProcessRegUpdateCommand(enum DataReg dataReg,UINT32 value)
      /****************************************************************************/
      /* void ProcessRegUpdateCommand ()                                          */
      /* Description:-																				 */
      /* This function is responsible for updating Current preset according to    */
      /* data reg number received in LCD command.                                 */
      /****************************************************************************/
{
	BOOLEAN PresetUpdate = FALSE;
	BOOLEAN WindowLimitsOn = FALSE;
	switch (dataReg) {
		case regSeekRampTime:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, SeekRampTime, CONV_MILLI_MICROSEC(value), ParaSeekSeekRampTime, value, 0, FALSE);
			break;
		case regWeldMode:
			PresetUpdate = TRUE;
			if ((BIT(EPromData.Systemtype) != RACKMOUNT_STD) && value > CONTINUOUSMODE)
				CurrentPreset.WeldMode = CONTINUOUSMODE;
			else{
				if((TIMEMODE == value) && (TRUE == lcdData.TimeMaintained))
					value = TIMEMAINTAINEDMODE;	// To log Time Maintained mode
				CHECKANDLOG_PRESETCHANGE(CurrentPreset, WeldMode, (MODES)value, WcParamWeldMode, value, 0, FALSE);
			}
			RegUpdateCmdFlag = TRUE;
			break;
		case regFreqOffset:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, FreqOffset, value, WcParamFreqOffset, value, 0, FALSE);
			break;
		case regcutoffs:
			PresetUpdate = TRUE;
			CurrentPreset.CutOffs = value;
			break;
		case reglimits:
			PresetUpdate = TRUE;
			CurrentPreset.Limits = value;
			break;
		case regWeldRampTime:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, WeldRampTime, CONV_MILLI_MICROSEC(value), DcpParamWeldRampTime, value, 0, FALSE);
			break;
		case regTimeMaintained:
			PresetUpdate = TRUE;
			lcdData.TimeMaintained = value;
			if((TIMEMODE == CurrentPreset.WeldMode) || (TIMEMAINTAINEDMODE == CurrentPreset.WeldMode)){
				if(TRUE == lcdData.TimeMaintained){
					CHECKANDLOG_PRESETCHANGE(CurrentPreset, WeldMode, TIMEMAINTAINEDMODE, WcParamWeldMode, TIMEMAINTAINEDMODE, 0, FALSE);
					CurrentPreset.SWDip.Flags.HandHeldMode = TRUE;		// Set Handheld flag when Handheld mode is selected from LCD
				}
				else {
					CHECKANDLOG_PRESETCHANGE(CurrentPreset, WeldMode, TIMEMODE, WcParamWeldMode, TIMEMODE, 0, FALSE);
					CurrentPreset.SWDip.Flags.HandHeldMode = FALSE;		// Clear Handheld flag when Time Pulse mode is selected from LCD
				}
			}
			break;
		case regSIG:
			PresetUpdate = TRUE;
			CurrentPreset.SIG = value;
			break;
		case regPowerWindowLimitLow:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, PowerWindowLimitLow, value, PowerWindowLimitLow, value, 0, FALSE);
			if (value > 0) {
				WinLimitLow = TRUE;
				WindowLimitsOn = TRUE;
			} else
				WinLimitLow = FALSE;

			break;
		case regPowerWindowLimitHigh:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, PowerWindowLimitHigh, value, PowerWindowLimitHigh, value, 0, FALSE);
			if (value > 0) {
				WinLimitHigh = TRUE;
				WindowLimitsOn = TRUE;
			} else
				WinLimitHigh = FALSE;
			break;
		case regMemoryClear:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.ClearMemAtRst, value, ClearMemAtRst, value, 0, FALSE);
			break;
		case regTimeModeVal:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, WeldTime, value, WcParamWeldTimeCP, value, 0, FALSE);
			break;
		case regEneregyModeVal:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, EnergyCP, value, WcParamEnergyCP, value, 0, FALSE);
			break;
		case regPKPowerModeVal:
			PresetUpdate = TRUE;
			CurrentPreset.PeakPowerCP = value;
			break;
		case regGndDtctModeVal:
			PresetUpdate = TRUE;
			CurrentPreset.ScrubTime = value;
			break;
		case regTimedSeek:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.TimedSeek, value, DcpParamTimedSeekFlag, value, 0, FALSE);
			break;
		case regSeekTime:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, SeekTime, CONV_MILLI_MICROSEC(value), ParaSeekSeekTime, value, 0, FALSE);
			break;
		case regAmplitude:
			PresetUpdate = TRUE;
			RunningPreset.WeldAmplitude = value;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, WeldAmplitude, value, WcParamAmplitude1, value, 0, FALSE);
			break;
		case regSoftwareVersion:
			LCDSWVersion = value;
			break;
		case regIP1:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.systemIP.addr = ((sysConfig.systemIP.addr & 0x00FFFFFF)
					  | (value << 24));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.systemIPStatic.addr = ((sysConfig.systemIPStatic.addr
					  & 0x00FFFFFF) | (value << 24));
			break;
		case regIP2:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.systemIP.addr = ((sysConfig.systemIP.addr & 0xFF00FFFF)
					  | (value << 16));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.systemIPStatic.addr = ((sysConfig.systemIPStatic.addr
					  & 0xFF00FFFF) | (value << 16));
			break;
		case regIP3:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.systemIP.addr = ((sysConfig.systemIP.addr & 0xFFFF00FF)
					  | (value << 8));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.systemIPStatic.addr = ((sysConfig.systemIPStatic.addr
					  & 0xFFFF00FF) | (value << 8));
			break;
		case regIP4:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.systemIP.addr = ((sysConfig.systemIP.addr & 0xFFFFFF00)
					  | (value));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.systemIPStatic.addr = ((sysConfig.systemIPStatic.addr
					  & 0xFFFFFF00) | (value));
			break;
		case regIPSetUp:
			sysConfig.currNetworkConfig = (NetworkConfiguration) value;
			if (sysConfig.currNetworkConfig == configDefault)
				SetDefaultIPSetUp();
			RegUpdateCmdFlag = TRUE;
			break;
		case regExAmplitude:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.ExtAmplitude, value, WcParamAmpStepCondition, value, 0, FALSE);
			break;
		case regMemStore:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.EndOfWeldStore, value, DcpParamMemoryStoreFlag, value, 0, FALSE);
			break;
		case regPwrUpSeekScan:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.PowerUpOptions, value, PowerUpOptions, value, 0, FALSE);
			break;
		case regCodeCRCLow:
			LCDCodeCRCLow = value & 0x00FF;
			break;
		case regCodeCRCHigh:
			LCDCodeCRCHigh = value & 0x00FF;
			break;
		case regSubNetMask1:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.netmask.addr = ((sysConfig.dhcpConfig.netmask.addr
					  & 0x00FFFFFF) | (value << 24));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.netmask.addr = ((sysConfig.netmask.addr & 0x00FFFFFF)
					  | (value << 24));
			break;
		case regSubNetMask2:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.netmask.addr = ((sysConfig.dhcpConfig.netmask.addr
					  & 0xFF00FFFF) | (value << 16));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.netmask.addr = ((sysConfig.netmask.addr & 0xFF00FFFF)
					  | (value << 16));
			break;
		case regSubNetMask3:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.netmask.addr = ((sysConfig.dhcpConfig.netmask.addr
					  & 0xFFFF00FF) | (value << 8));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.netmask.addr = ((sysConfig.netmask.addr & 0xFFFF00FF)
					  | (value << 8));
			break;
		case regSubNetMask4:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.netmask.addr = ((sysConfig.dhcpConfig.netmask.addr
					  & 0xFFFFFF00) | (value));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.netmask.addr = ((sysConfig.netmask.addr & 0xFFFFFF00)
					  | (value));
			break;
		case regGateWay1:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.gateway.addr = ((sysConfig.dhcpConfig.gateway.addr
					  & 0x00FFFFFF) | (value << 24));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.gw.addr = ((sysConfig.gw.addr & 0x00FFFFFF) | (value << 24));
			break;
		case regGateWay2:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.gateway.addr = ((sysConfig.dhcpConfig.gateway.addr
					  & 0xFF00FFFF) | (value << 16));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.gw.addr = ((sysConfig.gw.addr & 0xFF00FFFF) | (value << 16));
			break;
		case regGateWay3:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.gateway.addr = ((sysConfig.dhcpConfig.gateway.addr
					  & 0xFFFF00FF) | (value << 8));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.gw.addr = ((sysConfig.gw.addr & 0xFFFF00FF) | (value << 8));
			break;
		case regGateWay4:
			if (sysConfig.currNetworkConfig == configDhcpServer)
				sysConfig.dhcpConfig.gateway.addr = ((sysConfig.dhcpConfig.gateway.addr
					  & 0xFFFFFF00) | (value));
			else if (sysConfig.currNetworkConfig == configStatic)
				sysConfig.gw.addr = ((sysConfig.gw.addr & 0xFFFFFF00) | (value));
			break;
		case regExtFreq:
			PresetUpdate = TRUE;
			CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.ExtFrequency, value, WcParamFreqOffsetFlag, value, 0, FALSE);
			break;
		default:
			break;
	}
	CurrentPreset.SWDip.Flags.PowerLimitOn =
	      WindowLimitsOn ? ON : CurrentPreset.SWDip.Flags.PowerLimitOn;
	if (CurrentPreset.SWDip.Flags.PowerLimitOn == ON) {
		CurrentPreset.SWDip.Flags.MinusLimitCheck = WinLimitLow;
		CurrentPreset.SWDip.Flags.PlusLimitCheck = WinLimitHigh;
	}

	if (PresetUpdate)
		SavePreset(0);

}


void ReceiveCommandHandler::Tick() {
	static int usCount = 0;
	if (CmdReceivedFlag == FALSE) {
		usCount += usecPerTick;
		if (usCount >= KEYPRESSEDTIMEOUT) {
			LCDKeyStatus = 0;
		}
		if (usCount >= LCDCOMTIMEOUT) {
			//log entry should be done on the reply of next polling
			lcdCommunicationLost = TRUE;
			//LCD should be given the fresh set of all register values
			RegUpdateCmdFlag = TRUE;
			usCount = 0;
		}
	} else {
		usCount = 0;
		CmdReceivedFlag = FALSE;
	}
}


/************************************************************************************************
*  void ReceiveCommandHandler::ClrscrFT(void)                                                   *
*                                                                                               *
*  Purpose:                                                                                     *
*     This function clears the terminal screen                                                  *
*                                                                                               *
*  Entry condition:                                                                             *
*     None                                                                                      *
*                                                                                               *
*  Exit condition:                                                                              *
*     None                                                                                      *
************************************************************************************************/
void ReceiveCommandHandler::ClrscrFT(void)
{
	STR ClearingStringStr []  = CLEARSTRING;
	RowFT = 0;
	ColFT = 0;

	WritelnStrFT(ClearingStringStr);
	GotoXYFT(RowFT,ColFT);
}


/************************************************************************************************
*  void ReceiveCommandHandler::GotoXYFT(SINT32 Row, SINT32 Column)                              *
*                                                                                               *
*  Purpose:                                                                                     *
*     This function prints out a row and column position                                        *
*                                                                                               *
*  Entry condition:                                                                             *
*     Row and Column to be printed                                                              *
*                                                                                               *
*  Exit condition:                                                                              *
*     None                                                                                      *
************************************************************************************************/
void ReceiveCommandHandler::GotoXYFT(SINT32 Row,SINT32 Column)
{
	STR CursorHomePositionStr [] = CURSORHOMEPOSITION;
	STR PrintStr[COMMAND_INPUT_SIZE] = {0};

	memset(PrintStr,0,COMMAND_INPUT_SIZE);
	sprintf(PrintStr,CursorHomePositionStr,Row,Column);
	WritelnStrFT(PrintStr);
}


/***************************************************************************************************
*   void ReceiveCommandHandler::WritelnStrFT(STR *ValuePtr)                                           *
*                                                                                                  *
*   Purpose:                                                                                       *
*		Sends a string to the port                                                                   *
*                                                                                                  *
*   Entry condition:                                                                               *
*      ValuePtr                                                                                    *
*                                                                                                  *
*   Exit condition:                                                                                *
*	    None                                                                                        *
***************************************************************************************************/
void ReceiveCommandHandler::WritelnStrFT(STR *ValuePtr)
{
	UINT8 Character = 0;

	while(*ValuePtr){
		Character = *ValuePtr++;
		if(Character=='\n'){
		   port.Send(this,'\r',SENDTIMEOUT);
		} else {
		   port.Send(this,Character,SENDTIMEOUT);
		}
	}
}


/***************************************************************************************************
*   void ReceiveCommandHandler::WritelnCharFT(UINT8 Character)                                         *
*                                                                                                  *
*   Purpose:                                                                                       *
*		Sends a character to the port                                                                *
*                                                                                                  *
*  Entry condition:                                                                                *
*     Character                                                                                    *
*                                                                                                  *
*   Exit condition:                                                                                *
*		None                                                                                         *
***************************************************************************************************/
void ReceiveCommandHandler::WritelnCharFT(UINT8 Character)
{
	port.Send(this,Character,SENDTIMEOUT);
}










