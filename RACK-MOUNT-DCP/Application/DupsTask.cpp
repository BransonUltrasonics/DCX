/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/DupsTask.cpp_v   1.7.1.8.1.2   15 Sep 2016 15:55:50   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/DupsTask.cpp_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Added HORNSCAN_FAILED alarm log, Added CheckForInputsSIG() function
 *
 *    Rev 1.7.1.8.1.2   15 Sep 2016 15:55:50   EGUAJARDO
 * Added error messages to be sent to the Manager Basic app.
 * 
 *    Rev 1.7.1.8.1.1   25 Jul 2016 12:14:56   GButron
 * NumberofBytestoRead changed from int to SINT32
 * 
 *    Rev 1.7.1.8.1.0   06 Jun 2016 13:36:30   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.7.1.8   01 Sep 2015 08:37:08   sesharma
 * Fix for Memory clear with reset.
 * 
 *    Rev 1.7.1.7   01 Sep 2015 02:26:58   amaurya
 * Changed for Memory Clear.
 * 
 *    Rev 1.7.1.6   25 Jun 2015 07:05:50   amaurya
 * Added check for AC lost.
 * 
 *    Rev 1.7.1.5   04 Jun 2015 07:37:28   sesharma
 * Type casting for Data1 & Data2 fields changed to SINT32 when writing an event. Overload alarm output not activated in Rack-S on AC Power lost.
 * Removed unused variables. Branson coding standard formatter applied.
 * 
 *    Rev 1.7.1.4   09 Apr 2015 05:13:18   sesharma
 * Changes done to handle condition when in Time/energy mode,Weld is stopped by Reset Input.
 * 
 *    Rev 1.7.1.3   19 Mar 2015 06:40:42   sesharma
 * Overload alarm handling made compatible with S&V code, Energy Special handled for Rack-S system type. And Branson Coding Standard Applied.
 * 
 *    Rev 1.7.1.2   09 Mar 2015 09:36:38   sesharma
 * Generic Overload bit handled only when 15V is present.
 * 
 *    Rev 1.7.1.1   26 Feb 2015 08:24:52   sesharma
 * Generic overload Alarm condition handled.
 * 
 *    Rev 1.7.1.0   13 Jan 2015 06:00:44   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.7   07 Nov 2014 15:31:28   ewomack
 * SIG special added to EEPROM defaults.  Alarms added for Power Window Limits and Timeout in Energy Mode.
 * 
 *    Rev 1.6   28 Oct 2014 17:00:56   ewomack
 * Check FRAM version at powerup and re-initialize FRAM if the version has changed.
 * 
 *    Rev 1.5   17 Oct 2014 09:41:42   sesharma
 * Bug fixed DEF00770 and modifications in state Machine.
 * 
 *    Rev 1.4   16 Oct 2014 10:04:40   sesharma
 * DEF00770 resolved
 * 
 *    Rev 1.3   16 Oct 2014 03:47:08   sesharma
 * Tracker Issues Resolved
 * 
 *    Rev 1.2   17 Sep 2014 08:46:14   amaurya
 * Code review changes, Memory clear at reset fix, added defines
 * 
 *    Rev 1.1   03 Sep 2014 07:36:52   amaurya
 * Clear at Memory reset issue resolved & Changing the calling of InitUserIO(). 
 * 
 *    Rev 1.0   18 Aug 2014 09:17:22   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:58   amaurya
 * Initial revision.
 */

#include "DupsTask.h"
#include "stdio.h"
#include "FIFO.h"
#include "string.h"
#include "ADC.h"
#include "GLOBALS.h"
#include "FUNC.h"
#include "StateMachineDrv.h"
#include "Board.h"
#include "CPU.h"
#include "UserIO.h"
#include "IOTask.h"
#include "ReceiveCommandHandler.h"
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
#include "ShutDownTask.h"
#include "LED.h"
#include "CommandsFT.h"

#define MASK_OVERLOAD_BITS                                 0x78
#define OL_SHIFT                                           3       //Bits need to be shifted in fpga_input to match with Error Reason Bit positions
#define EIGHTBITSMASK                                      0xFF
#define NUMBEROFBYTESTOREAD                                127

/*--------------------Global Variables-------------------------------------*/
//Chip Select for FPGA
volatile UINT8 * cs2 = (UINT8*) FPGA_BASEADDRESS;
SINT8 FPGAVersionLabel[FPGASWVERSION_LEN] = {0};
SINT32 missedWrite = 0;
SINT32 msgCount = 0;
SINT32 missedWrite1 = 0;
SINT32 msgCount1 = 0;
SINT32 FreqOutDacVal = 0;
/*--------------------------------------------- ---------------------------*/
/*-------------------------- External variables --------------------------*/
extern UINT16 LCDKeyStatus;
extern SINT16 CurrentIndex;
extern UINT32 AccumulatedEnergy; /* the power is integrated to energy */
EUI_24AA025E48 * EpromPtr = NULL;
BOOLEAN MemoryClearAtRst;
BOOLEAN IOReadFlag = FALSE;
BOOLEAN AlarmFlag = FALSE;
/*-------------------------------------------- --------------------------*/
DupsTask::DupsTask(IOTask *adc) :
Task("DUPS", DUPSTASK_STACKSIZE), DupsTaskQ(DUPSTASK_FIFOSIZE, "DupsQ")
/****************************************************************************/
/* Description:-                                                            */
/* DupsTask Constructor                     											 */
/****************************************************************************/
{
   adcTask = adc;
   thisPtr = this;
}

DupsTask::~DupsTask()
/****************************************************************************/
/* Description:-                                                            */
/* DupsTask destructor                     											 */
/****************************************************************************/
{

}

void DupsTask::WriteAddress(UINT8 Addr, UINT8 Val)
/****************************************************************************/
/* Description:-                                                            */
/* This function will write on particular FPGA address.                     */
/* Input Parameters:-                                                       */
/* Addr : FPGA address to write                                             */
/* Val : value to write.                                                    */
/****************************************************************************/
{
   cs2 = (uint8*) (FPGA_BASEADDRESS | Addr);
   *cs2 = Val;
}

UINT8 DupsTask::ReadAddress(UINT8 Addr)
/****************************************************************************/
/* Description:-                                                            */
/* This function will read on particular FPGA address.                      */
/* Input Parameters:-                                                       */
/* Addr : FPGA address to read															 */
/* returns value read from FPGA.															 */
/****************************************************************************/
{
   cs2 = (uint8*) (FPGA_BASEADDRESS | Addr);
   return (*cs2);
}

void DupsTask::Run()
/****************************************************************************/
/* Description:-                                                            */
/* This function reads digital and analog inputs from ADC channels,         */
/* MI memory mapped I/O ,from FPGA and processes control loops.             */
/****************************************************************************/
{
   char i;
   AdcBuf *d;
   BOOLEAN oldCableDetect = FALSE, CurrCableDetect = FALSE, powerUpCableDetect =
   FALSE;
   static SINT32 DebouncedTime = 0;

   // select State Machine sequence
   SelectDCPSequence();
   OutputStatusToFPGA = 0 | SONIC_ON_OUT | FPGAOVERLOAD_OUT;

   // Write FPGA output
   thisPtr->WriteAddress(FPGA_OUTPUT_ADDR, OutputStatusToFPGA);

//	CyGOS::InitWatchDog();
//	CyGOS::EnableWatchDog();
   while (FOREVER) {   //Forever loop
      //Wait for wakeup message.
//		BOOLEAN read = DupsTaskQ.Read(this, i, -1);
      DupsTaskQ.Read(this, i, -1);
      if (adcTask->rxQueue.Read(this, d, 0)) {

//			int length = d->len; // Don't have any use of Length as of yet....but just have it.

         //data1 indexes are ADC channel numbers.
         /* Read Analog Inputs from various sources*/

         // Need to be added in preset also for software Digital tunning.
         // FreqOffsetInput value is read from MI if Ext frequency option is enabled from Website.
         // Currently it does nothing as we don't have MI board.
         if (RunningPreset.SWDip.Flags.ExtFrequency) {
            //Get MI board analog input value
            //If diagnostic from Web is not enabled
            if (!WebSite::CheckForDiagnosticFlag()) {
               FreqOffsetInput = UserIO::GetExternalFrequency();
            }
         }
         else
            FreqOffsetInput = ConvertFreqtoDCPformat(RunningPreset.FreqOffset);

         //Read phase from phase detector circuit
         PhaseInput = (d->data[PHASE_MAG_SEL]);

         // Amplitude - analog input signal 0..+10V from Instumentation board
         ActualAmplitude = (d->data[AMP_SEL]);

         // Used in weld cycle only
         // External amplitude value is read from MI if Ext Amplitude option is enabled from Website.
         //Get MI board analog input value
         //If diagnostic from Web is not enabled
         if (!WebSite::CheckForDiagnosticFlag()) ExtAmplitudeInput =
               (UserIO::GetExternalAmplitude());   //(SINT32)d->data1[EXTAMPL_SEL];

         // Read DC current - analog input signal 0..+10V from Instumentation board.
         CurrentInput = (d->data[CURRENT_SEL]);
         if(CurrentInput > WeldPeakCurrentResult)
         {
         	WeldPeakCurrentResult = CurrentInput;			//Gets the Peak current to send to DCX Manager Basic
         }
         if (BIT(EPromData.Systemtype) == RACKMOUNT_S) {
            if (!WebSite::CheckForDiagnosticFlag()) EnergyNeeded =
                  UserIO::GetRawExternalEnergyInPSForm();
            //TODO - Needed to disable this as it was clearing the WeldMode
         }
         //Read Power - analog input signal 0..+13V from Instumentation board.
         PowerInput = (d->data[POWER_SEL]);

         //Read Temperature - analog input signal 0..+10V from Instumentation board.
         InputTemperature = (d->data[TEMP_SEL]);
         IOReadFlag = FALSE;
         //see if cable detect is selected
         CurrCableDetect = UserIO::CheckForCableDetect();
         if (CurrCableDetect)			//cable detection is enabled
         {
            //if cable detection is enabled check the cable detect signal
            CurrCableDetect = UserIO::CheckForInputCable();
            if (CurrCableDetect != oldCableDetect) {
               DebouncedTime++;
               if (DebouncedTime > 10) {
                  DebouncedTime = 0;
                  oldCableDetect = CurrCableDetect;
                  Log::WriteEventLog(
                        CurrCableDetect ?
                              EVENT_INPUTCABLEFOUND : EVENT_INPUTCABLELOST, 0,
                        0);
               }
               powerUpCableDetect = TRUE;
               IsIOConfigured = (CurrCableDetect ? TRUE : IsIOConfigured);
            }
            else if (powerUpCableDetect == FALSE) {
               Log::WriteEventLog(
                     CurrCableDetect ?
                           EVENT_INPUTCABLEFOUND : EVENT_INPUTCABLELOST, 0, 0);
               powerUpCableDetect = TRUE;
            }
            if (CurrCableDetect == oldCableDetect) DebouncedTime = 0;
         }

      }
      //		else
      //			pprintf("R");
      CheckForPowerDown();
      //Run DUPS Application code
      Cycle_Run();
   }
}

void StartSonics()
/****************************************************************************/
/* Description:-                                                            */
/* This function sends SONIC_ON_OUT to FPGA.                                */
/*                                                                          */
/****************************************************************************/
{
   OutputStatusToFPGA = SONIC_ON_OUT;
}

void FPGAOverloadReset()
/****************************************************************************/
/* Description:-                                                            */
/* This function sends FPGAOVERLOAD_OUT to FPGA.                            */
/*                                                                          */
/****************************************************************************/
{
   OutputStatusToFPGA = FPGAOVERLOAD_OUT;
}

void SonicsOff()
/****************************************************************************/
/* Description:-                                                            */
/* This function sends NO_OUT to FPGA.It indicates that sonics must be off. */
/*                                                                          */
/****************************************************************************/
{
   OutputStatusToFPGA = NO_OUT;
}

void WriteAlarmLog()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for writing alarm log.                      */
/****************************************************************************/
{
   /*POWER_OVERLOAD = 0x01,
    CURRENT_OVERLOAD = 0x02 ,
    VOLTAGE_OVERLOAD = 0x04,
    TEMP_OVERLOAD = 0x08,*/
   //To avoid multiple log entries in the case power supply goes off
   //before DCP board we are checking that there should not be more than
   //1 overloadBit high at the same time.
   UINT8 skipLog = 0;
   UINT8 indx = 0;
   for (indx = 0; indx < 4; indx++) {
      if (ErrorReason & (1 << indx)) skipLog++;
   }
   if (skipLog <= 1) {
      if (ErrorReason & POWER_OVERLOAD) {
         //Update Overload Power LOG Entry
      	strcpy(ErrorMessageResult,"Overload - Power Limit");
         Log::WriteAlarmLog(OVERLOAD_POWERLIMIT, 0, 0);

      }
      if (ErrorReason & CURRENT_OVERLOAD) {
         //Update Overload Current LOG Entry.
      	strcpy(ErrorMessageResult,"Overload - Current");
         Log::WriteAlarmLog(OVERLOAD_CURRENT, 0, 0);

      }
      if (ErrorReason & VOLTAGE_OVERLOAD) {
         //Update Overload Voltage LOG Entry.
      	strcpy(ErrorMessageResult,"Overload - Voltage");
         Log::WriteAlarmLog(OVERLOAD_VOLTAGE, 0, 0);
      }
      if (ErrorReason & TEMP_OVERLOAD) {
         //Update Overload Temperature LOG Entry
      	strcpy(ErrorMessageResult,"Overload - Temperature");
         Log::WriteAlarmLog(OVERLOAD_TEMPRATURE, 0, 0);
      }
   }

   if (ErrorReason & DDSFREQ_LOWSEEKLIMIT) {
      //Update DDS frequency Low Seek Limit LOG Entry
   	strcpy(ErrorMessageResult,"Frequency - Low Seek Limit");
      Log::WriteAlarmLog(DDSFREQ_OUTSIDE_LOWSEEKLIMIT, 0, 0);
   }
   if (ErrorReason & DDSFREQ_HIGHSEEKLIMIT) {
      //Update DDS frequency high Seek Limit LOG Entry
   	strcpy(ErrorMessageResult,"Frequency - High Seek Limit");
      Log::WriteAlarmLog(DDSFREQ_OUTSIDE_HIGHSEEKLIMIT, 0, 0);
   }
   if (ErrorReason & DDSFREQ_LOWWELDLIMIT) {
      //Update DDS frequency low weld Limit LOG Entry
   	strcpy(ErrorMessageResult,"Frequency - Low Weld Limit");
      Log::WriteAlarmLog(DDSFREQ_OUTSIDE_LOWWELDLIMIT, 0, 0);
   }
   if (ErrorReason & DDSFREQ_HIGHWELDLIMIT) {
      //Update DDS frequency high weld Limit LOG Entry
   	strcpy(ErrorMessageResult,"Frequency - High Weld Limit");
      Log::WriteAlarmLog(DDSFREQ_OUTSIDE_HIGHWELDLIMIT, 0, 0);
   }
   if (ErrorReason & PHASELIMIT_TIMEERROR) {
      //Update phase limit error LOG Entry
   	strcpy(ErrorMessageResult,"Overload - Phase");
      Log::WriteAlarmLog(PHASELIMIT_TIMELIMIT_ERROR, 0, 0);
   }
   if (ErrorReason & PEAKPOWER_MINUSLIMIT) {
      //Update phase limit error LOG Entry
   	strcpy(ErrorMessageResult,"Power Low Limit");
      Log::WriteAlarmLog(PEAKPOWER_MINUS_LIMIT, 0, 0);
   }
   if (ErrorReason & PEAKPOWER_PLUSLIMIT) {
      //Update phase limit error LOG Entry
   	strcpy(ErrorMessageResult,"Power High Limit");
      Log::WriteAlarmLog(PEAKPOWER_PLUS_LIMIT, 0, 0);
   }
   if (ErrorReason & ENERGY_REACHEDTIMEOUT) {
      //Update Energy Timeout error LOG Entry
   	strcpy(ErrorMessageResult,"Energy - Timeout Reached");
      Log::WriteAlarmLog(ENERGYREACHED_TIMEOUT, 0, 0);

   }
   if (ErrorReason & AC_POWER_LOST) {
      //Update 230V AC Power Lost error LOG Entry
   	strcpy(ErrorMessageResult,"AC Power Lost");
      Log::WriteAlarmLog(ACPOWER_LOST, 0, 0);

   }
   if (ErrorReason & HORN_SCAN_FAILED) {
      //Update Horn Scan Failed error LOG Entry
   	strcpy(ErrorMessageResult,"Horn Scan Failed");
      Log::WriteAlarmLog(HORNSCAN_FAILED, 0, 0);

   }
}

BOOLEAN CheckForInputsSIG()
/****************************************************************************/
/* Description:-                                                            */
/* This function returns TRUE if any input is active .                      */
/* If SIG is active then it ignores the External Start Input                */
/* returns value TRUE or FALSE.												*/
/****************************************************************************/
{
   BOOLEAN result = FALSE;

 if ((UserIO::CheckForExternalTest()) || (UserIO::CheckForExternalStart() && !(RunningPreset.SIG && EPromData.SIGSpecial))
         || (UserIO::CheckForHornScan()) || (UserIO::CheckForMemoryClear())
         || (UserIO::CheckForExternalSeek())
         || (WebSite::CheckForWebTestCommand()) || (LCDKeyStatus & KB_TEST)) {
		   result = TRUE;
 	 }
   return result;
}

BOOLEAN CheckForInputs()
/****************************************************************************/
/* Description:-                                                            */
/* This function returnd TRUE if any input is active .                      */
/* returns value TRUE or FALSE.															 */
/****************************************************************************/
{
   BOOLEAN result = FALSE;

   if ((UserIO::CheckForExternalTest()) || (UserIO::CheckForExternalStart())
         || (UserIO::CheckForHornScan()) || (UserIO::CheckForMemoryClear())
         || (UserIO::CheckForExternalSeek())
         || (WebSite::CheckForWebTestCommand()) || (LCDKeyStatus & KB_TEST)) {
      result = TRUE;
   }
   return result;
}

BOOLEAN CheckForInputsExMemClr()
/****************************************************************************/
/* Description:-                                                            */
/* This function returns TRUE if any input except memory clear is active .  */
/* returns value TRUE or FALSE.															 */
/****************************************************************************/
{
   BOOLEAN result = FALSE;

   if ((UserIO::CheckForExternalTest()) || (UserIO::CheckForExternalStart())
         || (UserIO::CheckForHornScan()) || (UserIO::CheckForExternalSeek())
         || (WebSite::CheckForWebTestCommand()) || (LCDKeyStatus & KB_TEST)) {
      result = TRUE;
   }
   return result;
}

BOOLEAN CheckForPowerUpStatus()
{
   BOOLEAN result = FALSE;
   if ((PowerUpScan) || (PowerUpSeek) || (NoPowerUpSeekScan)) {
      result = TRUE;
   }
   return result;
}

void DupsTask::Cycle_Run(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for all the power supply control work.It    */
/* shall read digital and analog inputs from MI memory mapped               */
/* I/O ,FPGA and process them to produce control signals to FPGA. It also   */
/* update various counters like cycle, error, PowerOnSec,PowerOnMilliSec,   */
/* SonicsOnSec,SonicsOnMilliSec in FRAM.                                    */
/****************************************************************************/
{
   BOOLEAN boardTest = WebSite::CheckForMFBoardTestFlag();
   SINT32 Frequency = 0;
   SINT32 EnergyOut = 0;
   static BOOLEAN clkLost = FALSE;
   static SINT32 TempOLCheckTimeout = 0;
   /********* (Variables in each cycle evaluated)************/
   PowerOnMilliCounterIncrement += DCPTIME;
   /*(Analysis of the input signals from FPGA)*/
   fpga_input = DupsTask::thisPtr->ReadAddress(FPGA_INPUT_ADDR);
   fpga_input = ~fpga_input;

   /*(Analysis of the input signals from MI)
    needs to be changed for 2010 DCP board.*/
   // Check for Ext. Reset Input and Reset input from LCD or Web
   if (((UserIO::CheckForExternalReset() || (LCDKeyStatus & KB_RESET))
	         && AlarmFlag && !EnableWebIO && !CheckForInputsSIG())
	         || (WebSite::CheckForWebReset() && AlarmFlag)) {
	  WebSite::ResetWebResetFlag();
      // To Handle condition when in Time/energy mode,Weld is stopped
      // by Reset Input while external signal has already been released.
      if (CHECK_FOR_AC && !WeldStoppedbyReset) {
         AlarmFlag = FALSE;
         UserIO::SetGenAlarm(FALSE);
         ErrorReason = 0;
         ClearWindowLimitAlarm();
         TimedSeekTimerTicks = 0;
         ResetEnable = TRUE;
         if(CurrentPreset.SWDip.Flags.ClearMemAtRst == TRUE)
         	 MemoryClearAtRst = TRUE;	// Set Clear memory flag in Clear memory at reset is set.
      }
   }
   // Check for Ext. Reset Input, Reset from LCD with Clear memory
   else if ((((UserIO::CheckForExternalReset() || (LCDKeyStatus & KB_RESET))
	         && AlarmFlag && !EnableWebIO && !CheckForInputsExMemClr()
	         && UserIO::CheckForMemoryClear()))) {
      AlarmFlag = FALSE;
      UserIO::SetGenAlarm(FALSE);
      WebSite::ResetWebResetFlag();
      ErrorReason = 0;
      ClearWindowLimitAlarm();
      TimedSeekTimerTicks = 0;
      ResetEnable = TRUE;
      MemoryClearAtRst = TRUE;
   }
   /* (Check for P/S overload error)*/
   //we have observed a behavior during DCX A and F development that some times Generic overload bit
   //is not active even there is a specific overload bit is active. Generic bit is made active explicitly
   //in this case in FPGA code for DCX A and F. Here in S and V we are checking other bits in controller code.
   //Oring in if condition is in two parts just for clarity for future programmers.
   else if (((fpga_input & OL_MASK_EXCGEN) || (fpga_input & OVL_FUNCTION) || (!CHECK_FOR_AC))
         && (!boardTest)) {
      if (OutputStatusToFPGA & SONIC_ON_OUT) {
         if (fpga_input & OL_CURRENT) ErrorReason |= CURRENT_OVERLOAD;

         if (fpga_input & OL_VOLTAGE) ErrorReason |= VOLTAGE_OVERLOAD;

         if (fpga_input & OL_POWER) ErrorReason |= POWER_OVERLOAD;
      }
      if (fpga_input & OL_TEMPERATURE) {
         if (fpga_input & MULTIPLE_ALARM_MASK) {
            ErrorReason |= ((fpga_input & MULTIPLE_ALARM_MASK) >> OL_SHIFT);
            ErrorReason |= TEMP_OVERLOAD;
            TempOLCheckTimeout = 0;
         }
         else {
            if (TempOLCheckTimeout++ > 20) {
               ErrorReason |= TEMP_OVERLOAD;
               TempOLCheckTimeout = 0;
            }
         }
      }
      AlarmFlag = TRUE;
      TimedSeekTimerTicks = 0;
      //if AC is not present
      if (!CHECK_FOR_AC) ErrorReason &= ~ALL_ERROR_MASK;   //just make sure no false alarms are logged.
      if (!(((fpga_input & OL_MASK_EXCGEN) == OL_TEMPERATURE)
            && (!(ErrorReason & TEMP_OVERLOAD)))) OverLoadAlarm =
      TRUE;
      UserIO::SetGenAlarm(TRUE);
      //This alarm means the power supply has overloaded because of frequency, current, or phase.
      //The ultra sonics have been shut off.
      if (!(ErrorReason & AC_POWER_LOST))   // In case of AC Power lost Overload alarm
         UserIO::SetOverloadAlarm(TRUE);   // Overload Output should not be activated in Rack-S.
   }
   /* (If fault is maintained on RESET)*/
   else if (WeldError) {
      TimedSeekTimerTicks = 0;
   }
   /* (Check for Test Input from MI / LCD)*/
   else if (((UserIO::CheckForExternalTest() || (LCDKeyStatus & KB_TEST))
         && !IsIOConfigured && !EnableWebIO)
         || (WebSite::CheckForWebTestCommand())) {
      TimedSeekTimerTicks = 0;
      if (CurrentIndex == WAITING_IDX) {
         if (!CheckForPowerUpStatus()) TestEnable = TRUE;
      }
   }
   /* Check for Start Sonics Input */
   else if (UserIO::CheckForExternalStart() && !EnableWebIO
         && !IsIOConfigured) {
      TimedSeekTimerTicks = 0;
      if (CurrentIndex == WAITING_IDX) {
         if (!CheckForPowerUpStatus()) {
            StartInputEnable = TRUE;
         }
      }
   }
   // Check for Ext Horn Scan
   else if ((UserIO::CheckForHornScan() && !EnableWebIO && !IsIOConfigured)
         || WebSite::CheckForWebScanCommand()) {
      TimedSeekTimerTicks = 0;
      if (CurrentIndex == WAITING_IDX) {
         if (!CheckForPowerUpStatus()) ScanInitiate = TRUE;
      }
   }
   // Check Memory Clear Input from MI Board
   else if ((UserIO::CheckForMemoryClear() && !EnableWebIO && !IsIOConfigured)
         || ((CurrentPreset.SWDip.Flags.ClearMemAtRst)
               && (UserIO::CheckForExternalReset() || (LCDKeyStatus & KB_RESET) || WebSite::CheckForWebReset()))) {
   	MemoryClearAtRst = TRUE;
   	WebSite::ResetWebResetFlag();
   }
   /*  (nothing before and normally seek requested) */
   else if (CurrentIndex == WAITING_IDX) {
      /* Check for Ext Seek Input */
      if ((UserIO::CheckForExternalSeek() && !EnableWebIO && !IsIOConfigured)
            || (WebSeek == TRUE)) {
         if (!CheckForPowerUpStatus()) SeekEnable = TRUE;
      }
      /* Check for Timed Seek option from preset*/
      if (RunningPreset.SWDip.Flags.TimedSeek && !EnableWebIO) {
         TimedSeekTimerTicks += DCPTIME;
         if (TimedSeekTimerTicks >= CONV_MILLI_MICROSEC(MAX_TIMER_COUNTER)) {
            TimedSeekTimerTicks = 0;
            if (!CheckForPowerUpStatus()) TimedSeekEnabled = TRUE;
         }
      }
      else {
         TimedSeekTimerTicks = 0;
      }
   }
   else {
      TimedSeekTimerTicks = 0;
   }
   if ((CurrentIndex == WAITING_IDX) && (MemoryClearAtRst == TRUE)) {
      MemoryClearAtRst = FALSE;
      TimedSeekTimerTicks = 0;
      SeekMemoryFrequency = 0;
      WeldMemoryFrequency = 0;
      CurrentDCPStatus.SeekMemoryFrequency = 0;
      CurrentDCPStatus.WeldMemoryFrequency = 0;
      MemoryReset = TRUE;
      //Memory frequency is cleared at this option
      MemoryValue = 0;
      CurrentPreset.MemoryFreq = MemoryValue;
      RunningPreset.MemoryFreq = MemoryValue;

      IntegralValue = 0;
      IntegralFactor = 0;
      ddsFrequencyHigh = Midband + FREQ_MAX;		//v191
      ddsFrequencyLow = Midband + FREQ_MIN;		//v191
   }
   // State Machine Driver code
   StateMachine();

   if (boardTest)		//calculate the frequency in TEST mode
   {
      OutputStatusToFPGA = NO_OUT;
      Frequency = (BoardTestFrequency * RESOLUTION_FACTOR) / CLK_FREQ;
   }
   else		//calculate the frequency in Power Supply mode
   {
      // Check Boundary Condition for frequency
      if (FrequencyValue < FREQ_MIN) {
         FrequencyValue = FREQ_MIN;
         DDSFrequency = Midband + FREQ_MIN;
      }
      if (FrequencyValue > FREQ_MAX) {
         FrequencyValue = FREQ_MAX;
         DDSFrequency = Midband + FREQ_MAX;
      }

      // Calculate Frequency

      if (ScanEnable == TRUE) {
         Frequency = (DDSFrequency * RESOLUTION_FACTOR) / CLK_FREQ;
      }
      else
         Frequency = ((((DDSFrequency) * F_Factor) / 100) * RESOLUTION_FACTOR)
               / CLK_FREQ;
   }

   if(CommandsFT::Status==CommandsFT::FT_OFF) {
      // Write Frequency to FPGA
      WriteAddress(FPGA_FREQ_ADDR_BYTE4, ((Frequency) >> BITPOSITIONS) & EIGHTBITSMASK);
      WriteAddress(FPGA_FREQ_ADDR_BYTE3, ((Frequency) >> TWOTHIRDBITPOSITIONS) & EIGHTBITSMASK);
      WriteAddress(FPGA_FREQ_ADDR_BYTE2, ((Frequency) >> ONETHIRDBITPOSITIONS) & EIGHTBITSMASK);
      WriteAddress(FPGA_FREQ_ADDR_BYTE1, (Frequency) & EIGHTBITSMASK);

      // Check Boundary Condition of PWM Amp
      if (PWMAmp > MAX_PWM_AMP)
         PWMAmp = MAX_PWM_AMP;
      else if (PWMAmp < MIN_PWM_AMP)
      	PWMAmp = MIN_PWM_AMP;

      // Write Amplitude to FPGA
      WriteAddress(FPGA_AMP_ADDR_BYTE2, ((PWMAmp) >> ONETHIRDBITPOSITIONS) & EIGHTBITSMASK);
      WriteAddress(FPGA_AMP_ADDR_BYTE1, (PWMAmp) & EIGHTBITSMASK);
   }

   // Read phase Sign from FPGA
   PhaseSign = (~(fpga_input) & FPGA_SIGN) >> 2;

   //Update power on sonic time in FRAM
   if (OutputStatusToFPGA & SONIC_ON_OUT) {
      SonicOnMilliCounterIncrement += DCPTIME;
   }

   if (MFCycleFlag) {
      if ((CurrentIndex == WAITING_IDX)
            || ((CurrentIndex >= MANF_WELD_START_IDX)
                  && (CurrentIndex <= MANF_WELD_STOP_IDX))) {
         MFcycleResults.currCycleTime++;
      }
   }
   if ((MFCycleFlag) && /*(!AlarmFlag)*/(CurrentIndex != MANF_WAIT_STOP_IDX)
         && !(OutputStatusToFPGA & SONIC_ON_OUT)) {
      SonicsOffCounter++;
   }

   if (MFCycleFlag && (OutputStatusToFPGA & SONIC_ON_OUT)) {
      MFcycleResults.sonicsOnTime++;
      SonicsOnCounter++;
   }

   if(CommandsFT::Status==CommandsFT::FT_OFF) {
      // Write FPGA status output
      WriteAddress(FPGA_OUTPUT_ADDR, OutputStatusToFPGA);
   }
   WriteAddress(FPGA_BOARDTESTOUTPUT_ADDR, BoardTestOutput);
//	//Indicate to FPGA that CPU is active
//	WriteAddress(FPGA_OFFSET_ADDR_BYTE1, 0x01);

   // Set MI board analog output values in variables
   //If diagnostic from Web is not enabled
   if (!WebSite::CheckForDiagnosticFlag()) {
      UserIO::SetOutputAmplitude(
            ActualAmplitude * DAC_MULFACTOR / DAC_DIVFACTOR);
      UserIO::SetOutputPower(PowerInput);
      if (DDSFrequency > 0) {
         if (ScanEnable == TRUE)   //scan case frequencies are absolute value multiplied by ten
            FreqOutDacVal = (DDSFrequency * 100) / F_Factor;   //first get the required frequency range to be used in the formula 4 lines below.
         else
            //without scan frequencies are scaled using F_Factor
            FreqOutDacVal = DDSFrequency;

         //This DAC calculation works with frequencies scaled with F_factor
         FreqOutDacVal = ((FreqOutDacVal - (Midband + FREQ_MIN)) * DAC_MULFACTOR
               / FREQ_OUTFACTOR);
         if (FreqOutDacVal > DAC_MULFACTOR)   //Make sure we don't cross the DAC limits in any case
            FreqOutDacVal = DAC_MULFACTOR;
         else if (FreqOutDacVal < 0) FreqOutDacVal = 0;
         UserIO::SetOutputFrequency(FreqOutDacVal);
      }
      switch (UserIO::CheckForEnergyRange()) {
      case ENERGY_CONTROL_RANGE1:
         EnergyOut = ((AccumulatedEnergy / ENERGY_MULT_FAC) * EPromData.PSpower)
               / ADC11BITVALUE;
         EnergyOut = ((EnergyOut) * ADC11BITVALUE) / ENERGYRANGE_1_MAX;
         break;
      case ENERGY_CONTROL_RANGE2:
         EnergyOut = ((AccumulatedEnergy / ENERGY_MULT_FAC) * EPromData.PSpower)
               / ADC11BITVALUE;
         EnergyOut = ((EnergyOut) * ADC11BITVALUE) / ENERGYRANGE_2_MAX;
         break;
      case ENERGY_CONTROL_RANGE3:
         EnergyOut = ((AccumulatedEnergy / ENERGY_MULT_FAC) * EPromData.PSpower)
               / ADC11BITVALUE;
         EnergyOut = ((EnergyOut) * ADC11BITVALUE) / ENERGYRANGE_3_MAX;
         break;
      case ENERGY_CONTROL_RANGE4:
      	EnergyOut = ((AccumulatedEnergy / ENERGY_MULT_FAC) * EPromData.PSpower)
      			/ ADC11BITVALUE;
      	EnergyOut = ((EnergyOut) * ADC11BITVALUE) / ENERGYRANGE_4_MAX;
      	break;
      case ENERGY_CONTROL_RANGE5:
      	EnergyOut = ((AccumulatedEnergy / ENERGY_MULT_FAC) * EPromData.PSpower)
      			/ ADC11BITVALUE;
      	EnergyOut = ((EnergyOut) * ADC11BITVALUE) / ENERGYRANGE_5_MAX;
           	break;
      default:
         EnergyOut = 0;
      }
      UserIO::SetOutputEnergy(EnergyOut * DAC_MULFACTOR / DAC_DIVFACTOR);
   }

   while (ManFErrorCycleCounterInc != 0) {
      IncrementManFErrorCycleCounter(); /*Increments Cycle Counter	*/
      ManFErrorCycleCounterInc--;
   }

   //Counter handling.
   while (CycleCounterIncrement != 0) {
      IncrementCycleCounter(); /*Increments Cycle Counter	*/
      CycleCounterIncrement--;
   }
   while (ErrorCounterIncrement != 0) {
      IncrementErrorCounter(); /*Increments Error Counter	*/
      ErrorCounterIncrement--;
   }

   if (PowerOnMilliCounterIncrement >= CONV_MILLI_MICROSEC(MAXMILLISEC_ONE_SEC)) {
      IncrementPowerOnSecCounter(); /*Increments Power On Milli Second Counter	*/
      PowerOnMilliCounterIncrement = 0;
   }
   while (SonicOnMilliCounterIncrement
         >= CONV_MILLI_MICROSEC(MAXMILLISEC_ONE_SEC)) {
      IncrementSonicOnSecCounter();/*Increments Sonics On Milli Second Counter */
      SonicOnMilliCounterIncrement = 0;
   }
   if (MCF_CLOCK_PSR & MCF_CLOCK_PSR_LOCKS) {
      if (!clkLost) {
         Log::WriteEventLog(CLOCK_LOST, 0, 0, 0, false);
         clkLost = true;
         pprintf("\n clock lost");
      }
   }
   //Wake up IO task
   adcTask->outQ.Write(this, 'a', 0);
}/* void Cycle_Run() */

void DupsTask::Tick()
{
   static int usCount = 0;
   static int RedLedusCount = 0;
   static BOOLEAN SetLED = TRUE;
   static BOOLEAN FirstTime = TRUE;
   usCount += usecPerTick;
   if (FirstTime == TRUE) {
      FirstTime = FALSE;
      CyGOS::InitWatchDog(WATCHDOG_TIMEOUT);
      CyGOS::EnableWatchDog();
   }

   if (usCount >= 100000)   //100ms
         {
      CyGOS::KickWatchDog();
      usCount = 0;
      LED::Set(1, GREENLED_STATE);
      SetLED = TRUE;
   }
   if (SetLED == TRUE) {
      RedLedusCount += usecPerTick;
      if (RedLedusCount >= 50000)   //50 ms after wd feed
            {
         LED::Set(1, REDLED_STATE);
         RedLedusCount = 0;
         SetLED = FALSE;
      }
   }
}

void CheckForWDReset()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for checking whether reset is done by      */
/* watchdog.                                                                */
/****************************************************************************/
{
   // Write Power on Event in LOG
   // check for watch dog reset
   if (MCF_RCM_RSR & MCF_RCM_RSR_WDRCORE)
      Log::WriteEventLog(WATCHDOG_RESET, 0, 0);
}

void PowerUpInitilization(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for power up initialization of scan , weld  */
/* seek global parameters . It will Recall preset,counters from FRAM.       */
/****************************************************************************/
{
   int TempLen = 0;
   int indx = 0;
   short PowerDownReg = 0;
   UINT32 ERomDataChecksum = 0;
   CPU::SetupChipSelect(CHIPSELCT_PIN1, 10, 8, (UINT16*) FRAM_BASEADDRESS_CS1);
   CPU::SetupChipSelect(CHIPSELCT_PIN2, 20, 8, (UINT8*) FPGA_BASEADDRESS_CS2);
   CPU::SetupChipSelect(CHIPSELCT_PIN3, 20, 8,
         (UINT8*) MI_DIGITALIO_BASEADDRESS_CS3);

   //Initialize LOG module
   Log::Init();

   PowerDownReg = MCF_RCM_RSR;

   // Write Power on Event in LOG
   Log::WriteEventLog(EVENT_POWERON, PowerDownReg, 0);
   CheckForWDReset();
   /* Global Variables for Seek, weld ,Test and Scan are initialized */
   FrequencyValue = 0;
   DDSFrequency = 0;
   PWMAmp = 0;
   CycleCounterIncrement = 0;
   ErrorCounterIncrement = 0;
   ManFCycleCounterInc = 0;
   ManFErrorCycleCounterInc = 0;
   TimedSeekTimerTicks = 0;

   // Clear Seek Status
   Clear_Seek_Status();

   // Clear Weld Status
   Clear_Weld_Status();

   IntegralValue = 0;
   IntegralFactor = 0;

   PowerUpCompleted = FALSE;
   PowerUpRequired = FALSE;
   PowerUpDelay = 0;
   TotalPowerUpDelay = 0;
   WaitDelay = 0;
   PowerUpSeek = FALSE;
   PowerUpScan = FALSE;
   PowerUpScanStarted = FALSE;
   PowerUpSeekStarted = FALSE;
   //AutoSonicsStarted = FALSE;
   NoPowerUpSeekScan = FALSE;
   PresetVerifiedRequired = TRUE;
   StartMFCycle = FALSE;
   SonicsOnCounter = 0;
   SonicsOffCounter = 0;
   TempSonicsOnTimePerWeldCycle = 0;
   EnableWebIO = FALSE;
   MFTimeoutEnable = FALSE;
   IsIOConfigured = FALSE;
   MemoryClearAtRst = FALSE;
   EpromPtr = new EUI_24AA025E48(Board::GetI2C(0, 5, 5),
   I2CCHIP_24AA025E48_ADDRESS);
   TempLen = sizeof(EPromData);
   EpromPtr->Read(I2C_24AA025E48_DATA_ADDR, (UINT8 *) &EPromData, TempLen, MAXI2CRETRIES);
   ERomDataChecksum = ByteCRC((UINT8*) &EPromData, sizeof(EPromData) - sizeof(EPromData.checkSum));
   if ((TempLen < 0) || (EPromData.checkSum != ERomDataChecksum)) {
      Log::WriteEventLog(CRC_ERROR, (SINT32) EPromData.checkSum,
            (SINT32) ERomDataChecksum, USERIDWRITEEVENTLOG, TRUE);
      SetDefaultEEPROMData();
      Midband = F20KHZMIDBAND;
      F_Factor = F20KHZF_FACTOR;
   }

   //Check for FRAM version change
   FramCheck();

   //Save fresh values of current active configuration  of digital outputs at FRAM
   //base address so that from very second power up digital outputs goes correct
   //(Note: Hardware in RevF DCP board is like that the all digital outputs go high
   //on power up.)
   //This value is used by boot loader code to write the reverse of active configuration
   //of digital outputs. This way we limit the initially high going pulse to 100 ms for
   //those outputs which are configured as active high.
   __FRAM_START = 0;
   for (indx = 1; indx < NUM_DOUT; indx++)
      __FRAM_START |= ((UserIO::GetDigOutPutActiveConfiguration(
            (DIGITAL_OUTPUTS) indx)) << (indx - 1));

   //Initialize USER IO module
   UserIO::InitUserIO();
   pprintf("\n Digital output configuration is %02X", __FRAM_START);
   // Reset Weld buffer
   InitializeWeldbuffer();

   // Reset Scan buffer
   InitializeScanbuffer();
   SonicsOff();
   //Set Min and Max values in Minimum and Maximum preset parameters
   SetMaxMinValues();

   //set min max cycle parameters
   SetMinMaxCycleParam();
   ReadFPGAVersion();

   //this function checks for the hardware configuration at start up.
   CheckHWConfiguration();

   //Check and correct Counters at power up
   PowerOnCounterCheck();

   //Check for counter CRC at power up
   CheckDCPCounterCRC();

   //Set Min and Max values in Minimum and Maximum Scan preset
   SetMinMaxScanParameter();

   //Update Tune Frequency and Memory frequency at Power up
   InitMemOffsetAtPowerUp();

   // Verify Preset and scan Preset at power up.
   VerifyPresetAtPowerUp();

   //Print version String at power up
   pprintf("\n software version string = ");
   for (int i = 0; i < SWVERSION_LEN; i++)
      pprintf("%c", SWVersionLabel[i]);

   //pprintf("\n Power Supply Specification : ");
   pprintf("\n Power Supply frequency: %d", EPromData.PSfrequency);
   pprintf("\n Power Supply system: %d", EPromData.Systemtype);
   pprintf("\n Power Supply EDP : %d", EPromData.EDP);
   pprintf("\n Midband = %d and FFactor = %d", Midband, F_Factor);
   pprintf("\n FlashCRC=%d", sysConfig.DCXCRC);

   //sysConfig.CurrSystemType = OEM;
   CheckManFCycleParamCRC();
   MFCycleFlag = CurrentCycleParam.MfTest;
   //On powerup reatin the last value
   TempSonicsOnTimePerWeldCycle = CurrentCycleParam.sonicsOnTimePerWeldCycle;
   SetFPGAFrequency();
   RTOS::DelayMs(5);
   //Writing the maximum and minimum values once at powerup. This is a fix to prevent
   //analog output fluctuations
   UserIO::dac->WriteOutput(0, 0);   //Set the outputs to zero again.
   UserIO::dac->WriteOutput(1, 0);   //Set the outputs to zero again.
}

void UpdateFlashCRC()
/****************************************************************************/
/* Description:-                                                            */
/* This function calculate DCX Flash CRC and update it in System            */
/* Configuration .                                                          */
/****************************************************************************/
{
   UINT16 FlashCRC = 0;
   SINT32 CodeSize = (SINT32) __TEXT_END - (SINT32) __TEXT_START;
   SINT32 FlashOffset = (SINT32) __TEXT_START - (SINT32) __FLASH_RAM;
   printf("\n code size %d flash offset %d flash start %08X", (int) CodeSize,
         (int) FlashOffset, (unsigned int) (__FLASH_ROM + FlashOffset));
   FlashCRC = ByteCRC((UINT8 *) (__FLASH_ROM + FlashOffset), (UINT32) CodeSize);

   if (sysConfig.DCXCRC != FlashCRC)
      Log::WriteEventLog(CRC_CHANGE, (SINT32)sysConfig.DCXCRC,  (SINT32)FlashCRC);
   sysConfig.DCXCRC = FlashCRC;
   //delete codeImage;
}

void CheckHWConfiguration()
/****************************************************************************/
/* Description:-                                                            */
/* This function checks for Version change, the hardware configuration at   */
/* start up and  Checks whether MI board is present or not.                 */
/****************************************************************************/
{
   if (!CheckForVersionChange()) {
      // Perform power on copy
      Power_On_Copy();
   }
   UpdateMidBandAndFFactor();
   ddsFrequencyHigh = Midband + FREQ_MAX;
   ddsFrequencyLow = Midband + FREQ_MIN;

   //Calculate and update DCX Flash CRC in System config structure
   UpdateFlashCRC();

   CheckForFPGAVersionChange();
}

/*
 * Function updates the Midband and FFactor according to the
 * frequency of PowerSupply. This function gets called on powerup
 * and when frequency is changed from Manufacturing page
 */
void UpdateMidBandAndFFactor()
{
   switch (EPromData.PSfrequency) {
   case FREQ_20KHZ:
      Midband = F20KHZMIDBAND;
      F_Factor = F20KHZF_FACTOR;
      break;
   case FREQ_30KHZ:
      Midband = F30KHZMIDBAND;
      F_Factor = F30KHZF_FACTOR;
      break;
   case FREQ_40KHZ:
      Midband = F40KHZMIDBAND;
      F_Factor = F40KHZF_FACTOR;
      break;
   }
   pprintf("\n Midband = %d and FFactor = %d", Midband, F_Factor);
}

BOOLEAN CheckForVersionChange()
/****************************************************************************/
/* Description:-                                                            */
/* If DCP Software Version is changed update the newly added parameters with*/
/* default values.                                                          */
/* returns VersionChanged : returns true if version is changed else false.  */
/****************************************************************************/
{
   //UINT32 j;
   BOOLEAN VersionChanged = FALSE;
   int VersionChangedstatus = memcmp(DCPSoftwareVersionLabel, SWVersionLabel,
   SWVERSION_LEN);
   if (VersionChangedstatus != 0) {
      memcpy(DCPSoftwareVersionLabel, SWVersionLabel, SWVERSION_LEN);
      /*		CurrentWorkNumber = 0;
       memcpy(&ParmName, "DEFAULT 20KHz\n", MAXPARAMNAME_LEN);
       SetDefaultValues();
       for (j= 0; j< MAX_WORK_NUMBER; j++)
       {
       SavePreset(j);
       }
       SetDefaultScanParameters();
       SaveScanParamCRC();
       UserIO::ResetUserIO();
       SetDefaultCycleParam();*/
      Log::WriteEventLog(VERSION_CHANGE, 0, 0);
      //VersionChanged = TRUE;
      pprintf("\n DCP Software version changed");
   }
   return VersionChanged;
}

void Power_On_Copy(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for Recalling current preset from FRAM and  */
/* updating Running preset.                                                 */
/****************************************************************************/
{
   if (CurrentWorkNumber >= MAX_WORK_NUMBER) CurrentWorkNumber = 0;

   //Recall current preset from FRAM
   RecallPreset(CurrentWorkNumber);

   //Update Running preset
   UpdateRPreset = TRUE;

   //Check for Scan parameter CRC
   CheckScanParamCRC();
}

void PowerOnCounterCheck(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for checking valid counter values from FRAM.*/
/* If not within boundary , then initialized to zero.                        */
/****************************************************************************/
{
   if (DCPCounter.CycleCounter > MAX_CYCLE_COUNTER) {
      DCPCounter.CycleCounter = 0;
   }

   if ((DCPCounter.ErrorCounter) > MAX_ERROR_COUNTER) {
      DCPCounter.ErrorCounter = 0;
   }

   if (DCPCounter.PowerOnSecond > MAX_SEC_COUNTER) {
      DCPCounter.PowerOnSecond = 0;
   }

   if ((DCPCounter.SecondsOfSonic) > MAX_SEC_COUNTER) {
      DCPCounter.SecondsOfSonic = 0;
   }
}

void InitMemOffsetAtPowerUp(void)
/**********************************************************************************/
/* Description:-                                                                  */
/* This function sets stored frequency offset in FRAM to zero, if clear memory    */
/* at power up is set and assign stored Memory frequency to Global Memory value.  */
/**********************************************************************************/
{
   //Memory frequency is the offset from resonance frequency calculated
   //after a successful seek ,test and weld cycle (if memory store option is set)
	if(CurrentPreset.SWDip.Flags.ClrMemAtPwrUp == TRUE){
		CurrentPreset.MemoryFreq = 0;	//Set Stored Memory offset to zero
	   SavePreset(0);
	}
   MemoryValue = CurrentPreset.MemoryFreq;	//Set Global Memory value as Stored Memory offset
}

void SetFPGAFrequency()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for calculating Frequency value used in     */
/* FPGA.                                                                    */
/****************************************************************************/
{
   FrequencyValue = MemoryValue + FreqOffsetInput;
   DDSFrequency = FrequencyValue + CurrentPreset.DigitalTune;
}

SINT32 ConvertFreqtoDCPformat(SINT32 freqIn)
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for converting frequency value to DCP format*/
/* power.If failed, boundary values are assigned.                           */
/* Input parameters:-                      					                   */
/* freqIn : Indicates input frequency							                   */
/* returns RetVal : DCP converted Frequency value depending upon frequency  */
/* Multiplication factor.                                                   */
/****************************************************************************/
{
   switch (EPromData.PSfrequency) {
   case FREQ_20KHZ:
      freqIn = (freqIn * FREQUENCY_MULTFACTOR) / DUPS_20KHZ;
      break;
   case FREQ_30KHZ:
      freqIn = (freqIn * FREQUENCY_MULTFACTOR) / DUPS_30KHZ;
      break;
   case FREQ_40KHZ:
      freqIn = (freqIn * FREQUENCY_MULTFACTOR) / DUPS_40KHZ;
      break;
   default:
      break;
   }
   return freqIn;
}

void VerifyPresetAtPowerUp()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for verifying preset and scan structure at  */
/* power up.If failed, boundary values are assigned.                        */
/****************************************************************************/
{
   BOOLEAN PresetVerifiedSuccess = TRUE, ScanVerifiedSuccess = TRUE;
   if (PresetVerifiedRequired) {
      PresetVerifiedRequired = FALSE;
      PresetVerifiedSuccess = VerifyPreset();
      ScanVerifiedSuccess = VerifyScanParam();
   }
   if (!PresetVerifiedSuccess) {
      CheckAndAssignSeekCycleBoundaryValue();
      CheckAndAssignWeldCycleBoundaryValue();
   }
   if (!ScanVerifiedSuccess) {
      CheckAndAssignScanCycleBoundaryValue();
   }
}

void ReadFPGAVersion()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for reading and printing FPGA version string*/
/* from FPGA.                                                               */
/****************************************************************************/
{
   pprintf("\n Software version string of FPGA code = ");
   cs2 = (uint8*) 0x0800000D;
   FPGAVersionLabel[0] = *cs2;
   cs2 = (uint8*) 0x0800000E;
   FPGAVersionLabel[1] = *cs2;
   cs2 = (uint8*) 0x0800000F;
   FPGAVersionLabel[2] = *cs2;
   cs2 = (uint8*) 0x08000010;
   FPGAVersionLabel[3] = *cs2;
   FPGAVersionLabel[4] = 0;
   for (int i = 0; i < FPGASWVERSION_LEN; i++)
      pprintf("%c", FPGAVersionLabel[i]);
}

void CheckForFPGAVersionChange()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for checking FPGA Version at power up and   */
/* assign ADC factor accordingly for REV Board B and REV Board C.           */
/****************************************************************************/
{
   SINT32 VersionChangedstatus = strcmp(FPGAVersionLabel, FPGAVERSTRINGREVB);
   pprintf("\n VersionChangedstatus=%d", VersionChangedstatus);
}


void ReadMACAddressFromEEPRom(UINT8 * MacPtr)
/****************************************************************************/
/* Description:-                                                         */
/* This function reads the MAC Adress from EEPROM chip                   */
/****************************************************************************/
{
   int TempLen = MAC_ADDRESS_LEN;

   EpromPtr->Read(I2C_24AA025E48_MACSTART_ADDRESS, MacPtr, TempLen, MAXI2CRETRIES);
   if (TempLen < 0)
   	memcpy(MacPtr, DEFAULT_MAC_ADDR, MAC_ADDRESS_LEN);
}


/************************************************************************************************
 * void DupsTask::FillingEntireWritableMacchipArea(UINT16 Data)                                 *
 *                                                                                              *
 *  Purpose                                                                                     *
 *    Fill entire writable Macchip area with a value                                            *
 *                                                                                              *
 *  Entry Condition:                                                                            *
 *     Data                                                                                     *
 *                                                                                              *
 *                                                                                              *
 *  Exit Condition:                                                                             *
 *                                                                                              *
 ***********************************************************************************************/
void DupsTask::FillingEntireWritableMacchipArea(UINT16 Data)
{
	const UINT16 LowerMACChipAddress = 0;
	const UINT16 HigherMACChipAddress = 127;

	for(UINT16 Ctr=LowerMACChipAddress; Ctr<=HigherMACChipAddress; Ctr++){
		EpromPtr -> Write(Ctr, Data);
	}
}


/************************************************************************************************
 * void DupsTask::FillingEntireWritableMacchipAreawithItsOwnAddress(void)                       *
 *                                                                                              *
 *  Purpose                                                                                     *
 *    Fill entire writable Macchip area with its own address                                    *
 *                                                                                              *
 *  Entry Condition:                                                                            *
 *     None                                                                                     *
 *                                                                                              *
 *  Exit Condition:                                                                             *
 *                                                                                              *
 ***********************************************************************************************/
void DupsTask::FillingEntireWritableMacchipAreawithItsOwnAddress(void)
{
	const UINT16 LowerMACChipAddress = 0;
	const UINT16 HigherMACChipAddress = 127;

	for(UINT16 Ctr=LowerMACChipAddress; Ctr<=HigherMACChipAddress; Ctr++){
		EpromPtr -> Write(Ctr, Ctr);
	}
}


/************************************************************************************************
*  UINT16 DupsTask::ReadMACChip(UINT16 Address)                                                 *
*                                                                                               *
*  Purpose:                                                                                     *
*     This function reads a single byte from an I2C device                                      *
*                                                                                               *
*  Entry condition:                                                                             *
*     Address on I2C bus                                                                        *
*                                                                                               *
*                                                                                               *
*  Exit condition:                                                                              *
*     None                                                                                      *
************************************************************************************************/
UINT16 DupsTask::ReadMACChip(UINT16 Address)
{
	UINT16 DataRead = 0;

	DataRead = EpromPtr -> Read(Address, MACCHIPMAXRETRIES);
	return DataRead;
}


/************************************************************************************************
*  void DupsTask::WriteMACChip(UINT16 Address, UINT16 Data)                                     *
*                                                                                               *
*  Purpose:                                                                                     *
*     This function writes a single byte data to an I2C device                                  *
*                                                                                               *
*  Entry condition:                                                                             *
*     Address on I2C bus                                                                        *
*     Data to write to the device                                                               *
*                                                                                               *
*  Exit condition:                                                                              *
*     None                                                                                      *
*************************************************************************************************/
void DupsTask::WriteMACChip(UINT16 Address, UINT16 Data)
{
	EpromPtr -> Write(Address, Data);
}


/************************************************************************************************
 *  void DupsTask::ReadAllMACChipData(UINT8 * MacBackupPtr)                                     *
 *                                                                                              *
 *   Purpose:                                                                                   *
 *    This function is called to make a full backup of MACChip                                  *
 *                                                                                              *
 *                                                                                              *
 *   Entry condition:                                                                           *
 *    MacBackupPtr - Pointer to byte array where MAC content should be copied.                  *
 *                                                                                              *
 *                                                                                              *
 *   Exit condition:                                                                            *
 *    None.                                                                                     *
 ************************************************************************************************/
void DupsTask::ReadAllMACChipData(UINT8 * MacBackupPtr)
{
	SINT32 NumberofBytestoRead = NUMBEROFBYTESTOREAD;                                                     //128 Bytes (0 through 127), full backup
   EpromPtr -> Read(MACCHIP_RWDATAAREA_STARTADDR, MacBackupPtr, NumberofBytestoRead, NUMBER_OF_RETRIES); //Use the Read function of EUI_24AA025E48
}


/************************************************************************************************
 *   void DupsTask::RestoreMACChipData(UINT16 Address, UINT16 Data)                             *
 *                                                                                              *
 *   Purpose:                                                                                   *
 *		  Restore data to MAC Chip                                                                *
 *                                                                                              *
 *                                                                                              *
 *   Entry condition:                                                                           *
 *		  Address, Data                                                                           *
 *                                                                                              *
 *   Exit condition:                                                                            *
 *		  None.                                                                                   *
 ************************************************************************************************/
void DupsTask::RestoreMACChipData(UINT16 Address, UINT16 Data){
	EpromPtr -> Write(Address, Data);
}


void SetDefaultEEPROMData()
/****************************************************************************/
/* Description:-                                                            */
/* Set the default   frequency,power and serial numbers  					*/
/* in 24AA025E48 chip.             											*/
/****************************************************************************/
{
   EPromData.PSfrequency = FREQ_20KHZ;
   EPromData.PSpower = P2500W;
   EPromData.EnergyPowerSpecial = FALSE;
   EPromData.Systemtype = DEF_SYSTEM;
   EPromData.EDP = DEF_EDP;
   EPromData.SIGSpecial = FALSE;

   strcpy((char *) EPromData.SerialNo, DEFAULT_SERIAL_NUMBER);
   EPromData.checkSum = ByteCRC((UINT8*) &EPromData, sizeof(EPromData) - sizeof(EPromData.checkSum));
   EpromPtr->Write(I2C_24AA025E48_DATA_ADDR, (UINT8 *) &EPromData, sizeof(EPromData));
}


void ModifyEEPROMdata()
{
   /****************************************************************************/
   /* Description:-                                                            */
   /* Set the values already modified in global EEPromData object into  		*/
   /* 24AA025E48 chip.             											*/
   /****************************************************************************/
   EPromData.checkSum = ByteCRC((UINT8 *) &EPromData, sizeof(EPromData) - sizeof(EPromData.checkSum));
   EpromPtr->Write(I2C_24AA025E48_DATA_ADDR, (UINT8 *) &EPromData, sizeof(EPromData));
}


void CheckManFCycleParamCRC(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for checking valid Manufacturing Cycle param*/
/* from FRAM. If CRC does not matches then default value is assigned.       */
/****************************************************************************/
{
   UINT32 ManFCycleParamCRC;
   ManFCycleParamCRC = ByteCRC((UINT8 *) &CurrentCycleParam,
         sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
   if (CurrentCycleParam.CRC != ManFCycleParamCRC) {
      printf("\n manufacturing CRC Check fails");
      Log::WriteEventLog(CRC_ERROR, (SINT32) CurrentCycleParam.CRC,
            (SINT32) ManFCycleParamCRC, 3, true);
      SetDefaultCycleParam();
   }
}
