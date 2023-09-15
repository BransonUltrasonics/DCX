/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WebSite.h_v   1.3.1.3   06 Jun 2016 16:50:42   GButron  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WebSite.h_v  $
 * 
 *    Rev 1.3.1.3   06 Jun 2016 16:50:42   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.3.1.2   25 Jun 2015 07:07:46   amaurya
 * Added define to check if change in value then log an Event
 * 
 *    Rev 1.3.1.1   19 Mar 2015 07:15:00   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.3.1.0   13 Jan 2015 06:03:24   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.3   29 Sep 2014 08:06:44   amaurya
 * Added define for MI_ANALOGIP_MAXMV_FREQ
 * 
 *    Rev 1.2   25 Sep 2014 03:31:58   amaurya
 * Added define for  MI_ANALOGIP_MAXMV_AMP
 * 
 *    Rev 1.1   17 Sep 2014 09:07:28   amaurya
 * Code review changes fix and IO Config page changes
 * 
 *    Rev 1.0   18 Aug 2014 09:17:34   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:08   amaurya
 * Initial revision.
 */

#ifndef WEBSITE_H_
#define WEBSITE_H_

#include "portable.h"
#include "HttpServer.h"
#include "CyGOS.h"
#define DCXWEBSITE_PORT                       80         //The port this website will run on
#define MI_ANALOGIP_MAXMV                     10000      //Analog input values get multiplied by this factor before sending to webpage
#define MI_ANALOGIP_MAXMV_AMP                 20000
#define MI_ANALOGIP_MAXMV_FREQ                20000
                                                         //Define to check if change in value then log an Event.
#define CHECKANDLOG_PRESETCHANGE(Psstruct, Param, NewValue, ParamID, LogValue, UserId, Data) \
   if(Psstruct.Param != NewValue){\
      Log::WriteEventLog(PARAM_CHANGED,  ParamID, LogValue, UserId,Data);\
      (Psstruct.Param = NewValue);}

//Website users
enum UserType {
	ErrUser, Branson, User, Manufacturing1, Manufacturing2, //Board test
};

//Language in which strings should be shown on web pages
//currently only English language is supported
enum Language {
	English, French, Japanese, Chinese
};

//Users along with their ID will go into the array of this struct
struct Users {
	char* UserID;
	UserType User;
};

//Digital input/output functions to be shown on IO configuration page
//will go into the array of this struct.
struct WebDigitalIOFunctionTable {
	int stringID; //string ID as defined in English.lan file for digital IO functions in www folder
	int ActivehighID; //Active high ID as defined in enum USERDEFINED_DIGINPUTFUNC or USERDEFINED_DIGOUTPUTFUNC
	int ActivelowID; ////Active low ID as defined in enum USERDEFINED_DIGINPUTFUNC or USERDEFINED_DIGOUTPUTFUNC
};

//Analog Input/Output functions to be shown on IO configuration page will
//go into the array of this struct
struct WebAnalogIOFunctionTable {
	int stringID; //IDs as defined in English.lan file in www folder
	int functionID; //Function IDs for Analog IO functions as defined in USERDEFINED_ANALOGINPUTFUNC or USERDEFINED_ANALOGOUTPUTFUNC
	int Power; //zero if function is not power output else the max power that is equal to 10V output.
};

//Type of signals to be shown on IOconfig page
enum SignalType {
	sigDigIn, sigDigOut, sigAnalogIn, sigAnalogOut,
};

//this command will be sent in uri by javascript code
enum WebCommand {
	reqNone = 0,
	reqRestore,
	reqEventdata,
	reqDownloadEventdata,
	reqAlarmdata,
	reqDownloadAlarmData,
	reqScanStart,
	reqScanPoll,
	reqScanAbort,
	reqSeekStart, //9
	reqSeekPoll, //10
	reqReserved,
	reqSeekResult,
	reqDiagStart,
	reqDiagStop,
	reqDiagPoll,
	reqDiagLoopbackStart,
	reqClearAlarmLog,
	reqClearEventLog,
	reqDiagLoopbackPoll,
	reqSearchWelders,
	reqBoardTestLogin,
	reqBoardTestPoll, //22
	reqBoardTestRefresh, //23
	//Commands from Manufacturing Web Page
	reqMFOnLoad,
	reqDownloadCycleResults,
	reqRestoreDefaults,
	reqClearAlarmLogMF,
	reqClearEventLogMF,
	reqResetAlarmCounter,
	reqStartStopCycle,
	reqMFPoll,
	reqSaveSysSetup,
	regMFTestStart,
	regMFTestStop,
	reqMFTestPoll,	//35
	//Commands for WeldData / Scan Data
	reqTimeData,	//36
	reqPhaseData,	//37
	reqCurrentData,	//38
	reqAmplitudeData,	//39
	reqPowerData,	//40
	reqPWMAmplitudeData,	//41
	reqFrequencyData,	//42
	reqPresetData,	//43
	reqResFreq,	//44
	reqSpecialEnter,	//45
};

/*function type to deal with website commands*/
typedef void (*webCmdHandler)(char * paramStr, WebCommand cmd);
struct webSiteCommand {
	webCmdHandler handler;
};


class WebSite {
	static int port, interface;//port and ethernet interface on which this website will run
	static char* PostResponse(char* uri, UserType usr);
	static void GetLanguageStrings(char * strIds, char * response);
	static char * FindLanguageString(int StringId, char * fileBuff,
	      BOOLEAN strOnly);
	static char * GetFunctionString(int signalType, int funcID, int activeStatus,
	      char * fileBuff);
	static void RestoreOutPutStatus();
	static void UpdateDiagnosticIO(char * paramStr);
	static void DoDiagnosticLoopBack();
	static void StopIODiagnostic();
	static void DoDiagnosticLoopBackStart(void);
	static void PrepareDiagnosticResponse(bool inputsOnly = false);
	static void ProcessMFBoardTestPollResponse();
	static void ProcessMFBoardTestRefreshCmd(char * param);
	static void ProcessMFStartStopCommand(char * param);
	static void ProcessMFSavesystemSetupCommand(char * param);
	static BOOLEAN CheckForCycleParameterChange(void);
//	static void DoDiagnosticLoopBackStop();
	static void UpdateUser(UserType usr);
	static char * DownLoadRequest(char * uri, char * file);
	static char * HandleInitiateUpload();
	static void SetLanguageFileBuffer(Language lang);
	static unsigned int DcxWebUserID;
	static unsigned int DcxLogUserID;
	static BOOLEAN scanStartFlag;
	static BOOLEAN scanAbortFlag;
	static BOOLEAN diagFlag;
	static BOOLEAN resetFlag;
	static BOOLEAN mfBoardTestFlag;
	static BOOLEAN mfFlag;
	static int webtimeOutScan;
	static int webtimeOutTest;
	static int diagnosticTimeOut;
	static int diagnosticPSTimeOut;
	static int StartDiagTimeOut;
	static int mfBoardTestTimeOut;
	static int mfTimeOut;
	static char * languageBuff;
	static NetworkConfiguration currNetworkConfig;
public:
	static BOOLEAN DiagStartFlag;
	static BOOLEAN mfTestFlag;
	//Handlers
	static void GetIPSetupVal(char * paramStr, WebCommand cmd);
	static void SetLanguageAndUserID(char * paramStr, WebCommand cmd);//TODO nothing really needed
	static void GetIOConfigurationVal(char * paramStr, WebCommand cmd);
	static void GetModifyPrestVal(char * paramStr, WebCommand cmd);
	static void GetModifyPrestAdvancedVal(char * paramStr, WebCommand cmd);
	static void GetDiagnosticDataVal(char * paramStr, WebCommand cmd);
	static void GetSystemInformationVal(char * paramStr, WebCommand cmd);
	static void UpdateIPSetup(char * paramStr, WebCommand cmd);
	static void UpdateIOConfiguration(char * paramStr, WebCommand cmd);
	static void ProcessDiagnosticIOCommand(char * paramStr, WebCommand cmd);
	static void UpdatePreset(char * paramStr, WebCommand cmd);
	static void UpdatePresetAdvanced(char * paramStr, WebCommand cmd);
	static void ProcessTestCommand(char * paramStr, WebCommand cmd);
	static void ProcessScanCommand(char * paramStr, WebCommand cmd);
	static void GetLogData(char * paramStr, WebCommand cmd);
	static void GetLastSucessfullHornScanData(char * paramStr, WebCommand cmd);
	static void ProcessPSTestOnLoad(char * paramStr, WebCommand cmd);
	static void ProcessResetCommand(char * paramStr, WebCommand cmd);
//	static void ProcessMacChangeCmd(char * paramStr, WebCommand cmd);
	static void ProcessManufacturingCmd(char * paramStr, WebCommand cmd);
	static void ProcessManufacturingBoardTestCmd(char * paramStr,
	      WebCommand cmd);
	static void ViewWeldGraphGetDatahandeler(char * paramStr, WebCommand cmd);
	static void DiagnosticsHornSignatureGetDatahandeler(char * paramStr,
	      WebCommand cmd);
	static void ViewGraphHornSignatureGetDatahandeler(char * paramStr,
	      WebCommand cmd);
public:
	static BOOLEAN WebPostHandler(char * postData, char * uri,
	      HTTPReplyHeader & header);
	static BOOLEAN WebGetHandler(char * uri, HTTPReplyHeader & header);
	static BOOLEAN WebFileUploadHandler(char * data, int dataLen,
	      char * fileName, char* Action);
	static void InitWebSite(int intf, int srvrPort);

	static BOOLEAN CheckForWebTestCommand(void);
	static BOOLEAN CheckForWebScanCommand(void);
	static BOOLEAN CheckForWebScanAbortCommand(void);
	static BOOLEAN CheckForDiagnosticFlag(void);
	static BOOLEAN CheckForWebReset(void);
	static BOOLEAN CheckForMFBoardTestFlag(void);
	static BOOLEAN CheckForManufacturingFlag(void);
	static void ResetScanFlags(void);
	static void ResetWebResetFlag(void);
	static void Tick(int callbackus);
	static unsigned int GetUserID(void);
	static void ResetTestFlag(void);
};
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
