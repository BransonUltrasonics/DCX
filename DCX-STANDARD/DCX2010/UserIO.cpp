/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/UserIO.cpp_v   1.28   02 Jul 2014 09:15:40   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/UserIO.cpp_v  $
 * 
 *    Rev 1.28   02 Jul 2014 09:15:40   amaurya
 * Removed Amplitude Decay function.
 * 
 *    Rev 1.27   01 May 2014 10:53:52   amaurya
 * Removed TuneFreq
 * 
 *    Rev 1.26   30 Apr 2014 06:20:06   amaurya
 * Removed MemOut from AOutPtr
 * 
 *    Rev 1.25   09 Sep 2013 08:59:56   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.24   01 Mar 2013 08:16:52   amaurya
 * Added Energy Range In 250J.
 * 
 *    Rev 1.23   15 Jan 2013 04:42:50   amaurya
 * Energy Special.
 * 
 *    Rev 1.21.1.0   28 Mar 2012 11:13:52   PDwivedi
 * Added changes for simultaneous activation of digital inputs(like reset and memory clear) problem for correct operation.
 * 
 *    Rev 1.21   17 Oct 2011 07:52:58   RatneshA
 * Added Cable Detect Input related changes
 * 
 *    Rev 1.20   28 Sep 2011 03:46:06   RatneshA
 * Fixed the miss of writing max DAC value on channel 1
 * 
 *    Rev 1.19   27 Sep 2011 06:20:28   RatneshA
 * Frequency out bug fix
 * 
 *    Rev 1.18   26 Sep 2011 03:34:02   RatneshA
 * Frequency Out fix for 30 and 40 Khz PS
 * 
 *    Rev 1.17   16 Sep 2011 08:12:00   RatneshA
 * set the external amplitude and frequency to 50% if they are less than 10%
 * 
 *    Rev 1.16   16 Aug 2011 08:30:00   pdwivedi
 * code cleanup
 * 
 *    Rev 1.15   05 Aug 2011 10:34:52   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.14   02 Aug 2011 07:39:48   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.13   18 Jul 2011 10:38:38   pdwivedi
 * Added Exception Log entry changes. 
 * 
 *    Rev 1.12   15 Jul 2011 09:49:08   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.11   24 May 2011 04:19:28   ygupta
 * Startup digital output fix
 * 
 *    Rev 1.10   19 Apr 2011 10:53:10   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.9   04 Apr 2011 11:00:54   PDwivedi
 * Less than 1 Volt on Analog Inputs is considered as 5 Volt
 * 
 *    Rev 1.8   02 Mar 2011 08:24:18   PDwivedi
 * Enabled Displaylock input.
 * 
 *    Rev 1.7   14 Jan 2011 07:55:46   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.6   14 Dec 2010 11:15:14   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.5   09 Dec 2010 07:54:28   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.4   08 Dec 2010 07:42:28   PDwivedi
 * Added changes for User Configurable Analog outputs.
 * 
 *    Rev 1.3   29 Nov 2010 07:47:40   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.1   12 Nov 2010 10:52:00   nhahmed
 * Added ResetUserIO function.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:48   ygupta
 * Initial revision.

******************************************************************************/

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

CONTROL PresentConfiguration = OEM;

//ADC and DAC objects definition
TLV5604 * UserIO::dac;
MCP3208 * UserIO::adc;
#define DSPICSPIN_DAC   1
#define DSPICSPIN_ADC   2
//Input Flags
static BOOLEAN ExtReset,ExtAmpStep,ExtSeek,RecallPreset1, RecallPreset2,RecallPreset4,RecallPreset8,
			RecallPreset16,	RecallPreset32,	ExtStart,
			ExtSonicsDelay,	LockOutChangeLCD,
			SonicsDisable, InterLockInPlace,
			LoadNewPreset, CycleAbort, GroundDetect,
			StartHornScan, ExtTest,	HornDownMode,MemClear, CableDetect;
static BOOLEAN CableDetectEnabled;
static UINT16 EnergyRangeSelection;
//OutputFlags
static BOOLEAN ReadyFlg,SonicsActive ,GenAlarm,Beeper,CycleOK,PresetChanged,DummyOutput,
	HoldTimeActive, AfterBurstDelayActive, AfterBurstTimeActive,PlusTimeLimitAlarm,
	MinusTimeLimitAlarm,PlusEnergyLimitAlarm,MinusEnergyLimitAlarm,PlusPeakPowerLimitAlarm,
	MinusPeakPowerLimitAlarm,OverloadAlarm, ActuaterHome, WeldCycleComplete,
	PreloadedCycleCountReached, InHorndownMode, EndOfHoldTime,Amplitude1_2,StartSignalRelease,SeekOut;

BOOLEAN	*InValue[NUM_DIN];// __attribute__ ((section (".FRAM")));		// pointer to input variables
BOOLEAN	*OutValue[NUM_DOUT];// __attribute__ ((section (".FRAM")));		// pointer to output variables

/* This table contains a pointer to the BOOLEAN variable assigned to each input function number.*/
/* The variable is always used as active high.  Any conversion is done in I/O device driver.             */
UINT16	InBitsHi[NUM_DIN] = {0, DIN1_HI, DIN2_HI, DIN3_HI, DIN4_HI};
UINT16	InBitsLo[NUM_DIN] = {0, DIN1_LO, DIN2_LO, DIN3_LO, DIN4_LO};

//copy of FRAM variables in RAM
UINT16 UserIO::InputMask;
UINT16 UserIO::OutputMask;
BOOLEAN UserIO::DigInUseStatus[NUM_DIN];
BOOLEAN UserIO::DigOutUseStatus[NUM_DOUT];
BOOLEAN UserIO::AnalogInUseStatus[NUM_AIN];
BOOLEAN UserIO::AnalogOutUseStatus[NUM_AOUT];


//max last ADC values used for software filtering of analog signals
#define MAXADCVALUES_FILTER  3
//buffers to hold last 3 read analog input values for all analog inputs
static SINT32 BufferAin[NUM_AIN][MAXADCVALUES_FILTER];
//Pointers to the buffers holding last MAXADCVALUES_FILTER analog input values. Used in filtering logic
static SINT32 * AinFilterPtr[NUM_AIN] = {0, &BufferAin[AIN1][0], &BufferAin[AIN2][0]};
//filtered value of analog inputs
static SINT32 AinValue[NUM_AIN];


/* To disable a function from a particular version (standard, OEM) this table could contain */
/* a NULL for each function not required.                                                                               */

/* The member “Control” contains defines (OEM, STD, LCD, FB) for each configuration that this */
/* particular feature is available for.                                                                                                     */

DIGITALIOMAP InPtr[NUM_FUNC_IN + 1] = {

	/* Flag			Active		Control	#	Description                               */
	/*-----------		-------		--------		---------------------------------------*/
	{ 0,			  	   LOW,		(ALL)},		/* 0	Not used                            */
	{&ExtReset,		   HIGH,		(ALL)},		/* 1	External reset active high          */
	{&ExtReset,		   LOW,		(ALL)},		/* 2	External reset active low           */
	{&ExtSeek,		   HIGH,		(ALL)},		/* 3	External seek active high           */
	{&ExtSeek,		   LOW,		(ALL)},		/* 4	External seek active low            */
	{&ExtStart,	  	   HIGH,		(ALL)},		/* 5	External Trigger/Start active high  */
	{&ExtStart,	  	   LOW,		(ALL)},		/* 6	External Trigger/Start active low   */
	{&ExtTest, 		   HIGH,		(ALL)},		/* 7	External Test active high           */
	{&ExtTest,         LOW, 		(ALL)},		/*8    External test active low           */
	{&MemClear, 		   HIGH,		(ALL)},		/* 9	MemClear active high                */
	{&MemClear,	 	   LOW,		(ALL)},		/* 10	MemClear active low                 */
	{&StartHornScan,   HIGH,		(ALL)},		/* 11	Start HornScan  active high         */
	{&StartHornScan,   LOW,		(ALL)},		/* 12	Start HornScan  active low          */
	{&RecallPreset1,   HIGH,		(NO_CONFIG)},/* 13	Recall preset 1 active high only    */
	{&RecallPreset2,   HIGH,		(NO_CONFIG)},/* 14	Recall preset 2 active high only    */
	{&RecallPreset4,   HIGH,		(NO_CONFIG)},/* 15	Recall preset 4 active high only    */
	{&RecallPreset8,   HIGH,		(NO_CONFIG)},/* 16	Recall preset 8 active high only    */
	{&RecallPreset16,  HIGH,		(NO_CONFIG)},/* 17	Recall preset 16 active high only   */
	{&RecallPreset32,  HIGH,		(NO_CONFIG)},/* 18	Recall preset 32 active high only   */
	{&ExtSonicsDelay,  HIGH,		(NO_CONFIG)},/* 19	External sonic delay active high    */
	{&ExtSonicsDelay,  LOW,		(NO_CONFIG)},/* 20	External sonic delay active low     */
	{&LockOutChangeLCD,HIGH,		(ALL)},		/* 21	Lock Out Change active high         */
	{&LockOutChangeLCD,LOW,		(ALL)},		/* 22	Lock Out Change active low          */
	{&SonicsDisable,	HIGH,		(NO_CONFIG)},/* 23	Sonics Disable active high          */
	{&SonicsDisable,	LOW,		(NO_CONFIG)},/* 24	Sonics Disable active low           */
	{&InterLockInPlace,HIGH,		(NO_CONFIG)},/* 25	InterLockInPlace active high        */
	{&InterLockInPlace,LOW,		(NO_CONFIG)},/* 26	InterLockInPlace active low         */
	{&LoadNewPreset,	HIGH,		(NO_CONFIG)},/* 27	Load New Preset active high         */
	{&LoadNewPreset,	LOW,		(NO_CONFIG)},/* 28	Load New Preset active low          */
	{&CycleAbort,	  	HIGH,		(NO_CONFIG)},/* 29	Cycle Abort active high             */
	{&CycleAbort,	  	LOW,		(NO_CONFIG)},/* 30	Cycle Abort active low              */
	{&GroundDetect,	  	LOW,		(NO_CONFIG)},/* 31	Ground Detect active low            */
	{&ExtAmpStep,	 	HIGH,		(NO_CONFIG)},/* 32	External amplitude step active high */
	{&ExtAmpStep,	  	LOW,		(NO_CONFIG)},/* 33	External amplitude step active low  */
	{&HornDownMode,    HIGH,		(NO_CONFIG)},/*34 HornDown Mode active high            */
	{&HornDownMode,   	LOW,		(NO_CONFIG)},/*35 HornDown Mode active high            */
	{&CableDetect,		HIGH,		(ALL)},		/*36 Cable detect active high only			*/

};
/* This table contains a pointer to the BOOLEAN variable assigned to each output function number.*/
/* The variable is always used as active high.  Any conversion is done in I/O device driver.
             */
UINT16	OutBitsHi[NUM_DOUT] = {0, DOUT1_HI, DOUT2_HI, DOUT3_HI, DOUT4_HI};
UINT16	OutBitsLo[NUM_DOUT] = {0, DOUT1_LO, DOUT2_LO, DOUT3_LO, DOUT4_LO};

/* To disable a function from a particular version (standard, OEM) this table could contain */
/* a NULL for each function not required.                                                                               */

/* The member “Control” contains defines (OEM, STD, LCD, FB) for each configuration that this */
/* particular feature is available for.                                                                                                     */

DIGITALIOMAP  OutPtr[NUM_FUNC_OUT + 1] = {
	/* Flag				Active		Control	#	Description                                         */
	/*-----------		------		--------		--	-------------------------------------------------*/
	{0,		            		  LOW,		(ALL)},		/* 0	Not used                                */
	{&ReadyFlg,					  HIGH,		(ALL)},		/* 1	Ready active high                       */
	{&ReadyFlg,					  LOW,		(ALL)},		/* 2	Ready active low                        */
	{&SonicsActive,				  HIGH,		(ALL)},		/* 3	WeldOn active high                      */
	{&SonicsActive,				  LOW,		(ALL)},		/* 4	WeldOn active low                       */
	{&GenAlarm,					  HIGH,		(ALL)},		/* 5	General alarm active high               */
	{&GenAlarm,					  LOW,		(ALL)},		/* 6	General alarm active low                */
	{&OverloadAlarm,			  HIGH,		(ALL)},		/* 7	OverloadAlarm active high               */
	{&OverloadAlarm,			  LOW,		(ALL)},		/* 8	OverloadAlarm active low                */
	{&SeekOut,					  HIGH,		(ALL)},		/* 9	seek out active high                    */
	{&SeekOut,					  LOW,		(ALL)},		/* 10	seek out active low                     */
	{&DummyOutput,			  HIGH,		(ALL)},		/* 11 DummyOutput active high              */
	{&DummyOutput,			  LOW,		(ALL)},		/* 12 DummyOutput active low               */
	{&HoldTimeActive,			  HIGH,		(NO_CONFIG)},/* 13	HoldTimeActive active high              */
	{&HoldTimeActive,			  LOW,		(NO_CONFIG)},/* 14	HoldTimeActive active low               */
	{&AfterBurstDelayActive,	  HIGH,		(NO_CONFIG)},/* 15	AfterBurstDelayActive active high       */
	{&AfterBurstDelayActive,	  LOW,		(NO_CONFIG)},/* 16	AfterBurstDelayActive active low        */
	{&AfterBurstTimeActive,	  HIGH,		(NO_CONFIG)},/* 17	AfterBurstTimeActive active high        */
	{&AfterBurstTimeActive,	  LOW,		(NO_CONFIG)},/* 18	AfterBurstTimeActive active low         */
	{&PlusTimeLimitAlarm,		  HIGH,		(NO_CONFIG)},/* 19	PlusTimeLimitAlarm  active high         */
	{&PlusTimeLimitAlarm,		  LOW,		(NO_CONFIG)},/* 20	PlusTimeLimitAlarm  active low          */
	{&MinusTimeLimitAlarm,	  HIGH,		(NO_CONFIG)},/* 21	MinusTimeLimitAlarm active high         */
	{&MinusTimeLimitAlarm,	  LOW,		(NO_CONFIG)},/* 22	MinusTimeLimitAlarm active low          */
	{&PlusEnergyLimitAlarm,	  HIGH,		(NO_CONFIG)},/* 23	PlusEnergyLimitAlarm active high        */
	{&PlusEnergyLimitAlarm,	  LOW,		(NO_CONFIG)},/* 24	PlusEnergyLimitAlarm active low         */
	{&MinusEnergyLimitAlarm,	  HIGH,		(NO_CONFIG)},/* 25	MinusEnergyLimitAlarm active high       */
	{&MinusEnergyLimitAlarm,	  LOW,		(NO_CONFIG)},/* 26	MinusEnergyLimitAlarm active low        */
	{&PlusPeakPowerLimitAlarm, HIGH,		(NO_CONFIG)},/* 27   PlusPeakPowerLimitAlarm active high   */
	{&PlusPeakPowerLimitAlarm, LOW,		(NO_CONFIG)},/* 28	PlusPeakPowerLimitAlarm active low      */
	{&MinusPeakPowerLimitAlarm,HIGH,		(NO_CONFIG)},/* 29	MinusPeakPowerLimitAlarm active high    */
	{&MinusPeakPowerLimitAlarm,LOW,		(NO_CONFIG)},/* 30	MinusPeakPowerLimitAlarm active low     */
	{&Beeper,						  HIGH,		(NO_CONFIG)},/* 31	Beeper active high only                 */
	{&PresetChanged, 		     HIGH,     (NO_CONFIG)},/*32  Preset Changed active high              */
	{&ActuaterHome,		        HIGH,		(NO_CONFIG)},/* 33	ActuaterHome active high                */
	{&ActuaterHome,		        LOW,		(NO_CONFIG)},/* 34	ActuaterHome active low                 */
	{&WeldCycleComplete,	     HIGH,		(NO_CONFIG)},/* 35	WeldCycleComplete active high           */
	{&WeldCycleComplete,	     LOW,		(NO_CONFIG)},/* 36	WeldCycleComplete active low            */
	{&PreloadedCycleCountReached,HIGH,	(NO_CONFIG)},/* 37	PreloadedCycleCountReached active high  */
	{&PreloadedCycleCountReached,LOW,		(NO_CONFIG)},/* 38   PreloadedCycleCountReached active low */
	{&InHorndownMode,	        HIGH,		(NO_CONFIG)},/* 39   InHorndownMode active high            */
	{&InHorndownMode,	        LOW,		(NO_CONFIG)},/* 40	InHorndownMode active low               */
	{&EndOfHoldTime,		     HIGH,		(NO_CONFIG)},/* 41	EndOfHoldTime active high               */
	{&EndOfHoldTime,		     LOW,		(NO_CONFIG)},/* 42	EndOfHoldTime active low                */
	{&Amplitude1_2,		        HIGH,		(NO_CONFIG)},/* 43	Amplitude1_2 active high only           */
	{&StartSignalRelease,	     HIGH,		(NO_CONFIG)},/* 44	StartSignalRelease active high          */
	{&StartSignalRelease,	     LOW,		(NO_CONFIG)},/* 45	StartSignalRelease active low           */
	{&CycleOK,		           HIGH,		(NO_CONFIG)},/* 46	CycleOK active high                     */
	{&CycleOK,		           LOW,		(NO_CONFIG)},/* 47	CycleOK active low                      */
};

/*Analog IOs*/
SINT32 freqIn, amplitudeIn,EnergyIn;

static SINT32 powerOut,powerOut1,powerOut2,powerOut3,
powerOut4,powerOut5,powerOut6,powerOut7,powerOut8,powerOut9,
powerOut10,powerOut11,powerOut12,powerOut13,amplitudeOut, freqOut, memOut,EnergyOut;

SINT32	*AnalogInValue[NUM_AIN];// __attribute__ ((section (".FRAM")));		// pointer to input variables

SINT32	*AnalogOutValue[NUM_AOUT];// __attribute__ ((section (".FRAM")));		// pointer to output variables

ANALOGIOMAP AInPtr[NUM_FUNC_AIN + 1] =
{
		{0,  (ALL)},
		{&freqIn, (ALL)},//1
		{&amplitudeIn ,(ALL)},//2
		{&EnergyIn, (ALL)},//3
		{&EnergyIn ,(ALL)}//4
};

ANALOGIOMAP AOutPtr[NUM_FUNC_AOUT + 1] =
{
		{0, (ALL)},
		{&powerOut , (ALL)},//1
		{&amplitudeOut, (ALL)},//2
		{&memOut, (ALL)},//3
		{&freqOut, (ALL)},//4
		{&EnergyOut, (ALL)},//5
		{&powerOut1 , (ALL)},//6
		{&powerOut2 , (ALL)},//7
		{&powerOut3 , (ALL)},//8
		{&powerOut4 , (ALL)},//9
		{&powerOut5 , (ALL)},//10
		{&powerOut6 , (ALL)},//11
		{&powerOut7 , (ALL)},//12
		{&powerOut8 , (ALL)},//13
		{&powerOut9 , (ALL)},//14
		{&powerOut10 , (ALL)},//15
		{&powerOut11 , (ALL)},//16
		{&powerOut12 , (ALL)},//17
		{&powerOut13 , (ALL)},//18
};

UINT16	InChannles[NUM_AIN] = {0, AIN1_CHANNEL, AIN2_CHANNEL};
UINT16	OutChannles[NUM_AOUT] = {0, AOUT1_CHANNEL, AOUT2_CHANNEL};

extern SINT16 CurrentIndex;
extern UINT32 AccumulatedEnergy; /* the power is integrated to energy */
/*
 *
 */
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
	dac->WriteOutput(0 , adcOutVal);//Initially write maximum values on both DAC channels
	dac->WriteOutput(1 , adcOutVal);//Initially write maximum values on both DAC channels
	if(validIOConfiguration != IOCONFIGURED_CHECKSUM)
	{
		InitFRAMIOConfig();
		ConfigureDefaultUserIOs();
		Log::WriteEventLog(CRC_ERROR, (UINT16)validIOConfiguration, (UINT16)IOCONFIGURED_CHECKSUM, 7, true);
		validIOConfiguration = IOCONFIGURED_CHECKSUM;
		pprintf("\n IO Default Initialized ");
	}
	else
	{
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
	ConfigureDefaultUserIOs();
	validIOConfiguration = IOCONFIGURED_CHECKSUM;
}

void UserIO::ConfigureUserIOsFromFRAM()
{
	//Defaults for digital Inputs
	UserIO::AssignDigitalInputFunction(DIGIN1 , USERIOConfig.DigInUse[DIGIN1], USERIOConfig.DigInUseStatus[DIGIN1]);
	UserIO::AssignDigitalInputFunction(DIGIN2 , USERIOConfig.DigInUse[DIGIN2], USERIOConfig.DigInUseStatus[DIGIN2]);
	UserIO::AssignDigitalInputFunction(DIGIN3 , USERIOConfig.DigInUse[DIGIN3], USERIOConfig.DigInUseStatus[DIGIN3]);
	UserIO::AssignDigitalInputFunction(DIGIN4 , USERIOConfig.DigInUse[DIGIN4], USERIOConfig.DigInUseStatus[DIGIN4]);
	//Defaults for digital Outputs
	UserIO::AssignDigitalOutputFunction(DIGOUT1, USERIOConfig.DigOutUse[DIGOUT1], USERIOConfig.DigOutUseStatus[DIGOUT1]);
	UserIO::AssignDigitalOutputFunction(DIGOUT2, USERIOConfig.DigOutUse[DIGOUT2], USERIOConfig.DigOutUseStatus[DIGOUT2]);
	UserIO::AssignDigitalOutputFunction(DIGOUT3, USERIOConfig.DigOutUse[DIGOUT3], USERIOConfig.DigOutUseStatus[DIGOUT3]);
	UserIO::AssignDigitalOutputFunction(DIGOUT4, USERIOConfig.DigOutUse[DIGOUT4], USERIOConfig.DigOutUseStatus[DIGOUT4]);
	//Default for Analog Inputs
	UserIO::AssignAnalogInputFunction(AIN1, USERIOConfig.AnalogInUse[AIN1], USERIOConfig.AnalogInUseStatus[AIN1]);
	UserIO::AssignAnalogInputFunction(AIN2, USERIOConfig.AnalogInUse[AIN2], USERIOConfig.AnalogInUseStatus[AIN2]);
	//Default for Analog Outputs
	UserIO::AssignAnalogOutputFunction(AOUT1,USERIOConfig.AnalogOutUse[AOUT1], USERIOConfig.AnalogOutUseStatus[AOUT1]);
	UserIO::AssignAnalogOutputFunction(AOUT2,USERIOConfig.AnalogOutUse[AOUT2], USERIOConfig.AnalogOutUseStatus[AOUT2]);
}

void UserIO::InitFRAMIOConfig()
/******************************************************************************/
/* Reset the FRAM IO contents                                                 */
/******************************************************************************/
{
	memset(&USERIOConfig , 0 , sizeof(USERIOConfig));
	memset(AnalogInValue, 0, sizeof(AnalogInValue));		// pointer to input variables
	memset(AnalogOutValue, 0, sizeof(AnalogOutValue));
	memset(InValue, 0 ,sizeof(InValue));		// pointer to input variables
	memset(OutValue, 0, sizeof(OutValue));
}

void UserIO::InitCableDetectInput()
{
	int index = 0;
	BOOLEAN ExitFlag = FALSE;
	BOOLEAN cableDetectInput = FALSE;
	for(index = 1; ((index < NUM_DIN) && (ExitFlag == FALSE)); index++)
	{
		if(USERIOConfig.DigInUse[index] == CABLEDETECT_ACTIVEHIGH)
		{
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
	int index = 0;
	BOOLEAN ExitFlag = FALSE;
	UINT16 EnergyFuncNum = 0;
	for(index = 1; ((index < NUM_AIN) && (ExitFlag == FALSE)); index++)
	{
		if((USERIOConfig.AnalogInUse[index] == ENERGY_CONTROL_RANGE1) ||
			(USERIOConfig.AnalogInUse[index] == ENERGY_CONTROL_RANGE2))
		{
			if(USERIOConfig.AnalogInUseStatus[index] == ACTIVATED)
			{
				ExitFlag = TRUE;
				EnergyFuncNum = USERIOConfig.AnalogInUse[index];
			}
		}
	}
	SetEnergyFuncNum(EnergyFuncNum);
}

void UserIO::AssignDigitalOutputFunction(enum DIGITAL_OUTPUTS Output, UINT16 FuncNum, BOOLEAN Status)
/******************************************************************************/
/*@Output: User selectable digital output pin                                     */
/*@FuncNum: Function to be assigned on digital output pin                     */
/******************************************************************************/
{
	if ((OutPtr[FuncNum].Control & PresentConfiguration) == PresentConfiguration)
	{
		USERIOConfig.OutputMask = (USERIOConfig.OutputMask & ~(OutBitsLo[Output]));//(OutputMask & ~(OutBitsHi[Output]);		// strip out old bit

		if(Status == ACTIVATED)
		{
			if (OutPtr[FuncNum].Active == LOW)
				USERIOConfig.OutputMask = ((USERIOConfig.OutputMask) | OutBitsLo[Output]);	// set for active low
			else
				USERIOConfig.OutputMask = ((USERIOConfig.OutputMask) | OutBitsHi[Output]);	// set for active high
		}

		USERIOConfig.DigOutUseStatus[Output] = Status;
		USERIOConfig.DigOutUse[Output] = FuncNum;		// save function number assigned to input
		OutValue[Output] = OutPtr[FuncNum].Flag;// put variable address in input pin location
		DigOutUseStatus[Output] = Status;
		OutputMask = USERIOConfig.OutputMask;
	}
	else
	{
		/* FUNCTION NOT AVAILABLE */
	}
}

void UserIO::AssignDigitalInputFunction(enum DIGITAL_INPUTS Input, UINT16 FuncNum, BOOLEAN Status)
/******************************************************************************/
/*@Input: User selectable digital input pin                                    */
/*@FuncNum: Function to be assigned on digital input pin                     */
/******************************************************************************/
{
	if ((InPtr[FuncNum].Control & PresentConfiguration) == PresentConfiguration)
	{
		IsIOConfigured = TRUE;
		(USERIOConfig.InputMask) = ((USERIOConfig.InputMask) & ~(InBitsLo[Input]));//(InputMask AND ~(INBITSHI[Input])		// strip out old bit
		if (InPtr[FuncNum].Active == LOW)
			USERIOConfig.InputMask = ((USERIOConfig.InputMask) | InBitsLo[Input]);	// set for active low
		else
			USERIOConfig.InputMask = ((USERIOConfig.InputMask) | InBitsHi[Input]);	// set for active high

		//check whether some function was already assigned at input pin
		//and now the pin is being unassigned.
		//If yes make the previous flag inactive
		if((USERIOConfig.DigInUse[Input] != FuncNum) || (Status == DEACTIVATED))
		{
			if(InValue[Input])
				*(InValue[Input]) = FALSE;
		}
		USERIOConfig.DigInUseStatus[Input] =  Status;
		USERIOConfig.DigInUse[Input] = FuncNum;				// save function number assigned to input
		InValue[Input] = InPtr[FuncNum].Flag;	// put variable address in input pin location
		InputMask = USERIOConfig.InputMask;
		DigInUseStatus[Input] =  Status;
	}
	else  {
		/* FUNCTION NOT AVAILABLE */
		pprintf("\n INPUT FUNCTION NOT AVAILABLE %d", FuncNum);
	}
}

void UserIO::AssignAnalogOutputFunction(enum ANALOG_OUTPUTS Output, UINT16 FuncNum, BOOLEAN Status)
/******************************************************************************/
/*@Output: User selectable analog output pin                                    */
/*@FuncNum: Function to be assigned on analog output pin                     */
/******************************************************************************/
{
	if ((AOutPtr[FuncNum].Control & PresentConfiguration) == PresentConfiguration)
	{
		// save function number assigned to input
		USERIOConfig.AnalogOutUse[Output] = FuncNum;
		USERIOConfig.AnalogOutUseStatus[Output] = Status;
		AnalogOutValue[Output] = AOutPtr[FuncNum].value;// put variable address in input pin location
		AnalogOutUseStatus[Output] = Status;
	}
	else
	{
		/* FUNCTION NOT AVAILABLE */
		pprintf("\n OUTPUT FUNCTION NOT AVAILABLE %d", FuncNum);
	}
}

void UserIO::AssignAnalogInputFunction(enum ANALOG_INPUTS Input, UINT16 FuncNum, BOOLEAN Status)
/******************************************************************************/
/*@Input: User selectable analog input pin                                    */
/*@FuncNum: Function to be assigned on analog output pin                     */
/******************************************************************************/
{
	if ((AInPtr[FuncNum].Control & PresentConfiguration) == PresentConfiguration)
	{
		//check whether some function was already assigned at input pin
		//and now the pin is being unassigned.
		//If yes make the previous variable holding analog input value 0
		if((USERIOConfig.AnalogInUse[Input] != FuncNum) || (Status == DEACTIVATED))
		{
			if(AnalogInValue[Input])
				*(AnalogInValue[Input]) = 0;
		}
		USERIOConfig.AnalogInUseStatus[Input] = Status;
		USERIOConfig.AnalogInUse[Input] = FuncNum;		// save function number assigned to input
		AnalogInValue[Input] = AInPtr[FuncNum].value;	// put variable address in input pin location
		AnalogInUseStatus[Input] = Status;
	}
	else  {
		/* FUNCTION NOT AVAILABLE */
		pprintf(" Input function not availabel ");
	}
}

void UserIO::ReadInputs()
/******************************************************************************/
/* Read digital inputs from MI board through memory mapped IO                 */
/* Read analog inputs through ADC chip                                        */
/******************************************************************************/
{
	UINT16	InData;			// will contain all inputs values at once
	UINT16	Loop;
	int ch = 0;
	InData  = ReadDigitalIO();			// Value is raw data as read by input port and debounced
	InData = (InData ^ (InputMask));	// invert whatever signals need to be inverted.
//
//	/**  Now we need to take bits and convert to BOOLEAN variables for system use  **///
	for (Loop=1; Loop< NUM_DIN; Loop++)
	{
		if ((InValue[Loop] != NULL) && (DigInUseStatus[Loop] == ACTIVATED))
		{
				if ((InData & InBitsLo[Loop]) == InBitsLo[Loop])
					*InValue[Loop] = TRUE;
				else
					*InValue[Loop] = FALSE;
		}
	}

	//see which channlels need to be read
	for(Loop=1; Loop< NUM_AIN; Loop++)
	{
		if((AnalogInValue[Loop] != NULL) && (AnalogInUseStatus[Loop] == ACTIVATED))
		{
			ch = (1 << InChannles[Loop]);
			*AnalogInValue[Loop] =	DigFilterMISAin(adc->ReadInput(ch - 1), (ANALOG_INPUTS)Loop);
		}
			//ch |= (1 << InChannles[Loop]);
	}
}

void UserIO::WriteOutputs()
/******************************************************************************/
/* Write digital outputs to MI board through memory mapped IO                 */
/* Write analog outputs through DAC chip                                        */
/******************************************************************************/
{
	UINT16	OutData = 0;			// will contain value to be written to output ports
	UINT16	Loop;

	/** First convert all BOOLEAN variables into a single value bit assignment **/
	for (Loop=1; Loop< NUM_DOUT; Loop++) {
		if ((OutValue[Loop] != NULL) && (DigOutUseStatus[Loop] == ACTIVATED)) {
			if ((*OutValue[Loop] == TRUE))
				OutData = (OutData | OutBitsLo[Loop]);
		}
	}

	OutData = (OutData ^ (OutputMask));
	WriteDigitalIO (OutData);

	int adcOutVal[NUM_AOUT];
	static int OldadcOutVal[NUM_AOUT];
	int ch = 0;
	//check out which channels need to be written
	//and what values need to be written
	for(Loop=1; Loop< NUM_AOUT; Loop++)
	{
		if((AnalogOutValue[Loop] != NULL) && (AnalogOutUseStatus[Loop] == ACTIVATED))
	    {
	        //if(OldadcOutVal[Loop-1]!= *AnalogOutValue[Loop])
	         {
	      	   adcOutVal[Loop -1] = *AnalogOutValue[Loop];
	           ch = (1 << OutChannles[Loop]);
	           OldadcOutVal[Loop-1]= *AnalogOutValue[Loop];
	           dac->WriteOutput(ch - 1, adcOutVal[Loop -1]);
	          }
	     }
		else
		{
		   //if(OldadcOutVal[Loop-1]!= 0)
			{
			 	  adcOutVal[Loop -1] = 0;
				  ch = (1 << OutChannles[Loop]);
				  OldadcOutVal[Loop-1]= 0;
				  dac->WriteOutput(ch - 1, adcOutVal[Loop -1]);
			 }
		}
	 }
}

int UserIO::ReadDigitalIO()
/******************************************************************************/
/* Read the memory mapped IO contents                                                 */
/******************************************************************************/
{
	return DIGITAL_IO_VAL;
}

void UserIO::WriteDigitalIO(int val)
/******************************************************************************/
/* Write the memory mapped IO contents                                                */
/******************************************************************************/
{
	DIGITAL_IO_VAL = val;
}

void UserIO::ConfigureDefaultUserIOs ()
/******************************************************************************/
/* configure the digital and analog IO pins to                                */
/* default functions.                                                          */
/******************************************************************************/
{
	//Note:If you change the default configuration from code make sure you modify
	//the value of #define IOCONFIGURED_CHECKSUM so that InitUserIO function
	//would call this function to assign the changed default configuration
	//Defaults for digital Inputs
	Log::WriteEventLog(USERIO_DEFINED, 0, 0);
	SetCableDetect(FALSE);
	//Defaults for digital Inputs
	UserIO::AssignDigitalInputFunction(DIGIN1 , EXTSTART_ACTIVEHIGH,ACTIVATED);//Ext start active high
	UserIO::AssignDigitalInputFunction(DIGIN2 , EXTSEEK_ACTIVEHIGH,ACTIVATED);//Ext Seek active high
	UserIO::AssignDigitalInputFunction(DIGIN3 , EXTRESET_ACTIVEHIGH,ACTIVATED);//Ext Reset active high
	UserIO::AssignDigitalInputFunction(DIGIN4 , MEMCLEAR_ACTIVEHIGH,ACTIVATED);//Mem ClearActive High
	//Defaults for digital Outputs
	UserIO::AssignDigitalOutputFunction(DIGOUT1, READY_ACTIVEHIGH,ACTIVATED);//Ready active high
	UserIO::AssignDigitalOutputFunction(DIGOUT2, SONICACTIVE_ACTIVEHIGH,ACTIVATED);//Sonics active active high
	UserIO::AssignDigitalOutputFunction(DIGOUT3, GENALARM_ACTIVEHIGH,ACTIVATED);//GenAlarm active high
	UserIO::AssignDigitalOutputFunction(DIGOUT4, SEEKOUT_ACTIVEHIGH,ACTIVATED);//Seek out active high
	//Default for Analog Inputs
	UserIO::AssignAnalogInputFunction(AIN1, AMPL_CONTROL,ACTIVATED);// AMPLITUDE CONTROL INPUT
	UserIO::AssignAnalogInputFunction(AIN2, FREQ_CONTROL,ACTIVATED);// Frequency CONTROL INPUT
	//Default for Analog Outputs
	UserIO::AssignAnalogOutputFunction(AOUT1, POWER_OUT,ACTIVATED);
	UserIO::AssignAnalogOutputFunction(AOUT2, AMPL_OUT,ACTIVATED);
}

BOOLEAN UserIO::CheckSignalActive(BOOLEAN signal)
{
	BOOLEAN retVal = signal;
	if(CableDetectEnabled)
		retVal = signal && CableDetect;
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

BOOLEAN UserIO::GetGenAlarmFlag()
{
	return GenAlarm;
}

BOOLEAN UserIO::GetReadyFlag()
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
//Analog UserIO related func definitions
SINT32 UserIO::GetExternalAmplitude()
{
	if(amplitudeIn < RAWVAL_TENPER)
		amplitudeIn = RAWVAL_FIFTYPER;

	return amplitudeIn;
}

SINT32 UserIO::GetExternalFrequency()
{
	if(freqIn < RAWVAL_TENPER)
		freqIn = RAWVAL_FIFTYPER;

	SINT32 ExtFreqMv = freqIn;

	ExtFreqMv = (ExtFreqMv - EXT_FREQ_SCALINGFACTOR);
	ExtFreqMv = (ExtFreqMv * ((SINT32)((FREQ_OFFSET_LIMIT * F_Factor)/100)))/EXT_FREQ_SCALINGFACTOR;

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
	SINT32 EnergyInJoules = 0,EnergyInRawValue = 0;

	switch(UserIO::CheckForEnergyRange())
	{
		case ENERGY_CONTROL_RANGE1:
			EnergyInJoules = ((EnergyIn * ENERGYRANGE_1_MAX) / ADC11BITVALUE );
			if(EnergyInJoules > ENERGYRANGE_1_MIN)
			{
				CurrentPreset.WeldMode = ENERGYMODE;
				EnergyInRawValue = (EnergyInJoules)*((ENERGY_MULT_FAC * ADC11BITVALUE) / ePromData.PSpower);
			}
			else
				CurrentPreset.WeldMode = CONTINUOUSMODE;
			break;
		case ENERGY_CONTROL_RANGE2:
			EnergyInJoules = ((EnergyIn * ENERGYRANGE_2_MAX) / ADC11BITVALUE );
			if(EnergyInJoules > ENERGYRANGE_2_MIN)
			{
				CurrentPreset.WeldMode = ENERGYMODE;
				EnergyInRawValue = (EnergyInJoules)*((ENERGY_MULT_FAC * ADC11BITVALUE) / ePromData.PSpower);
			}
			else
				CurrentPreset.WeldMode = CONTINUOUSMODE;
		break;
		case ENERGY_CONTROL_RANGE3:
			EnergyInJoules = ((EnergyIn * ENERGYRANGE_3_MAX) /ADC11BITVALUE );
			if(EnergyInJoules > ENERGYRANGE_3_MIN)
			{
				EnergyInRawValue = (EnergyInJoules)*((ENERGY_MULT_FAC * ADC11BITVALUE) / ePromData.PSpower);
				CurrentPreset.WeldMode = ENERGYMODE;
			}
			else
				CurrentPreset.WeldMode = CONTINUOUSMODE;
		break;
		default:
			CurrentPreset.WeldMode = CONTINUOUSMODE;
			EnergyInRawValue = 0;
		break;
	}
	if(CurrentIndex == WAITING_IDX)
	{
		if(CurrentPreset.WeldMode != RunningPreset.WeldMode)
		{
//			pprintf("\n weld mode change");
			SavePreset(0);
		}
	}
	return EnergyInRawValue;
}

void UserIO::SetOutputAmplitude(SINT32 val)
{
	amplitudeOut = val;
}

void UserIO::SetOutputPower(SINT32 val)
/******************************************************************************/
/* Function checks for Power output Range and if output is assigned then      */
/* calculate the outout power as per the output range and set the output value*/
/******************************************************************************/
{
	SINT32 MaxWatt;
	UINT16 Index = 0;
	SINT32 TempPwrOut = 0;
	SINT32 Temp = (val * DAC_MULFACTOR) / DAC_DIVFACTOR;
	//printf("-%d-",Temp);
	for(Index = 1; Index < NUM_AOUT; Index++)
	{
		switch(USERIOConfig.AnalogOutUse[Index])
		{
			case POWER_OUT:
				MaxWatt = ePromData.PSpower;
				powerOut = (ePromData.PSpower * Temp) / MaxWatt;
				//printf("%d ",powerOut);
				break;
			case POWER_OUT400W:
				MaxWatt = 400;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut1 = TempPwrOut;
				//printf("%d ",powerOut1);
				break;
			case POWER_OUT750W:
				MaxWatt = 750;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut2 = TempPwrOut;
				//printf("%d ",powerOut2);
				break;
			case POWER_OUT800W:
				MaxWatt = 800;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut3 = TempPwrOut;
				//printf("%d ",powerOut3);
				break;
			case POWER_OUT1100W:
				MaxWatt = 1100;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut4 = TempPwrOut;
				//printf("%d ",powerOut4);
				break;
			case POWER_OUT1250W:
				MaxWatt = 1250;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut5 = TempPwrOut;
				//printf("%d ",powerOut5);
				break;
			case POWER_OUT1500W:
				MaxWatt = 1500;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut6 = TempPwrOut;
				//printf("%d ",powerOut6);
				break;
			case POWER_OUT1600W:
				MaxWatt = 1600;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut7 = TempPwrOut;
				//printf("%d ",powerOut7);
				break;
			case POWER_OUT2200W:
				MaxWatt = 2200;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut8 = TempPwrOut;
				//printf("%d ",powerOut8);
				break;
			case POWER_OUT2400W:
				MaxWatt = 2400;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut9 = TempPwrOut;
				//printf("%d ",powerOut9);
				break;
			case POWER_OUT2500W:
				MaxWatt = 2500;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut10 = TempPwrOut;
				//printf("%d ",powerOut10);
				break;
			case POWER_OUT3000W:
				MaxWatt = 3000;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut11 = TempPwrOut;
				//printf("%d ",powerOut11);
				break;
			case POWER_OUT3300W:
				MaxWatt = 3300;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut12 = TempPwrOut;
				//printf("%d ",powerOut12);
				break;
			case POWER_OUT4000W:
				MaxWatt = 4000;
				TempPwrOut = (ePromData.PSpower *Temp) / MaxWatt;
				if(TempPwrOut > DAC_MULFACTOR)
					TempPwrOut = DAC_MULFACTOR;
				powerOut13 = TempPwrOut;
				//printf("%d ",powerOut13);
				break;
			default:
				break;
		}
	}
}

void UserIO::SetOutputFrequency(SINT32 val)
{
	freqOut = val;
}

void UserIO::SetOutputMem(SINT32 val)
{
	memOut = val;
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
	if((USERIOConfig.DigInUse[digIn] > 0) && (USERIOConfig.DigInUseStatus[digIn] == ACTIVATED))
		ret =  ((*(InValue[digIn]))? 1: 0);

	return ret;
}

//-1 unassigned 0 inactive 1 active
int UserIO::GetDigOutPutActiveStatus(DIGITAL_OUTPUTS digOut)
/******************************************************************************/
/* @digOut:The output pin whose active status needs to be determined          */
/* OUTPUT:-1 unassigned 0 inactive 1 active
*******************************************************************************/
{
	int ret = -1;
	if((USERIOConfig.DigOutUse[digOut] > 0) && (USERIOConfig.DigOutUseStatus[digOut] == ACTIVATED))
		ret =  ((*(OutValue[digOut]))? 1: 0);

	return ret;
}

BOOLEAN UserIO::GetDigOutPutActiveConfiguration(DIGITAL_OUTPUTS digOut)
{
	int ret = 1;
	if((USERIOConfig.DigOutUse[digOut] > 0)  && (USERIOConfig.DigOutUseStatus[digOut] == ACTIVATED))
		ret =  OutPtr[USERIOConfig.DigOutUse[digOut]].Active;

	return ret;
}

void UserIO::SetDigOutPutActiveStatus(DIGITAL_OUTPUTS digOut, BOOLEAN val)
/******************************************************************************/
/* @digOut:The output pin whose active status needs to be changed          */
/* @val: new status to be set on output pin                                 */
/******************************************************************************/
{
	if((USERIOConfig.DigOutUse[digOut] > 0) && (USERIOConfig.DigOutUseStatus[digOut] == ACTIVATED))
		*(OutValue[digOut]) = val;
}

SINT32 UserIO::GetAnalogINPutValue(ANALOG_INPUTS aIn)
/******************************************************************************/
/* @aIn: The input pin where analog signal value needs to be determined       */
/*OUTPUT:-1 if unassigned else Analog Signal value
*******************************************************************************/
{
	int ret = -1;
	if((USERIOConfig.AnalogInUse[aIn] > 0) && (USERIOConfig.AnalogInUseStatus[aIn] == ACTIVATED))
		ret = *(AnalogInValue[aIn]);
	return ret;
}

SINT32 UserIO::GetAnalogOUTPutValue(ANALOG_OUTPUTS aOut)
/******************************************************************************/
/* @aOut: The output pin where analog signal value needs to be determined     */
/*OUTPUT: -1 if unassigned else the analog value we are driving on output pin
*******************************************************************************/
{
	int ret = -1;
	if((USERIOConfig.AnalogOutUse[aOut] > 0) && (USERIOConfig.AnalogOutUseStatus[aOut] == ACTIVATED))
		ret = *(AnalogOutValue[aOut]);
	return ret;
}

void UserIO::SetAnalogOUTPutValue(ANALOG_OUTPUTS aOut, SINT32 val)
/******************************************************************************/
/* @aOut: The output pin where analog signal value needs to be driven     */
/* @val: The signal value we want to drive through DAC chip
*******************************************************************************/
{
	if((USERIOConfig.AnalogOutUse[aOut] > 0) && (USERIOConfig.AnalogOutUseStatus[aOut] == ACTIVATED))
		*(AnalogOutValue[aOut]) = val;
}

UINT16 UserIO::CheckForEnergyRange()
{
	return EnergyRangeSelection;
}

void UserIO::SetEnergyFuncNum(UINT16 EnergyFuncNum)
{
	if((EnergyFuncNum != EnergyRangeSelection) &&(CurrentIndex != WAITING_IDX)&&(!MFCycleFlag))
	{
		NoInput = TRUE;
		WeldRunComplete = TRUE;
		AccumulatedEnergy = 0;
	}
	EnergyRangeSelection = EnergyFuncNum;
}

/*  SINT32 UserIO::DigFilterMISAin(UINT32 Element, ANALOG_INPUTS InputNumber)*/
/* 	To apply software filter on Analog inputs read from MI-S board	*/
/*  Element- Represents the value of analog input number passed in input number param.*/
/*  InputNumber- Represents the analog input number*/
/*  It returns the average of the last MAXADCVALUES_FILTER points.*/
/*  Using: (Sum of  MAXADCVALUES_FILTER values + New Value  – Last value) / MAXADCVALUES_FILTER*/
SINT32 UserIO::DigFilterMISAin(SINT32 Element, ANALOG_INPUTS InputNumber)
{
   SINT32 Temp;   // Last Value
   if(AinFilterPtr[InputNumber] == &BufferAin[InputNumber][MAXADCVALUES_FILTER - 1]){
      Temp = *(AinFilterPtr[InputNumber]);                     // Move old to temp
      *(AinFilterPtr[InputNumber]) = Element;
      AinFilterPtr[InputNumber] = &BufferAin[InputNumber][0];
   }
   else{
      Temp = *(AinFilterPtr[InputNumber]); // Move old to temp
      *(AinFilterPtr[InputNumber]) = Element;// Insert new into buffer
      ++AinFilterPtr[InputNumber];
   }
   AinValue[InputNumber] = AinValue[InputNumber] + Element - Temp;
   return (AinValue[InputNumber] / MAXADCVALUES_FILTER);
}

