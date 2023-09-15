/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/WebSite.cpp_v   1.57   Mar 06 2017 15:42:08   EGUAJARDO  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/WebSite.cpp_v  $
 * 
 *    Rev 1.57   Mar 06 2017 15:42:08   EGUAJARDO
 * Removed EthIPConfig variable due is not used anymore.
 * 
 *    Rev 1.56   Feb 08 2017 15:30:38   EGUAJARDO
 * Corrected message on Adv. diagnostics page for test, allowed IP and fieldbus adresses to be default after a cold start and also allowed to cause an event when hitting restore defaults on System page.
 * 
 *    Rev 1.55   02 Sep 2016 11:46:40   EGUAJARDO
 * Modified Get and Post data hanldlers to add Test Amplitude data to the Amplitude page.
 * 
 *    Rev 1.54   30 Jul 2015 08:14:24   RJamloki
 * WeldSetup page handler functions changed for Set Digital Tune With HornScan option added in weld setup page
 * 
 *    Rev 1.53   29 Jul 2015 07:57:36   RJamloki
 * WeldSetup page handler functions changed for Clear Memory with reset and clear memory before seek options.
 * 
 *    Rev 1.52   04 Mar 2015 08:24:18   amaurya
 * Logging event on communication failure
 * 
 *    Rev 1.51   16 Apr 2014 10:45:36   amaurya
 * Changed to make Diagnostic-UserIO page accessible when system is in error state.
 * 
 *    Rev 1.50   24 Mar 2014 14:09:32   rjamloki
 * Filed Service Event Data Fix, Setting memory offset to zero on digital tune change. Corrected the Diagnostic buffers.
 * 
 *    Rev 1.49   21 Mar 2014 09:20:30   rjamloki
 * Tracker issue fixes.
 * 
 *    Rev 1.48   13 Mar 2014 15:55:32   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.47   06 Mar 2014 07:30:56   rjamloki
 * Changes for DCP preset update.
 * 
 *    Rev 1.46   04 Mar 2014 07:49:36   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
 * 
 *    Rev 1.45   06 Dec 2013 09:41:28   rjamloki
 * S and V loop constants incorporated.
 * 
 *    Rev 1.44   02 Dec 2013 07:34:20   rjamloki
 * Changes for EthIP Conformance Test.
 * 
 *    Rev 1.43   20 Nov 2013 06:06:42   rjamloki
 * Profibus default address changed. Watchdog default timeout changed.
 * 
 *    Rev 1.42   15 Nov 2013 05:54:36   rjamloki
 * Export log fixes, Preready clearing the web command for statemachine, set up alarm implemented for invalid preset.
 * 
 *    Rev 1.41   14 Nov 2013 09:55:16   rjamloki
 * Tracker issue fixes.
 * 
 *    Rev 1.40   08 Nov 2013 09:03:10   rjamloki
 * LCD coldstart fix. removed powerup counter, Alarm log export data header fix.
 * 
 *    Rev 1.39   06 Nov 2013 08:48:34   ygupta
 * Sending Freq Offset to DCP on Prset Update state.
 * 
 *    Rev 1.38   06 Nov 2013 06:20:44   rjamloki
 * Going to preready on user input configuration change to prevent sonics.
 * 
 *    Rev 1.37   25 Oct 2013 11:59:20   ygupta
 * Disabled Nagle Algo on http connection, Added JSONStopDCX Data service, Tracker fixes, FinalData State renamed to CycleAbort State.
 * 
 *    Rev 1.36   15 Oct 2013 07:42:24   rjamloki
 * Tracker Fix, JSON Fix, Link Status debouncing configurable from Adv R&D page.
 * 
 *    Rev 1.35   08 Oct 2013 06:17:18   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.34   03 Oct 2013 04:26:48   rjamloki
 * Watchdog related Changes
 * 
 *    Rev 1.33   01 Oct 2013 03:16:40   ygupta
 * Change for fieldbus version.
 * 
 *    Rev 1.32   24 Sep 2013 06:28:52   rjamloki
 * Watch dog removed.
 * 
 *    Rev 1.31   20 Sep 2013 07:00:26   rjamloki
 * Added changes to build bootloader with same CyGOS.
 * 
 *    Rev 1.30   04 Sep 2013 13:11:44   rjamloki
 * Removed Test Offset
 * 
 *    Rev 1.29   03 Sep 2013 18:19:30   ygupta
 * Moved to tip from 1.27.1.23.1.3
 * 
 *    Rev 1.27.1.23.1.3   03 Sep 2013 11:14:18   amaurya
 * Tracker fixes and Synchronization fixes for User Outputs.
 * 
 *    Rev 1.27.1.23.1.2   28 Aug 2013 12:07:14   amaurya
 * Fixed tracker issue along Restore Default of Advanced Diagnostics.
 * 
 *    Rev 1.27.1.23.1.1   26 Aug 2013 09:03:06   amaurya
 * Fixed issue of load preset at start and tracker issue.
 * 
 *    Rev 1.27.1.23.1.0   24 Aug 2013 17:14:26   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.27.1.23   02 Aug 2013 10:55:58   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.27.1.22   18 Jul 2013 12:21:28   amaurya
 * Frequency output fixed.
 * 
 *    Rev 1.27.1.21   17 Jul 2013 06:23:18   amaurya
 * Fixed WeldTime Issue on History page and added diagnostic code.
 * 
 *    Rev 1.27.1.20   10 Jul 2013 07:53:18   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.27.1.19   08 Jul 2013 08:57:12   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.27.1.18   27 Jun 2013 05:58:24   amaurya
 * System crash fixes.
 * 
 *    Rev 1.27.1.17   14 Jun 2013 11:21:18   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.27.1.16   11 Jun 2013 09:53:24   amaurya
 * MH round I testing fixes.
 * 
 *    Rev 1.27.1.15   07 Jun 2013 10:21:06   amaurya
 * MH testing round I fixes.
 * 
 *    Rev 1.27.1.14   31 May 2013 11:51:52   amaurya
 * Fixed Login Issue.
 * 
 *    Rev 1.27.1.12   28 May 2013 07:45:12   amaurya
 * Fixed issues of Level II testing.
 * 
 *    Rev 1.27.1.11   24 May 2013 10:18:26   ygupta
 * Fixed Tracker Issue.
 * 
 *    Rev 1.27.1.10   22 May 2013 10:41:50   ygupta
 * Fixes for Disable Stepping in while in after burst, energy braking and scrub time.
 * 
 *    Rev 1.27.1.9   21 May 2013 12:26:12   amaurya
 * Code review fixes.
 * 
 *    Rev 1.27.1.8   07 May 2013 10:16:00   amaurya
 * Code review fixed.
 * 
 *    Rev 1.27.1.7   06 May 2013 09:22:44   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.27.1.6   30 Apr 2013 11:23:34   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.27.1.5   26 Apr 2013 11:04:52   amaurya
 * Fixed traker issue for Ver2.0U.
 * 
 *    Rev 1.27.1.4   23 Apr 2013 10:26:02   ygupta
 * Fixed issue of System Test page on website.
 * 
 *    Rev 1.27.1.3   22 Apr 2013 11:34:26   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.27.1.2   16 Apr 2013 11:21:02   amaurya
 * Fixed tracker issue of Ver2.0R.
 * 
 *    Rev 1.27.1.1   08 Apr 2013 08:26:26   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.27.1.0   20 Mar 2013 09:40:12   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.28   20 Mar 2013 08:20:04   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.27   15 Mar 2013 10:54:38   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.26   11 Mar 2013 02:30:44   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.25   18 Feb 2013 03:12:52   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.24   29 Jan 2013 02:25:02   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.23   22 Jan 2013 10:38:46   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.22   17 Jan 2013 07:27:30   ygupta
 * Profibus Related changes
 * 
 *    Rev 1.21   16 Jan 2013 06:31:24   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.20   11 Jan 2013 02:56:12   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.19   11 Jan 2013 01:52:08   ygupta
 * Stroke length and Amplitude Default Changed
 * 
 *    Rev 1.18   03 Jan 2013 07:19:56   ygupta
 * Modbus Comm problem Resolved
 * 
 *    Rev 1.17   27 Dec 2012 06:46:54   ygupta
 * Changes to fix Issues
 * 
 *    Rev 1.16   18 Dec 2012 07:40:06   ygupta
 * Changes for Multiple Overload alarm in case of 15V lost
 * 
 *    Rev 1.14   17 Dec 2012 02:19:52   ygupta
 * Level 2 passwords and Alarm(WC to DCP Comm) chnged
 * 
 *    Rev 1.13   14 Dec 2012 05:29:54   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.12   13 Dec 2012 00:14:20   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.11   29 Nov 2012 08:49:40   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.10   26 Nov 2012 07:19:44   amaurya
 * Fixed Step distance issue on Amplitue page.
 * 
 *    Rev 1.9   24 Nov 2012 08:07:48   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.8   09 Nov 2012 00:36:50   rjamloki
 * State Machine and website changes
 * 
 *    Rev 1.7   08 Nov 2012 01:22:34   rjamloki
 * Website changes
 * 
 *    Rev 1.6   05 Nov 2012 09:00:58   amaurya
 * State Machine changes
 * 
 *    Rev 1.5   29 Oct 2012 02:58:32   rjamloki
 * Printfs disbaled and changes in website
 * 
 *    Rev 1.4   26 Oct 2012 02:42:22   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.2   02 Jul 2012 13:04:16   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.1   29 Jun 2012 12:17:20   ygupta
 * First Compilable code, First check in after review, Needs more clanup
 * 
 *    Rev 1.0   21 Mar 2012 12:22:48   PDwivedi
 * Initial revision.
 *
 */

#include "WebSite.h"
#include "FileSystem.h"
#include "string.h"
#include "SysConfig.h"
#include "Globals.h"
#include "stdio.h"
#include "stdlib.h"
#include "FUNC.h"
#include "Version.h"
#include "UploadFirmwareTask.h"
#include "DebugNew.h"
#include "LOG.h"
#include "RTOS.h"
#include "UserIO.h"
#include "Board.h"
#include "Flash.h"
#include "CRC.h"
#include "DebugNew.h"
#include "preset.h"	  
#include "portable.h"
#include "WCTask.h"
#include "stdio.h"
#include "GLOBALS.h"
#include "RTClock.h"
#include "HttpServer.h"
#include "DEF_DEF.H"
#include "ModBusSendTask.h"
#include "ReceiveCommandHandler.h"
#include "ProfiBus.h"
#include "rcX_User.h"
#include "cifXEndianess.h"
#include "ProfibusFspms_Public.h"
#include "VerifyPreset.h"
#include "EthernetIP.h"
#include "NRTModbusTask.h"
#include "SysConfig.h"
#include "FieldbusTask.h"
//#define pprintf
#define printf
#define ENERGY_CALCULATION_FACTOR 10
#define MAX_RESPONSE_SIZE 90000//maximum response string length. This does not include html files
#define MAX_LANGUAGESTRING_LEN 100//maximum length of string in English.lan file in www folder

#define WEBSCAN_TIMEOUT  5000000 //in us
#define WEBTEST_TIMEOUT  5000000 //in us
#define WEBDIAG_TIMEOUT  5000000 //in us
#define LOOPBACK_TIMEOUT 5000000 //in us
#define MF_TIMEOUT 5000000 //in us
#define MSINMIN (60 * 1000)

#define NUM_BARGRAPH_SEGMENT 20
#define MIN_BOARD_TESTFREQUENCY (1000)   //in Hz

#define MAXFILELEN 45
//The port this website will run on
#define DCXWEBSITE_PORT 80
//Anlog input values get multiplied by this factor before sending to webpage
#define MI_ANALOGIP_MAXMV 10000

#define FREQ20KHz_START          19475
#define FREQ20KHz_HzPERSEGMENT   50
#define FREQ30KHz_START          29278
#define FREQ30KHz_HzPERSEGMENT   75 //76
#define FREQ40KHz_START          38950
#define FREQ40KHz_HzPERSEGMENT   100
#define TIMESTRINGLEN	20
#define NOON	12
#define ALARMDATAFORMAT_LENGTH		 120

/************Branson Passwords***************/
#define HIDDEN   443336
#define BURNIN   287646
#define BOARD    26273
#define FIELDBUSTEST   328378
#define SCOTT    72688
#define SERVICE  7378423
#define SPECIALS 77324257
#define BAUDUNKNOWN 5
#define BAUDNA		12
#define ADAVANCEDSYS	0
#define PROFIBUSSYS		1
#define ETHERNETIPSYS	2
#define Unassigned  "Unassigned"
#define INTELFORMAT "Intel"
#define MOTOROLAFORMAT "Motorola"
#define UNKNOWN "Unknown"
#define MAXIPSTRLEN  16
#define MAXTEMPSTRLEN 100
#define MAX_TEMPSTRLEN 20

Boardtest Inputs;
SlaveData Object;
SINT8 * Response;
SINT8 LangString[MAX_LANGUAGESTRING_LEN];

//At powerup it should be zero. That means no user has logged in using website.
//It will get assigned a value once the user will login.
SINT8 WebSite::DcxWebUserID[MAX_LEN_USERID + 1];
//At powerup it should be zero.It will be assigned a value in case some
//event is initiated from website. TEST,SCAN other events will be logged with this ID
SINT8 * WebSite::DcxLogUserID;
NetworkConfiguration WebSite::CurrNetworkConfig;
//This array gets filled with Alarm or Event log before sending to website

WebAlarmData * WebAlarmdata;
WebEventData * WebEventdata;
WebHistoryData * WeldHistory;

UINT32 WebSite::Port;      //port this website is running on
UINT32 WebSite::Interface; //ethernet interface;this website is running on

BOOLEAN WebSite::TestStartFlag;
BOOLEAN WebSite::ScanStartFlag;
BOOLEAN WebSite::ScanAbortFlag;
BOOLEAN WebSite::ScanFailedFlag;
BOOLEAN WebSite::diagFlag;
BOOLEAN WebSite::diagStartFlag;
BOOLEAN WebSite::resetFlag;
BOOLEAN WebSite::mfBoardTestFlag;
BOOLEAN WebSite::mfFlag;
BOOLEAN WebSite::mfTestFlag;
UINT32 WebSite::PowerUpId;

SINT32 WebSite::webtimeOutScan;
SINT32 WebSite::webtimeOutTest;
SINT32 WebSite::diagnosticTimeOut;
SINT32 WebSite::StartDiagTimeOut;
SINT32 WebSite::mfBoardTestTimeOut;
SINT32 WebSite::mfTimeOut;

UINT8 DataformatWeb;
SINT8 * WebSite::languageBuff;
#define RESDEFAULT "Unknown Error";

BOOLEAN TempDigOutPutStatus[NUM_DOUT];
SINT32  TempAnalogOutPutVals[NUM_AOUT];
BOOLEAN TokenAvailable = TRUE;		  
UINT8 DefaultSelected = 0;
BOOLEAN ACDInfoUpdated = FALSE;
extern DIGITALIOMAP OutPtr[NUM_FUNC_OUT + 1];
extern SINT16 * HornScanRawPhase;
extern BOOLEAN AlarmFlag, FanStatusFlag, DefaultFanStatus;
extern BOOLEAN RegUpdateCmdFlag;
extern BOOLEAN RegAmplitudeUpdateFlag;
//Once the diagnostic is read FieldbusTask write into this fifo to notify the website module that diagnostic data is available.
Fifo<UINT16> FbDiagQ(1, "FBDQ");
BOOLEAN SendDiagCmd = FALSE;
RTCTime RTCClocks; //object for RTC(PCF8563 Device) Information
USERIOConfiguration TempUSERIOConfig;
USERIOConfiguration CustomDefaultUSERIOConfig;
DcxPreset * TempParameterSet;
UserInfo * TempUserBuff;
extern NetworkConfiguration FBCurrNetworkConfig;
//Digital input function table to be shown on IOConfig page for function selection
//put 0 in high ID if function is only active low.put 0 in low ID if function is only active high
const WebDigitalIOFunctionTable webDigIOfuncTableIn[] = {
   {0,0,0},
   {100, EXTRESET_ACTIVEHIGH, EXTRESET_ACTIVELOW},  //ExtReset
   {101, EXTSEEK_ACTIVEHIGH,  0},  //External Seek
   {102, EXTSTART_ACTIVEHIGH, 0},  //ExtStart
   {103, EXTTEST_ACTIVEHIGH,  0},   //ExtTest
   {104, MEMCLEAR_ACTIVEHIGH, MEMCLEAR_ACTIVELOW},  //memClear
   {105, STARTHORNSCAN_ACTIVEHIGH, 0}, //ExtScan
   {106, LOCKOUT_ACTIVEHIGH, LOCKOUT_ACTIVELOW},    //LockOutChangeLCD
   {107, CABLEDETECT_ACTIVEHIGH, 0},                //Input signal to prevent accidental sonics
   {108, RECALLPRESET_1_ACTIVEHIGH, 0},
   {109, RECALLPRESET_2_ACTIVEHIGH, 0},
   {110, RECALLPRESET_4_ACTIVEHIGH, 0},
   {111, RECALLPRESET_8_ACTIVEHIGH, 0},
   {112, RECALLPRESET_16_ACTIVEHIGH, 0},
   {113, RECALLPRESET_32_ACTIVEHIGH, 0},
   {114, EXTSONICDELAY_ACTIVEHIGH, EXTSONICDELAY_ACTIVELOW},
   {115, SONICDISABLE_ACTIVEHIGH, SONICDISABLE_ACTIVELOW},
   {116, INTERLOCKINPLACE_ACTIVEHIGH, INTERLOCKINPLACE_ACTIVELOW},
   {117, LOADNEWPRESET_ACTIVEHIGH, LOADNEWPRESET_ACTIVELOW},
   {118, CYCLEABORT_ACTIVEHIGH, CYCLEABORT_ACTIVELOW},
   {119, EXTAMPL_STEPTRIGG_ACTIVEHIGH, EXTAMPL_STEPTRIGG_ACTIVELOW},
   //{120, GOINTOHORNDOWN_ACTIVEHIGH, GOINTOHORNDOWN_ACTIVELOW},
   {121, RFSTATUSFEEDBACK_ACTIVEHIGH, RFSTATUSFEEDBACK_ACTIVELOW},
   {122, RF_BITZERO_FEEDBACK_ACTIVEHIGH, 0},
   {123, RF_BITONE_FEEDBACK_ACTIVEHIGH, 0},
   {124, RF_BITTWO_FEEDBACK_ACTIVEHIGH, 0},
   {125, RF_BITTHREE_FEEDBACK_ACTIVEHIGH, 0},
   {126, PARTINPLCAE_ACTIVEHIGH, PARTINPLACE_ACTIVELOW},
   {127, STARTCYCLE_ACTIVEHIGH, 0},
   {128, UPPERLIMITSWITCH_ACTIVEHIGH, UPPERLIMITSWITCH_ACTIVELOW},
   {129, GROUNDDETECT_ACTIVEHIGH, GROUNDDETECT_ACTIVELOW},
   {130, ACTUATORPRESENT_ACTIVEHIGH, ACTUATORPRESENT_ACTIVELOW},
   {131, TRIGGERSWITCH_ACTIVEHIGH, 0}
};

//Digital output function table to be shown on IOConfig page for function selection
//put 0 in high ID if function is only active low.put 0 in low ID if function is only active high
const WebDigitalIOFunctionTable webDigIOfuncTableOut[] = {
   {0,0,0},
   {200, READY_ACTIVEHIGH, READY_ACTIVELOW},             //ReadyFlg
   {201, SONICACTIVE_ACTIVEHIGH, SONICACTIVE_ACTIVELOW}, //WeldOn
   {202, GENALARM_ACTIVEHIGH, GENALARM_ACTIVELOW},       //GenAlarm
   {203, OVERLOADALARM_ACTIVEHIGH, OVERLOADALARM_ACTIVELOW},  //Overload alarm
   {204, SEEKOUT_ACTIVEHIGH, SEEKOUT_ACTIVELOW},         //CycleOK
//   {205, AMPLDECAY_ACTIVEHIGH, AMPLDECAY_ACTIVELOW},     //Amplitude decay
   {206, HOLDTIME_ACTIVEHIGH, HOLDTIME_ACTIVELOW},
   {207, AFTERBUST_DELAY_ACTIVEHIGH, AFTERBUST_DELAY_ACTIVELOW},
   {208, AFTERBUST_TIME_ACTIVEHIGH, AFTERBUST_TIME_ACTIVELOW},
   {209, PLUSTIME_LIMITALARM_ACTIVEHIGH, PLUSTIME_LIMITALARM_ACTIVELOW},
   {210, MINUSTIME_LIMITALARM_ACTIVEHIGH,MINUSTIME_LIMITALARM_ACTIVELOW},
   {211, PLUSENERGY_LIMITALARM_ACTIVEHIGH, PLUSENERGY_LIMITALARM_ACTIVELOW},
   {212, MINUSENERGY_LIMITALARM_ACTIVEHIGH,MINUSENERGY_LIMITALARM_ACTIVELOW},
   {213, PLUSPKPOWER_LIMITALARM_ACTIVEHIGH, PLUSPKPOWER_LIMITALARM_ACTIVELOW},
   {214, MINUSPKPOWER_LIMITALARM_ACTIVEHIGH,MINUSPKPOWER_LIMITALARM_ACTIVELOW},
   {215, STATUS_ACTIVEHIGH, 0},
   {216, CONFIRM_PRESETCHANGE_ACTIVEHIGH, 0},
   {217, ACTUATORHOME_ACTIVEHIGH, ACTUATORHOME_ACTIVELOW},
   {218, WELDCYCLE_COMPLETE_ACTIVEHIGH, WELDCYCLE_COMPLETE_ACTIVELOW},
  // {219, PRELOADED_CYCCOUNT_REACHED_ACTIVEHIGH, PRELOADED_CYCCOUNT_REACHED_ACTIVELOW},
   //{220, INHORNDOWN_ACTIVEHIGH, INHORNDOWN_ACTIVELOW},
   {221, ENDOFHOLDTIME_ACTIVEHIGH, ENDOFHOLDTIME_ACTIVELOW},
   {222, STARTSIGNAL_RELEASE_ACTIVEHIGH, STARTSIGNAL_RELEASE_ACTIVELOW},
   {223, CYCLECOKAY_ACTIVEHIGH, CYCLECOKAY_ACTIVELOW},
   {224, RF_BITZERO_SELECT_ACTIVEHIGH, 0},
   {225, RF_BITONE_SELECT_ACTIVEHIGH, 0},
   {226, RF_BITTWO_SELECT_ACTIVEHIGH, 0},
   {227, RF_BITTHREE_SELECT_ACTIVEHIGH, 0},
   {228, CYCLESTARTOUT_ACTIVEHIGH, CYCLESTARTOUT_ACTIVELOW},
   {229, AMPL1_AMPL2, 0},
   {230, CUSTOM_ALARMOUTPUT_ACTIVEHIGH, CUSTOM_ALARMOUTPUT_ACTIVELOW},
};

//Analog input function table to be shown on IOConfig page for function selection
const WebAnalogIOFunctionTable webAnaIOfuncTableIn[] = {
   {0,0},
   {300, FREQ_CONTROL}, //freqin
   {301, AMPL_CONTROL}, //amplitudein
   {302, CUSTOM_INPUT1},
   {303, CUSTOM_INPUT2}
};

//Analog output function table to be shown on IOConfig page for function selection
const WebAnalogIOFunctionTable webAnaIOfuncTableOut[] = {
   {0,0},
   {400, POWER_OUT},    //powerout
   {401, AMPL_OUT},     //amplitude out
   {403, FREQ_OUT},     //frequencyOut
  // {404, AUTOMATION_OP1}// AUTOMATION OP1
};

/* Website command handlers array */
const webSiteCommand webCommandHandlers[] =
{
   {0},
   {WebSite::SetupWeldGetDatahandler},          //1  // Website new command handlers
   {WebSite::SetupWeldPostDatahandler},         //2
   {WebSite::SetupLimitsGetDatahandler},        //3
   {WebSite::SetupLimitsPostDatahandler},       //4
   {WebSite::SetupSaveRecallGetDatahandler},    //5
   {WebSite::SetupSaveRecallPostDatahandler},   //6
   {WebSite::SetupAmplitudeGetDatahandler},     //7
   {WebSite::SetupAmplitudePostDatahandler},    //8
   {WebSite::SetupSeekPowerUpGetDatahandler},   //9
   {WebSite::SetupSeekPowerUpPostDatahandler},  //10
   {WebSite::SetupAdvancedGetDatahandler},      //11
   {WebSite::SetupAdvancedPostDatahandler},     //12
   {WebSite::ViewSystemInfoGetDatahandler},     //13
   {WebSite::ProcessScanCommand},               //14
   {WebSite::ViewAlarmLogGetDatahandler},       //15
   {WebSite::ViewEventLogGetDatahandler},       //16
   {WebSite::ViewHistoryGetDatahandler},        //17
   {WebSite::ViewWeldGraphGetDatahandler},      //18
   {WebSite::ViewWeldGraphPostDatahandler},     //19
   {WebSite::DiagnosticSeekGetDatahandler},     //20
   {WebSite::DiagnosticSeekPostDatahandler},    //21
   {WebSite::DiagnosticHornSignatureGetDatahandler},  //22
   {WebSite::DiagnosticHornSignaturePostDatahandler}, //23
   {WebSite::DiagnosticUserIOGetDatahandler},   //24
   {WebSite::DiagnosticUserIOPostDatahandler},  //25
   {WebSite::DiagnosticFieldBusGetDatahandler}, //26
   {WebSite::ConfigurationSystemGetDatahandler},//27
   {WebSite::ConfigurationSystemPostDatahandler},   //28
   {WebSite::ConfigurationUserIOGetDatahandler},//29
   {WebSite::ConfigurationUserIOPostDatahandler},//30
   {WebSite::ConfigurationIPSetupGetDatahandler},//31
   {WebSite::ConfigurationIPSetupPostDatahandler},  //32
   {WebSite::ConfigurationPasscodesGetDatahandler}, //33
   {WebSite::ConfigurationPasscodesPostDatahandler},//34
   {WebSite::ConfigurationAlarmGetDatahandler}, //35
   {WebSite::ConfigurationAlarmPostDatahandler},//36
   {WebSite::ConfigurationRFSwitchGetDatahandler},  //37
   {WebSite::ConfigurationRFSwitchPostDatahandler}, //38
   {WebSite::BransonSecretValuesGetDatahandler},    //39
   {WebSite::BransonSecretValuesPostDatahandler},   //40
   {WebSite::BransonSystemTestGetDatahandler},      //41
   {WebSite::BransonBoardTestGetDatahandler},       //42
   {WebSite::BransonBoardTestPostDatahandler},      //43
   {WebSite::BransonFieldBusTestGetDatahandler},    //44
   {WebSite::BransonFieldBusTestPostDatahandler},   //45
   {WebSite::BransonAdvRnDTestGetDatahandler},      //46
   {WebSite::BransonAdvRnDTestPostDatahandler},     //47
   {WebSite::BransonFieldServiceGetDatahandler},    //48
   {WebSite::BransonFieldServicePostDatahandler},   //49
   {WebSite::BransonSpecialPresetGetDatahandler},   //50
   {WebSite::BransonSpecialPresetPostDatahandler},  //51
   {WebSite::SetLanguageAndUserID},                 //52
   {WebSite::SetupAdvancedDiagnosticGetDatahandler},//53 new added pages
   {WebSite::SetupAdvancedDiagnosticPostDatahandler},//54
   {WebSite::GetWebToken},                          //55
};

/*  SINT32 GetContentType(SINT8 * FileName)
 *
 *  Purpose:
 *    This function returns the type of file requested from website.This function gets called from WebGetHandler function.
 *
 *  Entry condition: FileName- The file Name with extension whose type is to be determined.
 *
 *  Exit condition: Type of file image.gtml or gif etc.
 */
SINT32 GetContentType(SINT8 * FileName)
{
   SINT8 * Ext = strchr(FileName, '.');
   SINT8 * Ext1 = Ext;
   SINT32 Ret = texthtml;
   while(Ext){
      Ext1 = Ext;
      Ext = strchr(Ext + 1, '.');
   }
   if(!strcmp(Ext1,".css"))   Ret = textcss;
   else if(!strcmp(Ext1,".gif")) Ret = imagegif;
   else if(!strcmp(Ext1,".jpg")) Ret = imagejpg;
   else if(!strcmp(Ext1,".png")) Ret = imagepng;
   else if(!strcmp(Ext1,".js"))  Ret = textjs;
   return Ret;
}

UINT32 CopyLength(UINT32 MaxExpectedLength, UINT32 ActualLength, int line){
	UINT32 RetVal = ActualLength;
	line = 0;
	if(ActualLength > MaxExpectedLength){
		RetVal = MaxExpectedLength;
		//CyGOS::WriteSystemEvent(STRLENGTH_ERROR, ActualLength, MaxExpectedLength, line, true);
	}
	return RetVal;
}
/*  UINT32 GenerateKey(void)
 *
 *  Purpose:
 *    This function is Called by Gethandler. It generates a random number at power up,
 *    which is set in client cookie at the time of login. Each time system powered up this unique number &
 *    User login, if this unique number doesn't match the client's cookies then user need to
 *    login again.
 *
 *
 *  Entry condition:
 *
 */
UINT32 WebSite::GenerateKey(void){
	UINT32 RandomNum = SysCounters->LifeTimeCycleCount + SysCounters->PowerOnHours +
			SysCounters->SonicsOnHours +  SysCounters->GeneralAlarmCount;
	if(RandomNum == 0)
		RandomNum = 0xAA775511;
   return RandomNum;
}
/*  void WebSite::PostResponse(SINT8 * Uri , UINT32 UserPrivilege)
 *
 *  Purpose:
 *    This function is Called by Posthandler. It finds out appropriate handler for post request based on function ID and command received in Uri.
 *
 *  Entry condition:
 *    Uri- Identifier of resource. It can be an HTML file or some other data specific to the page on website.
 *    UserPrivilege- user privilege to be filled in response.
 *
 *  Exit condition:
 *   The response data filled in global response array by one of the handlers.
 */
void WebSite::PostResponse(SINT8 * Uri , UINT32 UserPrivilege)
{
    SINT8 * TempStr = 0;
    SINT32 FuncID = 0, WebCmd = 0;
    Response[0] = 0;
    UpdateUser(UserPrivilege);
    TempStr = strstr(Uri , "func=");
    if(TempStr){
    	TempStr = strchr(TempStr, '=') + 1;
    	FuncID = atoi(TempStr);
    	TempStr = strstr(Uri , "cmd=");
    	if(TempStr){
			TempStr = strchr(TempStr, '=') + 1;
			WebCmd = atoi(TempStr);
			TempStr = strstr(Uri , "param=");
			if(TempStr){
				TempStr = strchr(TempStr, '=') + 1;
				if(((UINT32)FuncID > 0) && ((UINT32)FuncID < (sizeof(webCommandHandlers)/sizeof(webCommandHandlers[0])))){
				   if(webCommandHandlers[FuncID].handler)
					  webCommandHandlers[FuncID].handler(TempStr , (WebCommand)WebCmd);
				}
			}
    	}
    }
}

/*  UINT32 IPStrtoUint(SINT8 * IpStr)
 *
 *  Purpose:
 *    This function converts the IP in dotted string format i.e( xx.xx.xx.xx) into integer format.
 *    This function gets called form ConfigurationIPSetupPostDatahandler() and ProcessMFSavesystemsetupCommand().
 *
 *  Entry condition: IpStr- IP Address.
 *
 *  Exit condition: IP Address in integer Format.
 */
UINT32 IPStrtoUint(SINT8 * IpStr)
{
   UINT32 Ip = 0;
   Ip = atoi(IpStr) << 24;
   IpStr = strchr(IpStr, '.') + 1;
   Ip |= atoi(IpStr) << 16;
   IpStr = strchr(IpStr, '.') + 1;
   Ip |= atoi(IpStr) << 8;
   IpStr = strchr(IpStr, '.') + 1;
   Ip |= atoi(IpStr);
   return htonl(Ip);
}

/* void WebSite::InitWebSite(SINT32 Intf, SINT32 SrvrPort)
 *
 *  Purpose:
 *    This function assigns the handler functions for HTTP GET and POST requests coming from the
 *    website(HTTP Client on PC) to HTTPServer.This Function should be called once at powerup.
 *
 *  Entry condition: Intf- The index of Ethernet module of processor
 *                   SrvrPort-The HTTP port this website belongs to.
 *
 *  Exit condition: None.
 */
void WebSite::InitWebSite(SINT32 Intf, SINT32 SrvrPort){
   //Add the handlers below. The ethernet interface, port are used by below API’s to identify the HTTPServer.
   //Add Get Handler
	PowerUpId = GenerateKey();
   CyGOS::AddHttpGetHandler(Intf, SrvrPort, WebGetHandler);
   //Add post handler
   CyGOS::AddHttpPostHandler(Intf, SrvrPort, WebPostHandler);
   //Add timer callback for website
   CyGOS::AddTickCallbackForWebSite(Intf, SrvrPort, Tick);
   //Add file upload handler
   CyGOS::AddHttpFileUploadHandler(Intf, SrvrPort, WebFileUploadHandler);
   Port = SrvrPort;
   Interface = Intf;
   TestStartFlag = FALSE;
   ScanStartFlag = FALSE;
   ScanAbortFlag = FALSE;
   webtimeOutScan = WEBSCAN_TIMEOUT;
   webtimeOutTest = WEBTEST_TIMEOUT;
   languageBuff = 0;
   BoardTestFrequency = MIN_BOARD_TESTFREQUENCY;
   CurrNetworkConfig = CyGOS::GetNetworkConfiguration(Interface);
   WebAlarmdata = new WebAlarmData[MAX_ALARM_ENTRIES];
   WebEventdata = new WebEventData[MAX_EVENT_ENTRIES];
   WeldHistory = new WebHistoryData[MAX_WELD_RESULT_ENTRIES];
   Response = new SINT8[MAX_RESPONSE_SIZE];
   TempParameterSet = new DcxPreset;
   TempUserBuff = new UserInfo[MAX_USERS];
   //Save default settings at power up
   TempUSERIOConfig = *USERIOConfig;
   *TempParameterSet = ParameterSet[CurrPreset];
}

/* void WebSite::GetWebToken(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *    This function is used by website module to find out whether token is available or not.
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
void WebSite::GetWebToken(SINT8 * ParamStr, WebCommand Cmd)
{
   if(Cmd == REQ_TOKEN)
      sprintf(&Response[0],"%ld@",(SINT32)TokenAvailable);
   else if(Cmd == REQ_NONE)
	   sprintf(&Response[0],"%ld@",(SINT32)MacchipData.CurrSystemType);		// To set system type on branson tab.
   ParamStr = 0;
}

/* void WebSite::SetLanguageFileBuffer(Language Lang)
 *
 *  Purpose:
 *    Function gets called on every POST request.It opens the Language file and  copy the file contents
 *    in language buffer if the buffer is not already initialized.Function will be handy when we will deal with more languages.
 *
 *  Entry condition: Lang- The language in which language buffer is to be initialized.
 *
 *  Exit condition: None.
 */
void WebSite::SetLanguageFileBuffer(Language Lang)
{
   File * file = 0;
   SINT32 Len = 0;
   if(!languageBuff){
      CyGOS::FileOpen("A:\\ENGLISH.lan", O_RDONLY, &file);
      if(file){
         Len = CyGOS::FileLength(file);
         languageBuff = new SINT8[Len + 1];
         if (CyGOS::FileRead(file, languageBuff, (int*)&Len) == 0)
            CyGOS::FileClose(file);
         languageBuff[Len] = 0;
      }
   }
   Lang = English;
}

/* void SetupMinMaxPresetIndexes(void)
 *
 *  Purpose:
 *    This function is used to decide Preset parameter max & Min values(Whether they would be Tweak based or not).
 *    Function gets called by WebPostHandler function.
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
void SetupMinMaxPresetIndexes(void)
{
	if(Sysconfig->TweakRangeEnabled){
		MinPSNum = Preset_TweakMIN;
		MaxPSNum = Preset_TweakMAX;
		MinHSPSNum = HsPreset_TweakMIN;
		MaxHSPSNum = HsPreset_TweakMAX;
	}
	else{
		MinPSNum = Preset_MIN;
		MaxPSNum = Preset_MAX;
		MinHSPSNum = HsPreset_MIN;
		MaxHSPSNum = HsPreset_MAX;
	}
}

/* BOOLEAN WebSite::WebPostHandler(SINT8 * PostData, SINT8 * Uri, HTTPReplyHeader & Header)
 *
 *  Purpose:
 *    This function is used to handle the post request whether its download request,upload request,html
 *    request or request for some data.This function is called by HTTPServer task on receiving POST request.
 *
 *  Entry condition: request or request for some data.PostData-The data submitted by a web form in a predefined standard format.
 *    Uri-Identifier of resource.It can be an HTML file or some other data  specific to the page on website.
 *    Header-object which will be filled with response length,response type etc.
 *
 *  Exit condition: Returns TRUE in case of valid request otherwise returns FALSE.
 */
BOOLEAN WebSite::WebPostHandler(SINT8 * PostData, SINT8 * Uri, HTTPReplyHeader &Header){
	BOOLEAN RetVal = TRUE;
	BOOLEAN HtmlRequest = (strstr(Uri, ".html") != 0);//see if its html a file request
	BOOLEAN downLoadRequest = (strstr(Uri, ".dcxfiledownload") != 0);//see if its a download request
	BOOLEAN UploadRequest = (strstr(Uri, ".dcxfileupload") != 0);//see if its a upload request
	SINT8 * TempStr = 0, *TempStr1 = 0, *ReqSource, *Res = RESDEFAULT;
	BOOLEAN IdMisMatch = FALSE;
	UINT32 Passcode, Usertype, UniqueId;
	SINT32 LangID = 0, Contentlength = 0;
	SINT8 FileName[50], Userid[20], TempUniqueId[12];
	File * file = 0;
	Response[0] = 0;
	Header.contentLength = strlen(Res);
	Header.reqType = reqData;
	FileName[0] = 0;
	SetupMinMaxPresetIndexes();
	TokenAvailable = IsAccessTokenAvailable();
	if ((UploadRequest == TRUE) || (HtmlRequest == TRUE))
		ReqSource = PostData;
	else
		ReqSource = Uri;
	if (downLoadRequest) { //see if its a download request
		DownLoadRequest(Uri, Header.downloadFileName);
		Res = Response;
		Contentlength = strlen(Res);
		if (Contentlength == 0) { //special case for FRAM(binary) file download
			strcpy(FileName, "B:\\");
			strcat(FileName, "FRAM.cfg");
			CyGOS::FileOpen(FileName, O_RDONLY, &file);
			if (file) {
				Contentlength = CyGOS::FileLength(file);
				Header.reqType = reqBinFileDownload;
			}
			else
				RetVal = FALSE;
		}
		else
			Header.reqType = reqFileDownload;
	}
	else if (UploadRequest) {
		HandleInitiateUpload();
		Res = Response;
		Contentlength = strlen(Res);
		Header.reqType = reqFileUpload;
	}
	else {
		TempStr = strstr(ReqSource, "lang=");//////
		LangID = atoi(strchr(TempStr, '=') + 1);
		SetLanguageFileBuffer(Language(LangID));
		TempStr = strstr(ReqSource, "&usrid=");
		TempStr = strchr(TempStr, '=') + 1;
		TempStr1 = strstr(TempStr, "&");
		strncpy(Userid, TempStr, CopyLength(MAX_LEN_USERID, TempStr1 - TempStr, __LINE__));
		TempStr = strstr(ReqSource, "&passcode=");
		Passcode = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ReqSource, "&uniqueid=");
		TempStr = strchr(TempStr, '=') + 1;
		strncpy(TempUniqueId, TempStr, CopyLength(sizeof(TempUniqueId) - 1, strlen(TempStr), __LINE__));
		UniqueId = atoi(TempUniqueId);
		if((UniqueId != 0) && (UniqueId != PowerUpId) && (IsPasscodeRequired() == TRUE)){
			IdMisMatch = TRUE;
			if(HtmlRequest == FALSE){
				strcpy(Response, "IDMISMATCH");
				Res = Response;
				Header.reqType = reqData;
				Contentlength = strlen(Res);
			}
		}
		Usertype = GetUserPrivilege((UINT8 *) Userid, Passcode);
		if(strlen(Userid) <= MAX_LEN_USERID){
			strcpy(DcxWebUserID , Userid);		
		}

		DcxLogUserID = 0;
		if ((IdMisMatch == FALSE) && (IsPasscodeRequired() == TRUE) && (Usertype == PRIVILEGE_INVALID)) {
			strcpy(Response, "Authentication Failed");
			Res = Response;
			Header.reqType = reqData;
			Contentlength = strlen(Res);
		}
		else if (HtmlRequest) {
			Header.reqType = reqFile;
			strcpy(FileName, "A:\\");
			if (strlen(Uri) > MAXFILELEN) {
				Log::WriteEventLog(EVENT_BIG_FILE_NAME_POST, strlen(Uri));
				RetVal = FALSE;
			}
			else{
				if(IdMisMatch){
				strcat(FileName, "index.html");
				}
				else{
					strcat(FileName, Uri);
				}
				CyGOS::FileOpen(FileName, O_RDONLY, &file);
				if (file)
					Contentlength = CyGOS::FileLength(file);
				else
					RetVal = FALSE;
			}
		}
		else if(IdMisMatch == FALSE){//if its not a file or download request, its a request for some data
			//web page doesn't get refreshed in this case
			Header.reqType = reqData;
			if ((TempStr = strstr(Uri, "&lang=")))
				*TempStr = 0;
			PostResponse(Uri, Usertype);
			Res = Response;
			Contentlength = strlen(Res);
		}
	}
	
	Header.file = file;
	Header.contentLength = Contentlength;
	Header.data = Res;
	return RetVal;
}

/* UINT32 WebSite::GetUserID(void)
 *
 *  Purpose:
 *    This function returns the currently logged in user ID for logging.This function is called by WriteEventLog().
 *
 *  Entry condition: None.
 *
 *  Exit condition: Returns the currently logged in user ID.
 */
SINT8 * WebSite::GetUserID(void){
	static SINT8 * RetVal = "N/A";
	RetVal = "N/A";
	if(DcxLogUserID)
		RetVal = DcxLogUserID;
	return RetVal;
}

/* BOOLEAN WebSite::WebFileUploadHandler(SINT8 * Data, SINT32 DataLen, SINT8 * FileName, SINT8 * Action)
 *
 *  Purpose:
 *    This function handles the file upload request.This function gets called from file upload handler.
 *
 *  Entry condition: Data- pointer to file data.
 *                   DataLen- The length of file data.
 *                   FileName- The file name.
 *						Action- The action requrested by the html page.
 *  Exit condition: Returns true if file is successfully send to Upload Firmware task.
 */
BOOLEAN WebSite::WebFileUploadHandler(SINT8 * Data, int DataLen, SINT8 * FileName, SINT8 * Action)
{
	SINT8 * Ext, * Ext1;
	SINT8 * Buff = new SINT8[DataLen];
	Ext = strchr(FileName, '.');
	Ext1 = Ext;
	while(Ext){
	  Ext1 = Ext;
	  Ext = strchr(Ext + 1, '.');
   }
   FileType Ftype;
   if(!strcmp(Ext1, ".bin"))
		   Ftype =  FileTypeBin;
   else if(!strcmp(Ext1, ".cyg"))
		   Ftype =  FileTypeCyg;
   else if(!strcmp(Ext1, ".cfg"))
		   Ftype =  FileTypeCfg;
   else
	   Ftype =  FileTypeErr;
   memcpy(Buff, Data, DataLen);
   return UploadFirmware::thisPtr->ProgramFirmware(Ftype, Buff, DataLen, Action);
}

/* void FormatCycleResults(void)
 *
 *  Purpose:
 *    This function get called from Manufacturing Webpage.It converts the cycle time
 *    parameters which are in milliseconds to hours and minutes format.It also prepares the response
 *    according to CSV file format. This function gets called from DownLoadRequest function on “save results to file” button from Manufacturing Webpage.
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
void FormatCycleResults(void)
{
	SINT32 Temp, CycTimeHrs, CycTimeMins, SonicOnTimeHrs, SonicOnTimeMins;
  	SINT8 DateBuff[MAX_TIMESTAMP_LENGTH];
	SINT8 TimeBuff[MAX_TIMESTAMP_LENGTH];
	LogTime TM ={0,0,0,0,0,0};
	sprintf(DateBuff, "%02d-%02d-%04d", TM.TmMon, TM.TmDay, TM.TmYear);
	sprintf(TimeBuff, "%02d-%02d-%02d", TM.TmHour, TM.TmMin, TM.TmSec);
	Temp = SCALEDTIMETOWEBSITE(MFcycleResults->CurrCycleTime);
	CycTimeHrs = Temp / NMSINHOUR;
	CycTimeMins = Temp % NMSINHOUR;
	CycTimeMins = CycTimeMins / NMSINMINUTE;
	Temp = SCALEDTIMETOWEBSITE(MFcycleResults->SonicsOnTime);
	SonicOnTimeHrs =  Temp / NMSINHOUR;
	SonicOnTimeMins = Temp % NMSINHOUR;
	SonicOnTimeMins = SonicOnTimeMins / NMSINMINUTE;
   sprintf(Response, "Date,Time,Serial Number,Cycle Time,Cycles Run,Hours Run,Alarms,Frequency,Power\r\n"
        "%s,%s,%s,%2ld:%2ld,%u,%2ld:%2ld,%u,%ld,%ld", DateBuff, TimeBuff,MacchipData.SerialNo, CycTimeHrs, CycTimeMins,
        (unsigned int)MFcycleResults->CyclesCount, (SINT32)SonicOnTimeHrs, (SINT32)SonicOnTimeMins, (unsigned int)ReadManFErrorCounter(),
        (SINT32)MFcycleResults->ResultantFrequency, (SINT32)MFcycleResults->ResultantPower);
}

/* void WebSite::HandleInitiateUpload(void)
 *
 *  Purpose:
 *    The purpose of this function is to initiate upload request by copying READY String into
 *    global Response string.This function gets called from WebPostHandler function.
 *
 *  Entry condition: None.
 *
 *  Exit condition:  Response String prepared with reply.
 */
void WebSite::HandleInitiateUpload(void)
{
	//check if ready for upload
	Response[0] = 0;
	strcpy(Response, "READY");
}

/* void WebSite::DownLoadRequest(SINT8 * Uri, SINT8 * File)
 *
 *  Purpose:
 *  This function handles the file download request.This function gets called from WebPostHandler function.
 *
 *  Entry condition:
 *    Uri- The data submitted by a web form in a predefined standard format.It is used to determine the file to be downloaded.
 *    File- The file name specific to request is filled in this pointer.
 *
 *  Exit condition:
 *  Response String filled with reply.
 */
void WebSite::DownLoadRequest(SINT8 * Uri, SINT8 * File)
{
	SINT8 * TempStr = 0;
	Response[0] = 0;
	WebCommand Cmd = REQ_NONE;
	TempStr = strstr(Uri, "cmd=");
	if(TempStr){
		TempStr = strchr(TempStr, '=') + 1;
		Cmd = (WebCommand)(atoi(TempStr));
	}
	DcxLogUserID = DcxWebUserID;
   switch(Cmd){
      case REQ_DOWNLOAD_FRAM_CONFIGURATION:
//         printf("\n cfg file download request");
         strcpy(File, "FRAM.cfg");
      break;
      case REQ_DOWNLOAD_ALARMDATA:
         ViewAlarmLogGetDatahandler(Response, Cmd);
         strcpy(File, "AlarmLog.csv");
      break;
      case REQ_DOWNLOAD_WCEVENTDATA:
      case REQ_DOWNLOAD_DCPEVENTDATA:
         ViewEventLogGetDatahandler(Response, Cmd);
         strcpy(File, "EventLog.csv");
      break;
      case REQ_DOWNLOAD_CYCLERESULTS:
         FormatCycleResults();
         strcpy(File, "cycleResults.csv");
      break;
      case REQ_DOWNLOAD_WELDHISTORY:
         ViewHistoryGetDatahandler(Response, Cmd);
         strcpy(File, "History.csv");
      break;
      default:
         strcpy(Response, "err,err,err,err\r\nerr,err,err,err\r\n");
         strcpy(File, "Error.csv");
      break;
	}
   DcxLogUserID = 0;
}

const char ConvStr[16] = {'0', '1', '2','3','4','5','6','7','8','9','A', 'B','C', 'D','E','F'};
int inttoascii(unsigned int n, char * Res, int base) {
    int ret;
    int numChars = 0;
    // Count how much space we will need for the string
    unsigned int temp = n;
    do {
        numChars++;
        temp /= base;
    } while ( temp );
    //Allocate space for the string (1 for negative sign, 1 for each digit, and 1 for null terminator)
    //ret = new char[ numChars + 1 ];
    Res[numChars] = 0;
    ret = numChars;
    int i = numChars - 1;
    do {
    	Res[i--] = ConvStr[n%base];// + '0';
        n /= base;
    } while (n);
    return ret;
}
/* BOOLEAN WebSite::WebGetHandler(SINT8 * Uri, HTTPReplyHeader &Header)
 *
 *  Purpose:
 *  This function is called by HTTPServer task on receiving Get request and returns true in case
 *  of valid request otherwise returns false.The function gets called from AddHttpGetHandler.
 *
 *  Entry condition: Uri- The data submitted by a web form in a predefined standard format.
 *                   Header- Object which will be filled with the respose length,reponse type etc.
 *
 *  Exit condition: Returns TRUE in case of valid request otherwise returns FALSE.
 */
BOOLEAN WebSite::WebGetHandler(SINT8 * Uri, HTTPReplyHeader &Header)
{
   BOOLEAN RetVal = FALSE;
   ip_addr TempIP;
   BOOLEAN DefaultFileRequest = FALSE, RAMTp = FALSE, CrashDiag = FALSE, StateInfo = FALSE, ACDInfo = FALSE;//FRAMTp = FALSE;
   SINT8 FileName[50];
   SINT32 Len = 0;
	File * File = 0;
	strcpy(FileName, "A:\\");
   if(strlen(Uri + 1) > MAXFILELEN){
      Log::WriteEventLog(EVENT_BIG_FILE_NAME_GET, strlen(Uri + 1));
	}
   else{
		strcat(FileName, Uri + 1);
		if(*(Uri + 1)){//Indicates there is a request from some specific html file
		   if(strstr(FileName, "UserComment.html")){}
		   else if(strstr(FileName, "FirmwareUpload32923646.html"))
				DefaultFileRequest = TRUE;
		   else if(strstr(FileName, "TP32923646.html"))
			   RAMTp = TRUE;
		   else if(strstr(FileName, "CRD32923646.html"))
			   CrashDiag = TRUE;
		   else if(strstr(FileName, "SM32923646.html"))
			   StateInfo = TRUE;
		   else if(strstr(FileName, "ACDInfo32923646.html"))
			   ACDInfo = TRUE;

//		   else if(strstr(FileName, "FramTP32923646.html"))
//			   FRAMTp = TRUE;
		   else if(strstr(FileName, ".html"))
			   strcpy(FileName, "A:\\index.html");
		}//no file name means the home page request
		else
			strcpy(FileName, "A:\\index.html");

		if(strcmp(Header.cacheReq, MODIFIED_SINCE_STR) == 0){
		   Header.statusCode = httpNotModified;
			RetVal =  TRUE;
		}
		else{
			CyGOS::FileOpen(FileName, O_RDONLY, &File);
			if(File){
			   Len = CyGOS::FileLength(File);
				RetVal = TRUE;
			}
			else if(DefaultFileRequest == TRUE){
			   Len = strlen(FirmwareUpload);
			   RetVal = TRUE;
			   Header.reqType = reqData;
			   Header.data = (SINT8*)FirmwareUpload;
			   pprintf("\nDefalut.html\n");
			}
			else if(RAMTp == true){
				RetVal = TRUE;
				Response[0] = 0;
				Header.reqType = reqData;
				Header.data = (SINT8*)Response;
				char * TempPtr = Response;
				strcpy(TempPtr , "Task:");
				TempPtr += strlen("Task:");
				for(Len = 0; Len < 64; Len++){
					if(TPointsRam.TTP[(Len * 2)] || TPointsRam.TTP[(Len * 2) + 1]){
						strcpy(TempPtr, "<br/>Prio");
						TempPtr += strlen("<br/>Prio");
						TempPtr += inttoascii(Len, TempPtr, 10);
						strcpy(TempPtr, "=");
						TempPtr += strlen("=");
						TempPtr += inttoascii(TPointsRam.TTP[Len * 2], TempPtr, 10);
						strcpy(TempPtr, " , ");
						TempPtr += strlen(" , ");
						TempPtr += inttoascii(TPointsRam.TTP[(Len * 2) + 1], TempPtr, 10);
					}
				}
				strcat(TempPtr , "<br/>Interrupt:");
				TempPtr += strlen("<br/>Interrupt:");
				for(Len = 0; Len < 256; Len++){
					if(TPointsRam.ITP[(Len * 2)] || TPointsRam.ITP[(Len * 2) + 1]){
						strcpy(TempPtr, "<br/>Int");
						TempPtr += strlen("<br/>Int");
						TempPtr += inttoascii(Len, TempPtr, 10);
						strcpy(TempPtr, "=");
						TempPtr += strlen("=");
						TempPtr += inttoascii(TPointsRam.ITP[Len * 2], TempPtr, 10);
						strcpy(TempPtr, " , ");
						TempPtr += strlen(" , ");
						TempPtr += inttoascii(TPointsRam.ITP[(Len * 2) + 1], TempPtr, 10);
					}
				}
				if(tcpip){
					strcat(TempPtr , "<br/>TcpIPQ:");
					TempPtr += strlen("<br/>TcpIPQ:");
					TempPtr += inttoascii(tcpip->GetQCount(), TempPtr, 10);
				}
				Len = strlen(Response);
			}
			else if(CrashDiag == true){
				RetVal = TRUE;
				Response[0] = 0;
				Header.reqType = reqData;
				Header.data = (SINT8*)Response;
				char * TempPtr = Response;
				if(DebugSection->CrashValid == 0xAA22EF55){
					strcpy(Response, "Task:");
					TempPtr += strlen("Task:");
					strncpy(TempPtr, TaskName, 24);
					TempPtr = Response + strlen(Response);
					strcat(TempPtr, "<br/>Stack Begin:0X");
					TempPtr += strlen("<br/>Stack Begin:0X");
					TempPtr += inttoascii(Stackbegin, TempPtr, 16);
					strcat(TempPtr, "<br/>Stack End:0X");
					TempPtr += strlen("<br/>Stack End:0X");
					TempPtr += inttoascii(StackEnd, TempPtr, 16);
					strcat(TempPtr, "<br/>Curr Stack:0X");
					TempPtr += strlen("<br/>Curr Stack:0X");
					TempPtr += inttoascii(CurrStackPtr, TempPtr, 16);
					strcat(TempPtr, "<br/>MMUAR:0X");
					TempPtr += strlen("<br/>MMUAR:0X");
					TempPtr += inttoascii(MMUAR, TempPtr, 16);
					strcat(TempPtr, "<br/>Stack Content:<br/>");
					TempPtr += strlen("<br/>Stack Content:<br/>");
					for(Len = 0; (Len < (SINT32)CrashBuffPrintLen); Len++){
						if(Len < 4131){
							TempPtr += inttoascii(CrashBufferRam[Len], TempPtr, 16);
							strcat(TempPtr," ");
							TempPtr += 1;
						}
					}
					strcat(TempPtr, "<br/>Last ModbusPkt:<br/>");
					TempPtr += strlen("<br/>Last ModbusPkt:<br/>");
					for(Len = 0; (Len < (SINT32)sizeof(DebugSection->FRAMCrashMBRcvPktbuf)); Len++){
							TempPtr += inttoascii(DebugSection->FRAMCrashMBRcvPktbuf[Len], TempPtr, 16);
							strcat(TempPtr," ");
							TempPtr += 1;
					}
					strcat(TempPtr, "<br/>Last LCDPkt:<br/>");
					TempPtr += strlen("<br/>Last LCDPkt:<br/>");
					for(Len = 0; (Len < (SINT32)sizeof(DebugSection->FRAMCrashSerialRecBuff)); Len++){
							TempPtr += inttoascii(DebugSection->FRAMCrashSerialRecBuff[Len], TempPtr, 16);
							strcat(TempPtr," ");
							TempPtr += 1;
					}
					strcat(TempPtr, "<br/>Last FBMailboxPkt:<br/>");
					TempPtr += strlen("<br/>Last FBMailboxPkt:<br/>");
					for(Len = 0; (Len < (SINT32)sizeof(DebugSection->FRAMCrashMailBoxbuff)); Len++){
							TempPtr += inttoascii(DebugSection->FRAMCrashMailBoxbuff[Len], TempPtr, 16);
							strcat(TempPtr," ");
							TempPtr += 1;
					}
					strcat(TempPtr, "<br/>Last FBCyclicPkt:<br/>");
					TempPtr += strlen("<br/>Last FBCyclicPkt:<br/>");
					for(Len = 0; (Len < (SINT32)sizeof(DebugSection->FRAMCrashFBRecvBuffer)); Len++){
							TempPtr += inttoascii(DebugSection->FRAMCrashFBRecvBuffer[Len], TempPtr, 16);
							strcat(TempPtr," ");
							TempPtr += 1;
					}

				}
				else{
					strcpy(TempPtr , "No Crash Info:");
				}
				Len = strlen(Response);
			}
			else if(StateInfo == TRUE){
				RetVal = TRUE;
				Response[0] = 0;
				Header.reqType = reqData;
				Header.data = (SINT8*)Response;
				char * TempPtr = Response;
				strcpy(TempPtr , "WCState:");
				TempPtr += strlen("WCState:");
				TempPtr += inttoascii(CurrentIndex, TempPtr, 10);			
				strcpy(TempPtr , " ControlVar:0x");
				TempPtr += strlen(" ControlVar:0x");
				TempPtr += inttoascii(ControlVariable , TempPtr, 16);

				extern int FbFailCounter[7];
				extern int FbLastErrorReason;
				for(int i = 0; i < 7; i++){
					strcpy(TempPtr , " Cnt:");
					TempPtr += strlen(" Cnt:");
					TempPtr += inttoascii(i , TempPtr, 10);
					strcpy(TempPtr , "=");
					TempPtr += strlen("=");
					TempPtr += inttoascii(FbFailCounter[i] ,TempPtr, 10);
				}
				Len = strlen(Response);
			}
			else if(ACDInfo == TRUE)
			{
				RetVal = TRUE;
				Response[0] = 0;
				Header.reqType = reqData;
				Header.data = (SINT8*)Response;
				char * TempPtr = Response;
				if(FieldbusTask::thisPtr && (FieldbusTask::thisPtr->Fb) && (FieldBusType == HW_ASSEMBLY_ETHERNETIP))
				{
					if(ACDInfoUpdated == FALSE)
					{
						FieldbusTask::thisPtr->Fb->GetFaultData(RCX_E_DUPLICATE_NODE);
						RTOS::DelayMs(500);
					}
					if(ACDInfoUpdated == TRUE)
					{
						strcpy(TempPtr , "AcdActivity: ");
						TempPtr += strlen("AcdActivity: ");
						TempPtr += inttoascii(EthipnvData->LastAcd.AcdActivity, TempPtr, 16);
						strcat(TempPtr, "<br/>Remote MAC: ");
						TempPtr += strlen("<br/>Remote MAC: ");
						for(Len = 0; Len < 6; Len++){
							TempPtr += inttoascii(EthipnvData->LastAcd.RemoteMac[Len], TempPtr, 16);
							strcat(TempPtr," ");
							TempPtr += 1;
						}
						strcat(TempPtr, "<br/>HTYPE: ");
						TempPtr += strlen("<br/>HTYPE: ");
						TempPtr += inttoascii(EthipnvData->LastAcd.ArpPDU.Hwtype, TempPtr, 16);

						strcat(TempPtr, "<br/>PTYPE: ");
						TempPtr += strlen("<br/>PTYPE: ");
						TempPtr += inttoascii(EthipnvData->LastAcd.ArpPDU.Proto, TempPtr, 16);

						strcat(TempPtr, "<br/>HLEN: ");
						TempPtr += strlen("<br/>HLEN: ");
						TempPtr += inttoascii(((EthipnvData->LastAcd.ArpPDU.HwlenProtolen >> 8)& 0xFF), TempPtr, 16);
						strcat(TempPtr, "<br/>PLEN: ");
						TempPtr += strlen("<br/>PLEN: ");
						TempPtr += inttoascii((EthipnvData->LastAcd.ArpPDU.HwlenProtolen & 0xFF), TempPtr, 16);


						strcat(TempPtr, "<br/>OPER: ");
						TempPtr += strlen("<br/>OPER: ");
						TempPtr += inttoascii(((EthipnvData->LastAcd.ArpPDU.Opcode >> 8)& 0xFF), TempPtr, 16);

						strcat(TempPtr, "<br/>SHA: ");
						TempPtr += strlen("<br/>SHA: ");
						for(Len = 0; Len < 6; Len++){
							TempPtr += inttoascii(EthipnvData->LastAcd.ArpPDU.Shwaddr[Len], TempPtr, 16);
							strcat(TempPtr," ");
							TempPtr += 1;
						}

						strcat(TempPtr, "<br/>SPA: ");
						TempPtr += strlen("<br/>SPA: ");
						TempIP.addr = EthipnvData->LastAcd.ArpPDU.Sipaddr;
						strcat(TempPtr, TempIP.ToString());
						TempPtr += strlen(TempIP.ToString());

						strcat(TempPtr, "<br/>THA: ");
						TempPtr += strlen("<br/>THA: ");
						for(Len = 0; Len < 6; Len++){
							TempPtr += inttoascii(EthipnvData->LastAcd.ArpPDU.Dhwaddr[Len], TempPtr, 16);
							strcat(TempPtr," ");
							TempPtr += 1;
						}

						strcat(TempPtr, "<br/>THA: ");
						TempPtr += strlen("<br/>THA: ");
						TempIP.addr = EthipnvData->LastAcd.ArpPDU.Dipaddr;
						strcat(TempPtr, TempIP.ToString());
						TempPtr += strlen(TempIP.ToString());

						ACDInfoUpdated = FALSE;
					}
					else
						strcpy(TempPtr , "Please Try Again");

				}
				else
					strcpy(TempPtr , "Invalid Command");

				Len = strlen(Response);
			}
//			else if(FRAMTp == true){
//				RetVal = TRUE;
//				Response[0] = 0;
//				Header.reqType = reqData;
//				Header.data = (SINT8*)Response;
//				for(Len = 0; Len < 150; Len++){
//					sprintf(&Response[strlen(Response)],"\r\n Indx%d Val%u",Len,DebugSection->TPoints.TTP[Len]);
//				}
//				strcat(Response , "\nInterrupt:\n");
//				for(Len = 0; Len <= 255; Len++){
//					sprintf(&Response[strlen(Response)],"\r\n Indx%d Val%u", Len,DebugSection->TPoints.ITP[Len]);
//				}
//				 Len = strlen(Response);
//			}
			if(strstr(FileName, "index.html"))//reset user ID on home page
				DcxLogUserID = 0;
			Header.file = File;
			Header.contentLength = Len;
			Header.statusCode = httpOk;
			Header.contentType =  GetContentType(FileName);
		}	
   }
   	return RetVal;
}

/* void WebSite::UpdateUser(UINT32 UserPrivilege)
 *
 *  Purpose:
 *  This function set the user privilege before any response data. Based on this privilege javascript code on webpage decides
 *  that which tabs need to be shown, or any other things which is specific to branson user.The function gets called from PostResponse function.
 *
 *  Entry condition: UserPrivilege-  User Privilege to be filled in response.
 *
 *  Exit condition: None.
 */
void WebSite::UpdateUser(UINT32 UserPrivilege)
{
	sprintf(Response, "%ld/%ld/%ld@",(SINT32)UserPrivilege,(SINT32)TokenAvailable,(SINT32)MacchipData.CurrSystemType);
}

/* void WebSite::SetupWeldGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *  This function fills data  specific to Weld page in a format which javascript code on this page is
 *  expecting.This function gets called when Weld page is loaded.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::SetupWeldGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      DcxLogUserID = DcxWebUserID;
      SetDefaultValues((DEFAULT_PSCMD) DEFAULT_WELDPS);
      RegUpdateCmdFlag = TRUE;
      Log::WriteEventLog(EVENT_RESTORE_DEFAULTS, SETUP_WELD);

      DcxLogUserID = 0;
   }
   sprintf(&Response[strlen(Response)],
      "WeldMode:RB,%ld@"
      "WeldTime:TT,%ld,%ld,%ld@"
      "Energy:TT,%ld,%ld,%ld@"
      "PeakPower:TT,%ld,%ld,%ld@"
      "ABFlag:RB,%ld@"
      "ABDelay:TT,%ld,%ld,%ld@"
      "ABTime:TT,%ld,%ld,%ld@"
      "FreqOffsetFlag:RB,%ld@"
      "FreqOffset:TT,%ld,%ld,%ld@"
      "EBFlag:CB,%ld@"
      "HoldTime:TT,%ld,%ld,%ld@"
      "ScrubTime:TT,%ld,%ld,%ld@"
      "EndofWeld:CB,%ld@"
	  "ClrMemBfrSeek:CB,%ld@"
	  "ClrMemWithRst:CB,%ld@"
	  "SetDigTuneWithScan:CB,%ld@"
	  "DigTune:TT,%ld,%ld,%ld@",
      (SINT32)ParameterSet[CurrPreset].WcParam.WeldMode,
      (SINT32)ParameterSet[CurrPreset].WcParam.WeldTimeCP,(SINT32)ParameterSet[MinPSNum].WcParam.WeldTimeCP,(SINT32)ParameterSet[MaxPSNum].WcParam.WeldTimeCP,
      (SINT32)ParameterSet[CurrPreset].WcParam.EnergyCP,(SINT32)ParameterSet[MinPSNum].WcParam.EnergyCP,(SINT32)ParameterSet[MaxPSNum].WcParam.EnergyCP,
      (SINT32)ParameterSet[CurrPreset].WcParam.PeakPowerCP / 10,(SINT32)ParameterSet[MinPSNum].WcParam.PeakPowerCP / 10,(SINT32)ParameterSet[MaxPSNum].WcParam.PeakPowerCP / 10,
      (SINT32)ParameterSet[CurrPreset].WcParam.ABFlag,
      (SINT32)ParameterSet[CurrPreset].WcParam.ABDelay,(SINT32)ParameterSet[MinPSNum].WcParam.ABDelay,(SINT32)ParameterSet[MaxPSNum].WcParam.ABDelay,
      (SINT32)ParameterSet[CurrPreset].WcParam.ABTime,(SINT32)ParameterSet[MinPSNum].WcParam.ABTime,(SINT32)ParameterSet[MaxPSNum].WcParam.ABTime,
      (SINT32)ParameterSet[CurrPreset].WcParam.FreqOffsetFlag,
      (SINT32)(ParameterSet[CurrPreset].WcParam.FreqOffset / FREQ_SCALING_FACTOR),(SINT32)(ParameterSet[MinPSNum].WcParam.FreqOffset / FREQ_SCALING_FACTOR),(SINT32)(ParameterSet[MaxPSNum].WcParam.FreqOffset / FREQ_SCALING_FACTOR),
      (SINT32)ParameterSet[CurrPreset].WcParam.EBFlag,
      (SINT32)ParameterSet[CurrPreset].WcParam.HoldTime,(SINT32)ParameterSet[MinPSNum].WcParam.HoldTime,(SINT32)ParameterSet[MaxPSNum].WcParam.HoldTime,
      (SINT32)ParameterSet[CurrPreset].WcParam.ScrubTime,(SINT32)ParameterSet[MinPSNum].WcParam.ScrubTime,(SINT32)ParameterSet[MaxPSNum].WcParam.ScrubTime,
	  (SINT32)ParameterSet[CurrPreset].DcpParam.MemoryStoreFlag,
	  (SINT32)Sysconfig->ClearMemoryBeforeSeek,
	  (SINT32)Sysconfig->ClearMemoryWithReset,
	  (SINT32)Sysconfig->SetDigTuneWithHornScan,
	  (SINT32)SCALE_DOWN_FOR_N_BITS(HsParameterSet[CurrHsPreset].DigitalTuneScaledUP) / FREQ_SCALING_FACTOR,
      (SINT32)SCALE_DOWN_FOR_N_BITS(HsParameterSet[MinHSPSNum].DigitalTuneScaledUP) / FREQ_SCALING_FACTOR,(SINT32)SCALE_DOWN_FOR_N_BITS(HsParameterSet[MaxHSPSNum].DigitalTuneScaledUP) / SCANFREQFACTOR);
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::SetupWeldPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *  This function is used to handle the data posted by Weld page.This function gets called when save button is clicked on the Weld page.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::SetupWeldPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   SINT32 Freq;
	if(TokenAvailable){
	    DcxLogUserID = DcxWebUserID;
		SINT8 * TempStr = strstr(ParamStr, "&WeldMode=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, WeldMode, (WELD_MODE)(atoi(strchr(TempStr, '=') + 1)), WcParamWeldMode, (WELD_MODE)(atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldTime=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, WeldTimeCP, (atoi(strchr(TempStr, '=') + 1)), WcParamWeldTimeCP, (atoi(strchr(TempStr, '=') + 1)), 0);
	    TempStr = strstr(ParamStr, "&Energy=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, EnergyCP, (UINT32)(atoi(strchr(TempStr, '=') + 1)), WcParamEnergyCP, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&PeakPower=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, PeakPowerCP, ((atoi(strchr(TempStr, '=') + 1))*10), WcParamPeakPowerCP, ((atoi(strchr(TempStr, '=') + 1))*10), 0);
		TempStr = strstr(ParamStr, "ABFlag=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, ABFlag, (atoi(strchr(TempStr, '=') + 1)), WcParamABFlag, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&ABDelay=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, ABDelay, (atoi(strchr(TempStr, '=') + 1)), WcParamABDelay, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&ABTime=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, ABTime, (atoi(strchr(TempStr, '=') + 1)), WcParamABTime, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&FreqOffsetFlag=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, FreqOffsetFlag, (FrequencyOffsetType)(atoi(strchr(TempStr, '=') + 1)), WcParamFreqOffsetFlag, (FrequencyOffsetType)(atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&FreqOffset=");
		TempStr = strchr(TempStr, '=') + 1;
		if(TempStr[0] == '-')
			Freq = -(atoi(TempStr + 1));
		else
			Freq = atoi(TempStr);
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, FreqOffset, (Freq * FREQ_SCALING_FACTOR), WcParamFreqOffset, Freq, 0);
		TempStr = strstr(ParamStr, "&DigTune=");
		if(HsParameterSet[CurrHsPreset].DigitalTuneScaledUP != (SCALE_UP_FOR_N_BITS(atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR)){
			HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = 0;
			HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = 0;
		}
		CHECKANDLOG_HSPARMCHANGE(CurrHsPreset, DigitalTuneScaledUP, SCALE_UP_FOR_N_BITS(atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR, DigitalTune, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&EBFlag=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, EBFlag, atoi(strchr(TempStr, '=') + 1), WcParamEBFlag, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&HoldTime=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, HoldTime, atoi(strchr(TempStr, '=') + 1), WcParamHoldTime, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&ScrubTime=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, ScrubTime, atoi(strchr(TempStr, '=') + 1), WcParamScrubTime, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&EndofWeld=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, MemoryStoreFlag, (BOOLEAN)atoi(strchr(TempStr, '=') + 1), DcpParamMemoryStoreFlag, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&ClrMemBfrSeek=");
		Sysconfig->ClearMemoryBeforeSeek = (BOOLEAN)(atoi(strchr(TempStr, '=') + 1));
		TempStr = strstr(ParamStr, "&ClrMemWithRst=");
		Sysconfig->ClearMemoryWithReset = (BOOLEAN)(atoi(strchr(TempStr, '=') + 1));
		TempStr = strstr(ParamStr, "&SetDigTuneWithScan=");
		Sysconfig->SetDigTuneWithHornScan = (BOOLEAN)(atoi(strchr(TempStr, '=') + 1));
		CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_WC | SAVECRC_DCP));
		RegUpdateCmdFlag = TRUE;
		DcxLogUserID = 0;
		strcpy(Response, "OK");
	}
	else
		strcpy(Response, "TOKENNOTAVAILABLE");
	Cmd = REQ_NONE;
}

/* void WebSite::SetupLimitsGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function fills data specific to Limits page in a format which javascript code on this page is
 *   expecting.This function gets called during Limits page is loaded from website.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::SetupLimitsGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
	SINT32 FreqCutoffLowMin, FreqCutoffHighMin;
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      DcxLogUserID = DcxWebUserID;
      SetDefaultCutOffValues();
      SetDefaultValues((DEFAULT_PSCMD)DEFAULT_LIMITSPS);
      Log::WriteEventLog(EVENT_RESTORE_DEFAULTS, SETUP_LIMITS_CUTOFFS);
      RegUpdateCmdFlag = TRUE;
      DcxLogUserID = 0;
   }
   FreqCutoffHighMin = (SINT32)(ParameterSet[MinPSNum].WcParam.WeldFreqHighCutoff )/FREQ_SCALING_FACTOR;
   if(FreqCutoffHighMin < 1)
	   FreqCutoffHighMin = 1;
   FreqCutoffLowMin = (SINT32)(ParameterSet[MinPSNum].WcParam.WeldFreqLowCutoff )/FREQ_SCALING_FACTOR;
   if(FreqCutoffLowMin < 1)
	   FreqCutoffLowMin = 1;

   sprintf(&Response[strlen(Response)],
      "ControlLimits:RB,%ld@"
      "EnergyMLR:TT,%ld,%ld,%ld@"
      "EnergyPLR:TT,%ld,%ld,%ld@"
      "PowerMLR:TT,%ld,%ld,%ld@"
      "PowerPLR:TT,%ld,%ld,%ld@"
      "WeldTimeMLR:TT,%ld,%ld,%ld@"
      "WeldTimePLR:TT,%ld,%ld,%ld@"
      "EnergyN:CB,%ld@"
      "EnergyP:CB,%ld@"
      "PowerN:CB,%ld@"
      "PowerP:CB,%ld@"
      "FreqN:CB,%ld@"
      "FreqP:CB,%ld@"
      "TimeN:CB,%ld@"
      "TimeP:CB,%ld@"
      "TimeCutOff:TT,%ld,%ld,%ld@"
      "EnergyCutoff:TT,%ld,%ld,%ld@"
      "PeakPowerCutoff:TT,%ld,%ld,%ld@"
      "CustomAin1Cutoff:TT,%ld,%ld,%ld@"
      "CustomAin2Cutoff:TT,%ld,%ld,%ld@"
      "WeldFreqHigh:TT,%ld,%ld,%ld@"
      "WeldFreqLow:TT,%ld,%ld,%ld@"
      "CutOff:RB,%ld@"
      "TimeCutOffFlag:CB,%ld@"
      "EnergyCutoffFlag:CB,%ld@"
      "PeakPowerCutoffFlag:CB,%ld@"
      "CustomAin1CutoffFlag:CB,%ld@"
      "CustomAin2CutoffFlag:CB,%ld@"
      "WeldFreqHighFlag:CB,%ld@"
      "WeldFreqLowFlag:CB,%ld@"
      "CustomAin1Configured:T,%ld@"
      "CustomAin2Configured:T,%ld@",
      (SINT32)((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLE) == CONTROL_LIMITS_ENABLE),
      (SINT32)ParameterSet[CurrPreset].LimitsParam.EnergyMLR,(SINT32)ParameterSet[MinPSNum].LimitsParam.EnergyMLR,(SINT32)ParameterSet[MaxPSNum].LimitsParam.EnergyMLR,
      (SINT32)ParameterSet[CurrPreset].LimitsParam.EnergyPLR,(SINT32)ParameterSet[MinPSNum].LimitsParam.EnergyPLR,(SINT32)ParameterSet[MaxPSNum].LimitsParam.EnergyPLR,
      (SINT32)ParameterSet[CurrPreset].LimitsParam.PowerMLR,(SINT32)ParameterSet[MinPSNum].LimitsParam.PowerMLR,(SINT32)ParameterSet[MaxPSNum].LimitsParam.PowerMLR,
      (SINT32)ParameterSet[CurrPreset].LimitsParam.PowerPLR ,(SINT32)ParameterSet[MinPSNum].LimitsParam.PowerPLR ,(SINT32)ParameterSet[MaxPSNum].LimitsParam.PowerPLR,
      (SINT32)ParameterSet[CurrPreset].LimitsParam.WeldTimeMLR,(SINT32)ParameterSet[MinPSNum].LimitsParam.WeldTimeMLR,(SINT32)ParameterSet[MaxPSNum].LimitsParam.WeldTimeMLR,
      (SINT32)ParameterSet[CurrPreset].LimitsParam.WeldTimePLR,(SINT32)ParameterSet[MinPSNum].LimitsParam.WeldTimePLR,(SINT32)ParameterSet[MaxPSNum].LimitsParam.WeldTimePLR,
      (SINT32)((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENERGYMLR) ==  CONTROL_LIMITS_ENERGYMLR),
      (SINT32)((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_ENERGYPLR ) == CONTROL_LIMITS_ENERGYPLR),
      (SINT32)((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_POWERMLR ) == CONTROL_LIMITS_POWERMLR ),
      (SINT32)((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_POWERPLR ) == CONTROL_LIMITS_POWERPLR ),
      (SINT32)((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_FREQMLR ) == CONTROL_LIMITS_FREQMLR ),
      (SINT32)((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_FREQPLR ) == CONTROL_LIMITS_FREQPLR ),
      (SINT32)((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_TIMEMLR ) == CONTROL_LIMITS_TIMEMLR ),
      (SINT32)((ParameterSet[CurrPreset].LimitsParam.ControlLimits & CONTROL_LIMITS_TIMEPLR ) == CONTROL_LIMITS_TIMEPLR ),
      (SINT32)ParameterSet[CurrPreset].WcParam.TimeCutOff,(SINT32)ParameterSet[MinPSNum].WcParam.TimeCutOff,(SINT32)ParameterSet[MaxPSNum].WcParam.TimeCutOff,
      (SINT32)ParameterSet[CurrPreset].WcParam.EnergyCutoff,(SINT32)ParameterSet[MinPSNum].WcParam.EnergyCutoff,(SINT32)ParameterSet[MaxPSNum].WcParam.EnergyCutoff,
      (SINT32)ParameterSet[CurrPreset].WcParam.PeakPowerCutoff,(SINT32)ParameterSet[MinPSNum].WcParam.PeakPowerCutoff,(SINT32)ParameterSet[MaxPSNum].WcParam.PeakPowerCutoff,
      (SINT32)ParameterSet[CurrPreset].ActParam.CustomAIn1Cutoff,(SINT32)ParameterSet[MinPSNum].ActParam.CustomAIn1Cutoff,(SINT32)ParameterSet[MaxPSNum].ActParam.CustomAIn1Cutoff,
      (SINT32)ParameterSet[CurrPreset].ActParam.CustomAIn2Cutoff,(SINT32)ParameterSet[MinPSNum].ActParam.CustomAIn2Cutoff,(SINT32)ParameterSet[MaxPSNum].ActParam.CustomAIn2Cutoff,
      (SINT32)(ParameterSet[CurrPreset].WcParam.WeldFreqHighCutoff )/FREQ_SCALING_FACTOR, FreqCutoffHighMin,(SINT32)(ParameterSet[MaxPSNum].WcParam.WeldFreqHighCutoff)/FREQ_SCALING_FACTOR,
      (SINT32)(ParameterSet[CurrPreset].WcParam.WeldFreqLowCutoff)/FREQ_SCALING_FACTOR, FreqCutoffLowMin,(SINT32)(ParameterSet[MaxPSNum].WcParam.WeldFreqLowCutoff)/FREQ_SCALING_FACTOR,
      (SINT32)ParameterSet[CurrPreset].WcParam.CutOffEnabled,
      (SINT32)ParameterSet[CurrPreset].WcParam.TimeCutOffFlag,
      (SINT32)ParameterSet[CurrPreset].WcParam.EnergyCutoffFlag,
      (SINT32)ParameterSet[CurrPreset].WcParam.PeakPowerCutoffFlag,
      (SINT32)ParameterSet[CurrPreset].ActParam.CustomAIn1CutoffFlag,
      (SINT32)ParameterSet[CurrPreset].ActParam.CustomAIn2CutoffFlag,
      (SINT32)ParameterSet[CurrPreset].WcParam.WeldFreqHighCutOffFlag,
      (SINT32)ParameterSet[CurrPreset].WcParam.WeldFreqLowCutOffFlag,
      (SINT32)CustomInput1Configured, (SINT32)CustomInput2Configured);
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::SetupLimitsPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function Saves the Limits & Cutoff Parameters values set by User. This function gets called by Wepages Post Handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::SetupLimitsPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
		BOOLEAN Temp;
		SINT8 * TempStr = strstr(ParamStr, "ControlLimits=");
		DcxLogUserID = DcxWebUserID;
		Temp = atoi(strchr(TempStr, '=') + 1);
		if(Temp) ParameterSet[CurrPreset].LimitsParam.ControlLimits |= CONTROL_LIMITS_ENABLE;
		else ParameterSet[CurrPreset].LimitsParam.ControlLimits &= ~CONTROL_LIMITS_ENABLE;
		TempStr = strstr(ParamStr, "&EnergyMLR=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, EnergyMLR, (UINT32)atoi(strchr(TempStr, '=') + 1), LimitsParamEnergyMLR, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&EnergyPLR=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, EnergyPLR, (UINT32)atoi(strchr(TempStr, '=') + 1), LimitsParamEnergyPLR, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&PowerMLR=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, PowerMLR, atoi(strchr(TempStr, '=') + 1), LimitsParamPowerMLR, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&PowerPLR=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, PowerPLR, atoi(strchr(TempStr, '=') + 1), LimitsParamPowerPLR, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&WeldTimeMLR=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, WeldTimeMLR, atoi(strchr(TempStr, '=') + 1), LimitsParamWeldTimeMLR, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&WeldTimePLR=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, WeldTimePLR, atoi(strchr(TempStr, '=') + 1), LimitsParamWeldTimePLR, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&EnergyN=");
		Temp = atoi(strchr(TempStr, '=') + 1);
		if(Temp)  ParameterSet[CurrPreset].LimitsParam.ControlLimits |= CONTROL_LIMITS_ENERGYMLR;
		else ParameterSet[CurrPreset].LimitsParam.ControlLimits &= ~CONTROL_LIMITS_ENERGYMLR;
		TempStr = strstr(ParamStr, "&EnergyP=");
		Temp = atoi(strchr(TempStr, '=') + 1);
		if(Temp)  ParameterSet[CurrPreset].LimitsParam.ControlLimits |= CONTROL_LIMITS_ENERGYPLR;
		else ParameterSet[CurrPreset].LimitsParam.ControlLimits &= ~CONTROL_LIMITS_ENERGYPLR;
		TempStr = strstr(ParamStr, "&PowerN=");
		Temp = atoi(strchr(TempStr, '=') + 1);
		if(Temp)  ParameterSet[CurrPreset].LimitsParam.ControlLimits |= CONTROL_LIMITS_POWERMLR;
		else ParameterSet[CurrPreset].LimitsParam.ControlLimits &= ~CONTROL_LIMITS_POWERMLR;
		TempStr = strstr(ParamStr, "&PowerP=");
		Temp = atoi(strchr(TempStr, '=') + 1);
		if(Temp)  ParameterSet[CurrPreset].LimitsParam.ControlLimits |= CONTROL_LIMITS_POWERPLR;
		else ParameterSet[CurrPreset].LimitsParam.ControlLimits &= ~CONTROL_LIMITS_POWERPLR;
		TempStr = strstr(ParamStr, "&TimeN=");
		Temp = atoi(strchr(TempStr, '=') + 1);
		if(Temp)  ParameterSet[CurrPreset].LimitsParam.ControlLimits |= CONTROL_LIMITS_TIMEMLR;
		else ParameterSet[CurrPreset].LimitsParam.ControlLimits &= ~CONTROL_LIMITS_TIMEMLR;
		TempStr = strstr(ParamStr, "&TimeP=");
		Temp = atoi(strchr(TempStr, '=') + 1);
		if(Temp)
			ParameterSet[CurrPreset].LimitsParam.ControlLimits |= CONTROL_LIMITS_TIMEPLR;
		else
			ParameterSet[CurrPreset].LimitsParam.ControlLimits &= ~CONTROL_LIMITS_TIMEPLR;
		CHECKANDLOG_PRESETCHANGE(CurrPreset, LimitsParam, ControlLimits, ParameterSet[CurrPreset].LimitsParam.ControlLimits, LimitsParamControlLimits, ParameterSet[CurrPreset].LimitsParam.ControlLimits, 0);
		TempStr = strstr(ParamStr, "&TimeCutOff=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, TimeCutOff, (UINT32)atoi(strchr(TempStr, '=') + 1), WcParamTimeCutOff, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&EnergyCutoff=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, EnergyCutoff, (UINT32)atoi(strchr(TempStr, '=') + 1), WcParamEnergyCutoff, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&PeakPowerCutoff=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, PeakPowerCutoff, atoi(strchr(TempStr, '=') + 1), WcParamPeakPowerCutoff, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&WeldFreqLow=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, WeldFreqLowCutoff, ((atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR), WcParamWeldFreqLowCutoff, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldFreqHigh=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, WeldFreqHighCutoff, ((atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR), WcParamWeldFreqHighCutoff, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&CustomAin1Cutoff=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, ActParam, CustomAIn1Cutoff, atoi(strchr(TempStr, '=') + 1), ActParamCustomAIn1Cutoff, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&CustomAin2Cutoff=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, ActParam, CustomAIn2Cutoff, atoi(strchr(TempStr, '=') + 1), ActParamCustomAIn2Cutoff, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&CutOffEnabled=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, CutOffEnabled,  (BOOLEAN)(atoi(strchr(TempStr, '=') + 1)), WcParamCutOffEnabled, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&TimeCutOffFlag=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, TimeCutOffFlag,  (BOOLEAN)(atoi(strchr(TempStr, '=') + 1)), WcParamTimeCutOffFlag, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&EnergyCutoffFlag=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, EnergyCutoffFlag,  (BOOLEAN)(atoi(strchr(TempStr, '=') + 1)), WcParamEnergyCutoffFlag, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&PeakPowerCutoffFlag=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, PeakPowerCutoffFlag,  (BOOLEAN)(atoi(strchr(TempStr, '=') + 1)), WcParamPeakPowerCutoffFlag, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldFreqLowFlag=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, WeldFreqLowCutOffFlag, (BOOLEAN)(atoi(strchr(TempStr, '=') + 1)), WcParamWeldFreqLowCutOffFlag, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&WeldFreqHighFlag=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, WeldFreqHighCutOffFlag, (BOOLEAN)(atoi(strchr(TempStr, '=') + 1)), WcParamWeldFreqHighCutOffFlag, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&CustomAin1CutoffFlag=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, ActParam, CustomAIn1CutoffFlag,  (BOOLEAN)(atoi(strchr(TempStr, '=') + 1)), ActParamCustomAIn1CutoffFlag, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&CustomAin2CutoffFlag=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, ActParam, CustomAIn2CutoffFlag,  (BOOLEAN)(atoi(strchr(TempStr, '=') + 1)), ActParamCustomAIn2CutoffFlag, (atoi(strchr(TempStr, '=') + 1)), 0);
		CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_WC | SAVECRC_ACT | SAVECRC_LIMITS));
		RegUpdateCmdFlag = TRUE;
		DcxLogUserID = 0;
		strcpy(Response, "OK");
	}
	else  
		strcpy(Response, "TOKENNOTAVAILABLE");
	Cmd = REQ_NONE;
}
void SetDefaultTime(PRESET_NUMBER PresetNum){
	RTCTime TM;
	RTClock::GetCurrentTime(&TM);
	ParameterSet[PresetNum].InfoParam.ModifiedDate = TM.Day;
	ParameterSet[PresetNum].InfoParam.ModifiedHour = TM.Hour;
	ParameterSet[PresetNum].InfoParam.ModifiedMinute = TM.Minute;
	ParameterSet[PresetNum].InfoParam.ModifiedMonth = TM.Century_Month;
	ParameterSet[PresetNum].InfoParam.ModifiedYear = TM.Years;
	CalculateAndSavePresetCRC(SAVECRC_INFO,PresetNum);
}

/* void WebSite::SetupSaveRecallGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function fills data specific to Save Recall page in a format which javascriptcode on this page is expecting.This function gets called during SaveRecall page is loaded from website.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::SetupSaveRecallGetDatahandler(SINT8 * ParamStr, WebCommand Cmd){
   UINT8 Temp1, Temp2, Indx;
   PRESET_NUMBER Index;
   SINT8 TempTime[TIMESTRINGLEN];
   SINT8 * TempStr;
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      TempStr = strstr(ParamStr, "Index=");
      Index = (PRESET_NUMBER)(atoi(strchr(TempStr, '=') + 1));
      SetDefaultValues((DEFAULT_PSCMD) DEFAULT_ALLPS, Index);
      SetDefaultTime(Index);
   }
   else if((Cmd == REQ_CLEAR_MEMORYFREQ) && (TokenAvailable == TRUE)){
      TempStr = strstr(ParamStr, "Index=");
      Indx = atoi(strchr(TempStr, '=') + 1);
      HsParameterSet[Indx].MemoryOffsetScaledUP = 0;
      if(Indx == ParameterSet[CurrPreset].InfoParam.HornPresetNumber)
    	  HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = 0;
      //CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_HSPS) , CurrPreset, (HSPRESET_NUMBER)Indx);
   }
   for(Indx = 1; Indx <= MAXIMUM_USER_HSPRESETS; Indx++)
   	   sprintf(&Response[strlen(Response)],"%ld@",
   			   SCALE_DOWN_FOR_N_BITS(HsParameterSet[Indx].MemoryOffsetScaledUP+ HsParameterSet[Indx].DigitalTuneScaledUP) / FREQ_SCALING_FACTOR);
   for(Indx = 1; Indx <= MAXIMUM_USER_PRESETS; Indx++){
         Temp1 = ParameterSet[Indx].InfoParam.ModifiedMonth;
         Temp2 = ParameterSet[Indx].InfoParam.ModifiedDate;
         PrepareTimeStamp(TempTime, Temp1, Temp2, ParameterSet[Indx].InfoParam.ModifiedMinute, ParameterSet[Indx].InfoParam.ModifiedHour);
         sprintf(&Response[strlen(Response)],
         "%s;"
         "%02ld/%02ld/%02ld;"
         "%s;"
         "%ld;"
         "%ld@",
         (char*)ParameterSet[Indx].InfoParam.PresetID,
         (SINT32)Temp1, (SINT32)Temp2,
         (SINT32)ParameterSet[Indx].InfoParam.ModifiedYear,
         TempTime,
         (SINT32)ParameterSet[Indx].InfoParam.WeldCount,
         (SINT32)ParameterSet[Indx].InfoParam.HornPresetNumber
      );
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void ReplaceURLSpecialChar(SINT8 * Dst, const SINT8 * Src)
 *
 *  Purpose:
 *   This function is used to handle reception of Some Special Characters i.e. " ", "<", ">", "?", "#".
 *   During URL Post these characters are replaced by some predefined hex values.
 *   This function is called by Save/Recall post handler.
 *
 *  Entry condition: Dst- Pointer to Destination String.
 *                   Src- Ponter to Source String.
 *
 *  Exit condition: None.
 */
void ReplaceURLSpecialChar(SINT8 * Dst, const SINT8 * Src)
{
   SINT8 * Dst1 = Dst, Ch[3];
   memset(Ch, 0, sizeof(Ch));
   while(* Src){
      if((* Src == '%') && (strlen(Src) > 2)){
         memcpy(Ch, (Src + 1), 2);
         if(!strcmp(Ch, "20")){
            *Dst1++ = ' ';
            Src += 3;
         }
         else if(!strcmp(Ch, "22")){
            *Dst1++ = '?';
            Src += 3;
         }
         else if(!strcmp(Ch, "3C")){
            *Dst1++ = '<';
             Src += 3;
         }
         else if(!strcmp(Ch, "3E")){
            *Dst1++ = '>';
            Src += 3;
         }
         else if(!strcmp(Ch, "23")){
            *Dst1++ = '#';
            Src += 3;
         }
         else
            *Dst1++ = *Src++;
       }
      else
         *Dst1++ = *Src++;
   }
   *Dst1 = 0;
}

/* void WebSite::SetupSaveRecallPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function is used to Save, Recall, Verify Preset depending upon the command issued by User.This function gets called from the website module post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::SetupSaveRecallPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
      SINT8 * TempStr, * TempStr1, * TempStr2;
      HSPRESET_NUMBER Hornpresetno;
      SINT8 NamePS[MAX_PRESET_NAME_LEN + 1];
      SINT8 NameFromBrowser[MAX_PRESET_NAME_LEN * 3 + 1];//single special SINT8 is replaced by 3 chars by browser(e.g space = %20)
      SINT16 CurrIndx = CurrentIndex;
      PRESET_NUMBER Index;
      DcxLogUserID = DcxWebUserID;

      switch(Cmd){
         case REQ_NONE:
            strcpy(Response,AutoNamePreset());
         break;
         case REQ_SAVE_PRESET:
            TempStr = strstr(ParamStr, "Index=");
            Index =(PRESET_NUMBER)(atoi(strchr(TempStr, '=') + 1));
            TempStr2 = strstr(ParamStr, "name=");
            TempStr2 = strchr(TempStr2, '=') + 1;//points to name of preset
            TempStr1 = strstr(ParamStr, "hornpresetno=");
            Hornpresetno =(HSPRESET_NUMBER)atoi(strchr(TempStr1, '=') + 1);
            //Length of name is already checked on the Webpage.
            strncpy(NameFromBrowser, TempStr2, CopyLength(MAX_PRESET_NAME_LEN * 3, (TempStr1 - TempStr2), __LINE__));
            ReplaceURLSpecialChar(NamePS, NameFromBrowser);
            if(Hornpresetno == 0)
               Hornpresetno = (HSPRESET_NUMBER)ParameterSet[Index].InfoParam.HornPresetNumber;
            strcpy((SINT8 *)ParameterSet[CurrPreset].InfoParam.PresetID, NamePS);
            CalculateAndSavePresetCRC();
            if(SavePreset(Index, Hornpresetno)){
               strcpy(Response, "Saved");
               Log::WriteEventLog(EVENT_PARAMETER_CHANGED, ((Index << 8) | 0xFF));
            }
            else  strcpy(Response, "Preset No. Out of Range.");
         break;
         case REQ_VERIFY_PRESET://This functionality is disabled from web page
            TempStr = strstr(ParamStr, "Index=");
            Index = (PRESET_NUMBER)(atoi(strchr(TempStr, '=') + 1));
            pprintf(" Verify %d", Index);
            if(VerifyPresetFromWebPage(Index, (HSPRESET_NUMBER)ParameterSet[Index].InfoParam.HornPresetNumber))     //if(RecallAndVerifyPreset(Index))
               strcpy(Response,"Preset Verified");
            else  strcpy(Response,"Preset Not Verified");
         break;
         case REQ_RECALL_PRESET:
            if((CurrIndx == PREREADYSTATE_IDX) || (CurrIndx == READYSTATE_IDX) || (CurrIndx == ERROR_IDX)){
               TempStr = strstr(ParamStr, "Index=");
               Index = (PRESET_NUMBER)(atoi(strchr(TempStr, '=') + 1));
               if(RecallPreset(Index)){
            	  if(AdvanceConfigData->Advrnd.Flag2)
            		  Log::WriteEventLog(EVENT_PRESET_RECALL, Index, COMM_IFC_WEB);
                  strcpy(Response, "Preset Recall success.");
               }
               else  strcpy(Response, "Preset Recall failed.");
            }
            else  strcpy(Response, "System Busy!!!");
         break;
         case REQ_RESET_CYCLERUN:
            DcxLogUserID = DcxWebUserID;
            TempStr = strstr(ParamStr, "Index=");
            Index = (PRESET_NUMBER)(atoi(strchr(TempStr, '=') + 1));
           // CHECKANDLOG_PRESETCHANGE(Index, InfoParam, WeldCount, 0, InfoParamWeldCount, 0, 0);
            if(ParameterSet[Index].InfoParam.WeldCount != 0){
                Log::WriteEventLog(EVENT_PARAMETER_CHANGED, ((Index << 8) | InfoParamWeldCount), 0, 0);
                ParameterSet[Index].InfoParam.WeldCount = 0;
                CalculateAndSavePresetCRC(SAVECRC_INFO, Index);
            }
            DcxLogUserID = 0;
            strcpy(Response,"Reset Cycle count success");
         break;
         default: break;
      }
   }
   else  strcpy(Response, "TOKENNOTAVAILABLE");
   Cmd = REQ_NONE;
   DcxLogUserID = 0;
}

/* void WebSite::SetupAmplitudeGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function fills data specific to Amplitude page in a format which javascript code on this page is expecting.
 *   This function gets called when Amplitude page is loaded from website.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript in URI.
 *
 *  Exit condition: None.
 */
void WebSite::SetupAmplitudeGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      DcxLogUserID = DcxWebUserID;
      SetDefaultValues((DEFAULT_PSCMD)DEFAULT_AMPLITUDEPS);
      Log::WriteEventLog(EVENT_RESTORE_DEFAULTS, SETUP_AMPLITUDE);
      RegUpdateCmdFlag = TRUE;
      DcxLogUserID = 0;
   }
   sprintf(&Response[strlen(Response)],
      "%d@%d@"
      "AmpStepCondition:RB,%d@"
      "AmpStepTime:TT,%d,%d,%d@"
      "AmpStepEnergy:TT,%ld,%ld,%ld@"
      "AmpStepPower:TT,%d,%d,%d@"
      "Amplitude1:TT,%d,%d,%d@"
      "Amplitude2:TT,%d,%d,%d@"
      "WeldRampTime:TT,%d,%d,%d@"
      "ScrubAmp:TT,%d,%d,%d@"
      "ABAmp:TT,%d,%d,%d@"
	  "TestAmplitude:TT,%d,%d,%d@"
	  "TestAmplitudeCondition:RB,%d@",
      (ParameterSet[CurrPreset].WcParam.WeldMode == GroundDetectMode),ParameterSet[CurrPreset].WcParam.ABFlag,
      ParameterSet[CurrPreset].WcParam.AmpStepCondition,
      ParameterSet[CurrPreset].WcParam.AmpStepTime,ParameterSet[MinPSNum].WcParam.AmpStepTime,ParameterSet[MaxPSNum].WcParam.AmpStepTime,
      ParameterSet[CurrPreset].WcParam.AmpStepEnergy,ParameterSet[MinPSNum].WcParam.AmpStepEnergy,ParameterSet[MaxPSNum].WcParam.AmpStepEnergy,
      ParameterSet[CurrPreset].WcParam.AmpStepPower ,ParameterSet[MinPSNum].WcParam.AmpStepPower,ParameterSet[MaxPSNum].WcParam.AmpStepPower,
      ParameterSet[CurrPreset].WcParam.Amplitude1 ,ParameterSet[MinPSNum].WcParam.Amplitude1,ParameterSet[MaxPSNum].WcParam.Amplitude1,
      ParameterSet[CurrPreset].WcParam.Amplitude2,ParameterSet[MinPSNum].WcParam.Amplitude2,ParameterSet[MaxPSNum].WcParam.Amplitude2,
      ParameterSet[CurrPreset].DcpParam.WeldRampTime,ParameterSet[MinPSNum].DcpParam.WeldRampTime,ParameterSet[MaxPSNum].DcpParam.WeldRampTime,
      ParameterSet[CurrPreset].WcParam.ScrubAmp,ParameterSet[MinPSNum].WcParam.ScrubAmp,ParameterSet[MaxPSNum].WcParam.ScrubAmp,
      ParameterSet[CurrPreset].WcParam.ABAmp,ParameterSet[MinPSNum].WcParam.ABAmp,ParameterSet[MaxPSNum].WcParam.ABAmp,
      HsParameterSet[CurrHsPreset].ParaTest.Amplitude1,HsParameterSet[MinHSPSNum].ParaTest.Amplitude1,HsParameterSet[MaxHSPSNum].ParaTest.Amplitude1,
	  HsParameterSet[CurrHsPreset].TestSelection);

   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::SetupAmplitudePostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function Saves Amplitude Webpage Parameters values set by User. This function gets called by Wepages Post Handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::SetupAmplitudePostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
      DcxLogUserID = DcxWebUserID;
		SINT8 * TempStr = strstr(ParamStr, "&AmpStepCondition=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, AmpStepCondition, AMPSTEP_COND(atoi(strchr(TempStr, '=') + 1)), WcParamAmpStepCondition, AMPSTEP_COND(atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&AmpStepTime=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, AmpStepTime, atoi(strchr(TempStr, '=') + 1), WcParamAmpStepTime, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&AmpStepEnergy=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, AmpStepEnergy, (UINT32)atoi(strchr(TempStr, '=') + 1), WcParamAmpStepEnergy, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&AmpStepPower=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, AmpStepPower, atoi(strchr(TempStr, '=') + 1), WcParamAmpStepPower, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&Amplitude1=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, Amplitude1, atoi(strchr(TempStr, '=') + 1), WcParamAmplitude1, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&Amplitude2=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, Amplitude2, atoi(strchr(TempStr, '=') + 1), WcParamAmplitude2, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&WeldRampTime=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldRampTime, atoi(strchr(TempStr, '=') + 1), DcpParamWeldRampTime, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&ScrubAmp=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, ScrubAmp, atoi(strchr(TempStr, '=') + 1), WcParamScrubAmp, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&ABAmp=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, ABAmp, atoi(strchr(TempStr, '=') + 1), WcParamABAmp, atoi(strchr(TempStr, '=') + 1), 0);
		TempStr = strstr(ParamStr, "&TestAmplitude=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaTest, Amplitude1, (atoi(strchr(TempStr, '=') + 1)), ParaTestAmplitude1, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&TestAmplitudeCondition=");
		if(HsParameterSet[CurrHsPreset].TestSelection != TESTAMP_COND(atoi(strchr(TempStr, '=') + 1)))
		{
			Log::WriteEventLog(EVENT_PARAMETER_CHANGED, ((CurrHsPreset << 8) | ParaTestAmpCondition), TESTAMP_COND(atoi(strchr(TempStr, '=') + 1)), 0);
			HsParameterSet[CurrHsPreset].TestSelection = TESTAMP_COND(atoi(strchr(TempStr, '=') + 1));
		}
		CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_WC | SAVECRC_DCP));
		CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_DCP | SAVECRC_HSPS));
		RegUpdateCmdFlag = TRUE;
		DcxLogUserID = 0;
		strcpy(Response, "OK");
   }
   else
	   strcpy(Response,"TOKENNOTAVAILABLE");
	Cmd = REQ_NONE;
}

/* void WebSite::SetupSeekPowerUpGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function fills data specific to SeekPowerUp page in a format which javascript code on this page is expecting.
 *   This function gets called when SeekPowerUp page is loaded from website.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript in URI.
 *
 *  Exit condition: None.
 */
void WebSite::SetupSeekPowerUpGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      SetDefaultValues((DEFAULT_PSCMD) DEAFULT_SEEKPS);
      Log::WriteEventLog(EVENT_RESTORE_DEFAULTS, SETUP_SEEKPOWERUP);
      RegUpdateCmdFlag = TRUE;
   }
   sprintf(&Response[strlen(Response)],
      "SeekTime:TT,%d,%d,%d@"
      "SeekRampTime:TT,%d,%d,%d@"
	  "FreqOffset:TT,%ld,%ld,%ld@"
      "TimedSeekFlag:CB,%d@"
      "powerup:RB,%ld@",
      HsParameterSet[CurrHsPreset].ParaSeek.SeekTime,HsParameterSet[MinHSPSNum].ParaSeek.SeekTime,HsParameterSet[MaxHSPSNum].ParaSeek.SeekTime,
      HsParameterSet[CurrHsPreset].ParaSeek.SeekRampTime,HsParameterSet[MinHSPSNum].ParaSeek.SeekRampTime,HsParameterSet[MaxHSPSNum].ParaSeek.SeekRampTime,
      (SINT32)HsParameterSet [CurrHsPreset].ParaSeek.FreqOffset/FREQ_SCALING_FACTOR,(SINT32)HsParameterSet[MinHSPSNum].ParaSeek.FreqOffset/FREQ_SCALING_FACTOR,(SINT32)HsParameterSet[MaxHSPSNum].ParaSeek.FreqOffset/FREQ_SCALING_FACTOR,
      ParameterSet[CurrPreset].DcpParam.TimedSeekFlag,
      (SINT32)Sysconfig->PowerupOption);
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::SetupSeekPowerUpPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function Saves SeekPowerUp Webpage Parameters values set by User. This function gets called by Wepages Post Handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::SetupSeekPowerUpPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   SINT32 Freq;
   if(TokenAvailable){
      DcxLogUserID = DcxWebUserID;
      SINT8 * TempStr = strstr(ParamStr, "&SeekTime=");
      CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekTime, atoi(strchr(TempStr, '=') + 1), ParaSeekSeekTime, atoi(strchr(TempStr, '=') + 1), 0);
      TempStr = strstr(ParamStr, "&SeekRampTime=");
      CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekRampTime, atoi(strchr(TempStr, '=') + 1), ParaSeekSeekRampTime, atoi(strchr(TempStr, '=') + 1), 0);
	  TempStr = strstr(ParamStr, "&FreqOffset=");
	  TempStr = strchr(TempStr, '=') + 1;
	  if(TempStr[0] == '-')
		  Freq = -(atoi(TempStr + 1));
	  else
		  Freq = atoi(TempStr);
	  CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, FreqOffset, (Freq * FREQ_SCALING_FACTOR), ParaSeekFreqOffset, Freq, 0);
      TempStr = strstr(ParamStr, "&TimedSeekFlag=");
      CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, TimedSeekFlag, atoi(strchr(TempStr, '=') + 1), DcpParamTimedSeekFlag, atoi(strchr(TempStr, '=') + 1), 0);
      TempStr = strstr(ParamStr, "&powerup=");
      if(Sysconfig->RFSwitch.RFSwitchingType == NORFSWITCHING)
         Sysconfig->PowerupOption = atoi(strchr(TempStr, '=') + 1);
      CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_DCP | SAVECRC_HSPS));
      RegUpdateCmdFlag = TRUE;
      DcxLogUserID = 0;
      strcpy(Response, "OK");
   }
   else
	   strcpy(Response, "TOKENNOTAVAILABLE");
   Cmd = REQ_NONE;
}

/* void WebSite::SetupAdvancedGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function fills data specific to Advanced page in a format which javascript code on this page is expecting.
 *   This function gets called when Advanced page is loaded from website.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript in URI.
 *
 *  Exit condition: None.
 */
void WebSite::SetupAdvancedGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      DcxLogUserID = DcxWebUserID;
      SetDefaultValues((DEFAULT_PSCMD)(DEFAULT_ADVANCEPS));
      Log::WriteEventLog(EVENT_RESTORE_DEFAULTS, SETUP_ADVANCED_WELD);
      SetHDDefaults();
      DcxLogUserID = 0;
   }
   ParameterSet[CurrPreset].HDVal.LoopControl = Standard;
   sprintf(&Response[strlen(Response)],
      "WeldAmplitudeLoopC1:TT,%ld,%ld,%ld@"
      "WeldAmplitudeLoopC2:TT,%ld,%ld,%ld@"
      "WeldPhaseLoopI:TT,%ld,%ld,%ld@"
      "WeldPhaseLoopCF:TT,%ld,%ld,%ld@"
      "WeldFreqLow:TT,%ld,%ld,%ld@"
      "WeldFreqHigh:TT,%ld,%ld,%ld@"
      "WeldPhaseLimitTime:TT,%ld,%ld,%ld@"
      "WeldPhaseLimit:TT,%ld,%ld,%ld@"
      "FreqWindowSize:TT,%ld,%ld,%ld@"
      "WeldAmplitudeLoopC1New:TT,%ld,%ld,%ld@"
      "WeldAmplitudeLoopC2New:TT,%ld,%ld,%ld@"
      "WeldPhaseloopINew:TT,%ld,%ld,%ld@"
      "WeldPhaseloopPNew:TT,%ld,%ld,%ld@"
      "WeldFreqLowNew:TT,%ld,%ld,%ld@"
      "WeldFreqHighNew:TT,%ld,%ld,%ld@"
      "PhaseLimitTimeNew:TT,%ld,%ld,%ld@"
      "PhaseLimitNew:TT,%ld,%ld,%ld@"
      "WeldPhaseloopCFNew:TT,%ld,%ld,%ld@"
      "LoopControl:RB,%ld@"
      "HDLoopControl:RB,%ld@"
      "InitPara1:DD,%ld@"
      "InitPara2:TT,%ld,%ld,%ld@"
      "InitPara3:TT,%ld,%ld,%ld@"
      "InitPara4:TT,%ld,%ld,%ld@"
      "InitPara5:TT,%ld,%ld,%ld@"
      "InitPara6:TT,%ld,%ld,%ld@"
      "InitPara7:TT,%ld,%ld,%ld@"
      "InitPara8:TT,%ld,%ld,%ld@"
      "InitPara9:TT,%ld,%ld,%ld@"
      "InitPara10:TT,%ld,%ld,%ld@"
      "InitPara11:TT,%ld,%ld,%ld@"
      "InitPara12:TT,%ld,%ld,%ld@"
      "InitPara13:TT,%ld,%ld,%ld@"
      "InitPara14:TT,%ld,%ld,%ld@"
      "InitPara15:TT,%ld,%ld,%ld@"
      "Parameter1:TT,%ld@"
      "Parameter2:TT,%ld@"
      "Parameter3:TT,%ld@"
      "Parameter4:TT,%ld@"
      "Parameter5:TT,%ld@",
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldAmplitudeLoopC1,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldAmplitudeLoopC1,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldAmplitudeLoopC1,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldAmplitudeLoopC2,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldAmplitudeLoopC2,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldAmplitudeLoopC2,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopI,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldPhaseLoopI,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldPhaseLoopI,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopCF,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldPhaseLoopCF,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldPhaseLoopCF,
      (SINT32)(ParameterSet[CurrPreset].DcpParam.WeldFreqLow )/FREQ_SCALING_FACTOR,((SINT32)ParameterSet[MinPSNum].DcpParam.WeldFreqLow)/FREQ_SCALING_FACTOR,((SINT32)ParameterSet[MaxPSNum].DcpParam.WeldFreqLow )/FREQ_SCALING_FACTOR,
      (SINT32)(ParameterSet[CurrPreset].DcpParam.WeldFreqHigh )/FREQ_SCALING_FACTOR,((SINT32)ParameterSet[MinPSNum].DcpParam.WeldFreqHigh )/FREQ_SCALING_FACTOR,((SINT32)ParameterSet[MaxPSNum].DcpParam.WeldFreqHigh )/FREQ_SCALING_FACTOR,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldPhaseLimitTime,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldPhaseLimitTime,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldPhaseLimitTime,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldPhaseLimit,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldPhaseLimit,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldPhaseLimit,
      (SINT32)ParameterSet[CurrPreset].DcpParam.FreqWindowSize,(SINT32)ParameterSet[MinPSNum].DcpParam.FreqWindowSize,(SINT32)ParameterSet[MaxPSNum].DcpParam.FreqWindowSize,        // for PI values
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldAmplitudeLoopC1new,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldAmplitudeLoopC1new,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldAmplitudeLoopC1new,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldAmplitudeLoopC2new,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldAmplitudeLoopC2new,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldAmplitudeLoopC2new,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopInew,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldPhaseLoopInew,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldPhaseLoopInew,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopP,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldPhaseLoopP,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldPhaseLoopP,
      (SINT32)(ParameterSet[CurrPreset].DcpParam.WeldFreqLow)/FREQ_SCALING_FACTOR,((SINT32)ParameterSet[MinPSNum].DcpParam.WeldFreqLow)/FREQ_SCALING_FACTOR,((SINT32)ParameterSet[MaxPSNum].DcpParam.WeldFreqLow)/FREQ_SCALING_FACTOR,
      (SINT32)(ParameterSet[CurrPreset].DcpParam.WeldFreqHigh)/FREQ_SCALING_FACTOR,((SINT32)ParameterSet[MinPSNum].DcpParam.WeldFreqHigh)/FREQ_SCALING_FACTOR,((SINT32)ParameterSet[MaxPSNum].DcpParam.WeldFreqHigh )/FREQ_SCALING_FACTOR,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldPhaseLimitTime,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldPhaseLimitTime,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldPhaseLimitTime,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldPhaseLimit,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldPhaseLimit,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldPhaseLimit,
      (SINT32)ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopCFnew,(SINT32)ParameterSet[MinPSNum].DcpParam.WeldPhaseLoopCFnew,(SINT32)ParameterSet[MaxPSNum].DcpParam.WeldPhaseLoopCFnew,
      (SINT32)ParameterSet[CurrPreset].DcpParam.UseTrueValues,
      (SINT32)ParameterSet[CurrPreset].HDVal.LoopControl,
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[0],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[1],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[1],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[1],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[2],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[2],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[2],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[3],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[3],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[3],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[4],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[4],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[4],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[5],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[5],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[5],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[6],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[6],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[6],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[7],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[7],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[7],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[8],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[8],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[8],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[9],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[9],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[9],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[10],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[10],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[10],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[11],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[11],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[11],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[12],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[12],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[12],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[13],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[13],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[13],
      (SINT32)ParameterSet[CurrPreset].HDVal.InitPara[14],(SINT32)ParameterSet[MinPSNum].HDVal.InitPara[14],(SINT32)ParameterSet[MaxPSNum].HDVal.InitPara[14],
      (SINT32)ParameterSet[CurrPreset].HDVal.Status[0],
      (SINT32)ParameterSet[CurrPreset].HDVal.Status[1],
      (SINT32)ParameterSet[CurrPreset].HDVal.Status[2],
      (SINT32)ParameterSet[CurrPreset].HDVal.Status[3],
      (SINT32)ParameterSet[CurrPreset].HDVal.Status[4]);
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::SetupAdvancedPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function Saves Advanced Webpage Parameters values set by User. This function gets called by Wepages Post Handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::SetupAdvancedPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
		DcxLogUserID = DcxWebUserID;
		SINT8 * TempStr = strstr(ParamStr, "&LoopControl=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, UseTrueValues, (atoi(strchr(TempStr, '=') + 1)), DcpParamUseTrueValues, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldAmplitudeLoopC1=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldAmplitudeLoopC1, (atoi(strchr(TempStr, '=') + 1)), DcpParamWeldAmplitudeLoopC1, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldAmplitudeLoopC2=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldAmplitudeLoopC2, (atoi(strchr(TempStr, '=') + 1)), DcpParamWeldAmplitudeLoopC2, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldPhaseLoopI=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldPhaseLoopI, (atoi(strchr(TempStr, '=') + 1)), DcpParamWeldPhaseLoopI, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldPhaseLoopCF=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldPhaseLoopCF, (atoi(strchr(TempStr, '=') + 1)), DcpParamWeldPhaseLoopCF, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldFreqLow=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldFreqLow, (SINT32)(((atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR)), DcpParamWeldFreqLow, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldFreqHigh=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldFreqHigh, (SINT32)(((atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR)), DcpParamWeldFreqHigh, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldPhaseLimitTime=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldPhaseLimitTime, (atoi(strchr(TempStr, '=') + 1)), DcpParamWeldPhaseLimitTime, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldPhaseLimit=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldPhaseLimit, (atoi(strchr(TempStr, '=') + 1)), DcpParamWeldPhaseLimit, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldAmplitudeLoopC1New=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldAmplitudeLoopC1new, (atoi(strchr(TempStr, '=') + 1)), DcpParamWeldAmplitudeLoopC1new, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldAmplitudeLoopC2New=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldAmplitudeLoopC2new, (atoi(strchr(TempStr, '=') + 1)), DcpParamWeldAmplitudeLoopC2new, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldPhaseloopINew=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldPhaseLoopInew, (atoi(strchr(TempStr, '=') + 1)), DcpParamWeldPhaseLoopInew, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldPhaseloopPNew=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldPhaseLoopP, (atoi(strchr(TempStr, '=') + 1)), DcpParamWeldPhaseLoopP, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&FreqWindowSize=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, FreqWindowSize, (atoi(strchr(TempStr, '=') + 1)), DcpParamFreqWindowSize, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldFreqLowNew=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldFreqLow, (SINT32)(((atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR)), DcpParamWeldFreqLow, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldFreqHighNew=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldFreqHigh, (SINT32)(((atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR)), DcpParamWeldFreqHigh, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&PhaseLimitTimeNew=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldPhaseLimitTime, atoi(strchr(TempStr, '=') + 1), DcpParamWeldPhaseLimitTime, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&PhaseLimitNew=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldPhaseLimit, atoi(strchr(TempStr, '=') + 1), DcpParamWeldPhaseLimit, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&WeldPhaseloopCFNew=");
		CHECKANDLOG_PRESETCHANGE(CurrPreset, DcpParam, WeldPhaseLoopCFnew, atoi(strchr(TempStr, '=') + 1), DcpParamWeldPhaseLoopCFnew, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&LoopControl=");
		ParameterSet[CurrPreset].HDVal.LoopControl = (LOOP_CONTROL)(atoi(strchr(TempStr, '=') + 1));
		TempStr = strstr(ParamStr, "&InitPara1=");
		ParameterSet[CurrPreset].HDVal.InitPara[0] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara2=");
		ParameterSet[CurrPreset].HDVal.InitPara[1] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara3=");
		ParameterSet[CurrPreset].HDVal.InitPara[2] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara4=");
		ParameterSet[CurrPreset].HDVal.InitPara[3] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara5=");
		ParameterSet[CurrPreset].HDVal.InitPara[4] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara6=");
		ParameterSet[CurrPreset].HDVal.InitPara[5] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara7=");
		ParameterSet[CurrPreset].HDVal.InitPara[6] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara8=");
		ParameterSet[CurrPreset].HDVal.InitPara[7] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara9=");
		ParameterSet[CurrPreset].HDVal.InitPara[8] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "InitPara10=");
		ParameterSet[CurrPreset].HDVal.InitPara[9] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara11=");
		ParameterSet[CurrPreset].HDVal.InitPara[10] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara12=");
		ParameterSet[CurrPreset].HDVal.InitPara[11] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara13=");
		ParameterSet[CurrPreset].HDVal.InitPara[12] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara14=");
		ParameterSet[CurrPreset].HDVal.InitPara[13] = atoi(strchr(TempStr, '=') + 1);
		TempStr = strstr(ParamStr, "&InitPara15=");
		ParameterSet[CurrPreset].HDVal.InitPara[14] = atoi(strchr(TempStr, '=') + 1);
		CalculateAndSavePresetCRC(SAVECRC_DCP);
		RegUpdateCmdFlag = TRUE;
		DcxLogUserID = 0;
		strcpy(Response, "OK");
	}
	else
		strcpy(Response,"TOKENNOTAVAILABLE");
	Cmd = REQ_NONE;
}

/* void WebSite::SetupAdvancedDiagnosticGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function fills data specific to Advanced Diagnostic page in a format which javascript code on this page is expecting.
 *   This function gets called when Advanced Diagnostic page is loaded from website.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript in URI.
 *
 *  Exit condition: None.
 */
void WebSite::SetupAdvancedDiagnosticGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      DcxLogUserID = DcxWebUserID;
      SetDefaultValues((DEFAULT_PSCMD)(DEFAULT_ADVANCEDIAG));
      Log::WriteEventLog(EVENT_RESTORE_DEFAULTS, SETUP_ADV_DIAGNOSTICS);
      DcxLogUserID = 0;
   }
   sprintf(&Response[strlen(Response)],
    "SeekAmpLoopC1:TT,%ld,%ld,%ld@"
    "SeekAmpLoopC2:TT,%ld,%ld,%ld@"
    "SeekPhaseLoopI:TT,%ld,%ld,%ld@"
    "SeekPhaseLoopCF:TT,%ld,%ld,%ld@"
    "SeekFreqLow:TT,%ld,%ld,%ld@"
    "SeekFreqHigh:TT,%ld,%ld,%ld@"
    "SeekAmp:TT,%ld,%ld,%ld@"
    "MaxAmplitude:TT,%ld,%ld,%ld@"
    "MaxCurrent:TT,%ld,%ld,%ld@"
    "FrequencyStart:TT,%ld,%ld,%ld@"
    "FrequencyStop:TT,%ld,%ld,%ld@"
    "TimeDelay:TT,%ld,%ld,%ld@"
	"TestAmplitudeLoopC1:TT,%ld,%ld,%ld@"
	"TestAmplitudeLoopC2:TT,%ld,%ld,%ld@"
	"TestPhaseLoopI:TT,%ld,%ld,%ld@"
	"TestPhaseLoopCF:TT,%ld,%ld,%ld@"
	"TestFreqLow:TT,%ld,%ld,%ld@"
	"TestFreqHigh:TT,%ld,%ld,%ld@",
    //"Amplitude1:TT,%ld,%ld,%ld@",
    //"FreqOffset1:TT,%d,%d,%d@",
    (SINT32)HsParameterSet[CurrHsPreset].ParaSeek.SeekAmpLoopC1,(SINT32)HsParameterSet[MinHSPSNum].ParaSeek.SeekAmpLoopC1,(SINT32)HsParameterSet[MaxHSPSNum].ParaSeek.SeekAmpLoopC1,   // for seek values
    (SINT32)HsParameterSet[CurrHsPreset].ParaSeek.SeekAmpLoopC2,(SINT32)HsParameterSet[MinHSPSNum].ParaSeek.SeekAmpLoopC2,(SINT32)HsParameterSet[MaxHSPSNum].ParaSeek.SeekAmpLoopC2,
    (SINT32)HsParameterSet[CurrHsPreset].ParaSeek.SeekPhaseLoopI,(SINT32)HsParameterSet[MinHSPSNum].ParaSeek.SeekPhaseLoopI,(SINT32)HsParameterSet[MaxHSPSNum].ParaSeek.SeekPhaseLoopI,
    (SINT32)HsParameterSet[CurrHsPreset].ParaSeek.SeekPhaseLoopCF,(SINT32)HsParameterSet[MinHSPSNum].ParaSeek.SeekPhaseLoopCF,(SINT32)HsParameterSet[MaxHSPSNum].ParaSeek.SeekPhaseLoopCF,
    (SINT32)(HsParameterSet[CurrHsPreset].ParaSeek.SeekFreqLow )/FREQ_SCALING_FACTOR,(SINT32)(HsParameterSet[MinHSPSNum].ParaSeek.SeekFreqLow)/FREQ_SCALING_FACTOR,(SINT32)(HsParameterSet[MaxHSPSNum].ParaSeek.SeekFreqLow)/FREQ_SCALING_FACTOR,
    (SINT32)(HsParameterSet[CurrHsPreset].ParaSeek.SeekFreqHigh)/FREQ_SCALING_FACTOR,(SINT32)(HsParameterSet[MinHSPSNum].ParaSeek.SeekFreqHigh)/FREQ_SCALING_FACTOR,(SINT32)(HsParameterSet[MaxHSPSNum].ParaSeek.SeekFreqHigh)/FREQ_SCALING_FACTOR,
    (SINT32)HsParameterSet[CurrHsPreset].ParaSeek.SeekAmp,(SINT32)HsParameterSet[MinHSPSNum].ParaSeek.SeekAmp,(SINT32)HsParameterSet[MaxHSPSNum].ParaSeek.SeekAmp,
    (SINT32)HsParameterSet[CurrHsPreset].ParaScan.MaxAmplitude,(SINT32)HsParameterSet[MinHSPSNum].ParaScan.MaxAmplitude,(SINT32)HsParameterSet[MaxHSPSNum].ParaScan.MaxAmplitude,     // for Horn Scan
    (SINT32)HsParameterSet[CurrHsPreset].ParaScan.MaxCurrent,(SINT32)HsParameterSet[MinHSPSNum].ParaScan.MaxCurrent,(SINT32)HsParameterSet[MaxHSPSNum].ParaScan.MaxCurrent,
    (SINT32)(HsParameterSet[CurrHsPreset].ParaScan.FrequencyStart / SCANFREQFACTOR),(SINT32)(HsParameterSet[MinHSPSNum].ParaScan.FrequencyStart / SCANFREQFACTOR),(SINT32)(HsParameterSet[MaxHSPSNum].ParaScan.FrequencyStart / SCANFREQFACTOR),
    (SINT32)(HsParameterSet[CurrHsPreset].ParaScan.FrequencyStop / SCANFREQFACTOR),(SINT32)(HsParameterSet[MinHSPSNum].ParaScan.FrequencyStop / SCANFREQFACTOR),(SINT32)(HsParameterSet[MaxHSPSNum].ParaScan.FrequencyStop / SCANFREQFACTOR),
    (SINT32)HsParameterSet[CurrHsPreset].ParaScan.TimeDelay,(SINT32)HsParameterSet[MinHSPSNum].ParaScan.TimeDelay,(SINT32)HsParameterSet[MaxHSPSNum].ParaScan.TimeDelay,
    (SINT32)HsParameterSet[CurrHsPreset].ParaTest.AmplitudeLoopC1,(SINT32)HsParameterSet[MinHSPSNum].ParaTest.AmplitudeLoopC1,(SINT32)HsParameterSet[MaxHSPSNum].ParaTest.AmplitudeLoopC1,
    (SINT32)HsParameterSet[CurrHsPreset].ParaTest.AmplitudeLoopC2,(SINT32)HsParameterSet[MinHSPSNum].ParaTest.AmplitudeLoopC2,(SINT32)HsParameterSet[MaxHSPSNum].ParaTest.AmplitudeLoopC2,
    (SINT32)HsParameterSet[CurrHsPreset].ParaTest.PhaseLoopI,(SINT32)HsParameterSet[MinHSPSNum].ParaTest.PhaseLoopI,(SINT32)HsParameterSet[MaxHSPSNum].ParaTest.PhaseLoopI,
    (SINT32)HsParameterSet[CurrHsPreset].ParaTest.PhaseLoopCF,(SINT32)HsParameterSet[MinHSPSNum].ParaTest.PhaseLoopCF,(SINT32)HsParameterSet[MaxHSPSNum].ParaTest.PhaseLoopCF,
    (SINT32)(HsParameterSet[CurrHsPreset].ParaTest.WeldFreqLow )/FREQ_SCALING_FACTOR,(SINT32)(HsParameterSet[MinHSPSNum].ParaTest.WeldFreqLow )/FREQ_SCALING_FACTOR,(SINT32)(HsParameterSet[MaxHSPSNum].ParaTest.WeldFreqLow )/FREQ_SCALING_FACTOR,
    (SINT32)(HsParameterSet[CurrHsPreset].ParaTest.WeldFreqHigh)/FREQ_SCALING_FACTOR,(SINT32)(HsParameterSet[MinHSPSNum].ParaTest.WeldFreqHigh )/FREQ_SCALING_FACTOR,(SINT32)(HsParameterSet[MaxHSPSNum].ParaTest.WeldFreqHigh )/FREQ_SCALING_FACTOR);
    //(SINT32)HsParameterSet[CurrHsPreset].ParaTest.Amplitude1,(SINT32)HsParameterSet[MinHSPSNum].ParaTest.Amplitude1,(SINT32)HsParameterSet[MaxHSPSNum].ParaTest.Amplitude1);
    //(SINT32)(HsParameterSet[CurrHsPreset].ParaTest.FreqOffset / FREQ_SCALING_FACTOR),(SINT32)(HsParameterSet[MinHSPSNum].ParaTest.FreqOffset / FREQ_SCALING_FACTOR),(SINT32)(HsParameterSet[MaxHSPSNum].ParaTest.FreqOffset / FREQ_SCALING_FACTOR));
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::SetupAdvancedDiagnosticPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function Saves Advanced Diagnostic Webpage Parameters values set by User. This function gets called by Wepages Post Handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::SetupAdvancedDiagnosticPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
//	SINT32 Freq;
	if(TokenAvailable){
	   DcxLogUserID = DcxWebUserID;
		SINT8 * TempStr = strstr(ParamStr, "&SeekAmpLoopC1=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekAmpLoopC1, atoi(strchr(TempStr, '=') + 1), ParaSeekSeekAmpLoopC1, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&SeekAmpLoopC2=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekAmpLoopC2, atoi(strchr(TempStr, '=') + 1), ParaSeekSeekAmpLoopC2, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&SeekPhaseLoopI=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekPhaseLoopI, atoi(strchr(TempStr, '=') + 1), ParaSeekSeekPhaseLoopI, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&SeekPhaseLoopCF=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekPhaseLoopCF, atoi(strchr(TempStr, '=') + 1), ParaSeekSeekPhaseLoopCF, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&SeekFreqLow=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekFreqLow, (SINT32)(((atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR)), ParaSeekSeekFreqLow, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&SeekFreqHigh=");
        CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekFreqHigh, (SINT32)(((atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR)), ParaSeekSeekFreqHigh, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&SeekAmp=");
        CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaSeek, SeekAmp, atoi(strchr(TempStr, '=') + 1), ParaSeekSeekAmp, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&MaxAmplitude=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaScan, MaxAmplitude, atoi(strchr(TempStr, '=') + 1), ParaScanMaxAmplitude, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&MaxCurrent=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaScan, MaxCurrent, atoi(strchr(TempStr, '=') + 1), ParaScanMaxCurrent, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&FrequencyStart=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaScan, FrequencyStart, (atoi(strchr(TempStr, '=') + 1) * SCANFREQFACTOR), ParaScanFrequencyStart, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&FrequencyStop=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaScan, FrequencyStop, (atoi(strchr(TempStr, '=') + 1) * SCANFREQFACTOR), ParaScanFrequencyStop, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&TimeDelay=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaScan, TimeDelay, (atoi(strchr(TempStr, '=') + 1)), ParaScanTimeDelay, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&TestFreqLow=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaTest, WeldFreqLow, (SINT32)(((atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR)), ParaTestWeldFreqLow, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&TestFreqHigh=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaTest, WeldFreqHigh, (SINT32)(((atoi(strchr(TempStr, '=') + 1)) * FREQ_SCALING_FACTOR)), ParaTestWeldFreqHigh, (atoi(strchr(TempStr, '=') + 1)), 0);
		//TempStr = strstr(ParamStr, "&Amplitude1=");
		//CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaTest, Amplitude1, (atoi(strchr(TempStr, '=') + 1)), ParaTestAmplitude1, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&TestAmplitudeLoopC1=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaTest, AmplitudeLoopC1, (atoi(strchr(TempStr, '=') + 1)), ParaTestAmplitudeLoopC1, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&TestAmplitudeLoopC2=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaTest, AmplitudeLoopC2, (atoi(strchr(TempStr, '=') + 1)), ParaTestAmplitudeLoopC2, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&TestPhaseLoopI=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaTest, PhaseLoopI, (atoi(strchr(TempStr, '=') + 1)), ParaTestPhaseLoopI, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&TestPhaseLoopCF=");
		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaTest, PhaseLoopCF, (atoi(strchr(TempStr, '=') + 1)), ParaTestPhaseLoopCF, (atoi(strchr(TempStr, '=') + 1)), 0);
		TempStr = strstr(ParamStr, "&FreqOffset1=");
//		TempStr = strchr(TempStr, '=') + 1;
//		if (TempStr[0] == '-')
//			Freq = -(atoi(TempStr + 1));
//		else
//			Freq = atoi(TempStr);
//		CHECKANDLOG_HSPRESETCHANGE(CurrHsPreset, ParaTest, FreqOffset, (Freq * FREQ_SCALING_FACTOR), ParaTestFreqOffset, Freq, 0);
		CalculateAndSavePresetCRC(SAVECRC_HSPS);
		RegUpdateCmdFlag = TRUE;
		DcxLogUserID = 0;
		strcpy(Response, "OK");
   }
   else
	   strcpy(Response, "TOKENNOTAVAILABLE");
	Cmd = REQ_NONE;
}

/* void WebSite::ViewSystemInfoGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function fills data specific to SystemInfo page in a format which javascript code on this page is expecting.
 *   This function gets called when SystemInfo page is loaded from website.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript in URI.
 *
 *  Exit condition: None.
 */
void WebSite::ViewSystemInfoGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
    SINT8 FieldBusInfo[30], * NotAvailable = "N/A";
    SINT8 DCPBLver[10];
    UINT16 LCDcrc;
    SINT8 FBVersion[FBVERSIONLEN];

    const SINT8 * DispInfo = "OK";
 	const SINT8 * CurrSys = "Unknown";
 	const SINT8 * ActuatorInfo = "No";

 	if(LCDDetect == FALSE)
 		DispInfo = "Not Detected";

 	if(MacchipData.CurrSystemType == SYSTEM_ADVANCED){
 		CurrSys = "DCX A";
 	}else if(MacchipData.CurrSystemType == SYSTEM_FIELDBUS){
 		CurrSys = "DCX F";
 	}else if(MacchipData.CurrSystemType == SYSTEM_BAS){
 		CurrSys = "DCX F-BAS";
 		DispInfo = "N/A";
 	}

 	LCDcrc = ((ReceiveCommandHandler::LCDCodeCRCHigh << 8) | ReceiveCommandHandler::LCDCodeCRCLow);
 	if(ActuatorPresentAtPwrUp == TRUE)
 		ActuatorInfo = "Yes";
 	if(strlen(PowerUpData.BootLoaderVersionLabel))
 	   strcpy(DCPBLver, PowerUpData.BootLoaderVersionLabel);
 	else
 		strcpy(DCPBLver, "N/A");
 	if(
 		( (MacchipData.CurrSystemType == SYSTEM_BAS) || (MacchipData.CurrSystemType == SYSTEM_FIELDBUS) || (MacchipData.CurrSystemType == SYSTEM_FIELDBUS_HD) )
 	    && (HilscherSlaveCardDetected == TRUE)
 	  )
 	{
      if(FieldBusType == HW_ASSEMBLY_PROFIBUS)
    	  strcpy(FieldBusInfo, "Profibus");
      else if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
    	  strcpy(FieldBusInfo, "EthernetIP");
      else
    	  strcpy(FieldBusInfo, "Unknown") ;
      if((FieldbusTask::thisPtr) && (strlen(FieldbusTask::thisPtr->FBVersionInfo) < FBVERSIONLEN))
    	  strcpy(FBVersion,FieldbusTask::thisPtr->FBVersionInfo);
 	}
 	else{
 		strcpy(FieldBusInfo, "Not Detected");
 		strcpy(FBVersion,NotAvailable);
 	}
 	sprintf(&Response[strlen(Response)],
      "lifetimecycles:S,%ld"
      "@GeneralAlarms:S,%ld"
      "@hoursofsonics:S,%02ld hr %02ld min %02ld sec"
      "@poweronhours:S,%02ld hr %02ld min %02ld sec"
      "@currsys:S,%s SN %s"
      "@SoftwareVersionLabel:S,%s"
      "@DCxCRC:S,%04lX"
      "@Display:S,%s"
      "@LCDswversion:S,%d.%d"
      "@LCDcrc:S,%04X"
      "@DCPInfo:S,%lu Watts  %d KHz"
      "@DCPswversion:S,%s"
      "@DCPcrc:S,%04lX"
      "@WebInfo:S,%s"
      "@Webcrc:S,%04X"
      "@DCPBootLoaderInfo:S,%s"
      "@DCPBootLoaderVer:S,%s"
      "@DCPBootLoaderCRC:S,%04lX"
      "@WCBootLoaderInfo:S,%s"
      "@WCBootLoaderVer:S,%s"
      "@WCBootLoaderCRC:S,%04lX"
      "@FPGAInfo:S,%s"
      "@FPGAswVer:S,%s"
      "@FPGAcrc:S,%s"
      "@ActuatorInfo:S,%s"
      "@ActuatorswVer:S,%s"
      "@Actuatorcrc:S,%s"
      "@FieldBusInfo:S,%s"
      "@FieldBusswVer:S,%s"
      "@FieldBuscrc:S,%s",
      (SINT32)SysCounters->LifeTimeCycleCount,
      (SINT32)SysCounters->GeneralAlarmCount,
      (SINT32)(SysCounters->SonicsOnHours / 3600),
      (SINT32)((SysCounters->SonicsOnHours %3600)/60),
      (SINT32)((SysCounters->SonicsOnHours %3600)%60),
      (SINT32)(SysCounters->PowerOnHours / 3600),
      (SINT32)((SysCounters->PowerOnHours %3600)/60),
      (SINT32)((SysCounters->PowerOnHours %3600)%60),
      CurrSys,MacchipData.SerialNo,	SWVersionLabel, Sysconfig->DCXCRC,
      DispInfo, (ReceiveCommandHandler::LCDSWVersion / 10),(ReceiveCommandHandler::LCDSWVersion % 10) , LCDcrc,
      MacchipData.Power, MacchipData.Freq, PowerUpData.SWVersionLabel, PowerUpData.DCXCRC,
      NotAvailable, CompressedWebPageCRC,
      NotAvailable, DCPBLver, PowerUpData.BootLoaderCRC,
      NotAvailable, WCBootLoaderVersionLabel, WCBootLoaderCRC,
      NotAvailable, PowerUpData.FPGAVersionLabel, NotAvailable,
      ActuatorInfo, NotAvailable, NotAvailable,
      FieldBusInfo, FBVersion, NotAvailable
 	);
 	Cmd = REQ_NONE;
 	ParamStr = 0;
}


/* UINT8 GetHSPercent(void)
 *
 *  Purpose:
 *    This function gets called during SCAN poll from website and returns the percentage of Horn Scan completed.
 *
 *  Entry condition: None.
 *
 *  Exit condition: Returns percentage of Horn Scan competed.
 */
UINT8 GetHSPercent(void)
{
   UINT8 PercentScan;
   UINT32 CurrentFreq = (RtReceiveData.FreqOut);  //CalculateDDSFreqinPercent();
   UINT32 StartFreq, StopFreq;
   StartFreq = HsParameterSet[CurrHsPreset].ParaScan.FrequencyStart/SCANFREQFACTOR;
   StopFreq = HsParameterSet[CurrHsPreset].ParaScan.FrequencyStop/SCANFREQFACTOR;
   if(StopFreq < StartFreq)
      PercentScan = (StartFreq - CurrentFreq) * SCALING_FACTOR / (StartFreq - StopFreq);
   else
      PercentScan = (CurrentFreq - StartFreq) * SCALING_FACTOR / (StopFreq - StartFreq);
   return PercentScan;
}

void ClearScanFlags(void){
    ControlVariable &= ~WEBSCAN;
    WebSite::ScanStartFlag = FALSE;
    WebIOEnabled = FALSE;
}

/* void WebSite::ProcessScanCommand(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function gets called during SCAN poll from website and returns the percentage of Horn Scan completed.
 *
 *  Entry condition: ParamStr- Pointer to response buffer.
 *                   Cmd- Command sent by jscript in URI.
 *
 *  Exit condition: Returns percentage of Horn Scan competed.
 */
void WebSite::ProcessScanCommand(SINT8 * ParamStr, WebCommand Cmd)
{
   UINT8 Indx = 0;
   if(IsManualFunctionAvailable()){
      switch(Cmd){
         case REQ_SCAN_START:
            if(UserIO::GetReadyFlag() && !diagFlag){
               strcat(Response, "SCANSTARTED:");
               ControlVariable |= WEBSCAN;
               ScanAbortFlag = FALSE;
               ScanFailedFlag = FALSE;
               ScanStartFlag = TRUE;
               webtimeOutScan = WEBSCAN_TIMEOUT;
               DcxLogUserID = DcxWebUserID;
               WebIOEnabled = TRUE;
            }
            else  strcat(Response, "SYSTEMBUSY:");
         break;
         case REQ_SCAN_POLL:
            if((ScanStarted == TRUE) && (ScanAborted == FALSE) && ((ScanDone == FALSE) && (ScanFailed == FALSE))){
               sprintf(&Response[strlen(Response)], "SCANINPROGRESS:%ld", (SINT32)GetHSPercent());
            }
            else if(ScanAborted == TRUE){
               strcat(&Response[strlen(Response)], "SCANABORTED:");
               ScanAbortFlag = TRUE;
               ScanFailedFlag = FALSE;
               ClearScanFlags();
            }
            else if(ScanFailed == TRUE){
               strcat(&Response[strlen(Response)], "SCANFAILED:");
               ScanFailedFlag = TRUE;
               ScanAbortFlag = FALSE;
               ClearScanFlags();
            }
            else if((ScanDone == TRUE) && (ScanFailed == FALSE)){
               strcat(Response, "SCANCOMPLETED:");
               for(Indx = 0; Indx < DCP_NUMRESONANCES; Indx++)
                  sprintf(&Response[strlen(Response)], "%ld,%ld,", (SINT32)ResonanceFreqType[Indx], (SINT32)ResonanceFreqs[Indx]);
               WebIOEnabled = FALSE;
            }
            webtimeOutScan = WEBSCAN_TIMEOUT;
         break;
         case REQ_SCAN_ABORT:
        	 ScanAbortFlag = TRUE;
            strcat(&Response[strlen(Response)], "SCANABORTED:");
            webtimeOutScan = WEBSCAN_TIMEOUT;
  		    AbortReason = SCANABORT_USER;     //User Aborted
            ScanFailedFlag = FALSE;
            DcxLogUserID = DcxWebUserID;
            ClearScanFlags();
         break;
         default: strcpy(Response, "OK"); break;
      }
   }
   else
	   strcpy(Response, "AUTOMODE");
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ViewAlarmLogGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function fills data specific to Alarm Log page.In download request case data format is comma seperated to
 *   support csv download else the data format is what the javascript code on these pages is expecting.
 *   This function gets called when Alarm Log page is loaded from website.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript in URI.
 *
 *  Exit condition: None.
 */
void WebSite::ViewAlarmLogGetDatahandler (SINT8 * ParamStr, WebCommand Cmd)
{
   UINT8 Temp1 = 0, Temp2 = 0;
   UINT16 LogCount = 0, Index;
   SINT8 DataFormat[ALARMDATAFORMAT_LENGTH], TempTime[TIMESTRINGLEN];
   BOOLEAN Download = FALSE;
   SINT8 WeldMode = 0;
   SINT8 * WMode[] = {"Continuous", "Time", "Energy", "Peak Power", "Ground Detect", "Bad Mode"};
   if(Cmd == REQ_DOWNLOAD_ALARMDATA)
	   Download = TRUE;

   DcxLogUserID = DcxWebUserID;
   switch(Cmd){
      case REQ_CLEAR_ALARMLOG:
         LogData2 = LOG_CLEARED_BY_USER;
         Log::ResetLog(CLEARALARM);//only clearing alarm log
      case REQ_ALARMDATA:
      case REQ_DOWNLOAD_ALARMDATA:
         memset(WebAlarmdata, 0, sizeof(WebAlarmData) * MAX_ALARM_ENTRIES);
         LogCount = Log::ReadAlarmLog(WebAlarmdata, MAX_ALARM_ENTRIES);
         break;
      default:  break;
   }
   DcxLogUserID = 0;
   if(Download){
      sprintf(&Response[strlen(Response)],"%s,%s","Alarm #","Date & Time,Cycle Num,Alarm,Alarm Code,"
    		  "Alarm Freq,Alarm Power,Alarm Amp,Alarm Current,Weld Mode,"
    		  "Weld Time,Weld Energy,Power,Amp1,Amp2,"
    		  "Avg Amp,Preset No.,Temp,Seek Start,Seek End,"
    		  "Start Freq,End Freq,CustomIn1,CustomIn2\n");
      strcpy(DataFormat, "%d, %02d-%02d-%02d  %s,%d,%s,%03X,%d,%d,%d,%d,%s,%d,%d.%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d.%d,%d.%d\r\n");
   }
   else{
      sprintf(&Response[strlen(Response)], "%s$LOG,", "alarmLog");
      strcpy(DataFormat, "%d*%02d-%02d-%02d  %s*%d*%s*%03X*%d*%d*%d*%d*%s*%d*%d.%d*%d*%d*%d*%d*%d*%d*%d*%d*%d*%d*%d.%d*%d.%d#");
   }
   for(Index = 0; Index < LogCount; Index++){
         Temp1 = WebAlarmdata[Index].AlarmData.TS.Month;
         Temp2 = WebAlarmdata[Index].AlarmData.TS.Day;
         PrepareTimeStamp(TempTime, Temp1, Temp2, WebAlarmdata[Index].AlarmData.TS.Minute, WebAlarmdata[Index].AlarmData.TS.Hour, WebAlarmdata[Index].AlarmData.TS.Second);
         WeldMode = WebAlarmdata[Index].AlarmData.WeldMode;
         if((WeldMode < 0) || (WeldMode > 4)) WeldMode = 5;
         sprintf(&Response[strlen(Response)], DataFormat,
          WebAlarmdata[Index].AlarmNo,
          (SINT32)Temp1, (SINT32)Temp2, WebAlarmdata[Index].AlarmData.TS.Year,
          TempTime,
          WebAlarmdata[Index].AlarmData.CycleCounter,
          WebAlarmdata[Index].AlarmStr,
          WebAlarmdata[Index].AlarmData.ErrorReason,
          WebAlarmdata[Index].AlarmData.ErrorFrequency,
          WebAlarmdata[Index].AlarmData.ErrorPower,
          WebAlarmdata[Index].AlarmData.ErrorAmplitude,
          WebAlarmdata[Index].AlarmData.ErrorCurrent,
          WMode[(UINT8)WeldMode],
          WebAlarmdata[Index].AlarmData.WeldTime,
          WebAlarmdata[Index].AlarmData.WeldEnergy/ENERGY_CALCULATION_FACTOR,WebAlarmdata[Index].AlarmData.WeldEnergy%ENERGY_CALCULATION_FACTOR,
          WebAlarmdata[Index].AlarmData.WeldPower,
          WebAlarmdata[Index].AlarmData.Amplitude1,
          WebAlarmdata[Index].AlarmData.Amplitude2,
          WebAlarmdata[Index].AlarmData.AvgWeldAmplitude,
          WebAlarmdata[Index].AlarmData.PresetNum,
          WebAlarmdata[Index].AlarmData.Temperature,
          WebAlarmdata[Index].AlarmData.SeekStartFrequency,
          WebAlarmdata[Index].AlarmData.SeekStopFrequency,
          WebAlarmdata[Index].AlarmData.WeldStartFrequency,
          WebAlarmdata[Index].AlarmData.WeldStopFrequency,
          WebAlarmdata[Index].AlarmData.CustomAIn1/CUSTOM_INPUTS_MUL_FACTOR,  WebAlarmdata[Index].AlarmData.CustomAIn1%CUSTOM_INPUTS_MUL_FACTOR,
          WebAlarmdata[Index].AlarmData.CustomAIn2/CUSTOM_INPUTS_MUL_FACTOR,  WebAlarmdata[Index].AlarmData.CustomAIn2%CUSTOM_INPUTS_MUL_FACTOR);
   }
   ParamStr = 0;
   Cmd = REQ_NONE;
}

/* void WebSite::ViewEventLogGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to Event Log page.In download request case data format is comma seperated to
 *   support csv download else the data format is what the javascript code on this page is expecting.
 *   This function gets called when Event Log page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::ViewEventLogGetDatahandler (SINT8 * ParamStr, WebCommand Cmd)
{
   UINT8 Temp1 = 0, Temp2 = 0;
   UINT16 LogCount = 0, Indx;
   SINT8 DataFormat[100], TempTime[TIMESTRINGLEN];
   BOOLEAN Download = FALSE;
   if((Cmd == REQ_DOWNLOAD_WCEVENTDATA)||(Cmd == REQ_DOWNLOAD_DCPEVENTDATA))
	   Download = TRUE;
   DcxLogUserID = DcxWebUserID;
   switch(Cmd){
      case REQ_CLEAR_EVENTLOG:
         LogData2 = LOG_CLEARED_BY_USER;
         Log::ResetLog(CLEAREVENT);
      case REQ_WCEVENTDATA:
      case REQ_DOWNLOAD_WCEVENTDATA:
         memset(WebEventdata, 0, sizeof(WebEventData) * MAX_EVENT_ENTRIES);
         LogCount = Log::ReadWCEventLog(WebEventdata, MAX_EVENT_ENTRIES);
      break;
      case REQ_CLEAR_DCPEVENTLOG:
    	  Log::ResetLog(CLEARDCP);
      case REQ_DCPEVENTDATA:
      case REQ_DOWNLOAD_DCPEVENTDATA:
    	  memset(WebEventdata, 0, sizeof(WebEventData) * MAX_DCP_EVENT_ENTRIES);
         LogCount = Log::ReadDCPEventLog(WebEventdata, MAX_DCP_EVENT_ENTRIES);
      break;
      default: break;
   }
   DcxLogUserID = 0;
   if(Download){
      sprintf(&Response[strlen(Response)], "%s,%s", "Event#","Date & Time,Event,UserID,Data1,Data2\n");
      strcpy(DataFormat, "%d, %02d-%02d-%02d  %s,%s,%s,%d,%d\r\n");
   }
   else{
      sprintf(&Response[strlen(Response)], "%s$LOG,", "eventLog");
      strcpy(DataFormat,"%d*%02d-%02d-%02d  %s*%s*%s*%d*%d#");
   }
   for(Indx = 0; Indx < LogCount; Indx++){
      Temp1 = WebEventdata[Indx].TS.Month;
      Temp2 = WebEventdata[Indx].TS.Day;
      PrepareTimeStamp(TempTime, Temp1, Temp2, WebEventdata[Indx].TS.Minute, WebEventdata[Indx].TS.Hour, WebEventdata[Indx].TS.Second);
      sprintf(&Response[strlen(Response)], DataFormat,
         WebEventdata[Indx].EventNo,
         (SINT32)Temp1, (SINT32)Temp2, WebEventdata[Indx].TS.Year, TempTime,
         WebEventdata[Indx].EventStr,WebEventdata[Indx].UserID,
         WebEventdata[Indx].Data1, WebEventdata[Indx].Data2);
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ViewHistoryGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to History page.In download request case data format is comma seperated to
 *   support csv download else the data format is what the javascript code on this page is expecting.
 *   This function gets called when History page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::ViewHistoryGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   UINT8 Temp1 = 0, Temp2 = 0;
   SINT8 WeldMode = 0;
   UINT16 AlarmId = 0, LogCount = 0, Indx;
   SINT8 ErrReason[20], AmpStr1[5], AmpStr2[5];
   SINT8 DataFormat[100], TempTime[TIMESTRINGLEN];
   SINT16 FreqChange;
   BOOLEAN Download = FALSE;
   if(Cmd == REQ_DOWNLOAD_WELDHISTORY)
	   Download = TRUE;
   SINT8 * WMode[] = {"Continuous", "Time", "Energy", "Peak Power", "Ground Detect", "Bad Mode"};
   DcxLogUserID = 0;
   switch(Cmd){
      case REQ_CLEAR_WELDHISTORY:
         LogData2 = LOG_CLEARED_BY_USER;
         Log::ResetLog(CLEARHISTORY);
      case REQ_WELDHISTORY:
      case REQ_DOWNLOAD_WELDHISTORY:
         memset(WeldHistory, 0, sizeof(WebHistoryData) * MAX_WELD_RESULT_ENTRIES);
         LogCount = GetWeldHistory(WeldHistory, MAX_WELD_RESULT_ENTRIES);
      break;
      default: break;
   }
   DcxLogUserID = 0;
   if(Download){
       sprintf(&Response[strlen(Response)], "%s,%s", "Cycle #", "Date & Time,Weld Mode,Weld Time,Weld Energy,Peak Power, Amp1, Amp2, Preset No., Start Freq, End Freq, Freq Change, AlarmCode, CustomIn1, CustomIn2\n");
       strcpy(DataFormat, "%d, %02d-%02d-%02d  %s,%s,%d.%03d,%d.%d,%d,%s,%s,%d,%d,%d,%d,%s,%d.%d,%d.%d\r\n");
   }
   else{
       sprintf(&Response[strlen(Response)], "%s$HISTORY,", "HistoryLog");
       strcpy(DataFormat, "%d*%02d-%02d-%02d  %s*%s*%d.%03d*%d.%d*%d*%s*%s*%d*%d*%d*%d*%s*%d.%d*%d.%d#");
   }
   for(Indx = 0; Indx < LogCount; Indx++){
      memset(ErrReason, 0, 20);
      Temp1 = WeldHistory[Indx].TS.Month;
      Temp2 = WeldHistory[Indx].TS.Day;
      PrepareTimeStamp(TempTime, Temp1, Temp2, WeldHistory[Indx].TS.Minute, WeldHistory[Indx].TS.Hour, WeldHistory[Indx].TS.Second);
      FreqChange = WeldHistory[Indx].WeldStopFrequency - WeldHistory[Indx].WeldStartFrequency;
      //Check if the alarm was present in the weld cycle. In case of multiple alarms club at least two of
      //them in the result.
      if(WeldHistory[Indx].ErrorReason > 0){
    	  //Get the first alarm code from lower 16 bits and copy to local array after formatting
         AlarmId = (UINT16)WeldHistory[Indx].ErrorReason;
         sprintf(ErrReason, "%03X", AlarmId);
         //Check if cycle had another alarm present
         AlarmId = (UINT16)(WeldHistory[Indx].ErrorReason >> 16);
         if(AlarmId > 0){//If multiple alarms present club with first one seperated by ;
            sprintf(&ErrReason[strlen(ErrReason)], ";%03X", AlarmId);
         }
      }
      if(WeldHistory[Indx].Amplitude1 == NAAMP)
    	  strcpy(AmpStr1,"N/A");
      else
    	  sprintf(AmpStr1,"%d",WeldHistory[Indx].Amplitude1);
      if(WeldHistory[Indx].Amplitude2 == NAAMP)
    	  strcpy(AmpStr2,"N/A");
      else
		  sprintf(AmpStr2,"%d",WeldHistory[Indx].Amplitude2);
      WeldMode = WeldHistory[Indx].WeldMode;
      if((WeldMode < 0) || (WeldMode > 4))
    	  WeldMode = 5;
      //pprintf("\n History Index %d\n", Indx);
      sprintf(&Response[strlen(Response)], DataFormat,
         WeldHistory[Indx].CycleCounter,
         (SINT32)Temp1, (SINT32)Temp2, WeldHistory[Indx].TS.Year, TempTime, WMode[(UINT8)WeldMode],
         (WeldHistory[Indx].WeldTime/1000),(WeldHistory[Indx].WeldTime%1000), WeldHistory[Indx].WeldEnergy/ENERGY_CALCULATION_FACTOR,WeldHistory[Indx].WeldEnergy%ENERGY_CALCULATION_FACTOR, WeldHistory[Indx].PeakPower,
         AmpStr1, AmpStr2,
         WeldHistory[Indx].PresetNumber, WeldHistory[Indx].WeldStartFrequency, WeldHistory[Indx].WeldStopFrequency,FreqChange, ErrReason,
         WeldHistory[Indx].CustomAIn1/CUSTOM_INPUTS_MUL_FACTOR,  WeldHistory[Indx].CustomAIn1%CUSTOM_INPUTS_MUL_FACTOR,
         WeldHistory[Indx].CustomAIn2/CUSTOM_INPUTS_MUL_FACTOR,  WeldHistory[Indx].CustomAIn2%CUSTOM_INPUTS_MUL_FACTOR);
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ViewWeldGraphGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to Weld Graph page in a format what the javascript code on this page is expecting.
 *   This function gets called by Website Get handler to handle commands from WeldGraph Page.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::ViewWeldGraphGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   SINT32 Cnt, Indx = 0, StartIndx = 0, EndIndx = 0;
//   StartIndx = WeldBufferIndex - GRAPHPOINT;
//   EndIndx = WeldBufferIndex;
//
//   if(StartIndx < 0)
//      StartIndx = SIZE_WELDBUFFER - GRAPHPOINT + WeldBufferIndex;
   if(WeldBufferTime <= GRAPHPOINT){
      EndIndx = WeldBufferIndex;
      StartIndx = 1;
   }
   else
   {
	  StartIndx =  WeldBufferIndex + 1;
      EndIndx = GRAPHPOINT - 1;
   }
   Indx = StartIndx - 1;
   switch(Cmd){
      case REQ_TIME_DATA:
         strcpy(Response, "Time,");
         for(Cnt = 0; Cnt <= EndIndx; Cnt++){
            sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[Indx].Time);
            if(Indx == (SIZE_WELDBUFFER - 1))  Indx = 0;
            else  Indx++;
         }
      break;
      case REQ_CURRENT_DATA:
         strcpy(Response,"Current,");
         for(Cnt = 0; Cnt <= EndIndx; Cnt++){
            sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[Indx].Current);
            if(Indx == (SIZE_WELDBUFFER - 1))  Indx = 0;
            else  Indx++;
         }
      break;
      case REQ_PHASE_DATA:
         strcpy(Response,"Phase,");
         for(Cnt = 0; Cnt <= EndIndx; Cnt++){
            sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[Indx].Phase);
            if(Indx == (SIZE_WELDBUFFER - 1)) Indx = 0;
            else  Indx++;
         }
      break;
      case REQ_AMPLITUDE_DATA:
         strcpy(Response,"Amplitude,");
         for(Cnt = 0; Cnt <= EndIndx; Cnt++ ){
            sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[Indx].ActualAmplitude);
            if(Indx == (SIZE_WELDBUFFER - 1))  Indx = 0;
            else  Indx++;
         }
      break;
      case REQ_POWER_DATA:
         strcpy(Response,"Power,");
         for(Cnt = 0; Cnt <= EndIndx; Cnt++){
            sprintf(&Response[strlen(Response)],"%d,",WeldBuffer[Indx].Power);
            if(Indx == (SIZE_WELDBUFFER - 1))  Indx = 0;
            else  Indx++;
         }
      break;
      case REQ_PWMAMPLITUDE_DATA:
         strcpy(Response,"PWMAmplitude,");
         for(Cnt = 0; Cnt <= EndIndx; Cnt++){
            sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[Indx].PWMValue);
            if(Indx == (SIZE_WELDBUFFER - 1))  Indx = 0;
            else  Indx++;
         }
      break;
      case REQ_FREQUENCY_DATA:
         strcpy(Response,"Frequency,");
         for(Cnt = 0; Cnt <= EndIndx; Cnt++){
            sprintf(&Response[strlen(Response)],"%ld,",(SINT32)WeldBuffer[Indx].Frequency);
            if(Indx == (SIZE_WELDBUFFER - 1))  Indx = 0;
            else  Indx++;
         }
      break;
      case REQ_PRESET_DATA:
         strcpy(Response,"PresetData,");			// Pre formatted preset data
         sprintf(&Response[strlen(Response)],
            "%ld,%ld",
            (SINT32)(MinFrequency / FREQ_SCALING_FACTOR),
            (SINT32)(MaxFrequency / FREQ_SCALING_FACTOR)
         );
      break;
      default:  break;
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ViewWeldGraphPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function is used to Reset Alarm in response to Command issued from TEST Page.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::ViewWeldGraphPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
//If alarm is there then only reset is needed. There was a case when website resetFlag was true
//due to reset button was pressed from website. This caused very next alarm to be cleared
//as soon as it occurred. We can't control on this web site code because reset their should work
//in the cases where alarm is caused by some other means(i.e without activating TEST,SCAN etc from website
//)
   if(UserIO::GetAlarmStatus() == TRUE){
      resetFlag = TRUE;
      ControlVariable |= WEBRESET;
   }
   strcpy(Response, "OK");
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* UINT16 CalculateFreqinPercent(UINT32 Val)
 *
 * Purpose:
 *   This function is used to calculate Segments for frequency.This function is called from DiagnosticTestPostDatahandler.
 *
 * Entry condition: None.
 *	FreqVal: Frequency value for which number of segments to be calculated
 * Exit condition: No. of segments calculated.
 */
UINT16 CalculateFreqinPercent(UINT32 FreqVal)
{
   if(FreqVal >= FREQ40KHz_START)
	   FreqVal = ((FreqVal - FREQ40KHz_START)/ FREQ40KHz_HzPERSEGMENT) + 1;
   else if(FreqVal >= FREQ30KHz_START )
	   FreqVal = ((FreqVal - FREQ30KHz_START)/ FREQ30KHz_HzPERSEGMENT) + 1;
   else
	   FreqVal = ((FreqVal - FREQ20KHz_START)/ FREQ20KHz_HzPERSEGMENT) + 1;
   if(FreqVal > NUM_BARGRAPH_SEGMENT) // Total 20 icons for power level.
	   FreqVal = NUM_BARGRAPH_SEGMENT;
   return FreqVal;
}

/* UINT8 CalculatePowerinPercent(void)
 *
 * Purpose:
 *   This function is used to calculate Segments for Power Bar.This function is called from DiagnosticTestPostDatahandler.
 *
 * Entry condition: None.
 *
 * Exit condition: No. of segments calculated.
 */
UINT8 CalculatePowerinPercent(SINT16 PowerVal)
{
   //Convert current power input in percentage form
   UINT8 CurrPower = (PowerVal * NUM_BARGRAPH_SEGMENT)/ 100;
   if(CurrPower > NUM_BARGRAPH_SEGMENT) // Total 20 icons for power level.
      CurrPower = NUM_BARGRAPH_SEGMENT;
   return CurrPower;
}

/* UINT8 CalculateAmplitudeinPercent(void)
 *
 * Purpose:
 *   This function is used to calculate Segments for Amplitude Bar.This function is called from DiagnosticTestPostDatahandler.
 *
 * Entry condition: None.
 *
 * Exit condition: No. of segments calculated.
 */
UINT8 CalculateAmplitudeinPercent(SINT16 AmpVal)
{
   UINT8  Amplitude = (AmpVal * NUM_BARGRAPH_SEGMENT)/ 100;
   if(Amplitude > NUM_BARGRAPH_SEGMENT) // Total 20 icons for power level.
      Amplitude = NUM_BARGRAPH_SEGMENT;
   return Amplitude;
}

/* void WebSite::DiagnosticTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function gets called during Test on load from website and checks whether system is OK to run diagnostic.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.(Its Ignored)
 *
 * Exit condition: None.
 */
void WebSite::DiagnosticSeekGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   SINT16 CurrIndx = CurrentIndex;
   sprintf(&Response[strlen(Response)], "%s@%ld@%ld@%d@%ld@%ld@%ld@" , "OK",
		   (SINT32)(MinFrequency / FREQ_SCALING_FACTOR),
		   (SINT32)(MaxFrequency / FREQ_SCALING_FACTOR),
		   (BIT(DCPTimeControlParam.AdcResolution) - 1),
		   (SINT32)CalculateFreqinPercent(SCALE_DOWN_FOR_N_BITS(HsParameterSet[CurrHsPreset].DigitalTuneScaledUP +
				   HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP) / FREQ_SCALING_FACTOR),
		   (SINT32)(SCALE_DOWN_FOR_N_BITS(HsParameterSet[CurrHsPreset].DigitalTuneScaledUP +
				   HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP) /FREQ_SCALING_FACTOR),
		   (SINT32)CheckForCurrentFunction(CurrIndx)
		   );
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::DiagnosticTestPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function handles Command from Diagnostics Test page. This function gets called by Wepages Post Handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::DiagnosticSeekPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   SINT16 TempPwr, TempAmp;
   SINT32 TempMemout;
   SINT16 CurrIndx = CurrentIndex;
 //  static BOOLEAN RunFlag = FALSE;
   CurrentState CurState;

   if(IsManualFunctionAvailable()){
      switch(Cmd){
         case REQ_SEEK_START:
            if(UserIO::GetReadyFlag() && !diagFlag && !CurrentCycleParam->MfCycling){
               sprintf(&Response[strlen(Response)], "TESTSTARTED:%ld,%ld,%ld,%ld,%ld",(SINT32)stateSeek,
            		   (SINT32)CalculateFreqinPercent(RtReceiveData.FreqOut),(SINT32)CalculateFreqinPercent(SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut) /FREQ_SCALING_FACTOR),
            		   (SINT32)CalculateAmplitudeinPercent(RtReceiveData.AmplitudeOut),(SINT32)CalculatePowerinPercent(RtReceiveData.PowerOut));
               TestStartFlag = TRUE;
               ControlVariable |= WEB_SEEK;
               webtimeOutTest = WEBTEST_TIMEOUT;
               DcxLogUserID = DcxWebUserID;
               WebIOEnabled = TRUE;
            }
            else  strcat(Response, "SYSTEMBUSY:");
         break;
         /*case REQ_RUN_START:
        	 RunFlag = TRUE;
            if(UserIO::GetReadyFlag() && !diagFlag && !CurrentCycleParam->MfCycling){
               sprintf(&Response[strlen(Response)], "TESTSTARTED:%d,%d,%d,%d,%d",
            		   (SINT32)stateRun, (SINT32)CalculateFreqinPercent(RtReceiveData.FreqOut),
            		   (SINT32)CalculateFreqinPercent(SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut) /FREQ_SCALING_FACTOR),(SINT32)CalculateAmplitudeinPercent(),
            		   (SINT32)CalculatePowerinPercent());
               TestRunFlag = TRUE;
               ControlVariable |= WEBWELD_START;
               webtimeOutTest = WEBTEST_TIMEOUT;
               DcxLogUserID = DcxWebUserID;
               WebIOEnabled = TRUE;
            }
            else  strcat(Response , "SYSTEMBUSY:");
         break;*/
         case REQ_SEEK_POLL:
            TempPwr = RtReceiveData.PowerOut;
            TempAmp = RtReceiveData.AmplitudeOut;
            TempMemout = (SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut) / FREQ_SCALING_FACTOR);
            if((ControlVariable & WEB_SEEK) == WEB_SEEK)
            	sprintf(&Response[strlen(Response)], "TESTPOLLRESPONSE:");
            else{
            	sprintf(&Response[strlen(Response)], "TESTSTOPPED:");
          		 if((WeldBufferIndex >= 0) && (WeldBufferIndex < SIZE_WELDBUFFER)){
          			TempPwr = WeldBuffer[WeldBufferIndex].Power;
          			TempAmp = WeldBuffer[WeldBufferIndex].ActualAmplitude;
          			TempMemout = (SINT32)(SCALE_DOWN_FOR_N_BITS(HsParameterSet[CurrHsPreset].DigitalTuneScaledUP +
         				   HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP) /FREQ_SCALING_FACTOR);
          		 }
            }
            sprintf(&Response[strlen(Response)], "%ld,%ld,%ld,%ld,%ld@%ld,%ld,%ld,%ld",
               (SINT32)CheckForCurrentFunction(CurrIndx), (SINT32)CalculateFreqinPercent(RtReceiveData.FreqOut),
               (SINT32)CalculateFreqinPercent(TempMemout),(SINT32)CalculateAmplitudeinPercent(TempAmp),
               (SINT32)CalculatePowerinPercent(TempPwr),(SINT32)RtReceiveData.FreqOut,(SINT32)TempMemout,
               (SINT32)TempAmp,(SINT32)TempPwr);
				if(CheckForCurrentFunction(CurrIndx) == stateError)
					WebIOEnabled = FALSE;
         break;
        /* case REQ_SEEK_STOP:
             if((TestRunFlag == TRUE) && (ParameterSet[CurrPreset].DcpParam.MemoryStoreFlag == FALSE))
            	 TempMemout = (SINT32)(SCALE_DOWN_FOR_N_BITS(StoredFrequencyAbs[ParameterSet[CurrPreset].InfoParam.HornPresetNumber] ) /FREQ_SCALING_FACTOR) ;
             else
             	TempMemout = (SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut ) / FREQ_SCALING_FACTOR);

            sprintf(&Response[strlen(Response)], "TESTSTOPPED:%d,%d,%d,%d,%d@%d,%d,%d,%d",
               (SINT32)CheckForCurrentFunction(), (SINT32)CalculateFreqinPercent(RtReceiveData.FreqOut),
               (SINT32)CalculateFreqinPercent(TempMemout),(SINT32)CalculateAmplitudeinPercent(),
               (SINT32)CalculatePowerinPercent(),(SINT32)(RtReceiveData.FreqOut),
                TempMemout,
               (SINT32)RtReceiveData.AmplitudeOut,(SINT32)RtReceiveData.PowerOut);
            ResetTestFlag();
            WebIOEnabled = FALSE;*
         break;*/
         case REQ_SEEK_RESULT:
        	 WebIOEnabled = FALSE;
       		 CurState =  CheckForCurrentFunction(CurrIndx);
       		 TempPwr = 0;
       		 TempAmp = 0;
       		 if((WeldBufferIndex >= 0) && (WeldBufferIndex < SIZE_WELDBUFFER)){
       			TempPwr = WeldBuffer[WeldBufferIndex].Power;
       			TempAmp = WeldBuffer[WeldBufferIndex].ActualAmplitude;
       		 }
       		TempMemout = (SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut ) / FREQ_SCALING_FACTOR);
            sprintf(&Response[strlen(Response)], "TESTRESULT:%ld,%ld,%ld,%ld,%ld",
               (SINT32)CurState, (SINT32)CalculateFreqinPercent(RtReceiveData.FreqOut),
               (SINT32)CalculateFreqinPercent(TempMemout),(SINT32)CalculateAmplitudeinPercent(TempAmp),
               (SINT32)CalculatePowerinPercent(TempPwr));
         break;
         default: strcpy(Response, "NOK");   break;
      }
   }
   else  strcpy(Response, "AUTOMODE");
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::DiagnosticHornSignatureGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function gets called during diagnostic from website and checks whether system is OK to run diagnostic.
 *
 * Entry condition: ParamStr- Pointer to parameters array(Ignored).
 *                  Cmd- Command sent by jscript in URI(Ignored).
 *
 * Exit condition: None.
 */
void WebSite::DiagnosticHornSignatureGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   SINT32 Index;
   if((ScanStarted == TRUE) && (ScanAborted == FALSE) && ((ScanDone == FALSE) && (ScanFailed == FALSE)))
      strcpy(Response , "SCANINPROGRESS:");
   else{
      switch(Cmd){
         case REQ_CURRENT_DATA:
            strcpy(Response,"Current,");
            for(Index = 0; Index <= NumScanPoints; Index++)
               sprintf(&Response[strlen(Response)],"%d,", ScanBuffer[Index].Current);
         break;
         case REQ_PHASE_DATA:
            strcpy(Response,"Phase,");
            for(Index = 0; Index <= NumScanPoints; Index++)
               sprintf(&Response[strlen(Response)],"%d,", HornScanRawPhase[Index]);
         break;
         case REQ_AMPLITUDE_DATA:
            strcpy(Response,"Amplitude,");
            for(Index = 0; Index <= NumScanPoints; Index++)
               sprintf(&Response[strlen(Response)],"%d,", ScanBuffer[Index].Amplitude);
         break;
         case REQ_FREQUENCY_DATA:
            strcpy(Response,"Frequency,");
            for(Index = 0; Index <= NumScanPoints; Index++)
               sprintf(&Response[strlen(Response)],"%ld,",(SINT32)(ScanBuffer[Index].Frequency));
         break;
         case REQ_PRESET_DATA:
            strcpy(Response,"PresetData,");  // Pre formatted preset data
            sprintf(&Response[strlen(Response)],
               "\n\nFrequency Start (Hz),%ld,"
               "\nFrequency Stop (Hz),%ld,"
               "\nFrequency Step (Hz),%ld,"
               "\nStep - Delay (ms),%ld,"
               "\nAmplitude(%%),%ld,"
               "\nCurrent(%%),%ld",
               (SINT32)(HsParameterSet[CurrHsPreset].ParaScan.FrequencyStart  / SCANFREQFACTOR),
               (SINT32)(HsParameterSet[CurrHsPreset].ParaScan.FrequencyStop  / SCANFREQFACTOR),
               (SINT32)(HsParameterSet[CurrHsPreset].ParaScan.FrequencyStep  / SCANFREQFACTOR),
               (SINT32)(CONVMICRO_MILLI(HsParameterSet[CurrHsPreset].ParaScan.TimeDelay)),
               (SINT32)(HsParameterSet[CurrHsPreset].ParaScan.MaxAmplitude),
               (SINT32)(HsParameterSet[CurrHsPreset].ParaScan.MaxCurrent)
            );
         break;
         case REQ_RESFREQ:
            strcpy(Response,"ResonFreq,");
            for(SINT32 i = 0; i < DCP_NUMRESONANCES; i++){
               if (ResonanceFreqType[i] == FREQ_PARALLEL)
                  sprintf(&Response[strlen(Response)],"P:%ld,",(SINT32)ResonanceFreqs[i]);
               else if(ResonanceFreqType[i] == FREQ_SERIES)
                  sprintf(&Response[strlen(Response)],"S:%ld,",(SINT32)ResonanceFreqs[i]);
            }
         break;
         default:   break;
      }
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::DiagnosticHornSignaturePostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function gets called on load of HornSignature page from website.It fills the last successful horn scan result in Response buffer.
 *
 *  Entry condition: ParamStr- Pointer to parameters array(ignored).
 *                   Cmd- Command sent by jscript(ignored).
 *
 *  Exit condition: None.
 */
void WebSite::DiagnosticHornSignaturePostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   UINT8 Indx = 0;
   sprintf(&Response[strlen(Response)], "%s@PSFreqType:%ld,%ld@%d@","READY",
      (SINT32)((HsParameterSet[CurrHsPreset].ParaScan.FrequencyStart / SCANFREQFACTOR)),
      (SINT32)((HsParameterSet[CurrHsPreset].ParaScan.FrequencyStop  / SCANFREQFACTOR)),
      (BIT(DCPTimeControlParam.AdcResolution) - 1));
   if((ScanDone == TRUE) && (ScanFailed == FALSE)){
      strcat(Response, "SCANCOMPLETED:");
      for(Indx = 0; Indx < DCP_NUMRESONANCES; Indx++)
         sprintf(&Response[strlen(Response)], "%ld,%ld,", (SINT32)ResonanceFreqType[Indx], (SINT32)ResonanceFreqs[Indx]);
   }
   else if(ScanAborted == TRUE)
      strcat(Response, "SCANABORTED:");
   else if(ScanFailed == TRUE)
      strcat(Response, "SCANFAILED:");
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::DiagnosticUserIOGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function displays the status(i.e.active or low) of various functions assigned on digital input and output pins.
 *   Moreover it also fetches the values assigned to analog input and analog output pins.
 *   This function gets called during Diagnostics userio page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array(Ignored).
 *                  Cmd- Command sent by jscript in URI(Ignored).
 *
 * Exit condition:
 *
 * diagFlag and WebIOEnabled set to true if system is in alarm or ready state and not
 * manufacturing cycle is not going on.
 *
 */
void WebSite::DiagnosticUserIOGetDatahandler(SINT8 * ParamStr, WebCommand Cmd){
   if(((UserIO::GetAlarmStatus() || UserIO::GetReadyFlag() || (CurrentIndex == PREREADYSTATE_IDX) || (CurrentIndex == ERROR_IDX)) && !CurrentCycleParam->MfCycling) && (TokenAvailable == TRUE)){
      sprintf(&Response[strlen(Response)], "%s@", "READY");
      diagFlag = TRUE;
      WebIOEnabled = TRUE;
      diagnosticTimeOut = WEBDIAG_TIMEOUT;
   }
   else
      sprintf(&Response[strlen(Response)], "%s@", "NOTREADY");
   sprintf(&Response[strlen(Response)], "FanControl,%d@", FanStatusFlag);
   PrepareDiagnosticResponse();
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::DiagnosticUserIOPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function is used to store the status of digital outputs and analog outputs as selected by
 *   the user.This function gets called when data is posted by Diagnostics userio page.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::DiagnosticUserIOPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   SINT8 * TempStr;
   switch(Cmd){
      case REQ_DIAG_START:
         UpdateDiagnosticIO(ParamStr);
         diagnosticTimeOut = WEBDIAG_TIMEOUT;
      break;
      case REQ_DIAG_STOP:
         StopIODiagnostic();
         diagnosticTimeOut = WEBDIAG_TIMEOUT;
      break;
      case REQ_DIAG_POLL:
         diagnosticTimeOut = WEBDIAG_TIMEOUT;
         StartDiagTimeOut = WEBDIAG_TIMEOUT;
         if(diagFlag || ((UserIO::GetAlarmStatus() || UserIO::GetReadyFlag()) && !CurrentCycleParam->MfCycling && (IsManualFunctionAvailable() == TRUE))){
               sprintf(&Response[strlen(Response)], "OK@");
               WebIOEnabled = TRUE;
               PrepareDiagnosticResponse(diagFlag);
               diagFlag = TRUE;
         }
         else
            sprintf(&Response[strlen(Response)], "NOTOK@");
      break;
      case REQ_DIAG_LOOPBACK_START:
         DoDiagnosticLoopBackStart();
         diagnosticTimeOut = WEBDIAG_TIMEOUT;
      break;
      case REQ_DIAG_LOOPBACK_POLL:
         diagnosticTimeOut = WEBDIAG_TIMEOUT;
         StartDiagTimeOut = LOOPBACK_TIMEOUT;
         DoDiagnosticLoopBack();
         strcpy(Response, "OK@");
         PrepareDiagnosticResponse(TRUE);
      break;
      case REQ_FANCONTROL:
         TempStr = strstr(ParamStr, "FanControl=");
         UserIO::FanEnable(atoi(strchr(TempStr, '=') + 1));
         strcpy(Response, "OK");
      break;
      default:
         strcpy(Response, "NOTOK@");
      break;
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::DiagnosticFieldBusGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *    This function fills data specific to FieldBus page in a format which javascript code on this page is expecting.
 *    This function gets called during FieldBus page is loaded from  website.
 *
 * Entry condition: ParamStr- Pointer to parameters array(Ignored).
 *                  Cmd- Command sent by jscript in URI(Ignored).
 *
 * Exit condition: None.
 */
void WebSite::DiagnosticFieldBusGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   SINT8 Format[20], SlaveAddr[20];
   static SINT8 * BaudRate[] = {"9.6KBit/S", "19.2Kbit/S","93.75KBit/S","187.5KBit/S","500KBit/S","Unknown"
   ,"1.5MBit/S","3MBit/S","6MBit/S","12MBit/S","31.25Kbit/S","45.45KBit/S", "N/A"};
   const SINT8 * BaudRateWebDisplay = "Unknown";
   UINT16 Dummy = 0;
   //CommState Bit 0 to 7 Ready,Running, Bus On , Config Locked, Ignored, Ignored, Param Fault, Config Fault
   //FbInfo Bit 0 to 5 Offline, Stop, Idle, Operate, Ecat(0-11)
   UINT32 Baud, FbInfo = 0, CommState = 0;
   if(DataformatWeb == UP_DATA_FORMAT_BE)
	   strcpy(Format, MOTOROLAFORMAT);
   else if(DataformatWeb == UP_DATA_FORMAT_LE)
	   strcpy(Format, INTELFORMAT);
   else
	   strcpy(Format, UNKNOWN);
   CStat.ulCommunicationCOS = 0;
   CStat.ulCommunicationState = 0;
   CStat.ulCommunicationError = 0;
   EStat.ulBusAddresse = 0;
   SendDiagCmd = TRUE;//Invoke field bus task to get diagnostic status information from Hilscher card
   FbDiagQ.Read(0, Dummy, 1000);//Wait for some time for fieldbus task to update the the global
									  //Common status and extended status information
   CommState = CStat.ulCommunicationCOS & 0x3F;//upadte communication status
   if(Dummy == HW_ASSEMBLY_ETHERNETIP){
      if(CStat.ulCommunicationError == RCX_E_PARAMETER_ERROR)
         CommState |= BIT(6);
      if(CStat.ulCommunicationError == RCX_E_CONFIGURATION_FAULT)
         CommState |= BIT(7);
   }
   else if(Dummy == HW_ASSEMBLY_PROFIBUS){
   	if(ParameterFault)
   	   CommState |= BIT(6);
   	if(ConfigurationFault)
   	   CommState |= BIT(7);
   }
   if(CStat.ulCommunicationState != 0)
	   FbInfo = BIT((CStat.ulCommunicationState - 1));
   if(AlarmIndex > 0)
	   FbInfo |= BIT(4);
   if(Dummy == HW_ASSEMBLY_ETHERNETIP){
      Baud = BAUDNA;
      strcpy(Format, INTELFORMAT);//Ethernet IP works on only Intel format.
      strcpy(SlaveAddr, Sysconfig->FBSystemIP.ToString());
   }
   else if(Dummy == HW_ASSEMBLY_PROFIBUS){
      Baud = LE32_TO_HOST(EStat.ulBaudrate);
      sprintf(SlaveAddr, "%ld", LE32_TO_HOST(EStat.ulBusAddresse));
   }
   else{
      strcpy(Format, UNKNOWN);
      strcpy(SlaveAddr, UNKNOWN);
      Baud = BAUDUNKNOWN;
   }
   if(Baud <= 12)
	   BaudRateWebDisplay = BaudRate[Baud];
   else
	   BaudRateWebDisplay = "Unknown";
    sprintf(&Response[strlen(Response)],"Controlword1,%d@Controlword2,%d@Statusword1,%d@Statusword2,%d"
            "@SlaveAddress,%s@Baudrate,%s@Dataformat,%s@FBInfo,%ld@CosState,%ld",
            Controlword1,Controlword2,Statusword1,Statusword2,
            SlaveAddr, BaudRateWebDisplay,Format, FbInfo, CommState);
   ParamStr = 0;
   Cmd = REQ_NONE;
}

/* void WebSite::ConfigurationSystemGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to System page in a format which javascript code on this page
 *   is expecting.This function gets called during System page is loaded from  website.
 *
 * Entry condition: ParamStr- Pointer to parameters array(Ignored).
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::ConfigurationSystemGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   RTCTime TM;
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE))
	   SetDefaultConfigurationSystem();
   if(Cmd == REQ_DATETIME){
	   RTClock::GetCurrentTime(&TM);
	   sprintf(Response,
			   "Date,%d,%d,%d,%d@"
			   "Time,%d,%d,%d,%d",
			   Sysconfig->Dateformat,TM.Century_Month,TM.Day,TM.Years,
			   Sysconfig->Timeformat - 1,TM.Hour,TM.Minute,TM.Second
	   );	   
   }
   else{
	   sprintf(&Response[strlen(Response)],
	      "FormatDate:DD,%d@"
	      "FormatTime:DD,%d@"
	      "Externalstatus:CB,%d@"
	      "ErrorFlag:CB,%d@"
	      "AlarmFlag:CB,%d@"
	      "ExtPreset:CB,%d@"
	      "ActuatorPresent:AP,%ld@",
	      Sysconfig->Dateformat,
	      Sysconfig->Timeformat,
	      Sysconfig->Externalstatus,
	      Sysconfig->ErrorFlag,
	      Sysconfig->AlarmFlag,
	      Sysconfig->ExternalPresetFlag,
	      (SINT32)UserIO::CheckForActuatorPresent());
   }  
   ParamStr = 0;
}
void SendRTCSyncCommandToDCP(void){
	  NrtQ NRTSynccmd = (NrtQ){CMD_SYNCRTC, WRITE_REQ, 0};
	  DCPTimeSyncCounter = 0;
	  NRTModbus::thisPtr -> Nrtq.Write(0, NRTSynccmd, 0);
}
/* void WebSite::ConfigurationSystemPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function is handle the System Parameters values posted from System Webpage.This functions is called from Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript).
 *
 *  Exit condition: None.
 */
void WebSite::ConfigurationSystemPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
      SINT32 Years = 0;
      SINT8 * TempStr = strstr(ParamStr, "&Externalstatus=");
      Sysconfig->Externalstatus = (BOOLEAN)(atoi(strchr(TempStr, '=') + 1));
      TempStr = strstr(ParamStr, "&AlarmFlag=");
      Sysconfig->AlarmFlag = (BOOLEAN)(atoi(strchr(TempStr, '=') + 1));
      TempStr = strstr(ParamStr, "&ExtPreset=");
      if(Sysconfig->RFSwitch.RFSwitchingType == NORFSWITCHING)
         Sysconfig->ExternalPresetFlag = (BOOLEAN)(atoi(strchr(TempStr, '=') + 1));
      TempStr = strstr(ParamStr, "&ErrorFlag=");
      Sysconfig->ErrorFlag = (BOOLEAN)(atoi(strchr(TempStr, '=') + 1));
      if(Cmd == REQ_SAVE){
         TempStr = strstr(ParamStr, "&DateFormat=");
         Sysconfig->Dateformat = (DateFormat)(atoi(strchr(TempStr, '=') + 1));
         TempStr = strstr(ParamStr, "&TimeFormat=");
         Sysconfig->Timeformat = (TimeFormat)(atoi(strchr(TempStr, '=') + 1));                                           // Add time & date form RTClock
         TempStr = strstr(ParamStr, "HH:");
         RTCClocks.Hour = atoi(strchr(TempStr, ':') + 1);
         TempStr = strstr(ParamStr, "MIN:");
         RTCClocks.Minute = atoi(strchr(TempStr, ':') + 1);
         TempStr = strstr(ParamStr, "SS:");
         RTCClocks.Second = atoi(strchr(TempStr, ':') + 1);
         TempStr = strstr(ParamStr, "DD:");
         RTCClocks.Day = atoi(strchr(TempStr, ':') + 1);
         TempStr = strstr(ParamStr, "MM:");
         RTCClocks.Century_Month = atoi(strchr(TempStr, ':') + 1);
         TempStr = strstr(ParamStr, "YY:");
         Years = atoi(strchr(TempStr, ':') + 1);
         if(Years > 99)
            Years = Years % 100;
         RTCClocks.Years = Years;
         if(RTClock::SetCurrentTime(&RTCClocks)){
        	 SendRTCSyncCommandToDCP();
         }
      }
      RegUpdateCmdFlag = TRUE;
      strcpy(Response,"OK");
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ConfigurationUserIOGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to ConfiguserIO page in a format which javascript code on this page
 *   is expecting.This function gets called during ConfiguserIO page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::ConfigurationUserIOGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   UINT8 Indx, FuncNum = 0;
   SINT8 * TempStr;
   SINT8 * formatStrDigIn[] = {0 ,"dd_pin1:DDV,%d,%d@","dd_pin2:DDV,%d,%d@","dd_pin3:DDV,%d,%d@", "dd_pin4:DDV,%d,%d@","dd_pin11:DDV,%d,%d@","dd_pin12:DDV,%d,%d@","dd_pin13:DDV,%d,%d@","dd_pin16:DDV,%d,%d@","dd_pin23:DDV,%d,%d@"};
   SINT8 * formatStrDigOut[] = {0 ,"dd_pin7:DDV,%d,%d@","dd_pin8:DDV,%d,%d@","dd_pin9:DDV,%d,%d@", "dd_pin10:DDV,%d,%d@","dd_pin19:DDV,%d,%d@","dd_pin20:DDV,%d,%d@","dd_pin21:DDV,%d,%d@","dd_pin22:DDV,%d,%d@"};
   SINT8 * formatStrAIn[] = {0,"dd_pin17:DDV,%d,%d@","dd_pin18:DDV,%d,%d@"};
   SINT8 * formatStrAOut[] = {0,"dd_pin24:DDV,%d,%d@","dd_pin25:DDV,%d,%d@"};
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      DcxLogUserID = DcxWebUserID;
      TempStr = strstr(ParamStr, "DefaultTypeSelected=");
      DefaultSelected = atoi(strchr(TempStr, '=') + 1);
      CableDetectedPin = 0;
      CustomInput1Configured = CustomInput2Configured = FALSE;
      InterlockConfigured = PartInPlaceConfigured = FALSE;
      switch(DefaultSelected){
         case BRANSONDEFAULTS:
        	 UserIO::ConfigureBransonDefaultUserIOs();
        	 break;
         case ACTUATORDEFAULTS:
        	 UserIO::ConfigureActuatorDefaultUserIOs();
        	 break;
         case RFSWITCHINGDEFAULTS:
        	 UserIO::ConfigureRFDefaultUserIOs();
        	 break;
         case CUSTOMDEFAULTS:
        	 UserIO::ConfigureCustomDefaultUserIOs();
        	 break;
         default: break;
      }
      DcxLogUserID = 0;
   }
   TempStr = Response + strlen(Response);
   sprintf(TempStr, "DefaultTypeSelected=%d", DefaultSelected);
   strcat(TempStr,"@dd_pin1,dd_pin2,dd_pin3,dd_pin4,dd_pin11,dd_pin12,dd_pin13,dd_pin16,dd_pin23:DD,");
   FuncNum = sizeof(webDigIOfuncTableIn)/sizeof(WebDigitalIOFunctionTable);
   for(Indx = 1; Indx < FuncNum; Indx++){
      strcat(TempStr, FindLanguageString(webDigIOfuncTableIn[Indx].StringID, languageBuff, TRUE));
      sprintf(&TempStr[strlen(TempStr)], "#%ld#%ld", webDigIOfuncTableIn[Indx].ActivehighID,
      webDigIOfuncTableIn[Indx].ActivelowID);
      if(Indx < (FuncNum - 1))
         strcat(TempStr, ",");
   }
   strcat(TempStr,"@dd_pin7,dd_pin8,dd_pin9,dd_pin10,dd_pin19,dd_pin20,dd_pin21,dd_pin22:DD,");
   FuncNum = sizeof(webDigIOfuncTableOut)/sizeof(WebDigitalIOFunctionTable);
   for(Indx = 1; Indx < FuncNum; Indx++){
      strcat(TempStr,FindLanguageString(webDigIOfuncTableOut[Indx].StringID, languageBuff, TRUE));
      sprintf(&TempStr[strlen(TempStr)], "#%ld#%ld", webDigIOfuncTableOut[Indx].ActivehighID,
      webDigIOfuncTableOut[Indx].ActivelowID);
      if(Indx < (FuncNum - 1))
         strcat(TempStr, ",");
   }
   strcat(TempStr, "@dd_pin17,dd_pin18:DD,");
   FuncNum = sizeof(webAnaIOfuncTableIn)/sizeof(WebAnalogIOFunctionTable);
   for(Indx = 1; Indx < FuncNum; Indx++){
      strcat(TempStr,FindLanguageString(webAnaIOfuncTableIn[Indx].StringID, languageBuff, TRUE));
      sprintf(&TempStr[strlen(TempStr)] ,"#%ld", webAnaIOfuncTableIn[Indx].FunctionID);
      if(Indx < (FuncNum - 1))
         strcat(TempStr, ",");
   }
   strcat(TempStr,"@dd_pin24,dd_pin25:DD,");
   FuncNum = sizeof(webAnaIOfuncTableOut)/sizeof(WebAnalogIOFunctionTable);
   for(Indx = 1; Indx < FuncNum; Indx++){
      strcat(TempStr, FindLanguageString(webAnaIOfuncTableOut[Indx].StringID, languageBuff, TRUE));
      sprintf(&TempStr[strlen(TempStr)] ,"#%ld", webAnaIOfuncTableOut[Indx].FunctionID);
      if(Indx < (FuncNum - 1))
         strcat(TempStr,",");
   }
   //get current functions after function list seperated by @
   strcat(TempStr ,"@");
   for(Indx = 1; Indx < NUM_DIN; Indx++)
      sprintf(&TempStr[strlen(TempStr)], formatStrDigIn[Indx], USERIOConfig->DigInUse[Indx],USERIOConfig->DigInUseStatus[Indx]);
   for(Indx = 1; Indx < NUM_DOUT; Indx++)
      sprintf(&TempStr[strlen(TempStr)], formatStrDigOut[Indx], USERIOConfig->DigOutUse[Indx],USERIOConfig->DigOutUseStatus[Indx]);
   for(Indx = 1; Indx < NUM_AIN; Indx++)
      sprintf(&TempStr[strlen(TempStr)], formatStrAIn[Indx], USERIOConfig->AnalogInUse[Indx],USERIOConfig->AnalogInUseStatus[Indx]);
   for(Indx = 1; Indx < NUM_AOUT; Indx++)
      sprintf(&TempStr[strlen(TempStr)], formatStrAOut[Indx], USERIOConfig->AnalogOutUse[Indx],USERIOConfig->AnalogOutUseStatus[Indx]);
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ConfigurationUserIOPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function is handle the System Parameters values posted from System Webpage.This functions is called from Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript).
 *
 *  Exit condition: None.
 */
void WebSite::ConfigurationUserIOPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   UINT8 Indx, FuncID, ActiveStatus, FuncIDLog;
   UINT16 DigOutputsActiveConfiguration = 0;
   BOOLEAN WriteLowSelectEvent = FALSE, CableDetectInput = FALSE;
   SINT16 CurrIndx = CurrentIndex;
   SINT8 * TmpStr;
   DcxLogUserID = DcxWebUserID;
   if((CurrIndx == PREREADYSTATE_IDX) || (CurrIndx == READYSTATE_IDX)){
      if(TokenAvailable){
         CableDetectedPin = 0;
         CustomInput1Configured = CustomInput2Configured = FALSE;
         InterlockConfigured = PartInPlaceConfigured = FALSE;
         TmpStr = strstr(ParamStr, "DI:");   //TODO: Validate string present
         TmpStr = strchr(TmpStr, ':') + 1;
         for(Indx = 1; Indx < NUM_DIN; Indx++){
        	FuncIDLog = FuncID = atoi(TmpStr);
            TmpStr = strchr(TmpStr, ',') + 1;
            ActiveStatus = atoi(TmpStr);
            if((USERIOConfig->DigInUse[Indx] !=  FuncID) || (ActiveStatus != USERIOConfig->DigInUseStatus[Indx])){
            	if(ActiveStatus == FALSE)
            		FuncIDLog = 0;
            	DigitalInputConfigured = TRUE;
                Log::WriteEventLog(EVENT_USERIO_DEFINED, DigInPins[Indx], FuncIDLog);
            }
            UserIO::AssignDigitalInputFunction((DIGITAL_INPUTS)Indx, FuncID, ActiveStatus);
            if(FuncID == CABLEDETECT_ACTIVEHIGH)
               CableDetectInput = (BOOLEAN)ActiveStatus;
            TmpStr = strchr(TmpStr, ',') + 1;
         }
         TmpStr = strstr(ParamStr, "DO:");
         TmpStr = strchr(TmpStr, ':') + 1;
         for(Indx = 1; Indx < NUM_DOUT; Indx++){
        	FuncIDLog = FuncID = atoi(TmpStr);
            TmpStr = strchr(TmpStr, ',') + 1;
            ActiveStatus = atoi(TmpStr);
            if((USERIOConfig->DigOutUse[Indx] !=  FuncID)  || (ActiveStatus != USERIOConfig->DigOutUseStatus[Indx])){
            	if(ActiveStatus == FALSE)
            		FuncIDLog = 0;
             	Log::WriteEventLog(EVENT_USERIO_DEFINED, DigOutPins[Indx], FuncIDLog);
            }
            UserIO::AssignDigitalOutputFunction((DIGITAL_OUTPUTS)Indx, FuncID, ActiveStatus);
            //don't care if not active. Let bootloader do whatever it is doing
            DigOutputsActiveConfiguration |= (OutPtr[FuncID].Active << (Indx -1));
            TmpStr = strchr(TmpStr, ',') + 1;
         }
         TmpStr = strstr(ParamStr, "AI:");
         TmpStr = strchr(TmpStr, ':') + 1;
         for(Indx = 1; Indx < NUM_AIN; Indx++){
        	 FuncIDLog = FuncID = atoi(TmpStr);
            TmpStr = strchr(TmpStr, ',') + 1;
            ActiveStatus = atoi(TmpStr);
            if((USERIOConfig->AnalogInUse[Indx] !=  FuncID) || (ActiveStatus != USERIOConfig->AnalogInUseStatus[Indx])){
            	if(ActiveStatus == FALSE)
            		FuncIDLog = 0;
				Log::WriteEventLog(EVENT_USERIO_DEFINED, AinPins[Indx], FuncIDLog);
            }
            UserIO::AssignAnalogInputFunction((ANALOG_INPUTS)Indx, FuncID, ActiveStatus);
            TmpStr = strchr(TmpStr, ',') + 1;
         }
         TmpStr = strstr(ParamStr, "AO:");
         TmpStr = strchr(TmpStr, ':') + 1;
         for(Indx = 1; Indx < NUM_AOUT; Indx++){
        	 FuncIDLog = FuncID = atoi(TmpStr);
            TmpStr = strchr(TmpStr, ',') + 1;
            ActiveStatus = atoi(TmpStr);
            if((USERIOConfig->AnalogOutUse[Indx] !=  FuncID) || (ActiveStatus != USERIOConfig->AnalogOutUseStatus[Indx])){
            	if(ActiveStatus == FALSE)
            		FuncIDLog = 0;
                 Log::WriteEventLog(EVENT_USERIO_DEFINED, AoutPins[Indx], FuncIDLog);
            }
            UserIO::AssignAnalogOutputFunction((ANALOG_OUTPUTS)Indx, FuncID, ActiveStatus);
            TmpStr = strchr(TmpStr, ',') + 1;
         }
         if(Cmd == REQ_SAVE)
            CustomDefaultUSERIOConfig = *USERIOConfig;
         TmpStr = strstr(TmpStr, "&lowselect");
         TmpStr = strchr(TmpStr, ',') + 1;
         WriteLowSelectEvent = (BOOLEAN)atoi(TmpStr);
         DcxLogUserID = DcxWebUserID;
         if(WriteLowSelectEvent)
        	 Log::WriteEventLog(EVENT_ACTIVELOWDIGIN);
         __FRAM_START = DigOutputsActiveConfiguration;
         UserIO::SetCableDetectEnabledFlag((BOOLEAN)CableDetectInput);
         DcxLogUserID = 0;
         strcpy(Response, "OK");
      }
      Cmd = REQ_NONE;
      ParamStr = 0;
   }
   else
      strcpy(Response, "NOK");
   DcxLogUserID = 0;
}

/* void WebSite::ConfigurationIPSetupGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to IP Setup  page in a format which javascript code on this page
 *   is expecting.This function gets called during IP Setup page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array(ignored).
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::ConfigurationIPSetupGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   SINT8 Temp[18];
   SINT32 NetworkConfigJS = CurrNetworkConfig;//network configuration to be sent to javascript code on website
   UINT8 Systype = ADAVANCEDSYS;
   UINT8 * MACAddr;
   ip_addr TempIP;
   //1= dhcp client; 2 = dhcp server or static configuration selected
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      SetDHCPServerIPDefaults();
      SetStaticIPDefaults();
      SetFBStaticIPDefaults();
      Sysconfig->PbAddress = PB_DEFAULT_ADDR;
      strcpy(Response, "OK!");
   }
   if(((MacchipData.CurrSystemType == SYSTEM_BAS) || (MacchipData.CurrSystemType == SYSTEM_FIELDBUS) || (MacchipData.CurrSystemType == SYSTEM_FIELDBUS_HD))
      && (HilscherSlaveCardDetected == TRUE)){
      if(FieldBusType == HW_ASSEMBLY_PROFIBUS)
    	  Systype = PROFIBUSSYS;
      else if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
    	  Systype = ETHERNETIPSYS;
   }
   strcat(Response, "IPAddress:D,");
   strcat(Response, CyGOS::GetIPAddressStr(Interface));
   strcat(Response, "@SubnetMask:D,");
   strcat(Response, CyGOS::GetSubnetMaskStr(Interface));
   strcat(Response, "@GateWay:D,");
   if(CurrNetworkConfig == configDhcpClient)
      strcat(Response, CyGOS::GetGatewayAddressStr(Interface));
   else if(CurrNetworkConfig == configDhcpServer){
      NetworkConfigJS = configStatic;
      strcat(Response, Sysconfig->DhcpConfig.gateway.ToString());
   }
   else if(CurrNetworkConfig == configStatic)
      strcat(Response, Sysconfig->Gw.ToString());
   strcat(Response, "@ipconfig:RB,");
   sprintf(Temp, "%ld", NetworkConfigJS);
   strcat(Response, Temp);
   strcat(Response, "@dhcpserver:CB,");
   sprintf(Temp, "%d", (CurrNetworkConfig == configDhcpServer));
   strcat(Response, Temp);
   MACAddr = CyGOS::GetMacAddress(Interface);
   sprintf(Temp, "%02X-%02X-%02X-%02X-%02X-%02X",
   MACAddr[0], MACAddr[1], MACAddr[2], MACAddr[3], MACAddr[4], MACAddr[5]);
   strcat(Response, "@MAC:S,");
   strcat(Response, Temp);
   sprintf(&Response[strlen(Response)], "@SystemType:Sys,%ld", (SINT32)Systype);
   switch(Systype){
      case PROFIBUSSYS:
         sprintf(&Response[strlen(Response)], "@PFAddress:D,%ld", (SINT32)Sysconfig->PbAddress);
      break;
      case ETHERNETIPSYS:
         strcat(Response, "@FBipconfig:RB,");
         sprintf(&Response[strlen(Response)], "%d", FBCurrNetworkConfig);
         //Show the actual IP of card
         TempIP.addr = Sysconfig->FBSystemIP.addr;
         strcat(Response, "@FBIPAddress:D,");
         strcat(Response, TempIP.ToString());
         TempIP.addr = Sysconfig->FBNetmask.addr;
         strcat(Response, "@FBSubnetMask:D,");
         strcat(Response, TempIP.ToString());
         TempIP.addr = Sysconfig->FBGw.addr;
         strcat(Response, "@FBGateWay:D,");
         strcat(Response, TempIP.ToString());
      break;
      default:break;
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ConfigurationIPSetupPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function is handle the IPSetup Parameters values posted from IPSetup Webpage.This functions is called from Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript).
 *
 *  Exit condition: None.
 */
void WebSite::ConfigurationIPSetupPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
      NetworkConfiguration Config;
      UINT8 Systype = ADAVANCEDSYS;
      SINT8 ParamVal[100];
      SINT8 * TempPtr1, * TempPtr2;
      UINT32 SysIP, SubnetIP, GatewayIP;
      TempPtr1 = strstr(ParamStr, "&ipconfig:");
      TempPtr2 = strchr(TempPtr1, ':') + 1;
      Config = (NetworkConfiguration)atoi(TempPtr2);
      if(Config <= configDefault){
         Sysconfig->CurrNetworkConfig = Config;
         CyGOS::WriteSystemEvent(EVENT_IPCONFIG_CHANGED, __LINE__,Sysconfig->CurrNetworkConfig, (('W' << 24) | ('e' << 16) | ('b' << 8) | ('s')), true);
         if(Config == configDhcpClient) {}  //ignore other values
         else{
            //Set the values for DHCP Server configuration
            TempPtr1 = strstr(ParamStr,"IPAddress:");
            TempPtr1 = strchr(TempPtr1,':') + 1;   //Points to IP Address Value
            TempPtr2 = strstr(TempPtr1, "&SubnetMask:");
            if((TempPtr2 - TempPtr1) < MAXIPSTRLEN){
               strncpy(ParamVal, TempPtr1, TempPtr2 - TempPtr1);
               SysIP = IPStrtoUint(ParamVal);
               TempPtr1 = strstr(ParamStr,"SubnetMask:");
               TempPtr1 = strchr(TempPtr1,':') + 1;   //Points to Subnet mask Value
               TempPtr2 = strstr(TempPtr1, "&GateWay:");
               if((TempPtr2 - TempPtr1) < MAXIPSTRLEN){
                  strncpy(ParamVal, TempPtr1, TempPtr2 - TempPtr1);
                  SubnetIP = IPStrtoUint(ParamVal);
                  TempPtr1 = strstr(ParamStr,"GateWay:");
                  TempPtr1 = strchr(TempPtr1,':') + 1;  //Points to gateway Value
                  TempPtr2 = strstr(TempPtr1, "&ipconfig:");
                  if((TempPtr2 - TempPtr1) < MAXIPSTRLEN){
                     strncpy(ParamVal, TempPtr1, TempPtr2 - TempPtr1);
                     GatewayIP = IPStrtoUint(ParamVal);
                     if(Config == configDhcpServer){
                        Sysconfig->SystemIP = SysIP;
                        Sysconfig->DhcpConfig.dns = SysIP;  //NNS server is same
                        Sysconfig->DhcpConfig.gateway.addr = GatewayIP;
                        Sysconfig->DhcpConfig.netmask.addr = SubnetIP;
                        Sysconfig->DhcpConfig.poolStart.addr = (Sysconfig->SystemIP.addr & Sysconfig->DhcpConfig.netmask.addr) + htonl(1);
                     }
                     if(Config == configStatic){
                        Sysconfig->SystemIPStatic = SysIP;
                        Sysconfig->Netmask.addr = SubnetIP;
                        Sysconfig->Gw.addr = GatewayIP;
                     }
                  }
               }
            }
         }
      }
      if(((MacchipData.CurrSystemType == SYSTEM_BAS) || (MacchipData.CurrSystemType == SYSTEM_FIELDBUS) || (MacchipData.CurrSystemType == SYSTEM_FIELDBUS_HD))
         && (HilscherSlaveCardDetected == TRUE)){
         if(FieldBusType == HW_ASSEMBLY_PROFIBUS) Systype = PROFIBUSSYS;
         else if(FieldBusType == HW_ASSEMBLY_ETHERNETIP) Systype = ETHERNETIPSYS;
      }
      switch(Systype){
         case PROFIBUSSYS:  //HW_ASSEMBLY_PROFIBUS
            TempPtr1 = strstr(ParamStr, "&PFAddress:");
            TempPtr1 = strchr(TempPtr1, ':') + 1;
            Sysconfig->PbAddress = atoi(TempPtr1);
         break;
         case ETHERNETIPSYS:  //HW_ASSEMBLY_ETHERNETIP
            TempPtr1 = strstr(ParamStr, "&FBipconfig:");
            TempPtr2 = strchr(TempPtr1, ':') + 1;
            Config = (NetworkConfiguration)atoi(TempPtr2);
            if(Config <= configBootP){
               Sysconfig->FBCurrNetworkConfig = Config;
               if(Config == configStatic){
                  //Set the values for DHCP Server configuration
                  TempPtr1 = strstr(ParamStr,"FBIPAddress:");
                  TempPtr1 = strchr(TempPtr1,':') + 1;   //Points to IP Address Value
                  TempPtr2 = strstr(TempPtr1, "&FBSubnetMask:");
                  if((TempPtr2 - TempPtr1) < MAXIPSTRLEN){
                     strncpy(ParamVal, TempPtr1, TempPtr2 - TempPtr1);
                     SysIP = IPStrtoUint(ParamVal);
                     TempPtr1 = strstr(ParamStr,"FBSubnetMask:");
                     TempPtr1 = strchr(TempPtr1,':') + 1;   //Points to Subnet mask Value
                     TempPtr2 = strstr(TempPtr1, "&FBGateWay:");
                     if((TempPtr2 - TempPtr1) < MAXIPSTRLEN){
                        strncpy(ParamVal, TempPtr1, TempPtr2 - TempPtr1);
                        SubnetIP = IPStrtoUint(ParamVal);
                        TempPtr1 = strstr(ParamStr,"FBGateWay:");
                        TempPtr1 = strchr(TempPtr1,':') + 1;  //Points to gateway Value
                        TempPtr2 = strstr(TempPtr1, "&FBipconfig:");
                        if((TempPtr2 - TempPtr1) < MAXIPSTRLEN){
                           strncpy(ParamVal, TempPtr1, TempPtr2 - TempPtr1);
                           GatewayIP = IPStrtoUint(ParamVal);
                           Sysconfig->FBSystemIP = SysIP;
                           Sysconfig->FBNetmask.addr = SubnetIP;
                           Sysconfig->FBGw.addr = GatewayIP;
                        }
                     }
                  }
               }
            }
         break;
         default: break;
      }
      RegUpdateCmdFlag = TRUE;
      strcpy(Response,"OK");
   }
   else
      strcpy(Response,"NOK");
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ConfigurationPasscodesGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to Passcode page in a format which javascript code on this page
 *   is expecting.This function gets called during Passcode page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array(ignored).
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::ConfigurationPasscodesGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   UINT32 NValidUsers, Indx, Indx1;
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE))
      SetDefaultPasscodeConfiguration();
   NValidUsers = GetUsersDetail(TempUserBuff);
   sprintf(&Response[strlen(Response)], "%d@", IsPasscodeRequired());
   for(Indx = 0; Indx < NValidUsers; Indx++){
      sprintf(&Response[strlen(Response)], "User%ld#", Indx + 1);
      //Don't have null space for user iD field
      for(Indx1 = 0; Indx1 < MAX_LEN_USERID; Indx1++)
         sprintf(&Response[strlen(Response)], "%c", TempUserBuff[Indx].UserID[Indx1]);
      sprintf(&Response[strlen(Response)], "#%ld", TempUserBuff[Indx].PassCode);
      sprintf(&Response[strlen(Response)], "#%ld",(SINT32)TempUserBuff[Indx].UserPrivilege);
      strcat(Response,":");
   }

   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ConfigurationPasscodesPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function handles the Parameters values posted from Passcode Webpage.This functions is called from Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript).
 *
 *  Exit condition: None.
 */
void WebSite::ConfigurationPasscodesPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
	  BOOLEAN ExitFlag = FALSE;
      UINT32 Passcode;
      PRIVILEGE  Privilege;
      SINT8 Userprivilege[MAX_TEMPSTRLEN], Userid[MAX_TEMPSTRLEN];
      SINT32  Index;
      SINT8 * TmpStr = strstr(ParamStr, "@");//TODO: Validate string present
      SINT8 * TmpStr2, * TmpStr3;
      InvalidateUserEntries();
      for(Index = 0; (Index < MAX_USERS) && (ExitFlag == FALSE); Index++){
         TmpStr = strchr(TmpStr, '@') + 1;
         TmpStr = strchr(TmpStr, '=') + 1;
         TmpStr2 = strstr( TmpStr, "@");
         strncpy(Userid, TmpStr, CopyLength(MAX_LEN_USERID, TmpStr2 - TmpStr, __LINE__));
         TmpStr = strchr(TmpStr, '@') + 1;
         TmpStr = strchr(TmpStr, '=') + 1;
         Passcode = atoi(TmpStr);
         TmpStr = strchr(TmpStr, '@') + 1;
         TmpStr = strchr(TmpStr, '=') + 1;
         TmpStr3 = strchr(TmpStr, '@');
         strncpy(Userprivilege, TmpStr, CopyLength(sizeof(Userprivilege) - 1, TmpStr3 - TmpStr,__LINE__));
         if(!strcmp(Userprivilege, "ADMIN"))
        	 Privilege = PRIVILEGE_ADMIN;
         else if(!strcmp(Userprivilege, "USER"))
        	 Privilege = PRIVILEGE_USER;
         else
        	 Privilege = PRIVILEGE_INVALID;
         if(strlen(Userid)!= 0)
            AddUser((UINT8 *)Userid, Passcode, Privilege);
         if(TmpStr3){
			 if(strncmp(TmpStr3 , "@&PasscodeRequired", strlen("@&PasscodeRequired")) == 0)
				 ExitFlag = TRUE;
         }
      }
      TmpStr = strstr(ParamStr, "&PasscodeRequired=");
      if(TmpStr){
    	  TmpStr = strchr(TmpStr, '=') + 1;
    	  SetPasscodeRequired(atoi(TmpStr));
      }
      strcpy(Response,"OK");
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ConfigurationAlarmGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to Alarm page in a format which javascript code on this page
 *   is expecting.This function gets called during Alarm page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array(ignored).
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::ConfigurationAlarmGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE))
      SetDefaultAlarms();
   sprintf(&Response[strlen(Response)],
     "OverLoads,%ld@"
     "Cutoff,%ld@"
     "Setup,%ld@"
     "CycleModify,%ld@"
     "Warning,%ld@"
     "ProcessLimit,%ld@"
     "EQPfailure,%ld@"
     "NoCycle,%ld@"
     "CommFailure,%ld@"
     "HW,%ld@"
     "NCOverload,%ld",
     (SINT32)Alarmconfig->AlarmAction[OVERLOADS],
     (SINT32)Alarmconfig->AlarmAction[CUTOFFS],
     (SINT32)Alarmconfig->AlarmAction[SETUP],
     (SINT32)Alarmconfig->AlarmAction[CYCLE_MODIFIED],
     (SINT32)Alarmconfig->AlarmAction[WARNINGS],
     (SINT32)Alarmconfig->AlarmAction[PROCESS_LIMITS],
     (SINT32)Alarmconfig->AlarmAction[EQUIPMENT_FAILURES],
     (SINT32)Alarmconfig->AlarmAction[NOCYCLE],
     (SINT32)Alarmconfig->AlarmAction[COMM_FAILURES],
     (SINT32)Alarmconfig->AlarmAction[HARDWARE_FAILURE],
     (SINT32)Alarmconfig->AlarmAction[NON_CYCLE_OVERLOADS]);
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ConfigurationAlarmPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function handles the Parameters values posted from Alarms Webpage.This functions is called by Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript).
 *
 *  Exit condition: None.
 */
void WebSite::ConfigurationAlarmPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
      SINT8 * TempStr = strstr(ParamStr, "&OverLoads=");
      Alarmconfig->AlarmAction[OVERLOADS] = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Cutoff=");
      Alarmconfig->AlarmAction[CUTOFFS] = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Setup=");
      Alarmconfig->AlarmAction[SETUP] = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr,"&CycleModify=");
      Alarmconfig->AlarmAction[CYCLE_MODIFIED] = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Warning=");
      Alarmconfig->AlarmAction[WARNINGS] = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&ProcessLimit=");
      Alarmconfig->AlarmAction[PROCESS_LIMITS] = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&EQPfailure=");
      Alarmconfig->AlarmAction[EQUIPMENT_FAILURES] = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&NoCycle=");
      Alarmconfig->AlarmAction[NOCYCLE] = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&CommFailure=");
      Alarmconfig->AlarmAction[COMM_FAILURES] = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&HW=");
      Alarmconfig->AlarmAction[HARDWARE_FAILURE] = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&NCOverload=");
      Alarmconfig->AlarmAction[NON_CYCLE_OVERLOADS] = atoi(strchr(TempStr, '=') + 1);
//      SavePreset(CurrPreset, CurrHsPreset);
//      RegUpdateCmdFlag = TRUE;
      strcpy(Response, "OK");
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ConfigurationRFSwitchGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to RFSwitch page in a format which javascript code on this page
 *   is expecting.This function gets called during RFSwitch page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array(ignored).
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::ConfigurationRFSwitchGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE))
      SetDefaultRFswitch();
   sprintf(&Response[strlen(Response)],
      "RFSwitch:RB,%ld@"
      "FeedBack:RB,%ld@"
      "DirectSW:RB,%ld@"
      "FBTimeout:TT,%ld,%ld,%ld@"
      "SwitchDelay:TT,%ld,%ld,%ld@"
      "SwitchTime:TT,%ld,%ld,%ld@"
      "Codded:RB,%ld@",
      (SINT32)Sysconfig->RFSwitch.RFSwitchingType,
      (SINT32)Sysconfig->RFSwitch.RFFeedback,
      (SINT32)Sysconfig->RFSwitch.RFOption,
      (SINT32)Sysconfig->RFSwitch.FeedBackTimeOut,(SINT32)MIN_DELAY,(SINT32)MAX_DELAY,
      (SINT32)Sysconfig->RFSwitch.RFSwitchDelay,(SINT32)MIN_DELAY,(SINT32)MAX_DELAY,
      (SINT32)Sysconfig->RFSwitch.RelaySwitchTime,(SINT32)MIN_DELAY,(SINT32)MAX_DELAY,
      (SINT32)Sysconfig->RFSwitch.RFOutputType);
   
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ConfigurationRFSwitchPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function handles the Parameters values posted from RFSwitch Webpage.This functions is called by Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript).
 *
 *  Exit condition: None.
 */
void WebSite::ConfigurationRFSwitchPostDatahandler (SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
      SINT8 * TempStr = strstr(ParamStr, "&RFSwitch=");
      Sysconfig->RFSwitch.RFSwitchingType = (RFSwitching)atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&FeedBack=");
      Sysconfig->RFSwitch.RFFeedback = (RFSwitchingFeedBack)atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&DirectSW=");
      Sysconfig->RFSwitch.RFOption = (RFSwitchOption)atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&FBTimeout=");
      Sysconfig->RFSwitch.FeedBackTimeOut = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&SwitchDelay=");
      Sysconfig->RFSwitch.RFSwitchDelay = Freq_Mode(atoi(strchr(TempStr, '=') + 1));
      TempStr = strstr(ParamStr, "&SwitchTime=");
      Sysconfig->RFSwitch.RelaySwitchTime = Freq_Mode(atoi(strchr(TempStr, '=') + 1));
      TempStr = strstr(ParamStr, "&Codded=");
      Sysconfig->RFSwitch.RFOutputType = RFSwitchingOutput(atoi(strchr(TempStr, '=') + 1));
//      SavePreset(CurrPreset, CurrHsPreset);
//      RegUpdateCmdFlag = TRUE;
      strcpy(Response, "OK");
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::BransonSecretValuesGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to SECRET VALUES page in a format which javascript code on this page
 *   is expecting.This function gets called during SECRET VALUES page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array(ignored).
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::BransonSecretValuesGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   DCPSpeedControl LastSel = Sysconfig->DcpSpeedControl;
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      DcxLogUserID = DcxWebUserID;
      SetDefaultSecretValues();
      Log::WriteEventLog(EVENT_RESTORE_DEFAULTS, BRANSON_SECRET_VALUES);
      if(LastSel != Sysconfig->DcpSpeedControl){
    	   UpdatePresetsOnLoopSpeedSwitching();
    	   //TODO: Does DCP Need Live update if switched between CUSTOM and Standard
    	   SetDCPTimingParam(REQ_RESTORE);
      }
      RegUpdateCmdFlag = TRUE;
      DcxLogUserID = 0;
   }
   sprintf(&Response[strlen(Response)],
      "%ld@"
      "BrakeTime:TT,%ld,%ld,%ld@"
      "TargetAmplitude:TT,%ld,%ld,%ld@"
	  "LoopSpeed:RB,%ld@",
      (SINT32)HIDDEN,
      (SINT32)ParameterSet[CurrPreset].WcParam.EBTime, (SINT32)ParameterSet[MinPSNum].WcParam.EBTime, (SINT32)ParameterSet[MaxPSNum].WcParam.EBTime,
      (SINT32)ParameterSet[CurrPreset].WcParam.EBAmp, (SINT32)ParameterSet[MinPSNum].WcParam.EBAmp, (SINT32)ParameterSet[MaxPSNum].WcParam.EBAmp,
      (SINT32)Sysconfig->DcpSpeedControl
      );
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::BransonSecretValuesPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function handles the Parameters posted from SecretValues Webpage.This functions is called by Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::BransonSecretValuesPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   DCPSpeedControl LastSel = Sysconfig->DcpSpeedControl;
   if(TokenAvailable){
      DcxLogUserID = DcxWebUserID;
      SINT8 * TempStr = strstr(ParamStr, "BrakeTime=");
      CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, EBTime, (atoi(strchr(TempStr, '=') + 1)), WcParamEBTime, (atoi(strchr(TempStr, '=') + 1)), 0);
      TempStr = strstr(ParamStr, "&TargetAmplitude=");
      CHECKANDLOG_PRESETCHANGE(CurrPreset, WcParam, EBAmp, (atoi(strchr(TempStr, '=') + 1)), WcParamEBAmp, (atoi(strchr(TempStr, '=') + 1)), 0);
      CalculateAndSavePresetCRC(SAVECRC_WC);
      TempStr = strstr(ParamStr, "&LoopSpeed=");
      Sysconfig->DcpSpeedControl = (DCPSpeedControl)(atoi(strchr(TempStr, '=') + 1));
      //Sanity check to make sure things are OK from web
      if((Sysconfig->DcpSpeedControl > SPEEDCONTROL_MAX) ||
     		   (Sysconfig->DcpSpeedControl < SPEEDCONTROL_STANDARD))
     	   Sysconfig->DcpSpeedControl = SPEEDCONTROL_STANDARD;
        pprintf("\n Loop Control %d ", Sysconfig->DcpSpeedControl);

      if(LastSel != Sysconfig->DcpSpeedControl){
    	  UpdatePresetsOnLoopSpeedSwitching();
    	  //TODO: Does DCP Need live update if switched from CUSTOM to Standard
    	  SetDCPTimingParam(REQ_NONE);
      }
      else
    	  strcpy(Response, "OK");
      RegUpdateCmdFlag = TRUE;
      DcxLogUserID = 0;

   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* BOOLEAN GetCurrentStateOnManufacturing(void)
 *
 * Purpose:
 *   This function provides the information about current state of machine on manufacturing.This function is called by BransonSystemTestGetDatahandler.
 *
 * Entry condition: None.
 *
 * Exit condition: Return TRUE if Desired State Else Returns FALSE.
 */
BOOLEAN GetCurrentStateOnManufacturing(void)
{
   BOOLEAN RetVal = FALSE;
   if(UserIO::GetReadyFlag() || CurrentCycleParam->MfCycling || UserIO::GetAlarmStatus() || WebSite::mfTestFlag)
      RetVal = TRUE;
   return RetVal;
}

/* void WebSite::ProcessMFStartStopCommand(SINT8 * Param)
 *
 * Purpose:
 *   This function is used to temporarily hold some of the parameters required for cycling.This function is called by BransonSystemTestGetDatahandler.
 *
 * Entry condition: Param- String containing cycle parameters.
 *
 * Exit condition: None.
 */
void WebSite::ProcessMFStartStopCommand(SINT8 * Param)
{
   SINT8 * Temp = strstr(Param, "&cyctime:") + strlen("&cyctime:");
   TempCurrentCycleParam.TotalCycleTime = atoi(Temp);
   Temp = strstr(Param, "&sontime:") + strlen("&sontime:");
   TempCurrentCycleParam.SonicsOnTimePerWeldCycle = atoi(Temp);
   Temp = strstr(Param, "&sofftime:") + strlen("&sofftime:");
   TempCurrentCycleParam.SonicsOffTimePerWeldCycle = atoi(Temp);
   Temp = strstr(Param, "&weldamp:") + strlen("&weldamp:");
   TempCurrentCycleParam.Amplitude = atoi(Temp);
   Temp = strstr(Param, "&alarmrst:") + strlen("&alarmrst:");
   TempCurrentCycleParam.StopOnAlarm = atoi(Temp);
}

/* BOOLEAN WebSite::CheckForCycleParameterChange(void)
 *
 * Purpose:
 *   This function is used to check if Cycle parameters are modified by the user or not.This function is called by BransonSystemTestGetDatahandler.
 *
 * Entry condition: None.
 *
 * Exit condition: TRUE if cycle parameters sent from the website are not the same in FRAM else returns FALSE.
 */
BOOLEAN WebSite::CheckForCycleParameterChange(void)
{
   BOOLEAN Result = FALSE;
   if(CurrentCycleParam->TotalCycleTime != TempCurrentCycleParam.TotalCycleTime){
      CurrentCycleParam->TotalCycleTime = TempCurrentCycleParam.TotalCycleTime;
      Result = TRUE;
   }
   if(CurrentCycleParam->SonicsOnTimePerWeldCycle != TempCurrentCycleParam.SonicsOnTimePerWeldCycle){
      CurrentCycleParam->SonicsOnTimePerWeldCycle = TempCurrentCycleParam.SonicsOnTimePerWeldCycle;
      Result = TRUE;
   }
   if(CurrentCycleParam->SonicsOffTimePerWeldCycle != TempCurrentCycleParam.SonicsOffTimePerWeldCycle){
      CurrentCycleParam->SonicsOffTimePerWeldCycle = TempCurrentCycleParam.SonicsOffTimePerWeldCycle;
      Result = TRUE;
   }
   if(CurrentCycleParam->Amplitude != TempCurrentCycleParam.Amplitude){
      CurrentCycleParam->Amplitude = TempCurrentCycleParam.Amplitude;
      Result = TRUE;
   }
   if(CurrentCycleParam->StopOnAlarm != TempCurrentCycleParam.StopOnAlarm){
      CurrentCycleParam->StopOnAlarm = TempCurrentCycleParam.StopOnAlarm;
      Result = TRUE;
   }
   if(MFcycleResults->Cyclestatus != STATESTOPPED)
      Result = TRUE;
   return Result;
}

/* void SetManFDefaultFromWeb(void)
 *
 * Purpose:
 *   This function sets up the factory defaults from WEB.This function is called by BransonSystemTestGetDatahandler.
 *
 * Entry condition: None.
 *
 * Exit condition: None.
 */
void SetManFDefaultFromWeb(void)
{
   DCXFunc.MFCycleStart = CurrentCycleParam->MfCycling = FALSE;
   SetDefaultValues();
   SetMaxMinValues();
  // ResetCounters();
   SetMinMaxCycleParam();
   SetDefaultCycleParam();
   ResetManFErrorCounter();
   MemoryReset = TRUE;
   Log::ResetLog();
   SetDHCPServerIPDefaults();
   SetStaticIPDefaults();
   RegUpdateCmdFlag = TRUE;
   Log::WriteEventLog(EVENT_RESTORE_DEFAULTS, BRANSON_MANUFACTURING_PAGE);
}

/* void WebSite::BransonSystemTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to System Test page in a format which javascript code on this page
 *   is expecting.This function gets called during System Test page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::BransonSystemTestGetDatahandler (SINT8 * ParamStr, WebCommand Cmd)
{
   SINT8 * CurrStat = 0;
   SINT32 RemainingCycleTime = 0;
   mfTimeOut = MF_TIMEOUT;
   SINT16 CurrIndx = CurrentIndex;

   switch(Cmd){
      case REQ_MFONLOAD:
         if(GetCurrentStateOnManufacturing()){
            mfFlag = TRUE;
            CurrStat = "OK";
            WebIOEnabled = TRUE;
         }
         else
            CurrStat = "BUSY";
			if(CurrentCycleParam->TotalCycleTime > 0){
				 RemainingCycleTime = CurrentCycleParam->TotalCycleTime - SCALEDTIMETOWEBSITE(MFcycleResults->CurrCycleTime);
				 if(MFcycleResults->Cyclestatus != STATECOMPLETED)
					 RemainingCycleTime += MSINMIN;
			}
         sprintf(&Response[strlen(Response)],
            "%ld@%s@"
            "stat:RB,%d@"
            "ipaddr:TT,%s@"
            "snumber:TT,%s@"
            "cyctime:TT,%ld,%ld,%ld@"
            "sonicontime:TT,%ld,%ld,%ld@"
            "sonicofftime:TT,%ld,%ld,%ld@"
            "amplitude:TT,%ld,%ld,%ld@"
            "stoponalarm:CB,%ld@"
            "freqval:TT,%ld@"
            "pwrval:TT,%ld@"
            "ncycles:S,%ld@"
            "nhrs:S,%ld@"
            "crsonicsontime:S,%ld@"
            "nalarms:S,%ld@"
            "crremainingtime:S,%ld@"
            "SelectFreq:DD,%d@"
            "SelectWeldtype:DD,%d@"
            "SetupPwrWatt:TT,%ld@",
            (SINT32)BURNIN,
            CurrStat,
            MFcycleResults->Cyclestatus,
            CyGOS::GetIPAddressStr(Interface),
            MacchipData.SerialNo,
            (SINT32)CurrentCycleParam->TotalCycleTime,(SINT32)MinCycleParam.TotalCycleTime,(SINT32)MaxCycleParam.TotalCycleTime,
            (SINT32)CurrentCycleParam->SonicsOnTimePerWeldCycle,(SINT32)MinCycleParam.SonicsOnTimePerWeldCycle,(SINT32)MaxCycleParam.SonicsOnTimePerWeldCycle,
            (SINT32)CurrentCycleParam->SonicsOffTimePerWeldCycle,(SINT32)MinCycleParam.SonicsOffTimePerWeldCycle,(SINT32)MaxCycleParam.SonicsOffTimePerWeldCycle,
            (SINT32)CurrentCycleParam->Amplitude,(SINT32)MinCycleParam.Amplitude,(SINT32)MaxCycleParam.Amplitude,
            (SINT32)CurrentCycleParam->StopOnAlarm,
            (SINT32)MFcycleResults->ResultantFrequency,
            (SINT32)MFcycleResults->ResultantPower,
            (SINT32)MFcycleResults->CyclesCount, /*20*/
            (SINT32)(SCALEDTIMETOWEBSITE(MFcycleResults->CurrCycleTime)),
            (SINT32)(SCALEDTIMETOWEBSITE(MFcycleResults->SonicsOnTime)),
            (SINT32)MFcycleResults->AlarmsCount,
             RemainingCycleTime,
             MacchipData.Freq,
            MacchipData.CurrSystemType,
            (SINT32)(MacchipData.Power)
         );
      break;
      case REQ_RESTOREDEFAULTS:
         LogData2 = LOG_CLEARED_BY_CRC_ERROR_OR_MANUFACTURING_PAGE;
         SetManFDefaultFromWeb();
         strcpy(Response, "OK");
      break;
      case REQ_CLEAR_ALARMLOGMF:
    	 DcxLogUserID = DcxWebUserID;
         LogData2 = LOG_CLEARED_BY_CRC_ERROR_OR_MANUFACTURING_PAGE;
         Log::ResetLog(CLEARALARM);
         strcpy(Response, "OK");
         DcxLogUserID = 0;
      break;
      case REQ_CLEAR_EVENTLOGMF:
    	  DcxLogUserID = DcxWebUserID;
         LogData2 = LOG_CLEARED_BY_CRC_ERROR_OR_MANUFACTURING_PAGE;
         Log::ResetLog(CLEAREVENT);
         strcpy(Response, "OK");
         DcxLogUserID = 0;
      break;
      case REQ_RESET_ALARMCOUNTER:
         ResetManFErrorCounter(TRUE);
         strcpy(Response, "OK");
      break;
      case REQ_STARTSTOPCYCLE:
    	 if(IsManualFunctionAvailable() == TRUE){
			 if(CurrentCycleParam->MfCycling){
				MFcycleResults->Cyclestatus = STATESTOPPED;
				SonicsOff();
				SonicsOnCounter = 0;
				SonicsOffCounter = 0;
				CurrentCycleParam->MfCycling = FALSE;
				DCXFunc.MFCycleStart = CurrentCycleParam->MfCycling;
				ControlVariable &= ~MANUFACT_WELD;
				strcpy(Response, "STOPPED");
				CurrentCycleParam->CRC = ByteCRC((UINT8*)CurrentCycleParam, sizeof(CycleParam) - sizeof(CurrentCycleParam->CRC));
				RegAmplitudeUpdateFlag = TRUE;
			 }
			 else{
				if(UserIO::GetReadyFlag()){
				   ProcessMFStartStopCommand(ParamStr);
				   if(CheckForCycleParameterChange()){
					  memset(MFcycleResults, 0, sizeof(CycleResults));
					  if((CurrentCycleParam->SonicsOnTimePerWeldCycle == 0) || (CurrentCycleParam->SonicsOffTimePerWeldCycle == 0))
						 TempSonicsOnTimePerWeldCycle = CurrentCycleParam->TotalCycleTime;
					  else
						 TempSonicsOnTimePerWeldCycle = CurrentCycleParam->SonicsOnTimePerWeldCycle;
				   }
				   CurrentCycleParam->MfCycling = TRUE;
				   DCXFunc.MFCycleStart = CurrentCycleParam->MfCycling;
				   ControlVariable |= MANUFACT_WELD;
				   strcpy(Response, "STARTED");
				   CurrentCycleParam->CRC = ByteCRC((UINT8*)CurrentCycleParam, sizeof(CycleParam) - sizeof(CurrentCycleParam->CRC));
				   RegAmplitudeUpdateFlag = TRUE;
				}
				else
				   strcpy(Response, "BUSY");
			 }
    	 }
    	 else
    		 strcpy(Response, "AUTOMODE");
      break;
      case REQ_SAVE_SYSSETUP:
         ProcessMFSavesystemSetupCommand(ParamStr);
         strcpy(Response, "OK");
      break;
      case REQ_MFPOLL:
         if(CurrIndx == ERROR_IDX)
            CurrStat = "ERROR";
         else if(GetCurrentStateOnManufacturing() && (TokenAvailable == TRUE)){
            mfFlag = TRUE;
            WebIOEnabled = TRUE;
            CurrStat = "OK";
         }
         else
            CurrStat = "BUSY";
 		 if(CurrentCycleParam->TotalCycleTime > 0){
 				 RemainingCycleTime = CurrentCycleParam->TotalCycleTime - SCALEDTIMETOWEBSITE(MFcycleResults->CurrCycleTime);
 				 if(MFcycleResults->Cyclestatus != STATECOMPLETED)
 					 RemainingCycleTime += MSINMIN;
 		 }
         sprintf(&Response[strlen(Response)], "%s@"
            "freqval:TT,%ld@"
            "pwrval:TT,%ld@"
            "ncycles:S,%ld@"
            "nhrs:S,%ld@"
            "crsonicsontime:S,%ld@"
            "nalarms:S,%ld@"
            "crremainingtime:S,%ld@"
            "stat:RB,%ld@",
            CurrStat, (SINT32)MFcycleResults->ResultantFrequency, (SINT32)MFcycleResults->ResultantPower,
            (SINT32)MFcycleResults->CyclesCount, (SINT32)(SCALEDTIMETOWEBSITE(MFcycleResults->CurrCycleTime)),
            (SINT32)(SCALEDTIMETOWEBSITE(MFcycleResults->SonicsOnTime)), (SINT32)MFcycleResults->AlarmsCount,
            RemainingCycleTime,
            (SINT32)MFcycleResults->Cyclestatus);
      break;
      case REQ_MFTEST_START:
    	  if(IsManualFunctionAvailable() == TRUE){
			 if(UserIO::GetReadyFlag()){
				strcpy(Response,"TESTSTARTED");
				mfTestFlag = TRUE;
				ControlVariable |= WEBTEST;
				webtimeOutTest = WEBTEST_TIMEOUT;
				DcxLogUserID = DcxWebUserID;
			 }
			 else
				strcpy(Response , "SYSTEMBUSY");
    	  }
    	  else
    		  strcpy(Response , "AUTOMODE");
      break;
      case REQ_MFTEST_STOP:
         strcpy(Response, "TESTSTOPPED");
         ControlVariable &= ~WEBTEST;
         DcxLogUserID = 0;
         mfTestFlag = FALSE;
      break;
      case REQ_MFTEST_POLL:
         webtimeOutTest = MF_TIMEOUT;
         sprintf(Response, "MFTESTPOLLRESPONSE:%d",CheckForCurrentFunction(CurrIndx));
      break;
      case REQ_NONE:
         if(GetCurrentStateOnManufacturing() && (TokenAvailable == TRUE)){
            sprintf(&Response[strlen(Response)],"OK");
            mfFlag = TRUE;
            WebIOEnabled = TRUE;
         }
         else
            sprintf(&Response[strlen(Response)],"BUSY");
      break;
      default:
         strcpy(Response, "ErrCommand"); break;
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::ProcessMFStartStopCommand(SINT8 * Param)
 *
 * Purpose:
 *   This function is used to Save System Setup from Manufacturing Page.This function is called by BransonSystemTestGetDatahandler.
 *
 * Entry condition: Param- String containing IP, Serial number,Frequency and powerLevel.
 *
 * Exit condition: None.
 */
void WebSite::ProcessMFSavesystemSetupCommand(SINT8 * Param)
{
   UINT16 CurrFreq = MacchipData.Freq;
   SINT8 TempVal[MAXTEMPSTRLEN];
   SINT8 * Temp1;
   SINT8 * Temp = strstr(Param, "&freq:") + strlen("&freq:");
   MacchipData.Freq = (FREQUENCY)(atoi(Temp));
   Temp = strstr(Param, "&SetupPwrWatt:") + strlen("&SetupPwrWatt:");
   MacchipData.Power = (atoi(Temp));
   Temp = strstr(Param, "&WelderType:") + strlen("&WelderType:");
   MacchipData.CurrSystemType = (SYSTEMTYPE)(atoi(Temp));
   Temp = strstr(Param,"&ipaddr:") + strlen("&ipaddr:");//Points to IP Address Value
   Temp1 = strstr(Temp, "&snumber:");
   if(Temp1 - Temp < MAXTEMPSTRLEN){
      strncpy(TempVal, Temp, Temp1 - Temp);
      Sysconfig->SystemIPStatic = IPStrtoUint(TempVal);
      Sysconfig->SystemIP = IPStrtoUint(TempVal);
      Sysconfig->DhcpConfig.dns = IPStrtoUint(TempVal);//DNS server is same
      Temp = strstr(Param, "&snumber:") + strlen("&snumber:");
      if(strlen(Temp) < SERIALNUMBER_LEN)
         strcpy((SINT8*)MacchipData.SerialNo, Temp);
      //save power supply and frequency to MAC chip
      //save serial number to MAC chip
      ModifyMACCHIPRWData();
      if(CurrFreq != MacchipData.Freq){
    	 UpdateMidBandAndMinMaxFreq();
         memset(&ResonanceFreqType, 0, sizeof(ResonanceFreqType));
         memset(&ResonanceFreqs, 0, sizeof(ResonanceFreqs));
         ScanDone = FALSE;
         SetColdStartPresetValues();
         Log::WriteEventLog(EVENT_COLD_START, BRANSON_MANUFACTURING_PAGE);
      }
   }
}

/* void WebSite::BransonBoardTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to Board Test page in a format which javascript code on this page
 *   is expecting.This function gets called during Board Test page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::BransonBoardTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd){
   sprintf(&Response[strlen(Response)],
      "%ld@"
      "Overload,%d@"
      "Temperature,%d@"
      "Power,%d@"
      "RFvoltage,%d@"
      "RFcurrent,%d@"
      "Powerinput,%d@"
      "InputTemperature,%d@"
      "ActualAmplitude,%d@"
      "DCCurrent,%d@"
      "Phase,%d@"
      "Sign,%ld@"
      "testFreq,%ld@",
      (SINT32)BOARD,
      RtReceiveData.StatusFlags & OVL_FUNCTION,
      RtReceiveData.StatusFlags & OL_TEMPERATURE,
      RtReceiveData.StatusFlags & OL_POWER,
      RtReceiveData.StatusFlags & OL_VOLTAGE,
      RtReceiveData.StatusFlags & OL_CURRENT,
      RtReceiveData.PowerOut,
      RtReceiveData.Temperature,
      RtReceiveData.AmplitudeOut,
      RtReceiveData.Current,
      RtReceiveData.Phase,
      (SINT32)RtReceiveData.StatReg, //SIGN
      RtReceiveData.FreqOut
   );
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::BransonBoardTestPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function is handle the BoardTest Parameters values posted from BoardTest Webpage.This functions is called by Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::BransonBoardTestPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
      UINT16 BoardTstOP = 0;
      SINT8 * TempStr = strstr(ParamStr, "testFreq");
      TempStr = strchr(TempStr, ':');
      RtSendData.FreqIn = atoi(TempStr+1);
      TempStr = strstr(TempStr, "olrst:");
      TempStr = strchr(TempStr, ':') + 1;
      if(atoi(TempStr)) BoardTstOP = BoardTstOP | OLRST_BIT;
      TempStr = strstr(TempStr, "stop");
      TempStr = strchr(TempStr, ':') + 1;
      if(atoi(TempStr)) BoardTstOP = BoardTstOP | DPSSTOP_BIT;
      TempStr = strstr(TempStr, "phaseA");
      TempStr = strchr(TempStr, ':') + 1;
      if(atoi(TempStr)) BoardTstOP = BoardTstOP | PHASEA_BIT;
      TempStr = strstr(TempStr, "phaseB");
      TempStr = strchr(TempStr, ':') + 1;
      if(atoi(TempStr)) BoardTstOP = BoardTstOP | PHASEB_BIT;
      RtSendData.CmdFlags = BoardTstOP;
      strcpy(Response, "OK");
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

void WebSite::BransonFieldBusTestPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::BransonFieldBusTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to FieldBus page in a format which javascript code on this page
 *   is expecting.This function gets called during FieldBus page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::BransonFieldBusTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   UINT8 Systype = ADAVANCEDSYS, Index = 0;
   if(HilscherSlaveCardDetected == TRUE){
      if(FieldBusType == HW_ASSEMBLY_PROFIBUS)
    	  Systype = PROFIBUSSYS;
      else if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
    	  Systype = ETHERNETIPSYS;
   }
   sprintf(&Response[strlen(Response)],"%ld@Sys:%ld@", (SINT32)FIELDBUSTEST, (SINT32)Systype);
   if((Systype == PROFIBUSSYS) || (Systype == ETHERNETIPSYS)){
      sprintf(&Response[strlen(Response)],
         "ZSW1:%ld@"
         "ZSW2:%ld@"
         "STW1:%ld@"
         "STW2:%ld@",
         (SINT32)FbusIO.PZDOut.PZDu16Out.ZSWOut1, (SINT32)FbusIO.PZDOut.PZDu16Out.ZSWOut2,
         (SINT32)FbusIO.PZDIn.PZDu16In.STWIn1, (SINT32)FbusIO.PZDIn.PZDu16In.STWIn2);
   }
   if(Systype == PROFIBUSSYS){
      sprintf(&Response[strlen(Response)],
         "AKIn:%ld@"
         "PNUIn:%ld@"
         "INDIn:%ld@"
         "PWEhIn:%ld@"
         "PWElIn:%ld@"
         "AKOut:%ld@"
         "PNUOut:%ld@"
         "INDOut:%ld@"
         "PWEhOut:%ld@"
         "PWElOut:%ld",
         (SINT32)(FbusIO.PKWIn.PKWu16In.Pke >> 12), (SINT32)(FbusIO.PKWIn.PKWu16In.Pke & 0x0FFF),
         (SINT32)FbusIO.PKWIn.PKWu16In.Ind,
         (SINT32)FbusIO.PKWIn.PKWu16In.PWE.PWEu16In.PweHigh, (SINT32)FbusIO.PKWIn.PKWu16In.PWE.PWEu16In.PweLow,
         (SINT32)(FbusIO.PKWOut.PKWu16Out.Pke >> 12), (SINT32)(FbusIO.PKWOut.PKWu16Out.Pke & 0x0FFF),
         (SINT32)FbusIO.PKWOut.PKWu16Out.Ind,
         (SINT32)FbusIO.PKWOut.PKWu16Out.PWE.PWEu16Out.PweHigh, (SINT32)FbusIO.PKWOut.PKWu16Out.PWE.PWEu16Out.PweLow);
   }
   else if(Systype == ETHERNETIPSYS){
      sprintf(&Response[strlen(Response)],
         "EIPService:%ld@"
         "EIPClass:%ld@"
         "EIPInstance:%ld@"
         "EIPAttribute:%ld",
         (SINT32)(LE32_TO_HOST(LastCl3ResPkt.tData.ulService)),  (SINT32)(LE32_TO_HOST(LastCl3ResPkt.tData.ulObject)),
         (SINT32)(LE32_TO_HOST(LastCl3ResPkt.tData.ulInstance)), (SINT32)(LE32_TO_HOST(LastCl3ResPkt.tData.ulAttribute)));
         sprintf(&Response[strlen(Response)], "@EIPRequest:");
      for(Index = 0; Index < ServiceDataSizeIn; Index++){
         sprintf(&Response[strlen(Response)], "%02X ", LastCl3IndPkt.tData.abData[Index]);
      }
      sprintf(&Response[strlen(Response)], "@EIPResponse:");
      for(Index = 0; Index < ServiceDataSizeOut; Index++){
         sprintf(&Response[strlen(Response)], "%02X ", LastCl3ResPkt.tData.abData[Index]);
      }
      sprintf(&Response[strlen(Response)], "@EIPStatus:%ld", (SINT32)(LE32_TO_HOST(LastCl3ResPkt.tData.ulGRC)));
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

void SetDCPTimingParam(WebCommand Cmd){
      BOOLEAN TempFlag = FALSE;
      NRTcmd = (NrtQ){CMD_DCPTIMEPARAMWRITE, WRITE_REQ, 0};
	  SetDCPTimeControlParams();
	  TempFlag = NRTModbus::thisPtr -> Nrtq.Write(0, NRTcmd, 0);
	  pprintf(" status of CMD_DCPTIMEPARAMWRITE %d ", TempFlag);
	  if(Cmd == REQ_NONE){
		  if((TempFlag == TRUE))
			  strcpy(Response, "OK");
		  else
			  strcpy(Response, "TIME OUT");
	  }
}

/* void WebSite::BransonAdvRnDTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to Advanced RnD page in a format which javascript code on this page
 *   is expecting.This function gets called during Advanced RnD page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::BransonAdvRnDTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      SetDefaultAdvRnd();
      if(Sysconfig->DcpSpeedControl == SPEEDCONTROL_CUSTOM)//Send update to DCP only when custom speed is selected
    	  SetDCPTimingParam(REQ_RESTORE);
   }
   sprintf(&Response[strlen(Response)],
    "%ld@"
    "Gp1Value1:TT,%ld,%d,%d@"
    "Gp1Value2:TT,%ld,%d,%d@"
    "Gp1Value3:TT,%ld,%d,%d@"
    "Gp1Value4:TT,%ld,%d,%d@"
    "Gp1Value5:TTOP,%ld,%d,%d@"
    "Gp1Value6:TT,%ld,%u,%u@"
    "Gp2Value1:TT,%ld,%u,%u@"
    "Gp2Value2:TT,%ld,%u,%u@"
    "Gp2Value3:TT,%ld,%u,%u@"
    "Gp2Value4:TT,%ld,%u,%u@"
    "Gp2Value5:TT,%ld,%u,%u@"
    "Gp2Value6:TT,%ld,%u,%u@"
    "RevDigitalOP:CB,%ld@"
    "Flag2:CB,%ld@"
    "Flag3:CB,%ld@"
    "Flag4:CB,%ld@"
    "Flag5:CB,%ld@"
    "Flag6:CB,%ld@",
    (SINT32)SCOTT,
    (SINT32)AdvanceConfigData->Advrnd.DCPTimerRate,DCP_TIMERINTERRUPTUS_MIN,DCP_TIMERINTERRUPTUS_MAX,
    (SINT32)AdvanceConfigData->Advrnd.DCPStateMachineTime,DCP_STATEMACHINETIMEUS_MIN,DCP_STATEMACHINETIMEUS_MAX,
    (SINT32)AdvanceConfigData->Advrnd.DCPControlLoopTime,DCP_CONTROLLOOPTIMEUS_MIN,DCP_CONTROLLOOPTIMEUS_MAX,
    (SINT32)AdvanceConfigData->Advrnd.DCPModBusTime,DCP_MODBUSCOMMTIMEUS_MIN,DCP_MODBUSCOMMTIMEUS_MAX,
    (SINT32)AdvanceConfigData->Advrnd.DCPADCResolution,DCP_ADCRESOLUTION_11BIT,DCP_ADCRESOLUTION_16BIT,
    (SINT32)AdvanceConfigData->Advrnd.DCPBlindTime,DCP_BLINDTIMEUS_MIN,DCP_BLINDTIMEUS_MAX,
    (SINT32)AdvanceConfigData->Advrnd.Gp2Value1,WD_MIN,WD_MAX,
    (SINT32)AdvanceConfigData->Advrnd.Gp2Value2,UINT_MIN,UINT_MAX,
    (SINT32)AdvanceConfigData->Advrnd.Gp2Value3,UINT_MIN,UINT_MAX,
    (SINT32)AdvanceConfigData->Advrnd.Gp2Value4,UINT_MIN,UINT_MAX,
    (SINT32)AdvanceConfigData->Advrnd.Gp2Value5,UINT_MIN,UINT_MAX,
    (SINT32)AdvanceConfigData->Advrnd.Gp2Value6,UINT_MIN,UINT_MAX,
    (SINT32)AdvanceConfigData->Advrnd.RevDigitalOP,
    (SINT32)AdvanceConfigData->Advrnd.Flag2,
    (SINT32)AdvanceConfigData->Advrnd.Flag3,
    (SINT32)AdvanceConfigData->Advrnd.Flag4,
    (SINT32)AdvanceConfigData->Advrnd.Flag5,
    (SINT32)AdvanceConfigData->Advrnd.Flag6);
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::BransonAdvRnDTestPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function is used to handle the Advanced RnD Parameters values posted from Webpage.This functions is called by Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::BransonAdvRnDTestPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   if(TokenAvailable){
      SINT8 * TempStr = strstr(ParamStr, "Gp1Value1=");
      AdvanceConfigData->Advrnd.DCPTimerRate = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Gp1Value2=");
      AdvanceConfigData->Advrnd.DCPStateMachineTime = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Gp1Value3=");
      AdvanceConfigData->Advrnd.DCPControlLoopTime = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Gp1Value4=");
      AdvanceConfigData->Advrnd.DCPModBusTime = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Gp1Value5=");
      AdvanceConfigData->Advrnd.DCPADCResolution = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Gp1Value6=");
      AdvanceConfigData->Advrnd.DCPBlindTime = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Gp2Value1=");
      AdvanceConfigData->Advrnd.Gp2Value1 = atoi(strchr(TempStr, '=') + 1);
      if((AdvanceConfigData->Advrnd.Gp2Value1 > WD_MAX)||
    		  (AdvanceConfigData->Advrnd.Gp2Value1 < WD_MIN))
    	  AdvanceConfigData->Advrnd.Gp2Value1 = WD_DEFAULT;
      TempStr = strstr(ParamStr, "&Gp2Value2=");
      AdvanceConfigData->Advrnd.Gp2Value2 = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Gp2Value3=");
      AdvanceConfigData->Advrnd.Gp2Value3 = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Gp2Value4=");
      AdvanceConfigData->Advrnd.Gp2Value4 = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Gp2Value5=");
      AdvanceConfigData->Advrnd.Gp2Value5 = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Gp2Value6=");
      AdvanceConfigData->Advrnd.Gp2Value6 = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&RevDigitalOP=");
      AdvanceConfigData->Advrnd.RevDigitalOP =(BOOLEAN)atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Flag2=");
      AdvanceConfigData->Advrnd.Flag2 = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Flag3=");
      AdvanceConfigData->Advrnd.Flag3 = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Flag4=");
      AdvanceConfigData->Advrnd.Flag4 = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Flag5=");
      AdvanceConfigData->Advrnd.Flag5 = atoi(strchr(TempStr, '=') + 1);
      TempStr = strstr(ParamStr, "&Flag6=");
      AdvanceConfigData->Advrnd.Flag6 = atoi(strchr(TempStr, '=') + 1);
      if(Sysconfig->DcpSpeedControl == SPEEDCONTROL_CUSTOM)//Send update to DCP only when custom speed is selected
    	  SetDCPTimingParam(REQ_NONE);
      else
    	  strcpy(Response, "OK");
   }
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::BransonAdvRnDTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to Field service page in a format which javascript code on this page
 *   is expecting.This function gets called during Field service page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::BransonFieldServiceGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   UINT8 Indx;
   EventID ServiceEvents[] =
   {EVENT_CALIBRATION_PASS, EVENT_CALIBRATION_FAIL, EVENT_BOARD_REPLACED, EVENT_SYSTEM_CLEANED,
	EVENT_FIELD_SERVICE_ONE, EVENT_FIELD_SERVICE_TWO, EVENT_FIELD_SERVICE_THREE, EVENT_FIELD_SERVICE_FOUR};
   if((TokenAvailable == TRUE) && (Cmd == REQ_COLDSTART)){
      DcxLogUserID = DcxWebUserID;
      LogData2 = LOG_CLEARED_BY_OTHER;
      //The only cold start which can change the Speed Control setting is the one from Field Service page.
      Sysconfig->DcpSpeedControl = SPEEDCONTROL_STANDARD;
      //Not sending the values to DCP live here. Expected power cycle after this cold start
      ColdStart();
      Log::WriteEventLog(EVENT_COLD_START, BRANSON_FIELD_SERVICE);
      DcxLogUserID = 0;
      strcpy(Response, "Cold Start Done. \n Restart System ...");
   }
   else if((TokenAvailable == TRUE) && (Cmd == REQ_SYSCALIBRATION)){
      ParameterSet[CurrPreset].WcParam.WeldMode = TimeMode;
      ParameterSet[CurrPreset].WcParam.WeldTimeCP = 500;
      ParameterSet[CurrPreset].WcParam.HoldTime = 500;
      ParameterSet[CurrPreset].WcParam.Amplitude1 = 50;
      UserIO::AssignAnalogOutputFunction(AOUT1, POWER_OUT, TRUE);
      UserIO::AssignAnalogOutputFunction(AOUT2, FREQ_OUT, TRUE);
      CalculateAndSavePresetCRC(SAVECRC_WC);
      RegUpdateCmdFlag = TRUE;
      strcpy(Response, "System set for Calibration.");
   }
   else if((TokenAvailable == TRUE) && (Cmd == REQ_SAVE_SYSSETUP)){
	   TempUSERIOConfig = *USERIOConfig;
      * TempParameterSet = ParameterSet[CurrPreset];
      strcpy(Response, "System Settings Saved.");
   }
   else if((TokenAvailable == TRUE) && (Cmd == REQ_RESTORE)){
      CableDetectedPin = 0;
      CustomInput1Configured = CustomInput2Configured = FALSE;
      InterlockConfigured = PartInPlaceConfigured = FALSE;
      ParameterSet[CurrPreset] = *TempParameterSet;
      for(Indx = 1; Indx < NUM_DIN; Indx++){
         UserIO::AssignDigitalInputFunction((DIGITAL_INPUTS)Indx , TempUSERIOConfig.DigInUse[Indx], TempUSERIOConfig.DigInUseStatus[Indx]);
         if(TempUSERIOConfig.DigInUse[Indx] == CABLEDETECT_ACTIVEHIGH)
            UserIO::SetCableDetectEnabledFlag(TempUSERIOConfig.DigInUseStatus[Indx]);
      }
      for(Indx = 1; Indx < NUM_DOUT; Indx++)
         UserIO::AssignDigitalOutputFunction((DIGITAL_OUTPUTS)Indx, TempUSERIOConfig.DigOutUse[Indx], TempUSERIOConfig.DigOutUseStatus[Indx]);
      for(Indx = 1; Indx < NUM_AIN; Indx++)
         UserIO::AssignAnalogInputFunction((ANALOG_INPUTS)Indx, TempUSERIOConfig.AnalogInUse[Indx], TempUSERIOConfig.AnalogInUseStatus[Indx]);
      for(Indx = 1; Indx < NUM_AOUT; Indx++)
         UserIO::AssignAnalogOutputFunction((ANALOG_OUTPUTS)Indx, TempUSERIOConfig.AnalogOutUse[Indx], TempUSERIOConfig.AnalogOutUseStatus[Indx]);
      //this is to make sure preset gets updated in running preset and to DCP on restore
      CalculateAndSavePresetCRC(SAVECRC_WC);
      RegUpdateCmdFlag = TRUE;
      strcpy(Response, "Parameters Restored");
   }
   else if(Cmd == REQ_NONE){
      sprintf(&Response[strlen(Response)],
         "%ld@SelectEvent:", (SINT32)SERVICE);
	   for(Indx = 0;  Indx < (sizeof(ServiceEvents)/ sizeof(ServiceEvents[0])); Indx++)
            sprintf(&Response[strlen(Response)], "%s,%d:",Log::GetEventStringFromID(ServiceEvents[Indx]), ServiceEvents[Indx]);
   }
   else
      strcpy(Response, "TOKENNOTAVAILABLE");
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::BransonFieldServicePostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function is used to handle the Field service Parameters values posted from Webpage.This functions is called by Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::BransonFieldServicePostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
	EventID SelectedEventID;
	SINT32 Data1, Data2;
	DcxLogUserID = DcxWebUserID;
	SINT8 * TempStr = strstr(ParamStr, "&EventID=");
	SelectedEventID = (EventID) atoi(strchr(TempStr, '=') + 1);
	TempStr = strstr(ParamStr, "&Data1=");
	TempStr = strchr(TempStr, '=') + 1;
	if(TempStr[0] == '-')
		Data1 = -(atoi(TempStr + 1));
	else
		Data1 = atoi(TempStr);
	TempStr = strstr(ParamStr, "&Data2=");
	TempStr = strchr(TempStr, '=') + 1;
	if(TempStr[0] == '-')
		Data2 = -(atoi(TempStr + 1));
	else
		Data2 = atoi(TempStr);
	Log::WriteEventLog(SelectedEventID, Data1, Data2);
	strcpy(Response, "Event Added.");
	DcxLogUserID = 0;
	Cmd = REQ_NONE;
	ParamStr = 0;
}


/* void WebSite::BransonSpecialPresetGetDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to SpecialPreset page in a format which javascript code on this page
 *   is expecting.This function gets called during SpecialPreset page is loaded from website.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::BransonSpecialPresetGetDatahandler (SINT8 * ParamStr, WebCommand Cmd)
{
   SINT8 * TempStr,TempTime[TIMESTRINGLEN];
   UINT8 Indx, Temp1 = 0, Temp2 = 0;
   PSUPDATESPECIAL PsUpdateSpcl = UPDATENOSPECIALPS;
   if((Cmd == REQ_RESTORE) && (TokenAvailable == TRUE)){
      TempStr = strstr(ParamStr, "Index=");
      Indx = (PRESET_NUMBER)(atoi(strchr(TempStr, '=') + 1) + MAXIMUM_USER_PRESETS);
      DcxLogUserID = DcxWebUserID;
     // pprintf("\n Preset ID = %d",Indx);
      switch(Indx){
     	case Preset_MIN:
     		SetMaxMinValues(UPDATEMINPS);
     		PsUpdateSpcl = UPDATEMIN;
     		break;
      	case Preset_MAX:
      		SetMaxMinValues(UPDATEMAXPS);
      		PsUpdateSpcl = UPDATEMAX;
      		break;
      	case Preset_TweakMIN:
      		SetMaxMinValues(UPDATETWEAKMINPS);
      		PsUpdateSpcl = UPDATEMIN;
      		break;
      	case Preset_TweakMAX:
      		SetMaxMinValues(UPDATETWEAKMAXPS);
      		PsUpdateSpcl = UPDATEMAX;
      		break;
      	case Preset_Default:
      		SetColdStartPresetValues(UPDATEDEFAULTPS);
      		break;
      }
      Log::WriteEventLog(EVENT_RESTORE_DEFAULTS, BRANSON_SPECIAL_PRESET, Indx);
      if(PsUpdateSpcl != UPDATENOSPECIALPS)
	  {
		UpdateMinMaxLCDPreset(PsUpdateSpcl);
		SetMinMaxParamForFbTable(PsUpdateSpcl);
	  }
      DcxLogUserID = 0;
   }
   sprintf(&Response[strlen(Response)], "%ld@", (SINT32)SPECIALS);
   for(Indx = Preset_MIN; Indx < MAX_TOTAL_PRESETS; Indx++){
	   Temp1 = ParameterSet[Indx].InfoParam.ModifiedMonth;
	   Temp2 = ParameterSet[Indx].InfoParam.ModifiedDate;
	   PrepareTimeStamp(TempTime, Temp1, Temp2, ParameterSet[Indx].InfoParam.ModifiedMinute, ParameterSet[Indx].InfoParam.ModifiedHour);
       sprintf(&Response[strlen(Response)],
         "%s,"
         "%ld/%ld/%ld,"
         "%s,"
         "%ld@",
         (SINT8*)ParameterSet[Indx].InfoParam.PresetID,
         (SINT32)Temp1, (SINT32)Temp2, (SINT32)ParameterSet[Indx].InfoParam.ModifiedYear,
         TempTime,
         (SINT32)ParameterSet[Indx].InfoParam.HornPresetNumber);
   }
   sprintf(&Response[strlen(Response)], "TweakEnabled:CB,%d@", Sysconfig->TweakRangeEnabled);
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::BransonSpecialPresetPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
 *
 *  Purpose:
 *   This function is used to handle the SpecialPreset Parameters values posted from Webpage.This functions is called by Website post handler.
 *
 *  Entry condition: ParamStr- Pointer to parameters array.
 *                   Cmd- Command sent by jscript.
 *
 *  Exit condition: None.
 */
void WebSite::BransonSpecialPresetPostDatahandler(SINT8 * ParamStr, WebCommand Cmd)
{
   static BOOLEAN OldTweakEnabled = Sysconfig->TweakRangeEnabled;
   PSUPDATESPECIAL PsUpdateSpcl = UPDATENOSPECIALPS;

   if(TokenAvailable){
      HSPRESET_NUMBER Hornpresetno;
      PRESET_NUMBER Index;
      SINT8 * TempStr;
      if(Cmd == REQ_SAVE_PRESET){
         TempStr = strstr(ParamStr, "Index=");
         Index = (PRESET_NUMBER)(atoi(strchr(TempStr, '=') + 1) + MAXIMUM_USER_PRESETS);
         TempStr = strstr(ParamStr, "hornpresetno=");
         Hornpresetno = (HSPRESET_NUMBER)(atoi(strchr(TempStr, '=') + 1));
         if(SavePreset(Index, Hornpresetno)){
        	 if((Index == Preset_MIN) || (Index == Preset_TweakMIN))
        		 PsUpdateSpcl = UPDATEMIN;
        	 else if((Index == Preset_MAX) || (Index == Preset_TweakMAX))
        		 PsUpdateSpcl = UPDATEMAX;
            strcpy(Response, "Preset Saved");
         }
         else
            strcpy(Response, "Preset not Saved");
      }
      else if(Cmd == REQ_SAVE){
         TempStr = strstr(ParamStr, "TweakEnabled=");
         if(atoi(strchr(TempStr, '=') + 1) == 1)
            Sysconfig->TweakRangeEnabled = TRUE;
         else
            Sysconfig->TweakRangeEnabled = FALSE;
         strcpy(Response, "Saved");
      }
      else if(Cmd == REQ_RECALL_PRESET){
         SINT8 * TempStr = strstr(ParamStr, "Index=");
         Index = (PRESET_NUMBER)(atoi(strchr(TempStr, '=') + 1) + MAXIMUM_USER_PRESETS);

         if(RecallPreset(Index))
            strcpy(Response,"Preset Recall success");
         else
            strcpy(Response,"Preset Recall failed");
      }
	  if(OldTweakEnabled != Sysconfig->TweakRangeEnabled){
		OldTweakEnabled = Sysconfig->TweakRangeEnabled;
		PsUpdateSpcl = UPDATEMINMAX;
	  }
	  if(PsUpdateSpcl != UPDATENOSPECIALPS)
	  {
		UpdateMinMaxLCDPreset(PsUpdateSpcl);
		SetMinMaxParamForFbTable(PsUpdateSpcl);
	  }
   }
   else
      strcpy(Response, "TOKENNOTAVAILABLE");
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* void WebSite::SetLanguageAndUserID(SINT8 * ParamStr, WebCommand Cmd)
 *
 * Purpose:
 *   This function fills data specific to Index page in a format which javascript code on this page is expecting.
 *
 * Entry condition: ParamStr- Pointer to parameters array.
 *                  Cmd- Command sent by jscript in URI.
 *
 * Exit condition: None.
 */
void WebSite::SetLanguageAndUserID(SINT8 * ParamStr, WebCommand Cmd)
{
   SINT8 * TmpStr, * TmpStr2;
   SINT8 Userid[MAX_LEN_USERID + 1];
   UINT32 Passcode, Privilege;
   if(IsPasscodeRequired()){
      TmpStr = strstr(ParamStr, "&");
      TmpStr = strchr(TmpStr, '=') + 1;
      TmpStr2 = strstr(TmpStr, "&");
      strncpy(Userid, TmpStr, CopyLength(MAX_LEN_USERID, TmpStr2 - TmpStr, __LINE__));
      Passcode = atoi(strchr(TmpStr, '=') + 1);
      Privilege = GetUserPrivilege( (UINT8 *)Userid, Passcode);
      if((Privilege == PRIVILEGE_ADMIN)||(Privilege == PRIVILEGE_USER)||(Privilege == PRIVILEGE_BRANSON))
    	  sprintf(Response,"OK@%ld",PowerUpId); 		// Send Power up unique Key
      else
         strcpy(Response, "NOTOK");
   }
   else
	   sprintf(Response,"OK@%ld",PowerUpId);		// Send Power up unique Key
   Cmd = REQ_NONE;
   ParamStr = 0;
}

/* SINT8 * WebSite::GetFunctionString(SINT32 SignalType, SINT32 FuncID, SINT32 ActiveStatus, SINT8 * FileBuff)
 *
 * Purpose:
 *   This function finds the function string from language file based on FuncID.This function gets called from PrepareDiagnosticResponse function.
 *
 * Entry condition: FuncID- Function ID to determine language string ID for that function.
 *                  SignalType- the type of of function i.e. dig in/out or nalog in/out.
 *                  FileBuff- pointer to the content of language file.
 *
 * Exit condition: Function String.
 */
SINT8 * WebSite::GetFunctionString(SINT32 SignalType, SINT32 FuncID, SINT32 ActiveStatus, SINT8 * FileBuff)
{
   SINT32 FunctionsCnt, Indx, LangStrID = 0;
   BOOLEAN ExitFlag;
   SINT8 * FuncStr = Unassigned;
   if((FuncID > 0) && (ActiveStatus == ACTIVATED)){
      switch(SignalType){
         case SIG_DIGIN:      //digital input
            FunctionsCnt = sizeof(webDigIOfuncTableIn)/sizeof(WebDigitalIOFunctionTable);
            ExitFlag = FALSE;
            for(Indx = 1; ((Indx < FunctionsCnt) && (ExitFlag == FALSE)); Indx++){
               if((webDigIOfuncTableIn[Indx].ActivehighID == FuncID)
                     ||(webDigIOfuncTableIn[Indx].ActivelowID == FuncID)){
                  LangStrID = webDigIOfuncTableIn[Indx].StringID;
                  ExitFlag =  TRUE;
               }
            }
         break;
         case SIG_DIGOUT:     //digital output
            FunctionsCnt = sizeof(webDigIOfuncTableOut)/sizeof(WebDigitalIOFunctionTable);
            ExitFlag = FALSE;
            for(Indx = 1; ((Indx < FunctionsCnt) && (ExitFlag == FALSE)); Indx++){
               if((webDigIOfuncTableOut[Indx].ActivehighID == FuncID)
                     ||(webDigIOfuncTableOut[Indx].ActivelowID == FuncID)){
                  LangStrID = webDigIOfuncTableOut[Indx].StringID;
                  ExitFlag = TRUE;
               }
            }
         break;
         case SIG_ANALOGIN:      //analog input
            FunctionsCnt = sizeof(webAnaIOfuncTableIn)/sizeof(WebAnalogIOFunctionTable);
            ExitFlag = FALSE;
            for(Indx = 1; ((Indx < FunctionsCnt) && (ExitFlag == FALSE)); Indx++){
               if((webAnaIOfuncTableIn[Indx].FunctionID == FuncID)){
                  LangStrID = webAnaIOfuncTableIn[Indx].StringID;
                  ExitFlag = TRUE;
               }
            }
         break;
         case SIG_ANALOGOUT:     //analog output
            FunctionsCnt = sizeof(webAnaIOfuncTableOut)/sizeof(WebAnalogIOFunctionTable);
            ExitFlag = FALSE;
            for(Indx = 1; ((Indx < FunctionsCnt) && (ExitFlag == FALSE)); Indx++){
               if((webAnaIOfuncTableOut[Indx].FunctionID == FuncID)){
                  LangStrID = webAnaIOfuncTableOut[Indx].StringID;
                  ExitFlag = TRUE ;
               }
            }
         break;
      }
      FuncStr = FindLanguageString(LangStrID, FileBuff, TRUE);
   }
   return FuncStr;
}

/* SINT8 * WebSite::FindLanguageString(UINT32 StringId, SINT8 * FileBuff, BOOLEAN StrOnly)
 *
 * Purpose:
 *   This function finds the string in the file pointed by FileBuff.This function gets called from ConfigurationUserIOGetDatahandler and GetFunctionString functions.
 *
 * Entry condition: StringID- Type of request alarm or event.
 *                  StrOnly- flag to determine whether string is needed in format.
 *                  FileBuff- pointer to the content of language file.
 *
 * Exit condition:
 *
 * Global LangStringFunction String is filled with Function String corresponding to ID..
 */
SINT8 * WebSite::FindLanguageString(UINT32 StringId, SINT8 * FileBuff, BOOLEAN StrOnly)
{
   SINT8 * TempStr = FileBuff;
   BOOLEAN FirstStr = TRUE;
   BOOLEAN ExitFlag = FALSE;
   UINT32 StrID;
   SINT8 * Temp2;
   do{
      if(!FirstStr)
         TempStr = TempStr + 2;
      StrID = atoi(TempStr);
      if(StrID == StringId){
         Temp2 = strstr(TempStr  , "\r\n");
         if((Temp2 - TempStr) < MAX_LANGUAGESTRING_LEN)
            strncpy(LangString, TempStr, Temp2 - TempStr);
         ExitFlag = TRUE;
      }
      if(ExitFlag == FALSE){
    	  FirstStr = FALSE;
    	  TempStr = strstr(TempStr , "\r\n");
      }
   }while(TempStr && (ExitFlag == FALSE));

   if(!StrOnly){
      if(strlen(LangString) < MAX_LANGUAGESTRING_LEN - 1)
         strcat(LangString, "@");
   }
   else{
      if(strlen(LangString) < MAX_LANGUAGESTRING_LEN - 1 )
         strcpy(LangString, strchr(LangString, ',') + 1);
   }
   return LangString;
}

/* void WebSite::RestoreOutPutStatus(void)
 *
 * Purpose:
 *   This function restores the original output values(i.e. before diagnostic)of Digital and analog outputs and starts
 *   the state machine to receive commands from other interfaces.This function gets called from StopIODiagnostics and website tick.
 *
 * Entry condition: None.
 *
 * Exit condition: None.
 */
void WebSite::RestoreOutPutStatus(void)
{
   UINT8 Indx;
   for(Indx = 1; Indx < NUM_DOUT ;Indx++){
	   if(TempDigOutPutStatus[Indx])
		   UserIO::SetDigOutPutValue((DIGITAL_OUTPUTS)Indx, TRUE);
	   else
		   UserIO::SetDigOutPutValue((DIGITAL_OUTPUTS)Indx, FALSE);
   }
   for(Indx = 1; Indx < NUM_AOUT; Indx++)
      UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)Indx, TempAnalogOutPutVals[Indx]);
}

/* BOOLEAN  WebSite::CheckForWebTestCommand(void)
 *
 * Purpose:
 *   This function gets called by state machine to determine the test command status from website
 *
 * Entry condition: None.
 *
 * Exit condition: true if TEST is started from website else false.
 */
BOOLEAN  WebSite::CheckForWebTestCommand(void)
{
   return (TestStartFlag || mfTestFlag);
}

/* void WebSite::ResetTestFlag(void)
 *
 * Purpose:
 *   This function gets called by state machine to reset the test command status from website.
 *
 * Entry condition: None.
 *
 * Exit condition: None.
 */
void WebSite::ResetTestFlag(void)
{
   DcxLogUserID = 0;
   TestStartFlag = FALSE;
   ControlVariable &= ~(WEBWELD_START | WEBTEST);
   mfTestFlag = FALSE;
}

/* BOOLEAN WebSite::CheckForWebScanCommand(void)
 *
 * Purpose:
 *   This function gets called by state machine to determine the SCAN command status from website.
 *
 * Entry condition: None.
 *
 * Exit condition: true if SCAN is started from website else false.
 */
BOOLEAN WebSite::CheckForWebScanCommand(void)
{
   return ScanStartFlag;
}

/* BOOLEAN WebSite::CheckForWebScanAbortCommand(void)
 *
 * Purpose:
 *   This function gets called by state machine to determine the SCAN abort status from website
 *
 * Entry condition: None.
 *
 * Exit condition: TRUE if SCAN is aborted from website else FALSE.
 */
BOOLEAN WebSite::CheckForWebScanAbortCommand(void)
{
   return ScanAbortFlag;
}

/* void WebSite::ResetScanFlags(void)
 *
 * Purpose:
 *   This function gets called by state machine to reset scan flags after SCAN is completed or aborted.
 *
 * Entry condition: None.
 *
 * Exit condition: None.
 */
void WebSite::ResetScanFlags(BOOLEAN ResetID)
{
	ScanStartFlag = FALSE;
	ScanAbortFlag = FALSE;
	ScanFailedFlag = FALSE;
	if(ResetID)
		DcxLogUserID = 0;
}

/* BOOLEAN WebSite::CheckForDiagnosticFlag(void)
 *
 * Purpose:
 *   This function gets called by state machine to determine that whether the diagnostic is in progress.
 *
 * Entry condition: None.
 *
 * Exit condition: Returns TRUE, if diagnostic is under progress.
 */
BOOLEAN WebSite::CheckForDiagnosticFlag(void)
{
	return diagFlag;
}

/* BOOLEAN WebSite::CheckForWebReset(void)
 *
 * Purpose:
 *   This function gets called by state machine to check the reset command from website.
 *
 * Entry condition: None.
 *
 * Exit condition: Returns TRUE if reset is sent by website.
 */
BOOLEAN WebSite::CheckForWebReset(void)
{
   return resetFlag;
}

/* void WebSite::ResetWebResetFlag(void)
 *
 * Purpose:
 *   This function gets called by state machine to Clear the reset flag after Reset is done.
 *
 * Entry condition: None.
 *
 * Exit condition: None.
 */
void WebSite::ResetWebResetFlag(void)
{
	resetFlag = FALSE;
	ControlVariable &= ~WEBRESET;
}

/* BOOLEAN WebSite::CheckForMFBoardTestFlag(void)
 *
 * Purpose:
 *   This function gets called by state machine to check whether DCP board is in test mode or not.
 *
 * Entry condition: None.
 *
 * Exit condition: Return mfBoardTestFlag.
 */
BOOLEAN WebSite::CheckForMFBoardTestFlag(void)
{
	return mfBoardTestFlag;
}

/* BOOLEAN WebSite::CheckForManufacturingFlag(void)
 *
 * Purpose:
 *   This function gets called by ReceiveCommandHandler for making decision about Display.
 *
 * Entry condition: None.
 *
 * Exit condition: Return mfFlag.
 */
BOOLEAN WebSite::CheckForManufacturingFlag(void)
{
	return (mfFlag | CurrentCycleParam->MfCycling);
}

/* void WebSite::Tick(SINT32 callbackus)
 *
 * Purpose:
 *   This function gets called every RTOS tick.It determines whether SCAN,TEST or diagnostic commands from website has been timed out
 *   in some accidental case(e.g ethernet cable removed,website window closed etc).It disables the corresponding flags in that case.
 *
 * Entry condition: callbackus- RTOS tick in microseconds.
 *
 * Exit condition: None.
 */
void WebSite::Tick(int callbackus)
{
	if(ScanStartFlag){
		webtimeOutScan -= callbackus;
		if(webtimeOutScan <= 0){
			ScanAbortFlag = TRUE;
			ClearScanFlags();
		}
	}
	if(TestStartFlag || mfTestFlag){
		webtimeOutTest -= callbackus;
		if(webtimeOutTest <= 0){
		   ResetTestFlag();
		   WebIOEnabled = FALSE;
		}
	}

	if(diagFlag){
		diagnosticTimeOut -= callbackus;
		if(diagnosticTimeOut <= 0){
			diagFlag = FALSE;
			WebIOEnabled = FALSE;
			if(FanStatusFlag != DefaultFanStatus)
			   UserIO::FanEnable(DefaultFanStatus);
			DcxLogUserID = 0;
		}
	}
	if(diagStartFlag){
		StartDiagTimeOut -= callbackus;
		if(StartDiagTimeOut <= 0){
			diagStartFlag = FALSE;
			RestoreOutPutStatus();
			WebIOEnabled = FALSE;
		}
	}
	if(mfBoardTestFlag){
		mfBoardTestTimeOut -= callbackus;
		if(mfBoardTestTimeOut <= 0){
			WebIOEnabled = FALSE;
			mfBoardTestFlag = FALSE;
			BoardTestFrequency = MIN_BOARD_TESTFREQUENCY;
			BoardTestOutput = 0;
		}
	}
	if(mfFlag){
		mfTimeOut -= callbackus;
		if(mfTimeOut <= 0){
			mfFlag = FALSE;
			WebIOEnabled = CurrentCycleParam->MfCycling;
			if(CurrentCycleParam->MfCycling == FALSE)
				ControlVariable &= ~MANUFACT_WELD;
		}
	}
}

/* void WebSite::PrepareDiagnosticResponse(BOOLEAN InputsOnly)
 *
 * Purpose: This function fills Diagnostic UserIO data into Response Buffer.This function gets called from DiagnosticUserIOGetDatahandler() function.
 *
 * Entry condition: InputsOnly- if FALSE, Outputs are also filled in Buffer else only input are filled in Response Buffer.
 *
 * Exit condition: None.
 */
void WebSite::PrepareDiagnosticResponse(BOOLEAN InputsOnly)
{
   UINT8 Indx;
   SINT32 AIn;
   SINT8 * PINS[] = {0,"PIN1","PIN2","PIN3","PIN4","PIN11","PIN12","PIN13","PIN16","PIN23", "PIN7", "PIN8","PIN9", "PIN10",
   "PIN19","PIN20" ,"PIN21", "PIN22" , "PIN17", "PIN18", "PIN24", "PIN25"};
   sprintf(&Response[strlen(Response)], "FanControl,%d@", FanStatusFlag);
   for(Indx = 1; Indx < NUM_DIN; Indx++){
      sprintf(&Response[strlen(Response)],"%s:S,DI,%s,%ld,%ld@",PINS[Indx],
      GetFunctionString(0, USERIOConfig->DigInUse[Indx],USERIOConfig->DigInUseStatus[Indx],languageBuff),
      UserIO::GetDigINPutValue((DIGITAL_INPUTS)Indx),
      UserIO::GetDigitalinputActivityStatus(USERIOConfig->DigInUse[Indx]));
   }
   if(!InputsOnly){
      for(Indx = 1; Indx < NUM_DOUT; Indx++){
         sprintf(&Response[strlen(Response)], "%s:S,DO,%s,%ld,%ld@", PINS[Indx + 9],
         GetFunctionString(1, USERIOConfig->DigOutUse[Indx], USERIOConfig->DigOutUseStatus[Indx],languageBuff),
         UserIO::GetDigOutPutValue((DIGITAL_OUTPUTS)Indx),
         UserIO::GetDigitaloutputActivityStatus(USERIOConfig->DigOutUse[Indx]));
      }
   }
   for(Indx = 1; Indx < NUM_AIN; Indx++){
      AIn = UserIO::GetAnalogINPutValue((ANALOG_INPUTS)Indx);
      if(AIn < 0)
         AIn = 0;
      sprintf(&Response[strlen(Response)],"%s:S,AI,%s,%ld,%ld@",PINS[Indx + 17],
            GetFunctionString(2,USERIOConfig->AnalogInUse[Indx],
            USERIOConfig->AnalogInUseStatus[Indx],languageBuff),
            ((SINT32)(AIn * MI_ANALOGIP_MAXMV)/MAX_ADC_RAWVALUE),
            (SINT32)UserIO::GetAnalogINPutValue((ANALOG_INPUTS)Indx));
   }
   if(!InputsOnly){
      for(Indx = 1; Indx < NUM_AOUT; Indx++){
        sprintf(&Response[strlen(Response)],"%s:S,AO,%s,%ld,%ld@",PINS[Indx + 19],
               GetFunctionString(3, USERIOConfig->AnalogOutUse[Indx],
               USERIOConfig->AnalogOutUseStatus[Indx], languageBuff),
               ((SINT32)((UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS)Indx))* MI_ANALOGIP_MAXMV)/MAX_DAC_RAWVALUE),
               (SINT32)UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS)Indx));
      }
   }
}

/* void WebSite::UpdateDiagnosticIO(SINT8 * ParamStr)
 *
 * Purpose: This function Store analog and digital outputs received from Diagnostic UserIO Webpage.
 *   This function gets called from DiagnosticUserIOPostDatahandler() function.
 *
 * Entry condition: ParamStr- Pointer to buffer containing Data.
 *
 * Exit condition: None.
 */
void WebSite::UpdateDiagnosticIO(SINT8 * ParamStr)
{
   SINT8 * TempStr = ParamStr;
   UINT8 Indx;
   for(Indx = 1; Indx < NUM_DOUT ;Indx++)
      TempDigOutPutStatus[Indx] = UserIO::GetDigOutPutValue((DIGITAL_OUTPUTS)Indx);
   for(Indx = 1; Indx < NUM_AOUT; Indx++)
      TempAnalogOutPutVals[Indx] = UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS)Indx);
   diagStartFlag = TRUE;
   StartDiagTimeOut = WEBDIAG_TIMEOUT;
   for(Indx = 1;Indx < NUM_DOUT; Indx++){
      UserIO:: SetDigOutPutValue((DIGITAL_OUTPUTS)Indx, BOOLEAN(atoi(TempStr)));
      TempStr = strchr(TempStr, ',') + 1;
   }
   for(Indx = 1; Indx < NUM_AOUT; Indx ++){
      UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)Indx, ((atoi(TempStr) * MAX_DAC_RAWVALUE) / MI_ANALOGIP_MAXMV));
      TempStr = strchr(TempStr, ',') + 1;
   }
   strcpy(Response, "OK");
}

/* void WebSite::DoDiagnosticLoopBackStart(void)
 *
 * Purpose:
 *   This function Store analog and digital outputs in Temporary buffers.This function gets called from DiagnosticUserIOPostDatahandler() function.
 *
 * Entry condition: None.
 *
 * Exit condition: None.
 */
void WebSite::DoDiagnosticLoopBackStart(void)
{
   UINT8 Indx;
   diagStartFlag = TRUE;
   StartDiagTimeOut = LOOPBACK_TIMEOUT;
   for(Indx = 1; Indx < NUM_AOUT; Indx++){
     TempAnalogOutPutVals[Indx] = UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS)Indx);
     UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)Indx, 0);
   }
   for(Indx = 1; Indx < NUM_DOUT; Indx++)
     TempDigOutPutStatus[Indx] = UserIO::GetDigOutPutValue((DIGITAL_OUTPUTS)Indx);
   strcpy(Response, "OK");
}

/* void WebSite::DoDiagnosticLoopBack(void)
 *
 * Purpose: This function performs a loop back.This function gets called from DiagnosticUserIOPostDatahandler() function.
 *
 * Entry condition: None.
 *
 * Exit condition: None.
 */
void WebSite::DoDiagnosticLoopBack(void)
{
	BOOLEAN CurrState;
	SINT32 CurrDacOutPut;
	UINT8 Indx;
	for(Indx = 1; Indx < NUM_DOUT; Indx++){
	   CurrState = UserIO::GetDigOutPutValue((DIGITAL_OUTPUTS)Indx);
	   if(CurrState)
		   UserIO::SetDigOutPutValue((DIGITAL_OUTPUTS)Indx, FALSE);
	   else
		   UserIO::SetDigOutPutValue((DIGITAL_OUTPUTS)Indx, TRUE);
	}
	for(Indx = 1; Indx < NUM_AOUT; Indx++){
	   CurrDacOutPut = UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS)Indx);
	   if(CurrDacOutPut)
		   UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)Indx, 0);
	   else
		   UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)Indx, MAX_DAC_RAWVALUE);
	}
}

/* void WebSite::StopIODiagnostic(void)
 *
 * Purpose: This function is used to stop IO Diagnostic. This function gets called from DiagnosticUserIOPostDatahandler() function.
 *
 * Entry condition: None.
 *
 * Exit condition: None.
 */
void WebSite::StopIODiagnostic(void)
{
   diagStartFlag = FALSE;
   RestoreOutPutStatus();
   strcpy(Response, "OK@");
   PrepareDiagnosticResponse();
}

/* CurrentState CheckForCurrentFunction(void)
 *
 * Purpose: This function returns Current Index/state of State machine and it is called by website.
 *
 * Entry condition:
 * CurrIndx:Representing Current state of state machine.
 *
 * Exit condition: Current State of machine.
 */
CurrentState CheckForCurrentFunction(SINT16 CurrIndx)
{
	CurrentState State = stateError;
	switch(CurrIndx){
      case READYSTATE_IDX:
      case PREREADYSTATE_IDX:
    	  State = stateWait;
    	  break;
      case SEEKSTATE_IDX:
    	  State = stateSeek;
    	  break;
      case TESTSTATE_IDX:
      case SONICS_START_IDX:
      case ENERGY_BRAKING_IDX:
      case HOLD_OFF_TIME_IDX:
      case CHECK_LIMIT_IDX:
      case AFTER_BURST_IDX:
      case CYCLE_ABORT_IDX:
      case WAITULSTRS_IDX:
		   State = stateRun;
		   break;
      case ERROR_IDX:
    	  State = stateError;
    	  break;
      case SCANSTATE_IDX:
    	  State = stateScan;
    	  break;
      default:
    	  break;
	};
	if(AlarmIndex)
		State = stateError;
	return State;
}

/*void PrepareTimeStamp(SINT8 * TSStr, UINT8 & Date1, UINT8 &Date2, UINT8 Min, UINT8 Hour)
 * Purpose: To format the time stamp based on time and date settings in webpage
 * TsStr: To be filled with formatted time
 * Date1: Month part of date
 * Date2: Day part of date;
 * Min: Minutes for for time stamp
 * Hour: Hour for time stamp.
 * Secs: Default val -1: Not needed in timestamp else represent seconds value
 * Note:Always pass temp vars from calling function in Date1 and Date2 part as they gets modified in this function.
 */
void PrepareTimeStamp(SINT8 * TSStr, UINT8 & Date1, UINT8 &Date2, UINT8 Min, UINT8 Hour, SINT8 Secs)
{
	UINT8 Temp3;
	if(Sysconfig->Dateformat == DFormat1){ //DD/MM/YY OR SYNC_WITH_PC
		//Date1 is Month part of date and Date 2 is Day part of date before when called by some function.
		//they got swapped if the format is DD/MM/YY. i.e Data1 becomes date part of date and Date2 becomes
		//month part of date. The caller function just has to use values in fixed format i.e. "%02d-%02d" without
		//worrying about the values these two variables contain. Function does take care of that based on DateFormat
		//slected from web page.
		Date1 = Date1 + Date2;
		Date2 = Date1 - Date2;
		Date1 = Date1 - Date2;
	}
	if(Sysconfig->Timeformat == TFormat1){
	 if(Hour < NOON){
		Temp3 = Hour;
		if(Hour == 0)
		   Temp3 = NOON;
		if(Secs < 0)
			sprintf(TSStr, "%02d:%02d %s", Temp3, Min, "AM");
		else
			sprintf(TSStr, "%02d:%02d:%02d %s", Temp3, Min, Secs, "AM");
	 }
	 else if(Hour > NOON){
		 if(Secs < 0)
			 sprintf(TSStr, "%02d:%02d %s", (Hour % NOON), Min, "PM");
		 else
			 sprintf(TSStr, "%02d:%02d:%02d %s", (Hour % NOON), Min, Secs, "PM");
	 }
	 else if(Hour == NOON){
		 if(Secs < 0)
			 sprintf(TSStr, "%02d:%02d %s", Hour, Min, "PM");
		 else
			 sprintf(TSStr, "%02d:%02d:%02d %s", Hour, Min, Secs, "PM");
	 }
	}
	else{
		if(Secs < 0)
			sprintf(TSStr, "%02d:%02d", Hour, Min);
		else
			sprintf(TSStr, "%02d:%02d:%02d", Hour, Min, Secs);
	}
}
