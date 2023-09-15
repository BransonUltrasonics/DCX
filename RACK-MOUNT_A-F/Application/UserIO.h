/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/UserIO.h_v   1.2   24 Jul 2015 08:38:46   sesharma  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/UserIO.h_v  $
 * 
 *    Rev 1.2   24 Jul 2015 08:38:46   sesharma
 * Added CTART register defines for ADC and DAC.
 * 
 *    Rev 1.1   24 Jun 2015 08:38:40   amaurya
 * Removed AmplitudeDecay
 * 
 *    Rev 1.0   09 Jun 2015 09:10:46   tayars
 * Initial revision.
 * 
 *    Rev 1.28   08 Oct 2013 06:17:30   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.27   24 Aug 2013 17:14:14   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.26   08 Jul 2013 08:56:00   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.25   24 May 2013 10:16:54   ygupta
 * Fixed Tracker Issue.
 * 
 *    Rev 1.24   21 May 2013 12:26:02   amaurya
 * Code review fixes.
 * 
 *    Rev 1.23   06 May 2013 09:20:42   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.22   30 Apr 2013 11:22:22   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.21   22 Apr 2013 11:32:46   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.20   15 Mar 2013 10:53:36   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.19   18 Feb 2013 03:11:48   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.18   29 Jan 2013 02:24:20   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.17   17 Jan 2013 07:26:46   ygupta
 * Profibus Related changes
 * 
 *    Rev 1.16   11 Jan 2013 02:55:28   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.15   27 Dec 2012 06:46:10   ygupta
 * Changes to fix Issues
 * 
 *    Rev 1.14   18 Dec 2012 07:39:24   ygupta
 * Changes for Multiple Overload alarm in case of 15V lost
 * 
 *    Rev 1.12   13 Dec 2012 00:13:26   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.11   29 Nov 2012 08:48:58   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.10   24 Nov 2012 08:05:56   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.9   08 Nov 2012 01:22:04   rjamloki
 * Website changes
 * 
 *    Rev 1.8   29 Oct 2012 02:56:50   rjamloki
 * Printfs disbaled and changes in website
 * 
 *    Rev 1.7   26 Oct 2012 02:41:04   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.5   21 Sep 2012 01:34:54   rjamloki
 * Added State machine
 * 
 *    Rev 1.4   11 Sep 2012 10:12:30   rjamloki
 * User IO update and clean up
 * 
 *    Rev 1.3   29 Aug 2012 13:28:12   rjamloki
 * WC Modbus checkin and related changes
 * 
 *    Rev 1.2   05 Jul 2012 04:47:56   ygupta
 * Cleanup after review summary on database and BSP.
 * 
 *    Rev 1.1   02 Jul 2012 13:04:14   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.0   21 Jun 2012 12:38:30   ygupta
 * Initial revision.
 *
 */
/******************************************************************************/
//This module is the part of DCP Application code
//This module gives the interface to read and write digital and analog
//signals between DCP and MI board. The DCP state machine should use the
//function of this class (which are all static) to determine the UserIO
//status.In all comments in this module input signal means the signal coming
//from MI board to DCP board. The output signal means the signal going from
//DCP board to MI board.
#ifndef  USERIONEW_H_
#define  USERIONEW_H_

#include "MCP3208.h"
#include "TLV5604.h"
#include "TYP_DEF.h"


#define	NUM_DIN	  	 9 + 1 // Total available digital inputs on
                            //MI-A board. Plus 1 to facilitate driver design
#define	NUM_FUNC_IN	 53 //# of input functions defined in PRS


//Digital inputs bit position in the 16 bit value read from
//memory mapped IO base address on 8 bit wide data bus
#define	DIN1_BIT	  BIT(7)	     // Digital_in_1
#define	DIN2_BIT	  BIT(6)	     // Digital_in_2
#define	DIN3_BIT	  BIT(5)	     // Digital_in_3
#define	DIN4_BIT	  BIT(4)	     // Digital_in_4
#define  DIN5_BIT   BIT(3)		  // Digital_in_5
#define  DIN6_BIT   BIT(2)		  // Digital_in_6
#define  DIN7_BIT   BIT(1)		  // Digital_in_7
#define  DIN8_BIT   BIT(0)		  // Digital_in_8
#define  DIN9_BIT   BIT(15)	  // Digital_in_9

//defines for creating mask to be applied on digital inputs
//after reading the actual hardware signals.
#define	DIN1_LO	DIN1_BIT
#define	DIN1_HI	0
#define	DIN2_LO	DIN2_BIT
#define	DIN2_HI	0
#define	DIN3_LO	DIN3_BIT
#define	DIN3_HI	0
#define	DIN4_LO	DIN4_BIT
#define	DIN4_HI	0
#define  DIN5_LO 	DIN5_BIT
#define  DIN5_HI 	0
#define  DIN6_LO 	DIN6_BIT
#define  DIN6_HI 	0
#define  DIN7_LO 	DIN7_BIT
#define  DIN7_HI 	0
#define  DIN8_LO 	DIN8_BIT
#define  DIN8_HI 	0
#define  DIN9_LO 	DIN9_BIT
#define  DIN9_HI 	0

#define	NUM_DOUT	   8 + 1	 //Total available digital outputs on MI-A
							 //board.Plus 1 to facilitate IO driver design
#define	NUM_FUNC_OUT  53	//55	 // # of output functions defined in PRS

//Digital outputs bit position in the 8 bit value
//written to memory mapped IO base address on
#define	DOUT1_BIT	  BIT(7)		// Digital_out_1
#define	DOUT2_BIT	  BIT(6)		// Digital_out_2
#define	DOUT3_BIT	  BIT(5)		// Digital_out_3
#define	DOUT4_BIT	  BIT(4)		// Digital_out_4
#define  DOUT5_BIT     BIT(3)		// Digital_out_5
#define  DOUT6_BIT     BIT(2)		// Digital_out_6
#define  DOUT7_BIT     BIT(1)		// Digital_out_7
#define  DOUT8_BIT     BIT(0)		// Digital_out_8

//defines for creating mask to be applied on the BOOLAEN
//flags representing the out signal values, before writing on digital output pins.
#define	DOUT1_LO	    DOUT1_BIT
#define	DOUT1_HI	    0
#define	DOUT2_LO	    DOUT2_BIT
#define	DOUT2_HI	    0
#define	DOUT3_LO	    DOUT3_BIT
#define	DOUT3_HI	    0
#define	DOUT4_LO	    DOUT4_BIT
#define	DOUT4_HI	    0
#define  DOUT5_LO	    DOUT5_BIT
#define  DOUT5_HI     0
#define  DOUT6_LO	    DOUT6_BIT
#define  DOUT6_HI 	 0
#define  DOUT7_LO	    DOUT7_BIT
#define  DOUT7_HI 	 0
#define  DOUT8_LO	    DOUT8_BIT
#define  DOUT8_HI 	 0

#define  NUM_AIN      2 + 1   // Total available analog input pins on MI-A  board plus one to facilitate driver design
#define  NUM_FUNC_AIN 4       // # of input functions defined in PRS including automation inputs

//Max 9 digital inputs. Fix for 16 bit read from base address
#define DIGITAL_IO_READADDR (*((volatile UINT16*) MI_DIGITALIO_BASEADDRESS))
//Max 8 digital outputs. Fix for 8 bit write on base address
#define DIGITAL_IO_WRITEADDR (*((volatile UINT8*) MI_DIGITALIO_BASEADDRESS + 1))

//Fan and LED
#define DIGITAL_IO_FANANDLEDADDR (*((volatile UINT8*) MI_DIGITALIO_BASEADDRESS))

#define AIN1_CHANNEL  AdcInputCh0   //Analog Input 1 is channel_0 on ADC chip of MI-A board(Confirm??)
#define AIN2_CHANNEL  AdcInputCh1   //Analog Input 2 is channel_1 on ADC chip of MI-A board(Confirm??)

#define NUM_AOUT        2 + 1    // Total available analog outputs pins on MI-A board plus one to facilitate driver design
#define NUM_FUNC_AOUT   5        // # of output functions defined in PRS(including automation analog inputs).

#define AOUT1_CHANNEL DacOutputA //Analog Output 1 is channel_A on DAC chip of MI-A board(Confirm??)
#define AOUT2_CHANNEL DacOutputB //Analog Output 2 is channel_B on DAC chip of MI-A board(Confirm??)

//Maximum raw value from ADC
#define MAX_ADC_RAWVALUE   2047

//Maximum value of DAC output in raw to get the maximum required value of 10 Volt of analog outputs for the product.
//Note- The DAC output channels are connected to OP AMP of gain 2.5.
// WC code shall scale the output analog values based on this define.
#define MAX_DAC_RAWVALUE   818

//Half of the max raw value which comes from ADC represent offset 0
#define EXT_FREQ_SCALINGFACTOR   1024

//define used in frequency offset calculation
#define FREQ_OFFSET_LIMIT     1000
//Reference voltages in milivolt for ADC and DAC chips in MI-A bord
#define REFVOLTAGE_DAC    1250
#define REFVOLTAGE_ADC    2500

//Percents of maximum raw values from ADC. They are needed to check the minimum limit required in Analog input signals to avoid noise(max raw val from ADC channel is 2047)
#define RAWVAL_TENPER     (MAX_ADC_RAWVALUE / 10)
#define RAWVAL_MINIMUM    RAWVAL_TENPER   //to check frequency in and amplitude in minimum limit

//Default fifty percent if minimum limit of analog inputs is not there
#define RAWVAL_FIFTYPER   (MAX_ADC_RAWVALUE / 2)

//max last ADC values used for software filtering of analog signals
#define MAXADCVALUES_FILTER  3

//Number of times the digital input should return the same value before the logic change(1 to 0 or 0 to 1) on it is considered valid.
#define DEBOUNCE_TIME   5

//defines for the DSPI chip select pin numbers on CPU for the ADC and DAC device in MI-A board
#define DSPICSPIN_DAC   1
#define DSPICSPIN_ADC   2

// defines for DSPI control and transfer attribute register number to be used for ADC and DAC devices
#define DSPICTAR_DAC   1
#define DSPICTAR_ADC   2

//The define for return value of signals at digital/analog input or output pin in case the same is not assigned any function
//or the same is deactivated from webpage. Used in the functions called by website during diagnostic of IOs while sending signal
//values at a given input or output pin.
#define IOPINUNASSIGNEDORDEACTIVEATED -1

#define FIRST_DIGITAL_INPUT 	1
#define LAST_DIGITAL_INPUT		9

#define FIRST_DIGITAL_OUTPUT 	1
#define LAST_DIGITAL_OUTPUT		8

#define FIRST_ANALOG_INPUT 		1
#define LAST_ANALOG_INPUT		2

#define FIRST_ANALOG_OUTPUT 	1
#define LAST_ANALOG_OUTPUT		2

#define LEDSTATE 				3

enum{
	LEDGREEN = 5,
	LEDRED,
	LEDYELLOW
};
//digital signal configuration
enum ACTIVE {LOW, HIGH};

// These enums are used to determine which I/O functions are available for which product configurations.
enum CONTROL
{
	 DCXS = BIT(0),
	 DCXV = BIT(1),
	 DCXA = BIT(2),
	 DCXF = BIT(3),
	 ALL  = (DCXS | DCXV | DCXA | DCXF),
	 DCXAANDF = (DCXA | DCXF),
	 NO_CONFIG = 0,
};

enum DIGOP
{
	FANEN,
	REDLED,
	GREENLED,
};

enum DEFAULTTYPES{
   BRANSONDEFAULTS = 1,
   ACTUATORDEFAULTS,
   RFSWITCHINGDEFAULTS,
   CUSTOMDEFAULTS
};

//digital IO map
typedef struct {
   //Pointer to flag in application code representing one of he available digital IO functions
   BOOLEAN * Flag;
	enum	ACTIVE	Active;//active high or active low.
	enum	CONTROL	Control;//to check the function is suppported or not
}DIGITALIOMAP;

//available digital inputs
enum DIGITAL_INPUTS {DIGIN1 = 1, DIGIN2, DIGIN3, DIGIN4, DIGIN5, DIGIN6, DIGIN7, DIGIN8, DIGIN9};

//available digital output
enum DIGITAL_OUTPUTS {DIGOUT1 = 1, DIGOUT2, DIGOUT3, DIGOUT4, DIGOUT5, DIGOUT6, DIGOUT7, DIGOUT8};

//available analog inputs
enum ANALOG_INPUTS{AIN1 = 1, AIN2};

//available analog outputs
enum ANALOG_OUTPUTS{AOUT1 = 1, AOUT2};


/*Analog IOs */
typedef struct {
	SINT32 * Value; //pointer to variable in application code representing one of he available analog IO functions
	enum  CONTROL Control; //to check the function is suppported or not
}ANALOGIOMAP;

//function numbers for digital inputs defined in PRS are represented by this enum.
enum USERDEFINED_DIGINPUTFUNC
{
	INPUTPIN_DISABLED = 0,
	EXTRESET_ACTIVEHIGH,
	EXTRESET_ACTIVELOW,
	EXTSEEK_ACTIVEHIGH,
	EXTSEEK_ACTIVELOW,
	EXTSTART_ACTIVEHIGH,
	EXTSTART_ACTIVELOW,
	EXTTEST_ACTIVEHIGH,
	EXTTEST_ACTIVELOW,
	MEMCLEAR_ACTIVEHIGH,
	MEMCLEAR_ACTIVELOW,
	STARTHORNSCAN_ACTIVEHIGH,
	STARTHORNSCAN_ACTIVELOW,
	RECALLPRESET_1_ACTIVEHIGH,
	RECALLPRESET_2_ACTIVEHIGH,
	RECALLPRESET_4_ACTIVEHIGH,
	RECALLPRESET_8_ACTIVEHIGH,
	RECALLPRESET_16_ACTIVEHIGH,
	RECALLPRESET_32_ACTIVEHIGH,
	EXTSONICDELAY_ACTIVEHIGH,
	EXTSONICDELAY_ACTIVELOW,
	LOCKOUT_ACTIVEHIGH,
	LOCKOUT_ACTIVELOW,
	SONICDISABLE_ACTIVEHIGH,
	SONICDISABLE_ACTIVELOW,
	INTERLOCKINPLACE_ACTIVEHIGH,
	INTERLOCKINPLACE_ACTIVELOW,
	LOADNEWPRESET_ACTIVEHIGH,
	LOADNEWPRESET_ACTIVELOW,
	CYCLEABORT_ACTIVEHIGH,
	CYCLEABORT_ACTIVELOW,
	EXTAMPL_STEPTRIGG_ACTIVEHIGH,
	EXTAMPL_STEPTRIGG_ACTIVELOW,
	GOINTOHORNDOWN_ACTIVEHIGH,
	GOINTOHORNDOWN_ACTIVELOW,
	CABLEDETECT_ACTIVEHIGH,
	RFSTATUSFEEDBACK_ACTIVEHIGH,
	RFSTATUSFEEDBACK_ACTIVELOW,
	RF_BITZERO_FEEDBACK_ACTIVEHIGH,
	RF_BITONE_FEEDBACK_ACTIVEHIGH,
	RF_BITTWO_FEEDBACK_ACTIVEHIGH,
	RF_BITTHREE_FEEDBACK_ACTIVEHIGH,
	// Automation Inputs
	PARTINPLCAE_ACTIVEHIGH,
	PARTINPLACE_ACTIVELOW,
	STARTCYCLE_ACTIVEHIGH,
	STARTCYCLE_ACTIVELOW,
	UPPERLIMITSWITCH_ACTIVEHIGH,
	UPPERLIMITSWITCH_ACTIVELOW,
	GROUNDDETECT_ACTIVEHIGH,
	GROUNDDETECT_ACTIVELOW,
	ACTUATORPRESENT_ACTIVEHIGH,
	ACTUATORPRESENT_ACTIVELOW,
	TRIGGERSWITCH_ACTIVEHIGH,
	TRIGGERSWITCH_ACTIVELOW,
	USERDEFINED_DIGINPUTFUNCEND
};

//function numbers for digital outputs defined in PRS are represented by this enum
enum USERDEFINED_DIGOUTPUTFUNC
{
	OUTPUTPIN_DISABLED=0,
	READY_ACTIVEHIGH,
	READY_ACTIVELOW,
	SONICACTIVE_ACTIVEHIGH,
	SONICACTIVE_ACTIVELOW,
	GENALARM_ACTIVEHIGH,
	GENALARM_ACTIVELOW,
	OVERLOADALARM_ACTIVEHIGH,
	OVERLOADALARM_ACTIVELOW,
	SEEKOUT_ACTIVEHIGH,
	SEEKOUT_ACTIVELOW,
	HOLDTIME_ACTIVEHIGH,
	HOLDTIME_ACTIVELOW,
	AFTERBUST_DELAY_ACTIVEHIGH,
	AFTERBUST_DELAY_ACTIVELOW,
	AFTERBUST_TIME_ACTIVEHIGH,
	AFTERBUST_TIME_ACTIVELOW,
	PLUSTIME_LIMITALARM_ACTIVEHIGH,
	PLUSTIME_LIMITALARM_ACTIVELOW,
	MINUSTIME_LIMITALARM_ACTIVEHIGH,
	MINUSTIME_LIMITALARM_ACTIVELOW,
	PLUSENERGY_LIMITALARM_ACTIVEHIGH,
	PLUSENERGY_LIMITALARM_ACTIVELOW,
	MINUSENERGY_LIMITALARM_ACTIVEHIGH,
	MINUSENERGY_LIMITALARM_ACTIVELOW,
	PLUSPKPOWER_LIMITALARM_ACTIVEHIGH,
	PLUSPKPOWER_LIMITALARM_ACTIVELOW,
	MINUSPKPOWER_LIMITALARM_ACTIVEHIGH,
	MINUSPKPOWER_LIMITALARM_ACTIVELOW,
	STATUS_ACTIVEHIGH,
	CONFIRM_PRESETCHANGE_ACTIVEHIGH,
	ACTUATORHOME_ACTIVEHIGH,
	ACTUATORHOME_ACTIVELOW,
	WELDCYCLE_COMPLETE_ACTIVEHIGH,
	WELDCYCLE_COMPLETE_ACTIVELOW,
	PRELOADED_CYCCOUNT_REACHED_ACTIVEHIGH,
	PRELOADED_CYCCOUNT_REACHED_ACTIVELOW,
	INHORNDOWN_ACTIVEHIGH,
	INHORNDOWN_ACTIVELOW,
	ENDOFHOLDTIME_ACTIVEHIGH,
	ENDOFHOLDTIME_ACTIVELOW,
	AMPL1_AMPL2,
	STARTSIGNAL_RELEASE_ACTIVEHIGH,
	STARTSIGNAL_RELEASE_ACTIVELOW,
	CYCLECOKAY_ACTIVEHIGH,
	CYCLECOKAY_ACTIVELOW,
	RF_BITZERO_SELECT_ACTIVEHIGH,
	RF_BITONE_SELECT_ACTIVEHIGH,
	RF_BITTWO_SELECT_ACTIVEHIGH,
	RF_BITTHREE_SELECT_ACTIVEHIGH,
	//Automation outputs
	CYCLESTARTOUT_ACTIVEHIGH,
	CYCLESTARTOUT_ACTIVELOW,
	//CustomAlarmOutput
	CUSTOM_ALARMOUTPUT_ACTIVEHIGH,
	CUSTOM_ALARMOUTPUT_ACTIVELOW,
	USERDEFINED_DIGOUTPUTFUNCEND
};

//function numbers for analog inputs
enum USERDEFINED_ANALOGINPUTFUNC
{
	INPUT_DISABLED = 0,
	FREQ_CONTROL,
	AMPL_CONTROL,
	//Add automation inputs
	CUSTOM_INPUT1,
	CUSTOM_INPUT2,
	USERDEFINED_ANALOGINPUTFUNCEND
};

//function numbers for analog outputs
enum USERDEFINED_ANALOGOUTPUTFUNC
{
	OUTPUT_DISABLED = 0,
	POWER_OUT,
	AMPL_OUT,
	MEMFREQ_OUT,//unused
	FREQ_OUT,
	//Add automation outputs
	AUTOMATION_OP1,
	USERDEFINED_ANALOGOUTPUTFUNCEND
};

//an already assigned function status
enum STATUS
{
	DEACTIVATED,
	ACTIVATED
};

//The information below goes into FRAM
struct USERIOConfiguration
{
   //XOR mask for digital inputs
   //The mask ensures that the read back value
   //of a digital input if active will always go TRUE in the BOOLEAN flags in the Application code representing the signal at that particular digital input, regardless
   //of the finction at that pin is configured as active HIGH or active LOW from IO configuration web page.
	UINT16 InputMask;

	//keeps configured digital input function number.One of the values from USERDEFINED_DIGINPUTFUNC enum
	//if zero then no function is assigned
	//e.g if DigInUse[DIGIN1]  == EXTRESET_ACTIVELOW
	//that means digital input pin 1 is configured for external reset
   //function and activated when the pin goes low
	UINT16 DigInUse[NUM_DIN];

	//keep track whether function is active or inactive.One of the values from enum STATUS.
   //E.g if DigInUseStatus[DIGIN1] == DEACTIVATED then what ever function assigned on digital input pin1 shall
	//be ignored
	BOOLEAN DigInUseStatus[NUM_DIN];

	//XOR mask for digital outputs
   //Output flag values represnting a function (e.g READY_ACTIVEHIGH or READY_ACTIVELOW)
   //are always TRUE if the signal is active and always false if the signal is inactive from the Application point of view.
   //Mask ensures the written logic on the outputs pins is either LOW or HIGH depending on the assigned function.
	UINT16 OutputMask;

	//Keeps configured digital output function number. One of the vaues from enum USERDEFINED_DIGOUTPUTFUNC. Zero if no function
	//is assigned.
	UINT16 DigOutUse[NUM_DOUT];

	//to keep track whether function is active or inactive.One of the values from enum STATUS.
	BOOLEAN DigOutUseStatus[NUM_DOUT];

	//keeps configured analog input function number.One of  the values from enum USERDEFINED_ANALOGINPUTFUNC.
	//zero if no function is assigned
	UINT16 AnalogInUse[NUM_AIN];

	//to keep track whether analog function is active or inactive. One of the values from enum STATUS.
	BOOLEAN AnalogInUseStatus[NUM_AIN];

	//keeps configured analog output function number.One of the values from enum USERDEFINED_ANALOGOUTPUTFUNC.
	//zero if no function is assigned.
	UINT16 AnalogOutUse[NUM_AOUT];

	//to keep track whether analog output function is active or inactive. On of the values from enum STATUS.
	BOOLEAN AnalogOutUseStatus[NUM_AOUT];

	UINT16 UserIOCRC;
};


//This class defines a interface to communicate with MI-A board.
class UserIO
{
   //functions private to this module
   static SINT32 ReadDigitalIO(void);
   static void WriteDigitalIO(SINT32 Val);
   static void ConfigureUserIOsFromFRAM(void);
   static void ClearFRAMIOConfig(void);
   static UINT16 InputMask;
   static UINT16 OutputMask;
   static BOOLEAN DigInUseStatus[NUM_DIN];
   static BOOLEAN DigOutUseStatus[NUM_DOUT];
   static BOOLEAN AnalogInUseStatus[NUM_AIN];
   static BOOLEAN AnalogOutUseStatus[NUM_AOUT];
   static void InitCableDetectEnabledFlag(void);
   static SINT32 DigFilterMIAAin(SINT32 Element, ANALOG_INPUTS InputNumber);
	public://functions accessible by other modules.
	   static BOOLEAN CheckSignalActive(BOOLEAN Signal);
		static void ResetUserIO(void);
		static void AssignDigitalOutputFunction(enum DIGITAL_OUTPUTS Output, UINT16 FuncNum, BOOLEAN Status);
		static void AssignDigitalInputFunction(enum DIGITAL_INPUTS Input, UINT16 FuncNum, BOOLEAN Status);
		static void AssignAnalogOutputFunction(enum ANALOG_OUTPUTS Output, UINT16 FuncNum, BOOLEAN Status);
		static void AssignAnalogInputFunction(enum ANALOG_INPUTS Input, UINT16 FuncNum, BOOLEAN Status);
		static void ReadInputs(void);
		static void WriteOutputs(void);
		static void InitUserIO(void);
		static void ConfigureBransonDefaultUserIOs(void);
		static void ConfigureActuatorDefaultUserIOs(void);
		static void ConfigureRFDefaultUserIOs(void);
		static void ConfigureCustomDefaultUserIOs(void);
		static TLV5604 * DAC;
		static MCP3208 * ADC;
		//Digital IO functions
		//Input functions
		static BOOLEAN CheckForExternalAmpStep(void);
		static SINT32  CheckForExtRecallPreset(void);
		static BOOLEAN CheckForExternalStart(void);
		static BOOLEAN CheckForExternalSonicsDelay(void);
		static BOOLEAN CheckForLockOutChange(void);
		static BOOLEAN CheckForSonicsDisable(void);
		static BOOLEAN CheckForInterLockInPlace(void);
		static BOOLEAN CheckForLoadNewPreset(void);
		static BOOLEAN CheckForCycleAbort(void);
		static BOOLEAN CheckForMemoryClear(void);
		static BOOLEAN CheckForHornScan(void);
		static BOOLEAN CheckForExternalTest(void);
		static BOOLEAN CheckForHornDownMode(void);
		static BOOLEAN CheckForCableDetect(void);
		static BOOLEAN CheckForCableDetectEanbled(void);
		static BOOLEAN CheckForRfStatusfeedback(void);
		static SINT32  CheckForRfswitchfeedback(void);
		static BOOLEAN CheckForPartInPlace(void);
		static BOOLEAN CheckForStartCycle(void);
		static BOOLEAN CheckForGroundDetect(void);
		static BOOLEAN CheckForULS(void);
		static BOOLEAN CheckForActuatorPresent(void);
		static BOOLEAN CheckForTrigger(void);
		//Output functions
		static void SetReady(BOOLEAN State);
		static void SetSonicsActive(BOOLEAN State);
		static void SetGenAlarm(BOOLEAN State);
		static void SetSeekOut(BOOLEAN State);
		static void SetStatusOutput(BOOLEAN State);
		static void SetCycleOK(BOOLEAN State);
		static void SetPresetChanged(BOOLEAN State);
		static void SetHoldTimeActive(BOOLEAN State);
		static void SetAfterBurstDelayActive(BOOLEAN State);
		static void SetAfterBurstTimeActive(BOOLEAN State);
		static void SetPlusTimeLimitAlarm(BOOLEAN State);
		static void SetMinusTimeLimitAlarm(BOOLEAN State);
		static void SetPlusEnergyLimitAlarm(BOOLEAN State);
		static void SetMinusEnergyLimitAlarm(BOOLEAN State);
		static void SetPlusPeakPowerLimitAlarm(BOOLEAN State);
		static void SetMinusPeakPowerLimitAlarm(BOOLEAN State);
		static void SetOverloadAlarm(BOOLEAN State);
		static void SetActuaterHome(BOOLEAN State);
		static void SetWeldCycleComplete(BOOLEAN State);
		static void SetPreloadedCycleCountReached(BOOLEAN State);
		static void SetInHorndownMode(BOOLEAN State);
		static void SetEndOfHoldTime(BOOLEAN State);
		static void SetAmplitude1_2(BOOLEAN State);
		static void SetStartSignalRelease(BOOLEAN State);
		static void SetCableDetectEnabledFlag(BOOLEAN state);
		static void SetCycleStartOut(BOOLEAN State);
		static void SetCustomAlarmOutput(BOOLEAN State);
		static void SetRfselect(int Sel);
		//Analog IO functions
		//Input functions
		static SINT32 GetExternalAmplitude(void);
		static SINT32 GetExternalFrequency(void);
		static SINT32 GetCustomAIn1(void);
		static SINT32 GetCustomAIn2(void);
		//Output functions
		static void  SetOutputAmplitude(SINT32 Val);
		static void  SetOutputPower(SINT32 Val);
		static void  SetOutputFrequency(SINT32 Val);
		static void  SetAutomationOP1(SINT32 Val);
		//functions called during IO diagnostic
		static SINT32 GetDigINPutValue(DIGITAL_INPUTS DigIn);
		static SINT32 GetDigOutPutValue(DIGITAL_OUTPUTS DigOut);
		static void   SetDigOutPutValue(DIGITAL_OUTPUTS DigOut, BOOLEAN Val);
		static void   SetAnalogOUTPutValue(ANALOG_OUTPUTS AOut, SINT32 Val);
		static SINT32 GetAnalogINPutValue(ANALOG_INPUTS AIn);
		static SINT32 GetAnalogOUTPutValue(ANALOG_OUTPUTS AOut);
		static BOOLEAN GetAlarmStatus(void);
		static BOOLEAN GetReadyFlag(void);
		static BOOLEAN CheckForCycleRunning(void);
		static SINT32 GetDigitalinputActivityStatus(SINT32 FuncId);
		static SINT32 GetDigitaloutputActivityStatus(SINT32 FuncId);
		static void FanEnable(BOOLEAN State);
		static void ClearOutputs();
};
extern BOOLEAN CustomInput1Configured, CustomInput2Configured,InterlockConfigured,
		PartInPlaceConfigured;
void InitExternalToFreqOffsetMapping();
BOOLEAN CheckForLoadNewPreset(void);
extern const UINT8 DigInPins[NUM_DIN];
extern const UINT8 DigOutPins[NUM_DOUT];
extern const UINT8 AinPins[NUM_AIN];
extern const UINT8 AoutPins[NUM_AOUT];
extern BOOLEAN CableFailure;
#endif
