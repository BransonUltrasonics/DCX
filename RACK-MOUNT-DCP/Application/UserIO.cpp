/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UserIO.cpp_v   1.12.1.7.1.3   22 Sep 2016 11:26:38   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UserIO.cpp_v  $
 * 
 *    Rev 1.12.1.7.1.3   22 Sep 2016 11:26:38   EGUAJARDO
 * Applied coding standard.
 * 
 *    Rev 1.12.1.7.1.2   15 Sep 2016 15:33:24   EGUAJARDO
 * Added two new energy ranges 10J and 30J.
 * 
 *    Rev 1.12.1.7.1.1   25 Jul 2016 12:48:32   GButron
 * RACKMOUNT_BASIC changed to DCX_S
 *
 *    Rev 1.12.1.7.1.0   06 Jun 2016 16:14:04   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.12.1.7   18 Jun 2015 03:59:22   sesharma
 * User IOs should be configured from FRAM for DC-Basic at Power Up. As JP3 configuration can be changed from weld preset page.
 * 
 *    Rev 1.12.1.6   04 Jun 2015 07:53:22   sesharma
 * Type casting for Data1 & Data2 fields changed to SINT32 when writing an event. Branson coding standard formatter applied.
 * 
 *    Rev 1.12.1.5   09 Apr 2015 05:31:46   sesharma
 * Energy Input Fnction initialized for 10K energy range at Power Up.
 * 
 *    Rev 1.12.1.4   19 Mar 2015 07:12:00   sesharma
 * Branson Coding Standard Applied and Analog Input for 10K Energy Range Added.
 * 
 *    Rev 1.12.1.3   26 Feb 2015 08:31:32   sesharma
 * Unused Function Removed.
 * 
 *    Rev 1.12.1.2   18 Feb 2015 01:00:14   SaPawar
 * function for getting power window limit alarm added
 * 
 *    Rev 1.12.1.0   13 Jan 2015 06:02:42   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.12   07 Nov 2014 16:24:52   ewomack
 * IO Defaults for DC Std.
 * 
 *    Rev 1.11   28 Oct 2014 17:25:10   ewomack
 * Bitshift error
 * 
 *    Rev 1.10   28 Oct 2014 16:53:08   ewomack
 * Bugfix for system type detection at powerup
 * 
 *    Rev 1.9   22 Oct 2014 19:05:16   ewomack
 * Fixed bug where Preset Information was being lost while changing system type.  Issue was Preset being saved to FRAM before being recalled from FRAM.
 * 
 *    Rev 1.8   21 Oct 2014 14:32:02   ewomack
 * Fixed bug where extra events were being logged on Powerup for DC Basic.
 * 
 *    Rev 1.7   21 Oct 2014 14:28:32   ewomack
 * Force user IO to default while system type is Rackmount Basic.  Make all Digital Outputs active low for Basic.
 * 
 *    Rev 1.6   16 Oct 2014 03:49:42   sesharma
 * Tracker Issues Resolved
 * 
 *    Rev 1.5   25 Sep 2014 03:28:06   amaurya
 * Replaced General Alarm for Overload alarm in Rack Basic, Standard .
 * 
 *    Rev 1.4   19 Sep 2014 08:35:18   amaurya
 * Correct amplitude MinRaw value for Rack S.
 * 
 *    Rev 1.3   18 Sep 2014 10:55:32   amaurya
 * Changed amplitude signal down to 20% from 50%
 * 
 *    Rev 1.2   17 Sep 2014 09:05:20   amaurya
 * Renamed defines. 
 * 
 *    Rev 1.1   03 Sep 2014 07:44:04   amaurya
 * UserIO defaults resolved.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:30   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:06   amaurya
 * Initial revision.
 */

//This module is the part of DCP Application code
//This module gives the interface to read and write digital and analog
//signals between DCP and MI board. The DCP state machine should use the
//function of this class (which are all static) to determine the UserIO
//status.In all comments in this module input signal means the signal coming
//from MI board to DCP board. The output signal means the signal going from
//DCP board to MI board.
#include "UserIO.h"
#include "SPI.h"
#include "Board.h"
#include "SysConfig.h"
#include "DEF_DEF.h"
#include "FUNC.h"
#include "LOG.h"
#include "GLOBALS.h"
#include "DebugNew.h"

UINT32 PresentConfiguration;

//ADC and DAC objects definition
TLV5604 * UserIO::dac;
MCP3208 * UserIO::adc;
#define DSPICSPIN_DAC   1
#define DSPICSPIN_ADC   2
//Input Flags
static BOOLEAN ExtReset, ExtAmpStep, ExtSeek, RecallPreset1, RecallPreset2,
      RecallPreset4, RecallPreset8, RecallPreset16, RecallPreset32, ExtStart,
      ExtSonicsDelay, LockOutChangeLCD, SonicsDisable, InterLockInPlace,
      LoadNewPreset, CycleAbort, GroundDetect, StartHornScan, ExtTest,
      HornDownMode, MemClear, CableDetect;

static BOOLEAN CableDetectEnabled;
static UINT16 EnergyRangeSelection;
//OutputFlags
static BOOLEAN ReadyFlg, SonicsActive, GenAlarm, Beeper, CycleOK, PresetChanged,
      DummyOutput, HoldTimeActive, AfterBurstDelayActive, AfterBurstTimeActive,
      PlusTimeLimitAlarm, MinusTimeLimitAlarm, PlusEnergyLimitAlarm,
      MinusEnergyLimitAlarm, PlusPeakPowerLimitAlarm, MinusPeakPowerLimitAlarm,
      OverloadAlarm, ActuaterHome, WeldCycleComplete,
      PreloadedCycleCountReached, InHorndownMode, EndOfHoldTime, Amplitude1_2,
      StartSignalRelease, SeekOut, RFSwitchSelectA, RFSwitchSelectB,
      RFSwitchSelectC, RFSwitchSelectD, CycleStart, CustomAlarm,
      PlusPeakPowerLimit, MinusPeakPowerLimit;

BOOLEAN *InValue[NUM_DIN];   // __attribute__ ((section (".FRAM")));		// pointer to input variables
BOOLEAN *OutValue[NUM_DOUT];   // __attribute__ ((section (".FRAM")));		// pointer to output variables

/* This table contains a pointer to the BOOLEAN variable assigned to each input function number.*/
/* The variable is always used as active high.  Any conversion is done in I/O device driver.             */
UINT16 InBitsHi[NUM_DIN] = { 0, DIN1_HI, DIN2_HI, DIN3_HI, DIN4_HI };
UINT16 InBitsLo[NUM_DIN] = { 0, DIN1_LO, DIN2_LO, DIN3_LO, DIN4_LO };

//copy of FRAM variables in RAM
UINT16 UserIO::InputMask;
UINT16 UserIO::OutputMask;
BOOLEAN UserIO::DigInUseStatus[NUM_DIN];
BOOLEAN UserIO::DigOutUseStatus[NUM_DOUT];
BOOLEAN UserIO::AnalogInUseStatus[NUM_AIN];
BOOLEAN UserIO::AnalogOutUseStatus[NUM_AOUT];

BOOLEAN OldGenAlarm = FALSE;

//max last ADC values used for software filtering of analog signals
#define MAXADCVALUES_FILTER  3
//buffers to hold last 3 read analog input values for all analog inputs
static SINT32 BufferAin[NUM_AIN][MAXADCVALUES_FILTER];
//Pointers to the buffers holding last MAXADCVALUES_FILTER analog input values. Used in filtering logic
static SINT32 * AinFilterPtr[NUM_AIN] = { 0, &BufferAin[AIN1][0],
      &BufferAin[AIN2][0] };
//filtered value of analog inputs
static SINT32 AinValue[NUM_AIN];
//Special buffer for storing diagnostic outputs
static UINT8 DiagOutputs = 0;

DIGITALIOMAP InPtr[NUM_FUNC_IN + 1] = {

/* Flag					Active		Control	#					Description                               */
/*-----------			-------		--------						---------------------------------------*/
{ 0, LOW, (RMBASIC_S_STD) }, /* 0	Not used                            */
{ &ExtReset, HIGH, (RMBASIC_S_STD) }, /* 1	External reset active high          */
{ &ExtReset, LOW, (RMBASIC_S_STD) }, /* 2	External reset active low           */
{ &ExtSeek, HIGH, (RMBASIC_S_STD) }, /* 3	External seek active high           */
{ &ExtSeek, LOW, (RMBASIC_S_STD) }, /* 4	External seek active low            */
{ &ExtStart, HIGH, (RMBASIC_S_STD) }, /* 5	External Trigger/Start active high  */
{ &ExtStart, LOW, (RMBASIC_S_STD) }, /* 6	External Trigger/Start active low   */
{ &ExtTest, HIGH, (RACKMOUNT_S) }, /* 7	External Test active high           */
{ &ExtTest, LOW, (RACKMOUNT_S) }, /*8    External test active low           */
{ &MemClear, HIGH, (RACKMOUNT_S) }, /* 9	MemClear active high                */
{ &MemClear, LOW, (RACKMOUNT_S) }, /* 10	MemClear active low                 */
{ &StartHornScan, HIGH, (RACKMOUNT_S) }, /* 11	Start HornScan  active high         */
{ &StartHornScan, LOW, (RACKMOUNT_S) }, /* 12	Start HornScan  active low          */
{ &RecallPreset1, HIGH, (NOTUSED) }, /* 13	Recall preset 1 active high only    */
{ &RecallPreset2, HIGH, (NOTUSED) }, /* 14	Recall preset 2 active high only    */
{ &RecallPreset4, HIGH, (NOTUSED) }, /* 15	Recall preset 4 active high only    */
{ &RecallPreset8, HIGH, (NOTUSED) }, /* 16	Recall preset 8 active high only    */
{ &RecallPreset16, HIGH, (NOTUSED) }, /* 17	Recall preset 16 active high only   */
{ &RecallPreset32, HIGH, (NOTUSED) }, /* 18	Recall preset 32 active high only   */
{ &ExtSonicsDelay, HIGH, (NOTUSED) }, /* 19	External sonic delay active high    */
{ &ExtSonicsDelay, LOW, (NOTUSED) }, /* 20	External sonic delay active low     */
{ &LockOutChangeLCD, HIGH, (RACKMOUNT_S) }, /* 21	Lock Out Change active high         */
{ &LockOutChangeLCD, LOW, (RACKMOUNT_S) }, /* 22	Lock Out Change active low          */
{ &SonicsDisable, HIGH, (NOTUSED) }, /* 23	Sonics Disable active high          */
{ &SonicsDisable, LOW, (NOTUSED) }, /* 24	Sonics Disable active low           */
{ &InterLockInPlace, HIGH, (NOTUSED) }, /* 25	InterLockInPlace active high        */
{ &InterLockInPlace, LOW, (NOTUSED) }, /* 26	InterLockInPlace active low         */
{ &LoadNewPreset, HIGH, (NOTUSED) }, /* 27	Load New Preset active high         */
{ &LoadNewPreset, LOW, (NOTUSED) }, /* 28	Load New Preset active low          */
{ &CycleAbort, HIGH, (NOTUSED) }, /* 29	Cycle Abort active high             */
{ &CycleAbort, LOW, (NOTUSED) }, /* 30	Cycle Abort active low              */
{ &GroundDetect, LOW, (NOTUSED) }, /* 31	Ground Detect active low            */
{ &ExtAmpStep, HIGH, (NOTUSED) }, /* 32	External amplitude step active high */
{ &ExtAmpStep, LOW, (NOTUSED) }, /* 33	External amplitude step active low  */
{ &HornDownMode, HIGH, (NOTUSED) }, /*34 HornDown Mode active high            */
{ &HornDownMode, LOW, (NOTUSED) }, /*35 HornDown Mode active high            */
{ &CableDetect, HIGH, (RMBASIC_S_STD) }, /*36 Cable detect active high only			*/

};
/* This table contains a pointer to the BOOLEAN variable assigned to each output function number.*/
/* The variable is always used as active high.  Any conversion is done in I/O device driver.
 */
UINT16 OutBitsHi[NUM_DOUT] = { 0, DOUT1_HI, DOUT2_HI, DOUT3_HI, DOUT4_HI,
      DOUT5_HI };
UINT16 OutBitsLo[NUM_DOUT] = { 0, DOUT1_LO, DOUT2_LO, DOUT3_LO, DOUT4_LO,
      DOUT5_LO };

DIGITALIOMAP OutPtr[NUM_FUNC_OUT + 1] = {
/* Flag							Active		Control	#	Description                                         */
/*-----------					------		--------		--	-------------------------------------------------*/
{ 0, LOW, (RMBASIC_S_STD) }, /* 0	Not used                                */
{ &ReadyFlg, HIGH, (RMBASIC_S_STD) }, /* 1	Ready active high                       */
{ &ReadyFlg, LOW, (RMBASIC_S_STD) }, /* 2	Ready active low                        */
{ &SonicsActive, HIGH, (RMBASIC_S_STD) }, /* 3	WeldOn active high                      */
{ &SonicsActive, LOW, (RMBASIC_S_STD) }, /* 4	WeldOn active low                       */
{ &GenAlarm, HIGH, (RMBASIC_S_STD) }, /* 5	General alarm active high               */
{ &GenAlarm, LOW, (RMBASIC_S_STD) }, /* 6	General alarm active low                */
{ &OverloadAlarm, HIGH, (RMBASIC_S_STD) }, /* 7	OverloadAlarm active high               */
{ &OverloadAlarm, LOW, (RMBASIC_S_STD) }, /* 8	OverloadAlarm active low                */
{ &SeekOut, HIGH, (RACKMOUNT_S) }, /* 9	seek out active high                    */
{ &SeekOut, LOW, (RACKMOUNT_S) }, /* 10	seek out active low                     */
{ &DummyOutput, HIGH, (RMBASIC_S_STD) }, /* 11 DummyOutput active high              */
{ &DummyOutput, LOW, (RMBASIC_S_STD) }, /* 12 DummyOutput active low               */
{ &HoldTimeActive, HIGH, (NOTUSED) },/* 13	HoldTimeActive active high              */
{ &HoldTimeActive, LOW, (NOTUSED) },/* 14	HoldTimeActive active low               */
{ &AfterBurstDelayActive, HIGH, (NOTUSED) },/* 15	AfterBurstDelayActive active high       */
{ &AfterBurstDelayActive, LOW, (NOTUSED) },/* 16	AfterBurstDelayActive active low        */
{ &AfterBurstTimeActive, HIGH, (NOTUSED) },/* 17	AfterBurstTimeActive active high        */
{ &AfterBurstTimeActive, LOW, (NOTUSED) },/* 18	AfterBurstTimeActive active low         */
{ &PlusTimeLimitAlarm, HIGH, (NOTUSED) },/* 19	PlusTimeLimitAlarm  active high         */
{ &PlusTimeLimitAlarm, LOW, (NOTUSED) },/* 20	PlusTimeLimitAlarm  active low          */
{ &MinusTimeLimitAlarm, HIGH, (NOTUSED) },/* 21	MinusTimeLimitAlarm active high         */
{ &MinusTimeLimitAlarm, LOW, (NOTUSED) },/* 22	MinusTimeLimitAlarm active low          */
{ &PlusEnergyLimitAlarm, HIGH, (NOTUSED) },/* 23	PlusEnergyLimitAlarm active high        */
{ &PlusEnergyLimitAlarm, LOW, (NOTUSED) },/* 24	PlusEnergyLimitAlarm active low         */
{ &MinusEnergyLimitAlarm, HIGH, (NOTUSED) },/* 25	MinusEnergyLimitAlarm active high       */
{ &MinusEnergyLimitAlarm, LOW, (NOTUSED) },/* 26	MinusEnergyLimitAlarm active low        */
{ &PlusPeakPowerLimitAlarm, HIGH, (NOTUSED) },/* 27   PlusPeakPowerLimitAlarm active high   */
{ &PlusPeakPowerLimitAlarm, LOW, (NOTUSED) },/* 28	PlusPeakPowerLimitAlarm active low      */
{ &MinusPeakPowerLimitAlarm, HIGH, (NOTUSED) },/* 29	MinusPeakPowerLimitAlarm active high    */
{ &MinusPeakPowerLimitAlarm, LOW, (NOTUSED) },/* 30	MinusPeakPowerLimitAlarm active low     */
{ &Beeper, HIGH, (NOTUSED) },/* 31	Beeper active high only                 */
{ &PresetChanged, HIGH, (NOTUSED) },/*32  Preset Changed active high              */
{ &ActuaterHome, HIGH, (NOTUSED) },/* 33	ActuaterHome active high                */
{ &ActuaterHome, LOW, (NOTUSED) },/* 34	ActuaterHome active low                 */
{ &WeldCycleComplete, HIGH, (NOTUSED) },/* 35	WeldCycleComplete active high           */
{ &WeldCycleComplete, LOW, (NOTUSED) },/* 36	WeldCycleComplete active low            */
{ &PreloadedCycleCountReached, HIGH, (NOTUSED) },/* 37	PreloadedCycleCountReached active high  */
{ &PreloadedCycleCountReached, LOW, (NOTUSED) },/* 38   PreloadedCycleCountReached active low */
{ &InHorndownMode, HIGH, (NOTUSED) },/* 39   InHorndownMode active high            */
{ &InHorndownMode, LOW, (NOTUSED) },/* 40	InHorndownMode active low               */
{ &EndOfHoldTime, HIGH, (NOTUSED) },/* 41	EndOfHoldTime active high               */
{ &EndOfHoldTime, LOW, (NOTUSED) },/* 42	EndOfHoldTime active low                */
{ &Amplitude1_2, HIGH, (NOTUSED) },/* 43	Amplitude1_2 active high only           */
{ &StartSignalRelease, HIGH, (NOTUSED) },/* 44	StartSignalRelease active high          */
{ &StartSignalRelease, LOW, (NOTUSED) },/* 45	StartSignalRelease active low           */
{ &CycleOK, HIGH, (NOTUSED) },/* 46	CycleOK active high                     */
{ &CycleOK, LOW, (NOTUSED) },/* 47	CycleOK active low                      */
{ &RFSwitchSelectA, HIGH, (NOTUSED) },/* 48	StartSignalRelease active high          */
{ &RFSwitchSelectB, LOW, (NOTUSED) },/* 49	StartSignalRelease active low           */
{ &RFSwitchSelectC, HIGH, (NOTUSED) },/* 50	CycleOK active high                     */
{ &RFSwitchSelectD, LOW, (NOTUSED) },/* 51	CycleOK active low                      */
{ &CycleStart, HIGH, (NOTUSED) },/* 52	StartSignalRelease active low           */
{ &CycleStart, LOW, (NOTUSED) },/* 53	CycleOK active high                     */
{ &CustomAlarm, HIGH, (NOTUSED) },/* 54	CycleOK active low                      */
{ &CustomAlarm, LOW, (NOTUSED) },/* 55	CycleOK active low                      */
{ &PlusPeakPowerLimit, HIGH, (RACKMOUNT_STD) },/* 56	PlusPeakPower active high               */
{ &PlusPeakPowerLimit, LOW, (RACKMOUNT_STD) },/* 57	PlusPeakPower active low                */
{ &MinusPeakPowerLimit, HIGH, (RACKMOUNT_STD) },/* 58	MinusPeakPower active high              */
{ &MinusPeakPowerLimit, LOW, (RACKMOUNT_STD) }, /* 59	MinusPeakPower active low               */
};

/*Analog IOs*/
SINT32 FreqIn = 0;
SINT32 AmplitudeIn = 0;
SINT32 EnergyIn = 0;
static SINT32 PowerOut = 0;
static SINT32 PowerOut1 = 0;
static SINT32 PowerOut2 = 0;
static SINT32 PowerOut3 = 0;
static SINT32 PowerOut4 = 0;
static SINT32 PowerOut5 = 0;
static SINT32 PowerOut6 = 0;
static SINT32 PowerOut7 = 0;
static SINT32 PowerOut8 = 0;
static SINT32 PowerOut9 = 0;
static SINT32 PowerOut10 = 0;
static SINT32 PowerOut11 = 0;
static SINT32 PowerOut12 = 0;
static SINT32 PowerOut13 = 0;
static SINT32 AmplitudeOut = 0;
static SINT32 FreqOut = 0;
static SINT32 MemOut = 0;
static SINT32 EnergyOut = 0;

SINT32 *AnalogInValue[NUM_AIN];   // pointer to input variables

SINT32 *AnalogOutValue[NUM_AOUT];   // pointer to output variables

ANALOGIOMAP AInPtr[NUM_FUNC_AIN + 1] = { { 0, (RMBASIC_S_STD) }, { &FreqIn,
      (RMBASIC_S) },   							//1
      { &AmplitudeIn, (RMBASIC_S_STD) },  //2
      { &EnergyIn, (RACKMOUNT_S) },   		//3
      { &EnergyIn, (RACKMOUNT_S) },   		//4
      { &EnergyIn, (RACKMOUNT_S) },   		//5
     	{ &EnergyIn, (RACKMOUNT_S) },	 		//6
      { &EnergyIn, (RACKMOUNT_S) }   		//7
};

ANALOGIOMAP AOutPtr[NUM_FUNC_AOUT + 1] = { { 0, (RMBASIC_S_STD) }, { &PowerOut,
      (RMBASIC_S_STD) },   //1
      { &AmplitudeOut, (RMBASIC_S_STD) },   //2
      { &MemOut, (RMBASIC_S_STD) },   //3
      { &FreqOut, (RMBASIC_S_STD) },   //4
      { &EnergyOut, (RMBASIC_S_STD) },   //5
      { &PowerOut1, (RACKMOUNT_S) },   //6
      { &PowerOut2, (RACKMOUNT_S) },   //7
      { &PowerOut3, (RACKMOUNT_S) },   //8
      { &PowerOut4, (RACKMOUNT_S) },   //9
      { &PowerOut5, (RACKMOUNT_S) },   //10
      { &PowerOut6, (RACKMOUNT_S) },   //11
      { &PowerOut7, (RACKMOUNT_S) },   //12
      { &PowerOut8, (RACKMOUNT_S) },   //13
      { &PowerOut9, (RACKMOUNT_S) },   //14
      { &PowerOut10, (RACKMOUNT_S) },   //15
      { &PowerOut11, (RACKMOUNT_S) },   //16
      { &PowerOut12, (RACKMOUNT_S) },   //17
      { &PowerOut13, (RACKMOUNT_S) },   //18
      };

UINT16 InChannles[NUM_AIN] = { 0, AIN1_CHANNEL, AIN2_CHANNEL };
UINT16 OutChannles[NUM_AOUT] = { 0, AOUT1_CHANNEL, AOUT2_CHANNEL };

extern SINT16 CurrentIndex;
extern UINT32 AccumulatedEnergy; /* the power is integrated to energy */

void UserIO::InitUserIO()
/******************************************************************************/
/* Initialize the adc and dac chips. Both chips uses same SPI instance        */
/* to read/write values from/to MI board                                     */
/******************************************************************************/
{
   int adcOutVal = DAC_MULFACTOR;
   //Initialize SPI instance to be used by ADC and DAC
   SPI * qspi = Board::GetSPI(0, 5, 5);
   //Initialize ADC and DAC objects
   dac = new TLV5604(qspi, DSPICSPIN_DAC, REFVOLTAGE_DAC, REFVOLTAGE_DAC);
   adc = new MCP3208(qspi, DSPICSPIN_ADC, REFVOLTAGE_ADC);
   //fix for fluctuating analog OP's. Values get written back to zero again at the end of power up initialization.
   dac->WriteOutput(0, adcOutVal);   //Initially write maximum values on both DAC channels
   dac->WriteOutput(1, adcOutVal);   //Initially write maximum values on both DAC channels
   PresentConfiguration = BIT(EPromData.Systemtype);
   if (validIOConfiguration != IOCONFIGURED_CHECKSUM) {
      InitFRAMIOConfig();
      ConfigureDefaultUserIOs();
      Log::WriteEventLog(CRC_ERROR, (SINT32) validIOConfiguration,
            (SINT32) IOCONFIGURED_CHECKSUM, 7, true);
      validIOConfiguration = IOCONFIGURED_CHECKSUM;
      pprintf("\n IO Default Initialized ");
   }
   else if (BIT(EPromData.Systemtype) & RACKMOUNT_STD) {
	  InitFRAMIOConfig();
      ConfigureDefaultUserIOs(false);
      validIOConfiguration = IOCONFIGURED_CHECKSUM;
      pprintf("\n IO Default Initialized ");
   }
   else if (BIT(EPromData.Systemtype) & DCX_S) {
      //Analog outputs on JP3 can be configured from weld preset page i.e. Freq out & Amp Out
	  ConfigureUserIOsFromFRAM();
      validIOConfiguration = IOCONFIGURED_CHECKSUM;
      pprintf("\n IO Default Initialized ");
   }
   else { //For Rack-S System type.
      ConfigureUserIOsFromFRAM();
      InitCableDetectInput();
      InitEnergyRangeSelection();
      pprintf("\n IO Initialized from FRAM");
   }
   pprintf("\n IO Init done");
}

void UserIO::ResetUserIO()
{
   InitFRAMIOConfig();
   PresentConfiguration = BIT(EPromData.Systemtype);
   ConfigureDefaultUserIOs();
   validIOConfiguration = IOCONFIGURED_CHECKSUM;
   //printf("\r\n PresentConfig %d", PresentConfiguration);
}

void UserIO::ConfigureUserIOsFromFRAM()
{
   //Defaults for digital Inputs
   UserIO::AssignDigitalInputFunction(DIGIN1, USERIOConfig.DigInUse[DIGIN1],
         USERIOConfig.DigInUseStatus[DIGIN1]);
   UserIO::AssignDigitalInputFunction(DIGIN2, USERIOConfig.DigInUse[DIGIN2],
         USERIOConfig.DigInUseStatus[DIGIN2]);
   UserIO::AssignDigitalInputFunction(DIGIN3, USERIOConfig.DigInUse[DIGIN3],
         USERIOConfig.DigInUseStatus[DIGIN3]);
   UserIO::AssignDigitalInputFunction(DIGIN4, USERIOConfig.DigInUse[DIGIN4],
         USERIOConfig.DigInUseStatus[DIGIN4]);
   //Defaults for digital Outputs
   UserIO::AssignDigitalOutputFunction(DIGOUT1, USERIOConfig.DigOutUse[DIGOUT1],
         USERIOConfig.DigOutUseStatus[DIGOUT1]);
   UserIO::AssignDigitalOutputFunction(DIGOUT2, USERIOConfig.DigOutUse[DIGOUT2],
         USERIOConfig.DigOutUseStatus[DIGOUT2]);
   UserIO::AssignDigitalOutputFunction(DIGOUT3, USERIOConfig.DigOutUse[DIGOUT3],
         USERIOConfig.DigOutUseStatus[DIGOUT3]);
   UserIO::AssignDigitalOutputFunction(DIGOUT4, USERIOConfig.DigOutUse[DIGOUT4],
         USERIOConfig.DigOutUseStatus[DIGOUT4]);
   UserIO::AssignDigitalOutputFunction(DIGOUT5, USERIOConfig.DigOutUse[DIGOUT5],
         USERIOConfig.DigOutUseStatus[DIGOUT5]);
   //Default for Analog Inputs
   UserIO::AssignAnalogInputFunction(AIN1, USERIOConfig.AnalogInUse[AIN1],
         USERIOConfig.AnalogInUseStatus[AIN1]);
   UserIO::AssignAnalogInputFunction(AIN2, USERIOConfig.AnalogInUse[AIN2],
         USERIOConfig.AnalogInUseStatus[AIN2]);
   //Default for Analog Outputs
   UserIO::AssignAnalogOutputFunction(AOUT1, USERIOConfig.AnalogOutUse[AOUT1],
         USERIOConfig.AnalogOutUseStatus[AOUT1]);
   UserIO::AssignAnalogOutputFunction(AOUT2, USERIOConfig.AnalogOutUse[AOUT2],
         USERIOConfig.AnalogOutUseStatus[AOUT2]);
}

void UserIO::InitFRAMIOConfig()
/******************************************************************************/
/* Reset the FRAM IO contents                                                 */
/******************************************************************************/
{
   memset(&USERIOConfig, 0, sizeof(USERIOConfig));
   memset(AnalogInValue, 0, sizeof(AnalogInValue));   // pointer to input variables
   memset(AnalogOutValue, 0, sizeof(AnalogOutValue));
   memset(InValue, 0, sizeof(InValue));		// pointer to input variables
   memset(OutValue, 0, sizeof(OutValue));
}

void UserIO::InitCableDetectInput()
{
   SINT32 index = 0;
   BOOLEAN ExitFlag = FALSE;
   BOOLEAN cableDetectInput = FALSE;
   for (index = 1; ((index < NUM_DIN) && (ExitFlag == FALSE)); index++) {
      if (USERIOConfig.DigInUse[index] == CABLEDETECT_ACTIVEHIGH) {
         ExitFlag = TRUE;
         cableDetectInput = USERIOConfig.DigInUseStatus[index];
      }
   }
   SetCableDetect(cableDetectInput);
}

void UserIO::InitEnergyRangeSelection()
/******************************************************************************/
/* Initialize the energy input function if selected.                          */
/******************************************************************************/
{
   SINT32 index = 0;
   BOOLEAN ExitFlag = FALSE;
   UINT16 EnergyFuncNum = 0;
   for (index = 1; ((index < NUM_AIN) && (ExitFlag == FALSE)); index++) {
      if ((USERIOConfig.AnalogInUse[index] == ENERGY_CONTROL_RANGE1)
            || (USERIOConfig.AnalogInUse[index] == ENERGY_CONTROL_RANGE2)
            || (USERIOConfig.AnalogInUse[index] == ENERGY_CONTROL_RANGE3)
           	|| (USERIOConfig.AnalogInUse[index] == ENERGY_CONTROL_RANGE4)
            || (USERIOConfig.AnalogInUse[index] == ENERGY_CONTROL_RANGE5)) {
         if (USERIOConfig.AnalogInUseStatus[index] == ACTIVATED) {
            ExitFlag = TRUE;
            EnergyFuncNum = USERIOConfig.AnalogInUse[index];
         }
      }
   }
   SetEnergyFuncNum(EnergyFuncNum);
}

void UserIO::AssignDigitalOutputFunction(enum DIGITAL_OUTPUTS Output,
      UINT16 FuncNum, BOOLEAN Status)
/******************************************************************************/
/*@Output: User selectable digital output pin                                 */
/*@FuncNum: Function to be assigned on digital output pin if available in the */
/*          present configuration.            											*/
/******************************************************************************/
{
   if ((OutPtr[FuncNum].Control & PresentConfiguration)
         == PresentConfiguration) {
      USERIOConfig.OutputMask =
            (USERIOConfig.OutputMask & ~(OutBitsLo[Output]));	//(OutputMask & ~(OutBitsHi[Output]);		// strip out old bit
      if (Status == ACTIVATED) {
         if (OutPtr[FuncNum].Active == LOW)
            USERIOConfig.OutputMask = ((USERIOConfig.OutputMask)
                  | OutBitsLo[Output]);	// set for active low
         else
            USERIOConfig.OutputMask = ((USERIOConfig.OutputMask)
                  | OutBitsHi[Output]);	// set for active high
      }
      USERIOConfig.DigOutUseStatus[Output] = Status;
      USERIOConfig.DigOutUse[Output] = FuncNum;   // save function number assigned to input
      OutValue[Output] = OutPtr[FuncNum].Flag;	// put variable address in input pin location
      DigOutUseStatus[Output] = Status;
      OutputMask = USERIOConfig.OutputMask;
   }
   else {
      /* FUNCTION NOT AVAILABLE */
      pprintf("\n DIGITAL OUTPUT FUNCTION NOT AVAILABLE %d", FuncNum);
   }
}

void UserIO::AssignDigitalInputFunction(enum DIGITAL_INPUTS Input,
      UINT16 FuncNum, BOOLEAN Status)
/******************************************************************************/
/*@Input: User selectable digital input pin                                    */
/*@FuncNum: Function to be assigned on digital output pin if available in the */
/*          present configuration.            											*/
/******************************************************************************/
{
   if ((InPtr[FuncNum].Control & PresentConfiguration)
         == PresentConfiguration) {
      IsIOConfigured = TRUE;
      (USERIOConfig.InputMask) =
            ((USERIOConfig.InputMask) & ~(InBitsLo[Input]));   //(InputMask AND ~(INBITSHI[Input])		// strip out old bit
      if (InPtr[FuncNum].Active == LOW)
         USERIOConfig.InputMask = ((USERIOConfig.InputMask) | InBitsLo[Input]);	// set for active low
      else
         USERIOConfig.InputMask = ((USERIOConfig.InputMask) | InBitsHi[Input]);	// set for active high
      //check whether some function was already assigned at input pin
      //and now the pin is being unassigned.
      //If yes make the previous flag inactive
      if ((USERIOConfig.DigInUse[Input] != FuncNum)
            || (Status == DEACTIVATED)) {
         if (InValue[Input]) *(InValue[Input]) = FALSE;
      }
      USERIOConfig.DigInUseStatus[Input] = Status;
      USERIOConfig.DigInUse[Input] = FuncNum;	// save function number assigned to input
      InValue[Input] = InPtr[FuncNum].Flag;	// put variable address in input pin location
      InputMask = USERIOConfig.InputMask;
      DigInUseStatus[Input] = Status;
   }
   else {
      /* FUNCTION NOT AVAILABLE */
      pprintf("\n ANALOG INPUT FUNCTION NOT AVAILABLE %d", FuncNum);
   }
}

void UserIO::AssignAnalogOutputFunction(enum ANALOG_OUTPUTS Output,
      UINT16 FuncNum, BOOLEAN Status)
/******************************************************************************/
/*@Output: User selectable analog output pin                                    */
//*@FuncNum: Function to be assigned on digital output pin if available in the */
/*           present configuration.            											*/
/******************************************************************************/
{
   if ((AOutPtr[FuncNum].Control & PresentConfiguration)
         == PresentConfiguration) {
      // save function number assigned to input
      USERIOConfig.AnalogOutUse[Output] = FuncNum;
      USERIOConfig.AnalogOutUseStatus[Output] = Status;
      AnalogOutValue[Output] = AOutPtr[FuncNum].value;	// put variable address in input pin location
      AnalogOutUseStatus[Output] = Status;
   }
   else {
      /* FUNCTION NOT AVAILABLE */
      pprintf("\n ANALOG OUTPUT FUNCTION NOT AVAILABLE %d", FuncNum);
   }
}

void UserIO::AssignAnalogInputFunction(enum ANALOG_INPUTS Input, UINT16 FuncNum,
      BOOLEAN Status)
/******************************************************************************/
/*@Input: User selectable analog input pin                                    */
/*@FuncNum: Function to be assigned on digital output pin if available in the */
/*          present configuration.            											*/
/******************************************************************************/
{
   if ((AInPtr[FuncNum].Control & PresentConfiguration)
         == PresentConfiguration) {
      //check whether some function was already assigned at input pin
      //and now the pin is being unassigned.
      //If yes make the previous variable holding analog input value 0
      if ((USERIOConfig.AnalogInUse[Input] != FuncNum)
            || (Status == DEACTIVATED)) {
         if (AnalogInValue[Input]) *(AnalogInValue[Input]) = 0;
      }
      USERIOConfig.AnalogInUseStatus[Input] = Status;
      USERIOConfig.AnalogInUse[Input] = FuncNum;	// save function number assigned to input
      AnalogInValue[Input] = AInPtr[FuncNum].value;	// put variable address in input pin location
      AnalogInUseStatus[Input] = Status;
   }
   else {
      /* FUNCTION NOT AVAILABLE */
      pprintf(" Input function not available");
   }
}

void UserIO::ReadInputs()
/******************************************************************************/
/* Read digital inputs from MI board through memory mapped IO                 */
/* Read analog inputs through ADC chip                                        */
/******************************************************************************/
{
   UINT16 InData;			// will contain all inputs values at once
   UINT16 Loop;
   int ch = 0;
   InData = ReadDigitalIO();	// Value is raw data as read by input port and debounced
   InData = (InData ^ (InputMask));   // invert whatever signals need to be inverted.
//	/**  Now we need to take bits and convert to BOOLEAN variables for system use  **///
   for (Loop = 1; Loop < NUM_DIN; Loop++) {
      if ((InValue[Loop] != NULL) && (DigInUseStatus[Loop] == ACTIVATED)) {
         if ((InData & InBitsLo[Loop]) == InBitsLo[Loop])
            *InValue[Loop] = TRUE;
         else
            *InValue[Loop] = FALSE;
      }
   }

   //see which channlels need to be read
   for (Loop = 1; Loop < NUM_AIN; Loop++) {
      if ((AnalogInValue[Loop] != NULL)
            && (AnalogInUseStatus[Loop] == ACTIVATED)) {
         ch = (1 << InChannles[Loop]);
         *AnalogInValue[Loop] = DigFilterMISAin(adc->ReadInput(ch - 1),
               (ANALOG_INPUTS) Loop);
      }
   }
}

void UserIO::WriteOutputs()
/******************************************************************************/
/* Write digital outputs to MI board through memory mapped IO                 */
/* Write analog outputs through DAC chip                                        */
/******************************************************************************/
{
   UINT16 OutData = 0;		// will contain value to be written to output ports
   UINT16 Loop;

   /** First convert all BOOLEAN variables into a single value bit assignment **/
   for (Loop = 1; Loop < NUM_DOUT; Loop++) {
      if ((OutValue[Loop] != NULL) && (DigOutUseStatus[Loop] == ACTIVATED)) {
         if ((*OutValue[Loop] == TRUE)) OutData = (OutData | OutBitsLo[Loop]);
      }
   }

   OutData = (OutData ^ (OutputMask));
   WriteDigitalIO(OutData);

   int adcOutVal[NUM_AOUT];
   static int OldadcOutVal[NUM_AOUT];
   int ch = 0;
   //check out which channels need to be written and what values need to be written
   for (Loop = 1; Loop < NUM_AOUT; Loop++) {
      if ((AnalogOutValue[Loop] != NULL)
            && (AnalogOutUseStatus[Loop] == ACTIVATED)) {
         adcOutVal[Loop - 1] = *AnalogOutValue[Loop];
         ch = (1 << OutChannles[Loop]);
         OldadcOutVal[Loop - 1] = *AnalogOutValue[Loop];
         dac->WriteOutput(ch - 1, adcOutVal[Loop - 1]);

      }
      else {
         adcOutVal[Loop - 1] = 0;
         ch = (1 << OutChannles[Loop]);
         OldadcOutVal[Loop - 1] = 0;
         dac->WriteOutput(ch - 1, adcOutVal[Loop - 1]);
      }
   }
}


int UserIO::ReadDigitalIO()
/******************************************************************************/
/* Read the memory mapped IO contents                                         */
/******************************************************************************/
{
   return DIGITAL_IO_VAL;
}


/*******************************************************************************************************
 *  SINT32 UserIO::ReadDigitalIOFT(void)                                                                   *
 *                                                                                                     *
 *  Purpose:                                                                                           *
 *     Read the memory mapped IO contents for the FT Commands                                          *
 *                                                                                                     *
 *  Entry condition:                                                                                   *
 *     Nothing                                                                                         *
 *                                                                                                     *
 *                                                                                                     *
 *  Exit condition:                                                                                    *
 *     Returns DIGITAL_IO_VAL                                                                           *
 ******************************************************************************************************/
SINT32 UserIO::ReadDigitalIOFT(void)
{
   return DIGITAL_IO_VAL;
}


void UserIO::WriteDigitalIO(int val)
/******************************************************************************/
/* Write the memory mapped IO contents                                                */
/******************************************************************************/
{
   if (WebSite::DiagStartFlag) {
      DIGITAL_IO_VAL = ~DiagOutputs;
   }
   else
      DIGITAL_IO_VAL = ~val;
}

void UserIO::ConfigureDefaultUserIOs(bool log)
/******************************************************************************/
/* configure the digital and analog IO pins to                                */
/* default functions.                                                          */
/******************************************************************************/
{
   //Note:If you change the default configuration from code make sure you modify
   //the value of #define IOCONFIGURED_CHECKSUM so that InitUserIO function
   //would call this function to assign the changed default configuration
   //Defaults for digital Inputs
   if (log) Log::WriteEventLog(USERIO_DEFINED, 0, 0);
   SetCableDetect(FALSE);

   UserIO::AssignDigitalInputFunction(DIGIN1, EXTSTART_ACTIVEHIGH, ACTIVATED);   //Ext start active high
   UserIO::AssignDigitalInputFunction(DIGIN2, EXTSEEK_ACTIVEHIGH, ACTIVATED);   //Ext Seek active high
   UserIO::AssignDigitalInputFunction(DIGIN3, EXTRESET_ACTIVEHIGH, ACTIVATED);   //Ext Reset active high
   //Default for Analog Inputs
   UserIO::AssignAnalogInputFunction(AIN1, AMPL_CONTROL, ACTIVATED);   // AMPLITUDE CONTROL INPUT
   UserIO::AssignAnalogInputFunction(AIN2, FREQ_CONTROL, ACTIVATED);   // Frequency CONTROL INPUT
   //Default for Analog Outputs
   UserIO::AssignAnalogOutputFunction(AOUT1, POWER_OUT, ACTIVATED);
   UserIO::AssignAnalogOutputFunction(AOUT2, AMPL_OUT, ACTIVATED);
   switch (BIT(EPromData.Systemtype)) {
   case RACKMOUNT_STD:
      UserIO::AssignDigitalInputFunction(DIGIN4, INPUTPIN_DISABLED, ACTIVATED);   //Disable Pin 4
      UserIO::AssignDigitalOutputFunction(DIGOUT1, GENALARM_ACTIVEHIGH,
            ACTIVATED);          //Ready active LOW
      UserIO::AssignDigitalOutputFunction(DIGOUT2, SONICACTIVE_ACTIVELOW,
            ACTIVATED);          //Sonics active active LOW
      UserIO::AssignDigitalOutputFunction(DIGOUT3, GENALARM_ACTIVELOW,
            ACTIVATED);          //GenAlarm active LOW
      UserIO::AssignDigitalOutputFunction(DIGOUT4, PPEAKPOWERLIMIT_ACTIVELOW,
            ACTIVATED);          //Sonics active active LOW
      UserIO::AssignDigitalOutputFunction(DIGOUT5, MPEAKPOWERLIMIT_ACTIVELOW,
            ACTIVATED);          //GenAlarm active LOW

      break;
   case DCX_S:
      UserIO::AssignDigitalInputFunction(DIGIN4, INPUTPIN_DISABLED, ACTIVATED);   //Disable Pin 4
      UserIO::AssignDigitalOutputFunction(DIGOUT1, GENALARM_ACTIVEHIGH,
            ACTIVATED);          //Ready active LOW
      UserIO::AssignDigitalOutputFunction(DIGOUT2, SONICACTIVE_ACTIVELOW,
            ACTIVATED);          //Sonics active active LOW
      UserIO::AssignDigitalOutputFunction(DIGOUT3, GENALARM_ACTIVELOW,
            ACTIVATED);          //GenAlarm active LOW
      UserIO::AssignDigitalOutputFunction(DIGOUT4, OUTPUTPIN_DISABLED,
            ACTIVATED);          //Sonics active active LOW
      UserIO::AssignDigitalOutputFunction(DIGOUT5, OUTPUTPIN_DISABLED,
            ACTIVATED);          //GenAlarm active LOW

      break;
   case RACKMOUNT_S:

      UserIO::AssignDigitalInputFunction(DIGIN4, MEMCLEAR_ACTIVEHIGH,
            ACTIVATED);          //Mem ClearActive High
      //Defaults for digital Outputs
      UserIO::AssignDigitalOutputFunction(DIGOUT1, READY_ACTIVEHIGH, ACTIVATED);   //Ready active high
      UserIO::AssignDigitalOutputFunction(DIGOUT2, SONICACTIVE_ACTIVEHIGH,
            ACTIVATED);				//Sonics active active high

      //GeneralAlam signal is not available for RACK_STD & RACK_BASIC but for Rack-S
      UserIO::AssignDigitalOutputFunction(DIGOUT3, GENALARM_ACTIVEHIGH,
            ACTIVATED);				//GenAlarm active high
      UserIO::AssignDigitalOutputFunction(DIGOUT4, SEEKOUT_ACTIVEHIGH,
            ACTIVATED);				//Seek out active high
      break;
   default:
      break;
   }
}

BOOLEAN UserIO::CheckSignalActive(BOOLEAN signal)
{
   BOOLEAN retVal = signal;
   if (CableDetectEnabled) retVal = signal && CableDetect;
   return retVal;

}
//Digital Input function definitions
BOOLEAN UserIO::CheckForExternalReset(void)
{
   return CheckSignalActive(ExtReset);
}

BOOLEAN UserIO::CheckForExternalAmpStep(void)
{
   return CheckSignalActive(ExtAmpStep);
}

BOOLEAN UserIO::CheckForExternalSeek(void)
{
   return CheckSignalActive(ExtSeek);
}

BOOLEAN UserIO::CheckForRecallPreset1(void)
{
   return CheckSignalActive(RecallPreset1);
}

BOOLEAN UserIO::CheckForRecallPreset2(void)
{
   return CheckSignalActive(RecallPreset2);
}

BOOLEAN UserIO::CheckForRecallPreset4(void)
{
   return CheckSignalActive(RecallPreset4);
}

BOOLEAN UserIO::CheckForRecallPreset8(void)
{
   return CheckSignalActive(RecallPreset8);
}

BOOLEAN UserIO::CheckForRecallPreset16(void)
{
   return CheckSignalActive(RecallPreset16);
}

BOOLEAN UserIO::CheckForRecallPreset32(void)
{
   return CheckSignalActive(RecallPreset32);
}

BOOLEAN UserIO::CheckForExternalStart(void)
{
   return CheckSignalActive(ExtStart);
}

BOOLEAN UserIO::CheckForExternalSonicsDelay(void)
{
   return CheckSignalActive(ExtSonicsDelay);
}

BOOLEAN UserIO::CheckForLockOutChange(void)
{
   return CheckSignalActive(LockOutChangeLCD);
}

BOOLEAN UserIO::CheckForSonicsDisable(void)
{
   return CheckSignalActive(SonicsDisable);
}

BOOLEAN UserIO::CheckForInterLockInPlace(void)
{
   return CheckSignalActive(InterLockInPlace);
}

BOOLEAN UserIO::CheckForLoadNewPreset(void)
{
   return CheckSignalActive(LoadNewPreset);
}

BOOLEAN UserIO::CheckForCycleAbort(void)
{
   return CheckSignalActive(CycleAbort);
}

BOOLEAN UserIO::CheckForMemoryClear(void)
{
   return CheckSignalActive(MemClear);
}

BOOLEAN UserIO::CheckForGroundDetect(void)
{
   return CheckSignalActive(GroundDetect);
}

BOOLEAN UserIO::CheckForHornScan(void)
{
   return CheckSignalActive(StartHornScan);
}

BOOLEAN UserIO::CheckForExternalTest(void)
{
   return CheckSignalActive(ExtTest);
}

BOOLEAN UserIO::CheckForHornDownMode(void)
{
   return CheckSignalActive(HornDownMode);
}

BOOLEAN UserIO::CheckForInputCable(void)
{
   return CableDetect;
}

BOOLEAN UserIO::CheckForCableDetect()
{
   return CableDetectEnabled;
}

void UserIO::SetCableDetect(BOOLEAN state)
{
   CableDetectEnabled = state;
}
//Digital Output function definitions
void UserIO::SetReady(BOOLEAN State)				// Set or clear Ready flag
{
   ReadyFlg = State;
}

void UserIO::SetSonicsActive(BOOLEAN State)
{
   SonicsActive = State;
}
void UserIO::SetGenAlarm(BOOLEAN State)
{
   GenAlarm = State;
}

BOOLEAN UserIO::GetGenAlarmFlag(void)
{
   return GenAlarm;
}

BOOLEAN UserIO::GetReadyFlag(void)
{
   return ReadyFlg;
}

void UserIO::SetBeeper(BOOLEAN State)
{
   Beeper = State;
}

void UserIO::SetSeekOut(BOOLEAN State)
{
   SeekOut = State;
}

void UserIO::SetCycleOK(BOOLEAN State)
{
   CycleOK = State;
}
void UserIO::SetPrsetChanged(BOOLEAN State)
{
   PresetChanged = State;
}
void UserIO::SetHoldTimeActive(BOOLEAN State)
{
   HoldTimeActive = State;
}
void UserIO::SetAfterBurstDelayActive(BOOLEAN State)
{
   AfterBurstDelayActive = State;
}
void UserIO::SetAfterBurstTimeActive(BOOLEAN State)
{
   AfterBurstTimeActive = State;
}
void UserIO::SetPlusTimeLimitAlarm(BOOLEAN State)
{
   PlusTimeLimitAlarm = State;
}
void UserIO::SetMinusTimeLimitAlarm(BOOLEAN State)
{
   MinusTimeLimitAlarm = State;
}
void UserIO::SePlusEnergyLimitAlarm(BOOLEAN State)
{
   PlusEnergyLimitAlarm = State;
}
void UserIO::SetMinusEnergyLimitAlarm(BOOLEAN State)
{
   MinusEnergyLimitAlarm = State;
}
void UserIO::SetPlusPeakPowerLimitAlarm(BOOLEAN State)
{
   PlusPeakPowerLimitAlarm = State;
}
void UserIO::SetMinusPeakPowerLimitAlarm(BOOLEAN State)
{
   MinusPeakPowerLimitAlarm = State;
}
void UserIO::SetOverloadAlarm(BOOLEAN State)
{
   OverloadAlarm = State;
}
void UserIO::SetActuaterHome(BOOLEAN State)
{
   ActuaterHome = State;
}
void UserIO::SetWeldCycleComplete(BOOLEAN State)
{
   WeldCycleComplete = State;
}
void UserIO::SetPreloadedCycleCountReached(BOOLEAN State)
{
   PreloadedCycleCountReached = State;
}
void UserIO::SetInHorndownMode(BOOLEAN State)
{
   InHorndownMode = State;
}
void UserIO::SetEndOfHoldTime(BOOLEAN State)
{
   EndOfHoldTime = State;
}
void UserIO::SetAmplitude1_2(BOOLEAN State)
{
   Amplitude1_2 = State;
}
void UserIO::SetStartSignalRelease(BOOLEAN State)
{
   StartSignalRelease = State;
}
void UserIO::SetPowerWindowLimitHigh(BOOLEAN State)
{
   PlusPeakPowerLimit = State;
}
void UserIO::SetPowerWindowLimitLow(BOOLEAN State)
{
   MinusPeakPowerLimit = State;
}


//Analog UserIO related func definitions
SINT32 UserIO::GetExternalAmplitude()
{
   UINT16 MinRawVal = 0;
   if ((BIT(EPromData.Systemtype) == DCX_S) || (BIT(EPromData.Systemtype) == RACKMOUNT_STD)) {
      MinRawVal = RAWVAL_TWENTYPER;
      if (AmplitudeIn < MinRawVal)
      	AmplitudeIn = MinRawVal;
   }
   else {
      MinRawVal = RAWVAL_TENPER;
      if (AmplitudeIn < MinRawVal)
      	AmplitudeIn = RAWVAL_FIFTYPER;
   }

   return AmplitudeIn;
}


SINT32 UserIO::GetExternalFrequency()
{
   SINT32 ExtFreqMv = 0;

   if(FreqIn < RAWVAL_TENPER)
   	FreqIn = RAWVAL_FIFTYPER;

   ExtFreqMv = FreqIn;
   ExtFreqMv = (ExtFreqMv - EXT_FREQ_SCALINGFACTOR);
   ExtFreqMv = (ExtFreqMv * ((SINT32) ((FREQ_OFFSET_LIMIT * F_Factor) / 100)))
         / EXT_FREQ_SCALINGFACTOR;

   ExtFreqMv = ConvertFreqtoDCPformat(ExtFreqMv);
   return ExtFreqMv;
}

SINT32 UserIO::GetRawExternalEnergyInPSForm()
/******************************************************************************/
/* Function checks for Energy input Range and the input voltage also.         */
/* if input is assigned and input voltage is bigger then minimum value for    */
/* that range then change the weld mode to ENERGY. also convert the ADC value */
/* to Raw energy value.                                                       */
/******************************************************************************/
{
   SINT32 EnergyInJoules = 0, EnergyInRawValue = 0;

   if(EPromData.EnergyPowerSpecial == TRUE){
   	switch (UserIO::CheckForEnergyRange()) {
   	case ENERGY_CONTROL_RANGE1:
   		EnergyInJoules = ((EnergyIn * ENERGYRANGE_1_MAX) / ADC11BITVALUE);
   		if (EnergyInJoules > ENERGYRANGE_1_MIN) {
   			CurrentPreset.WeldMode = ENERGYMODE;
   			EnergyInRawValue = (EnergyInJoules)
               		* ((ENERGY_MULT_FAC * ADC11BITVALUE) / EPromData.PSpower);
   		}
   		else
   		{
   			CurrentPreset.WeldMode = CONTINUOUSMODE;
   		}
   		break;
   	case ENERGY_CONTROL_RANGE2:
   		EnergyInJoules = ((EnergyIn * ENERGYRANGE_2_MAX) / ADC11BITVALUE);
   		if (EnergyInJoules > ENERGYRANGE_2_MIN) {
   			CurrentPreset.WeldMode = ENERGYMODE;
   			EnergyInRawValue = (EnergyInJoules)
               		* ((ENERGY_MULT_FAC * ADC11BITVALUE) / EPromData.PSpower);
   		}
   		else
   		{
   			CurrentPreset.WeldMode = CONTINUOUSMODE;
   		}
   		break;
   	case ENERGY_CONTROL_RANGE3:
   		EnergyInJoules = ((EnergyIn * ENERGYRANGE_3_MAX) / ADC11BITVALUE);
   		if (EnergyInJoules > ENERGYRANGE_3_MIN) {
   			EnergyInRawValue = (EnergyInJoules)
               		* ((ENERGY_MULT_FAC * ADC11BITVALUE) / EPromData.PSpower);
   			CurrentPreset.WeldMode = ENERGYMODE;
   		}
   		else
   		{
   			CurrentPreset.WeldMode = CONTINUOUSMODE;
   		}
   		break;
   	case ENERGY_CONTROL_RANGE4:
   		EnergyInJoules = ((EnergyIn * ENERGYRANGE_4_MAX) / ADC11BITVALUE);
   		if (EnergyInJoules > ENERGYRANGE_4_MIN) {
   			EnergyInRawValue = (EnergyInJoules)
							* ((ENERGY_MULT_FAC * ADC11BITVALUE) / EPromData.PSpower);
   			CurrentPreset.WeldMode = ENERGYMODE;
   		}
   		else
   		{
   			CurrentPreset.WeldMode = CONTINUOUSMODE;
   		}
   		break;
   	case ENERGY_CONTROL_RANGE5:
   		EnergyInJoules = ((EnergyIn * ENERGYRANGE_5_MAX) / ADC11BITVALUE);
   		if (EnergyInJoules > ENERGYRANGE_5_MIN) {
   			EnergyInRawValue = (EnergyInJoules)
 			* ((ENERGY_MULT_FAC * ADC11BITVALUE) / EPromData.PSpower);
   			CurrentPreset.WeldMode = ENERGYMODE;
   		}
   		else
   		{
   			CurrentPreset.WeldMode = CONTINUOUSMODE;
   		}
   		break;
   	default:
   		CurrentPreset.WeldMode = CONTINUOUSMODE;
   		EnergyInRawValue = 0;
   		break;
   	}
   }
     else
     {
   	  CurrentPreset.WeldMode = CONTINUOUSMODE;
     }

   	if (CurrentIndex == WAITING_IDX) {
   		if (CurrentPreset.WeldMode != RunningPreset.WeldMode) {
//			pprintf("\n weld mode change");
   			SavePreset(0);
   		}
   	}
   	return EnergyInRawValue;
}

void UserIO::SetOutputAmplitude(SINT32 val)
{
   AmplitudeOut = val;
}

void UserIO::SetOutputPower(SINT32 val)
/******************************************************************************/
/* Function checks for Power output Range and if output is assigned then      */
/* calculate the outout power as per the output range and set the output value*/
/******************************************************************************/
{
   SINT32 MaxWatt = 0;
   UINT16 Index = 0;
   SINT32 TempPwrOut = 0;
   SINT32 Temp = (val * DAC_MULFACTOR) / DAC_DIVFACTOR;
   //printf("-%d-",Temp);
   for (Index = 1; Index < NUM_AOUT; Index++) {
      switch (USERIOConfig.AnalogOutUse[Index]) {
      case POWER_OUT:
         MaxWatt = EPromData.PSpower;
         PowerOut = (EPromData.PSpower * Temp) / MaxWatt;
         break;
      case POWER_OUT400W:
         MaxWatt = 400;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut1 = TempPwrOut;
         break;
      case POWER_OUT750W:
         MaxWatt = 750;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut2 = TempPwrOut;
         break;
      case POWER_OUT800W:
         MaxWatt = 800;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut3 = TempPwrOut;
         break;
      case POWER_OUT1100W:
         MaxWatt = 1100;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut4 = TempPwrOut;
         break;
      case POWER_OUT1250W:
         MaxWatt = 1250;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut5 = TempPwrOut;
         break;
      case POWER_OUT1500W:
         MaxWatt = 1500;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut6 = TempPwrOut;
         break;
      case POWER_OUT1600W:
         MaxWatt = 1600;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut7 = TempPwrOut;
         break;
      case POWER_OUT2200W:
         MaxWatt = 2200;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut8 = TempPwrOut;
         break;
      case POWER_OUT2400W:
         MaxWatt = 2400;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut9 = TempPwrOut;
         break;
      case POWER_OUT2500W:
         MaxWatt = 2500;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut10 = TempPwrOut;
         break;
      case POWER_OUT3000W:
         MaxWatt = 3000;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut11 = TempPwrOut;
         break;
      case POWER_OUT3300W:
         MaxWatt = 3300;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut12 = TempPwrOut;
         break;
      case POWER_OUT4000W:
         MaxWatt = 4000;
         TempPwrOut = (EPromData.PSpower * Temp) / MaxWatt;
         if (TempPwrOut > DAC_MULFACTOR) TempPwrOut = DAC_MULFACTOR;
         PowerOut13 = TempPwrOut;
         break;
      default:
         break;
      }
   }
}

void UserIO::SetOutputFrequency(SINT32 val)
{
   FreqOut = val;
}

void UserIO::SetOutputMem(SINT32 val)
{
   MemOut = val;
}

void UserIO::SetOutputEnergy(SINT32 val)
{
   EnergyOut = val;
}

int UserIO::GetDigINPutActiveStatus(DIGITAL_INPUTS digIn)
/******************************************************************************/
/* @digIn:The input pin whose active status needs to be determined            */
/* OUTPUT:-1 unassigned 0 inactive 1 active
 ******************************************************************************/
{
   int ret = -1;
   if ((USERIOConfig.DigInUse[digIn] > 0)
         && (USERIOConfig.DigInUseStatus[digIn] == ACTIVATED))
      ret = ((*(InValue[digIn])) ? 1 : 0);

   return ret;
}

int UserIO::GetDigOutPutActiveStatus(DIGITAL_OUTPUTS digOut)
/******************************************************************************/
/* @digOut:The output pin whose active status needs to be determined          */
/* OUTPUT:-1 unassigned 0 inactive 1 active
 *******************************************************************************/
{
   OldGenAlarm = UserIO::GetGenAlarmFlag();
   int ret = -1;
   if ((USERIOConfig.DigOutUse[digOut] > 0)
         && (USERIOConfig.DigOutUseStatus[digOut] == ACTIVATED))
      ret = ((*(OutValue[digOut])) ? 1 : 0);

   return ret;
}

BOOLEAN UserIO::GetDigOutPutActiveConfiguration(DIGITAL_OUTPUTS digOut)
{
   int ret = 1;
   if ((USERIOConfig.DigOutUse[digOut] > 0)
         && (USERIOConfig.DigOutUseStatus[digOut] == ACTIVATED)) ret =
         OutPtr[USERIOConfig.DigOutUse[digOut]].Active;

   return ret;
}

void UserIO::SetDigOutPutActiveStatus(DIGITAL_OUTPUTS digOut, BOOLEAN val)
/******************************************************************************/
/* @digOut:The output pin whose active status needs to be changed          */
/* @val: new status to be set on output pin                                 */
/******************************************************************************/
{
   if ((USERIOConfig.DigOutUse[digOut] > 0)
         && (USERIOConfig.DigOutUseStatus[digOut] == ACTIVATED))
      *(OutValue[digOut]) = val;
}

void UserIO::SetDiagnosticOutput(DIGITAL_OUTPUTS digOut, BOOLEAN val)
{
   if ((USERIOConfig.DigOutUse[digOut] > 0)
         && (USERIOConfig.DigOutUseStatus[digOut] == ACTIVATED)) {
      //If a signal is active High, the modulus here is 1.
      if (!(val ^ (OutPtr[USERIOConfig.DigOutUse[digOut]].Active == 1))) {
         pprintf("digOut = %d\n", digOut);
         DiagOutputs |= OutBitsLo[digOut];
      }
      else {
         DiagOutputs &= ~OutBitsLo[digOut];
      }
   }
}

SINT32 UserIO::GetAnalogINPutValue(ANALOG_INPUTS aIn)
/******************************************************************************/
/* @aIn: The input pin where analog signal value needs to be determined       */
/*OUTPUT:-1 if unassigned else Analog Signal value
 *******************************************************************************/
{
   SINT32 ret = -1;
   if ((USERIOConfig.AnalogInUse[aIn] > 0)
         && (USERIOConfig.AnalogInUseStatus[aIn] == ACTIVATED)) ret =
         *(AnalogInValue[aIn]);
   return ret;
}

SINT32 UserIO::GetAnalogOUTPutValue(ANALOG_OUTPUTS aOut)
/******************************************************************************/
/* @aOut: The output pin where analog signal value needs to be determined     */
/*OUTPUT: -1 if unassigned else the analog value we are driving on output pin
 *******************************************************************************/
{
   SINT32 ret = -1;
   if ((USERIOConfig.AnalogOutUse[aOut] > 0)
         && (USERIOConfig.AnalogOutUseStatus[aOut] == ACTIVATED)) ret =
         *(AnalogOutValue[aOut]);
   return ret;
}

void UserIO::SetAnalogOUTPutValue(ANALOG_OUTPUTS aOut, SINT32 val)
/******************************************************************************/
/* @aOut: The output pin where analog signal value needs to be driven     */
/* @val: The signal value we want to drive through DAC chip
 *******************************************************************************/
{
   if ((USERIOConfig.AnalogOutUse[aOut] > 0)
         && (USERIOConfig.AnalogOutUseStatus[aOut] == ACTIVATED))
      *(AnalogOutValue[aOut]) = val;
}

UINT16 UserIO::CheckForEnergyRange()
{
   return EnergyRangeSelection;
}

void UserIO::SetEnergyFuncNum(UINT16 EnergyFuncNum)
{
   if ((EnergyFuncNum != EnergyRangeSelection) && (CurrentIndex != WAITING_IDX)
         && (!MFCycleFlag)) {
      NoInput = TRUE;
      WeldRunComplete = TRUE;
      AccumulatedEnergy = 0;
   }
   EnergyRangeSelection = EnergyFuncNum;
}
SINT32 UserIO::DigFilterMISAin(SINT32 Element, ANALOG_INPUTS InputNumber)
/************************************************************************************************/
/*  SINT32 UserIO::DigFilterMISAin(UINT32 Element, ANALOG_INPUTS InputNumber)*/
/* 	To apply software filter on Analog inputs read from MI-S board	*/
/*  Element- Represents the value of analog input number passed in input number param.*/
/*  InputNumber- Represents the analog input number*/
/*  It returns the average of the last MAXADCVALUES_FILTER points.*/
/*  Using: (Sum of  MAXADCVALUES_FILTER values + New Value  – Last value) / MAXADCVALUES_FILTER*/
/************************************************************************************************/
{
   SINT32 Temp;   // Last Value
   if (AinFilterPtr[InputNumber]
         == &BufferAin[InputNumber][MAXADCVALUES_FILTER - 1]) {
      Temp = *(AinFilterPtr[InputNumber]);                   // Move old to temp
      *(AinFilterPtr[InputNumber]) = Element;
      AinFilterPtr[InputNumber] = &BufferAin[InputNumber][0];
   }
   else {
      Temp = *(AinFilterPtr[InputNumber]);   // Move old to temp
      *(AinFilterPtr[InputNumber]) = Element;   // Insert new into buffer
      ++AinFilterPtr[InputNumber];
   }
   AinValue[InputNumber] = AinValue[InputNumber] + Element - Temp;
   return (AinValue[InputNumber] / MAXADCVALUES_FILTER);
}

void UserIO::ClearOutputs()
{
   WriteDigitalIO(0);
}
