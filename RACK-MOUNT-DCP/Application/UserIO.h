/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UserIO.h_v   1.4.1.4.1.1   15 Sep 2016 15:43:16   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UserIO.h_v  $
 * 
 *    Rev 1.4.1.4.1.1   15 Sep 2016 15:43:16   EGUAJARDO
 * Extended analog inputs for the energy ranges 10J and 30J.
 * 
 *    Rev 1.4.1.4.1.0   06 Jun 2016 16:18:10   GButron
 * V1.3.7A FT Commands
 *
 *    Rev 1.4.1.4   19 Mar 2015 07:13:00   sesharma
 * Branson Coding Standard Applied.
 * 
 *    Rev 1.4.1.3   26 Feb 2015 08:32:36   sesharma
 * Unused Function Removed.
 * 
 *    Rev 1.4.1.2   18 Feb 2015 01:01:24   SaPawar
 * function for getting power window limit alarm added
 * 
 *    Rev 1.4.1.0   13 Jan 2015 06:02:52   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.4   07 Nov 2014 16:25:46   ewomack
 * Added functions to set Power Window Limit signals.
 * 
 *    Rev 1.3   21 Oct 2014 14:29:16   ewomack
 * Force user IO to default while system type is Rackmount Basic.  Make all Digital Outputs active low for Basic.
 * 
 *    Rev 1.2   18 Sep 2014 10:53:30   amaurya
 * Added defines for RAWVAL_TWENTYPER
 * 
 *    Rev 1.1   17 Sep 2014 09:05:20   amaurya
 * Renamed defines. 
 * 
 *    Rev 1.0   18 Aug 2014 09:17:32   amaurya
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
#ifndef USERIONEW_H_
#define USERIONEW_H_

#include "MCP3208.h"
#include "TLV5604.h"
#include "TYP_DEF.h"
#include "DupsTask.h"

#define	NUM_DIN		4+1		// one more than needed, don’t use 0 index
#define	NUM_FUNC_IN	36		// # of input functions defined in PRS
#define	DIN1_BIT		BIT(7)		// Digital_in_1 J32 Pin-1 as defined by input hardware
#define	DIN2_BIT		BIT(6)		// Digital_in_2 J32 Pin-2 as defined by input hardware
#define	DIN3_BIT		BIT(5)		// Digital_in_3 J32 Pin-3 as defined by input hardware
#define	DIN4_BIT		BIT(4)		// Digital_in_4 J32 Pin-4 as defined by input hardware
#define	DIN1_LO		DIN1_BIT    // used to make XOR bit to make active low
#define	DIN1_HI		0				// used to make XOR bit to make active high
#define	DIN2_LO		DIN2_BIT    // used to make XOR bit to make active low
#define	DIN2_HI		0				// used to make XOR bit to make active high
#define	DIN3_LO		DIN3_BIT    // used to make XOR bit to make active low
#define	DIN3_HI		0				// used to make XOR bit to make active high
#define	DIN4_LO		DIN4_BIT    // used to make XOR bit to make active low
#define	DIN4_HI		0				// used to make XOR bit to make active high
#define	NUM_DOUT			5+1		// one more than needed, don’t use 0 index
#define	NUM_FUNC_OUT	59		// # of output functions defined in PRS
#define	DOUT1_BIT		BIT(7)		// Digital_out_1 J32 Pin-7 as defined by input hardware
#define	DOUT2_BIT		BIT(6)		// Digital_out_2 J32 Pin-8 as defined by input hardware
#define	DOUT3_BIT		BIT(5)		// Digital_out_3 J32 Pin-16 as defined by input hardware
#define	DOUT4_BIT		BIT(4)		// Digital_out_4 J32 Pin-17 as defined by input hardware
#define	DOUT5_BIT		BIT(1)		//	Digital_out_5 J1-19 DC-STD
#define	DOUT1_LO			DOUT1_BIT	// used to make XOR bit to make active low
#define	DOUT1_HI			0				// used to make XOR bit to make active high
#define	DOUT2_LO			DOUT2_BIT	// used to make XOR bit to make active low
#define	DOUT2_HI			0				// used to make XOR bit to make active high
#define	DOUT3_LO			DOUT3_BIT	// used to make XOR bit to make active low
#define	DOUT3_HI			0				// used to make XOR bit to make active high
#define	DOUT4_LO			DOUT4_BIT	// used to make XOR bit to make active low
#define	DOUT4_HI			0				// used to make XOR bit to make active high
#define  DOUT5_LO			DOUT5_BIT
#define  DOUT5_HI			0

#define	NUM_AIN		2+1				// one more than needed, don’t use 0 index
#define	NUM_FUNC_AIN	7				// # of input functions defined in PRS
//There are 8 data lines (and no address lines) between DCX and MI
//board connected via a bidirectional buffer. These lines can be
//read/written through chip select by processor. The address defined
//below is reserved for UserIO
//TODO: Exact address to be determined not to conflict other memory mapped devices
//For the application it would look like reading
//or writing some memory address.
//Current designs have only 8 digital UserIOs.
#define DIGITAL_IO_VAL (*((volatile unsigned char*)MI_DIGITALIO_BASEADDRESS))

//Analog Input 1 is channel_0 on MCP(ADC) chip of MI board
//Analog Input 2 is channel_1 on MCP(ADC) chip of MI board
#define AIN1_CHANNEL  AdcInputCh0
#define AIN2_CHANNEL  AdcInputCh1

#define	NUM_AOUT		2+1		// one more than needed, don’t use 0 index
#define	NUM_FUNC_AOUT	18		// # of output functions defined in PRS
//Analog Output 1 is channel_A on TLV(DAC) chip of MI board
//Analog Output 2 is channel_B on TLV(DAC) chip of MI board
#define AOUT1_CHANNEL DacOutputA
#define AOUT2_CHANNEL DacOutputB

//The value to determine that whether the
//UserIO module ever been initialized.
#define IOCONFIGURED_CHECKSUM 0x12CC33CB

#define EXT_FREQ_SCALINGFACTOR 1024//1250
#define FREQ_OFFSET_LIMIT      500

#define REFVOLTAGE_DAC    2500
#define REFVOLTAGE_ADC    2500
#define RAWVAL_TENPER     204
#define RAWVAL_TWENTYPER  409
#define RAWVAL_FIFTYPER   1024
#define RAWVAL_TWOPER     40

//1KJ range, 1V = 1J, 10V = 10J
#define ENERGYRANGE_1_MIN    1
#define ENERGYRANGE_1_MAX    10

//1KJ range, 1V = 3J, 10V = 30J
#define ENERGYRANGE_2_MIN    3
#define ENERGYRANGE_2_MAX    30

//1KJ range, 1V = 25J, 10V = 250J
#define ENERGYRANGE_3_MIN    25
#define ENERGYRANGE_3_MAX    250

//1KJ range, 1V = 100J, 10V = 1000J
#define ENERGYRANGE_4_MIN    100
#define ENERGYRANGE_4_MAX    1000

//1KJ range, 1V = 1000J, 10V = 10000J
#define ENERGYRANGE_5_MIN	 1000
#define ENERGYRANGE_5_MAX	 10000

#define ENERGY_MULT_FAC      1000

enum ACTIVE {
	LOW, HIGH
};

// These enums are used to determine which I/O functions are available for which configurations.

typedef struct {
	BOOLEAN *Flag;
	enum ACTIVE Active;
	UINT32 Control;
} DIGITALIOMAP;

enum DIGITAL_INPUTS {
	DIGIN1 = 1, DIGIN2, DIGIN3, DIGIN4
};
enum DIGITAL_OUTPUTS {
	DIGOUT1 = 1, DIGOUT2, DIGOUT3, DIGOUT4, DIGOUT5
};

/*Analog IOs*/
typedef struct {
	SINT32 *value;
	UINT32 Control;
} ANALOGIOMAP;

struct USERIOConfiguration {
	// XOR mask for digital inputs
	UINT16 InputMask;

	// function assignment
	UINT16 DigInUse[NUM_DIN];

	// function assignment Status
	BOOLEAN DigInUseStatus[NUM_DIN];

	// XOR mask for digital outputs
	UINT16 OutputMask;

	// function assignment
	UINT16 DigOutUse[NUM_DOUT];

	// function assignment Status
	BOOLEAN DigOutUseStatus[NUM_DOUT];

	// function assignment
	UINT16 AnalogInUse[NUM_AIN];

	// function assignment
	BOOLEAN AnalogInUseStatus[NUM_AIN];

	// function assignment
	UINT16 AnalogOutUse[NUM_AOUT];

	// function assignment
	BOOLEAN AnalogOutUseStatus[NUM_AOUT];
};

enum ANALOG_INPUTS {
	AIN1 = 1, AIN2, AIN3, AIN4, AIN5, AIN6, AIN7, AIN8
};
enum ANALOG_OUTPUTS {
	AOUT1 = 1, AOUT2
};

enum USERDEFINED_ANALOGINPUTFUNC {
	INPUT_DISABLED = 0,
	FREQ_CONTROL,
	AMPL_CONTROL,
	ENERGY_CONTROL_RANGE1,
	ENERGY_CONTROL_RANGE2,
	ENERGY_CONTROL_RANGE3,
	ENERGY_CONTROL_RANGE4,
	ENERGY_CONTROL_RANGE5
};

enum USERDEFINED_ANALOGOUTPUTFUNC {
	OUTPUT_DISABLED = 0,
	POWER_OUT,
	AMPL_OUT,
	MEMFREQ_OUT,
	FREQ_OUT,
	ENERGY_OUT,
	POWER_OUT400W,
	POWER_OUT750W, //406:S,Power Out-750 Watts
	POWER_OUT800W, //407:S,Power Out-800 Watts
	POWER_OUT1100W, //408:S,Power Out-1100 Watts
	POWER_OUT1250W, //409:S,Power Out-1250 Watts
	POWER_OUT1500W, //410:S,Power Out-1500 Watts
	POWER_OUT1600W, //411:S,Power Out-1600 Watts
	POWER_OUT2200W, //412:S,Power Out-2200 Watts
	POWER_OUT2400W, //413:S,Power Out-2400 Watts
	POWER_OUT2500W, //414:S,Power Out-2500 Watts
	POWER_OUT3000W, //415:S,Power Out-3000 Watts
	POWER_OUT3300W, //416:S,Power Out-3300 Watts
	POWER_OUT4000W, //417:S,Power Out-4000 Watts
};

enum USERDEFINED_DIGINPUTFUNC {
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
	GRNDDETECT_ACTIVELOW,
	EXTAMPL_STEPTRIGG_ACTIVEHIGH,
	EXTAMPL_STEPTRIGG_ACTIVELOW,
	GOINTOHORNDOWN_ACTIVEHIGH,
	GOINTOHORNDOWN_ACTIVELOW,
	CABLEDETECT_ACTIVEHIGH,
};

enum USERDEFINED_DIGOUTPUTFUNC {
	OUTPUTPIN_DISABLED = 0,
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
	DummyOutput_ACTIVEHIGH,
	DummyOutput_ACTIVELOW,
	HOLDTIME_ACTIVEHIGH,
	HOLDTIME_ACTIVELOW,
	AFTERBUSTDELAY_ACTIVEHIGH,
	AFTERBUSTDELAY_ACTIVELOW,
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
	BEEPER_ACTIVEHIGH,
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
	RFSWITCHSELECDTA,
	RFSWITCHSELECDTB,
	RFSWITCHSELECDTC,
	RFSWITCHSELECDTD,
	CYCLESTART_ACTIVEHIGH,
	CYCLESTART_ACTIVELOW,
	CUSTOMALARM_ACTIVEHIGH,
	CUSTOMALARM_ACTIVELOW,
	PPEAKPOWERLIMIT_ACTIVEHIGH,
	PPEAKPOWERLIMIT_ACTIVELOW,
	MPEAKPOWERLIMIT_ACTIVEHIGH,
	MPEAKPOWERLIMIT_ACTIVELOW
};

enum STATUS {
	DEACTIVATED, ACTIVATED
};

//This class defines a interface to communicate with MI board.
class UserIO {
	static int ReadDigitalIO();
	static void WriteDigitalIO(int val);
	static void ConfigureUserIOsFromFRAM();
	static UINT16 InputMask;
	static UINT16 OutputMask;
	static BOOLEAN DigInUseStatus[NUM_DIN];
	static BOOLEAN DigOutUseStatus[NUM_DOUT];
	static BOOLEAN AnalogInUseStatus[NUM_AIN];
	static BOOLEAN AnalogOutUseStatus[NUM_AOUT];
	static BOOLEAN CheckSignalActive(BOOLEAN signal);
	static void InitCableDetectInput();
	static void InitEnergyRangeSelection();
public:
	static SINT32 ReadDigitalIOFT(void);
	static void InitFRAMIOConfig();
	static unsigned int validIOConfiguration;
	static void ResetUserIO();
	static void AssignDigitalOutputFunction(enum DIGITAL_OUTPUTS Output,
	      UINT16 FuncNum, BOOLEAN Status);
	static void AssignDigitalInputFunction(enum DIGITAL_INPUTS Input,
	      UINT16 FuncNum, BOOLEAN Status);
	static void AssignAnalogOutputFunction(enum ANALOG_OUTPUTS Output,
	      UINT16 FuncNum, BOOLEAN Status);
	static void AssignAnalogInputFunction(enum ANALOG_INPUTS Input,
	      UINT16 FuncNum, BOOLEAN Status);
	static void ReadInputs();
	static void WriteOutputs();
	static void InitUserIO(void);
	static void ConfigureDefaultUserIOs(bool log = true);
	//uncomment following object declaration later
	static TLV5604 * dac;
	static MCP3208 * adc;
	//Digital IO functions
	//Input functions
	static BOOLEAN CheckForExternalReset(void);
	static BOOLEAN CheckForExternalAmpStep(void);
	static BOOLEAN CheckForExternalSeek(void);
	static BOOLEAN CheckForRecallPreset1(void);
	static BOOLEAN CheckForRecallPreset2(void);
	static BOOLEAN CheckForRecallPreset4(void);
	static BOOLEAN CheckForRecallPreset8(void);
	static BOOLEAN CheckForRecallPreset16(void);
	static BOOLEAN CheckForRecallPreset32(void);
	static BOOLEAN CheckForExternalStart(void);
	static BOOLEAN CheckForExternalSonicsDelay(void);
	static BOOLEAN CheckForLockOutChange(void);
	static BOOLEAN CheckForSonicsDisable(void);
	static BOOLEAN CheckForInterLockInPlace(void);
	static BOOLEAN CheckForLoadNewPreset(void);
	static BOOLEAN CheckForCycleAbort(void);
	static BOOLEAN CheckForMemoryClear(void);
	static BOOLEAN CheckForGroundDetect(void);
	static BOOLEAN CheckForHornScan(void);
	static BOOLEAN CheckForExternalTest(void);
	static BOOLEAN CheckForHornDownMode(void);
	static BOOLEAN CheckForInputCable(void);
	static BOOLEAN CheckForCableDetect();

	//Output functions
	static void SetReady(BOOLEAN State);				// Set or clear Ready flag
	static void SetSonicsActive(BOOLEAN State);
	static void SetSeekOut(BOOLEAN State);
	static void SetGenAlarm(BOOLEAN State);
	static void SetBeeper(BOOLEAN State);
	static void SetCycleOK(BOOLEAN State);
	static void SetPrsetChanged(BOOLEAN State);
	static void SetHoldTimeActive(BOOLEAN State);
	static void SetAfterBurstDelayActive(BOOLEAN State);
	static void SetAfterBurstTimeActive(BOOLEAN State);
	static void SetPlusTimeLimitAlarm(BOOLEAN State);
	static void SetMinusTimeLimitAlarm(BOOLEAN State);
	static void SePlusEnergyLimitAlarm(BOOLEAN State);
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
	static void SetCableDetect(BOOLEAN state);
	static void SetPowerWindowLimitHigh(BOOLEAN state);
	static void SetPowerWindowLimitLow(BOOLEAN state);
	//Analog IO functions
	//Input functions
	static SINT32 GetExternalAmplitude();
	static SINT32 GetExternalFrequency();
	//Output functions
	static void SetOutputAmplitude(SINT32 val);
	static void SetOutputPower(SINT32 val);
	static void SetOutputFrequency(SINT32 val);
	static void SetOutputMem(SINT32 val);
	static void SetOutputEnergy(SINT32 val);
	//
	static int GetDigINPutActiveStatus(DIGITAL_INPUTS digIn);
	static int GetDigOutPutActiveStatus(DIGITAL_OUTPUTS digOut);
	static BOOLEAN GetDigOutPutActiveConfiguration(DIGITAL_OUTPUTS digOut);
	static void SetDigOutPutActiveStatus(DIGITAL_OUTPUTS digOut, BOOLEAN val);
	static void SetAnalogOUTPutValue(ANALOG_OUTPUTS aOut, SINT32 val);
	static SINT32 GetAnalogINPutValue(ANALOG_INPUTS aIn);
	static SINT32 GetAnalogOUTPutValue(ANALOG_OUTPUTS aOut);
	static SINT32 DigFilterMISAin(SINT32 Element, ANALOG_INPUTS InputNumber);
	static BOOLEAN GetGenAlarmFlag();
	static BOOLEAN GetReadyFlag();
	static SINT32 GetRawExternalEnergyInPSForm();
	static void SetEnergyFuncNum(UINT16 EnergyFuncNum);
	static UINT16 CheckForEnergyRange();
	static void ClearOutputs();
	static void SetDiagnosticOutput(DIGITAL_OUTPUTS digOut, BOOLEAN val);
};

#endif /* USERIONEW_H_ */
