/*
 $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/WCTask.cpp_v   1.6   06 Nov 2015 07:02:48   sesharma  $
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
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/WCTask.cpp_v  $
 * 
 *    Rev 1.6   06 Nov 2015 07:02:48   sesharma
 * Fix for DEF01161(Global variable added to store profibus address at power up)
 * 
 *    Rev 1.5   21 Aug 2015 05:52:22   RJamloki
 * Added define for soft reset bit for MII devices
 * 
 *    Rev 1.4   10 Aug 2015 09:42:24   RJamloki
 * Clearing memory frequencies at power up based on clear at power up option. Added cyclic update for fieldbus MemoryClearStatus.
 * 
 *    Rev 1.3   24 Jul 2015 08:39:46   sesharma
 * Added function ConfigureResetForPHYandHilscher and SendResetToPhyAndHilscher for all three boards.
 * 
 *    Rev 1.2   08 Jul 2015 02:43:58   sesharma
 * At power up called ClearMemoryFrequency() to clear Stored memory frequency in all Horn Presets at power up.
 * 
 *    Rev 1.1   25 Jun 2015 05:53:56   rjamloki
 * Removed Amplitude Decay.  Added GPIO configuration for resetting PHY and Hilscher slave.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:46   tayars
 * Initial revision.
 * 
 *    Rev 1.48   13 Mar 2014 15:55:16   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 */
#include "WCTask.h"
#include "stdio.h"
#include "FIFO.h"
#include "string.h"
#include "ADC.h"
#include "GLOBALS.h"
#include "FUNC.h"
#include "Board.h"
#include "CPU.h"
#include "MIATask.h"
#include "LOG.h"
#include "AT24C64.h"
#include "24AA025E48.h"
#include "LOG.h"
#include "Version.h"
#include "CRC.h"
#include "SysConfig.h"
#include "Website.h"
#include "DebugNew.h"
#include "RTOS.h"
#include "LED.h"
#include "RTClock.h"
#include "TYP_DEF.h"
#include "ModBusSendTask.h"
#include "ModBusRcvTask.h"
#include "StateTable.h"
#include "Alarms.h"
#include "ShutDownTask.h"
#include "InputTask.h"
#include "FieldbusTask.h"
#include "FieldBus.h"
#include "ReceiveCommandHandler.h"
#include "DcxManagerHandler.h"
#include "FRAM.h"
#include "MII.h"

//#define pprintf
//#define printf

#define CABLE_DETECT_DEBOUNCE_TIME 10000  //10ms
#define FANENABLE_TEMP  60    //60 degC
#define ONE_SECOND   1000000
//define for soft reset bit in Basic Mode Control Register as per MII device's standard
#define MII_SOFTRESET_BIT 0x8000

bool FramCsDone = FALSE;
WCTask  * WCTask::thisPtr;
//Global pointer to MAC chip device object. It gets initialized
//in PowerupInitialization function.
EUI_24AA025E48 * MacdevicePtr;

//global object in RAM always contain the copy of MAC chip data.
MACCHIPRWData MacchipData;
BOOLEAN OverLoadAlarm = FALSE;

BOOLEAN MIARead;
BOOLEAN LCDDetect;
BOOLEAN DefaultFanStatus = FALSE;
SINT8  WCBootLoaderVersionLabel[BOOTSWVERSION_LEN];
UINT32 WCBootLoaderCRC;
UINT32 TimedSeekTimerTicks = 0;
extern UINT32 AvgPowerCount, AvgPower;
extern UINT32 * HornScanRawFrequency;
extern SINT16 * HornScanRawPhase;
extern SINT16 TempPeakPower;

/***Function Declaration****/
void ReadBootLoaderInfo(void);
void CheckforError(void);
void CheckForWDReset(void);
void CheckForVersionChange(void);

//Profibus Address to be displayed on webpage till next Power cycle.
UINT8 ProfibusAddr = 0;

/* void TimerInterruptCallback(void)
 *
 * Purpose:
 *    This is the callback function for Timer interrupt created in Main() function and it is called
 *    every TIMERINTERRUPT_INTERVALUS usec intervals.
 *    It invokes the MIATask, WCTask and ModbusSend Task by writing to the dummy queues of these tasks.
 *
 * Entry condition: None.
 *
 * Exit condition: None.
 */
void TimerInterruptCallback(void)
{
   if (RTOS::IsRunning())
	   MIATask::thisPtr->MIATaskQ.Write(0, 'i', 0);

	//Invoke MIA task, WC task and ModbusSendTask by writing to dummy queues
#if 0
	if ((RTOS::IsRunning())){
      MIATask::thisPtr -> MIATaskQ.Write(0, 'i', 0);
      WCTask::thisPtr -> WCTaskQ.Write(0, 'd', 0);
      if(PowerupCommunicationDone){
   		 if(!ModBusSendTask::thisptr -> ModBusRealTimeQ.Write(0, 'm', 0)){
   			printf("\nMBQOF");
   		 }
      }
      FbTaskMSTick();
   }
#endif
}

/*  void CheckForClockLost(void)
 *
 *  Purpose:
 *     This function is responsible for checking the CPU PLL Clock
 *     status. Write a Eveny in case of Clock Lost.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void CheckForClockLost(void)
{
   static BOOLEAN ClkLost = FALSE;
   if(MCF_CLOCK_PSR & MCF_CLOCK_PSR_LOCKS){
      if(ClkLost == FALSE){
    	  ASSERT("0");
          Log::WriteEventLog(EVENT_CLOCK_LOST);
          ClkLost = TRUE;
//          pprintf("\n clock lost");
       }
   }
}

/*  void CheckForPowerandLCDSensor(void)
 *
 *  Purpose:
 *    This function is used to check whether 24V signal and LCD are present or not
 *    It is called by Run() of WC task.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    if 24V is not present,RTOSShutDown flag is SET.
 */
void CheckForPowerandLCDSensor(void)
{
   static BOOLEAN SensorTest = FALSE, LCDdetectFlag = FALSE;
   if(!CHECK_FOR_24V){
      // Set the flag and ShutDownTask will take care of this when ever it will get the time.
      ShutDownTask::RTOSShutDown = TRUE;
      if(SensorTest == TRUE){
         printf("\n\n Loosing 24 Volt");
         SensorTest = FALSE;
      }
   }
   else if(SensorTest == FALSE){
      SensorTest = TRUE;
   }
   CheckForClockLost();
   LCDDetect = CHECK_LCD_PREESENT;
   if((LCDDetect == TRUE) && (LCDdetectFlag == FALSE) && (ReceiveCommandHandler::thisptr->SystemInfoReceived == TRUE)){
      LCDdetectFlag = TRUE;
      UINT16 LCDcrc = ((ReceiveCommandHandler::LCDCodeCRCHigh << 8) | ReceiveCommandHandler::LCDCodeCRCLow);
      Log::WriteEventLog(EVENT_LCD_BOARD_DETECTED, LCDcrc, ReceiveCommandHandler::LCDSWVersion);
   }
   if(LCDDetect == FALSE){
	   LCDdetectFlag = FALSE;
	   AlarmQueueWrite(HW04);
   }
}

/****************************************************************************/
/* Description:-                                                            */
/* WCTask Constructor                     									*/
/****************************************************************************/
WCTask::WCTask(MIATask * MIAadc) :
	Task("DUPS", WCTASK_STACKSIZE), WCTaskQ(WCTASK_FIFOSIZE, "DupsQ")
{
	AdcTask = MIAadc;
	thisPtr = this;
}

/****************************************************************************/
/* Description:-                                                            */
/* DupsTask destructor                     									*/
/****************************************************************************/
WCTask::~WCTask()
{

}

/*  void WCTask::Run(void)
 *
 *  Purpose:
 *    This function is the Run function of WC task.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void WCTask::Run(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function reads digital and analog inputs from ADC channels,         */
/* MI memory mapped I/O ,from FPGA and processes control loops.             */
/****************************************************************************/
{
   char I;
   BOOLEAN OldCableDetect = FALSE, CurrCableDetect, PowerUpCableDetect = FALSE;
   UINT32 DebouncedTime = 0;
   DelayMs(300);
   CyGOS::InstallTimer(TIMERINTERRUPT_INTERVALUS, TimerInterruptCallback, 1);    // to initialize and register timer interrupt.
   CyGOS::InitWatchDog(AdvanceConfigData->Advrnd.Gp2Value1);
   CyGOS::EnableWatchDog();
   DelayMs(200);
   SelectWCSequence();
   while(FOREVER){  //Forever loop
      //Wait for wakeup message.
      WCTaskQ.Read(this, I, -1);
      CycleRun();
      if(PowerupCommunicationDone)
	  {
		 if(!ModBusSendTask::thisptr -> ModBusRealTimeQ.Write(0, 'm', 0))
		 {
			printf("\nMBQOF");
		 }
	  }
      CheckForPowerandLCDSensor();
      if(!WebSite::CheckForDiagnosticFlag()){
         UserIO::SetOutputAmplitude(RtReceiveData.AmplitudeOut);
         UserIO::SetOutputPower(RtReceiveData.PowerOut);
         UserIO::SetOutputFrequency(RtReceiveData.FreqOut);
      }
      //see if cable detect is selected
      CurrCableDetect = UserIO::CheckForCableDetectEanbled();
      if(CurrCableDetect){//cable detection is enabled
         //if cable detection is enabled check the cable detect signal
         CurrCableDetect = UserIO::CheckForCableDetect();
         if(CurrCableDetect != OldCableDetect){
            DebouncedTime += WCTIME;
            if(DebouncedTime > CABLE_DETECT_DEBOUNCE_TIME){
               DebouncedTime = 0;
               OldCableDetect = CurrCableDetect;
               Log::WriteEventLog(CurrCableDetect? EVENT_IO_CABLE_DETECTED:EVENT_IO_CABLE_NOTDETECTED, DigInPins[CableDetectedPin]);
               if(CurrCableDetect == FALSE)
                  AlarmQueueWrite(EQ07);
            }
            PowerUpCableDetect = TRUE;
         }
         else if(PowerUpCableDetect == FALSE){
            Log::WriteEventLog(CurrCableDetect? EVENT_IO_CABLE_DETECTED:EVENT_IO_CABLE_NOTDETECTED, DigInPins[CableDetectedPin]);
            if(CurrCableDetect == FALSE)
               AlarmQueueWrite(EQ07);
            PowerUpCableDetect = TRUE;
         }
         if(CurrCableDetect == OldCableDetect)
            DebouncedTime = 0;
      }
      DCPTimeSyncCounter += TIMERINTERRUPT_INTERVALUS;
      if(DCPTimeSyncCounter >= NUSINMINUTE)
    	  SendRTCSyncCommandToDCP();
     // UpdateTPVal(priority);
      MIATask::thisPtr->MIATaskQ.Write(0, 'b', 0);
	  if(FieldbusTask::thisPtr){    //If fieldbus task is running
		 PrepareFBStatus();
		 FieldbusTask::thisPtr->WaitForWCTask.Write(0, 'a', 0);
	  }

   }
}

void WCTask::CycleRun(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for all the power supply control work.It    */
/* shall read digital and analog inputs from MI memory mapped               */
/* I/O, FPGA and process them to produce control signals to FPGA. It also   */
/* update various counters like cycle, error, PowerOnSec,PowerOnMilliSec,   */
/* SonicsOnSec,SonicsOnMilliSec in FRAM.                                    */
/****************************************************************************/
{
   static UINT32 SonicOnCnt = 0, PowerOnCount = 0;
   if (WebIOEnabled == FALSE) {
		if ((CurrentIndex >= WAITULSTRS_IDX) && (CurrentIndex < CYCLE_ABORT_IDX) &&
				(UserIO::CheckForCycleAbort() == TRUE)) {
			CycleAbortFlag = TRUE;
			AlarmQueueWrite(CM02);
		}
		if((ActuatorPresentAtPwrUp == TRUE) && ((CurrentIndex > WAITULSTRS_IDX) &&
			(CurrentIndex < CYCLE_ABORT_IDX)) && (UserIO::CheckForULS() == TRUE)){
			UserIO::SetActuaterHome(TRUE);
			if(UserIO::CheckForTrigger() == TRUE)
				AlarmQueueWrite(EQ02);
			if(CurrentIndex < CHECK_LIMIT_IDX)
				AlarmQueueWrite(EQ12);
		}
	}
   if((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) == MB_SONICACTIVE_OUT){
      SonicOnCnt += WCTIME;
      if(SonicOnCnt > ONE_SECOND){
    	  SysCounters->SonicsOnHours += 1;
         SonicOnCnt = 0;
      }
   }
   PowerOnCount += WCTIME;
   if(PowerOnCount > ONE_SECOND){
	   SysCounters->PowerOnHours += 1;
      PowerOnCount = 0;
   }

   /* Check for Timed Seek option from preset*/
   if(ParameterSet[CurrPreset].DcpParam.TimedSeekFlag && !WebIOEnabled && ((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) != MB_SONICACTIVE_OUT)){
      TimedSeekTimerTicks += WCTIME;
      if(TimedSeekTimerTicks >= MAX_TIMER_COUNTER){
         TimedSeekTimerTicks = 0;
         ControlVariable |= TIMEDSEEK;
      }
   }
   else{
      TimedSeekTimerTicks = 0;
      ControlVariable &= ~TIMEDSEEK;
   }
   //Need to avoid adding offset during after burst state of cycle
   if((CurrentIndex <= PRESET_UPDATE_IDX) || (CurrentIndex >= ERROR_IDX))
	   UpdateFrequencyOffset();

   CheckforError();
   StateMachine();
   RtSendData.MemoryValue = HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP +
  		   HsParameterSet[CurrHsPreset].DigitalTuneScaledUP;
   InputTemperature = RtReceiveData.Temperature;
   if((InputTemperature > FANENABLE_TEMP) && (WebIOEnabled != TRUE)){
      UserIO::FanEnable(TRUE);
      DefaultFanStatus = TRUE;
   }

}

/* void ConfigureResetForPHYandHilscherWC991 (void)
 * Purpose: To create a PlaceHolder for function pointer assignment.
 * Entry Condition:
 * 		Once at Reset
 * Exit Condition: 
 *		None.
*/
void ConfigureResetForPHYandHilscherWC991 (void)
{
	//Do nothing as no Reset line connected to PHY and Hilscher card on WC 991 board.
}

/* void ConfigureResetForPHYandHilscherSC1054 (void)
 * Purpose: This function Configure Reset line B7 for Ethernet PHYs and Hilscher connected to SC1054 board.
 * 		It is called from PowerUpInitialization() function.
 * Entry Condition:
 * 		Once at Reset.
 * Exit Condition:
 * 		B7 Configured as GPIO.
*/
void ConfigureResetForPHYandHilscherSC1054 (void)
{
	   //Configure Reset line added for Ethernet PHYs and Hilscher
	   MCF_PAD_PAR_PCI &= ~MCF_PAD_PAR_PCI_PAR_GNT3_GNT3;	//Configure as GPIO
	   MCF_GPIO_PDDR_PCI |= MCF_GPIO_PDDR_PCI_PDDR_PCI7;	//Configure as Output
	   MCF_GPIO_PODR_PCI |= MCF_GPIO_PODR_PCI_PODR_PCI7;	//Drive High to keep reset line inactive
}

/* void ConfigureResetForPHYandHilscherWC1169 (void)
 * Purpose: This function Configure Reset line B7 for Ethernet PHYs and Hilscher connected to WC1169 board.
 * 		It is called from PowerUpInitialization() function.
 * Entry Condition:
 * 		Once at Reset.
 * Exit Condition:
 *		B7 Configured as GPIO.
*/
void ConfigureResetForPHYandHilscherWC1169 (void)
{
	   //Configure Reset line added for Ethernet PHYs and Hilscher
	   MCF_PAD_PAR_PCI &= ~MCF_PAD_PAR_PCI_PAR_GNT3_GNT3;	//Configure as GPIO
	   MCF_GPIO_PDDR_PCI |= MCF_GPIO_PDDR_PCI_PDDR_PCI7;	//Configure as Output
	   MCF_GPIO_PODR_PCI |= MCF_GPIO_PODR_PCI_PODR_PCI7;	//Drive High to keep reset line inactive
}


/* void SendResetToPhyAndHilscherWC991 (void)
 * Purpose: To create a PlaceHolder for function pointer assignment.
 * Entry Condition:
 * 		Once at Software or Internal Watch dog Reset.
 * Exit Condition:
 *		None.
*/
void SendResetToPhyAndHilscherWC991(void)
{
	//Do nothing as no Reset line connected to PHY and Hilscher card on WC 991 board.
}


/* void SendResetToPhyAndHilscherSC1054 (void)
 * Purpose: This function sends reset pulse to Hilscher and PHY devices on SC1054 board.
 * 		It is called from PowerUpInitialization() function.
 * Entry Condition:
 * 		Once at Power up if reset reason is not power on reset.
 * Exit Condition:
 *		Reset pulse sent to Hilscher card and PHY chip.
*/
void SendResetToPhyAndHilscherSC1054(void)
{
	MCF_GPIO_PODR_PCI &= ~MCF_GPIO_PODR_PCI_PODR_PCI7;	//Drive Low
	RTOS::DelayMs(50);
	MCF_GPIO_PODR_PCI |= MCF_GPIO_PODR_PCI_PODR_PCI7;	//Drive High
}

/* void SendResetToPhyAndHilscherWC1169 (void)
 * Purpose: This function sends reset pulse to Hilscher and PHY devices on WC1169 board.
 * 		It is called from PowerUpInitialization() function.
 * Entry Condition:
 * 		Once at Power up if reset reason is not power on reset.
 * Exit Condition:
 *		Reset pulse sent to Hilscher card and PHY chip.
*/
void SendResetToPhyAndHilscherWC1169(void)
{
   //Below code is added to soft reset the LAN8710 chip.
   //Read current Basic Mode Control Register value from LAN8710
   SINT32 BMCRValue = Board::GetMII(ETHERNET_INTERFACE_WCTODCP)->ReadRegister(miiBMCR);
   BMCRValue |= MII_SOFTRESET_BIT;//Writing 1 to bit 15 of BMCR register soft resets the LAN8710 chip
   Board::GetMII(ETHERNET_INTERFACE_WCTODCP)->WriteRegister(miiBMCR, BMCRValue);//write to soft reset the LAN chip.
   //KSZ8863RLL chip and Hilscher cards are reset by sending a reset pulse on GPIO
   MCF_GPIO_PODR_PCI &= ~MCF_GPIO_PODR_PCI_PODR_PCI7; //Drive Low
   RTOS::DelayMs(50);
   MCF_GPIO_PODR_PCI |= MCF_GPIO_PODR_PCI_PODR_PCI7; //Drive High
}


/*  void SetDefaultMACCHIPRWData(void)
 *
 *  Purpose:
 *    This function is called to set default values in MAC chip RW area. Right now it does save only serial number.
 *    This function is called from PowerupInitialization() function when checksum of data saved in data area is not found correct.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void SetDefaultMACCHIPRWData(void)
{
   UINT16 Crc = 0;
   strcpy((char *)MacchipData.SerialNo, DEFAULT_SERIAL_NUMBER);
   MacchipData.CurrSystemType = DEFAULT_SYSTEM;
   MacchipData.Freq = DEFAULT_FREQ;
   MacchipData.Power = DEFAULT_POWER;
   Crc = ByteCRC((UINT8 *)&MacchipData, sizeof(MACCHIPRWData) - sizeof(MacchipData.CheckSum));
   MacchipData.CheckSum = Crc;
   //Use Write function of EUI_24AA025E48 class to save defaults values.
   MacdevicePtr -> Write(MACCHIP_RWDATAAREA_STARTADDR, (UINT8 *)&MacchipData, sizeof(MacchipData));
}

/*  void PowerUpInitialization(void)
 *
 *  Purpose:
 *    This function is called once at power up from main()function.
 *    It does call the functions to setup flex bus interface to access various memory mapped devices connected to WC CPU.
 *    It calls the functions to initialize various software modules.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void PowerUpInitialization(void)
{
   UINT32 ERomDataChecksumFailed = 0;
   int TempLen = 0;
   UINT16 Chksum = 0;
   //Setup flexbus interface for FRAM
   CPU::SetupChipSelect(CHIPSELCT_PIN1, WAITSTATES_FRAM_CS1, DATABUSWIDTH_FRAM_CS1, (void *)FRAM_BASEADDRESS);
   FramCsDone = true;
   //Setup flexbus interface for DPRAM
   CPU::SetupChipSelect(CHIPSELCT_PIN2, WAITSTATES_DPRAM_CS2, DATABUSWIDTH_DPRAM_CS2, (void *)HILSCHERDPRAM_BASEADDRESS);
   //Setup flexbus interface for Memory mapped IO
   CPU::SetupChipSelect(CHIPSELCT_PIN3, WAITSTATES_DIGITALIO_CS3, DATABUSWIDTH_DIGITALIO_CS3, (void*) MI_DIGITALIO_BASEADDRESS);
   printf("\n Initializing MAC chip");
   //Allocate the object of to access MAC chip and assign the returned pointer to global pointer macdevicePtr;
   MacdevicePtr = new EUI_24AA025E48(Board::GetI2C(0, 5, 5, I2C_CLK), MACCHIIP_I2C_ADDRESS);
   printf("\n MAC chip Init done1");
   TempLen = sizeof(MACCHIPRWData);
   //Use MAC chip device Read function to read Serial number.
   MacdevicePtr -> Read(MACCHIP_RWDATAAREA_STARTADDR, (unsigned char *)&MacchipData, TempLen, NUMBER_OF_RETRIES);
   printf("\n MAC chip Init done2");
   //ByteCRC to calculate the checksum of bytea in macchipData object excluding Checksum field.
   Chksum = ByteCRC((UINT8 *)&MacchipData, sizeof(MACCHIPRWData) - sizeof(MacchipData.CheckSum));
   printf("\n MAC chip Init done3");
   if((TempLen < 0) || (Chksum != MacchipData.CheckSum)){
	  printf("\n MAC chip Init done4");
	  ERomDataChecksumFailed = Chksum;//Store wrong check sum for logging purpose
      printf("\n MAC chip default");
      SetDefaultMACCHIPRWData();
   }
   printf("\n Power Up Init 6");
   UpdateMidBandAndMinMaxFreq();//Frequency of system shall be known before FRAM check handler called
   printf("\n Power Up Init 4");
   RTClock::InitRTC();// Make sure RTC is initialized before we write any events.
   //Fram Test handler shall run here
   FramCheck();
   //Log the Eprom Data Checksum failed error.
   if(ERomDataChecksumFailed > 0)
   {
	   Log::WriteEventLog(EVENT_CRC_ERROR, MacchipData.CheckSum, ERomDataChecksumFailed, CRC_ERROR_MACCHIPRWDATA);
   }

   printf("\n Power Up Init 3");
   InitGlobalBuffers();
   printf("\n Power Up Init 5");
   Log::Init();               // to initialize Log module.
   printf("\n Power Up Init 7");
   UserIO::InitUserIO();         // to initialize user IO module.
   printf("\n Power Up Init 8");

   if((Sysconfig->DcpSpeedControl > SPEEDCONTROL_MAX) ||
 		   (Sysconfig->DcpSpeedControl < SPEEDCONTROL_STANDARD)){//just an extra caution
 	   Sysconfig->DcpSpeedControl = SPEEDCONTROL_STANDARD;
 	   SetColdStartPresetValues();
 	   printf("DcpSpeedControl Set to default");
   }
   printf("\n Sysconfig->DcpSpeedControl %d", Sysconfig->DcpSpeedControl);
   InitFreqDependentParametersMinMaxDefault();
   printf("\n Power Up Init 9");
   InitPasscodeConfiguration();// to initialize passcode configuration.
   // Write Power on Event in LOG with data1 is the value denoting reason of last powerdown.
   // Note- include CpuDefs.h file for the definition of MCF_RCM_RSR.
   // Refer to MCF5445x manual for the description of MCF_RCM_RSR //i.e. Reset status register
   Log::WriteEventLog(EVENT_POWERON, MCF_RCM_RSR, Sysconfig->PowerupOption, BoardType);		// Send Board type at power up to log event
   CheckForWDReset();
   UpdateFlashCRC();

   if((CheckPresetCRC(Preset_Default) == FALSE) || (CheckPresetCRC(Preset_MIN) == FALSE)
		   || (CheckPresetCRC(Preset_MAX) == FALSE))
	   SetColdStartPresetValues();

   CheckPresetCRC(CurrPreset);
   UpdateRunningPreset();
   ReadBootLoaderInfo();
   CheckForVersionChange();
   SetMinMaxCycleParam();
   SetDCPTimeControlParams();
   // Delay is inserted to handle RTC initialization in case battery in not present.
   RTOS::DelayMs(500);
   CheckForRTCBattery();
   CustomDefaultUSERIOConfig = *USERIOConfig;   //Custom defaults same as Branson Defaults on PowerUp
   DCXFunc.MFCycleStart = CurrentCycleParam->MfCycling;
   ControlVariable = 0;
   if(TRUE == Sysconfig->ClearMemoryAtPowerUp)
	   ClearMemoryFrequencyAtPowerup();
   if(Sysconfig->PowerupOption == POWERUP_SEEK){
	   ControlVariable |= POWERUPSEEK;
   }
   else if(Sysconfig->PowerupOption == POWERUP_SCAN){
	   ControlVariable |= POWERUPSCAN;
   }
   if((AdvanceConfigData->Advrnd.Gp2Value1 > WD_MAX)||
 		  (AdvanceConfigData->Advrnd.Gp2Value1 < WD_MIN))
 	  AdvanceConfigData->Advrnd.Gp2Value1 = WD_DEFAULT;

   //Read Profibus address
   ProfibusAddr = Sysconfig->PbAddress;

	ConfigDigitalOutputLatch();//Digital outputs need to be enabled here so that cold start based on jumper works properly.
							   //See CheckForColdStart function in Main which is called after PowerUpInitialization is returned
	ConfigureResetPHYandHilscher();//configure the GPIOS for reset line going to 
	//Here Reset the Phy chips and hilscher card here if required.
	if(!(MCF_RCM_RSR & MCF_RCM_RSR_POR))//any thing other than power on reset 
		SendResetToPHYandHilscher();
}

/*  void ReadMACAddressFromMacChip(UINT8 * MacPtr)
 *
 *   Purpose:
 *    This function is called once at power up from Main() function.
 *    It does read the MAC from on board MAC chip.
 *
 *   Entry condition:
 *    MacPtr - Pointer to byte array where MAC address should be copied.
 *    (Byte array should at least be of MAC_ADDRESS_LEN bytes. No overflow condition checked in this function)
 *
 *   Exit condition:
 *    None.
 */
void ReadMACAddressFromMacChip(UINT8 * MacPtr)
{
   int TempLen = MAC_ADDRESS_LEN;
   //Use the Read function of EUI_24AA025E48 to read the MAC address.
   MacdevicePtr -> Read(MACSTART_ADDRESS, MacPtr, TempLen, NUMBER_OF_RETRIES);
   if(TempLen < 0){
	   memcpy(MacPtr, DEFAULT_MAC_ADDR, MAC_ADDRESS_LEN);
       Log::WriteEventLog(EVENT_ERROR_READMACADDR);
   }
}


/*  void StoreWeldData(void)
 *
 *  Purpose:
 *    This function is responsible for collecting Weld points during weld/test/seek. This function is
 *    called from ProcessModBusReply() function of ModBusRcvTask.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void StoreWeldData(void)
{
    if(((RtReceiveData.StatusFlags & SIGNAL_COLLECTWELDPOINT) == SIGNAL_COLLECTWELDPOINT) &&
   		(CurrentIndex >= SONICS_START_IDX) && (CurrentIndex != AFTER_BURST_IDX)){ //check for weld point data
     WeldBufferIndex = RtReceiveData.Reserved16;//((WeldBufferIndex + 1) % SIZE_WELDBUFFER); // Circular Wrap
     static SINT16 OldIndex = -1;
     if((WeldBufferIndex >= 0) && (WeldBufferIndex < SIZE_WELDBUFFER)){
        if(OldIndex > WeldBufferIndex)
           OldIndex = WeldBufferIndex - 1;
        for(;OldIndex < WeldBufferIndex;){
           OldIndex = ((OldIndex + 1) % SIZE_WELDBUFFER);
           WeldBuffer[OldIndex].Time = OldIndex;
           WeldBuffer[OldIndex].Frequency = RtReceiveData.FreqOut;
           WeldBuffer[OldIndex].PWMValue = RtReceiveData.PWM;
           WeldBuffer[OldIndex].NominalAmplitude = RtReceiveData.NominalAmplitude;
           WeldBuffer[OldIndex].ActualAmplitude = RtReceiveData.AmplitudeOut;
           WeldBuffer[OldIndex].Current = RtReceiveData.Current;
           WeldBuffer[OldIndex].Power = RtReceiveData.PowerOut;
           WeldBuffer[OldIndex].Phase = RtReceiveData.Phase;
           WeldBuffer[OldIndex].logValid = TRUE;
           WeldBufferTime++;
           JsonWeldBuffTime++;
           if (CurrentIndex < SEEKSTATE_IDX){
        	    if((JsonWeldBuffTime == 1) && (SampleRate > 1))
        	    	WriteWeldDatForDCXM(&WeldBuffer[OldIndex], JsonWeldBuffTime);//need first point in any case
        	    else if (SampleRate > 0) {
					if ((JsonWeldBuffTime % SampleRate) == 0)
						WriteWeldDatForDCXM(&WeldBuffer[OldIndex], JsonWeldBuffTime);
				}
			}
		}
     }
     AvgPower += RtReceiveData.PowerOut;
     AvgPowerCount++;
   }
    if(WebSite::CheckForDiagnosticFlag() == FALSE){
    	UserIO::SetSonicsActive((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) == MB_SONICACTIVE_OUT);
    	UserIO::SetSeekOut((RtReceiveData.DigOutputs & MB_SEEKSCAN_OUT) == MB_SEEKSCAN_OUT);
    }
}

/*  void CollectScanPoints(void)
 *
 *  Purpose:
 *    This function is used to collect scan point data when scan is in progress.
 *    This function is called by ModBusRcvTask::ProcessModBusReply().
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void CollectScanPoints(void)
{
//   CriticalSection cs;
   BOOLEAN UpdateScanBuffer = TRUE;
   if((RtReceiveData.StatusFlags & SIGNAL_COLLECTHORNSCANPOINT) == SIGNAL_COLLECTHORNSCANPOINT){
      if(NumScanPoints >= 0){
    	 UpdateScanBuffer = FALSE;
         if(RtReceiveData.FreqOut != ScanBuffer[NumScanPoints].Frequency)
            UpdateScanBuffer = TRUE;
      }
      if(UpdateScanBuffer == TRUE){
         NumScanPoints = ((NumScanPoints + 1) % SIZE_SCANBUFFER);
         ScanBuffer[NumScanPoints].Frequency =  RtReceiveData.FreqOut;
         HornScanRawFrequency[NumScanPoints] = ScanBuffer[NumScanPoints].Frequency;
         ScanBuffer[NumScanPoints].PWMValue  = RtReceiveData.PWM;
         ScanBuffer[NumScanPoints].Amplitude = RtReceiveData.AmplitudeOut;
         ScanBuffer[NumScanPoints].Current   = RtReceiveData.Current;
         ScanBuffer[NumScanPoints].Power = RtReceiveData.PowerOut;
         ScanBuffer[NumScanPoints].Phase = RtReceiveData.Phase;
         ScanBuffer[NumScanPoints].Index = RtReceiveData.Reserved16;
         HornScanRawPhase[NumScanPoints] = RtReceiveData.Phase;
      }
   }
}

/*  void CheckforError(void)
 *
 *  Purpose:
 *    This function is used to continuously monitor the status of Different flags required by various states of state machine.
 *    This function is called by WCTask::CycleRun().
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void CheckforError(void)
{
   static BOOLEAN GeneralAlarm = FALSE;
   DCPReadyFlag = ((RtReceiveData.DigOutputs & MB_READY_OUT) == MB_READY_OUT);
   if((RtReceiveData.DigOutputs & MB_OVERLOADALARM_OUT) == MB_OVERLOADALARM_OUT){
      if(OverLoadAlarm == FALSE){
         OverLoadAlarm = TRUE;
         CreateOverloadAlarm(RtReceiveData.ErrorReason);
      }
   }
   else
      OverLoadAlarm = FALSE;
   if((RtReceiveData.DigOutputs & MB_GENERALALARM_OUT) == MB_GENERALALARM_OUT){
      if(GeneralAlarm == FALSE){
         GeneralAlarm = TRUE;
         CreateDCPGeneralAlarm(RtReceiveData.ErrorReason);
      }
   }
   else
      GeneralAlarm = FALSE;
}

/*  void ModifyMACCHIPRWData(void)
 *
 *  Purpose:
 *    This function Set the values already modified in global MACCHIPRWData object into 24AA025E48 chip.
 *    This function is called by Website Module.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void ModifyMACCHIPRWData(void)
{
   UINT16 Crc = 0;
   Crc = ByteCRC((UINT8 *)&MacchipData, sizeof(MACCHIPRWData) - sizeof(MacchipData.CheckSum));
   MacchipData.CheckSum = Crc;
   //Use Write function of EUI_24AA025E48 class to save defaults values.
   MacdevicePtr -> Write(MACCHIP_RWDATAAREA_STARTADDR, (UINT8 *)&MacchipData, sizeof(MacchipData));
}

void SonicsOff(void)
{
	RtSendData.CmdFlags &= ~MB_START;
}

/*  void UpdateMidBandAndMinMaxFreq(void)
 *
 *  Purpose:
 *     This function updates the Midband and MinMax frequencies according to the frequency of PowerSupply.
 *     This function gets called on powerup from Main() function
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
void UpdateMidBandAndMinMaxFreq(void)
{
	 switch (MacchipData.Freq){
	      case FREQ_20KHZ:
	         Midband = F20KHZMIDBAND;
	         MaxFrequency = (F20KHZMAXABS);
	         MinFrequency = (F20KHZMINABS);
	         MaxOffset = FREQOFFSET20K_MAX;
	      break;
	      case FREQ_30KHZ:
	         Midband = F30KHZMIDBAND;
	         MaxFrequency = (F30KHZMAXABS);
	         MinFrequency = (F30KHZMINABS);
	         MaxOffset = FREQOFFSET30K_MAX;
	      break;
	      case FREQ_40KHZ:
	         Midband = F40KHZMIDBAND;
	         MaxFrequency = (F40KHZMAXABS);
	         MinFrequency = (F40KHZMINABS);
	         MaxOffset = FREQOFFSET40K_MAX;
	      break;
	      default:
	         Midband = F20KHZMIDBAND;
	         MaxFrequency = (F20KHZMAXABS);
	         MinFrequency = (F20KHZMINABS);
	         MaxOffset = FREQOFFSET20K_MAX;
	      break;
	   }
}

/*  void ReadBootLoaderInfo(void)
 *
 *  Purpose:
 *    This function reads version string and CRC of Bootloader code from predefined FRAM locations and copy
 *    the same to PowerUpData object which is to be sent to WC.
 *    This function is called once from PowerUpInitialization() function.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void ReadBootLoaderInfo(void)
{
   UINT8 * FramBase = (UINT8 *)FRAM_BASEADDRESS;
   memcpy(WCBootLoaderVersionLabel, (FramBase + BOOTLOADERVERSIONOFFSET_FRAM) ,BOOTSWVERSION_LEN);
   memcpy(&WCBootLoaderCRC, (FramBase + BOOTLOADERCRCOFFSET_FRAM) ,sizeof(PowerUpData.BootLoaderCRC));
}

/*  void CheckForVersionChange(void)
 *
 *  Purpose:
 *    This function checks for the version change of WC software by comparing he one in FRAM to the one saved in static
 *    global variable SWVersionLabel. This function is called from PowerUpInitialization() function.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *     Returns TRUE if version is changed else return FALSE.
 */
void CheckForVersionChange(void)
{
   UINT32 VersionChangedstatus = (UINT32)memcmp(FramInfo->WCSoftwareVersionLabel, SWVersionLabel, SWVERSION_LEN);
   if(VersionChangedstatus != 0){
    Log::WriteEventLog(EVENT_VERSION_CHANGED,
    ((SWVersionLabel[3]<<24)|(SWVersionLabel[4]<<16) | (SWVersionLabel[5] << 8)| (SWVersionLabel[6])),
    ((FramInfo->WCSoftwareVersionLabel[3]<<24)|(FramInfo->WCSoftwareVersionLabel[4] << 16) | (FramInfo->WCSoftwareVersionLabel[5] << 8) | (FramInfo->WCSoftwareVersionLabel[6]))
    );
     memcpy(FramInfo->WCSoftwareVersionLabel, SWVersionLabel, SWVERSION_LEN);
   }
}
/*  void UpdateFlashCRC(void)
 *
 *  Purpose:
 *    This function calculates Flash CRC and update it in System Configuration to check for CRC change at next powerup.
 *    This function is called once at powerup from CheckHWConfiguration function().
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void UpdateFlashCRC(void)
{
   UINT16 FlashCRC = 0;
   SINT32 CodeSize = (SINT32)__TEXT_END - (SINT32)__TEXT_START;
   SINT32 FlashOffset = (SINT32)__TEXT_START - (SINT32)__FLASH_RAM;
  
   printf("\n code size %d flash offset %d flash start %08X", (int)CodeSize, (int)FlashOffset, (unsigned int)(__FLASH_ROM + FlashOffset));
   FlashCRC = ByteCRC((UINT8 *)(__FLASH_ROM + FlashOffset), (UINT32)CodeSize);
   if(Sysconfig->DCXCRC != FlashCRC)
      Log::WriteEventLog(EVENT_POWERON_CRC_CHANGED, (UINT16)Sysconfig->DCXCRC, FlashCRC);

   printf("\n code CRC is %d", FlashCRC);
   Sysconfig->DCXCRC = FlashCRC;
}

/*  void InitGlobalBuffers(void)
 *
 *  Purpose:
 *    This function is used to initialize all the global buffers used.
 *    This function is called once at PowerUpInitialization() function.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void InitGlobalBuffers(void)
{
   HornScanRawFrequency = new UINT32[SIZE_SCANBUFFER];
   HornScanRawPhase = new SINT16[SIZE_SCANBUFFER];
   WeldBuffer = new WeldData[SIZE_WELDBUFFER];
   ScanBuffer = new ScanData[SIZE_SCANBUFFER];
   memset(HornScanRawFrequency,0,SIZE_SCANBUFFER*4);
   memset(HornScanRawPhase,0,SIZE_SCANBUFFER*2);
   memset(WeldStatusSet , 0, sizeof(WeldStatusSet));
}

/*  void PrepareFBStatus(void)
 *
 *  Purpose:
 *    This function is used to prepare Fieldbus Status if fieldbus task is running.
 *    This function is called by Run() of WC Task.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void PrepareFBStatus(void)
{
	SINT16 CurrIndx;
	CurrIndx = CurrentIndex;
	if(Sysconfig->ActivePreset < MAXIMUM_USER_PRESETS)
		FBStat.SelectedParamSet = Sysconfig->ActivePreset;
	else
		FBStat.SelectedParamSet = 0;

	if(Sysconfig->RFSwitch.RFSwitchingType != RFSWITCHING_FB){
		if(ParameterSet[Sysconfig->ActivePreset].InfoParam.HornPresetNumber < MAXIMUM_USER_HSPRESETS)
			FBStat.HornNumber = ParameterSet[Sysconfig->ActivePreset].InfoParam.HornPresetNumber;
		else
			FBStat.HornNumber = 0;
	}


	FBStat.ReadyForUs = (CurrIndx == READYSTATE_IDX);
	FBStat.UsActive = (((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) == MB_SONICACTIVE_OUT)||
			(UserIO::CheckForCycleRunning() == TRUE));
	FBStat.MemoryClearStatus = MemoryReset;//use the flag MemoryReset to send the memory cleared information to Fieldbus
										   //This is the same flag which is used to send the same information to LCD
}

/*  BOOLEAN GetAccessToken(CommIfc CalledFromCommIfc)
 *
 *  Purpose:
 *    This function is used to get Token if its free.
 *    This function is called by Profibus Task.
 *
 *  Entry condition:
 *    CalledFromCommIfc: Communication Interface.
 *
 *  Exit condition:
 *    If Token is Accessed, returns TRUE else returns FALSE.
 */
BOOLEAN GetAccessToken(CommIfc CalledFromCommIfc)
{
   BOOLEAN RetVal = FALSE;
   SINT16 CurrIndx;
   CurrIndx = CurrentIndex;
   if((CurrIndx == READYSTATE_IDX) || (CurrIndx == ERROR_IDX)){
	   if ((CommIfcAccessToken == COMM_IFC_NO_IFC) || (CommIfcAccessToken == CalledFromCommIfc)) {
		  /*Access Token free, Token to "eCalledFromCommIfc" */
		  CommIfcAccessToken = CalledFromCommIfc;
		  RetVal = TRUE;
	   }
   }
   return RetVal;
}

/*  BOOLEAN IsAccessTokenAvailable(void)
 *
 *  Purpose:
 *    This function is used to check whether token is available or not.
 *    This function is called before taking any action based on USERIO/Webpage command.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    If Token is available, returns TRUE else returns FALSE.
 */
BOOLEAN IsAccessTokenAvailable(void)
{
	return (CommIfcAccessToken == COMM_IFC_NO_IFC);
}

/*  BOOLEAN IsManualFunctionAvailable(void)
 *
 *  Purpose:This function is used to find the operation mode.

 *  Entry condition:
 *    None.
 *
 *  Exit condition: Operation mode i.e. MANUAL OR NOMODE
 */
BOOLEAN IsManualFunctionAvailable(void)
{
	return ((OpMode == MANUAL) || (OpMode == NOMODE));
}

/*  BOOLEAN ReleaseAccessToken(CommIfc CalledFromCommIfc)
 *
 *  Purpose:
 *    This function is used to release Token.
 *    This function is called by Profibus Task.
 *
 *  Entry condition:
 *    CalledFromCommIfc: Communication Interface.
 *
 *  Exit condition:
 *    If Token is released, returns TRUE else returns FALSE.
 */
BOOLEAN ReleaseAccessToken(CommIfc CalledFromCommIfc)
{
   BOOLEAN RetVal = FALSE;
   if ((CommIfcAccessToken == COMM_IFC_NO_IFC) || (CommIfcAccessToken == CalledFromCommIfc)) {
     /*Access Token free, Token to "eCalledFromCommIfc" */
     CommIfcAccessToken = COMM_IFC_NO_IFC;
     RetVal = TRUE;
   }
   return RetVal;
}


/*  void CheckForWDReset(void)
 *
 *  Purpose:
 *     This function checks whether reset is done by watchdog. If so its the
 *     the event in event log.
 *     This function is called once at powerup from PowerUpInitilization() function.
 *
 *   Entry condition: None.
 *
 *   Exit condition: None.
 */
void CheckForWDReset(void)
{
   // Write Power on Event in LOG
   // check for watch dog reset
   if(MCF_RCM_RSR & MCF_RCM_RSR_WDRCORE)
      Log::WriteEventLog(EVENT_WATCHDOG_RESET);
}

/*  void SetDCPTimeControlParams(void)
 *
 *  Purpose:
 *     This function is called once at powerup from PowerUpInitilization() function.
 *
 *   Entry condition: None.
 *
 *   Exit condition: None.
 */
void SetDCPTimeControlParams(void)
{
	if(Sysconfig->DcpSpeedControl == SPEEDCONTROL_CUSTOM){
		//DCP Timer Interrupt Time
		if((AdvanceConfigData->Advrnd.DCPTimerRate >= DCP_TIMERINTERRUPTUS_MIN)
				&& (AdvanceConfigData->Advrnd.DCPTimerRate <= DCP_TIMERINTERRUPTUS_MAX))
			DCPTimeControlParam.DCPTime = AdvanceConfigData->Advrnd.DCPTimerRate;
		else
			AdvanceConfigData->Advrnd.DCPTimerRate = DCPTimeControlParam.DCPTime = DCP_TIMERINTERRUPTUS_DEFAULT;
		//DCP State Machine time.
		if((AdvanceConfigData->Advrnd.DCPStateMachineTime >= DCP_STATEMACHINETIMEUS_MIN)
					&& (AdvanceConfigData->Advrnd.DCPStateMachineTime <= DCP_STATEMACHINETIMEUS_MAX)
					&& ((AdvanceConfigData->Advrnd.DCPStateMachineTime % AdvanceConfigData->Advrnd.DCPTimerRate) == 0))
			DCPTimeControlParam.StateMachineTime = AdvanceConfigData->Advrnd.DCPStateMachineTime;
		else
			AdvanceConfigData->Advrnd.DCPStateMachineTime = DCPTimeControlParam.StateMachineTime = DCP_STATEMACHINETIMEUS_DEFAULT;
		//DCP Control Loop Time.
		if((AdvanceConfigData->Advrnd.DCPControlLoopTime >= DCP_CONTROLLOOPTIMEUS_MIN)
						&& (AdvanceConfigData->Advrnd.DCPControlLoopTime <= DCP_CONTROLLOOPTIMEUS_MAX)
						&& ((AdvanceConfigData->Advrnd.DCPControlLoopTime % AdvanceConfigData->Advrnd.DCPTimerRate) == 0))
			DCPTimeControlParam.ControlLoopTime = AdvanceConfigData->Advrnd.DCPControlLoopTime;
		else
			AdvanceConfigData->Advrnd.DCPControlLoopTime = DCPTimeControlParam.ControlLoopTime = DCP_CONTROLLOOPTIMEUS_DEFAULT;
		//DCP Modbus Time.
		if((AdvanceConfigData->Advrnd.DCPModBusTime >= DCP_MODBUSCOMMTIMEUS_MIN)
						&& (AdvanceConfigData->Advrnd.DCPModBusTime <= DCP_MODBUSCOMMTIMEUS_MAX)
						&& ((AdvanceConfigData->Advrnd.DCPModBusTime % AdvanceConfigData->Advrnd.DCPTimerRate) == 0))
			DCPTimeControlParam.ModBusCommTime = AdvanceConfigData->Advrnd.DCPModBusTime;
		else
			AdvanceConfigData->Advrnd.DCPModBusTime = DCPTimeControlParam.ModBusCommTime = DCP_MODBUSCOMMTIMEUS_DEFAULT;
		//DCP ADC Resolution.
		if((AdvanceConfigData->Advrnd.DCPADCResolution == DCP_ADCRESOLUTION_11BIT)
							|| (AdvanceConfigData->Advrnd.DCPADCResolution == DCP_ADCRESOLUTION_16BIT))
			DCPTimeControlParam.AdcResolution = AdvanceConfigData->Advrnd.DCPADCResolution;
		else
			AdvanceConfigData->Advrnd.DCPADCResolution = DCPTimeControlParam.AdcResolution =  DCP_ADCRESOLUTION_11BIT;
		//DCP Blind Time.
		if(AdvanceConfigData->Advrnd.DCPBlindTime <= DCP_BLINDTIMEUS_MAX)
				DCPTimeControlParam.BlindTime = AdvanceConfigData->Advrnd.DCPBlindTime;
			else
				AdvanceConfigData->Advrnd.DCPBlindTime = DCPTimeControlParam.BlindTime = DCP_BLINDTIMEUS_DEFAULT;
	}
	else//Apply S and V defaults
	{
		DCPTimeControlParam.DCPTime = DCP_TIMERINTERRUPTUS_DEFAULT_SV;
		DCPTimeControlParam.StateMachineTime = DCP_STATEMACHINETIMEUS_DEFAULT_SV;
		DCPTimeControlParam.ControlLoopTime = DCP_CONTROLLOOPTIMEUS_DEFAULT_SV;
		DCPTimeControlParam.BlindTime = DCP_BLINDTIMEUS_DEFAULT_SV;
		DCPTimeControlParam.ModBusCommTime = DCP_MODBUSCOMMTIMEUS_DEFAULT_SV;
		DCPTimeControlParam.AdcResolution = DCP_ADCRESOLUTION_DEFAULT_SV;
	}
	DCPTimeControlParam.CRC = ByteCRC((UINT8 *)&DCPTimeControlParam, (sizeof(DCPTimeControlParam) - sizeof(DCPTimeControlParam.CRC)));
}


/*  void CheckForRTCBattery(void)
 *
 *  Purpose:
 *     This function is called once at powerup from PowerUpInitilization() function.
 *	   It generates an event if the date read from RTC is found to be zero.
 *   Entry condition: None.
 *
 *   Exit condition: None.
 */
void CheckForRTCBattery(void)
{
	RTCTime TM;
	RTClock::GetCurrentTime(&TM);//Read RTC
	if((TM.Day == 0) && (TM.Century_Month == 0) && (TM.Years == 0))
		Log::WriteEventLog(EVENT_RTC_BATTERYFAILURE);
}


/*  void UpdatePeakPower(void)
 *
 *  Purpose:
 *     TO update the Peak power value to be shown on LCD during cycle
 *   Entry condition: None.
 *
 *   Exit condition: None.
 */
void UpdatePeakPower(void)
{
	if((TempPeakPower < RtReceiveData.PowerOut)
		&&((RtReceiveData.ErrorReason & MULTIPLE_OVERLOAD_OR_AC_LOST) != MULTIPLE_OVERLOAD_OR_AC_LOST))
		TempPeakPower = RtReceiveData.PowerOut;
}

/*  void UpdateFrequencyOffset(void)
 *
 *  Purpose:
 *     TO set the Frequency offset for DCP live. It doesn't affect the weld frequency during weld.
 *   Entry condition: None.
 *
 *   Exit condition: None.
 */
void UpdateFrequencyOffset(void)
{
   if((IsManualFunctionAvailable() == TRUE) || (FieldBusType == HW_ASSEMBLY_PROFIBUS))
	  RtSendData.FreqIn = UserIO::GetExternalFrequency();
   else
	  RtSendData.FreqIn = FieldBus::GetExternalFrequencyOffset();
}
