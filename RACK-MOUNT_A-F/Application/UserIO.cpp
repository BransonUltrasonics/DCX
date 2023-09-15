/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/UserIO.cpp_v   1.3   24 Jul 2015 08:38:34   sesharma  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/UserIO.cpp_v  $
 * 
 *    Rev 1.3   24 Jul 2015 08:38:34   sesharma
 * Constructor calls fix for passing CTAR number.
 * 
 *    Rev 1.2   02 Jul 2015 10:01:32   RJamloki
 * Fixed function declaration as per review comment.
 * 
 *    Rev 1.1   24 Jun 2015 08:38:32   amaurya
 * Removed SetAmplitudeDecay()
 * 
 *    Rev 1.0   09 Jun 2015 09:10:48   tayars
 * Initial revision.
 * 
 *    Rev 1.35   27 Mar 2014 13:00:02   hasanchez
 * Changes of the tracker about Default Input/Output
 * 
******************************************************************************/
//This module is the part of WC Application code
//This module gives the interface to read and write digital and analog
//signals between WC and MI board. The WC state machine should use the
//function of this class (which are all static) to determine the UserIO
//status.In all comments in this module input signal means the signal coming
//from MI board to WC board. The output signal means the signal going from
//WC board to MI board.

#include "UserIO.h"
#include "SPI.h"
#include "Board.h"
#include "SysConfig.h"
#include "DEF_DEF.h"
#include "FUNC.h"
#include "LOG.h"
#include "GLOBALS.h"
#include "DebugNew.h"
#include "assert.h"
#include "LOG.h"

BOOLEAN FanStatusFlag = FALSE;
//the present product configuration
const CONTROL PresentConfiguration = DCXAANDF;
UINT8 CableDetectedPin = 0;
//ADC and DAC objects pointers.They gets initialized in Init() function of this class.
TLV5604 * UserIO::DAC;
MCP3208 * UserIO::ADC;

//Input Flags corresponding to the input functions defined in PRS
static BOOLEAN ExtReset, ExtAmpStep, ExtSeek, RecallPreset1, RecallPreset2,
RecallPreset4, RecallPreset8, RecallPreset16, RecallPreset32, ExtStart,
ExtSonicsDelay, LockOutChangeLCD, SonicsDisable, InterLockInPlace,
LoadNewPreset, CycleAbort, StartHornScan, ExtTest, HornDownMode, MemClear,
CableDetect, Rfstatusfeedback, Rfbit0feedback, Rfbit1feedback, Rfbit2feedback, Rfbit3feedback,

//Automation inputs
UpperLimitSwitch, TriggerInput, Startcycle, Partinplace, ActuatorPresent, GroundDetect;

//Flag to check if cableDetect feature is enabled.
//This flag goes TRUE if any of the digital inputs has been assigned the function CABLEDETECT_ACTIVEHIGH.
//If this flag is true then any digital input signal is considered valid only if the pin being assigned CABLEDETECT_ACTIVEHIGH. is at logic 1.
//This is to prevent accidental activation of ultrasonics if the I/O cable is removed.If the cable is removed the signal on this pin drops
//to 0 volts preventing ultrasonics from coming on.The ultrasonics will remain disabled until the cable is reconnected.
static BOOLEAN CableDetectEnabled;

//Output Flags corresponding to the output functions defined in PRS
static BOOLEAN ReadyFlg, SonicsActive, GenAlarm, StatusFlag, CycleOK, PresetChanged,
HoldTimeActive, AfterBurstDelayActive, AfterBurstTimeActive, PlusTimeLimitAlarm,
MinusTimeLimitAlarm, PlusEnergyLimitAlarm, MinusEnergyLimitAlarm, PlusPeakPowerLimitAlarm,
MinusPeakPowerLimitAlarm, Overloadalarm, ActuaterHome, WeldCycleComplete, PreloadedCycleCountReached,
InHorndownMode, EndOfHoldTime, Amplitude1_2, StartSignalRelease, SeekOut, Rfbit0select, Rfbit1select,
Rfbit2select, Rfbit3select,
//Automation outputs
Cyclestartout, CustomAlarmOutput;
extern BOOLEAN SonicsDisableFlag;
BOOLEAN CustomInput1Configured = FALSE, CustomInput2Configured = FALSE,InterlockConfigured = FALSE,
		PartInPlaceConfigured = FALSE;
BOOLEAN CableFailure = FALSE;
//pointer to input variables. Array elements contain the pointers to digital input flags based on the input function assigned to a digital input pin.
//e.g if DIGIN1 has been assigned the function number EXTRESET_ACTIVEHIGH or EXTRESET_ACTIVELOW
//InValue[DIGIN1] contains the address of ExtReset flag.
BOOLEAN  * InValue[NUM_DIN];

//pointer to output variables. Array elements contain the pointers to digital output flags. based on the output function assigned to a digital output pin
//e.g if DIGOUT1 has been assigned the function number READY_ACTIVEHIGH or READY_ACTIVELOW the OutValue[DIGOUT1] contains the address of Ready flag.
BOOLEAN  * OutValue[NUM_DOUT];

/* This array elements are used while creating masks for digital inputs in AssignDigitalIntput Function.*/
const UINT16 InBitsHi[NUM_DIN] = {0, DIN1_HI, DIN2_HI, DIN3_HI, DIN4_HI, DIN5_HI, DIN6_HI, DIN7_HI, DIN8_HI, DIN9_HI};

/* This array elements are used while creating masks for digital inputs in AssignDigitalIntput Function.
Additionaly this array elemnts are representing the bit position of digital inputs in the way they will appear in data bus
and shall be used to detect the digital input logic in the value read from mamory mapped address e.g. if tempVal is the value
read on IO address then (tempVal & InBitsLo[DIGIN1] == InBitsLo[DIGIN1]) is represnting that the logic at digital input pin 1 is high.
*/
const UINT16 InBitsLo[NUM_DIN] = {0, DIN1_LO, DIN2_LO, DIN3_LO, DIN4_LO, DIN5_LO, DIN6_LO, DIN7_LO, DIN8_LO, DIN9_LO};

//copy of userIO configuration FRAM variables in RAM which are accessed every ms to save fram access flexbus cycles.
UINT16  UserIO::InputMask;
UINT16  UserIO::OutputMask;
BOOLEAN UserIO::DigInUseStatus[NUM_DIN];
BOOLEAN UserIO::DigOutUseStatus[NUM_DOUT];
BOOLEAN UserIO::AnalogInUseStatus[NUM_AIN];
BOOLEAN UserIO::AnalogOutUseStatus[NUM_AOUT];

SINT32 ExtRawFreqToFreqOffset[MAX_ADC_RAWVALUE + 1];

/* This table contains a pointer to the BOOLEAN variable assigned to each input
 * function number. The member “Control” contains defines (OEM, STD, LCD, FB) for each configuration that this
 * particular feature is available for.  Given a input function number , table  is handy to
 * access the flag representing that function, the way it will be activated(LOW or HIGH) and whether function
 * is supported in this product configuration.
*/

const DIGITALIOMAP InPtr[NUM_FUNC_IN + 1] = {
	/* Flag		    Active	   Control	       #Description                    */
	/*-----------	     -------	   --------	---------------------------------------*/
   { 0,		           LOW,	   (ALL)},		   /* 0 Not used                        */
	{&ExtReset,	     	  HIGH,	   (ALL)},		   /* 1 External reset                  */
	{&ExtReset,	    	  LOW,	   (ALL)},	 	   /* 2 External reset active low       */
	{&ExtSeek,	     	  HIGH,	   (ALL)},		   /* 3 External seek active high       */
	{&ExtSeek,	     	  LOW,	   (ALL)},		   /* 4 External seek active low        */
	{&ExtStart,	     	  HIGH,	   (ALL)},        /* 5 External Trigger/Start active high  */
	{&ExtStart,	     	  LOW,	   (ALL)},		   /* 6 External Trigger/Start active low   */
	{&ExtTest, 	     	  HIGH,	   (ALL)},    	   /* 7 External Test active high        */
	{&ExtTest,       	  LOW, 	   (ALL)},   	   /* 8 External test active low         */
	{&MemClear,      	  HIGH,	   (ALL)},   	   /* 9 MemClear active high             */
	{&MemClear,	     	  LOW,	   (ALL)},   	   /* 10 MemClear active low             */
	{&StartHornScan, 	  HIGH,	   (ALL)},  	   /* 11 Start HornScan  active high     */
	{&StartHornScan, 	  LOW,	   (ALL)},  	   /* 12 Start HornScan  active low      */
	{&RecallPreset1, 	  HIGH,	   (DCXAANDF)},	/* 13 Recall preset 1 active high only   */
	{&RecallPreset2, 	  HIGH,		(DCXAANDF)},	/* 14 Recall preset 2 active high only   */
	{&RecallPreset4, 	  HIGH,		(DCXAANDF)},	/* 15 Recall preset 4 active high only   */
	{&RecallPreset8, 	  HIGH,		(DCXAANDF)},	/* 16 Recall preset 8 active high only   */
	{&RecallPreset16,	  HIGH,		(DCXAANDF)},	/* 17 Recall preset 16 active high only  */
	{&RecallPreset32,	  HIGH,		(DCXAANDF)},	/* 18 Recall preset 32 active high only  */
	{&ExtSonicsDelay,	  HIGH,		(DCXAANDF)},	/* 19 External sonic delay active high   */
	{&ExtSonicsDelay,   LOW,		(DCXAANDF)},	/* 20 External sonic delay active low    */
	{&LockOutChangeLCD, HIGH,    	(ALL)},		   /* 21 Lock Out Change active high        */
	{&LockOutChangeLCD, LOW,     	(ALL)},		   /* 22 Lock Out Change active low         */
	{&SonicsDisable,	  HIGH,	   (DCXAANDF)},	/* 23 Sonics Disable active high         */
	{&SonicsDisable,	  LOW,		(DCXAANDF)},	/* 24 Sonics Disable active low          */
	{&InterLockInPlace, HIGH,  	(DCXAANDF)},	/* 25 InterLockInPlace active high       */
	{&InterLockInPlace, LOW,	   (DCXAANDF)},	/* 26 InterLockInPlace active low        */
	{&LoadNewPreset,	  HIGH,		(DCXAANDF)},	/* 27 Load New Preset active high        */
	{&LoadNewPreset,	  LOW,	   (DCXAANDF)},	/* 28 Load New Preset active low         */
	{&CycleAbort,	     HIGH,		(DCXAANDF)},   /* 29 Cycle Abort active high            */
	{&CycleAbort,	     LOW,		(DCXAANDF)},   /* 30 Cycle Abort active low             */
	{&ExtAmpStep,       HIGH, 		(DCXAANDF)},   /* 31 External amplitude step active high */
	{&ExtAmpStep,	     LOW, 		(DCXAANDF)},   /* 32 External amplitude step active low  */
	{&HornDownMode,     HIGH, 		(DCXAANDF)},   /* 33 HornDown Mode active high         */
	{&HornDownMode,     LOW,	 	(DCXAANDF)},   /* 34 HornDown Mode active high         */
	{&CableDetect,	     HIGH,	  	(ALL)},	      /* 35 Cable detect active high only	*/
	//RF switch status and feedback enable input
	{&Rfstatusfeedback, HIGH, 		(DCXAANDF)},
	{&Rfstatusfeedback, LOW, 		(DCXAANDF)},
	//status bits
	{&Rfbit0feedback,	  HIGH,    	(DCXAANDF)},
	{&Rfbit1feedback,   HIGH,		(DCXAANDF)},
	{&Rfbit2feedback,   HIGH,		(DCXAANDF)},
	{&Rfbit3feedback,   HIGH,     (DCXAANDF)},
	//Automation Inputs
	{&Partinplace, 	  HIGH,		(DCXAANDF)},
	{&Partinplace, 	  LOW, 		(DCXAANDF)},
	{&Startcycle, 		  HIGH,	   (DCXAANDF)},
	{&Startcycle, 		  LOW ,		(DCXAANDF)},
	{&UpperLimitSwitch, HIGH,   	(DCXAANDF)},
	{&UpperLimitSwitch, LOW, 	   (DCXAANDF)},
	{&GroundDetect,	  HIGH,	 	(DCXAANDF)},
	{&GroundDetect,	  LOW,	 	(DCXAANDF)},
	{&ActuatorPresent,  HIGH, 	   (DCXAANDF)},
	{&ActuatorPresent,  LOW, 		(DCXAANDF)},
	{&TriggerInput,     HIGH,		(DCXAANDF)},
	{&TriggerInput,     LOW,		(DCXAANDF)}
};

/* These array elements are used while creating masks for digital outputs in AssignDigitalOutput function Function */
const UINT16  OutBitsHi[NUM_DOUT] = {0, DOUT1_HI, DOUT2_HI, DOUT3_HI, DOUT4_HI,DOUT5_HI,DOUT6_HI, DOUT7_HI, DOUT8_HI};

//Additionally this array is representing the bit position of outputs in data bus.(e.g  OutBitsLo[DIGOUT1] = DOUT1_LO = BIT(7)) and
//used for creating mask value and combining the application flags in a single variable before writing to memory mapped IO.
const UINT16  OutBitsLo[NUM_DOUT] = {0, DOUT1_LO, DOUT2_LO, DOUT3_LO, DOUT4_LO, DOUT5_LO, DOUT6_LO, DOUT7_LO, DOUT8_LO};

/* This table contains a pointer to the BOOLEAN variable assigned to each output function number. The member “Control” contains defines (OEM, STD, LCD, FB) for each configuration that this  particular feature is available for.  Given a output function number , table  is handy to
access the flag representing that function, the way it will be activated(LOW or HIGH) and whether function is supported in this product configuration.
 */
DIGITALIOMAP  OutPtr[NUM_FUNC_OUT + 1] = {
	/* Flag		          Active		Control	#Description                                      */
	/*----------------------------------------------------------------------------                                       */
   {0,		          			 LOW,	   (ALL)},  /* 0  Not used    */
	{&ReadyFlg,		    		    HIGH,	(ALL)},/* 1Ready active high  */
	{&ReadyFlg,		   			 LOW,	   (ALL)}, /* 2Ready active low  */
	{&SonicsActive,	 			 HIGH,	(ALL)},/* 3WeldOn active high */
	{&SonicsActive,	 			 LOW,	   (ALL)},/* 4WeldOn active low  */
	{&GenAlarm,		    		    HIGH,	(ALL)},/* 5 General alarm active high */
	{&GenAlarm,		    		    LOW,	   (ALL)},/* 6General alarm active low  */
	{&Overloadalarm,	 		    HIGH,	(ALL)},/* 7OverloadAlarm active high*/
	{&Overloadalarm,	 		    LOW,	   (ALL)},/* 8OverloadAlarm active low */
	{&SeekOut,		    		    HIGH,	(ALL)},/* 9seek out active high   */
	{&SeekOut,		    		    LOW,	   (ALL)},/* 10seek out active low  */
	{&HoldTimeActive,	 		    HIGH,	(DCXAANDF)},/* 13HoldTimeActive */
	{&HoldTimeActive,  			 LOW, 	(DCXAANDF)},/* 14 HoldTimeActive  active low  */
	{&AfterBurstDelayActive, 	 HIGH,	(DCXAANDF)},/* 15AfterBurstDelayActive active*/
	{&AfterBurstDelayActive,  	 LOW,	   (DCXAANDF)},/* 16AfterBurstDelayActive low  */
	{&AfterBurstTimeActive, 	 HIGH,   (DCXAANDF)},/*17AfterBurstTimeActive high */
	{&AfterBurstTimeActive, 	 LOW,    (DCXAANDF)},/*18AfterBurstTimeActive active low */
	{&PlusTimeLimitAlarm,  		 HIGH,	(DCXAANDF)},     /* 19PlusTimeLimitAlarm active high*/
	{&PlusTimeLimitAlarm,  		 LOW,	   (DCXAANDF)},  /* 20PlusTimeLimitAlarm  active low*/
	{&MinusTimeLimitAlarm,  	 HIGH,	(DCXAANDF)},/* 21MinusTimeLimitAlarm active high*/
	{&MinusTimeLimitAlarm,  	 LOW,	   (DCXAANDF )},/* 22MinusTimeLimitAlarm active low */
	{&PlusEnergyLimitAlarm,  	 HIGH,	(DCXAANDF)},/* 23PlusEnergyLimitAlarm active high */
	{&PlusEnergyLimitAlarm,  	 LOW,	   (DCXAANDF)},/* 24PlusEnergyLimitAlarm active low */
	{&MinusEnergyLimitAlarm,  	 HIGH,	(DCXAANDF)},/* 25MinusEnergyLimitAlarm active high*/
	{&MinusEnergyLimitAlarm,	 LOW,	   (DCXAANDF)},/* 26MinusEnergyLimitAlarm active low */
	{&PlusPeakPowerLimitAlarm,  HIGH,	(DCXAANDF)},/* 27  PlusPeakPowerLimitAlarm active high   */
	{&PlusPeakPowerLimitAlarm,  LOW,	   (DCXAANDF)},/* 28PlusPeakPowerLimitAlarm active low*/
	{&MinusPeakPowerLimitAlarm, HIGH,	(DCXAANDF)},/*29MinusPeakPowerLimitAlarm active high */
	{&MinusPeakPowerLimitAlarm, LOW, 	(DCXAANDF)},/* 30MinusPeakPowerLimitAlarm active low */
	{&StatusFlag,	           	 HIGH, 	(DCXAANDF)},/* 31Beeper active high only*/
	{&PresetChanged,            HIGH,   (DCXAANDF)},/*32  Preset Changed active high*/
	{&ActuaterHome,           	 HIGH,   (DCXAANDF)},/* 33ActuaterHome active high*/
	{&ActuaterHome,           	 LOW,    (DCXAANDF)},/* 34ActuaterHome active low*/
	{&WeldCycleComplete,        HIGH,	(DCXAANDF)},/* 35WeldCycleComplete active high*/
	{&WeldCycleComplete,        LOW,	   (DCXAANDF)},/* 36WeldCycleComplete active low*/
	{&PreloadedCycleCountReached, HIGH, (DCXAANDF)},/*37PreloadedCycleCountReachedactive high*/
	{&PreloadedCycleCountReached, LOW,  (DCXAANDF)},/*38 PreloadedCycleCountReached active low*/
	{&InHorndownMode,	          HIGH,	(DCXAANDF)},/* 39   InHorndownMode active high*/
	{&InHorndownMode,	          LOW,	   (DCXAANDF)},/* 40InHorndownMode active low*/
	{&EndOfHoldTime,	       	 HIGH,	(DCXAANDF)},/* 41EndOfHoldTime active high*/
	{&EndOfHoldTime,	          LOW,	   (DCXAANDF)},/* 42	EndOfHoldTime active low*/
	{&Amplitude1_2,	       	 HIGH,	(DCXAANDF)},/* 43Amplitude1_2 active high only*/
	{&StartSignalRelease,    	 HIGH,	(DCXAANDF )},/* 44StartSignalRelease active high*/
	{&StartSignalRelease,    	 LOW,	   (DCXAANDF)},/* 45StartSignalRelease active low*/
	{&CycleOK,	            	 HIGH,	(DCXAANDF)},/* 46CycleOK active high*/
	{&CycleOK,	            	 LOW,	   (DCXAANDF)},/* 47CycleOK active low*/
	//current RF switch selection always active high.
	{&Rfbit0select,  			    HIGH ,  (DCXAANDF)},
	{&Rfbit1select, 			    HIGH, 	(DCXAANDF)},
	{&Rfbit2select, 			    HIGH ,	(DCXAANDF)},
	{&Rfbit3select,             HIGH ,  (DCXAANDF)},
	//Automation outputs
	{&Cyclestartout,     		 HIGH,   (DCXAANDF)},/*  CYCLESTRATOUT  */
	{&Cyclestartout,   			 LOW,    (DCXAANDF)},
	//Custom Alarm Ouptput
	{&CustomAlarmOutput,        HIGH,   (DCXAANDF)},/*  Custom Alarm Output  */
	{&CustomAlarmOutput,        LOW,    (DCXAANDF)},
};

UINT16 OutBitsPosFANamdLED[LEDSTATE] = {BIT(LEDYELLOW), BIT(LEDRED)/*Red LED*/, BIT(LEDGREEN)/*Green LED*/};
//Analog inputs
SINT32 FreqIn, AmplitudeIn,
//Automation analog inputs.
CustomInput1, CustomInput2;

//Analog outputs
static SINT32 PowerOut, AmplitudeOut, FreqOut, MemOut,
//Automation analog outputs.
AutomationOP1;

//pointer to input variables. Array elements contain the pointers to analog input variable based on the input function
//assigned to a Analog input pin.
//e.g if AIN1 has been assigned the function number AMPL_CONTROL.AnalogInValue[AIN1) contains the address of amplitudeIn.
SINT32 * AnalogInValue[NUM_AIN];

//pointer to output variables. Array elements contain the pointers to analog output variable based on the output function
// assigned to a Analogoutput pin.
//e.g if AOUT1 has been assigned the function number POWER_OUT. AnalogOutValue[AIN1) contains the address of powerOut.
SINT32 * AnalogOutValue[NUM_AOUT];

/* This table contains a pointer to the SINT32 variable assigned to each input function number. The member “Control”
 * contains defines (OEM, STD, LCD, FB) for each configuration that this  particular feature is available for.
 * Given a input function number, table is handy to access the variable representing that function, and whether
 * function is supported in this product configuration.
 */
const ANALOGIOMAP AInPtr[NUM_FUNC_AIN + 1] =
{
	{0,  (ALL)},
	{&FreqIn, (ALL)},				//1
	{&AmplitudeIn ,(ALL)},		//2
	//Custom inputs
	{&CustomInput1,(ALL)},//3
	{&CustomInput2,(ALL)},//4
};

/* This table contains a pointer to the SINT32 variable assigned to each output function number. The member “Control” contains defines (OEM, STD, LCD, FB) for each configuration that this  particular feature is available for.  Given a output function number , table  is handy to
access the variable representing that function, and whether function is supported in this product configuration.
*/
const ANALOGIOMAP AOutPtr[NUM_FUNC_AOUT + 1] =
{
	{0, (ALL)},
	{&PowerOut , (ALL)},		//1
	{&AmplitudeOut, (ALL)},	//2
	{&MemOut, (ALL)},			//3 not used
	{&FreqOut, (ALL)},		//4
	//Automation outputs
	{&AutomationOP1, (DCXAANDF)},//5
};

//This array elements contain the channel number of ADC chip corresponding to an
//analog input. E.g. InChannles[AIN1] is representing the channel number of ADC for analog input 1
const UINT16 InChannles[NUM_AIN] = {0, AIN1_CHANNEL, AIN2_CHANNEL};
//This array elements contain the channel number of DAC chip corresponding to an
//analog output. E.g. OutChannles[AOUT1] is representing the channel number of DAC for analog output 1
const UINT16 OutChannles[NUM_AOUT] = {0, AOUT1_CHANNEL, AOUT2_CHANNEL};

//buffers to hold last 3 read analog input values for all analog inputs
static SINT32 BufferAin[NUM_AIN][MAXADCVALUES_FILTER];

//Pointers to the buffers holding last MAXADCVALUES_FILTER analog input values. Used in filtering logic
static SINT32 * AinFilterPtr[NUM_AIN] = {0, &BufferAin[AIN1][0], &BufferAin[AIN2][0]};

//filtered value of analog inputs
static SINT32 AinValue[NUM_AIN];

//defines for J3 Connector MI-A. Index is representing IO Number , Value is representing pin on J3
const UINT8 DigInPins[NUM_DIN] = {0, 1, 2, 3, 4, 11, 12, 13, 16, 23};
const UINT8 DigOutPins[NUM_DOUT] = {0 , 7, 8, 9, 10, 19, 20, 21, 22};
const UINT8 AinPins[NUM_AIN] = {0, 17, 18};
const UINT8 AoutPins[NUM_AOUT] = {0 , 24, 25};

/*************************BRANSON Defaults*************************************************/
//Default functions to be assigned on digital input pins
const enum USERDEFINED_DIGINPUTFUNC BransonDefaultDigInputFunctions[LAST_DIGITAL_INPUT] =
{
   EXTSTART_ACTIVEHIGH, //default for digital input 1
   EXTSEEK_ACTIVEHIGH,  //default for digital input 2
   EXTRESET_ACTIVEHIGH, //default for digital input 3
   MEMCLEAR_ACTIVEHIGH, //default for digital input 4
   RECALLPRESET_1_ACTIVEHIGH, //default for digital input 5
   RECALLPRESET_2_ACTIVEHIGH, //default for digital input 6
   GROUNDDETECT_ACTIVEHIGH,   //default for digital input 7
   CYCLEABORT_ACTIVEHIGH,     //default for digital input 8
   LOCKOUT_ACTIVEHIGH,    //default for digital input 9
};

//Default functions to be assigned on digital output pins
const enum USERDEFINED_DIGOUTPUTFUNC BransonDefaultDigOutputFunctions[LAST_DIGITAL_OUTPUT] =
{
   READY_ACTIVEHIGH,       //default for digital output 1
   SONICACTIVE_ACTIVEHIGH, //default for digital output 2
   GENALARM_ACTIVEHIGH,    //default for digital output 3
   SEEKOUT_ACTIVEHIGH,     //default for digital output 4
   CONFIRM_PRESETCHANGE_ACTIVEHIGH,//default for digital output 5
   OVERLOADALARM_ACTIVEHIGH,  //default for digital output 6
   PLUSPKPOWER_LIMITALARM_ACTIVEHIGH,  //default for digital output 7
   MINUSPKPOWER_LIMITALARM_ACTIVEHIGH, //default for digital output 8
};

//Default functions to be assigned on analog input pins
const enum USERDEFINED_ANALOGINPUTFUNC BransonDefaultAnalogInputFunctions[LAST_ANALOG_INPUT] =
{
   AMPL_CONTROL,  //default for analog input 1
   FREQ_CONTROL   //default for analog input 2
};

//Defaults functions to be assigned on analog output pins
const enum USERDEFINED_ANALOGOUTPUTFUNC BransonDefaultAnalogOutputFunctions[LAST_ANALOG_OUTPUT] =
{
	POWER_OUT, //default for analog output 1
	AMPL_OUT   //default for analog output 2
};

/*************************ACTUATOR Defaults*************************************************/
//Default functions to be assigned on digital input pins
const enum USERDEFINED_DIGINPUTFUNC ActuatorDefaultDigInputFunctions[LAST_DIGITAL_INPUT] =
{
   STARTCYCLE_ACTIVEHIGH, //default for digital input 1
   EXTSEEK_ACTIVEHIGH,  //default for digital input 2
   EXTRESET_ACTIVEHIGH, //default for digital input 3
   MEMCLEAR_ACTIVEHIGH, //default for digital input 4
   UPPERLIMITSWITCH_ACTIVEHIGH, //default for digital input 5
   CYCLEABORT_ACTIVEHIGH,  //default for digital input 6
   GROUNDDETECT_ACTIVEHIGH, //default for digital input 7
   INPUTPIN_DISABLED,			//default for digital input 8
   TRIGGERSWITCH_ACTIVEHIGH //default for digital input 9
};

//Default functions to be assigned on digital output pins
const enum USERDEFINED_DIGOUTPUTFUNC ActuatorDefaultDigOutputFunctions[LAST_DIGITAL_OUTPUT] =
{
   READY_ACTIVEHIGH,       //default for digital output 1
   SONICACTIVE_ACTIVEHIGH, //default for digital output 2
   GENALARM_ACTIVEHIGH,    //default for digital output 3
   SEEKOUT_ACTIVEHIGH,     //default for digital output 4
   CYCLESTARTOUT_ACTIVEHIGH,
   OVERLOADALARM_ACTIVEHIGH,  //default for digital output 6
   OUTPUTPIN_DISABLED,  //default for digital output 7
   ACTUATORHOME_ACTIVEHIGH//default for digital output 8
};

//Default functions to be assigned on analog input pins
const enum USERDEFINED_ANALOGINPUTFUNC ActuatorDefaultAnalogInputFunctions[LAST_ANALOG_INPUT] =
{
   AMPL_CONTROL,      //default for analog input 1
   CUSTOM_INPUT1   //default for analog input 2
};

//Defaults functions to be assigned on analog output pins
const enum USERDEFINED_ANALOGOUTPUTFUNC ActuatorDefaultAnalogOutputFunctions[LAST_ANALOG_OUTPUT] =
{
   POWER_OUT, //default for analog output 1
   AMPL_OUT   //default for analog output 2
};
/********************************************************************************/


/*************************RF Switching Defaults*************************************************/
//Default functions to be assigned on digital input pins
const enum USERDEFINED_DIGINPUTFUNC RFDefaultDigInputFunctions[LAST_DIGITAL_INPUT] =
{
   EXTSTART_ACTIVEHIGH, //default for digital input 1
   EXTSEEK_ACTIVEHIGH,  //default for digital input 2
   EXTRESET_ACTIVEHIGH, //default for digital input 3
   MEMCLEAR_ACTIVEHIGH, //default for digital input 4
   RECALLPRESET_1_ACTIVEHIGH, //default for digital input 5
   RECALLPRESET_2_ACTIVEHIGH, //default for digital input 6
   RECALLPRESET_4_ACTIVEHIGH, //default for digital input 7
   RFSTATUSFEEDBACK_ACTIVEHIGH, //default for digital input 8
   LOCKOUT_ACTIVEHIGH, //default for digital input 9
};

//Default functions to be assigned on digital output pins
const enum USERDEFINED_DIGOUTPUTFUNC RFDefaultDigOutputFunctions[LAST_DIGITAL_OUTPUT] =
{
   READY_ACTIVEHIGH,       //default for digital output 1
   SONICACTIVE_ACTIVEHIGH, //default for digital output 2
   GENALARM_ACTIVEHIGH,    //default for digital output 3
   SEEKOUT_ACTIVEHIGH,     //default for digital output 4
   CONFIRM_PRESETCHANGE_ACTIVEHIGH,
   RF_BITZERO_SELECT_ACTIVEHIGH,  //default for digital output 6
   RF_BITONE_SELECT_ACTIVEHIGH,  //default for digital output 7
   RF_BITTWO_SELECT_ACTIVEHIGH,//default for digital output 8
};

//Default functions to be assigned on analog input pins
const enum USERDEFINED_ANALOGINPUTFUNC RFDefaultAnalogInputFunctions[LAST_ANALOG_INPUT] =
{
   AMPL_CONTROL,      //default for analog input 1
   FREQ_CONTROL   //default for analog input 2
};

//Defaults functions to be assigned on analog output pins
const enum USERDEFINED_ANALOGOUTPUTFUNC RFDefaultAnalogOutputFunctions[LAST_ANALOG_OUTPUT] =
{
   POWER_OUT, //default for analog output 1
   AMPL_OUT   //default for analog output 2
};
/********************************************************************************/

/*  void UserIO::InitUserIO(void)
 *
 *   Purpose
 * 	 	To initialize the adc and dac chip objects to read/write anlog values
 *     	from MI-A board . Validate the checksum of IO configuration data in FRAM.
 *    	This function gets called once from  PowerUpInitialization()  function.
 *
 *   Entry condition
 *       NONE.
 *
 *   Exit condition
 *   	NONE.
 */
void UserIO::InitUserIO(void)
{
   UINT16 Calcrc = 0;
   //Allocate object of DAC device to access DAC chip and assign to class variable
	//Note: The chip support having different output voltages for
	//first two channels and another reference for another two channels
	//we are giving the same reference voltage. refer to MI-A schematic
	DAC = new TLV5604(Board::GetSPI(0, 5, 5), DSPICSPIN_DAC, DSPICTAR_DAC);
	//Allocate object of ADC device to access  ADC chip and assign to class  //variable
	ADC = new MCP3208(Board::GetSPI(0, 5, 5), DSPICSPIN_ADC, DSPICTAR_ADC);
	ADC->SetResolution(ADC_RESOLUTION_11BIT);
	//Check the validity of FRAM data
	Calcrc = ByteCRC((UINT8 *)USERIOConfig, (sizeof(USERIOConfiguration) - sizeof(USERIOConfig->UserIOCRC)));
	if((USERIOConfig->UserIOCRC !=  Calcrc) || (USERIOConfig->UserIOCRC == 0)){
	   pprintf("\n UserIOCRC=%d, CalCRC=%d\n", USERIOConfig->UserIOCRC, Calcrc);
	   Log::WriteEventLog(EVENT_CRC_ERROR, USERIOConfig->UserIOCRC, Calcrc, CRC_ERROR_IOCONFIG);
	   ResetUserIO();
   }
	else{
    	ConfigureUserIOsFromFRAM(); 	//to assign functions saved in FRAM to digital and analog IO pins.
    	InitCableDetectEnabledFlag();	// to check whether the CableDetect  function was assigned on some digital input pin.
   }
   if((USERIOConfig->AnalogInUse[1] == CUSTOM_INPUT1) ||(USERIOConfig->AnalogInUse[2] == CUSTOM_INPUT1))
       CustomInput1Configured = TRUE;
   if((USERIOConfig->AnalogInUse[1] == CUSTOM_INPUT2) ||(USERIOConfig->AnalogInUse[2] == CUSTOM_INPUT2))
       CustomInput2Configured = TRUE;

   InitExternalToFreqOffsetMapping();
}

/*  void InitExternalToFreqOffsetMapping()
 *
 *   Purpose
 * 	 	To initialize the Mapping between external frequency analog ADC value and corrsponding
 *		frequency offset. This function is called from UserIO::InitUserIO function at powerup.
 *   Entry condition
 *       NONE.
 *
 *   Exit condition
 *   	NONE.
 */
void InitExternalToFreqOffsetMapping()
{
	SINT32 Indx;
	for(Indx = 0; Indx < (MAX_ADC_RAWVALUE + 1); Indx++){
		if(Indx < RAWVAL_MINIMUM)
			ExtRawFreqToFreqOffset[Indx] = 0;
		else
			ExtRawFreqToFreqOffset[Indx] = ((MaxOffset * (Indx - EXT_FREQ_SCALINGFACTOR)) / EXT_FREQ_SCALINGFACTOR);
	}
}

/*  void UserIO::ResetUserIO(void)
 *
 *  Purpose
 *     This function is used to reset the UserIO to default.
 *     This function gets called from ColdStart()function and InitUserIO() function.
 *
 *  Entry condition
 *     None.
 *
 *  Exit condition
 *     None.
 */
void UserIO::ResetUserIO(void)
{
	ClearFRAMIOConfig();		//to clear the content of FRAM area saving IO configuration data.
	ConfigureBransonDefaultUserIOs();	//() to select the default functions on digital and analog IO pins.
   USERIOConfig->UserIOCRC = ByteCRC((UINT8 *)USERIOConfig, (sizeof(USERIOConfiguration) - sizeof(USERIOConfig->UserIOCRC)));
}

/*  void UserIO::ConfigureUserIOsFromFRAM(void)
 *
 *   Purpose
 *   	This function is used to assign the functions saved in FRAM to digital and analog IO pins.
 *   	This function is called once at power up from InitUserIO() function.
 *
 *   Entry condition:
 *   	None.
 *
 *   Exit condition:
 *   	None.
 */
void UserIO::ConfigureUserIOsFromFRAM(void)
{
	UINT16 Indx;
	for(Indx = FIRST_DIGITAL_INPUT; Indx <= LAST_DIGITAL_INPUT; Indx++)
		UserIO::AssignDigitalInputFunction((DIGITAL_INPUTS)Indx, USERIOConfig->DigInUse[Indx], USERIOConfig->DigInUseStatus[Indx]);

	for(Indx = FIRST_DIGITAL_OUTPUT; Indx <= LAST_DIGITAL_OUTPUT; Indx++)
		UserIO::AssignDigitalOutputFunction((DIGITAL_OUTPUTS)Indx, USERIOConfig->DigOutUse[Indx], USERIOConfig->DigOutUseStatus[Indx]);

	for(Indx = FIRST_ANALOG_INPUT; Indx <= LAST_ANALOG_INPUT; Indx++)
		UserIO::AssignAnalogInputFunction((ANALOG_INPUTS)Indx, USERIOConfig->AnalogInUse[Indx], USERIOConfig->AnalogInUseStatus[Indx]);

	for(Indx = FIRST_ANALOG_OUTPUT; Indx <= LAST_ANALOG_OUTPUT; Indx++)
		UserIO::AssignAnalogOutputFunction((ANALOG_OUTPUTS)Indx, USERIOConfig->AnalogOutUse[Indx], USERIOConfig->AnalogOutUseStatus[Indx]);
}

/*   void UserIO::InitCableDetectEnabledFlag(void)
 *
 *   Purpose
 *   	To check whether cable detect function was assigned on some digital input pin.
 *      The function gets called from InitUserIO () function.
 *
 *   Entry condition
 *		None.
 *
 *   Exit condition
 *      NONE.
 */
void UserIO::InitCableDetectEnabledFlag(void)
{
	SINT32 Index = 0;
	BOOLEAN ExitFlag = FALSE;
	BOOLEAN TempFlag = FALSE;

	for(Index = 1; ((Index < NUM_DIN) && (ExitFlag == FALSE)); Index++){
		if(USERIOConfig->DigInUse[Index] == CABLEDETECT_ACTIVEHIGH){
			ExitFlag = TRUE;
			TempFlag = USERIOConfig->DigInUseStatus[Index];
		}
	}
	SetCableDetectEnabledFlag(TempFlag);
}

/*  void UserIO::AssignDigitalOutputFunction(enum DIGITAL_OUTPUTS Output, UINT16 FuncNum, BOOLEAN Status)
 *
 *  Purpose
 *   	This function is used to assign one of the digital output functions given in parameter FuncNum to a digital output
 *    	pin given in parameter Output.
 *   	This function gets called from website module when IO Configuration is modified and ConfigureDefaultUserIO()
 *    	function when default function numbers are to be assigned on output pins.
 *
 *  Entry condition
 *   	Output: digital output number
 *   	FuncNum: Function to be assigned on digital output pin
 *   	Status: param denote assigned function shall be activated or not.
 *
 *  Exit condition
 *  	None.
 */
void UserIO::AssignDigitalOutputFunction(enum DIGITAL_OUTPUTS Output, UINT16 FuncNum, BOOLEAN Status)
{
	ASSERT( Output <= DIGOUT8);
	ASSERT( FuncNum < USERDEFINED_DIGOUTPUTFUNCEND );

	if((OutPtr[FuncNum].Control & PresentConfiguration) == PresentConfiguration){
		USERIOConfig->OutputMask = (USERIOConfig->OutputMask & ~(OutBitsLo[Output]));			//(OutputMask & ~(OutBitsHi[Output]);//strip out old bit
		if(Status == ACTIVATED){
	    	if(OutPtr[FuncNum].Active == LOW)
	    		USERIOConfig->OutputMask = ((USERIOConfig->OutputMask) | OutBitsLo[Output]);	//set for active low
	    	else
	    		USERIOConfig->OutputMask = ((USERIOConfig->OutputMask) | OutBitsHi[Output]);	//set for active high
		}//if(Status == ACTIVATED)
		USERIOConfig->DigOutUseStatus[Output] = DigOutUseStatus[Output] = Status;
		USERIOConfig->DigOutUse[Output] = FuncNum; //save function number assigned to input
		OutValue[Output] = OutPtr[FuncNum].Flag;  //put variable address in input pin location
		OutputMask = USERIOConfig->OutputMask;
	   //CRC Calculate
	   USERIOConfig->UserIOCRC = ByteCRC((UINT8 *)USERIOConfig, (sizeof(USERIOConfiguration) - sizeof(USERIOConfig->UserIOCRC)));
	}
}

/*  void UserIO::AssignDigitalInputFunction(enum DIGITAL_INPUTS Input, UINT16 FuncNum, BOOLEAN Status)
 *
 *   Purpose
 *  	This function  is used  to assign one of the digital input functions given in parameter
 *  	FuncNum to a digital input pin given in parameter Input.
 *		This function gets called from  website module when IO Configuration is modified and
 *   	ConfigureDefaultUserIO() function when default function numbers are to be assigned on input pins.
 *
 *   Entry condition
 *     	Input: digital input number
 *     	FuncNum: Function to be assigned on digital input pin
 *     	Status: param denote assigned function shall be activated or not..
 *
 *   Exit condition
 *   	NONE.
 */
void UserIO::AssignDigitalInputFunction(enum DIGITAL_INPUTS Input, UINT16 FuncNum, BOOLEAN Status)
{
	ASSERT(Input <= DIGIN9);
	ASSERT(FuncNum < USERDEFINED_DIGINPUTFUNCEND);

	if((InPtr[FuncNum].Control & PresentConfiguration) == PresentConfiguration){
		(USERIOConfig->InputMask) = ((USERIOConfig->InputMask) & ~(InBitsLo[Input]));		//(InputMask AND ~(INBITSHI[Input])		// strip out old bit
		if(Status == ACTIVATED){
			if(InPtr[FuncNum].Active == LOW)
		    	USERIOConfig->InputMask = ((USERIOConfig->InputMask) | InBitsLo[Input]);	// set for active low
			else
		    	USERIOConfig->InputMask = ((USERIOConfig->InputMask) | InBitsHi[Input]);	// set for active high
		}//if(Status == ACTIVATED)
	    //check whether some function was already assigned at input pin
	    //and now the pin is being unassigned.
	    //If yes make the previous flag inactive
	   if((USERIOConfig->DigInUse[Input] != FuncNum) || (Status == DEACTIVATED)){
	   	if(InValue[Input])
		   	* (InValue[Input]) = FALSE;
	   }
	   USERIOConfig->DigInUseStatus[Input] = DigInUseStatus[Input] = Status;
	   USERIOConfig->DigInUse[Input] = FuncNum;				// save function number assigned to input
	   InValue[Input] = InPtr[FuncNum].Flag;				// put variable address in input pin location
	   InputMask =  USERIOConfig->InputMask;
	   //CRC Calculate
	   USERIOConfig->UserIOCRC = ByteCRC((UINT8 *)USERIOConfig, (sizeof(USERIOConfiguration) - sizeof(USERIOConfig->UserIOCRC)));
	   if(FuncNum == CABLEDETECT_ACTIVEHIGH)
	      CableDetectedPin = Input;
	   if((FuncNum == PARTINPLCAE_ACTIVEHIGH)||(FuncNum == PARTINPLACE_ACTIVELOW))
		   PartInPlaceConfigured = TRUE;
	   if((FuncNum == INTERLOCKINPLACE_ACTIVEHIGH)||(FuncNum == INTERLOCKINPLACE_ACTIVELOW))
		   InterlockConfigured = TRUE;
	}
}

/*  void UserIO::AssignAnalogOutputFunction(enum ANALOG_OUTPUTS Output, UINT16 FuncNum, BOOLEAN Status)
 *
 *   Purpose
 *  	This function  is used  to assign one of the analog output functions given in parameter FuncNum to a Analog
 *    	output pin given in parameter Output.
 *  	This function gets called from  website module when IO Configuration is modified and ConfigureDefaultUserIO()
 *   	function when default function numbers are to be assigned on output pins.
 *
 *   Entry condition
 *  	Output: Analog output number
 *  	FuncNum: Function to be assigned on analog output pin
 *  	Status: param denote assigned function shall be activated or not.
 *
 *   Exit condition
 *   	None.
 */
void UserIO::AssignAnalogOutputFunction(enum ANALOG_OUTPUTS Output, UINT16 FuncNum, BOOLEAN Status)
{
   ASSERT(Output <= AOUT2 );
   ASSERT(FuncNum < USERDEFINED_ANALOGOUTPUTFUNCEND);
   if((AOutPtr[FuncNum].Control & PresentConfiguration) == PresentConfiguration){
      //save function number assigned to input
	   USERIOConfig->AnalogOutUse[Output] = FuncNum;
	   USERIOConfig->AnalogOutUseStatus[Output] = AnalogOutUseStatus[Output] = Status;
	   AnalogOutValue[Output] = AOutPtr[FuncNum].Value;// put variable address in input pin location
	   //CRC Calculate
	   USERIOConfig->UserIOCRC = ByteCRC((UINT8 *)USERIOConfig, (sizeof(USERIOConfiguration) - sizeof(USERIOConfig->UserIOCRC)));
   }
}

/*  void UserIO::AssignAnalogInputFunction(enum ANALOG_INPUTS Input, UINT16 FuncNum, BOOLEAN Status)
 *
 *   Purpose
 *  	This function is used to assign one of the analog input functions given in parameter FuncNum to a Analog input
 *   	pin given in parameter Input.
 *  	This function gets called from  website module when IO Configuration is modified and ConfigureDefaultUserIO()
 *  	function when default function numbers are to be assigned on input pins.
 *
 *   Entry condition
 *  	Input: Analog input number
 *  	FuncNum: Function to be assigned on analog input pin
 *  	Status: param denote assigned function shall be activated or not..
 *
 *   Exit condition
 *		None.
 */
void UserIO::AssignAnalogInputFunction(enum ANALOG_INPUTS Input, UINT16 FuncNum, BOOLEAN Status)
{
	ASSERT( Input <= AIN2 );
	ASSERT( FuncNum < USERDEFINED_ANALOGINPUTFUNCEND );
	if((AInPtr[FuncNum].Control & PresentConfiguration) == PresentConfiguration){
		//check whether some function was already assigned at input pin
		//and now the pin is being unassigned.
		//If yes make the previous variable holding analog input value 0
		if((USERIOConfig->AnalogInUse[Input] != FuncNum) || (Status == DEACTIVATED)){
			if(AnalogInValue[Input])
				* (AnalogInValue[Input]) = 0;
		}
		USERIOConfig->AnalogInUseStatus[Input] = AnalogInUseStatus[Input] = Status;
		USERIOConfig->AnalogInUse[Input] = FuncNum;		// save function number assigned to input
		AnalogInValue[Input] = AInPtr[FuncNum].Value;	// put variable address in input pin location
	   if(USERIOConfig->AnalogInUse[Input] == CUSTOM_INPUT1)
	      CustomInput1Configured = TRUE;
	   if(USERIOConfig->AnalogInUse[Input] == CUSTOM_INPUT2)
	      CustomInput2Configured = TRUE;
		//CRC Calculate
		USERIOConfig->UserIOCRC = ByteCRC((UINT8 *)USERIOConfig, (sizeof(USERIOConfiguration) - sizeof(USERIOConfig->UserIOCRC)));
	}
}

/*  SINT32 UserIO::DigFilterMIAAin(UINT32 Element, ANALOG_INPUTS InputNumber)
 *
 *   Purpose:
 *   	To apply software filter on Analog inputs read from MI-A board
 *  	This function is called from RadInputs function of this class and private to this class.
 *
 *   Entry condition
 *  	Element- Represents the value of analog input number passed in input number param.
 *  	InputNumber- Represents the analog input number
 *
 *   Exit condition
 *  	It returns the average of the last MAXADCVALUES_FILTER points.
 *  	Using: (Sum of  MAXADCVALUES_FILTER values + New Value  – Last value) / MAXADCVALUES_FILTER
 */
SINT32 UserIO::DigFilterMIAAin(SINT32 Element, ANALOG_INPUTS InputNumber)
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

/*  void UserIO::ReadInputs(void)
 *
 *   Purpose:
 *   	To read all digital inputs from MI-A board and apply mask them to make usable by Application code.
 *  	To read analog inputs from ADC chip.
 *  	This function is called from MIATask forever loop every TIMERINTERRUPT_INTERVALUS  microseconds interval.
 *
 *   Entry condition
 *   	None.
 *
 *   Exit condition
 *   	None.
 */
void UserIO::ReadInputs(void)
{
	UINT32 Tempvar = 0;
	UINT16 InData;			// will contain all inputs values at once
	UINT16 Loop;
	SINT32 Ch = 0;
	int Data[3];
	InData = ReadDigitalIO();	//Value is raw data as read by input port and debounced
	InData = (InData ^ (InputMask));	//invert whatever signals need to be inverted.
 ///** Now we need to take bits and convert to BOOLEAN variables for system use  **///
	for(Loop = 1; Loop < NUM_DIN; Loop++){
		if((InValue[Loop] != NULL) && (DigInUseStatus[Loop] == ACTIVATED)){
			if((InData & InBitsLo[Loop]) == InBitsLo[Loop]){
				* InValue[Loop] = TRUE;
				switch(USERIOConfig->DigInUse[Loop]){
					case EXTSTART_ACTIVEHIGH:
					case EXTSTART_ACTIVELOW:
						Tempvar |= IOWELD_START;
						break;
					case STARTCYCLE_ACTIVEHIGH:
					case STARTCYCLE_ACTIVELOW:
						Tempvar |= IOCYCLE_START;
						break;
					case TRIGGERSWITCH_ACTIVEHIGH:
					case TRIGGERSWITCH_ACTIVELOW:
						Tempvar |= IOTRIGGER;
						break;
					case EXTSEEK_ACTIVEHIGH:
					case EXTSEEK_ACTIVELOW:
						Tempvar |= IOSEEK;
						break;
					case STARTHORNSCAN_ACTIVEHIGH:
					case STARTHORNSCAN_ACTIVELOW:
						Tempvar |= IOSCAN;
						break;
					case EXTTEST_ACTIVEHIGH:
					case EXTTEST_ACTIVELOW:
						Tempvar |= IOEXTTEST;
						break;
					case EXTRESET_ACTIVEHIGH:
					case EXTRESET_ACTIVELOW:
						Tempvar |= IORESET;
						break;
				}
			}
			else
				* InValue[Loop] = FALSE;
		}

	}//for(Loop = 1; Loop < NUM_DIN; Loop++)
	if((CableDetectEnabled == TRUE) && (CableDetect == FALSE) && (IsManualFunctionAvailable() == TRUE))
		CableFailure = TRUE;
	else
		CableFailure = FALSE;

	if((CableFailure == TRUE) || (IsManualFunctionAvailable() == FALSE) || (WebIOEnabled == TRUE))
		Tempvar = 0;
	ControlVariable = (Tempvar | (ControlVariable & ~IOMASK));
	//see which channels need to be read
	ADC -> ReadInputs(0x03,&Data[0]);
	for(Loop = 1; Loop < NUM_AIN; Loop++){
		if((AnalogInValue[Loop] != NULL) && (AnalogInUseStatus[Loop] == ACTIVATED)){
			Ch = InChannles[Loop];
			* AnalogInValue[Loop] =	DigFilterMIAAin(Data[Loop - 1] , (ANALOG_INPUTS)Loop);
			//Data[Loop - 1];
		}
	}
}

/*  void UserIO::WriteOutputs(void)
 *
 *   Purpose:
 *   	To combine the Application flags rpresenting digital output values in a single variable and mask them before calling
 *     	function to write on hardware through memory mapped IO.
 *      To write analog output values to DAC chip.
 *   	This function is called from MIATask forever loop every TIMERINTERRUPT_INTERVALUS  microseconds interval.
 *
 *   Entry condition
 *   	None.
 *
 *   Exit condition
 *		None.
 */
void UserIO::WriteOutputs(void)
{
   UINT16 OutData = 0;	   // will contain value to be written to output ports
   UINT16 Loop;
   UINT32 AnalogVal;
   //
   int DacVals[NUM_AOUT];
   int DacChannels = 0;;
   //
   /** First convert all BOOLEAN variables into a single value bit assignment **/
   for(Loop = 1; Loop < NUM_DOUT; Loop++){
      if((OutValue[Loop] != NULL) && (DigOutUseStatus[Loop] == ACTIVATED)){
         if((* OutValue[Loop] == TRUE))
            OutData = (OutData | OutBitsLo[Loop]);
	  }
   }
   OutData = (OutData ^ (OutputMask));
   WriteDigitalIO(OutData);
   //check out which channels need to be written
   //and what values need to be written
   for(Loop = 1; Loop < NUM_AOUT; Loop++){
      if((AnalogOutValue[Loop] != NULL) && (AnalogOutUseStatus[Loop] == ACTIVATED)){
         AnalogVal = * AnalogOutValue[Loop];
         DacVals[Loop - 1] = * AnalogOutValue[Loop];
      }
	  else{
		 AnalogVal = 0;
		 DacVals[Loop - 1] = 0;
	  }
     DacChannels |= BIT(OutChannles[Loop]);
//	  Ch = (OutChannles[Loop]);
//	  UserIO::DAC -> WriteOutput(Ch, AnalogVal);
   }
   UserIO::DAC->WriteOutputs(DacChannels, DacVals);
}

/*  SINT32 UserIO::ReadDigitalIO(void)
 *
 *   Purpose
 *    	Read the memory mapped IO contents and apply bitwise debounce algorithm.
 *      Bitwise debounce algorithm: AND last DEBOUNCE_TIME values together to create ANDResult.
 *      OR last DEBOUNCE_TIME values to create ORResult.
 *      If a bit is the same in all samples, it will be either 1 in ANDResult(all samples were 1) or 0 in ORResult all samples
 *   	were 0). Complement the ORResult, now a 1-bit in either mask corresponds to a bit that is the same in all samples.
 *   	OR together ANDResult and ~ORResult, we have a mask of all bits that are the same in all samples.
 *   	Having done all that, now just use this mask to strip bits out of LastInput value and substitute the new bits from the port.
 *
 *   Entry Condition
 *   	This function gets called from ReadInputs() function of this class.
 *
 *   Exit Condition
 *   	returns the value read at digital inputs after applying debouncing algorithm.
 */
//Note - Debounce time sets up minimum limit  for which the weld must be
//activated and one can not have weld cycle of less than Debounce time ms from IOs.
SINT32 UserIO::ReadDigitalIO(void)
{
   static UINT16  DebounceIndex = 0;
   static UINT16  DiginValues[DEBOUNCE_TIME];
   static UINT16  LastInput = 0;
   UINT16  ANDResult;
   UINT16  ORResult;
   UINT16  Mask;
   UINT16  CurrentInput;
   UINT16  LocalIndex;

   CurrentInput = DiginValues[DebounceIndex] = DIGITAL_IO_READADDR;// to CurrentInput and DiginValues at index DebounceIndex
   DebounceIndex++;
   if(DebounceIndex >= DEBOUNCE_TIME)
      DebounceIndex = 0;
   ANDResult = DiginValues[0];
   ORResult  = DiginValues[0];
   for(LocalIndex = 1; LocalIndex < DEBOUNCE_TIME; LocalIndex++){
      ANDResult &= DiginValues[LocalIndex];
      ORResult  |= DiginValues[LocalIndex];
   }
   ORResult = ~ORResult;
   Mask = ANDResult | ORResult;
   LastInput = ((LastInput & (~Mask)) | (CurrentInput & Mask));
   return LastInput;
}

/*  void UserIO::ClearFRAMIOConfig(void)
 *
 *   Purpose
 *   	Clear the FRAM area which saves IO configuration data.
 *  	This function gets called from ResetUserIO().
 *
 *   Entry condition
 *   	None.
 *
 *   Exit condition
 *		None.
 */
void UserIO::ClearFRAMIOConfig(void)
{
   //Clear global USERIOConfig object;
   memset(USERIOConfig, 0, sizeof(USERIOConfiguration));
}

/*  void UserIO::ClearOutputs(void)
 *
 *   Purpose
 *   	Clear all outputs on MI_A  board.
 *   	This function is called from MIATask::Run() function before entering the forever loop.
 *
 *   Entry condition
 *   	None.
 *
 *   Exit condition
 *		None.
 */
void UserIO::ClearOutputs(void)
{
	WriteDigitalIO(0);
}

/*  void UserIO::WriteDigitalIO(SINT32 Val)
 *
 *   Purpose
 *   	Write digital outputs to MI_A  board using memory mapped IO adress.
 *   	This function is called from WriteOutputs() function of this class.
 *
 *   Entry condition
 *   	val- The combined output value for all digital outputs.
 *
 *   Exit condition
 *		None.
 */
void UserIO::WriteDigitalIO(SINT32 Val){
	UINT8 OpVal = Val & 0xFF;
	if(AdvanceConfigData->Advrnd.RevDigitalOP == TRUE)
		OpVal = ~OpVal;
	DIGITAL_IO_WRITEADDR = OpVal;
}

/*  void UserIO::ConfigureBransonDefaultUserIOs(void)
 *
 *  Purpose
 *   	This function is used to assign Branson defaults on digital and analog IO pins.
 *   	This function is called from ResetUserIO() function.
 *
 *  Entry condition
 *   	None.
 *
 *  Exit condition
 *   	None.
 */
void UserIO::ConfigureBransonDefaultUserIOs(void)
{
	UINT16 Indx;
	DigitalInputConfigured = TRUE;
	Log::WriteEventLog(EVENT_USERIO_DEFINED, 0, BRANSONDEFAULTS);
	SetCableDetectEnabledFlag(FALSE);
	//Set defaults for digital Inputs
	for(Indx = FIRST_DIGITAL_INPUT; Indx <= LAST_DIGITAL_INPUT; Indx++)
		UserIO::AssignDigitalInputFunction((DIGITAL_INPUTS)Indx, BransonDefaultDigInputFunctions[Indx -1], ACTIVATED);
	//set defaults for digital Outputs
	for(Indx = FIRST_DIGITAL_OUTPUT; Indx <= LAST_DIGITAL_OUTPUT; Indx++)
		UserIO::AssignDigitalOutputFunction((DIGITAL_OUTPUTS)Indx, BransonDefaultDigOutputFunctions[Indx -1], ACTIVATED);
	//set defaults for Analog Inputs
	for(Indx = FIRST_ANALOG_INPUT; Indx <= LAST_ANALOG_INPUT; Indx++)
		UserIO::AssignAnalogInputFunction((ANALOG_INPUTS)Indx, BransonDefaultAnalogInputFunctions[Indx -1], ACTIVATED);
	//Default for Analog Outputs
	for(Indx = FIRST_ANALOG_OUTPUT; Indx <= LAST_ANALOG_OUTPUT; Indx++)
		UserIO::AssignAnalogOutputFunction((ANALOG_OUTPUTS)Indx, BransonDefaultAnalogOutputFunctions[Indx -1],ACTIVATED);
}

/*  void UserIO::ConfigureActuatorDefaultUserIOs(void)
 *
 *  Purpose
 *    This function is used to assign Actuator defaults on digital and analog IO pins.
 *    This function is called from ResetUserIO() function.
 *
 *  Entry condition
 *    None.
 *
 *  Exit condition
 *    None.
 */
void UserIO::ConfigureActuatorDefaultUserIOs(void)
{
   UINT16 Indx;
   Log::WriteEventLog(EVENT_USERIO_DEFINED, 0, ACTUATORDEFAULTS);
   DigitalInputConfigured = TRUE;
   SetCableDetectEnabledFlag(FALSE);
   //Set defaults for digital Inputs
   for(Indx = FIRST_DIGITAL_INPUT; Indx <= LAST_DIGITAL_INPUT; Indx++)
      UserIO::AssignDigitalInputFunction((DIGITAL_INPUTS)Indx, ActuatorDefaultDigInputFunctions[Indx -1], ACTIVATED);
   //set defaults for digital Outputs
   for(Indx = FIRST_DIGITAL_OUTPUT; Indx <= LAST_DIGITAL_OUTPUT; Indx++)
      UserIO::AssignDigitalOutputFunction((DIGITAL_OUTPUTS)Indx, ActuatorDefaultDigOutputFunctions[Indx -1], ACTIVATED);
   //set defaults for Analog Inputs
   for(Indx = FIRST_ANALOG_INPUT; Indx <= LAST_ANALOG_INPUT; Indx++)
      UserIO::AssignAnalogInputFunction((ANALOG_INPUTS)Indx, ActuatorDefaultAnalogInputFunctions[Indx -1], ACTIVATED);
   //Default for Analog Outputs
   for(Indx = FIRST_ANALOG_OUTPUT; Indx <= LAST_ANALOG_OUTPUT; Indx++)
      UserIO::AssignAnalogOutputFunction((ANALOG_OUTPUTS)Indx, ActuatorDefaultAnalogOutputFunctions[Indx -1],ACTIVATED);
}

/*  void UserIO::ConfigureRFDefaultUserIOs(void)
 *
 *  Purpose
 *    This function is used to assign RF defaults on digital and analog IO pins.
 *    This function is called from ResetUserIO() function.
 *
 *  Entry condition
 *    None.
 *
 *  Exit condition
 *    None.
 */
void UserIO::ConfigureRFDefaultUserIOs(void)
{
   UINT16 Indx;
   Log::WriteEventLog(EVENT_USERIO_DEFINED, 0, RFSWITCHINGDEFAULTS);
   DigitalInputConfigured = TRUE;
   SetCableDetectEnabledFlag(FALSE);
   //Set defaults for digital Inputs
   for(Indx = FIRST_DIGITAL_INPUT; Indx <= LAST_DIGITAL_INPUT; Indx++)
      UserIO::AssignDigitalInputFunction((DIGITAL_INPUTS)Indx, RFDefaultDigInputFunctions[Indx -1], ACTIVATED);
   //set defaults for digital Outputs
   for(Indx = FIRST_DIGITAL_OUTPUT; Indx <= LAST_DIGITAL_OUTPUT; Indx++)
      UserIO::AssignDigitalOutputFunction((DIGITAL_OUTPUTS)Indx, RFDefaultDigOutputFunctions[Indx -1], ACTIVATED);
   //set defaults for Analog Inputs
   for(Indx = FIRST_ANALOG_INPUT; Indx <= LAST_ANALOG_INPUT; Indx++)
      UserIO::AssignAnalogInputFunction((ANALOG_INPUTS)Indx, RFDefaultAnalogInputFunctions[Indx -1], ACTIVATED);
   //Default for Analog Outputs
   for(Indx = FIRST_ANALOG_OUTPUT; Indx <= LAST_ANALOG_OUTPUT; Indx++)
      UserIO::AssignAnalogOutputFunction((ANALOG_OUTPUTS)Indx, RFDefaultAnalogOutputFunctions[Indx -1],ACTIVATED);
}

/*  void UserIO::ConfigureCustomDefaultUserIOs(void)
 *
 *  Purpose
 *    This function is used to assign Custom defaults on digital and analog IO pins.
 *    This function is called from ResetUserIO() function.
 *
 *  Entry condition
 *    None.
 *
 *  Exit condition
 *    None.
 */
void UserIO::ConfigureCustomDefaultUserIOs(void)
{
   UINT16 Indx;
   Log::WriteEventLog(EVENT_USERIO_DEFINED, 0, CUSTOMDEFAULTS);
   SetCableDetectEnabledFlag(FALSE);
   DigitalInputConfigured = TRUE;
   for(Indx = FIRST_DIGITAL_INPUT; Indx <= LAST_DIGITAL_INPUT; Indx++)
      UserIO::AssignDigitalInputFunction((DIGITAL_INPUTS)Indx, CustomDefaultUSERIOConfig.DigInUse[Indx], CustomDefaultUSERIOConfig.DigInUseStatus[Indx]);
   for(Indx = FIRST_DIGITAL_OUTPUT; Indx <= LAST_DIGITAL_OUTPUT; Indx++)
      UserIO::AssignDigitalOutputFunction((DIGITAL_OUTPUTS)Indx, CustomDefaultUSERIOConfig.DigOutUse[Indx], CustomDefaultUSERIOConfig.DigOutUseStatus[Indx]);
   for(Indx = FIRST_ANALOG_INPUT; Indx <= LAST_ANALOG_INPUT; Indx++)
      UserIO::AssignAnalogInputFunction((ANALOG_INPUTS)Indx, CustomDefaultUSERIOConfig.AnalogInUse[Indx], CustomDefaultUSERIOConfig.AnalogInUseStatus[Indx]);
   for(Indx = FIRST_ANALOG_OUTPUT; Indx <= LAST_ANALOG_OUTPUT; Indx++)
      UserIO::AssignAnalogOutputFunction((ANALOG_OUTPUTS)Indx, CustomDefaultUSERIOConfig.AnalogOutUse[Indx], CustomDefaultUSERIOConfig.AnalogOutUseStatus[Indx]);
}

/*  BOOLEAN UserIO::CheckSignalActive(BOOLEAN Signal)
 *
 *   Purpose
 *  	To check if the input signal is active, if cable detect function is enabled in one of the digital input pins.
 *  	This function is called by all functions of this class which returns the BOOLEAN flags represnting digital input functions.
 *
 *   Entry condition
 *  	Signal: The  signal to be checked active.
 *
 *   Exit condition
 *  	Returns signal value if the cable detect is not enabled.
 *  	else returns the signal value ANDED with CableDetect signal value.
 */
BOOLEAN UserIO::CheckSignalActive(BOOLEAN Signal)
{
	BOOLEAN RetVal = Signal;
	if(CableDetectEnabled)
		RetVal = Signal && CableDetect;
	if(IsManualFunctionAvailable() == FALSE)
		RetVal = FALSE;
	return RetVal;
}

/*   BOOLEAN UserIO::CheckForExternalAmpStep(void)
 *
 *   Purpose
 *  	This function checks the state of external amplitude step signal.
 *  	This function  is called from WC State Machine to check whether to switch the Amplitude from Amplitude1
 *  	to Amplitude 2 defined in preset.
 *
 *   Entry condition
 *
 *   Exit condition
 *  	returns the BOOLEAN value whether the signal is active or not
 */
BOOLEAN UserIO::CheckForExternalAmpStep(void)
{
	return CheckSignalActive(ExtAmpStep);
}

/*  SINT32 UserIO::CheckForExtRecallPreset(void)
 *
 *   Purpose
 *  	This function return the preset number selected from IO.
 *		This function is called by WCTask before weld cycle to check whether a preset has been recalled from IO if LoadNewpreset signal is active.
 *
 *	 Entry condition
 *	 	None.
 *
 *   Exit condition
 *  	returns the preset number selected from IO
 */
SINT32 UserIO::CheckForExtRecallPreset(void)// name in the documentation is Preset
{
	SINT32  PresetNum = 0;
	if((UserIO::CheckForLoadNewPreset() == FALSE) || ((CableDetectEnabled == TRUE) &&  (CableDetect == FALSE))
			|| (IsManualFunctionAvailable() == FALSE))
		PresetNum = 0;
	else{
		PresetNum = RecallPreset1 | (RecallPreset2 << 1) | (RecallPreset4 << 2) |
		(RecallPreset8 << 3) | (RecallPreset16 << 4) | (RecallPreset32 << 5);

		if(PresetNum > MAXIMUM_USER_PRESETS){
			PresetNum = 0;
			AlarmQueueWrite(SE03);
		}
	}
	return PresetNum;
}

/*  BOOLEAN UserIO::CheckForExternalStart(void)
 *
 *   Purpose
 *   	This function checks the state of external start signal.
 *   	This function is called from WCTask Run() function to check
 *   	whether external start(weld)is activated or not.
 *
 *   Entry condition
 *   	None.
 *
 *   Exit condition
 *   	returns the BOOLEAN value whether the signal is active or not
 */
BOOLEAN UserIO::CheckForExternalStart(void)
{
    return CheckSignalActive(ExtStart);
}

/*  BOOLEAN UserIO::CheckForExternalSonicsDelay(void)
 *
 *  Purpose
 *     This function checks the state of external sonic delay signal
 *     This function  is called from WC State machine
 *     to check whether external sonic dealy is activated or not.
 *
 *  Entry condition
 *     None.
 *
 *  Exit condition
 *     returns the BOOLEAN value whether the signal is active or not.
 */
BOOLEAN UserIO::CheckForExternalSonicsDelay(void)
{
   return CheckSignalActive(ExtSonicsDelay);
}

/* BOOLEAN UserIO::CheckForLockOutChange(void)
 *
 *  Purpose
 *     This function checks the state of external lockout  signal
 *	   for  LCD-A
 *     This function  is called from ReceiveCommandHandler task
 *     to check whether the LCD needs be locked out to make changes
 *     in preset.
 *
 *  Entry condition
 *	   None.
 *  Exit condition
 *     returns the BOOLEAN value whether the signal is active or not
 */
BOOLEAN UserIO::CheckForLockOutChange(void)
{
	return CheckSignalActive(LockOutChangeLCD);
}

/* BOOLEAN UserIO::CheckForSonicsDisable(void)
*
*  Purpose
*  This function checks the state of external sonic disable signal.
*  This function  is called from WC State machine
*  to check whether external sonic disable is activated or not for
*   running dummy weld cycle without ultrasonics.
*
*  Entry condition
*
*  Exit condition
*
*  returns the BOOLEAN value whether the signal is active or not
*/
BOOLEAN UserIO::CheckForSonicsDisable(void)
{
	return CheckSignalActive(SonicsDisable);
}

/*  BOOLEAN UserIO::CheckForInterLockInPlace(void)
*
*  Purpose
*   This function checks the state of InterLockInPlace signal.
*   This function  is called from WC State machine .
*   //Discuss.
*   Entry condition
*
*    Exit condition
*
*   returns the BOOLEAN value whether the signal is active or not
*/
BOOLEAN UserIO::CheckForInterLockInPlace(void)
{
	BOOLEAN SigActive = TRUE;
	if(InterlockConfigured == TRUE)
		SigActive = CheckSignalActive(InterLockInPlace);
	return SigActive;
}

/* BOOLEAN UserIO::CheckForLoadNewPreset(void)
*
*  Purpose
*  This function checks the state of load new preset signal.
*  This function  is called from WC State machine
*  to check whether a new preset is to be loaded from IO preset
*  select pins.
*  Entry condition:
*
*  Exit condition
*
*  returns the BOOLEAN value whether the signal is active or not
*/

BOOLEAN UserIO::CheckForLoadNewPreset(void)
{
   BOOLEAN RetVal = FALSE;
   if(((Sysconfig->ExternalPresetFlag == TRUE) && ((CheckForExternalStart() == TRUE) || (CheckForStartCycle() == TRUE)))
		 || (CheckSignalActive(LoadNewPreset)))
	  RetVal = TRUE;
   else
	  RetVal = FALSE;
   return RetVal;
}

BOOLEAN CheckForLoadNewPreset(void)
{
	return UserIO::CheckSignalActive(LoadNewPreset);
}

/*  BOOLEAN UserIO::CheckForCycleAbort(void)
 *
 *  Purpose
 *   This function checks the state of cycle abort signal.
 *   This function  is called from WC State machine to check whether a running weld cycle needs to be aborted.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    Returns BOOLEAN value whether the signal is active or not
 */
BOOLEAN UserIO::CheckForCycleAbort(void)
{
   return CheckSignalActive(CycleAbort);
}

/*  BOOLEAN UserIO::CheckForMemoryClear(void)
*
*   Purpose
*   This function checks the state of memory clear signal.
*   This function  is called from WC Tsk
*   to check whether memory frequency needs to be cleared or not.
*
*   Entry condition
*
*   Exit condition
*
*    returns the BOOLEAN value whether the signal is active or not
*/
BOOLEAN UserIO::CheckForMemoryClear(void)
{
	return CheckSignalActive(MemClear);
}

/* BOOLEAN UserIO::CheckForGroundDetect(void)
*
*  Purpose
*  This function checks the state of ground detect signal.
*  This function  is called from WC State machine
*  during weld cycle in ground detect mode.
*
*  Entry condition
*
*  Exit condition
*
*  returns the BOOLEAN value whether the signal is active or not
*/
BOOLEAN UserIO::CheckForGroundDetect(void)
{
	return CheckSignalActive(GroundDetect);
}

/*  BOOLEAN UserIO::CheckForHornScan(void)
*
*  Purpose
*  This function checks the state of External Horn scan signal.
*  This function  is called from WC Task Run() function to check whether horn scan is
*	activated from IO.
*
*  Entry condition
*
*  Exit condition
*
*  returns the BOOLEAN value whether the signal is active or not
*/
BOOLEAN UserIO::CheckForHornScan(void)
{
	return CheckSignalActive(StartHornScan);
}

/*  BOOLEAN UserIO::CheckForExternalTest(void)
*
*  Purpose
*  This function checks the state of External Test signal.
*  This function  is called from WC Task Run() function to check whether external Test is
*	activated from IO.
*
*  Entry condition
*
*  Exit condition
*
*   returns the BOOLEAN value whether the signal is active or not
*/
BOOLEAN UserIO::CheckForExternalTest(void)
{
	return CheckSignalActive(ExtTest);
}

/*  BOOLEAN UserIO::CheckForHornDownMode(void)
*
*  Purpose
*  This function checks the state of  Horn down mode signal.
*	//this is for future and not used in DCX
*	//Discuss
*   Entry condition
*
*  Exit condition
*
*  returns the BOOLEAN value whether the signal is active or not
*/
BOOLEAN UserIO::CheckForHornDownMode(void)
{
	return CheckSignalActive(HornDownMode);
}

/*  BOOLEAN UserIO::CheckForCableDetect(void)
*
*  Purpose
*  This function checks the state of Cable Detect signal.
*  This function  is called from WC Task Run() function if CableDetectEnabled flag is
*  TRUE.to check whether IO Cable has been removed or attached to log the events.
*
*  Entry condition
*
*  Exit condition
*
*  returns the BOOLEAN value whether the signal is active or not
*/
BOOLEAN UserIO::CheckForCableDetect(void)
{
	return CableDetect;
}

/* BOOLEAN UserIO:: CheckForCableDetectEanbled(void)
 *
 *  Purpose
 *     This function checks the whether the IO cable detection is enabled
 *	   or not. This function is called from WC task Run() function to monitor the IO cable.
 *
 *  Entry condition:None.
 *
 *  Exit condition
 *     returns the BOOLEAN value whether the signal is active or not
 */
BOOLEAN UserIO:: CheckForCableDetectEanbled(void)
{
	return CableDetectEnabled;
}

/* SINT32 UserIO::CheckForRfswitchfeedback(void)
 *
 * Purpose
 * This function is called after the call to SetRfselect() function of this class.
 * The horn number present in preset is transfered to RF switch using SetRfselect function of this class.
 * This function then returns the Horn number currently selected by RF switch as confirmation feedback before
 * starting sonics.
 * This function is called from WCTask if RF switching through UserIO is enabled.
 *
 * Entry condition: None.
 *
 * Exit condition:
 *    Returns the preset to be selected or one out of 7 horns selected by RF switch.
 */
SINT32 UserIO::CheckForRfswitchfeedback(void)
{
   SINT32  RetVal = 0;
   if((IsManualFunctionAvailable() == FALSE) || ((CableDetectEnabled == TRUE) && (CableDetect == FALSE)))
		RetVal = 0;
    else
    	RetVal = ((Rfbit3feedback << 3 ) | (Rfbit2feedback << 2 ) | (Rfbit1feedback << 1) | (Rfbit0feedback));
   return RetVal;
}

/* void UserIO::SetCableDetectEnabledFlag(BOOLEAN State)
 *
 *  Purpose:
 *  To enable or disable the IO cable monitoring.This function is called from InitUserIO function
 *	 at powerup and from Website module whenever IO configuration is modified from web page.
 *
 *  Entry condition:
 *	   state - Flag containing the value to enable or disable the IO cable monitoring.
 *
 *  Exit condition: None.
 *
 */
void UserIO::SetCableDetectEnabledFlag(BOOLEAN State)
{
	CableDetectEnabled = State ;// the state
}

/* void UserIO::SetCycleStartOut(BOOLEAN State)
 *
 * Purpose:
 *   Set or clear Cyclestartout flag. This function is called by WC State machine to set the flag if weld cycle is in process.
 *
 * Entry condition: State - The value to be assigned to Cyclestartout flag.
 *
 * Exit condition: None.
 */
void UserIO::SetCycleStartOut(BOOLEAN State)
{
	 Cyclestartout = State;
}

/* BOOLEAN  UserIO::CheckForPartInPlace(void)
 *
 * Purpose
 *   This function checks the part in place signal
 *
 *	//Discuss
 * Entry condition: None.
 *
 * Exit condition:
 *   returns the BOOLEAN value whether the signal is active or not
 */
BOOLEAN UserIO::CheckForPartInPlace(void)
{
	BOOLEAN SigActive = TRUE;
	if(PartInPlaceConfigured == TRUE)
		SigActive = CheckSignalActive(Partinplace);
	return SigActive;
}

/* BOOLEAN UserIO::CheckForStartCycle(void)
 *
 * Purpose:
 *   This function checks the Cycle Start signal.
 *   This function is called from WC task Run() function to check whether a weld cycle needs to be started or not.
 *
 * Entry condition:
 *   None.
 *
 * Exit condition:
 *   returns the BOOLEAN value whether the signal is active or not
 */
BOOLEAN UserIO::CheckForStartCycle(void)
{
   return CheckSignalActive(Startcycle);
}

/* BOOLEAN UserIO::CheckForULS(void)
 *
 *  Purpose
 *  This function checks ULS(upper limit switch) signal.This function is called from WC State
 *  machine to check ULS signal after a cycle is started, if the ActuatorPresent signal is TRUE.
 *
 *  Entry condition:None.
 *  Exit condition:
 *   returns the BOOLEAN value whether the signal is active or not
 */
BOOLEAN UserIO::CheckForULS(void)
{
	return CheckSignalActive(UpperLimitSwitch);
}

/* BOOLEAN UserIO::CheckForActuatorPresent(void)
 *
 *  Purpose
 *    This function checks Actuator present signal.
 * 	This function  is called from WC State machine to check whether the actuator is present or not.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition
 *    returns the BOOLEAN value whether the signal is active or not
*/
BOOLEAN UserIO::CheckForActuatorPresent(void)
{
	return CheckSignalActive(ActuatorPresent);
}

/* BOOLEAN UserIO::CheckForTrigger(void)
 *
 *  Purpose
 *     This function checks Trigger signal.
 *     //Discuss
 *
 *  Entry condition
 *     None.
 *
 *  Exit condition
 *     returns the BOOLEAN value whether the signal is active or not.
 */
BOOLEAN UserIO::CheckForTrigger(void)
{
     return CheckSignalActive(TriggerInput);
}

/* BOOLEAN UserIO::CheckForRfStatusfeedback(void)
 *
 * Purpose
 *   This function checks Trigger signal.This function is called from SelectRFRelayIO() function.
 *   //Discuss
 *
 * Entry condition
 *   None.
 *
 * Exit condition
 *   returns the BOOLEAN value whether the requested horn has been selected by RF switch
 */
BOOLEAN UserIO::CheckForRfStatusfeedback(void)
{
     return CheckSignalActive(Rfstatusfeedback);
}

/*  BOOLEAN UserIO::CheckForCycleRunning(void)
*
*   Purpose
*   To Check whether the Weld Cycle is running or not.
*
*  Entry condition
*	None.
*
*   Exit condition:None.
*   returns the BOOLEAN value whether the cycle is currently running or not
*
*/
BOOLEAN UserIO::CheckForCycleRunning(void)
{
	 return Cyclestartout;
}

/* void UserIO::SetReady(BOOLEAN State)
*
*  Purpose
*     Set or clear Ready flag. This function is called by WC State machine
* 	  to set the flag if system is ready for new input(i.e weld,scan etc) and to
*	  clear the flag if some process (weld,seek scan etc) is going on or there
*	  is some alarm condition.
*
*  Entry Condition
*     State: The value to be assigned to ReadyFlag.
*
*  Exit Condition
*     None.
*/
//Digital Output function definitions
void UserIO::SetReady(BOOLEAN State)				// Set or clear Ready flag
{
	ReadyFlg = State;
}

/*  void UserIO::SetSonicsActive(BOOLEAN State)
*
*   Purpose
* 		Set or clear Sonics active flag. This function is called by WC State machine
* 		to set the flag if the soncis are currently active.
*
*   Entry condition
* 		State: The value to be assigned to SonicsActive flag.
*
*   Exit condition
*
*/
void UserIO::SetSonicsActive(BOOLEAN State)
{
	if(SonicsDisableFlag == FALSE)
	   SonicsActive = State;
}

/* void UserIO::SetGenAlarm(BOOLEAN State)
*
*  Purpose
*  Set or clear GenAlarm flag. This function is called by WC State machine
* 	If any type of alarm is present in system.
*
*  Entry condition
*	State: The value to be assigned to GenAlarm flag.
*
*   Exit condition
*
*/
void UserIO::SetGenAlarm(BOOLEAN State)
{
	GenAlarm = State;
}

/*  BOOLEAN UserIO::GetAlarmStatus(void)
 *
 *  Purpose:
 *    To check whether the System is in alarm state. This function is called by website module and PrepareDCXStatusByte
 *    The diagnostics rom webpage can only be started if the system is in ready or alarm state.
 *
 *  Entry condition
 *    None.
 *
 *  Exit condition
 *    Return TURE if some alarm is present else return FALSE.
 */
BOOLEAN UserIO::GetAlarmStatus(void)
{
	BOOLEAN AlarmStatus = FALSE;
	if(WebSite::CheckForDiagnosticFlag() == FALSE)
		AlarmStatus =  (GenAlarm || Overloadalarm);
	else
		AlarmStatus = FALSE;

	return AlarmStatus;
}

/*  BOOLEAN UserIO::GetReadyFlag(void)
 *
 *    Purpose:
 *    To check whether the System is in ready state or not.This function is called by website module.
 *    The diagnostics from webpage can only be started if the system is in ready or alarm state.
 *
 *    Entry condition:
 *       None.
 *
 *    Exit condition:
 *    Returns TRUE if system is in ready state else returns FALSE.
 */
BOOLEAN UserIO::GetReadyFlag(void)
{
	 return ReadyFlg;
}

/*  void UserIO::SetStatusOutput(BOOLEAN State)
 *
 *  Purpose:
 *  Set or clear StatusFlag. This function is called by WC State machine.
 *
 *  Entry condition
 *    State: The value to be assigned to StatusFlag.
 *
 *  Exit condition:
 *    None.
 */
void UserIO::SetStatusOutput(BOOLEAN State)
{
   StatusFlag = State;
}

/* void UserIO::SetSeekOut(BOOLEAN State)
*
*  Purpose
*  Set or clear SeekOut flag. This function is called by WC State machine.if the seek or scan process is going on.
*
*  Entry condition
*	State: The value to be assigned to SeekOut flag.
*
*   Exit condition
*
*/
void UserIO::SetSeekOut(BOOLEAN State)
{
   if(SonicsDisableFlag == FALSE)
      SeekOut = State;
}

/* void UserIO::SetCycleOK(BOOLEAN State)
 *
 *  Purpose:
 *  Set or clear CycleOK flag. This function is called by WC State machine.
 *
 *
 *  Entry condition
 *	   State: The value to be assigned to CycleOK flag.
 *
 *  Exit condition:None.
 */
void UserIO::SetCycleOK(BOOLEAN State)
{
	CycleOK = State;
}

/* void UserIO::SetPresetChanged(BOOLEAN State)
*
*  Purpose
*  Set or clear PresetChanged flag. This function is called by WC State machine
*
* 	Entry condition
*	State: The value to be assigned to PresetChanged flag.
*
*  Exit condition: None.
*/
void UserIO::SetPresetChanged(BOOLEAN State)
{
	PresetChanged = State;
}


/* void UserIO::SetHoldTimeActive(BOOLEAN State)
*
*  Purpose
*  Set or clear HoldTimeActive flag. This function is called by WC State machine
*
*
*  Entry condition
*	State: The value to be assigned to HoldTimeActive flag.
*
*   Exit condition
*
*/
void UserIO::SetHoldTimeActive(BOOLEAN State)
{
	HoldTimeActive = State;
}

/* void UserIO::SetAfterBurstDelayActive(BOOLEAN State)
*
*  Purpose
*  Set or clear AfterBurstDelayActive flag. This function is called by WC State machine.
*
*
*  Entry condition
*	State: The value to be assigned to AfterBurstDelayActive flag.
*
*   Exit condition
*
*/
void UserIO::SetAfterBurstDelayActive(BOOLEAN State)
{
	AfterBurstDelayActive = State;
}

/* void UserIO::SetAfterBurstTimeActive(BOOLEAN State)
*
*  Purpose
*  Set or clear AfterBurstTimeActive flag. This function is called by WC State machine
*
*  Entry condition
*	State: The value to be assigned to AfterBurstTimeActive flag.
*
*  Exit condition
*
*/
void UserIO::SetAfterBurstTimeActive(BOOLEAN State)
{
	AfterBurstTimeActive = State;
}

/* void UserIO::SetPlusTimeLimitAlarm(BOOLEAN State)
*
*  Purpose
*  Set or clear PlusTimeLimitAlarm flag. This function is called by WC State machine.
*
*
*  Entry condition
*	State: The value to be assigned to PlusTimeLimitAlarm flag.
*
*   Exit condition
*
*/
void UserIO::SetPlusTimeLimitAlarm(BOOLEAN State)
{
	PlusTimeLimitAlarm = State;
}

/* void UserIO::SetMinusTimeLimitAlarm(BOOLEAN State)
*
*  Purpose
*  Set or clear MinusTimeLimitAlarm flag. This function is called by WC State machine
*
*
*  Entry condition
*	State: The value to be assigned to MinusTimeLimitAlarm flag.
*
*  Exit condition
*
*/
void UserIO::SetMinusTimeLimitAlarm(BOOLEAN State)
{
	MinusTimeLimitAlarm = State;
}

/*  void UserIO::SetPlusEnergyLimitAlarm(BOOLEAN State)
 *
 *   Purpose
 *  	Set or clear PlusEnergyLimitAlarm flag. This function is called by WC State machine.
 *
 *
 *   Entry condition
 *		State: The value to be assigned to PlusEnergyLimitAlarm flag.
 *
 *   Exit condition
 *		None.
 */
void UserIO::SetPlusEnergyLimitAlarm(BOOLEAN State)
{
	PlusEnergyLimitAlarm = State;
}

/*  void UserIO::SetMinusEnergyLimitAlarm(BOOLEAN State)
 *
 *  Purpose
 *  	Set or clear MinusEnergyLimitAlarm flag.
 *
 *  Entry condition
 *		State: The value to be assigned to MinusEnergyLimitAlarm flag.
 *
 *  Exit condition
 *		None.
 */
void UserIO::SetMinusEnergyLimitAlarm(BOOLEAN State)
{
	MinusEnergyLimitAlarm = State;
}

/*  void UserIO::SetPlusPeakPowerLimitAlarm(BOOLEAN State)
 *
 *   Purpose
 *  	Set or clear PlusPeakPowerLimitAlarm flag.
 *
 *
 *   Entry condition
 *		State: The value to be assigned to PlusPeakPowerLimitAlarm flag.
 *
 *   Exit condition
 *		None.
 */
void UserIO::SetPlusPeakPowerLimitAlarm(BOOLEAN State)
{
	PlusPeakPowerLimitAlarm = State;
}

/*  void UserIO::SetMinusPeakPowerLimitAlarm(BOOLEAN State)
 *
 *   Purpose
 *   	Set or clear MinusPeakPowerLimitAlarm flag.
 *
 *   Entry condition
 *		State: The value to be assigned to MinusPeakPowerLimitAlarm flag.
 *
 *   Exit condition
 *		None.
 */
void UserIO::SetMinusPeakPowerLimitAlarm(BOOLEAN State)
{
	MinusPeakPowerLimitAlarm = State;
}

/*  void UserIO::SetOverloadAlarm(BOOLEAN State)
 *
 *   Purpose
 *  	Set or clear OverloadAlarm flag. This function is called by WC State machine
 * 		to set the flag if some overload condition occurs. The reason for overload
 *		is send by DCP via modbus. The flag is cleared when reset is activated from
 *		either LCD-A, website or MI-A external reset input.
 *
 *   Entry condition
 *		State: The value to be assigned to OverloadAlarm flag.
 *
 *   Exit condition
 *		NOne.
 */
void UserIO::SetOverloadAlarm(BOOLEAN State)
{
	Overloadalarm = State;
}

/*  void UserIO::SetActuaterHome(BOOLEAN State)
 *
 *   Purpose
 *      Set or clear ActuatorHome flag.
 *		//discuss
 *
 *   Entry condition
 *		State: The value to be assigned to ActuatorHome flag.
 *
 *   Exit condition
 *		NOne.
 */
void UserIO::SetActuaterHome(BOOLEAN State)
{
	ActuaterHome = State;
}

/*  void  UserIO::SetRfselect(int Sel)
 *
 *   Purpose
 *		To send command to RF switch to select a horn.
 *
 *   Entry condition
 *		Sel: The horn number to be selected on RF Switch.
 *
 *   Exit condition
 *		None.
 */
void  UserIO::SetRfselect(int Sel)
{
	//ASSERT (Sel >= 1 || Sel <= 7);
	//Update the selection flags.
	if(Sysconfig->RFSwitch.RFOutputType == UNCODED)
	   Sel = (UINT8)(BIT((Sel - 1)));
  	Rfbit0select = Sel & BIT(0);
  	Rfbit1select = Sel & BIT(1);
  	Rfbit2select = Sel & BIT(2);
  	Rfbit3select = Sel & BIT(3);
}

/* void UserIO::SetWeldCycleComplete(BOOLEAN State)
*
*  Purpose
*  Set or clear WeldCycleComplete flag. This function is called by WC State machine
* 	to set the flag when weld cycle is completed.
*
*  Entry condition
*	State: The value to be assigned to WeldCycleComplete flag.
*
*   Exit condition
*
*/
void UserIO::SetWeldCycleComplete(BOOLEAN State)
{
	WeldCycleComplete = State;
}

/*  void UserIO::SetPreloadedCycleCountReached(BOOLEAN State)
 *
 *  Purpose
 *   	Set or clear PreloadedCycleCountReached flag.
 *		//discuss
 *
 *  Entry condition
 *		State: The value to be assigned to PreloadedCycleCountReached flag.
 *
 *  Exit condition
 *		None.
 */
void UserIO::SetPreloadedCycleCountReached(BOOLEAN State)
{
	PreloadedCycleCountReached = State;
}

/*  void UserIO::SetInHorndownMode(BOOLEAN State)
 *
 *   Purpose
 *   	Set or clear InHorndownMode flag.
 * 		//Discuss
 *
 *   Entry condition
 *		State: The value to be assigned to InHorndownMode flag.
 *
 *   Exit condition
 *		NOne.
 */
void UserIO::SetInHorndownMode(BOOLEAN State)
{
	InHorndownMode = State;
}

/*  void UserIO::SetEndOfHoldTime(BOOLEAN State)
 *
 *   Purpose
 *   	Set or clear EndOfHoldTime flag.
 *  	///Discuss
 *
 *   Entry condition
 *		State: The value to be assigned to EndOfHoldTime flag.
 *
 *   Exit condition
 *		NOne.
 */
void UserIO::SetEndOfHoldTime(BOOLEAN State)
{
	EndOfHoldTime = State;
}

/*  void UserIO::SetAmplitude1_2(BOOLEAN State)
 *
 *   Purpose
 *   	Set or clear Amplitude1_2  flag. This function is called by WC State machine to set the flag.
 * 	  	If the weld amplitude is set to Amplitude2, clear the flag if weld amplitude is set to Amplitude1.
 *
 *   Entry condition
 *		State: The value to be assigned to Amplitude1_2  flag.
 *
 *   Exit condition
 *		None.
 */
void UserIO::SetAmplitude1_2(BOOLEAN State)
{
      Amplitude1_2 = State;
}

/*  void UserIO::SetStartSignalRelease(BOOLEAN State)
 *
 *  Purpose
 *  	Set or clear StartSignalRelease flag.
 *
 *  Entry condition
 *		State: The value to be assigned to StartSignalRelease flag.
 *
 *  Exit condition
 *  	None
 */
void UserIO::SetStartSignalRelease(BOOLEAN State)
{
	StartSignalRelease = State;
}

//Analog UserIO related func definitions
/*  SINT32 UserIO::GetExternalAmplitude(void)
 *
 *   Purpose
 *  	This function  returns the external amplitude value in raw format.
 *      This function is called by WCTask Run() function.
 *	 	This amplitude is used for weld if the External Amplitude is enabled in preset.
 *
 *   Entry condition
 *		None
 *
 *   Exit condition
 *   	return amplitude value in raw from.
 */
SINT32 UserIO::GetExternalAmplitude(void)
{
	if(AmplitudeIn < RAWVAL_MINIMUM)
		AmplitudeIn = RAWVAL_FIFTYPER;

	return ((AmplitudeIn * 100) / MAX_ADC_RAWVALUE);
}

/*  SINT32 UserIO::GetExternalFrequency(void)
 *
 *   Purpose
 *  	This function returns the frequency offset value set from analog input.
 *		This  function is called every millisecond from WCTask Run() function.
 *  	if the extern frequency offset otion is selected in preset.
 *
 *   Entry condition
 *   	None
 *
 *   Exit condition
 *     	return Frequency value which can be used directly as an offset
 *     to midband
 */
SINT32 UserIO::GetExternalFrequency(void)
{
	SINT32 ExtFreq;
	if(RunningPreset.WcParam.FreqOffsetFlag == OFF)
	   ExtFreq = RunningPreset.WcParam.FreqOffset;
	else{
		if(FreqIn > MAX_ADC_RAWVALUE){//Sanity check for array overflow
			FreqIn = MAX_ADC_RAWVALUE;
		}
		ExtFreq = ExtRawFreqToFreqOffset[FreqIn];
	}
	return ExtFreq;
}

SINT32 UserIO::GetCustomAIn2(void)
{
   return ((CustomInput2 * 100) / MAX_ADC_RAWVALUE);
}

/*  SINT32 UserIO::GetCustomAIn1(void)
 *
 *   Purpose: This function is used to get value of CustomAIn1.
 *
 *   Entry condition
 *		None.
 *
 *   Exit condition
 *   	return encoderIpPosition
 */
SINT32 UserIO::GetCustomAIn1(void)
{
   return ((CustomInput1 * 100) / MAX_ADC_RAWVALUE);
}

/*  void UserIO::SetOutputAmplitude(SINT32 Val)
 *
 *   Purpose
 *  	This function sets the millivolt value for DAC corresponding
 *    to the Output Amplitude values is the
 *     real amplitude value(from instrumentation board)
 *     received via modbus from DCP board.
 *		This function is called from  WC task every millisecond values received from DCP are in the range 0 to 100%
 *		Should be scaled to 0 to 2000 range before passing to this function.
 *
 *   Entry condition
 *   	val : amplitude value.
 *
 *   Exit condition
 *  	None
*/
void UserIO::SetOutputAmplitude(SINT32 Val)
{
	AmplitudeOut = (Val * MAX_DAC_RAWVALUE / SCALING_FACTOR);
}

/*  void UserIO::SetOutputPower(SINT32 Val)
 *
 *   Purpose
 *   	This function sets the millivolt value for DAC corresponding to the output Power.
 *   	This power value is the one received via modbus from DCP board.
 *		This function is called from WC task every millisecond values received from DCP are in the range 0 to 100%
 *      Should be scaled to 0 to 2000 range before passing to this function.
 *
 *   Entry condition
 *		Val : value for power
 *
 *   Exit condition
 *		None
 */
void UserIO::SetOutputPower(SINT32 Val)
{
	PowerOut = (Val* MAX_DAC_RAWVALUE / SCALING_FACTOR);
}

/*  void UserIO::SetOutputFrequency(SINT32 Val)
 *
 *   Purpose
 *  	This function sets the millivolt value for DAC corresponding to the DDSfrequency received from DCP board via
 *  modbus.
 *   	The frequency should be scaled accordingly(in the range 0 to 4000 to get the output 0 to 10 Volt)
 *   before passing to this function.
 *		min freq – 0 Volt  milli volt value 0
 *		midband  - 5 volt milli volt value  1000
 *  	max frequency- 10 volt milli volt value 2000
 *
 *   Entry Condition
 *		Val = output frequency
 *
 *   Exit condition
 *		None.
 */
void UserIO::SetOutputFrequency(SINT32 Val)
{
	Val = Val * FREQ_SCALING_FACTOR;
	Val = ((Val - MinFrequency) * MAX_DAC_RAWVALUE) / (MaxFrequency - MinFrequency);
	if(Val < 0)
		Val = 0;
	else if(Val > MAX_DAC_RAWVALUE)
		Val = MAX_DAC_RAWVALUE;
	FreqOut = Val;
}

/*  void UserIO::SetAutomationOP1(SINT32 Val)
 *
 *   Purpose
 *     This function sets the value of automation output 1
 *   //Discuss
 *
 *   Entry condition
 *		Val:
 *
 *   Exit condition
 *   	None.
 */
void UserIO::SetAutomationOP1(SINT32 Val)
{
	AutomationOP1 = Val;
}

/*  SINT32 UserIO:: GetDigINPutValue(DIGITAL_INPUTS DigIn)
 *
 *  Purpose
 *  	This function is used to get the logic at digital input passed in parameter digIn.
 *    This function is called from website module during diagnostic of IOs.
 *
 *  Entry condition
 *  	DigIn: The Input pin whose active status needs to be determined.
 *
 *  Exit condition
 *  	returns -1 if no function is assigned to the digital input else return the logic at digital input.
 */
SINT32 UserIO::GetDigINPutValue(DIGITAL_INPUTS DigIn)
{
   SINT32 Ret = IOPINUNASSIGNEDORDEACTIVEATED;
   ASSERT(DigIn <= DIGIN9);
   if((USERIOConfig->DigInUse[DigIn] > 0) && (USERIOConfig->DigInUseStatus[DigIn] == ACTIVATED)){
		if((*(InValue[DigIn])) == TRUE)
			Ret = 1;
		else
			Ret = 0;
   }
   return Ret;
}

/*  SINT32 UserIO::GetDigOutPutValue(DIGITAL_OUTPUTS DigOut)
 *
 *  Purpose
 *    This function is used to get the logic at digital output passed in parameter DigOut.
 *    This function is called from website module during diagnostic of IOs.
 *
 *  Entry condition
 *    DigOut: The output pin whose active status needs to be determined
 *
 *  Exit condition
 *    -1 unassigned 0 inactive 1 active
 */
SINT32 UserIO::GetDigOutPutValue(DIGITAL_OUTPUTS DigOut)
{
	SINT32 Ret = IOPINUNASSIGNEDORDEACTIVEATED;
	ASSERT(DigOut <= DIGOUT8);
	if((USERIOConfig->DigOutUse[DigOut] > 0) && (USERIOConfig->DigOutUseStatus[DigOut] == ACTIVATED)){
		if((*(OutValue[DigOut])) == TRUE)
			Ret = 1;
		else
			Ret = 0;
	}
	return Ret;
}

/*  void UserIO::SetDigOutPutValue(DIGITAL_OUTPUTS DigOut, BOOLEAN Val)
 *
 *   Purpose
 *   	To drive the digital output pin with logic give in param Val
 *   	This function is called from the website module during diagnostic of IOs from webpage
 *
 *   Entry condition
 *   	DigOut- The digital output number
 *   	Val- The logic to be driven on output
 *
 *   Exit condition
 * 	 	None.
 */
void UserIO::SetDigOutPutValue(DIGITAL_OUTPUTS DigOut, BOOLEAN Val)
{
	ASSERT (DigOut <= DIGOUT8);
	if((USERIOConfig->DigOutUse[DigOut] > 0) && (USERIOConfig->DigOutUseStatus[DigOut] == ACTIVATED))
		* (OutValue[DigOut]) = Val;
}

/*  SINT32 UserIO::GetAnalogINPutValue(ANALOG_INPUTS AIn)
 *
 *   Purpose
 *   	This function return the current value of the specified analog input given in param aIn.
 *   	This function is called from the website module during diagnostic of IOs from webpage.
 *
 *   Entry condition
 *   	AIn: The analog input number.
 *
 *   Exit condition
 *   	returns -1 if no function is assigned at the pin or the function is not activated
 *	 	else return the current value of input.
 */
SINT32 UserIO::GetAnalogINPutValue(ANALOG_INPUTS AIn)
{
	SINT32 Ret = IOPINUNASSIGNEDORDEACTIVEATED;
	ASSERT (AIn <= AIN2);
	if((USERIOConfig->AnalogInUse[AIn] > 0) && (USERIOConfig->AnalogInUseStatus[AIn] == ACTIVATED))
		Ret = *(AnalogInValue[AIn]);

	return Ret;
}

/*  SINT32 UserIO::GetAnalogOUTPutValue(ANALOG_OUTPUTS AOut)
 *
 *   Purpose
 *  	This function return the current value of the specified analog output given in param aOut.
 *  	This function is called from the website module during diagnostic of IOs from webpage
 *
 *   Entry condition
 *   	AOut  :The analog output number.
 *
 *   Exit condition
 *  	returns -1 if no function is assigned at the pin or the function is not activated
 *		else return the current value beign driven at output.
 */
SINT32 UserIO::GetAnalogOUTPutValue(ANALOG_OUTPUTS AOut)
{
	SINT32 Ret = IOPINUNASSIGNEDORDEACTIVEATED;
	ASSERT(AOut <= AOUT2);
	if((USERIOConfig->AnalogOutUse[AOut] > 0) && (USERIOConfig->AnalogOutUseStatus[AOut] == ACTIVATED))
		Ret = *(AnalogOutValue[AOut]);

	return Ret;
}

/*  void UserIO::SetAnalogOUTPutValue(ANALOG_OUTPUTS AOut, SINT32 Val)
 *
 *   Purpose
 *   	This function sets the analog output value to the specified analog output in param aOut.
 *      This function is called from the website module during diagnostic of IOs from webpage.
 *
 *   Entry condition
 *     	AOut : The analog output number.
 *     	Val  : The signal value we want to drive through DAC chip.
 *
 *   Exit condition
 *		None.
 */
void UserIO::SetAnalogOUTPutValue(ANALOG_OUTPUTS AOut, SINT32 Val)
{
    ASSERT(AOut <=  AOUT2);
	if((USERIOConfig->AnalogOutUse[AOut] > 0) && (USERIOConfig->AnalogOutUseStatus[AOut] == ACTIVATED))
		* (AnalogOutValue[AOut]) = Val;
}

/*  void UserIO::GetDigitalinputActivityStatus(SINT32 FuncId)
 *
 *  Purpose
 *     This function is used to get the Activity Status of Digital Input. This function is called from PrepareDiagnosticResponse() function.
 *
 *  Entry condition
 *     FuncId: Digital Input ID.
 *
 *  Exit condition: Activity Status of Digital Input.
 */
SINT32 UserIO::GetDigitalinputActivityStatus(SINT32 FuncId)
{
	return InPtr[FuncId].Active;
}

/*  void UserIO::GetDigitaloutputActivityStatus(SINT32 FuncId)
 *
 *  Purpose
 *     This function is used to get the Activity Status of Digital Output. This function is called from PrepareDiagnosticResponse() function.
 *
 *  Entry condition
 *     FuncId: Digital Output ID.
 *
 *  Exit condition: Activity Status of Digital Output.
 */
SINT32 UserIO::GetDigitaloutputActivityStatus(SINT32 FuncId)
{
   return  OutPtr[FuncId].Active;
}

/*  void UserIO::FanEnable(BOOLEAN State)
 *
 *   Purpose
 *   Set or clear FanStatusFlag. This function form CycleRun() of WCTask and form Website whenever FAN ON/OFF command is issued from Diagnostic UserIO Webpage.
 *
 *  Entry condition
 *     State: The value to be assigned to FanStatusFlag.
 *
 *   Exit condition:None.
 */
void UserIO::FanEnable(BOOLEAN State){
	UINT8 OutData = DIGITAL_IO_FANANDLEDADDR;
	if(State == TRUE)
		OutData &= ~(OutBitsPosFANamdLED[FANEN] );//| OutBitsPosFANamdLED[REDLED] | OutBitsPosFANamdLED[GREENLED]);
	else
		OutData |= (OutBitsPosFANamdLED[FANEN] );
//	if(AdvanceConfigData.Advrnd.RevDigitalOP == TRUE)
//		OutData = ~OutData;
	DIGITAL_IO_FANANDLEDADDR = OutData;
	FanStatusFlag = State;
}

/*  void UserIO::SetCustomAlarmOutput(BOOLEAN State)
 *
 *  Purpose
 *   Set or clear CustomAlarmOutput flag. This function is called by AlarmQueueWrite() function if the CUSTOM_ALARM_OUTPUT Bit for that Alarm is SET by User.
 *   CustomAlarmOutput flag is cleared as Alarm is Cleared External reset.
 *
 *  Entry condition
 *   State: The value to be assigned to CustomAlarmOutput flag.
 *
 *  Exit condition:None.
 */
void UserIO::SetCustomAlarmOutput(BOOLEAN State)
{
   CustomAlarmOutput = State;
}



/*  void ClearAlarmOutputs(void)
 *
 *  Purpose
 *   Set or clear CustomAlarmOutput flag. This function is called by AlarmQueueWrite() function if the CUSTOM_ALARM_OUTPUT Bit for that Alarm is SET by User.
 *   CustomAlarmOutput flag is cleared as Alarm is Cleared External reset.
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
void ClearAlarmOutputs(void)
{
	UserIO::SetPlusTimeLimitAlarm(FALSE);
	UserIO::SetMinusTimeLimitAlarm(FALSE);
	UserIO::SetPlusPeakPowerLimitAlarm(FALSE);
	UserIO::SetMinusPeakPowerLimitAlarm(FALSE);
	UserIO::SetPlusEnergyLimitAlarm(FALSE);
	UserIO::SetMinusEnergyLimitAlarm(FALSE);
	UserIO::SetCustomAlarmOutput(FALSE);
	UserIO::SetOverloadAlarm(FALSE);
	UserIO::SetGenAlarm(FALSE);
	UserIO::SetStatusOutput(FALSE);
}


