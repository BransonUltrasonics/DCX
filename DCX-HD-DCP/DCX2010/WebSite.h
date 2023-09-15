/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/WebSite.h_v   1.7.2.9.1.5   May 28 2018 23:56:44   EGUAJARDO  $
*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/WebSite.h_v  $
 * 
 *    Rev 1.7.2.9.1.5   May 28 2018 23:56:44   EGUAJARDO
 * Added functions for stack adjustment flag.
 * 
 *    Rev 1.7.2.9.1.4   Dec 29 2017 14:11:26   hasanchez
 * Added new web commands
 * 
 *    Rev 1.7.2.9.1.3   Mar 29 2017 09:18:34   hasanchez
 * Updated the sample rate.
 * 
 *    Rev 1.7.2.9.1.2   Mar 15 2017 12:15:34   hasanchez
 * Added the maximum and minimum sample limits. 
 * 
 *    Rev 1.7.2.9.1.1   Feb 09 2017 15:19:08   hasanchez
 * Added commands and definitions to Manager Basic
 * 
 *    Rev 1.7.2.9.1.0   17 Mar 2016 16:23:56   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.7.2.10   17 Mar 2016 16:07:34   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.7.2.9   19 Sep 2014 07:35:26   amaurya
 * Changes for complete.bin upload through website.
 * 
 *    Rev 1.7.2.8   02 Sep 2014 07:03:06   amaurya
 * Added command for Web Seek.
 * 
 *    Rev 1.7.2.7   01 Apr 2014 07:53:50   rjamloki
 * extern the variable WebWeldBufferIndex & WebWeldBufferTime to be used in Weld.cpp.
 * 
 *    Rev 1.7.2.6   13 Feb 2013 09:23:12   amaurya
 * Added Graphs, Energy special and rescaling special.
 * 
 *    Rev 1.9   15 Jan 2013 04:44:14   amaurya
 * Energy Special.
 * 
 *    Rev 1.7.2.4.1.1   01 Nov 2012 05:16:12   amaurya
 * Added multiple resonance marker.
 * 
 *    Rev 1.7.2.4.1.0   23 Aug 2012 11:34:44   amaurya
 * Implemented S & V Graph Plot.
 * 
 *    Rev 1.7.2.4   28 Aug 2011 11:42:28   ygupta
 * Ver1.01M
 * 
 *    Rev 1.7.2.3   11 Aug 2011 08:57:56   pdwivedi
 * Added test button on manufacturing.
 * 
 *    Rev 1.7.2.2   15 Jul 2011 10:32:16   pdwivedi
 * Added changes for Team Track issue and requirement changes.
 * 
 *    Rev 1.7.2.1   09 May 2011 08:35:48   ygupta
 * Timeouts increased
 * 
 *    Rev 1.7   20 Apr 2011 11:39:08   ygupta
 * Added changes for tracker issues.
 * 
 *    Rev 1.6   14 Apr 2011 06:24:08   PDwivedi
 * Added changes for IP Setup issue and added comments.
 * 
 *    Rev 1.5   04 Apr 2011 10:58:18   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.4   02 Mar 2011 08:18:12   PDwivedi
 * Added Manufacturing Board Test Webpage support.
 * 
 *    Rev 1.3   14 Jan 2011 07:55:46   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.2   16 Dec 2010 08:17:44   PDwivedi
 * Added changes for Website Code Clean Up and Compressed file download from serial.
 * 
 *    Rev 1.1   14 Dec 2010 11:15:16   PDwivedi
 * Added changes for WebSite support.
Revision 1.1  2010/12/08 16:24:12  Noor
code to test http server problem.

 * 
 *    Rev 1.0   29 Oct 2010 10:40:48   ygupta
 * Initial revision.

******************************************************************************/

#ifndef WEBSITE_H_
#define WEBSITE_H_

#include "portable.h"
#include "HttpServer.h"
#include "CyGOS.h"

//The port this website will run on
#define DCXWEBSITE_PORT 80
//Anlog input values get multiplied by this factor before sending to webpage
#define MI_ANALOGIP_MAXMV 10000
#define MINSAMPLETIME 50
#define MAXSAMPLETIME 2000

#define MINSAMPLELIMIT 120
#define MAXSAMPLELIMIT 2000


//Website users
enum UserType
{
	ErrUser,
	Branson,
	User,
	Manufacturing1,
	Manufacturing2,//Board test
};

//Language in which strings should be shown on web pages
//currently only English language is supported
enum Language
{
	English,
	French,
	Japanese,
	Chinese
};

//Users along with their ID will go into the array of this struct
struct Users
{
	char* UserID;
	UserType User;
};


//Digital input/output functions to be shown on IO configuration page
//will go into the array of this struct.
struct WebDigitalIOFunctionTable
{
	int stringID;//string ID as defined in English.lan file for digital IO functions in www folder
	int ActivehighID;//Active high ID as defined in enum USERDEFINED_DIGINPUTFUNC or USERDEFINED_DIGOUTPUTFUNC
	int ActivelowID;////Active low ID as defined in enum USERDEFINED_DIGINPUTFUNC or USERDEFINED_DIGOUTPUTFUNC
};

//Analog Input/Output functions to be shown on IO configuration page will
//go into the array of this struct
struct WebAnalogIOFunctionTable
{
	int stringID;//IDs as defined in English.lan file in www folder
	int functionID;//Function IDs for Analog IO functions as defined in USERDEFINED_ANALOGINPUTFUNC or USERDEFINED_ANALOGOUTPUTFUNC
	int Power;//zero if function is not power output else the max power that is equal to 10V output.
};

//Type of signals to be shown on IOconfig page
enum SignalType
{
	sigDigIn,
	sigDigOut,
	sigAnalogIn,
	sigAnalogOut,
};


//this command will be sent in uri by javascript code
enum WebCommand
{
	reqNone = 0,
	reqRestore,//1
	reqEventdata,//2
	reqDownloadEventdata,//3
	reqAlarmdata,
	reqDownloadAlarmData,
	reqScanStart,
	reqScanPoll,
	reqScanAbort,
	reqSeekStart,//9
	reqSeekPoll,//10
	reqReserved,
	reqSeekResult,
	reqDiagStart,
	reqDiagStop,
	reqDiagPoll,
	reqDiagLoopbackStart,
	reqClearAlarmLog ,
	reqClearEventLog ,
	reqDiagLoopbackPoll,
	reqSearchWelders,
	reqBoardTestLogin,
	reqBoardTestPoll,//22
	reqBoardTestRefresh,//23
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
	regMFTestStart, //33
	regMFTestStop,
	reqMFTestPoll,
	//Commands for WeldData / Scan Data
	reqTimeData,//36
	reqPhaseData,//37
	reqCurrentData,//38
	reqAmplitudeData,//39
	reqPowerData,//40
	reqPWMAmplitudeData,//41
	reqFrequencyData,//42
	reqPresetData,//43
	reqResFreq,//44
	reqSpecialEnter,//45
	//temp download commands
	reqDownloadWeldGraphData,//46
	reqDownloadScanGraphData,//47

	reqPWMAmpValue, // 48
	reqPWMStartTest,//49
	reqPWMStopTest,//50
};

/*function type to deal with website commands*/
typedef void (*webCmdHandler)(char * paramStr, WebCommand cmd);
struct webSiteCommand
{
	webCmdHandler handler;
};

class WebSite
{
	static int port,interface;//port and ethernet interface on which this website will run
	static char* PostResponse(char* uri, UserType usr);
	static void GetLanguageStrings(char * strIds, char * response);
	static char * FindLanguageString(int StringId, char * fileBuff, BOOLEAN strOnly);
	static char * GetFunctionString(int signalType, int funcID, int activeStatus, char * fileBuff);
	static void RestoreOutPutStatus();
	static void UpdateDiagnosticIO(char * paramStr);
	static void DoDiagnosticLoopBack();
	static void StopIODiagnostic();
	static void DoDiagnosticLoopBackStart();
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
	static BOOLEAN testStartFlag;
	static BOOLEAN scanStartFlag;
	static BOOLEAN scanAbortFlag;
	static BOOLEAN diagFlag;
	static BOOLEAN diagPSFlag;//todo
	static BOOLEAN diagStartFlag;
	static BOOLEAN resetFlag;
	static BOOLEAN mfBoardTestFlag;
	static BOOLEAN mfFlag;
	static int webtimeOutScan;
	static int webtimeOutTest;
	static int diagnosticTimeOut;
	static int diagnosticPSTimeOut;
	static int StartDiagTimeOut;
	static int mfBoardTestTimeOut;
	static int WebTimeOutUpdateGraph;
	static int mfTimeOut;
	static char * languageBuff;
	static NetworkConfiguration currNetworkConfig;
	public:
	static BOOLEAN mfTestFlag;
	static BOOLEAN StackAdjFlag;
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
	static void  ProcessPSTestOnLoad(char * paramStr, WebCommand cmd);
	static void ProcessResetCommand(char * paramStr, WebCommand cmd);
//	static void ProcessMacChangeCmd(char * paramStr, WebCommand cmd);
	static void ProcessManufacturingCmd(char * paramStr, WebCommand cmd);
	static void ProcessManufacturingBoardTestCmd(char * paramStr, WebCommand cmd);
	static void ViewWeldGraphGetDatahandeler(char * paramStr, WebCommand cmd);
	static void DiagnosticsHornSignatureGetDatahandeler(char * paramStr, WebCommand cmd);
	static void ViewGraphHornSignatureGetDatahandeler(char * paramStr, WebCommand cmd);
    //HD
	static void GetHDParamVal(char * paramStr, WebCommand cmd);
	static void UpdateHDParamVal(char * paramStr, WebCommand cmd);
	static void UpdateHDPWMAmp(char * paramStr, WebCommand cmd);
    static void UpdateHDParamVal1(char * paramStr, WebCommand cmd);
	static void GetGraphData(char * paramStr, WebCommand cmd);
	
	public:
	static BOOLEAN WebPostHandler(char * postData, char * uri, HTTPReplyHeader & header);
	static BOOLEAN WebGetHandler(char * uri, HTTPReplyHeader & header);
	static BOOLEAN WebFileUploadHandler(char * data, int dataLen,  char * fileName, char* Action);
	static void InitWebSite(int intf, int srvrPort);

	static BOOLEAN CheckForWebTestCommand();
	static BOOLEAN CheckForWebScanCommand();
	static BOOLEAN CheckForWebScanAbortCommand();
	static BOOLEAN CheckForDiagnosticFlag();
	static BOOLEAN CheckForWebReset();
	static BOOLEAN CheckForMFBoardTestFlag();
	static BOOLEAN CheckForManufacturingTestFlag();
	static BOOLEAN CheckForManufacturingFlag();
	static BOOLEAN GetStackAdjStatus();
	static void ResetStackAdjFlag();
	static void ResetScanFlags();
	static void ResetWebResetFlag();
	static void Tick(int callbackus);
	static unsigned int GetUserID();
	static void ResetTestFlag();
};
extern SINT16 WebWeldBufferIndex;
extern BOOLEAN WebWeldBuffRequestPending;
extern UINT32 WebWeldBufferTime;

#endif /*WEBSITE_H_*/
