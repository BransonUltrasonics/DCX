/*
 $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/DupsTask.cpp_v   1.45.1.8   02 Jul 2014 09:15:38   amaurya  $
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
 $Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/DupsTask.cpp_v  $
 * 
 *    Rev 1.45.1.8   02 Jul 2014 09:15:38   amaurya
 * Removed Amplitude Decay function.
 * 
 *    Rev 1.45.1.7   13 Jun 2014 08:41:30   amaurya
 * Added check to log Temperature Overload.
 * 
 *    Rev 1.45.1.6   23 May 2014 09:52:20   amaurya
 * Removed extra ;
 * 
 *    Rev 1.45.1.5   01 May 2014 10:52:06   amaurya
 * Added check for DDSFrequency 
 * 
 *    Rev 1.45.1.4   30 Apr 2014 06:18:06   amaurya
 * Added change for Digital Tune
 * 
 *    Rev 1.45.1.3   12 Mar 2014 07:52:28   rjamloki
 * Sequencing Error Fix, Generic OL bit taken care, Time field in logs separated by colon.
 * 
 *    Rev 1.45.1.2   04 Dec 2013 06:57:12   rjamloki
 * Fixed PowerUpScan graph issue.
 * 
 *    Rev 1.45.1.1   10 Oct 2013 10:07:58   rjamloki
 * Fixed Temperature Overload issue at power down.
 * 
 *    Rev 1.45.1.0   09 Sep 2013 08:59:50   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.45   01 Mar 2013 08:16:40   amaurya
 * Added Energy Range In 250J.
 * 
 *    Rev 1.44   15 Jan 2013 04:39:28   amaurya
 * Energy Special.
 * 
 *    Rev 1.38.2.1   30 Mar 2012 06:42:14   PDwivedi
 * Table driven CRC routine for all CRC calculations
 * 
 *    Rev 1.38.2.0   28 Mar 2012 11:12:58   PDwivedi
 * Added changes for simultaneous activation of digital inputs(like reset and memory clear) problem for correct operation.
 * 
 *    Rev 1.38   18 Oct 2011 09:45:00   RatneshA
 * Cable detect logic fixed to force state machine on prewait once the cable is detected.
 * 
 *    Rev 1.37   17 Oct 2011 07:40:30   RatneshA
 * Added logic to detect Cable Detect signal
 * 
 *    Rev 1.36   16 Sep 2011 08:19:44   RatneshA
 * Fixed frequncy offset
 * 
 *    Rev 1.35   01 Sep 2011 13:05:52   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.34   29 Aug 2011 11:37:46   ygupta
 * Ver1.01P. Implemented Shut Down Process.
 * 
 *    Rev 1.33   28 Aug 2011 12:09:04   ygupta
 * Ver1.01N
 * 
 *    Rev 1.32   28 Aug 2011 11:42:24   ygupta
 * Ver1.01M
 * 
 *    Rev 1.31   16 Aug 2011 08:22:26   pdwivedi
 * code cleanup
 * 
 *    Rev 1.30   03 Aug 2011 09:35:36   pdwivedi
 * Added changes for CRC Error.
 * 
 *    Rev 1.29   02 Aug 2011 07:37:02   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.28   18 Jul 2011 10:37:20   pdwivedi
 * Added Exception Log entry changes. 
 * 
 *    Rev 1.26   15 Jul 2011 09:44:38   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.25   21 Jun 2011 11:05:32   PDwivedi
 * Fixes after mongle horde round1
 * 
 *    Rev 1.24   13 Jun 2011 09:26:54   PDwivedi
 * Chnages due to frequency and power level combination.
 * 
 *    Rev 1.23   24 May 2011 04:18:54   ygupta
 * Startup digital output fix
 * 
 *    Rev 1.22   09 May 2011 10:33:18   ygupta
 * multiple Alarm log fix
 * 
 *    Rev 1.19   27 Apr 2011 10:46:56   PDwivedi
 * minor bug fixes, system info page GUI updated for max serail number,Debug re enabled
 * 
 *    Rev 1.18   25 Apr 2011 08:44:02   PDwivedi
 * Changes after MAC chip addition
 * 
 *    Rev 1.17   25 Apr 2011 06:34:04   PDwivedi
 * Restore default should not modify the frquency,power and serial number
 * 
 *    Rev 1.16   20 Apr 2011 11:37:24   ygupta
 * Added changes for tracker issues.
 * 
 *    Rev 1.15   19 Apr 2011 10:46:38   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.14   14 Apr 2011 06:22:08   PDwivedi
 * Added changes for IP Setup issue and added comments.
 * 
 *    Rev 1.13   04 Apr 2011 10:58:30   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.12   02 Mar 2011 08:32:00   PDwivedi
 * Changes to support board test
 * 
 *    Rev 1.11   14 Jan 2011 07:55:40   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.10   14 Dec 2010 11:15:10   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.9   11 Dec 2010 05:15:04   PDwivedi
 * Modified Cycle_Run function to not store preset every time memory clear signal comes.It takes too much CPU time if we do it every millisec.
 * 
 *    Rev 1.8   09 Dec 2010 07:54:26   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.7   08 Dec 2010 07:42:26   PDwivedi
 * Added changes for User Configurable Analog outputs.
 * 
 *    Rev 1.6   07 Dec 2010 08:58:18   PDwivedi
 * Added changes for REV Board C , code clean up.
 * 
 *    Rev 1.5   06 Dec 2010 09:56:52   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.4   29 Nov 2010 07:47:34   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.3   22 Nov 2010 09:04:22   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.2   15 Nov 2010 08:47:12   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.1   12 Nov 2010 10:39:16   nhahmed
 * Added Support for External Amplitude and FrequencyOffset and Check for MI Board.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:42   ygupta
 * Initial revision.

 ******************************************************************************/
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

//#define printf
//#define pprintf
/*--------------------Global Variables-------------------------------------*/
BOOLEAN AlarmFlag = FALSE;

//Chip Select for FLASH
//volatile uint16 * cs0 = (uint16 *) FLASH_BASEADDRESS;

//Chip Select for FRAM
volatile uint16 * cs1 = (uint16 *) FRAM_BASEADDRESS;

//Chip Select for FPGA
volatile uint8 * cs2 = (uint8 *) FPGA_BASEADDRESS;

//Chip Select for MI Board
volatile uint8 * cs3 = (uint8 *) MI_DIGITALIO_BASEADDRESS;

int missedWrite = 0;
int msgCount = 0;
int missedWrite1 = 0;
int msgCount1 = 0;
char FPGAVersionLabel[FPGASWVERSION_LEN];
/*--------------------------------------------- ---------------------------*/

/*-------------------------- External variables --------------------------*/
//extern SINT16 HornScanRawPhase[DCP_HORNSCANMAXPOINTS];
extern SINT16 *HornScanRawPhase;
extern SINT16 HornScanPhase[DCP_HORNSCANPOINTS];
extern UINT16 NumScanPoints;
extern UINT16 LCDKeyStatus;
extern SINT16 CurrentIndex;
EUI_24AA025E48 * epromPtr;
BOOLEAN IORead;
extern UINT32 AccumulatedEnergy; /* the power is integrated to energy */
SINT32 FreqOutDacVal;

#define MASK_OVERLOAD_BITS 0x78
#define OL_SHIFT  3//Bits need to be shifted in fpga_input to match with Error Reason Bit positions

/*--------------------------------------------- --------------------------*/

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

void DupsTask::WriteAddress(unsigned char Addr, unsigned char Val)
/****************************************************************************/
/* Description:-                                                            */
/* This function will write on particular FPGA address.                     */
/* Input Parameters:-                                                       */
/* Addr : FPGA address to write                                             */
/* Val : value to write.                                                    */
/****************************************************************************/
{
	cs2 = (uint8*)(FPGA_BASEADDRESS | Addr);
	*cs2 = Val;
}

unsigned char DupsTask::ReadAddress(unsigned char Addr)
/****************************************************************************/
/* Description:-                                                            */
/* This function will read on particular FPGA address.                      */
/* Input Parameters:-                                                       */
/* Addr : FPGA address to read															 */
/* returns value read from FPGA.															 */
/****************************************************************************/
{
	cs2 = (uint8*)(FPGA_BASEADDRESS | Addr);
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
	BOOLEAN oldCableDetect = FALSE, CurrCableDetect = FALSE, powerUpCableDetect = FALSE;
	static int DebouncedTime = 0;

	// select State Machine sequence
	SelectDCPSequence();
	OutputStatusToFPGA = 0 | SONIC_ON_OUT | FPGAOVERLOAD_OUT;

	// Write FPGA output
	thisPtr->WriteAddress(FPGA_OUTPUT_ADDR, OutputStatusToFPGA);

//	CyGOS::InitWatchDog();
//	CyGOS::EnableWatchDog();
	while (FOREVER) { //Forever loop
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
			if (RunningPreset.SWDip.Flags.ExtFrequency)
			{
				//Get MI board analog input value
				//If diagnostic from Web is not enabled
				if(!WebSite::CheckForDiagnosticFlag())
				{
					FreqOffsetInput = UserIO::GetExternalFrequency();
				}
			}
			else
				FreqOffsetInput = ConvertFreqtoDCPformat(RunningPreset.FreqOffset);

			//Read phase from phase detector circuit
			PhaseInput = (d->data[PHASE_MAG_SEL]);

			// Amplitude - analog input signal 0..+10V from Instumentation board
			ActualAmplitude = (d->data[AMP_SEL] );

			// Used in weld cycle only
			// External amplitude value is read from MI if Ext Amplitude option is enabled from Website.
			//Get MI board analog input value
			//If diagnostic from Web is not enabled
			if(!WebSite::CheckForDiagnosticFlag())
				ExtAmplitudeInput = (UserIO::GetExternalAmplitude());//(SINT32)d->data1[EXTAMPL_SEL];

			// Read DC current - analog input signal 0..+10V from Instumentation board.
			CurrentInput = (d->data[CURRENT_SEL]);

			if(!WebSite::CheckForDiagnosticFlag())
				EnergyNeeded = UserIO::GetRawExternalEnergyInPSForm();

			//Read Power - analog input signal 0..+13V from Instumentation board.
			PowerInput = (d->data[POWER_SEL]);

			//Read Temperature - analog input signal 0..+10V from Instumentation board.
			InputTemperature = (d->data[TEMP_SEL] );
			IORead = FALSE;
			//see if cable detect is selected
			CurrCableDetect = UserIO::CheckForCableDetect();
			if(CurrCableDetect)//cable detection is enabled
			{
				//if cable detection is enabled check the cable detect signal
				CurrCableDetect = UserIO::CheckForInputCable();
				if(CurrCableDetect != oldCableDetect)
				{
					DebouncedTime++;
					if(DebouncedTime > 10)
					{
						DebouncedTime = 0;
						oldCableDetect = CurrCableDetect;
						Log::WriteEventLog(CurrCableDetect?EVENT_INPUTCABLEFOUND:EVENT_INPUTCABLELOST, 0 , 0);
					}
					powerUpCableDetect = TRUE;
					IsIOConfigured = (CurrCableDetect?TRUE:IsIOConfigured);
				}
				else if(powerUpCableDetect == FALSE)
				{
					Log::WriteEventLog(CurrCableDetect?EVENT_INPUTCABLEFOUND:EVENT_INPUTCABLELOST, 0 , 0);
					powerUpCableDetect = TRUE;
				}
				if(CurrCableDetect == oldCableDetect)
					DebouncedTime = 0;
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
	OutputStatusToFPGA= SONIC_ON_OUT;
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
	OutputStatusToFPGA= NO_OUT;
}

//void CheckForPowerDown()
///****************************************************************************/
///* Description:-                                                            */
///* This function is responsible for writing alarm log.                      */
///****************************************************************************/
//{
//	  /*POWER_OVERLOAD = 0x01,
//	  CURRENT_OVERLOAD = 0x02 ,
//	  VOLTAGE_OVERLOAD = 0x04,
//	  TEMP_OVERLOAD = 0x08,*/
//	//To avoid multiple log entries in the case power supply goes off
//	//before DCP board we are checking that there should not be more than
//	//1 overloadBit high at the same time.
//	UINT8 skipLog = 0;
//	UINT8 indx = 0;
//	if(ShutDownTask::RTOSShutDown == FALSE)
//	{
//		for(indx = 1; indx < 4; indx++)
//		{
//			if(ErrorReason & (1 << indx))
//				skipLog++;
//		}
//		if(skipLog > 1)
//		{
//			//ShutDown the RTOS as Instrumentation board has lost the power and
//			// DCP will loose the power in next one second (due to power circuit delay.
//			// Set the flag and ShutDownTask will take care of this when ever it will get the time.
//			ShutDownTask::RTOSShutDown = TRUE;
//			ShutDownTask::AlarmReasons = ErrorReason;
//			//pprintf("\nSet the ShutDown Flag");
//		}
//	}
//}

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
	for(indx = 0; indx < 4; indx++)
	{
		if(ErrorReason & (1 << indx))
			skipLog++;
	}
	if(skipLog <= 1)
	{
		if (ErrorReason & POWER_OVERLOAD)
		{
			//Update Overload Power LOG Entry
			Log::WriteAlarmLog(OVERLOAD_POWERLIMIT, 0, 0);
		}
		if(ErrorReason & CURRENT_OVERLOAD)
		{
			//Update Overload Current LOG Entry.
			Log::WriteAlarmLog(OVERLOAD_CURRENT, 0, 0);
		}
		if(ErrorReason & VOLTAGE_OVERLOAD)
		{
			//Update Overload Voltage LOG Entry.
			Log::WriteAlarmLog(OVERLOAD_VOLTAGE, 0, 0);
		}
		if(ErrorReason & TEMP_OVERLOAD)
		{
			//Update Overload Temperature LOG Entry
			Log::WriteAlarmLog(OVERLOAD_TEMPRATURE, 0, 0);
		}
	}

	if(ErrorReason & DDSFREQ_LOWSEEKLIMIT)
	{
		//Update DDS frequency Low Seek Limit LOG Entry
		Log::WriteAlarmLog(DDSFREQ_OUTSIDE_LOWSEEKLIMIT, 0, 0);
	}
	if(ErrorReason & DDSFREQ_HIGHSEEKLIMIT)
	{
		//Update DDS frequency high Seek Limit LOG Entry
		Log::WriteAlarmLog(DDSFREQ_OUTSIDE_HIGHSEEKLIMIT, 0, 0);
	}
	if(ErrorReason & DDSFREQ_LOWWELDLIMIT)
	{
		//Update DDS frequency low weld Limit LOG Entry
		Log::WriteAlarmLog(DDSFREQ_OUTSIDE_LOWWELDLIMIT, 0, 0);
	}
	if(ErrorReason & DDSFREQ_HIGHWELDLIMIT)
	{
		//Update DDS frequency high weld Limit LOG Entry
		Log::WriteAlarmLog(DDSFREQ_OUTSIDE_HIGHWELDLIMIT, 0, 0);
	}
	if(ErrorReason & PHASELIMIT_TIMEERROR)
	{
		//Update phase limit error LOG Entry
		Log::WriteAlarmLog(PHASELIMIT_TIMELIMIT_ERROR, 0, 0);
	}
}

BOOLEAN CheckForInputs()
/****************************************************************************/
/* Description:-                                                            */
/* This function returnd TRUE if any input is active .                      */
/* returns value TRUE or FALSE.															 */
/****************************************************************************/
{
	BOOLEAN result = FALSE;

	if((UserIO::CheckForExternalTest()) ||
	(UserIO::CheckForExternalStart()) ||
	(UserIO::CheckForHornScan()) ||
	(UserIO::CheckForMemoryClear()) ||
	(UserIO::CheckForExternalSeek())||
	(WebSite::CheckForWebTestCommand())||
	(LCDKeyStatus & KB_TEST))
	{
		result = TRUE;
	}
	return result;
}

BOOLEAN CheckForInputsExMemClr()
/****************************************************************************/
/* Description:-                                                            */
/* This function returnd TRUE if any input except memory clear is active .                      */
/* returns value TRUE or FALSE.															 */
/****************************************************************************/
{
	BOOLEAN result = FALSE;

	if((UserIO::CheckForExternalTest()) ||
	(UserIO::CheckForExternalStart()) ||
	(UserIO::CheckForHornScan()) ||
	(UserIO::CheckForExternalSeek())||
	(WebSite::CheckForWebTestCommand())||
	(LCDKeyStatus & KB_TEST))
	{
		result = TRUE;
	}
	return result;
}

BOOLEAN CheckForPowerUpStatus()
{
	BOOLEAN result = FALSE;
	if((PowerUpScan) ||
	(PowerUpSeek) ||
	(NoPowerUpSeekScan))
	{
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
	SINT32 frequency,EnergyOut;
	static bool clkLost = false;
	static int TempOLCheckTimeout = 0;
	/********* (Variables in each cycle evaluated)************/
	PowerOnMilliCounterIncrement+= DCPTIME;
	/*(Analysis of the input signals from FPGA)*/
	fpga_input = DupsTask::thisPtr->ReadAddress(FPGA_INPUT_ADDR);
	fpga_input = ~fpga_input;

	/*(Analysis of the input signals from MI)
	 needs to be changed for 2010 DCP board.*/
	/*(Check for Ext. Reset Input and Reset input from LCD)*/
	if ((((UserIO::CheckForExternalReset()|| (LCDKeyStatus & KB_RESET))
			&& AlarmFlag && !EnableWebIO && !CheckForInputs())) || ((WebSite::CheckForWebReset()) && AlarmFlag))
	{
		AlarmFlag = FALSE;
		UserIO::SetGenAlarm(FALSE);
		WebSite::ResetWebResetFlag();
		ErrorReason = 0;
		TimedSeekTimerTicks = 0;
		ResetEnable = TRUE;
	}
	else if ((((UserIO::CheckForExternalReset()|| (LCDKeyStatus & KB_RESET))
				&& AlarmFlag && !EnableWebIO && !CheckForInputsExMemClr() && UserIO::CheckForMemoryClear())))
	{
		AlarmFlag = FALSE;
		UserIO::SetGenAlarm(FALSE);
		WebSite::ResetWebResetFlag();
		ErrorReason = 0;
		TimedSeekTimerTicks = 0;
		ResetEnable = TRUE;
		MemoryClearAtRst = TRUE;
	}
    /* (Check for P/S overload error)*/
	//we have observed a behavior during DCX A and F development that some times Generic overload bit
	//is not active even there is a specific overload bit is active. Generic bit is made active explicitly
	//in this case in FPGA code for DCX A and F. Here in S and V we are checking other bits in controller code.
	//Oring in if condition is in two parts just for clarity for future programmers.
	else if (((fpga_input & OL_MASK_EXCGEN) || (fpga_input & OVL_FUNCTION)) && (!boardTest))
	{
		if(OutputStatusToFPGA & SONIC_ON_OUT)
		{
			if (fpga_input & OL_CURRENT)
				ErrorReason |= CURRENT_OVERLOAD;

			if (fpga_input & OL_VOLTAGE)
				ErrorReason |= VOLTAGE_OVERLOAD;

			if (fpga_input & OL_POWER)
				ErrorReason |= POWER_OVERLOAD;
		}
		if (fpga_input & OL_TEMPERATURE){
			if(fpga_input & MULTIPLE_ALARM_MASK){
				ErrorReason |= ((fpga_input & MULTIPLE_ALARM_MASK) >> OL_SHIFT);
				ErrorReason |= TEMP_OVERLOAD;
				TempOLCheckTimeout = 0;
			}
			else{
				if(TempOLCheckTimeout++ > 20){
					ErrorReason |= TEMP_OVERLOAD;
					TempOLCheckTimeout = 0;
				}
			}
		}
		AlarmFlag = TRUE;
		TimedSeekTimerTicks = 0;
		//if AC is not present
		if(!CHECK_FOR_AC)
			ErrorReason &= ~ALL_ERROR_MASK; //just make sure no false alarms are logged.
		if (!(((fpga_input & OL_MASK_EXCGEN) == OL_TEMPERATURE) && (!(ErrorReason & TEMP_OVERLOAD))))
			OverLoadAlarm = TRUE;
		UserIO::SetGenAlarm(TRUE);
		//This alarm means the power supply has overloaded because of frequency, current, or phase.
		//The ultra sonics have been shut off.
		UserIO::SetOverloadAlarm(TRUE);
	}
	/* (If fault is maintained on RESET)*/
	else if (WeldError)
	{
		TimedSeekTimerTicks = 0;
	}
	/* (Check for Test Input from MI / LCD)*/
	else if (((UserIO::CheckForExternalTest() || (LCDKeyStatus & KB_TEST)) && !IsIOConfigured && !EnableWebIO) || (WebSite::CheckForWebTestCommand()))
	{
		TimedSeekTimerTicks = 0;
		if (CurrentIndex == WAITING_IDX)
		{
			//pprintf("\n Test key ");
			if(!CheckForPowerUpStatus())
				TestEnable = TRUE;
		}
	}
	/* Check for Start Sonics Input */
	else if (UserIO::CheckForExternalStart() && !EnableWebIO && !IsIOConfigured)
	{
		TimedSeekTimerTicks = 0;
		if (CurrentIndex == WAITING_IDX)
		{
			if(!CheckForPowerUpStatus())
			{
				StartInputEnable = TRUE;
			}
		}
	}
	// Check for Ext Horn Scan
	else if((UserIO::CheckForHornScan() && !EnableWebIO && !IsIOConfigured) || WebSite::CheckForWebScanCommand())
	{
		TimedSeekTimerTicks = 0;
		if (CurrentIndex == WAITING_IDX)
		{
			//pprintf("\n Start Horn Scan");
			if(!CheckForPowerUpStatus())
				ScanInitiate = TRUE;
		}
	}
	// Check Memory Clear Input from MI Board
	else if(UserIO::CheckForMemoryClear() && !EnableWebIO && !IsIOConfigured)
	{
		TimedSeekTimerTicks = 0;
		if ((CurrentIndex == WAITING_IDX) && !MemoryClearAtRst)
		{
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
			ddsFrequencyHigh = Midband + FREQ_MAX;//v191
			ddsFrequencyLow = Midband + FREQ_MIN;//v191
		}
	}
	/*  (nothing before and normally seek requested) */
	else if (CurrentIndex == WAITING_IDX)
	{
		/* Check for Ext Seek Input */
		if((UserIO::CheckForExternalSeek() && !EnableWebIO && !IsIOConfigured) || (WebSeek == TRUE))
		{
			if(!CheckForPowerUpStatus())
				SeekEnable = TRUE;
		}
		/* Check for Timed Seek option from preset*/
		if (RunningPreset.SWDip.Flags.TimedSeek && !EnableWebIO)
		{
			TimedSeekTimerTicks+= DCPTIME;
			if (TimedSeekTimerTicks >= CONV_MILLI_MICROSEC(MAX_TIMER_COUNTER))
			{
				TimedSeekTimerTicks = 0;
				if(!CheckForPowerUpStatus())
					TimedSeekEnabled = TRUE;
			}
		}
		else
		{
			TimedSeekTimerTicks = 0;
		}
	}
	else
	{
		TimedSeekTimerTicks = 0;
	}
	if ((CurrentIndex == WAITING_IDX)&&(MemoryClearAtRst))
	{
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
		ddsFrequencyHigh = Midband + FREQ_MAX;//v191
		ddsFrequencyLow = Midband + FREQ_MIN;//v191
	}
	// State Machine Driver code
	StateMachine();

	if(boardTest)//calculate the frequency in TEST mode
	{
		OutputStatusToFPGA = NO_OUT;
		frequency = (BoardTestFrequency * RESOLUTION_FACTOR)
						/ CLK_FREQ;
	}
	else//calculate the frequency in Power Supply mode
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

		if(ScanEnable == TRUE){
			frequency = (DDSFrequency * RESOLUTION_FACTOR) / CLK_FREQ;
		}
		else
			frequency = ((((DDSFrequency)* F_Factor)/ 100) * RESOLUTION_FACTOR) / CLK_FREQ;
	}
	// Write Frequency to FPGA
	WriteAddress(FPGA_FREQ_ADDR_BYTE4, ((frequency) >> 24) & 0xff);
	WriteAddress(FPGA_FREQ_ADDR_BYTE3, ((frequency) >> 16) & 0xff);
	WriteAddress(FPGA_FREQ_ADDR_BYTE2, ((frequency) >> 8) & 0xff);
	WriteAddress(FPGA_FREQ_ADDR_BYTE1, (frequency) & 0xff);

	// Check Boundary Condition of PWM Amp
	if (PWMAmp > MAX_PWM_AMP)
		PWMAmp = MAX_PWM_AMP;
	else if (PWMAmp < MIN_PWM_AMP)
		PWMAmp = MIN_PWM_AMP;

	// Write Amplitude to FPGA
	WriteAddress(FPGA_AMP_ADDR_BYTE2, ((PWMAmp) >> 8 ) & 0xFF);
	WriteAddress(FPGA_AMP_ADDR_BYTE1, (PWMAmp) & 0xFF);

	// Read phase Sign from FPGA
	PhaseSign = (~(fpga_input) & FPGA_SIGN) >> 2;

	//Update power on sonic time in FRAM
	if (OutputStatusToFPGA & SONIC_ON_OUT) {
		SonicOnMilliCounterIncrement += DCPTIME;
	}

	if(MFCycleFlag)
	{
		if((CurrentIndex == WAITING_IDX) || ((CurrentIndex >= MANF_WELD_START_IDX)
				&& (CurrentIndex <= MANF_WELD_STOP_IDX)))
		{
			MFcycleResults.currCycleTime++;
//			MFcycleResults.remainingCycleTime--;
		}
	}
	if((MFCycleFlag) && /*(!AlarmFlag)*/(CurrentIndex!= MANF_WAIT_STOP_IDX) && !(OutputStatusToFPGA & SONIC_ON_OUT))
	{
		SonicsOffCounter++;
	}

	if(MFCycleFlag && (OutputStatusToFPGA & SONIC_ON_OUT))
	{
		MFcycleResults.sonicsOnTime++;
		SonicsOnCounter++;
	}
	// Write FPGA status output
	WriteAddress(FPGA_OUTPUT_ADDR, OutputStatusToFPGA);
	WriteAddress(FPGA_BOARDTESTOUTPUT_ADDR, BoardTestOutput);
//	//Indicate to FPGA that CPU is active
//	WriteAddress(FPGA_OFFSET_ADDR_BYTE1, 0x01);

	// Set MI board analog output values in variables
	//If diagnostic from Web is not enabled
	if(!WebSite::CheckForDiagnosticFlag())
	{
		UserIO::SetOutputAmplitude(ActualAmplitude * DAC_MULFACTOR / DAC_DIVFACTOR);
		UserIO::SetOutputPower(PowerInput);
		if(DDSFrequency > 0){
			if(ScanEnable == TRUE)//scan case frequencies are absolute value multiplied by ten
				FreqOutDacVal = (DDSFrequency * 100) / F_Factor;//first get the required frequency range to be used in the formula 4 lines below.
			else//without scan frequencies are scaled using F_Factor
				FreqOutDacVal = DDSFrequency;

			//This DAC calculation works with frequencies scaled with F_factor
			FreqOutDacVal = ((FreqOutDacVal -(Midband + FREQ_MIN)) * DAC_MULFACTOR / FREQ_OUTFACTOR);
			if(FreqOutDacVal > DAC_MULFACTOR)//Make sure we don't cross the DAC limits in any case
				FreqOutDacVal = DAC_MULFACTOR;			
			else if(FreqOutDacVal < 0)			
				FreqOutDacVal = 0;			
			UserIO::SetOutputFrequency(FreqOutDacVal);
		}
		switch(UserIO::CheckForEnergyRange())
		{
			case ENERGY_CONTROL_RANGE1:
				EnergyOut = ((AccumulatedEnergy / ENERGY_MULT_FAC)* ePromData.PSpower)/ADC11BITVALUE;
				EnergyOut = ((EnergyOut) * ADC11BITVALUE)/ ENERGYRANGE_1_MAX;
				break;
			case ENERGY_CONTROL_RANGE2:
				EnergyOut = ((AccumulatedEnergy / ENERGY_MULT_FAC)* ePromData.PSpower)/ADC11BITVALUE;
				EnergyOut = ((EnergyOut) * ADC11BITVALUE)/ ENERGYRANGE_2_MAX;
				break;
			case ENERGY_CONTROL_RANGE3:
				EnergyOut = ((AccumulatedEnergy / ENERGY_MULT_FAC)* ePromData.PSpower)/ADC11BITVALUE;
				EnergyOut = ((EnergyOut) * ADC11BITVALUE)/ ENERGYRANGE_3_MAX;
				break;
			default:
				EnergyOut = 0;
		}
		UserIO::SetOutputEnergy(EnergyOut * DAC_MULFACTOR / DAC_DIVFACTOR);
	}

	while(ManFErrorCycleCounterInc != 0)
	{
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
	while (SonicOnMilliCounterIncrement >= CONV_MILLI_MICROSEC(MAXMILLISEC_ONE_SEC)) {
		IncrementSonicOnSecCounter();/*Increments Sonics On Milli Second Counter */
		SonicOnMilliCounterIncrement = 0;
	}
	if(MCF_CLOCK_PSR & MCF_CLOCK_PSR_LOCKS)
	{
	   if(!clkLost)
       {
          Log::WriteEventLog(CLOCK_LOST, 0, 0,0,false);
          clkLost = true;
          pprintf("\n clock lost");
       }
	}
//	CyGOS::KickWatchDog();
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
	if(FirstTime == TRUE)
	{
		FirstTime = FALSE;
		CyGOS::InitWatchDog();
		CyGOS::EnableWatchDog();
	}

	if(usCount >= 100000)//100ms
	{
		CyGOS::KickWatchDog();
		usCount = 0;
		LED::Set(1,GREENLED_STATE);
		SetLED = TRUE;
	}
	if(SetLED == TRUE)
	{
		RedLedusCount += usecPerTick;
		if(RedLedusCount >= 50000) //50 ms after wd feed
		{
			LED::Set(1,REDLED_STATE);
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
	  if(MCF_RCM_RSR & MCF_RCM_RSR_WDRCORE)
           Log::WriteEventLog(WATCHDOG_RESET, 0, 0);
}

void PowerUpInitilization(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for power up initialization of scan , weld  */
/* seek global parameters . It will Recall preset,counters from FRAM.       */
/****************************************************************************/
{
	int tempLen;
	int indx = 0;
	short PowerDownReg = 0;
	unsigned int eRomDataChecksum;
	CPU::SetupCS8bit(1, 10, 8, cs1);
	CPU::SetupCS8bit(2, 20, 8, cs2);
	CPU::SetupCS8bit(3, 20, 8, cs3);

	//Initialize LOG module
	Log::Init();

	//Initialize USER IO module
	UserIO::InitUserIO();

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
	epromPtr = new EUI_24AA025E48(Board::GetI2C(0, 5, 5), I2CCHIP_24AA025E48_ADDRESS);
	tempLen = sizeof(ePromData);
	epromPtr->Read(I2C_24AA025E48_DATA_ADDR, (unsigned char *)&ePromData, tempLen, MAXI2CRETRIES);

	eRomDataChecksum = ByteCRC((UINT8*)&ePromData, sizeof(ePromData) - sizeof(ePromData.checkSum));

	if((tempLen < 0) || (ePromData.checkSum != eRomDataChecksum))
	{
		Log::WriteEventLog(CRC_ERROR, (UINT16)ePromData.checkSum, (UINT16)eRomDataChecksum, 4, true);
		SetDefaultEEPROMData();
		Midband = F20KHZMIDBAND;
		F_Factor = F20KHZF_FACTOR;
	}
	//Save fresh values of current active configuration  of digital outputs at FRAM
	//base address so that from very second power up digital outputs goes correct
	//(Note: Hardware in RevF DCP board is like that the all digital outputs go high
	//on power up.)
	//This value is used by boot loader code to write the reverse of active configuration
	//of digital outputs. This way we limit the initially high going pulse to 100 ms for
	//those outputs which are configured as active high.
	__FRAM_START = 0;
	for(indx = 1; indx < NUM_DOUT; indx++)
		__FRAM_START |= ((UserIO::GetDigOutPutActiveConfiguration((DIGITAL_OUTPUTS)indx)) << (indx -1));

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
	InitMemFreqAndTuneFreqAtPowerUp();

   // Verify Preset and scan Preset at power up.
	VerifyPresetAtPowerUp();

	//Print version String at power up
	pprintf("\n software version string = ");
	for (int i=0; i< SWVERSION_LEN; i++)
		pprintf("%c", SWVersionLabel[i]);

	pprintf("\n Power Supply Specification : ");
	pprintf("\n Power Supply frequency: %d",ePromData.PSfrequency);
	pprintf("\n Power Supply power : %d",ePromData.PSpower);
	pprintf("\n Midband = %d and FFactor = %d", Midband, F_Factor);
	pprintf("\n FlashCRC=%d",sysConfig.DCXCRC);

	sysConfig.CurrSystemType = OEM;
	CheckManFCycleParamCRC();
	MFCycleFlag = CurrentCycleParam.MfTest;
	//On powerup reatin the last value
	TempSonicsOnTimePerWeldCycle = CurrentCycleParam.sonicsOnTimePerWeldCycle;
	SetFPGAFrequency();
	RTOS::DelayMs(5);
	//Writing the maximum and minimum values once at powerup. This is a fix to prevent
	//analog output fluctuations
	UserIO::dac->WriteOutput(0 , 0);//Set the outputs to zero again.
	UserIO::dac->WriteOutput(1 , 0);//Set the outputs to zero again.
}

void UpdateFlashCRC()
/****************************************************************************/
/* Description:-                                                            */
/* This function calculate DCX Flash CRC and update it in System            */
/* Configuration .                                                          */
/****************************************************************************/
{
	UINT16 FlashCRC = 0;
	int codeSize = (int)__FLASH_END - (int)__TEXT_START;
	int flashOffset = (int)__TEXT_START - (int)__FLASH_RAM;
	unsigned char * codeImage = new unsigned char[codeSize];

	memcpy((void *) codeImage, (unsigned long *)(__FLASH_ROM + flashOffset), codeSize);
	FlashCRC = ByteCRC(codeImage, (UINT32)codeSize);

	if(sysConfig.DCXCRC != FlashCRC)
		Log::WriteEventLog(CRC_CHANGE, (UINT16)sysConfig.DCXCRC, FlashCRC);
	sysConfig.DCXCRC = FlashCRC;
	delete codeImage;
}

void CheckHWConfiguration()
/****************************************************************************/
/* Description:-                                                            */
/* This function checks for Version change, the hardware configuration at   */
/* start up and  Checks whether MI board is present or not.                 */
/****************************************************************************/
{
	if(!CheckForVersionChange())
	{
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
	switch (ePromData.PSfrequency)
	{
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
	UINT32 j;
	BOOLEAN VersionChanged = FALSE;
	int VersionChangedstatus = memcmp(DCPSoftwareVersionLabel,SWVersionLabel,SWVERSION_LEN);
	if(VersionChangedstatus != 0)
	{
		memcpy(DCPSoftwareVersionLabel,SWVersionLabel,SWVERSION_LEN);
		CurrentWorkNumber = 0;
		memcpy(&ParmName, "DEFAULT 20KHz\n", MAXPARAMNAME_LEN);
		SetDefaultValues();
		for (j= 0; j< MAX_WORK_NUMBER; j++)
		{
			SavePreset(j);
		}
		SetDefaultScanParameters();
		SaveScanParamCRC();
		UserIO::ResetUserIO();
		SetDefaultCycleParam();
		Log::WriteEventLog(VERSION_CHANGE, 0, 0);
		VersionChanged = TRUE;
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
	if (CurrentWorkNumber >= MAX_WORK_NUMBER)
		CurrentWorkNumber = 0;

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
	if(DCPCounter.CycleCounter > MAX_CYCLE_COUNTER)
	{
		DCPCounter.CycleCounter = 0;
	}
	
	if((DCPCounter.ErrorCounter)> MAX_ERROR_COUNTER)
	{
		DCPCounter.ErrorCounter = 0;
	}
	
	if(DCPCounter.PowerOnSecond > MAX_SEC_COUNTER)
	{
		DCPCounter.PowerOnSecond = 0;
	}
	
	if((DCPCounter.SecondsOfSonic) > MAX_SEC_COUNTER)
	{
		DCPCounter.SecondsOfSonic = 0;
	}
}

int CheckForMIBoard()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for checking whether MI Board is present    */
/* at power up or not.                                                      */
/*  Returns length read.If -1, then MI board is not present, else no of     */
/*  bytes read is returned.                                                 */
/****************************************************************************/
{
	unsigned char dataBuf[32];
	int length = sizeof(dataBuf);
	AT24C64 * at24c = new AT24C64(Board::GetI2C(0, 5, 5), I2CCHIP_AT24C64_ADDRESS);
	at24c->Read(I2C_AT24C64_READADDRESS, dataBuf, length, MAXI2CRETRIES);
	if (length != -1)
	{
		pprintf("\n MI Board is present");
		Log::WriteEventLog(MI_BOARD_DETECTED, 0, 0);
	}
	return length;
}

void InitMemFreqAndTuneFreqAtPowerUp()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for initializing MemoryFreq and TuneFreq    */
/* variables at power up.                                                   */
/****************************************************************************/
{
	//Memory frequency is the offset from resonance frequency calculated
	//after a successful seek ,test (if memory store option is set)
	//and in weld cycle (if memory store option is set).
	//Intial value at power up is zero
	CurrentPreset.MemoryFreq = 0;
	MemoryValue = 0;

	SavePreset(0);
}

void SetFPGAFrequency()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for calculating Frequency value used in     */
/* FPGA.                                                                    */
/****************************************************************************/
{
	FrequencyValue = MemoryValue +  FreqOffsetInput;
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
	switch(ePromData.PSfrequency)
	{
		case FREQ_20KHZ:
			freqIn =(freqIn * FREQUENCY_MULTFACTOR) / DUPS_20KHZ;
			break;
		case FREQ_30KHZ:
			freqIn =(freqIn * FREQUENCY_MULTFACTOR) / DUPS_30KHZ;
			break;
		case FREQ_40KHZ:
			freqIn =(freqIn * FREQUENCY_MULTFACTOR) / DUPS_40KHZ;
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
	BOOLEAN PresetVerifiedSuccess = TRUE,ScanVerifiedSuccess = TRUE;
	if(PresetVerifiedRequired)
	{
		PresetVerifiedRequired = FALSE;
		PresetVerifiedSuccess = VerifyPreset();
		ScanVerifiedSuccess = VerifyScanParam();
	}
	if(!PresetVerifiedSuccess)
	{
		CheckAndAssignSeekCycleBoundaryValue();
		CheckAndAssignWeldCycleBoundaryValue();
	}
	if(!ScanVerifiedSuccess)
	{
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
	cs2 = (uint8*)0x0800000D;
	FPGAVersionLabel[0] = *cs2;
	cs2 = (uint8*)0x0800000E;
	FPGAVersionLabel[1] = *cs2;
	cs2 = (uint8*)0x0800000F;
	FPGAVersionLabel[2] = *cs2;
	cs2 = (uint8*)0x08000010;
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
	int VersionChangedstatus = strcmp(FPGAVersionLabel,FPGAVERSTRINGREVB);
	pprintf("\n VersionChangedstatus=%d", VersionChangedstatus);
	if(VersionChangedstatus != 0)
	{
		pprintf("\n Revision board F");
		Adc_MultFact_Phase = (int)ADC_MULT_FACTOR_PHASE_REVC;
		Adc_MultFact_Ampl = (int)ADC_MULT_FACTOR_AMPL_REVC;
		Adc_MultFact_ExtAmpl = (int)ADC_MULT_FACTOR_EXTAMPL_REVC;
		Adc_MultFact_Curr = (int)ADC_MULT_FACTOR_CURR_REVC;
		Adc_MultFact_Temp = (int)ADC_MULT_FACTOR_TEMP_REVC;
		Adc_MultFact_Power = (int)ADC_MULT_FACTOR_POWER_REVC;
	}
	else
	{
		pprintf("\n Revision board B");
		Adc_MultFact_Phase = (int)ADC_MULT_FACTOR_PHASE_REVB;
		Adc_MultFact_Ampl = (int)ADC_MULT_FACTOR_AMPL_REVB;
		Adc_MultFact_ExtAmpl = (int)ADC_MULT_FACTOR_EXTAMPL_REVB;
		Adc_MultFact_Curr = (int)ADC_MULT_FACTOR_CURR_REVB;
		Adc_MultFact_Temp = (int)ADC_MULT_FACTOR_TEMP_REVB;
		Adc_MultFact_Power = (int)ADC_MULT_FACTOR_POWER_REVB;
	}
}

void ReadMACAddressFromEEPRom(UINT8 * macPtr)
/****************************************************************************/
	/* Description:-                                                         */
	/* This function reads the MAC Adress from EEPROM chip                   */
/****************************************************************************/
{
	int tempLen = MAC_ADDRESS_LEN;
	epromPtr->Read(I2C_24AA025E48_MACSTART_ADDRESS, macPtr, tempLen, MAXI2CRETRIES);
	if(tempLen < 0)
		memcpy(macPtr , DEFAULT_MAC_ADDR, MAC_ADDRESS_LEN);
}

void SetDefaultEEPROMData()
/****************************************************************************/
/* Description:-                                                            */
/* Set the default   frequency,power and serial numbers  					*/
/* in 24AA025E48 chip.             											*/
/****************************************************************************/
{
	ePromData.PSfrequency = FREQ_20KHZ;
	ePromData.PSpower = P2500W;
	ePromData.EnergyPowerSpecial = FALSE;
	strcpy((char *)ePromData.SerialNo, DEFAULT_SERIAL_NUMBER);
	ePromData.checkSum = ByteCRC((UINT8*)&ePromData, sizeof(ePromData) - sizeof(ePromData.checkSum));
	epromPtr -> Write(I2C_24AA025E48_DATA_ADDR, (UINT8 *)&ePromData, sizeof(ePromData));
}


void ModifyEEPROMdata()
{
	/****************************************************************************/
	/* Description:-                                                            */
	/* Set the values already modified in global EEPromData object into  		*/
	/* 24AA025E48 chip.             											*/
	/****************************************************************************/
	ePromData.checkSum = ByteCRC((UINT8 *)&ePromData, sizeof(ePromData) - sizeof(ePromData.checkSum));
	epromPtr -> Write(I2C_24AA025E48_DATA_ADDR, (UINT8 *)&ePromData, sizeof(ePromData));
}

void CheckManFCycleParamCRC(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for checking valid Manufacturing Cycle param*/
/* from FRAM. If CRC does not matches then default value is assigned.       */
/****************************************************************************/
{
	UINT32 ManFCycleParamCRC;
	ManFCycleParamCRC = ByteCRC((UINT8 *)&CurrentCycleParam, sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
	if(CurrentCycleParam.CRC != ManFCycleParamCRC)
	{
		printf("\n manufacturing CRC Check fails");
		Log::WriteEventLog(CRC_ERROR, (UINT16)CurrentCycleParam.CRC, (UINT16)ManFCycleParamCRC, 3, true);
		SetDefaultCycleParam();
	}
}
