/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/preset.h_v   1.2   10 Aug 2015 09:40:58   RJamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/preset.h_v  $
 * 
 *    Rev 1.2   10 Aug 2015 09:40:58   RJamloki
 * Added ClearMemoryFrequencyAtPowerup() function declaration
 * 
 *    Rev 1.1   08 Jul 2015 02:42:06   sesharma
 * Added declaration for ClearMemoryFrequency().
 * 
 *    Rev 1.0   09 Jun 2015 09:10:50   tayars
 * Initial revision.
 * 
 *    Rev 1.29   21 Mar 2014 09:25:18   rjamloki
 * Tracker issue fixes.
 */
 
#ifndef PRESETNEW_H_
#define PRESETNEW_H_
#include "portable.h"
#include "CyGOS.h"
#include "SysConfig.h"
#include "DEF_DEF.H"
#include "TYP_DEF.h"

/*---------------------------- Default Preset values -------------------------------*/
#define OFF					    0
#define ON					    1
#define MAX_TIME_CUTOFF         6000

#define CUSTOM_INPUTS_MUL_FACTOR 10
//Actuator
#define CUSTOM_INPUT1_DEFAULT	CUSTOM_INPUT1_MAX//STROKE_MIN
#define CUSTOM_INPUT2_DEFAULT   CUSTOM_INPUT2_MAX//STROKE_MIN

//WC
#define ABAMP_MIN			10
#define ABAMP_MAX			100  // 100 %
#define ABAMP_DEFAULT      	100

#define ABDELAY_MIN			100
#define ABDELAY_MAX			2000
#define ABDELAY_DEFAULT    	100

#define ABTIME_MIN			100
#define ABTIME_MAX			2000
#define ABTIME_DEFAULT     	100

#define AMP1_MIN			10
#define AMP1_MAX			100
#define AMP1_DEFAULT       	100

#define AMP2_MIN		    10
#define AMP2_MAX			100
#define AMP2_DEFAULT       	100

#define AMPSTEPCONDITION_MIN	AMPSTEP_COND_OFF
#define AMPSTEPCONDITION_MAX   	AMPSTEP_COND_EXTERNAL
#define AMPSTEPCONDITION_DEFAULT	AMPSTEP_COND_OFF

#define AMPSTEPTIME_MIN		   	1
#define AMPSTEPTIME_MAX		   	30000
#define AMPSTEPTIME_DEFAULT		10

#define AMPSTEPENERGY_MIN	   	1
#define AMPSTEPENERGY_MAX	   	99990
#define AMPSTEPENERGY_DEFAULT	100

#define AMPSTEPPOWER_MIN	   	1
#define AMPSTEPPOWER_MAX	   	100
#define AMPSTEPPOWER_DEFAULT	1
#define DIGITALTUNE20K_MIN		194500
#define DIGITALTUNE30K_MIN		292500
#define DIGITALTUNE40K_MIN		389000

#define DIGITALTUNE20K_MAX   	204500
#define DIGITALTUNE30K_MAX   	307500
#define DIGITALTUNE40K_MAX   	409000

#define DIGITALTUNE20K_DEF		F20KHZMIDBAND
#define DIGITALTUNE30K_DEF		F30KHZMIDBAND
#define DIGITALTUNE40K_DEF		F40KHZMIDBAND


#define EBAMP_MIN            	1
#define EBAMP_MAX             	100
#define EBAMP_DEFAULT           3

#define EBTIME_MIN		      	10
#define EBTIME_MAX		      	1000
#define EBTIME_DEFAULT		    20

#define ENERGYCP_MIN 		   	1
#define ENERGYCP_MAX 		   	99990
#define ENERGYCP_DEFAULT 		5000

#define ENERGYCOMP_MIN 	     	1
#define ENERGYCOMP_MAX 	      	99990
#define ENERGYCOMP_DEFAULT 	    1

#define ENERGYCUTOFF_MIN     	1
#define ENERGYCUTOFF_MAX      	99990
#define ENERGYCUTOFF_DEFAULT    10

#define EXTTRIGDELAYTIME_MIN  	10
#define EXTTRIGDELAYTIME_MAX  	10000
#define EXTTRIGDELAYTIME_DEFAULT 10

#define HOLDTIME_MIN          	0
#define HOLDTIME_MAX          	30000
#define HOLDTIME_DEFAULT        10

#define MAXWELDTIME_MIN 	  	10
#define MAXWELDTIME_MAX 	   	30000
#define MAXWELDTIME_DEFAULT 	10

#define PEAKPOWERCP_MIN  	  	10
#define PEAKPOWERCP_MAX  	   	1000
#define PEAKPOWERCP_DEFAULT  	10

#define PEAKPOWERCUTOFF_MIN   	10
#define PEAKPOWERCUTOFF_MAX   	100
#define PEAKPOWERCUTOFF_DEFAULT 10

#define SCRUBAMP_MIN       		10
#define SCRUBAMP_MAX       		100
#define SCRUBAMP_DEFAULT       	100

#define SCRUBTIME_MIN       	1
#define SCRUBTIME_MAX       	500
#define SCRUBTIME_DEFAULT      	1

#define TIMECUTOFF_MIN		   	1
#define TIMECUTOFF_MAX		   	30000
#define TIMECUTOFF_DEFAULT		6000

#define WELDMODE_MIN		    ContinuousMode
#define WELDMODE_MAX		    GroundDetectMode
#define WELDMODE_DEFAULT		ContinuousMode

#define WELDTIMECP_MIN		   	10
#define WELDTIMECP_MAX		   	30000
#define WELDTIMECP_DEFAULT		10

#define WELD_PHASELIMIT_MIN		     4
#define WELD_PHASELIMIT_MAX		     9765
#define WELD_PHASELIMIT_DEFAULT		 1000

#define WELD_PHASELIMIT_TIME_MIN	 1
#define WELD_PHASELIMIT_TIME_MAX	 20000
#define WELD_PHASELIMIT_TIME_DEFAULT 500

#define WELD_FREQCUTOFFLOW20KHZ_MIN 	10
#define WELD_FREQCUTOFFLOW20KHZ_MAX 	5000
#define WELD_FREQCUTOFFLOW20KHZ_DEFAULT 5000

#define WELD_FREQCUTOFFHIGH20KHZ_MIN 		10
#define WELD_FREQCUTOFFHIGH20KHZ_MAX 		5000
#define WELD_FREQCUTOFFHIGH20KHZ_DEFAULT 	5000

#define WELD_FREQLOW20KHZ_MIN 		10
#define WELD_FREQLOW20KHZ_MAX 		5000
#define WELD_FREQLOW20KHZ_DEFAULT 	5000

#define WELD_FREQHIGH20KHZ_MIN 		10
#define WELD_FREQHIGH20KHZ_MAX 		5000
#define WELD_FREQHIGH20KHZ_DEFAULT 	5000

#define FREQOFFSET20K_MIN 	-5000
#define FREQOFFSET20K_MAX 	5000
#define FREQOFFSET_DEFAULT 	0

#define SEEKFREQLOW20K_MIN 		10
#define SEEKFREQLOW20K_MAX 		5000
#define SEEKFREQLOW20K_DEFAULT 	5000

#define SEEKFREQHIGH20K_MIN 		10
#define SEEKFREQHIGH20K_MAX 		5000
#define SEEKFREQHIGH20K_DEFAULT 	5000

#define TESTFREQLOW20KHZ_MIN 		10
#define TESTFREQLOW20KHZ_MAX 		5000
#define TESTFREQLOW20KHZ_DEFAULT 	5000

#define TESTFREQHIGH20KHZ_MIN 		10
#define TESTFREQHIGH20KHZ_MAX 		5000
#define TESTFREQHIGH20KHZ_DEFAULT 	5000

#define TESTFREQOFFSET20KHZ_MIN 	-5000
#define TESTFREQOFFSET20KHZ_MAX 	5000
#define TESTFREQOFFSET20KHZ_DEFAULT 0

#define WELD_FREQCUTOFFLOW30KHZ_MIN 	10
#define WELD_FREQCUTOFFLOW30KHZ_MAX 	7500
#define WELD_FREQCUTOFFLOW30KHZ_DEFAULT 7500

#define WELD_FREQCUTOFFHIGH30KHZ_MIN 		10
#define WELD_FREQCUTOFFHIGH30KHZ_MAX 		7500
#define WELD_FREQCUTOFFHIGH30KHZ_DEFAULT 	7500

#define WELD_FREQLOW30KHZ_MIN 		10
#define WELD_FREQLOW30KHZ_MAX 		7500
#define WELD_FREQLOW30KHZ_DEFAULT 	7500

#define WELD_FREQHIGH30KHZ_MIN 		10
#define WELD_FREQHIGH30KHZ_MAX 		7500
#define WELD_FREQHIGH30KHZ_DEFAULT 	7500

#define FREQOFFSET30K_MIN -7500
#define FREQOFFSET30K_MAX  7500

#define SEEKFREQLOW30K_MIN 		10
#define SEEKFREQLOW30K_MAX 		7500
#define SEEKFREQLOW30K_DEFAULT 	7500

#define SEEKFREQHIGH30K_MIN 		10
#define SEEKFREQHIGH30K_MAX 		7500
#define SEEKFREQHIGH30K_DEFAULT 	7500

#define TESTFREQLOW30KHZ_MIN 		10
#define TESTFREQLOW30KHZ_MAX 		7500
#define TESTFREQLOW30KHZ_DEFAULT 	7500

#define TESTFREQHIGH30KHZ_MIN 		10
#define TESTFREQHIGH30KHZ_MAX 		7500
#define TESTFREQHIGH30KHZ_DEFAULT 	7500

#define TESTFREQOFFSET30KHZ_MIN 	-7500
#define TESTFREQOFFSET30KHZ_MAX 	7500
#define TESTFREQOFFSET30KHZ_DEFAULT 0

#define WELD_FREQCUTOFFLOW40KHZ_MIN 	10
#define WELD_FREQCUTOFFLOW40KHZ_MAX 	10000
#define WELD_FREQCUTOFFLOW40KHZ_DEFAULT 10000

#define WELD_FREQCUTOFFHIGH40KHZ_MIN 		10
#define WELD_FREQCUTOFFHIGH40KHZ_MAX 		10000
#define WELD_FREQCUTOFFHIGH40KHZ_DEFAULT 	10000

#define WELD_FREQLOW40KHZ_MIN 		10
#define WELD_FREQLOW40KHZ_MAX 		10000
#define WELD_FREQLOW40KHZ_DEFAULT 	10000

#define WELD_FREQHIGH40KHZ_MIN 		10
#define WELD_FREQHIGH40KHZ_MAX 		10000
#define WELD_FREQHIGH40KHZ_DEFAULT 	10000

#define FREQOFFSET40K_MIN -10000
#define FREQOFFSET40K_MAX 10000
#define FREQOFFSET40K_DEFAULT 0

#define SEEKFREQLOW40K_MIN 10
#define SEEKFREQLOW40K_MAX 10000
#define SEEKFREQLOW40K_DEFAULT 10000

#define SEEKFREQHIGH40K_MIN 10
#define SEEKFREQHIGH40K_MAX 10000
#define SEEKFREQHIGH40K_DEFAULT 10000

#define TESTFREQLOW40KHZ_MIN 10
#define TESTFREQLOW40KHZ_MAX 10000
#define TESTFREQLOW40KHZ_DEFAULT 10000

#define TESTFREQHIGH40KHZ_MIN 10
#define TESTFREQHIGH40KHZ_MAX 10000
#define TESTFREQHIGH40KHZ_DEFAULT 10000

#define TESTFREQOFFSET40KHZ_MIN -10000
#define TESTFREQOFFSET40KHZ_MAX 10000
#define TESTFREQOFFSET40KHZ_DEFAULT 0

#define WELDRAMPTIME_MIN         10
#define WELDRAMPTIME_MAX         1000
#define WELDRAMPTIME_DEFAULT	 80

#define WELD_PROFILERAMPTIME_MIN    	10
#define WELD_PROFILERAMPTIME_MAX 		1000
#define WELD_PROFILERAMPTIME_DEFAULT  	80

#define CREATEDATE_DEFAULT			1
#define CREATEHOUR_DEFAULT 			0
#define CREATEMINUTE_DEFAULT 		0
#define CREATEMONTH_DEFAULT 		1
#define CREATEYEAR_DEFAULT 			12
#define HARDWARECONFIG_DEFAULT 		1
#define HORNPRESETNUMBER_DEFAULT 	1
#define MODIFIEDDATE_DEFAULT 		1
#define MODIFIEDHOUR_DEFAULT 		0
#define MODIFIEDMINUTE_DEFAULT 		0
#define MODIFIEDMONTH_DEFAULT 		1
#define MODIFIEDYEAR_DEFAULT 		12
#define OPERATORID_DEFAULT 			"Branson"
#define PSFREQ_DEFAULT   			FREQ_20KHZ
#define PSSERIALNO_DEFAULT 	     	"Default1234"
#define PSPOWER_DEFAULT  			1500
#define PRESETID_DEFAULT 			"Default Preset"
#define PRESETSTATUS_DEFAULT 		NOTVERIFIED
#define WELDCOUNT_DEFAULT 			0

/*---------------------------- Minimum Preset values -------------------------------*/
//Act min
#define CUSTOM_INPUT1_MIN		(0 * CUSTOM_INPUTS_MUL_FACTOR)
#define CUSTOM_INPUT2_MIN     	(0 * CUSTOM_INPUTS_MUL_FACTOR)

//Info Min
#define CREATEDATE_MIN	      	1
#define CREATEHOUR_MIN        	0
#define CREATEMINUTE_MIN  	  	0
#define CREATEMONTH_MIN 	  	1
#define CREATEYEAR_MIN 		  	12
#define HARDWARECONFIG_MIN 		1
#define HORNPRESETNUMBER_MIN  	1
#define MODIFIEDDATE_MIN 	   	1
#define MODIFIEDHOUR_MIN 	   	0
#define MODIFIEDMINUTE_MIN 		0
#define MODIFIEDMONTH_MIN 	   	1
#define MODIFIEDYEAR_MIN 	   	12
#define OPERATORID_MIN 		   	"Branson"
#define PSFREQ_MIN   		   	FREQ_20KHZ
#define PSSERIALNO_MIN 		   	"Default1234"
#define PSPOWER_MIN  		   	100
#define PRESETID_MIN 		   	"MinPS"
#define PRESETIDTWEK_MIN 		"TweakMinPS"
#define PRESETSTATUS_MIN 	   	NOTVERIFIED
#define WELDCOUNT_MIN 		   	0

//Limits
#define ENERGYMLR_MIN			1
#define ENERGYMLR_MAX		    99990
#define ENERGYMLR_DEFAULT 		10

#define ENERGYPLR_MIN			1
#define ENERGYPLR_MAX		    99990
#define ENERGYPLR_DEFAULT     	99990

#define POWERMLR_MIN	     	1
#define POWERMLR_MAX			100
#define POWERMLR_DEFAULT	   	1

#define POWERPLR_MIN		 	1
#define POWERPLR_MAX			100
#define POWERPLR_DEFAULT	   	100

#define WELDTIMEMLR_MIN			10
#define WELDTIMEMLR_MAX		    30000
#define WELDTIMEMLR_DEFAULT   	10

#define WELDTIMEPLR_MIN			10
#define WELDTIMEPLR_MAX		    30000
#define WELDTIMEPLR_DEFAULT   	30000

/*---------------------------- Maximum Preset values -------------------------------*/

//Actuator max
#define CUSTOM_INPUT1_MAX	(10 * CUSTOM_INPUTS_MUL_FACTOR)
#define CUSTOM_INPUT2_MAX  	(10 * CUSTOM_INPUTS_MUL_FACTOR)

//Info Max
#define CREATEDATE_MAX	       31
#define CREATEHOUR_MAX        23
#define CREATEMINUTE_MAX      59
#define CREATEMONTH_MAX       12
#define CREATEYEAR_MAX        99
#define HARDWARECONFIG_MAX    1
#define HORNPRESETNUMBER_MAX  16
#define HORNPRESETNUMBER_MAX_UNCODED  4
#define MODIFIEDDATE_MAX      31
#define MODIFIEDHOUR_MAX      23
#define MODIFIEDMINUTE_MAX    59
#define MODIFIEDMONTH_MAX     12
#define MODIFIEDYEAR_MAX      99
#define OPERATORID_MAX        "Branson"
#define PSFREQ_MAX            FREQ_40KHZ
#define PSSERIALNO_MAX        "Default1234"
#define PSPOWER_MAX           10000
#define PRESETID_MAX          "MaxPS"
#define PRESETIDTWEK_MAX      "TweakMaxPS"
#define PRESETSTATUS_MAX      VERIFIED
#define WELDCOUNT_MAX         UINT_MAX

#define SEEKTIME_MIN			  10
#define SEEKTIME_MAX			  1000
#define SEEKTIME_DEFAULT		  500

#define SEEKAMP_MIN			      1    // 1 % of 100
#define SEEKAMP_MAX			      10
#define SEEKAMP_DEFAULT		      10

#define SEEKRAMPTIME_MIN		  	10
#define SEEKRAMPTIME_MAX		  	1000
#define SEEKRAMPTIME_DEFAULT		80

#define TEST_TIMEERROR_HIGH_MIN     0
#define TEST_TIMEERROR_HIGH_MAX     30000
#define TEST_TIMEERROR_HIGH_DEFAULT 0

 /*---------------------------- Default Scan Preset values -------------------------------*/
 // 20 KHZ
#define STARTFREQ20K 	194500
#define STOPFREQ20K 	204500
#define FREQSTEP20K 	10
#define TIMEDELAY20K 	10
#define MAXAMPLITUDE20K 10
#define MAXCURRENT20K 	10

// 30 KHZ
#define STARTFREQ30K    292500
#define STOPFREQ30K     307500
#define FREQSTEP30K     10
#define TIMEDELAY30K    10
#define MAXAMPLITUDE30K 10
#define MAXCURRENT30K   10

// 40 KHZ
#define STARTFREQ40K    389000
#define STOPFREQ40K     409000
#define FREQSTEP40K     10
#define TIMEDELAY40K    10
#define MAXAMPLITUDE40K 10
#define MAXCURRENT40K   10

/*---------------------------- Minimum/Maximum scan Preset values -------------------------------*/
#define SCANFREQ20KSTEP_MIN       10
#define SCANFREQ20KSTEP_MAX      500
#define SCANFREQ30KSTEP_MIN       10
#define SCANFREQ30KSTEP_MAX      500
#define SCANFREQ40KSTEP_MIN       10
#define SCANFREQ40KSTEP_MAX      500
#define SCANFREQ20KSTART_MIN  194500
#define SCANFREQ20KSTART_MAX  204500

#define SCANFREQ20KEND_MIN    194500
#define SCANFREQ20KEND_MAX    204500
#define SCANFREQ30KSTART_MIN  292500
#define SCANFREQ30KSTART_MAX  307500
#define SCANFREQ30KEND_MIN    292500
#define SCANFREQ30KEND_MAX    307500

#define SCANFREQ40KSTART_MIN  389000
#define SCANFREQ40KSTART_MAX  409000
#define SCANFREQ40KEND_MIN    389000
#define SCANFREQ40KEND_MAX    409000

#define SCANMAXAMP_MIN           10
#define SCANMAXAMP_MAX           50
#define SCANMAXCURRENT_MIN       10
#define SCANMAXCURRENT_MAX       50
#define SCANTIMEDELAY_MIN        10
#define SCANTIMEDELAY_MAX        100


#define SCANTIMERROR_HIGH_MIN  	10000
#define SCANTIMERROR_HIGH_MAX 	35000
#define SCANTIMERRORHIGH_DEFAULT 30000
//others
#define MAX_STATUS_SET MAX_TOTAL_PRESETS
#define MAX_HSSTATUS_SET MAX_TOTAL_HSPRESETS
#define MAX_HS_FUNC 8
#define PRESETLEN   12
#define SERIAL_NO_LEN  24
#define MAX_PRESET_NAME_LEN  24

#define STOPWELD_AT_PRIMARYPARAMETER_REACH   BIT(0)
/*-------------------- Minimum/Maximum RF Delay -----------------*/
#define MIN_DELAY 0
#define MAX_DELAY 5000
#define FEEDBACKTIMEOUT  50
#define RFSWITCHDELAY    150
#define RELAYSWITCHTIME  50

//Min Max Default of advanced parameters for Weld, TEST and seek. SV prefix is used for the values used in S and V product
#define STD_AMPLITUDE_LOOP_C1_MIN		0
#define STD_AMPLITUDE_LOOP_C1_MAX		500
#define STD_AMPLITUDE_LOOP_C1_DEF20K	60
#define STD_AMPLITUDE_LOOP_C1_DEF30K	120
#define STD_AMPLITUDE_LOOP_C1_DEF40K	120

#define SV_STD_AMPLITUDE_LOOP_C1_MIN	1
#define SV_STD_AMPLITUDE_LOOP_C1_MAX	20000
#define SV_STD_AMPLITUDE_LOOP_C1_DEF20K	100
#define SV_STD_AMPLITUDE_LOOP_C1_DEF30K	205
#define SV_STD_AMPLITUDE_LOOP_C1_DEF40K	205

#define STD_AMPLITUDE_LOOP_C2_MIN		0
#define STD_AMPLITUDE_LOOP_C2_MAX		100
#define STD_AMPLITUDE_LOOP_C2_DEF20K	0
#define STD_AMPLITUDE_LOOP_C2_DEF30K	0
#define STD_AMPLITUDE_LOOP_C2_DEF40K	0

#define SV_STD_AMPLITUDE_LOOP_C2_MIN	1
#define SV_STD_AMPLITUDE_LOOP_C2_MAX	20000
#define SV_STD_AMPLITUDE_LOOP_C2_DEF20K	300
#define SV_STD_AMPLITUDE_LOOP_C2_DEF30K	370
#define SV_STD_AMPLITUDE_LOOP_C2_DEF40K	370

#define STD_PHASE_LOOP_MIN			50
#define STD_PHASE_LOOP_MAX			5000
#define STD_PHASE_LOOP_DEF20K		3680
#define STD_PHASE_LOOP_DEF30K		800
#define STD_PHASE_LOOP_DEF40K		490

#define SV_STD_PHASE_LOOP_MIN		50
#define SV_STD_PHASE_LOOP_MAX		500
#define SV_STD_PHASE_LOOP_DEF20K	450
#define SV_STD_PHASE_LOOP_DEF30K	100
#define SV_STD_PHASE_LOOP_DEF40K	60

#define STD_PHASE_LOOP_CF_MIN		50
#define STD_PHASE_LOOP_CF_MAX		500
#define STD_PHASE_LOOP_CF_DEF20K	450
#define STD_PHASE_LOOP_CF_DEF30K	200
#define STD_PHASE_LOOP_CF_DEF40K	200

#define SV_STD_PHASE_LOOP_CF_MIN	50
#define SV_STD_PHASE_LOOP_CF_MAX	500
#define SV_STD_PHASE_LOOP_CF_DEF20K	450
#define SV_STD_PHASE_LOOP_CF_DEF30K	200
#define SV_STD_PHASE_LOOP_CF_DEF40K	200

//PI
#define PI_AMPLITUDE_LOOP_C1_MIN	0
#define PI_AMPLITUDE_LOOP_C1_MAX	500
#define PI_AMPLITUDE_LOOP_C1_DEF20K	60
#define PI_AMPLITUDE_LOOP_C1_DEF30K	60
#define PI_AMPLITUDE_LOOP_C1_DEF40K	60

#define SV_PI_AMPLITUDE_LOOP_C1_MIN		1
#define SV_PI_AMPLITUDE_LOOP_C1_MAX		500
#define SV_PI_AMPLITUDE_LOOP_C1_DEF20K	100
#define SV_PI_AMPLITUDE_LOOP_C1_DEF30K	205
#define SV_PI_AMPLITUDE_LOOP_C1_DEF40K	205

#define PI_AMPLITUDE_LOOP_C2_MIN		0
#define PI_AMPLITUDE_LOOP_C2_MAX		100
#define PI_AMPLITUDE_LOOP_C2_DEF20K		0
#define PI_AMPLITUDE_LOOP_C2_DEF30K		0
#define PI_AMPLITUDE_LOOP_C2_DEF40K		0

#define SV_PI_AMPLITUDE_LOOP_C2_MIN		1
#define SV_PI_AMPLITUDE_LOOP_C2_MAX		500
#define SV_PI_AMPLITUDE_LOOP_C2_DEF20K	5
#define SV_PI_AMPLITUDE_LOOP_C2_DEF30K	5
#define SV_PI_AMPLITUDE_LOOP_C2_DEF40K	5

#define PI_PHASE_INTEGRAL_GAIN_MIN		0
#define PI_PHASE_INTEGRAL_GAIN_MAX		300
#define PI_PHASE_INTEGRAL_GAIN_DEF20K	60
#define PI_PHASE_INTEGRAL_GAIN_DEF30K	60
#define PI_PHASE_INTEGRAL_GAIN_DEF40K	60

#define SV_PI_PHASE_INTEGRAL_GAIN_MIN		1
#define SV_PI_PHASE_INTEGRAL_GAIN_MAX		500
#define SV_PI_PHASE_INTEGRAL_GAIN_DEF20K	22
#define SV_PI_PHASE_INTEGRAL_GAIN_DEF30K	25
#define SV_PI_PHASE_INTEGRAL_GAIN_DEF40K	41

#define PI_PHASE_PROPORTIONAL_GAIN_MIN 		0
#define PI_PHASE_PROPORTIONAL_GAIN_MAX 		10000
#define PI_PHASE_PROPORTIONAL_GAIN_DEF20K	500
#define PI_PHASE_PROPORTIONAL_GAIN_DEF30K	500
#define PI_PHASE_PROPORTIONAL_GAIN_DEF40K	500

#define SV_PI_PHASE_PROPORTIONAL_GAIN_MIN 		1
#define SV_PI_PHASE_PROPORTIONAL_GAIN_MAX 		500
#define SV_PI_PHASE_PROPORTIONAL_GAIN_DEF20K	5
#define SV_PI_PHASE_PROPORTIONAL_GAIN_DEF30K	5
#define SV_PI_PHASE_PROPORTIONAL_GAIN_DEF40K	5

#define PI_FREQUENCY_WINDOW_SIZE_MIN  		1
#define PI_FREQUENCY_WINDOW_SIZE_MAX		100
#define PI_FREQUENCY_WINDOW_SIZE_DEF20K		1
#define PI_FREQUENCY_WINDOW_SIZE_DEF30K		1
#define PI_FREQUENCY_WINDOW_SIZE_DEF40K		1

#define SV_PI_FREQUENCY_WINDOW_SIZE_MIN  	1
#define SV_PI_FREQUENCY_WINDOW_SIZE_MAX		500
#define SV_PI_FREQUENCY_WINDOW_SIZE_DEF20K	100
#define SV_PI_FREQUENCY_WINDOW_SIZE_DEF30K	100
#define SV_PI_FREQUENCY_WINDOW_SIZE_DEF40K	100

#define PF_PHASE_CONTROL_LOOP_MIN 			50
#define PF_PHASE_CONTROL_LOOP_MAX 			500
#define PI_PF_PHASE_CONTROL_LOOP_DEF20K		450
#define PI_PF_PHASE_CONTROL_LOOP_DEF30K		450
#define PI_PF_PHASE_CONTROL_LOOP_DEF40K		450

#define SV_PF_PHASE_CONTROL_LOOP_MIN 		1
#define SV_PF_PHASE_CONTROL_LOOP_MAX 		500
#define SV_PI_PF_PHASE_CONTROL_LOOP_DEF20K	450
#define SV_PI_PF_PHASE_CONTROL_LOOP_DEF30K	200
#define SV_PI_PF_PHASE_CONTROL_LOOP_DEF40K	200

//Advanced SEEK MIN MAX and defaults
#define SEEK_AMPLITUDE_LOOP_C1_STD_MIN		0
#define SEEK_AMPLITUDE_LOOP_C1_STD_MAX		500
#define SEEK_AMPLITUDE_LOOP_C1_STD_DEF20K	60
#define SEEK_AMPLITUDE_LOOP_C1_STD_DEF30K	120
#define SEEK_AMPLITUDE_LOOP_C1_STD_DEF40K	120

#define SV_SEEK_AMPLITUDE_LOOP_C1_STD_MIN		100
#define SV_SEEK_AMPLITUDE_LOOP_C1_STD_MAX		500
#define SV_SEEK_AMPLITUDE_LOOP_C1_STD_DEF20K	205
#define SV_SEEK_AMPLITUDE_LOOP_C1_STD_DEF30K	205
#define SV_SEEK_AMPLITUDE_LOOP_C1_STD_DEF40K	205

#define SEEK_AMPLITUDE_LOOP_C2_STD_MIN		0
#define SEEK_AMPLITUDE_LOOP_C2_STD_MAX		100
#define SEEK_AMPLITUDE_LOOP_C2_STD_DEF20K	0
#define SEEK_AMPLITUDE_LOOP_C2_STD_DEF30K	0
#define SEEK_AMPLITUDE_LOOP_C2_STD_DEF40K	0

#define SV_SEEK_AMPLITUDE_LOOP_C2_STD_MIN		100
#define SV_SEEK_AMPLITUDE_LOOP_C2_STD_MAX		500
#define SV_SEEK_AMPLITUDE_LOOP_C2_STD_DEF20K	370
#define SV_SEEK_AMPLITUDE_LOOP_C2_STD_DEF30K	370
#define SV_SEEK_AMPLITUDE_LOOP_C2_STD_DEF40K	370

#define SEEK_PHASE_LOOP_STD_MIN				50
#define SEEK_PHASE_LOOP_STD_MAX				5000
#define SEEK_PHASE_LOOP_STD_DEF20K			1840
#define SEEK_PHASE_LOOP_STD_DEF30K			400
#define SEEK_PHASE_LOOP_STD_DEF40K			245

#define SV_SEEK_PHASE_LOOP_STD_MIN			50
#define SV_SEEK_PHASE_LOOP_STD_MAX			500
#define SV_SEEK_PHASE_LOOP_STD_DEF20K		140
#define SV_SEEK_PHASE_LOOP_STD_DEF30K		100
#define SV_SEEK_PHASE_LOOP_STD_DEF40K		60

#define SEEK_PHASE_LOOP_CF_STD_MIN			50
#define SEEK_PHASE_LOOP_CF_STD_MAX			500
#define SEEK_PHASE_LOOP_CF_STD_DEF20K		450
#define SEEK_PHASE_LOOP_CF_STD_DEF30K		450
#define SEEK_PHASE_LOOP_CF_STD_DEF40K		450

#define SV_SEEK_PHASE_LOOP_CF_STD_MIN		50
#define SV_SEEK_PHASE_LOOP_CF_STD_MAX		500
#define SV_SEEK_PHASE_LOOP_CF_STD_DEF20K	200
#define SV_SEEK_PHASE_LOOP_CF_STD_DEF30K	200
#define SV_SEEK_PHASE_LOOP_CF_STD_DEF40K	200

#define MAX_FBALARMS 		48
#define MAX_HSSTATUSDWORDS	15

// Default Alarm code
#define DEFOVERLOADS           	0x3B
#define DEFCUTOFFS           	0x3B
#define DEFSETUP           		0x08
#define DEFCYCLE_MODIFIED      	0x1B
#define DEFWARNINGS           	0x1B
#define DEFPROCESS_LIMITS      	0x3B
#define DEFEQUIPMENT_FAILURES  	0x1A
#define DEFNOCYCLE           	0x0A
#define DEFCOMM_FAILURES       	0x1A
#define DEFTEMP           		0
#define DEFHARDWARE_FAILURE    	0x1B
#define DEFNON_CYCLE_OVERLOADS 	0x3A

enum CONTROL_LIMITS{
   CONTROL_LIMITS_ENABLE = BIT(0),
   CONTROL_LIMITS_ENERGYMLR = BIT(1),
   CONTROL_LIMITS_ENERGYPLR = BIT(2),
   CONTROL_LIMITS_POWERMLR = BIT(3),
   CONTROL_LIMITS_POWERPLR = BIT(4),
   CONTROL_LIMITS_FREQMLR = BIT(5),
   CONTROL_LIMITS_FREQPLR = BIT(6),
   CONTROL_LIMITS_TIMEMLR = BIT(7),
   CONTROL_LIMITS_TIMEPLR = BIT(8),
   CONTROL_LIMITS_ENABLEALL = (CONTROL_LIMITS_ENABLE | CONTROL_LIMITS_ENERGYMLR | CONTROL_LIMITS_ENERGYPLR |
               CONTROL_LIMITS_POWERMLR | CONTROL_LIMITS_POWERPLR | CONTROL_LIMITS_TIMEMLR | CONTROL_LIMITS_TIMEPLR)
};

typedef enum {
   CurrPreset  = 0u,
   MAXIMUM_USER_PRESETS = 32,
   Preset_MIN,
   Preset_MAX,
   Preset_TweakMIN,
   Preset_TweakMAX,
   Preset_Default,//cold start
   MAX_TOTAL_PRESETS,
}PRESET_NUMBER;

typedef enum {
   CurrHsPreset = 0u,
   MAXIMUM_USER_HSPRESETS = 16,
   HsPreset_MIN,
   HsPreset_MAX,
   HsPreset_TweakMIN,
   HsPreset_TweakMAX,
   HsPreset_Default,//cold start
   MAX_TOTAL_HSPRESETS,
}HSPRESET_NUMBER;

enum PRESET_STATUS
{
	NOTVERIFIED,
	VERIFIED,
};

enum AMPSTEP_COND
{
   AMPSTEP_COND_OFF,//fix on Amplitude 1
   AMPSTEP_COND_EXTERNALAIN,//fix on external ampiltude
   AMPSTEP_COND_TIME,
   AMPSTEP_COND_ENERGY,
   AMPSTEP_COND_POWER,
   AMPSTEP_COND_EXTERNAL,//step on digital IO pin activation
};

enum CYCLE_ABORT
{
	CYCLE_ABORT_OFF,
	CYCLE_ABORT_ON,
};

enum WELD_MODE
{
   ContinuousMode,
   TimeMode,
   EnergyMode,
   PeakpowerMode,
   GroundDetectMode,
};

//default commands
enum DEFAULT_PSCMD
{
	DEFAULT_WELDPS = BIT(0),//specific to setup-weld webpage
	DEFAULT_ADVANCEPS = BIT(1),//specific to setup-advanced webpage
	DEFAULT_AMPLITUDEPS = BIT(2),//specific to setup-amplitude web page
	DEAFULT_SEEKPS = BIT(3),//specific to setup-seek amd powerup web page
	DEFAULT_ACTPS = BIT(4),
	DEFAULT_DCPPS = BIT(5),
	DEFAULT_INFOPS = BIT(6),
	DEFAULT_LIMITSPS = BIT(7),//setup-limit webpage
	DEFAULT_PMCPS = BIT(8),
	DEFAULT_VQSPS = BIT(9),
	DEFAULT_WCPS = BIT(10),
	DEFAULT_HSPS = BIT(11),
	DEFAULT_HDVAL = BIT(12),
	DEFAULT_SPECIALPRESET = BIT(13),
	DEFAULT_SPECIALTEST = BIT(14),
	DEFAULT_ADVANCEDIAG = BIT(15),//specific to setup-advanced diag webpage
	DEFAULT_ALLPS = (DEFAULT_ACTPS | DEFAULT_DCPPS | DEFAULT_INFOPS | DEFAULT_LIMITSPS
			| DEFAULT_PMCPS | DEFAULT_VQSPS | DEFAULT_WCPS | DEFAULT_HSPS | DEFAULT_HDVAL | DEFAULT_SPECIALPRESET | DEFAULT_SPECIALTEST),
};

enum LOOP_CONTROL
{
   Standard = 0,
   PI   = 1,
   HD  =  2
};
//what's this in FB???
//needed in status set
enum ResFrequencyOK
{
   NOTOK,
   OK,
};

enum HornFunction
{
	HFSEEK,
	HFTEST,
	HFSCAN,
};

enum SAVECRC_CMD
{
   SAVECRC_ACT = BIT(0),
   SAVECRC_DCP = BIT(1),
   SAVECRC_INFO = BIT(2),
   SAVECRC_LIMITS = BIT(3),
   SAVECRC_PMC = BIT(4),
   SAVECRC_VQS = BIT(5),
   SAVECRC_WC = BIT(6),
   SAVECRC_HSPS = BIT(7),
   SAVECRC_ALL = (SAVECRC_ACT | SAVECRC_DCP | SAVECRC_INFO | SAVECRC_LIMITS | SAVECRC_PMC | SAVECRC_VQS | SAVECRC_WC | SAVECRC_HSPS),
};

//DCX Preset
typedef struct act {				// OPTIONAL
   SINT32		CustomAIn1Cutoff;		// Absolute cutoff distance
   BOOLEAN		CustomAIn1CutoffFlag;		// Absolute cutoff flag
   SINT32      CustomAIn2Cutoff;    // Absolute cutoff distance
   BOOLEAN     CustomAIn2CutoffFlag;      // Absolute cutoff flag
} ACTDCXPS;

typedef struct dcp{						// REQUIRED
	SINT32		WeldAmplitudeLoopC1;	// Amplitude control loop variable C1
	SINT32		WeldAmplitudeLoopC2;	// Amplitude control loop variable C2
   SINT32      WeldAmplitudeLoopC1new; // Amplitude control loop variable C1
   SINT32      WeldAmplitudeLoopC2new; // Amplitude control loop variable C2
	SINT32		WeldPhaseLimit;			// Phase limit control loop variable
	SINT32		WeldPhaseLimitTime;		// Phase time control loop variable
	SINT32		WeldPhaseLoopI;			// Phase control loop variable
	SINT32		WeldPhaseLoopCF;		// Phase loop CF variable
   SINT32      WeldPhaseLoopInew;         // Phase control loop variable
   SINT32      WeldPhaseLoopCFnew;     // Phase loop CF variable
	SINT32  	   WeldPhaseLoopP;		// Phase Proportional Gain
	UINT16	   FreqWindowSize;
	SINT32		WeldFreqHigh;			// Frequency control variable
	SINT32		WeldFreqLow;			// Frequency control variable
	UINT16		WeldRampTime;			// Ramp time for weld
	UINT16		WeldProfileRampTime;	// Ramp time for weld Amplitude profile(A1 to A2)
										 //: Not used currently For profiling purpose whether its start ramp
										  //or stepping between AMP1 and AMP2 WeldRamp time above is used.
	BOOLEAN		MemoryStoreFlag;		// Indicates if the frequency should be stored at end of weld
	BOOLEAN     UseTrueValues;
	BOOLEAN	 	TimedSeekFlag;			// Indicates timed seek is enabled
}DCPDCXPS;

typedef struct DCPWeldStr{                 // REQUIRED
   SINT32      WeldAmplitudeLoopC1; // Amplitude control loop variable C1
   SINT32      WeldAmplitudeLoopC2; // Amplitude control loop variable C2
   SINT32      WeldPhaseLimit;         // Phase limit control loop variable
   SINT32      WeldPhaseLimitTime;     // Phase time control loop variable
   SINT32      WeldPhaseLoopI;         // Phase control loop variable
   SINT32      WeldPhaseLoopCF;     // Phase loop CF variable
   SINT32      WeldPhaseLoopP;      // Phase Proportional Gain
   UINT16      FreqWindowSize;
   SINT32      WeldFreqHigh;        // Frequency control variable
   SINT32      WeldFreqLow;         // Frequency control variable
   UINT16      WeldRampTime;        // Ramp time for weld
   UINT16      WeldProfileRampTime; // Ramp time for weld Amplitude profile(A1 to A2)
									  //: Not used currently For profiling purpose whether its start ramp
									   //or stepping between AMP1 and AMP2 WeldRamp time above is used.
   BOOLEAN     MemoryStoreFlag;     // Indicates if the frequency should be stored at end of weld
   BOOLEAN     UseTrueValues;
   BOOLEAN     TimedSeekFlag;       // Indicates timed seek is enabled
}DCPPresetDCXPS;

typedef struct info {				// REQUIRED
	SINT8		CreatedDate;		// Date preset was created or cold started
	SINT8		CreatedHour;		// Hour preset was created or cold started
	SINT8		CreatedMinute;		// Minute preset was created or cold started
	SINT8		CreatedMonth;		// Month preset was created or cold started
	SINT8		CreatedYear;		// Year preset was created or cold started
	UINT32	HardwareConfig;		// Indicates the configuration of hardware when preset saved
	UINT16	HornPresetNumber;	// Horn preset/horn number associated with weld preset
	SINT8		ModifiedDate;		// Date preset was last saved
	SINT8		ModifiedHour;		// Hour preset was last saved
	SINT8		ModifiedMinute;	// Minute preset was last saved
	SINT8		ModifiedMonth;	// Month preset was last saved
	SINT8		ModifiedYear;		// Year preset was last saved
	UINT8	   OperatorID[OPERATORIDLEN_MAX];		// Operator ID who saved preset for future
	enum FREQUENCY PowerSupplyFreq;	// Power supply frequency
	UINT8		PowerSupplySN[SERIAL_NO_LEN];	// Serial number of power supply preset was created on
	UINT16	PowerSupplyWatts;	// Power supply rating in watts
	UINT16	PresetID[MAX_PRESET_NAME_LEN];		// Ascii or unicode string for name of preset
	enum PRESET_STATUS	PresetStatus;  // Indicates Verified, Modified or Saved status of preset
	UINT32	WeldCount;			// Number of cycles run or number of cycles requested
}INFODCXPS;

typedef struct limits{				// OPTIONAL
	UINT16		ControlLimits;		// Globally enable/disable of control limits
	UINT32		EnergyMLR;		// Energy minus reject limit
	UINT32		EnergyPLR;		// Energy plus reject limit
	UINT16		PowerMLR;		// Power minus reject limit
	UINT16		PowerPLR;		// Power plus reject limit
	UINT16		WeldTimeMLR;		// Weld time minus reject limit
	UINT16		WeldTimePLR;		// Weld time plus reject limit
}LIMITSDCXPS;

typedef struct pmc {//use in future

} PMCDCXPS;

typedef struct sblvqs {//use in future

}SBLVQSDCXPS;

typedef struct wc{				// REQUIRED
	UINT16		ABAmp;			// After burst amplitude
	UINT16		ABDelay;		// After burst delay
	BOOLEAN		ABFlag;			// After burst flag
	UINT16		ABTime;		//After burst time
	UINT16		Amplitude1;		// Amplitude 1 of 2
	UINT16		Amplitude2;		// Amplitude 2 of 2
	enum AMPSTEP_COND	AmpStepCondition;	// Trigger to cause amplitude to switch to Amplitude 2
	UINT16 		AmpStepTime;
	UINT32		AmpStepEnergy;	// Energy value at which amplitude will switch to Amplitude 2
	BOOLEAN		AmpStepExternal;	// Digital input which will switch between Amplitude 1 & Amplitude 2
	UINT16		AmpStepPower;	// Power value at which amplitude will switch to Amplitude 2
	enum CYCLE_ABORT	CycleAborts;		// Enable / disable flags for each cycle abort
	UINT16		EBAmp;			// Energy braking target amplitude
	BOOLEAN		EBFlag;			// Indicates if energy braking function is enabled
	UINT16		EBTime;		// Energy braking time
	UINT32		EnergyCP;		// Energy mode control parameter
	UINT32		EnergyComp;		// Energy compensation value
	BOOLEAN		EnergyCompFlag;	// Indicates if energy compensation is enabled
	UINT32		EnergyCutoff;		// Energy cutoff value
	BOOLEAN		EnergyCutoffFlag;	// Indicates if energy cutoff is enabled
	BOOLEAN		ExtTrigDelayFlag;	// Indicates when external trigger delay is enabled
	UINT16		ExtTrigDelayTime;	// Delay time when external trigger delay is enabled
	SINT32		FreqOffset;		// Frequency offset when frequency offset is enabled
	BOOLEAN		FreqOffsetFlag;		// Indicates if frequency offset is enabled
	BOOLEAN		GeneralAlarmFlag;	// Indicates if reset is required to clear an alarm
	BOOLEAN		GrdDetectFlagCP;	// Indicates when ground detect mode is enabled
	UINT16		HoldTime;		// Ultrasonics off time with SV active
	BOOLEAN		HoldTimeFlag;		// Indicates if hold time function is enabled
	UINT16		MaxWeldTime;		// Maximum time for modes other than time
	UINT16		PeakPowerCP;		// Peak power control parameter
	UINT16		PeakPowerCutoff;	// Peak power cutoff value
	BOOLEAN		PeakPowerCutoffFlag;	// Indicates when peak power cutoff is enabled
	BOOLEAN		PostWeldSeekFlag;	// Indicates when a post weld seek is required
	UINT16		ScrubAmp;		// Amplitude for scrub time on ground detect mode
	UINT16		ScrubTime;		// Time to remain in scrub mode
	BOOLEAN		ScrubTimeFlag;		// Indicates when scrub time is enabled
	UINT32		TimeCutOff;
	BOOLEAN 	   TimeCutOffFlag;
	enum WELD_MODE	WeldMode;		// Indicates which weld modes are enabled
	UINT16		WeldTimeCP;		// Weld time for time mode
	//-Added
	BOOLEAN		CutOffEnabled;
	BOOLEAN		WeldFreqHighCutOffFlag;
	BOOLEAN		WeldFreqLowCutOffFlag;
    SINT32      WeldFreqHighCutoff;  // Frequency control variable
    SINT32      WeldFreqLowCutoff;   // Frequency control variable
}WCDCXPS;

#define  SIZE_STATUS 5
#define  SIZE_PREINITPARAMS 15
#define  SIZE_INITPARAMS    15

typedef struct HDParam
{
	SINT32  InitPara[SIZE_INITPARAMS];
	SINT32  PreInitPara[SIZE_PREINITPARAMS];
	SINT32  Status[SIZE_STATUS];
	enum LOOP_CONTROL LoopControl;
}HDVALUES;

 struct DcxPreset {
	ACTDCXPS 		ActParam;
	DCPDCXPS		DcpParam;
	INFODCXPS 		InfoParam;
	LIMITSDCXPS		LimitsParam;
	WCDCXPS			WcParam;
	PMCDCXPS		PmcParam;
	SBLVQSDCXPS     SblvqsParam;
	HDVALUES        HDVal;          // for HD Values
	UINT32			CRCAct;			// CRC for actuator structure – 0 indicates structure not used
	UINT32			CRCDcp;			// CRC for DCP structure
	UINT32			CRCInfo;		// CRC for Information structure
	UINT32			CRCLimits;		// CRC for limits structure – 0 indicates structure not used
	UINT32			CRCWC;			// CRC for WC structure
	UINT32			CRCPmc;			// CRC for PMC structure
	UINT32			CRCSblVqs;		// CRC for SBLVQS structure
	UINT32			CRCHD;
	UINT8			Reserved[256];
};

//Horn specific parameter set
typedef struct Hspreset
{
	//Seek parameters
	struct
	{
		UINT16		SeekTime;		// Length of time for seeking
		UINT16		SeekAmp;		// Amplitude for seek
		UINT16		SeekRampTime;		// Ramp time for seek
		SINT32		SeekAmpLoopC1;	// Control variable
		SINT32		SeekAmpLoopC2;	// Control variable
		SINT32		FreqOffset;//discuss.
		SINT32		SeekPhaseLoopI;	// Seek control variable SeekPhaseLoopI
		SINT32		SeekPhaseLoopCF;	// Seek control variable
		SINT32		SeekFreqHigh;		// Frequency control variable
		SINT32		SeekFreqLow;		// Frequency control variable
	}ParaSeek;
	//Test parameters
	struct
	{
		UINT16			Amplitude1;		// Amplitude 1 of 2
		enum AMPSTEP_COND	AmpStepCondition;
		UINT32 			AmplitudeProfileTime;
		UINT16 			Amplitude2;		// Amplitude 2 of 2
		UINT16 			AmplitudeStartRampTime;
		UINT16 			AmplitudeProfileRampTime;
		SINT32			AmplitudeLoopC1;	// Amplitude control loop variable C1
		SINT32			AmplitudeLoopC2;	// Amplitude control loop variable C2
		SINT32			FreqOffset;
		SINT32			PhaseLoopI;
		SINT32			PhaseLoopCF;
		SINT32			WeldFreqHigh;		// Frequency control variable
		SINT32			WeldFreqLow;		// Frequency control variable
		UINT32 			TimeErrorHigh;
	}ParaTest;
	//Scan parameters
	struct
	{
		SINT32 FrequencyStart;
		SINT32 FrequencyStop;
		SINT32 FrequencyStep;
		SINT32 TimeDelay;
		UINT16 MaxAmplitude;
		UINT16 MaxCurrent;
		UINT32 TimeErrorHigh;
	}ParaScan;
	SINT32 DigitalTuneScaledUP;				//from horn scan or from webpage
	SINT32 MemoryOffsetScaledUP;			//Seek/scan/store at end weld frequency minus Digital Tune
	UINT32 CRChsp;
}HSPRESET;


struct HspresetRls6
{
	//Seek parameters
	struct
	{
		UINT16		SeekTime;		// Length of time for seeking
		UINT16		SeekAmp;		// Amplitude for seek
		UINT16		SeekRampTime;		// Ramp time for seek
		SINT32		SeekAmpLoopC1;	// Control variable
		SINT32		SeekAmpLoopC2;	// Control variable
		SINT32		FreqOffset;//discuss.
		SINT32		SeekPhaseLoopI;	// Seek control variable SeekPhaseLoopI
		SINT32		SeekPhaseLoopCF;	// Seek control variable
		SINT32		SeekFreqHigh;		// Frequency control variable
		SINT32		SeekFreqLow;		// Frequency control variable
	}ParaSeek;
	//Test parameters
	struct
	{
		UINT16			Amplitude1;		// Amplitude 1 of 2
		enum AMPSTEP_COND	AmpStepCondition;
		UINT32 			AmplitudeProfileTime;
		UINT16 			Amplitude2;		// Amplitude 2 of 2
		UINT16 			AmplitudeStartRampTime;
		UINT16 			AmplitudeProfileRampTime;
		SINT32			AmplitudeLoopC1;	// Amplitude control loop variable C1
		SINT32			AmplitudeLoopC2;	// Amplitude control loop variable C2
		SINT32			FreqOffset;
		SINT32			PhaseLoopI;
		SINT32			PhaseLoopCF;
		SINT32			WeldFreqHigh;		// Frequency control variable
		SINT32			WeldFreqLow;		// Frequency control variable
		UINT32 			TimeErrorHigh;
	}ParaTest;
	//Scan parameters
	struct
	{
		SINT32 FrequencyStart;
		SINT32 FrequencyStop;
		SINT32 FrequencyStep;
		SINT32 TimeDelay;
		UINT16 MaxAmplitude;
		UINT16 MaxCurrent;
		UINT32 TimeErrorHigh;
	}ParaScan;
	UINT32 CRChsp;
	UINT8  Reserved[256];
};

//preset need to be transferred to DCP
struct DCPPreset
{
   DCPPresetDCXPS WeldParam;//for weld function
	UINT16    PeakPwrNeeded;
	UINT16 	 PeakPwrCutoff;
	UINT32 	 EnergyNeeded;
	UINT32 	 EnergyCutoff;
	/***********************/
	UINT16    TimeNeeded;
	UINT16    TimeCutoff;
	SINT32    FreqLowCutoff;
	SINT32    FreqHighCutoff;
	UINT8  	  Amplitude1;
	UINT8	  Amplitude2;
	UINT16    StepAtTime;
	UINT32    StepAtEnergy;
	UINT16    StepAtPower;
	UINT16    ExtraFlags;  //send EB Status(Req or not) to DCP(More information may be added in future)
	/***********************/
	UINT32    WeldPresetCrc;
	HSPRESET  HsParam;//for horn function
};

//Weld status. saved after weld
struct WeldStatus
{
	//WC weld result
	UINT32 CycleCounter;
	union{
		UINT32 Date;
		struct{
		UINT8 WeekDay;
		UINT8 Day;
		UINT8 Month;
		UINT8 Year;
		}RTDate;
	}RTCDate;
	union{
		UINT32 Time;
		struct{
		UINT8 TenMilliSecond;
		UINT8 Second;
		UINT8 Minute;
		UINT8 Hour;
		}RTTime;
	}RTCTime;
	UINT16 WeldTime;
	UINT32 WeldEnergy;
	UINT8 PeakPower;
	UINT16 CustomAIn1;
	UINT8 Amplitude_1;
	UINT8 Amplitude_2;
	UINT8 PresetNumber;
	UINT16 WeldStartFrequency;
	UINT16 WeldStopFrequency;
	UINT16 FrequencyChange;
	SINT32 ErrorPsv;
	UINT32 ErrorReason;
	//extra for FB interface
	UINT16 HoldTime;
	UINT32 AvgPower;
	UINT8 AvgAmplitude1;
	UINT8 AvgAmplitude2;
	UINT32 RecalledResonanceFrequencyAbs;
	SINT32 StoredFrequencyAbs;
	ResFrequencyOK ResFreqOK;
	SINT32 EndAmplitudeSet;
	//Inst
	SINT32 EndAmplitude;
	SINT32 EndPower;
	SINT32 EndCurrent;
	SINT32 EndPhase;
	SINT32 EndTemperature;
	//reseved for Counters and errors for FB interface.
	UINT32 FBAlarms[MAX_FBALARMS];
	UINT32 ErrorCodeFB;
};

//Horn specific status set
struct HSSTATUS
{
	HSPRESET HsPreset;
	union{
		UINT32 Date;
		struct{
		UINT8 WeekDay;
		UINT8 Day;
		UINT8 Month;
		UINT8 Year;
		}RTDate;
	}RTCDate;
	union{
		UINT32 Time;
		struct{
		UINT8 TenMilliSecond;
		UINT8 Second;
		UINT8 Minute;
		UINT8 Hour;
		}RTTime;
	}RTCTime;
	UINT32 FBAlarms[MAX_FBALARMS];
	UINT32 ErrorCodeFB;
	union
	{
		UINT32 Status[MAX_HSSTATUSDWORDS];
		struct
		{
			UINT32 Time;
			UINT32 AvgAmplitude;
			UINT32 RecalledMidBandFreqAbs;
			SINT32 SeekStartFrequencyAbs;
			SINT32 SeekStopFrequencyAbs;
			SINT32 SeekMemoryFrequencyAbs;
			ResFrequencyOK ResFreqOK;
			SINT32 EndSetAmplitude;
			//Inst board
			SINT32 EndAmplitude;
			SINT32 EndPwm;
			SINT32 EndPower;
			SINT32 EndCurrent;
			SINT32 EndPhase;
			SINT32 EndTemperature;
			SINT32 Reserved;
		}SeekStatus;
		struct
		{
			UINT32 Time;
			UINT32 AvgAmplitude1;
			UINT32 AvgAmplitude2;
			UINT32 RecalledResFreqAbs;
			ResFrequencyOK ResFreqOK;
			SINT32 TestStartFrequencyAbs;
			SINT32 TestStopFrequencyAbs;
			SINT32 EndAmplitudeSet;//set value for amplitude loop
			//Inst board
			SINT32 EndAmplitude;
			SINT32 EndPwm;
			SINT32 EndPower;
			SINT32 EndCurrent;
			SINT32 EndPhase;
			SINT32 EndTemperature;
			SINT32 Reserved;
		}TestStatus;
		struct
		{
			UINT32 Time;
			SINT32 ScanStartFrequency;
			SINT32 ScanStopFrequency;
			//Inst
			SINT32 EndAmplitude;
			SINT32 EndPwm;
			SINT32 EndPower;
			SINT32 EndCurrent;
			SINT32 EndPhase;
			SINT32 EndTemperature;
			SINT32 Reserved[6];
		}ScanStatus;
	}HsStatus;
};

//weld status set
struct WSTATUS
{
	DcxPreset DcxPs;
	WeldStatus Wstat;
};

enum FreqDependentParamenum{
	WELDFREQLOWCUTOFF, //Weld Freq Low Cutoff
	WELDFREQHIGHCUTOFF,//Weld Freq High Cutoff
	WELDFREQLOW, //Weld Freq Low Overload limit
	WELDFREQHIGH,//Weld Freq High Overload limit
	WELDFREQOFFSET,//Weld Freq Offset
	//WELD Standard
	WELDAMPLITUDELOOPC1,//Weld Amp Loop C1 std
	WELDAMPLITUDELOOPC2,//Weld Amp Loop C2 std
	WELDPHASELOOPI,//Weld Phase Loop I std
	WELDPHASELOOPCF,//Weld Phase Loop CF std
	//WELD PI
	PIWELDAMPLITUDELOOPC1,//Weld Amp loop C1 for PI
	PIWELDAMPLITUDELOOPC2,//Weld Amp Loop C2 for PI
	PIWELDPHASELOOPI, // Weld Phase Loop I for PI
	PIWELDPHASELOOPCF,//Weld Phase Loop CF for PI
	PIWELDPHASELOOPP,// Weld Phase Proportional for PI
	PIFREQWINDOWSIZE,//Weld Freq Window for PI
	//Seek
	SEEKFREQLOW,//Seek Freq Low Limit
	SEEKFREQHIGH,//Seek Freq High Limit
	SEEKFREQOFFSET,//Seek Freq Offset
	//Seek Std
	SEEKAMPLITUDELOOPC1,//Seek Amp Loop C1 std
	SEEKAMPLITUDELOOPC2,//Seel Amp Loop C2 std
	SEEKPHASELOOPI,//Seek Phase Loop I std
	SEEKPHASELOOPCF,//Seek Phase Loop CF std
	//Test
	TESTFREQLOW,//Test Freq Low Limit
	TESTFREQHIGH,//Test Freq High Limit
	TESTFREQOFFSET,//Test Freq Offset
	//Test Std
	TESTAMPLITUDELOOPC1,//Weld Amp Loop C1 std
	TESTAMPLITUDELOOPC2,//Weld Amp Loop C2 std
	TESTPHASELOOPI,//Weld Phase Loop I std
	TESTPHASELOOPCF,//Weld Phase Loop CF std
	//Scan
	SCANFREQUENCYSTART,
	SCANFREQUENCYSTOP,
	SCANFREQUENCYSTEP,
	SCANTIMEDELAY,
	SCANMAXAMPLITUDE,
	SCANMAXCURRENT,
	DIGITALTUNE,
	MEMOFFSET,
	TOTALFREQPARAM,
};
#define MINVALUEINDEX 0
#define MAXVALUEINDEX 1
#define DEFAULTVALUEINDEX 2
#define TOTALVALUES 3

extern SINT32 FreqDependentParam[TOTALFREQPARAM][TOTALVALUES];

//Memory frequency plus midband result of seek,test or scan
extern SINT32   StoredFrequencyAbs[MAX_HSSTATUS_SET + 1];
extern HSSTATUS HsStatusSet[MAX_HSSTATUS_SET + 1][MAX_HS_FUNC];
extern WSTATUS WeldStatusSet[MAX_STATUS_SET + 1];

BOOLEAN SavePreset(PRESET_NUMBER PresetNum, HSPRESET_NUMBER HspsNum);
BOOLEAN RecallPreset(PRESET_NUMBER PresetNum);
BOOLEAN RecallAndVerifyPreset(PRESET_NUMBER PresetNum);

extern UINT32 AvgPowerCount, AvgPower;
extern UINT32 AvgAmplitude, SeekAmpCount;

void SetDefaultValues(DEFAULT_PSCMD DefaulPSCmd = DEFAULT_ALLPS, PRESET_NUMBER PresetNum = CurrPreset);
void SetDefaultCutOffValues(void);

void SetMaxMinValues(PSUPDATESPECIAL PresetSpcl = UPDATENOSPECIALPS);
void SetColdStartPresetValues(PSUPDATESPECIAL PresetSpcl = UPDATENOSPECIALPS);

void PrepareDCPPreset(BOOLEAN MFState = FALSE);
void CalculateAndSavePresetCRC(SAVECRC_CMD CrcCmd = SAVECRC_ALL, PRESET_NUMBER PresetNum = CurrPreset ,
		HSPRESET_NUMBER HspresetNum = CurrHsPreset, BOOLEAN CheckForPresetChange = TRUE);
BOOLEAN CheckPresetCRC(PRESET_NUMBER PresetNum = CurrPreset);
SINT8 * AutoNamePreset(void);
void SaveHSStatusSet(HornFunction HSFunc);
void SaveWeldStatusSet(void);

void SetDefaultAlarms(void);
void SetDefaultSystem(void);
void SetDefaultSecretValues(void);
void UpdatePresetsOnLoopSpeedSwitching(void);
void SetDefaultRFswitch(void);
void SetDefaultAdvRnd(void);
void SetDefaultMACCHIPRWData(void);
void SetDefaultCycleParam(void);
void SetMinMaxCycleParam(void);
void ClearAllCounters(void);
BOOLEAN VerifyPresetFromWebPage(PRESET_NUMBER PresetNum, HSPRESET_NUMBER HsPresetNum);
void InitFreqDependentParametersMinMaxDefault();
void InitFreqAndLoopspeedDependantLoopConstantsMinMaxDefault();
void SetHDDefaults();
void ClearMemoryFrequencyAtPowerup(void);
extern BOOLEAN SendToDCXM[MAX_TOTAL_PRESETS];
#define HSPSCRCDATA_SIZE (sizeof(HSPRESET) - sizeof(RunningHSPreset.CRChsp) \
				 - sizeof(RunningHSPreset.DigitalTuneScaledUP) - sizeof(RunningHSPreset.MemoryOffsetScaledUP))
#endif
