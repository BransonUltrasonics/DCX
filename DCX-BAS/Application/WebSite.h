/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/WebSite.h_v   1.29   13 Mar 2014 15:55:34   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/WebSite.h_v  $
 * 
 *    Rev 1.29   13 Mar 2014 15:55:34   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.28   04 Mar 2014 07:49:36   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
 * 
 *    Rev 1.27   06 Dec 2013 09:42:14   rjamloki
 * S and V loop constants incorporated.
 * 
 *    Rev 1.26   06 Nov 2013 06:20:44   rjamloki
 * Going to preready on user input configuration change to prevent sonics.
 * 
 *    Rev 1.25   08 Oct 2013 06:17:18   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.24   20 Sep 2013 07:00:26   rjamloki
 * Added changes to build bootloader with same CyGOS.
 * 
 *    Rev 1.23   24 Aug 2013 17:14:26   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.22   10 Jul 2013 07:53:38   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.21   08 Jul 2013 08:57:32   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.20   27 Jun 2013 05:58:46   amaurya
 * System crash fixes.
 * 
 *    Rev 1.19   14 Jun 2013 11:21:18   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.18   31 May 2013 10:37:26   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.17   28 May 2013 07:45:28   amaurya
 * Fixed issues of Level II testing.
 * 
 *    Rev 1.16   21 May 2013 12:26:14   amaurya
 * Code review fixes.
 * 
 *    Rev 1.15   22 Apr 2013 11:34:42   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.14   15 Mar 2013 10:54:54   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.13   11 Mar 2013 02:31:02   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.12   18 Feb 2013 03:13:06   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.11   29 Jan 2013 02:25:24   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.10   11 Jan 2013 02:56:30   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.9   18 Dec 2012 07:40:22   ygupta
 * Changes for Multiple Overload alarm in case of 15V lost
 * 
 *    Rev 1.8   13 Dec 2012 00:14:40   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.7   29 Nov 2012 08:50:00   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.6   24 Nov 2012 08:08:06   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.5   08 Nov 2012 01:22:50   rjamloki
 * Website changes
 * 
 *    Rev 1.4   26 Oct 2012 02:42:38   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.2   02 Jul 2012 13:04:16   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.1   29 Jun 2012 12:17:32   ygupta
 * First Compilable code, First check in after review, Needs more clanup
 * 
 *    Rev 1.0   21 Mar 2012 12:22:52   PDwivedi
 * Initial revision.
 *
 */

#ifndef WEBSITE_H_
#define WEBSITE_H_

#include "portable.h"
#include "HttpServer.h"
#include "CyGOS.h"
#include "SysConfig.h"
#include "TYP_DEF.h"
#include "PasscodeConfig.h"
//The port this website will run on
#define DCXWEBSITE_PORT 80
#define CHECKANDLOG_PRESETCHANGE(PresetNum, Psstruct, Param, NewValue, ParamID, EventLogValue, UserId) \
       if(ParameterSet[PresetNum].Psstruct.Param != NewValue){\
         Log::WriteEventLog(EVENT_PARAMETER_CHANGED, ((PresetNum << 8) | ParamID), EventLogValue, UserId);\
         (ParameterSet[PresetNum].Psstruct.Param = NewValue);\
          Sysconfig->ActivePreset = CurrPreset;}

#define CHECKANDLOG_HSPRESETCHANGE(HsPresetNum, Hspsstruct, Param, NewValue, ParamID, EventLogValue, UserId) \
       if(HsParameterSet[HsPresetNum].Hspsstruct.Param != NewValue){\
         Log::WriteEventLog(EVENT_PARAMETER_CHANGED, ((HsPresetNum << 8) | ParamID), EventLogValue, UserId);\
         (HsParameterSet[HsPresetNum].Hspsstruct.Param = NewValue);}


#define CHECKANDLOG_HSPARMCHANGE(HsPresetNum, Param, NewValue, ParamID, EventLogValue, UserId) \
       if(HsParameterSet[HsPresetNum].Param != NewValue){\
         Log::WriteEventLog(EVENT_PARAMETER_CHANGED, ((HsPresetNum << 8) | ParamID), EventLogValue, UserId);\
         (HsParameterSet[HsPresetNum].Param = NewValue); \
         (HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].Param = NewValue);}

//Type of signals to be shown on IOconfig page
enum SignalType{
	SIG_DIGIN,
	SIG_DIGOUT,
	SIG_ANALOGIN,
	SIG_ANALOGOUT,
};

//Digital input/output functions to be shown on IO configuration page will go into the array of this struct.
struct WebDigitalIOFunctionTable{
	SINT32 StringID; //string ID as defined in English.lan file for digital IO functions in www folder
	SINT32 ActivehighID; //Active high ID as defined in enum USERDEFINED_DIGINPUTFUNC or USERDEFINED_DIGOUTPUTFUNC
	SINT32 ActivelowID; //Active low ID as defined in enum USERDEFINED_DIGINPUTFUNC or USERDEFINED_DIGOUTPUTFUNC
};

//Analog Input/Output functions to be shown on IO configuration page will go into the array of this struct.
struct WebAnalogIOFunctionTable{
	SINT32 StringID;//IDs as defined in English.lan file in www folder
	SINT32 FunctionID;//Function IDs for Analog IO functions as defined in USERDEFINED_ANALOGINPUTFUNC or USERDEFINED_ANALOGOUTPUTFUNC
};

//this command will be sent in uri by javascript code
enum WebCommand{
	REQ_NONE = 0,
	REQ_RESTORE,//1
	REQ_DCPEVENTDATA,//2
	REQ_WCEVENTDATA,//3
	REQ_DOWNLOAD_DCPEVENTDATA, //4
	REQ_DOWNLOAD_WCEVENTDATA, //5
	REQ_ALARMDATA,//6
	REQ_DOWNLOAD_ALARMDATA,//7
	REQ_SCAN_START,//8
	REQ_SCAN_POLL,//9
	REQ_SCAN_ABORT,//10
	REQ_SEEK_START,//11
	REQ_SEEK_POLL,//12
	REQ_SEEK_STOP,//13
	REQ_SEEK_RESULT,//14
	REQ_DIAG_START,//15
	REQ_DIAG_STOP,//16
	REQ_DIAG_POLL,//17
	REQ_DIAG_LOOPBACK_START,//18
	REQ_CLEAR_ALARMLOG ,//19
	REQ_CLEAR_EVENTLOG ,//20
	REQ_DIAG_LOOPBACK_POLL,//21
	REQ_SEARCH_WELDERS,//22
	REQ_BOARDTEST_LOGIN,//23
	REQ_BOARDTEST_POLL,//24
	REQ_BOARDTEST_REFRESH,//25
	//COMMANDS FROM MANUFACTURING WEB PAGE
	REQ_MFONLOAD,//26
	REQ_DOWNLOAD_CYCLERESULTS,//27
	REQ_RESTOREDEFAULTS,//28
	REQ_CLEAR_ALARMLOGMF,//29
	REQ_CLEAR_EVENTLOGMF,//30
	REQ_RESET_ALARMCOUNTER,//31
	REQ_STARTSTOPCYCLE,//32
	REQ_MFPOLL,//33
	REQ_SAVE_SYSSETUP,//34
	REQ_MFTEST_START,//35
	REQ_MFTEST_STOP,//36
	REQ_MFTEST_POLL,//37
   REQ_SAVE_PRESET,//38
   REQ_VERIFY_PRESET,//39
   REQ_RECALL_PRESET,//40
   REQ_RESET_CYCLERUN,//41
//  COMMANDS FOR WELDDATA / SCAN DATA
   REQ_CURRENT_DATA,//42
   REQ_PHASE_DATA,//43
   REQ_AMPLITUDE_DATA,//44
   REQ_FREQUENCY_DATA,//45
   REQ_PRESET_DATA, //46
   REQ_TIME_DATA,//47
   REQ_POWER_DATA, //48
   REQ_PWMAMPLITUDE_DATA,//49
   REQ_WELDHISTORY,//50
   REQ_DOWNLOAD_WELDHISTORY,//51
   REQ_CLEAR_WELDHISTORY,//52
   REQ_DOWNLOAD_FRAM_CONFIGURATION,//53
   REQ_FANCONTROL,//54
   REQ_RESFREQ,//55
   REQ_SAVE,//56
   REQ_TOKEN,//57
   REQ_COLDSTART,//58
   REQ_SYSCALIBRATION,//59
   REQ_RUN_START,//60
   REQ_CLEAR_DCPEVENTLOG,//61
   REQ_CLEAR_MEMORYFREQ,//62
   REQ_DATETIME//63
};

enum WebPagesForEventWrite{
   SETUP_WELD = 1,
   SETUP_AMPLITUDE,
   SETUP_LIMITS_CUTOFFS,
   SETUP_SEEKPOWERUP,
   SETUP_ADVANCED_WELD,
   SETUP_ADV_DIAGNOSTICS,
   BRANSON_SECRET_VALUES,
   BRANSON_MANUFACTURING_PAGE,
   BRANSON_FIELD_SERVICE,
   BRANSON_SPECIAL_PRESET,
};
void SetDCPTimingParam(WebCommand Cmd);
/*function type to deal with website commands*/
typedef void (*webCmdHandler)(SINT8 * ParamStr, WebCommand cmd);
struct webSiteCommand{
   webCmdHandler handler;
};
struct SlaveData{
   UINT32  SlaveAddress;
   UINT32  DataFormat;
};
struct Boardtest{
   SINT32 Testfreq;
   UINT32 Overloadreset;
   UINT32 Stop;
   SINT32 PhaseA;
   SINT32 PhaseB;
};

class WebSite
{
   static UINT32 Port,Interface;//port and ethernet interface on which this website will run
	static void PostResponse(SINT8 * Uri, UINT32 UserPrivilege);
	static void GetLanguageStrings(SINT8 * StrIds, SINT8 * Response);
	static SINT8 * FindLanguageString(UINT32 StringId, SINT8 * FileBuff, BOOLEAN StrOnly);
	static SINT8 * GetFunctionString(SINT32 SignalType, SINT32 FuncID, SINT32 ActiveStatus, SINT8 * FileBuff);
	static void RestoreOutPutStatus(void);
	static void UpdateDiagnosticIO(SINT8 * ParamStr);
	static void DoDiagnosticLoopBack(void);
	static void StopIODiagnostic(void);
	static void DoDiagnosticLoopBackStart(void);
	static void PrepareDiagnosticResponse(BOOLEAN InputsOnly = false);
	static void ProcessMFBoardTestPollResponse(void);
	static void ProcessMFBoardTestRefreshCmd(SINT8 * Param);
	static void ProcessMFStartStopCommand(SINT8 * Param);
	static void ProcessMFSavesystemSetupCommand(SINT8 * Param);
	static BOOLEAN CheckForCycleParameterChange(void);
	static void UpdateUser(UINT32 UserPrivilege);
	static void DownLoadRequest(SINT8 * uri, SINT8 * file);
	static void HandleInitiateUpload(void);
	static void SetLanguageFileBuffer(Language lang);
	static SINT8 DcxWebUserID[MAX_LEN_USERID + 1];
	static SINT8 * DcxLogUserID;
	static BOOLEAN TestStartFlag;
	static BOOLEAN ScanAbortFlag;
	static BOOLEAN ScanFailedFlag;

	static BOOLEAN diagStartFlag;
	static BOOLEAN mfBoardTestFlag;
	static BOOLEAN mfFlag;
	static SINT32 webtimeOutScan;
	static SINT32 webtimeOutTest;
	static SINT32 diagnosticTimeOut;
	static SINT32 StartDiagTimeOut;
	static SINT32 mfBoardTestTimeOut;
	static SINT32 mfTimeOut;
	static SINT8 * languageBuff;
   public:
	static BOOLEAN diagFlag;
   static BOOLEAN resetFlag;
	static BOOLEAN mfTestFlag;
   static BOOLEAN ScanStartFlag;
	static UINT32 PowerUpId;
 // Website new command handlers
   static void SetupWeldGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupWeldPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupLimitsGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupLimitsPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupSaveRecallGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupSaveRecallPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupAmplitudeGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupAmplitudePostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupSeekPowerUpGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupSeekPowerUpPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupAdvancedGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupAdvancedPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ProcessScanCommand(SINT8 * ParamStr, WebCommand Cmd);
   static void ViewAlarmLogGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ViewEventLogGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ViewHistoryGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ViewWeldGraphGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ViewWeldGraphPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void DiagnosticSeekGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void DiagnosticSeekPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void DiagnosticHornSignatureGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void DiagnosticHornSignaturePostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void DiagnosticUserIOGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void DiagnosticUserIOPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void DiagnosticFieldBusGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationSystemGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationSystemPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationUserIOGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationUserIOPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationIPSetupGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationIPSetupPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationPasscodesGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationPasscodesPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationAlarmGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationAlarmPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationRFSwitchGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void ConfigurationRFSwitchPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonSecretValuesGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonSecretValuesPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonSystemTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonBoardTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonBoardTestPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonFieldBusTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonFieldBusTestPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonAdvRnDTestGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonAdvRnDTestPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonFieldServiceGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonFieldServicePostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonSpecialPresetGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void BransonSpecialPresetPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetLanguageAndUserID(SINT8 * ParamStr, WebCommand Cmd);
   static void ViewSystemInfoGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupAdvancedDiagnosticGetDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void SetupAdvancedDiagnosticPostDatahandler(SINT8 * ParamStr, WebCommand Cmd);
   static void GetWebToken(SINT8 * ParamStr, WebCommand Cmd);
	public:
	static BOOLEAN WebPostHandler(SINT8 * PostData, SINT8 * Uri, HTTPReplyHeader &Header);
	static BOOLEAN WebGetHandler(SINT8 * Uri, HTTPReplyHeader &Header);
	static BOOLEAN WebFileUploadHandler(SINT8 * Data, int DataLen,  SINT8 * FileName, SINT8 * Action);
	static void InitWebSite(SINT32 Intf, SINT32 SrvrPort);
	static BOOLEAN CheckForWebTestCommand(void);
	static BOOLEAN CheckForWebScanCommand(void);
   static BOOLEAN CheckForWebScanAbortCommand(void);
	static BOOLEAN CheckForDiagnosticFlag(void);
   static BOOLEAN CheckForWebReset(void);
	static BOOLEAN CheckForMFBoardTestFlag(void);
	static BOOLEAN CheckForManufacturingTestFlag(void);
	static BOOLEAN CheckForManufacturingFlag(void);
	static void ResetScanFlags(BOOLEAN ResetID = TRUE);
	static void ResetWebResetFlag(void);
	static void Tick(int callbackus);
	static SINT8 * GetUserID(void);
	static NetworkConfiguration CurrNetworkConfig;
	static void ResetTestFlag(void);
	static UINT32 GenerateKey(void);
};
//new dummy defaults
void SetDefaultSecretValues(void);
void SetDefaultRFswitch(void);
void SetDefaultAdvRnd(void);
void SendRTCSyncCommandToDCP(void);
CurrentState CheckForCurrentFunction(SINT16 CurrIndx);
UINT32 CopyLength(UINT32 MaxExpectedLength, UINT32 ActualLength, int line);
void PrepareTimeStamp(SINT8 * TSStr, UINT8 & Date1, UINT8 &Date2, UINT8 Min, UINT8 Hour, SINT8 Secs = -1);
static const SINT8 * const FirmwareUpload =
		"<HTML>"
		"<script language=\"javascript\" type=\"text/javascript\">"
		"var SupportedExtensionsArr = [\"bin\", \"cyg\", \"cfg\"];"
		"var MaxFileNameLen = 200;"
		"var uploadInProgress = false, uploadInitiated = false;"
		"var http = new XMLHttpRequest();"
		"function HandleFileUploadInitiateCommand(){"
		" if(http.readyState == 4){"
		"  if (http.responseText == \"READY\")"
		"            SendFileUploadStartCommand();"
		"        else {"
		"            document.getElementById(\"btnupload\").disabled = false;"
		"            uploadInitiated = false;"
		"            alert(\"File upload not initiated\");"
		"        }"
		"    }"
		"}"
		"function SendFileUploadInitiateCommand(){"
		"    var indx;"
		"    var extMatched = false;"
		" if (uploadInitiated != true){"
		"  if (document.getElementById(\"file\").value == \"\"){"
		"   alert(\"Please select a file to upload\");"
		"  }"
		"  else{"
		"   var fileExt = document.getElementById(\"file\").value.split(\".\");"
		"   var ext = fileExt[fileExt.length - 1];"
		"   try{"
		"    for (indx = 0; indx < SupportedExtensionsArr.length && extMatched == false; indx++)"
		"     if (SupportedExtensionsArr[indx] == ext)"
		"      extMatched = true;"
		"   }"
		"   catch(err){};"
		"   if(extMatched){"
		"    var fileLen = document.getElementById(\"file\").value;"
		"    if (fileLen.length >= MaxFileNameLen)"
		"     alert(\"FileName including path should be less than 200 characters\");"
		"    else{"
		"     uploadInitiated = true;"
		"     document.getElementById(\"btnupload\").disabled = true;"
		"     var SystemInfoUrl = \"func=0\"  + \"cmd=0\"  + \"?param=.dcxfileupload\";"
		"     SystemInfoUrl += (\"&lang=0\" + \"&usrid=BRANSON\" + \"&passcode=32923646\");"
		"     http.open(\"POST\", SystemInfoUrl, true);"
		"     http.onreadystatechange = HandleFileUploadInitiateCommand;"
		"     http.send(null);"
		"    }"
		"   }"
		"   else"
		"    alert(\"Only bin and cyg file is supported\");"
		"  }"
		" }"
		"}"
		"function SendFileUploadStartCommand(){"
		"    if (uploadInProgress != true){"
		"     uploadInProgress = true;"
		"     document.getElementById(\"btnupload\").disabled = true;"
		"     fileLen = document.getElementById(\"file\").value.split(\"\\\\\");"
		"     document.getElementById(\"file_upload_form\").action = fileLen[fileLen.length - 1];"
		"     document.getElementById(\"file_upload_form\").submit();"
		"  }"
		"}"
		"</script>"
		"<BODY>"
		"<div class = \"PageHeader\"> Firmware Upload </div>"
		" <div class=\"contentBoxCenter\" >"
		" <div class=\"gradientBox400\">"
		" <form id=\"file_upload_form\" method=\"post\" enctype=\"multipart/form-data\" action=\"temp\">"
		" <input name=\"file\" id=\"file\" size=\"30\" type=\"file\" /><br /><br />"
		" <input type=\"button\" name=\"submitbutton\" id=\"btnupload\" value=\"Upload\" onclick=\"SendFileUploadInitiateCommand()\" /><br />"
		" </form>"
		" </div>"
		"</div>"
		"</BODY>"
		"</HTML>";

#endif /*WEBSITE_H_*/
