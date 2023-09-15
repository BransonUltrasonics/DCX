/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WebSite.cpp_v   1.32.1.3   Jan 11 2017 15:40:22   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WebSite.cpp_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Upload Default Values when activating any special
 *
 *    Rev 1.32.1.3   Jan 11 2017 15:40:22   EGUAJARDO
 * Added the system name DCX 222 RM on system information page when SIG special is activated.
 * 
 *    Rev 1.32.1.2   15 Sep 2016 15:46:44   EGUAJARDO
 * Added the energy ranges 10J and 30J to the analog input table.
 * 
 *    Rev 1.32.1.1   25 Jul 2016 13:07:38   GButron
 * RACKMOUNT_BASIC changed to DCX_S
 * 
 *    Rev 1.32.1.0   06 Jun 2016 16:47:04   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.32   19 Nov 2015 01:38:44   sesharma
 * The SIG special is activated by "159-132-2113" not by "179-266-418" . "159-132-2113" code is displayed on popup message as well as system information webpage, when SIG special is activated.
 * 
 *    Rev 1.30   03 Nov 2015 01:54:42   sesharma
 * Fixed for Handheld checkbox.
 * 
 *    Rev 1.29   30 Oct 2015 08:54:14   sesharma
 * Added define for SIG EDP no, Changed for Handheld mode
 * 
 *    Rev 1.28   01 Sep 2015 02:39:24   amaurya
 * Changed for Clear Memory and Set with Horn Scan checkbox.
 * 
 *    Rev 1.27   25 Jun 2015 07:07:36   amaurya
 * Changed for logging Event on change of weld parameter.
 * 
 *    Rev 1.26   18 Jun 2015 04:02:24   sesharma
 * Jumper2 settings checked only for DC-Basic. Unused header files removed.
 * 
 *    Rev 1.25   04 Jun 2015 07:55:16   sesharma
 * Added Clear memory before seek checkbox on Weld Preset page. Branson coding standard formatter applied.
 * 
 *    Rev 1.24   13 May 2015 08:47:24   sesharma
 * Frequency offset type casted by "short" instead of "int" when updated from webpage to correctly display values with negative sign. This change is done in GetModifyPrestVal function.
 * 
 *    Rev 1.23   09 Apr 2015 05:33:22   sesharma
 * FPGA Version String Added on System Information page.Reset Input from webpage will reset the alarm only when no external input is active.
 * 
 *    Rev 1.22   19 Mar 2015 07:14:22   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.21   26 Feb 2015 08:34:44   sesharma
 * Impulse Start should be restored to default only when system type is Modified.
 * 
 *    Rev 1.20   20 Feb 2015 17:14:46   ewomack
 * Removed Impulse Start assignment on system type change.  Asssignment now happens in Preset.cpp.
 * 
 *    Rev 1.19   18 Feb 2015 01:03:20   SaPawar
 * impulse start default value assignment on system type change and system busy on window limit alarm handled
 * 
 *    Rev 1.16   28 Jan 2015 05:38:24   sesharma
 * Unused variables removed.
 * 
 *    Rev 1.15   22 Jan 2015 04:52:46   sesharma
 * Date & Time in Event Log modified to Days & Hours.
 * 
 *    Rev 1.14   16 Jan 2015 06:31:02   sesharma
 * Changed Date to Days and Time to Hours in Alarm log, also changed Power On hours to Power On Days on System Information page.
 * 
 *    Rev 1.13   13 Jan 2015 06:03:12   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.12   27 Nov 2014 09:16:24   sesharma
 * Webpages and SIG related changes 
 * 
 *    Rev 1.11   13 Nov 2014 14:59:50   ewomack
 * Fixed formatting of hours of sonics and hours powered on.
 * 
 *    Rev 1.10   13 Nov 2014 12:23:02   ewomack
 * Fixed formatting of column headers when exporting even and alarm logs.
 * 
 *    Rev 1.9   07 Nov 2014 16:28:14   ewomack
 * Added SIG special.  Changes to Weld Preset page for DC Std.
 * 
 *    Rev 1.8   28 Oct 2014 16:51:26   ewomack
 * Reset weld preset parameters on PS frequency change.
 * 
 *    Rev 1.7   22 Oct 2014 19:08:04   ewomack
 * Fixed bug where Preset Information was being lost while changing system type.  On System Type change, all Preset, Cycle, and Horn Scan parameters were being set to default.
 * 
 *    Rev 1.6   16 Oct 2014 03:50:34   sesharma
 * Tracker Issues Resolved
 * 
 *    Rev 1.5   01 Oct 2014 10:09:46   amaurya
 * Corrected the freauency analog input scaling for Rack-S from -10V to 10V,
 * 
 *    Rev 1.4   29 Sep 2014 08:05:56   amaurya
 * Added changes to show -10.00V to 10.00V for Rack-Basic
 * 
 *    Rev 1.3   25 Sep 2014 03:30:48   amaurya
 * Added changes to show amplitude input values from -10V to +10V on webpages for Rack Basic and Standard.
 * 
 *    Rev 1.2   17 Sep 2014 09:07:28   amaurya
 * Code review changes fix and IO Config page changes
 * 
 *    Rev 1.1   03 Sep 2014 07:45:32   amaurya
 * Added changes to handle UserIO defaults, correct pin number, Clear memory at reset. 
 * 
 *    Rev 1.0   18 Aug 2014 09:17:32   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:08   amaurya
 * Initial revision.
 */

#include "WebSite.h"
#include "string.h"
#include "SysConfig.h"
#include "Globals.h"
#include "stdio.h"
#include "stdlib.h"
#include "FUNC.h"
#include "Version.h"
#include "LOG.h"
#include "StateMachineDrv.h"
#include "UserIO.h"
#include "TYP_DEF.h"
#include "UploadFirmwareTask.h"
#include "WelderSearchTask.h"
#include "ReceiveCommandHandler.h"
#include "preset.h"

#define MAX_RESPONSE_SIZE 80000//maximum response string length. This does not include html files
#define MAX_LANGUAGESTRING_LEN 100//maximum length of string in English.lan file in www folder
#define WEBSCAN_TIMEOUT 5000000 //in us
#define WEBTEST_TIMEOUT 5000000 //in us
#define WEBDIAG_TIMEOUT 5000000 //in us
#define LOOPBACK_TIMEOUT 5000000 //in us
#define MFBOARDTEST_TIMEOUT 5000000 //in us
#define MF_TIMEOUT 5000000 //in us
#define MFBOARDTESTPASSWORD "3278378"
#define NUM_BARGRAPH_SEGMENT 20
#define MIN_BOARD_TESTFREQUENCY 100 * 10//in Hz
#define TEST_BIT BIT(0)
#define OLRST_BIT BIT(1)
#define DPSSTOP_BIT BIT(2)
#define PHASEA_BIT BIT(3)
#define PHASEB_BIT BIT(4)
#define SPECIALPASSWORD "179-266-326" //EDP number for Energy and power out special.

#define SIG_SPECIAL "159-132-2113" //Special code for the SIG special
#define CLEAR_SPECIALS "000-000-000"

char Response[MAX_RESPONSE_SIZE];
char langString[MAX_LANGUAGESTRING_LEN];

#define MAXFILELEN 		45
#define USERIDBRANSON 	32923646
#define USERIDMANUF1 	287646
#define USERIDMANUF2 	26273
#define MAXUSERIDLEN 	15
//At powerup it should be zero. That means no user has logged in using website.
//It will get assigned a value once the user will login.
unsigned int WebSite::DcxWebUserID;
//At powerup it should be zero.It will be assigned a value in case some
//event is initiated from website. TEST,SCAN other events will be logged with this ID
unsigned int WebSite::DcxLogUserID;

//This array gets filled with Alarm or Event log before sending to website
LogData webLogData[MAX_EVENT_ENTRIES];

int WebSite::port;   //port this website is running on
int WebSite::interface;   //ethernet interface this website is running on

BOOLEAN WebSite::scanStartFlag;
BOOLEAN WebSite::scanAbortFlag;
BOOLEAN WebSite::diagFlag;
BOOLEAN WebSite::DiagStartFlag;
BOOLEAN WebSite::resetFlag;
BOOLEAN WebSite::mfBoardTestFlag;
BOOLEAN WebSite::mfFlag;
BOOLEAN WebSite::mfTestFlag;

extern BOOLEAN AlarmFlag;
extern char FPGAVersionLabel[FPGASWVERSION_LEN];

int WebSite::webtimeOutScan;
int WebSite::webtimeOutTest;
int WebSite::diagnosticTimeOut;
int WebSite::diagnosticPSTimeOut;
int WebSite::StartDiagTimeOut;
int WebSite::mfBoardTestTimeOut;
int WebSite::mfTimeOut;

char * WebSite::languageBuff;
char * funcStrUnassigned = "Unassigned";
char * resDefault = "Unknown Error";
NetworkConfiguration WebSite::currNetworkConfig;

BOOLEAN tempDigOutPutStatus[NUM_DOUT];
SINT32 tempAnalogOutPutVals[NUM_AOUT];

extern DIGITALIOMAP OutPtr[NUM_FUNC_OUT + 1];
//#define printf
//#define pprintf

//Users with random IDs,
//TODO: support for adding users and modifying their rights is not there
Users USERS[] = { { "32923646", Branson }, { "287646", Manufacturing1 }, {
      "26273", Manufacturing2 }, };

//Digital input function table to be shown on IOConfig page for function selection
//put 0 in high ID if function is only active low.put 0 in low ID if function is only active high
WebDigitalIOFunctionTable webDigIOfuncTableIn[] = { { 0, 0, 0 }, { 100,
      EXTRESET_ACTIVEHIGH, EXTRESET_ACTIVELOW },   //ExtReset
      { 101, EXTSEEK_ACTIVEHIGH, EXTSEEK_ACTIVELOW, },   //External Seek
      { 102, EXTSTART_ACTIVEHIGH, EXTSTART_ACTIVELOW },   //ExtStart
      { 103, EXTTEST_ACTIVEHIGH, EXTTEST_ACTIVELOW },   //ExtTest
      { 104, MEMCLEAR_ACTIVEHIGH, MEMCLEAR_ACTIVELOW },   //memClear
      { 105, STARTHORNSCAN_ACTIVEHIGH, STARTHORNSCAN_ACTIVELOW },   //ExtScan
      { 106, LOCKOUT_ACTIVEHIGH, LOCKOUT_ACTIVELOW },   //LockOutChangeLCD
      { 107, CABLEDETECT_ACTIVEHIGH, 0 },   //Input signal to prevent accidental sonics
      };

//Digital output function table to be shown on IOConfig page for function selection
//put 0 in high ID if function is only active low.put 0 in low ID if function is only active high
WebDigitalIOFunctionTable webDigIOfuncTableOut[] = { { 0, 0, 0 }, { 200,
      READY_ACTIVEHIGH, READY_ACTIVELOW },   //ReadyFlg
      { 201, SONICACTIVE_ACTIVEHIGH, SONICACTIVE_ACTIVELOW },   //WeldOn
      { 202, GENALARM_ACTIVEHIGH, GENALARM_ACTIVELOW },   //GenAlarm
      { 203, OVERLOADALARM_ACTIVEHIGH, OVERLOADALARM_ACTIVELOW },   //Overload alarm
      { 204, SEEKOUT_ACTIVEHIGH, SEEKOUT_ACTIVELOW },   //CycleOK
      { 205, PPEAKPOWERLIMIT_ACTIVEHIGH, PPEAKPOWERLIMIT_ACTIVELOW }, { 206,
            MPEAKPOWERLIMIT_ACTIVEHIGH, MPEAKPOWERLIMIT_ACTIVELOW } };

//Analog input function table to be shown on IOConfig page for function selection
WebAnalogIOFunctionTable webAnaIOfuncTableIn[] = { { 0, 0, 0 }, { 300,
      FREQ_CONTROL, 0 },   //freqin
      { 301, AMPL_CONTROL, 0 },   //amplitudein
      { 302, ENERGY_CONTROL_RANGE1, 0 },   //Energyin range 10
      { 303, ENERGY_CONTROL_RANGE2, 0 },   //Energyin range 30
      { 304, ENERGY_CONTROL_RANGE3, 0 },   //Energyin range 250
      {	305, ENERGY_CONTROL_RANGE4, 0 },   //Energyin range 1K
      { 306, ENERGY_CONTROL_RANGE5, 0 }	   //Energyin range 10K
      };

//Analog output function table to be shown on IOConfig page for function selection
WebAnalogIOFunctionTable webAnaIOfuncTableOut[] = { { 0, 0, 0 }, { 400,
      POWER_OUT, 0 },   //powerout
      { 401, AMPL_OUT, 0 },   //amplitude out
      { 403, FREQ_OUT, 0 },   //frequencyOut
      { 404, ENERGY_OUT, 0 },   //EnergyOut
      { 405, POWER_OUT400W, 400 }, { 406, POWER_OUT750W, 750 },   //406:S,Power Out-750 Watts
      { 407, POWER_OUT800W, 800 },   //407:S,Power Out-800 Watts
      { 408, POWER_OUT1100W, 1100 },   //408:S,Power Out-1100 Watts
      { 409, POWER_OUT1250W, 1250 },   //409:S,Power Out-1250 Watts
      { 410, POWER_OUT1500W, 1500 },   //410:S,Power Out-1500 Watts
      { 411, POWER_OUT1600W, 1600 },   //411:S,Power Out-1600 Watts
      { 412, POWER_OUT2200W, 2200 },   //412:S,Power Out-2200 Watts
      { 413, POWER_OUT2400W, 2400 },   //413:S,Power Out-2400 Watts
      { 414, POWER_OUT2500W, 2500 },   //414:S,Power Out-2500 Watts
      { 415, POWER_OUT3000W, 3000 },   //415:S,Power Out-3000 Watts
      { 416, POWER_OUT3300W, 3300 },   //416:S,Power Out-3300 Watts
      { 417, POWER_OUT4000W, 4000 },   //417:S,Power Out-4000 Watts
      };

/*
 * Website command handlers array
 */
webSiteCommand webCommandHandlers[] = { { 0 }, { WebSite::GetIPSetupVal },   //1
      { WebSite::SetLanguageAndUserID },   //2
      { WebSite::GetIOConfigurationVal },   //3
      { WebSite::GetModifyPrestVal },   //4
      { WebSite::GetModifyPrestAdvancedVal },   //5
      { WebSite::GetDiagnosticDataVal },   //6
      { WebSite::GetSystemInformationVal },   //7
      { WebSite::UpdateIPSetup },   //8
      { WebSite::UpdateIOConfiguration },   //9
      { WebSite::ProcessDiagnosticIOCommand },   //10
      { WebSite::UpdatePreset },   //11
      { WebSite::UpdatePresetAdvanced },   //12
      { WebSite::ProcessTestCommand },   //13
      { WebSite::ProcessScanCommand },   //14
      { WebSite::GetLogData },   //15
      { WebSite::ProcessResetCommand },   //16
      { WebSite::GetLastSucessfullHornScanData },   //17
      { WebSite::ProcessPSTestOnLoad },   //18
      { WebSite::ProcessManufacturingCmd },   //19
      { WebSite::ProcessManufacturingBoardTestCmd },   //20
      { WebSite::ViewWeldGraphGetDatahandeler },   //21        New Weld-Graph Handler
      { WebSite::ViewGraphHornSignatureGetDatahandeler }   //22
};
UserType ValidateUser(char * userID)
/**************************************************************************************/
/*Function validates the logged in user ID
 @param userID:  The unique ID of DCP Event.
 @return: Err user if the ID doesn't math with any in USERS array
 else return user's privilege
 */
/****************************************************************************************/
{
   BOOLEAN ExitFlag = FALSE;
   SINT32 size = sizeof(USERS) / sizeof(Users);
   UserType userType = User;
   for (SINT32 i = 0; ((i < size) && (ExitFlag == FALSE)); i++) {
      if (!strcmp(userID, USERS[i].UserID)) {
         userType = USERS[i].User;
         ExitFlag = TRUE;
      }
   }
   return userType;
}
int GetContentType(char * fileName)
/**************************************************************************************/
/*Function return the type of file requested from website
 @param fileName: The fileNam with extension whose type is to be determined
 @return: the type of file image.gtml or gif etc.
 */
/***************************************************************************************/
{
   char * ext = strchr(fileName, '.');
   char * ext1 = ext;
   while (ext) {
      ext1 = ext;
      ext = strchr(ext + 1, '.');
   }
   int ret = texthtml;
   if (!strcmp(ext1, ".css"))
      ret = textcss;
   else if (!strcmp(ext1, ".gif"))
      ret = imagegif;
   else if (!strcmp(ext1, ".jpg"))
      ret = imagejpg;
   else if (!strcmp(ext1, ".png"))
      ret = imagepng;
   else if (!strcmp(ext1, ".js")) ret = textjs;

   return ret;
}
unsigned int IPStrtoUint(char * ipStr)
/*************************************************************************************************/
/*Function converts the IP in dotted string format(e.g XX.XX.XX.XX) into integer format
 @param ipStr: The fileNam with extension whose type is to be determined
 @return: IP in integer format
 */
/*************************************************************************************************/
{
   unsigned int ip = 0;
   ip = atoi(ipStr) << 24;
   ipStr = strchr(ipStr, '.') + 1;
   ip |= atoi(ipStr) << 16;
   ipStr = strchr(ipStr, '.') + 1;
   ip |= atoi(ipStr) << 8;
   ipStr = strchr(ipStr, '.') + 1;
   ip |= atoi(ipStr);
   return htonl(ip);
}

void WebSite::InitWebSite(int intf, int srvrPort)
/******************************************************************************************/
/*
 Function should be called once at powerup. It assigns the handler
 functions for HTTP GET and POST requests coming from website(HTTP Client on PC)
 to HTTPServer. HTTPServer calls these handler functions on receiving HTTP GET or POST requests.
 @param intf: The index of Ethernet module of processor
 @param srvrPort: The HTTP port this website belongs to
 */
/******************************************************************************************/
{
   //Add the handlers below. The ethernet interface
   //index and port are used by below API’s to
   //identify the HTTPServer.
   //Add Get Handler
   CyGOS::AddHttpGetHandler(intf, srvrPort, WebGetHandler);
   //Add post handler
   CyGOS::AddHttpPostHandler(intf, srvrPort, WebPostHandler);
   //Add timer callback for website
   CyGOS::AddTickCallbackForWebSite(intf, srvrPort, Tick);
   //Add file upload handler
   CyGOS::AddHttpFileUploadHandler(intf, srvrPort, WebFileUploadHandler);
   port = srvrPort;
   interface = intf;
   scanStartFlag = FALSE;
   scanAbortFlag = FALSE;
   webtimeOutScan = WEBSCAN_TIMEOUT;
   webtimeOutTest = WEBTEST_TIMEOUT;
   languageBuff = 0;
   BoardTestFrequency = MIN_BOARD_TESTFREQUENCY;
   currNetworkConfig = CyGOS::GetNetworkConfiguration(interface);
}
void WebSite::SetLanguageFileBuffer(Language lang)
/******************************************************************************************/
/*function gets called on every POST request
 * it opens the Language file and  copy the file contents
 *  in language buffer if the buffer is not already initialized
 *  Function will be handy when we will deal with more languages
 *  @param: The language in which language buffer is to be initialized
 */
/******************************************************************************************/
{

   File * file = 0;
   int len = 0;
   if (!languageBuff) {
      CyGOS::FileOpen("A:\\ENGLISH.lan", O_RDONLY, &file);
      if (file) {
         len = CyGOS::FileLength(file);
         languageBuff = new char[len + 1];
         if (CyGOS::FileRead(file, languageBuff, &len) == 0) {
            CyGOS::FileClose(file);
         }
         languageBuff[len] = 0;
      }
   }
   lang = English;
}

BOOLEAN WebSite::WebPostHandler(char * postData, char * uri,
      HTTPReplyHeader & header)
/*****************************************************************************************/
/*This function is called by HTTPServer task on receiving POST request
 @param postData : The data submitted by a web form in a predefined
 standard format.
 @param uri : Identifier of resource.  It can be an HTML file or some
 other data specific to the page on website.
 @header: Object which will be filled with response length, response type etc.
 @return: returns true in case of valid request. otherwise returns false
 */
/*****************************************************************************************/
{
   //pprintf("\n uri POST = %s", uri);
   BOOLEAN retVal = TRUE;
   BOOLEAN htmlRequest = (strstr(uri, ".html") != 0);   //see if its html a file request
   BOOLEAN downLoadRequest = (strstr(uri, ".logdownload") != 0);	//see if its a download request
   BOOLEAN uploadRequest = (strstr(uri, ".dcxfileupload") != 0);	//see if its a upload request
   char * tempStr = 0, *res = resDefault;
   int langID = 0;
   char fileName[50];
   File * file = 0;
   UserType userType;
   char * UID = 0;
   int content_length = 0;
   header.contentLength = strlen(res);
   header.reqType = reqData;
   fileName[0] = 0;
   if (downLoadRequest)   //see if its a download request
   {
      res = DownLoadRequest(uri, header.downloadFileName);
      content_length = strlen(res);
      header.reqType = reqFileDownload;
   }
   else if (uploadRequest) {
      pprintf("\n requested file upload");
      res = HandleInitiateUpload();
      content_length = strlen(res);
      header.reqType = reqFileUpload;
   }
   else if (htmlRequest) {
      header.reqType = reqFile;
      tempStr = strstr(postData, "lang=");
      langID = atoi(strchr(tempStr, '=') + 1);
      SetLanguageFileBuffer(Language(langID));
      tempStr = strstr(postData, "&userid1=");
      tempStr = strchr(tempStr, '=') + 1;
      userType = ValidateUser(tempStr);
      UID = tempStr;
      DcxWebUserID = atoi(UID);
      if (userType == ErrUser)
         retVal = FALSE;
      else {
         strcpy(fileName, "A:\\");
         if (strlen(uri) > MAXFILELEN) {
            Log::WriteEventLog(POST_FREQ, strlen(uri), 0, 0, false);
            pprintf("\n Post File Name Too Long\n");
            pprintf("\n fileName = %s", uri);
            return FALSE;
         }
         strcat(fileName, uri);
         CyGOS::FileOpen(fileName, O_RDONLY, &file);
         retVal = FALSE;
         if (file) {
            retVal = TRUE;
            content_length = CyGOS::FileLength(file);
         }
      }
   }
   else	//if its not a file or download request, its a request for some data
         //web page doesnt get refreshed in this case
   {
      header.reqType = reqData;
      tempStr = strstr(uri, "&lang=");
      langID = atoi(strchr(tempStr, '=') + 1);
      SetLanguageFileBuffer(Language(langID));
      tempStr = strstr(uri, "&userid1=");
      tempStr = strchr(tempStr, '=') + 1;
      userType = ValidateUser(tempStr);
      DcxWebUserID = atoi(tempStr);
      if (userType == ErrUser)
         retVal = FALSE;
      else {
         if ((tempStr = strstr(uri, "&lang="))) *tempStr = 0;

         res = PostResponse(uri, userType);
         content_length = strlen(res);
      }
   }
   header.file = file;
   header.contentLength = content_length;
   header.data = res;

   return retVal;

}
BOOLEAN WebSite::WebFileUploadHandler(char * data, int dataLen, char * fileName,
      char* Action)
/***********************************************************************************************/
/*This function handles the file upload request.
 @param data : pointer to file data.
 @param dataLen: The length of file data
 @param fileName: The file name
 @return: returns true if file is successfully send
 to Upload Firmware task.
 */
/***********************************************************************************************/
{
   char * ext, *ext1;
   ext = strchr(fileName, '.');
   ext1 = ext;
   while (ext) {
      ext1 = ext;
      ext = strchr(ext + 1, '.');
   }
   FileType type =
         (!strcmp(ext1, ".bin")) ?
               fileTypeBin :
               ((!strcmp(ext1, ".cyg")) ? fileTypeCyg : fileTypeErr);
   char * buff = new char[dataLen];
   memcpy(buff, data, dataLen);
   return UploadFirmware::thisPtr->ProgramFirmware(type, buff, dataLen, Action);

}
void FormatCycleResults(void)
/****************************************************************************************/
/*This function gets called from DownLoadRequest() function on "save results to file"
 * button from Manufacturing Webpage.It converts the cycle time parametes which are in
 * milliseconds  to hours and minutes format. It also prepare the response according to
 * CSV file format.
 */
/****************************************************************************************/
{
   int temp, CycTimeHrs, CycTimeMins, sonicOnTimeHrs, sonicOnTimeMins;
   char * date, *time;
   char dateTime[MAX_TIMESTAMP_LENGTH];
   LogTime tm;
   temp = ReadPowerOnSecCounter();		//SecondsSincePowerUp();
   CalculateTimeSincePowerUp(temp, &tm);
   sprintf(dateTime, "%02d-%02d-%04d %02d-%02d-%02d", tm.tm_mon, tm.tm_day,
         tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec);
   date = dateTime;
   time = strchr(dateTime, ' ') + 1;
   time[-1] = 0;

   temp = SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime);
   CycTimeHrs = temp / NMSINHOUR;
   CycTimeMins = temp % NMSINHOUR;
   CycTimeMins = CycTimeMins / NMSINMINUTE;

   temp = SCALEDTIMETOWEBSITE(MFcycleResults.sonicsOnTime);
   sonicOnTimeHrs = temp / NMSINHOUR;
   sonicOnTimeMins = temp % NMSINHOUR;
   sonicOnTimeMins = sonicOnTimeMins / NMSINMINUTE;

   sprintf(Response,
         "Date,Time,Serial Number,Cycle Time,Cycles Run,Hours Run,Alarms,Frequency,Power\r\n"
               "%s,%s,%s,%2d:%2d,%d,%2d:%2d,%d,%d,%d", date, time,
         EPromData.SerialNo, CycTimeHrs, CycTimeMins,
         (int) MFcycleResults.nCycles, (int) sonicOnTimeHrs,
         (int) sonicOnTimeMins, (int) ReadManFErrorCounter(),
         (int) ((MFcycleResults.resultantFrequency * F_Factor) / 1000),
         (int) ((MFcycleResults.resultantPower * 100) / ADC11BITVALUE));

}

char * WebSite::HandleInitiateUpload()
{
   //check if readu for upload
   Response[0] = 0;
   strcpy(Response, "READY");
   return Response;
}
char * WebSite::DownLoadRequest(char * uri, char * file)
/********************************************************************************************/
/*This function handles the file download request.
 @param uri : The data submitted by a web form in a predefined
 standard format.It used to determine the file to be downloaded
 @param file: The file name specific to request is filled in this pointer
 @return: returns the file data to be sent
 */
/********************************************************************************************/
{
   char * tempStr;
   Response[0] = 0;
   WebCommand cmd;
   tempStr = strstr(uri, "cmd=");
   tempStr = strchr(tempStr, '=') + 1;
   cmd = (WebCommand) (atoi(tempStr));
   if (cmd == reqDownloadAlarmData) {
      GetLogData(Response, cmd);
      strcpy(file, "alarmLog.csv");
   }
   else if (cmd == reqDownloadEventdata) {
      GetLogData(Response, cmd);
      strcpy(file, "eventLog.csv");
   }
   else if (cmd == reqDownloadCycleResults) {
      FormatCycleResults();
      strcpy(file, "cycleResults.csv");
   }
   else {
      strcpy(Response, "err,err,err,err\r\nerr,err,err,err\r\n");
      strcpy(file, "error.csv");
   }

   return Response;
}
char* WebSite::PostResponse(char* uri, UserType usr)
/**********************************************************************************************/
/*This function is Called by Posthandler. It finds out the
 * appropriate handler for post request based on function ID
 * and command received in uri.
 * @param uri: Identifier of resource.  It can be an HTML file or some
 other data specific to the page on website.
 * @ param usr: user privilege to be filled in response
 * @return: The response data
 * */
/**********************************************************************************************/
{
   char * tempStr;
   int funcID, webCmd;
   Response[0] = 0;
   UpdateUser(usr);
   tempStr = strstr(uri, "func=");
   tempStr = strchr(tempStr, '=') + 1;
   funcID = atoi(tempStr);
   tempStr = strstr(uri, "cmd=");
   tempStr = strchr(tempStr, '=') + 1;
   webCmd = atoi(tempStr);
   tempStr = strstr(uri, "param=");
   tempStr = strchr(tempStr, '=') + 1;

   // pprintf("\n Functions = %d", sizeof(webCommandHandlers)/sizeof(webCommandHandlers[0]));
   if (((unsigned int) funcID > 0)
         && ((unsigned int) funcID
               < (sizeof(webCommandHandlers) / sizeof(webCommandHandlers[0])))) {
      if (webCommandHandlers[funcID].handler)
         webCommandHandlers[funcID].handler(tempStr, (WebCommand) webCmd);
   }
   else
      pprintf("func ID %d", funcID);

   return Response;
}

BOOLEAN WebSite::WebGetHandler(char * uri, HTTPReplyHeader & header)
/*************************************************************************************/
/*This function is called by HTTPServer task on receiving GET request
 @header: Object which will be filled with the response length, response type etc.
 @param uri: Identifier of resource.  It can be an HTML file or some other
 data specific to the page on website. It may also contain parameter IDs
 to be received.
 @return: returns true in case of valid request. otherwise returns false
 */
/*************************************************************************************/
{
   BOOLEAN retVal = FALSE;
   char fileName[50];
   File * file = 0;
   BOOLEAN DefaultFileRequest = FALSE;
   strcpy(fileName, "A:\\");
   if (strlen(uri + 1) > MAXFILELEN) {
      Log::WriteEventLog(GET_FREQ, strlen(uri + 1), 0, 0, false);
      pprintf("\n Get File Name Too Long\n");
      return FALSE;
   }
   strcat(fileName, uri + 1);
   if (*(uri + 1)) {   //Indicates there is a request from some specific html file
      if (strstr(fileName, "UserComment.html")) {
      }
      else if (strstr(fileName, "FirmwareUpload32923646.html"))
         DefaultFileRequest = TRUE;
      else if (strstr(fileName, ".html")) strcpy(fileName, "A:\\index.html");
   }		//no file name means the home page request
   else
      strcpy(fileName, "A:\\index.html");
   if (strcmp(header.cacheReq, MODIFIED_SINCE_STR) == 0) {
      header.statusCode = httpNotModified;
      retVal = TRUE;
   }
   else {
      CyGOS::FileOpen(fileName, O_RDONLY, &file);
      int len = 0;
      if (file) {
         len = CyGOS::FileLength(file);
         retVal = TRUE;
      }
      else if (DefaultFileRequest == TRUE) {
         len = strlen(FirmwareUpload);
         retVal = TRUE;
         header.reqType = reqData;
         header.data = (SINT8*) FirmwareUpload;
         pprintf("\nDefalut.html\n");
      }
      if (strstr(fileName, "index.html"))   //TODO:Error A;//?? reset user ID on home page
            {
         DcxWebUserID = 0;
      }

      header.file = file;
      header.contentLength = len;
      header.statusCode = httpOk;
      header.contentType = GetContentType(fileName);
   }
   return retVal;
}
void WebSite::UpdateUser(UserType usr)
/*************************************************************************************/
/*This function set the user privilege before any response data
 * base on this privilege javascript code on webpage decides
 * that which tabs need to be shown, or any other
 * things which is specific to branson user
 * @ param usr: user privilege to be filled in response
 */
/************************************************************************************/
{
   sprintf(Response, "%d/%d@", (UINT8) (EPromData.Systemtype + 1), usr);
}

void WebSite::GetIPSetupVal(char * paramStr, WebCommand cmd)
/**********************************************************************************/
/*This function fills data specific to IPSetup page
 * @param paramStr: pointer to parameter array
 * #param cmd: Command sent by jscript in uri.
 * 	        parameter is ignored by this function
 */
/**********************************************************************************/
{
   char temp[18];
   int networkConfigJS = currNetworkConfig;	//network configuration to be sent to javascript code on website
   //1= dhcp client 2= dhcp server or static configuration selected
   strcat(Response, "IPAddress:D,");
   strcat(Response, CyGOS::GetIPAddressStr(interface));
   strcat(Response, "@SubnetMask:D,");
   strcat(Response, CyGOS::GetSubnetMaskStr(interface));
   strcat(Response, "@GateWay:D,");

   if (currNetworkConfig == configDhcpClient)
      strcat(Response, CyGOS::GetGatewayAddressStr(interface));
   else if (currNetworkConfig == configDhcpServer) {
      networkConfigJS = configStatic;
      strcat(Response, sysConfig.dhcpConfig.gateway.ToString());
   }
   else if (currNetworkConfig == configStatic)
      strcat(Response, sysConfig.gw.ToString());

   strcat(Response, "@ipconfig:RB,");
   sprintf(temp, "%d", networkConfigJS);
   strcat(Response, temp);
   strcat(Response, "@dhcpserver:CB,");
   sprintf(temp, "%d", (currNetworkConfig == configDhcpServer));
   strcat(Response, temp);
   unsigned char * MacAddr = CyGOS::GetMacAddress(interface);
   sprintf(temp, "%02X-%02X-%02X-%02X-%02X-%02X", MacAddr[0], MacAddr[1],
         MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
   strcat(Response, "@MAC:S,");
   strcat(Response, temp);
   cmd = reqNone;
   paramStr = 0;
}

UINT16 GetNumberofAnalogOut()
{
   UINT16 nFunctions = sizeof(webAnaIOfuncTableOut)
         / sizeof(WebAnalogIOFunctionTable);
   UINT16 OutFunctions = 1;
   for (UINT16 i = 1; i < nFunctions; i++) {
      if (EPromData.PSpower >= (UINT32) webAnaIOfuncTableOut[i].Power)
         OutFunctions++;
   }
   return OutFunctions;

}
void WebSite::GetIOConfigurationVal(char * paramStr, WebCommand cmd)
/**********************************************************************************************/
/*This function fills data specific to IO configuration page in a format whic javascript
 * code on webpage is expecting
 * @param paramStr: ponter to parameters array
 * @param cmd:cmd sent in uri by jscript code
 */
/**********************************************************************************************/
{
   UINT8 indx;
   UINT16 nFunctions;
   UINT16 FunctionNotReq = 0;
   if (cmd == reqRestore) {
      DcxLogUserID = DcxWebUserID;
      UserIO::ConfigureDefaultUserIOs();
      DcxLogUserID = 0;
   }
   if (BIT(EPromData.Systemtype) == RACKMOUNT_S) FunctionNotReq =
         MINUS_PLUS_PEAKPOWER;
   char * tempStr = Response + strlen(Response);
   strcat(tempStr, "dd_pin1,dd_pin2,dd_pin3,dd_pin4:DD,");
   nFunctions = sizeof(webDigIOfuncTableIn) / sizeof(WebDigitalIOFunctionTable);
   for (UINT16 i = 1; i < nFunctions; i++) {
      strcat(tempStr,
            FindLanguageString(webDigIOfuncTableIn[i].stringID, languageBuff,
            TRUE));
      sprintf(&tempStr[strlen(tempStr)], "#%d#%d",
            webDigIOfuncTableIn[i].ActivehighID,
            webDigIOfuncTableIn[i].ActivelowID);

      if (i < (nFunctions - 1)) strcat(tempStr, ",");

   }

   strcat(tempStr, "@dd_pin7,dd_pin8,dd_pin9,dd_pin10:DD,");
   nFunctions = (sizeof(webDigIOfuncTableOut)
         - (FunctionNotReq * (sizeof(WebDigitalIOFunctionTable))))
         / sizeof(WebDigitalIOFunctionTable);
   printf("\r\n nFunctions %d Sys %d", nFunctions, EPromData.Systemtype);
   for (UINT16 i = 1; i < nFunctions; i++) {
      strcat(tempStr,
            FindLanguageString(webDigIOfuncTableOut[i].stringID, languageBuff,
            TRUE));
      sprintf(&tempStr[strlen(tempStr)], "#%d#%d",
            webDigIOfuncTableOut[i].ActivehighID,
            webDigIOfuncTableOut[i].ActivelowID);
      if (i < (nFunctions - 1)) strcat(tempStr, ",");

   }

   strcat(tempStr, "@dd_pin17,dd_pin18:DD,");
   if (EPromData.EnergyPowerSpecial == TRUE) {
      nFunctions = sizeof(webAnaIOfuncTableIn)
            / sizeof(WebAnalogIOFunctionTable);
   }
   else
      nFunctions = NUM_AIN;

   for (UINT16 i = 1; i < nFunctions; i++) {
      strcat(tempStr,
            FindLanguageString(webAnaIOfuncTableIn[i].stringID, languageBuff,
            TRUE));
      sprintf(&tempStr[strlen(tempStr)], "#%d",
            webAnaIOfuncTableIn[i].functionID);
      if (i < (nFunctions - 1)) strcat(tempStr, ",");

   }
   strcat(tempStr, "@dd_pin24,dd_pin25:DD,");
   if (EPromData.EnergyPowerSpecial == TRUE) {
      nFunctions = GetNumberofAnalogOut();
   }
   else
      nFunctions = NUM_AOUT + 1;

   for (UINT16 i = 1; i < nFunctions; i++) {
      strcat(tempStr,
            FindLanguageString(webAnaIOfuncTableOut[i].stringID, languageBuff,
            TRUE));
      sprintf(&tempStr[strlen(tempStr)], "#%d",
            webAnaIOfuncTableOut[i].functionID);
      if (i < (nFunctions - 1)) strcat(tempStr, ",");
   }

   //get current functions after function list seperated by @
   strcat(tempStr, "@");
   sprintf(&tempStr[strlen(tempStr)], "SystemType:Sys,%d@",
         (int) EPromData.Systemtype);
   char * formatStrDigIn[] = { 0, "dd_pin1:DDV,%d,%d@", "dd_pin2:DDV,%d,%d@",
         "dd_pin3:DDV,%d,%d@", "dd_pin4:DDV,%d,%d@" };
   char * formatStrDigOut[] = { 0, "dd_pin7:DDV,%d,%d@", "dd_pin8:DDV,%d,%d@",
         "dd_pin9:DDV,%d,%d@", "dd_pin10:DDV,%d,%d@", "dd_pin19:DDV,%d,%d@" };
   char * formatStrAIn[] = { 0, "dd_pin17:DDV,%d,%d@", "dd_pin18:DDV,%d,%d@" };
   char * formatStrAOut[] = { 0, "dd_pin24:DDV,%d,%d@", "dd_pin25:DDV,%d,%d@" };
   for (indx = 1; indx < NUM_DIN; indx++) {
      sprintf(&tempStr[strlen(tempStr)], formatStrDigIn[indx],
            USERIOConfig.DigInUse[indx], USERIOConfig.DigInUseStatus[indx]);
   }
   for (indx = 1; indx < NUM_DOUT; indx++) {
      sprintf(&tempStr[strlen(tempStr)], formatStrDigOut[indx],
            USERIOConfig.DigOutUse[indx], USERIOConfig.DigOutUseStatus[indx]);
   }
   for (indx = 1; indx < NUM_AIN; indx++) {
      sprintf(&tempStr[strlen(tempStr)], formatStrAIn[indx],
            USERIOConfig.AnalogInUse[indx],
            USERIOConfig.AnalogInUseStatus[indx]);
   }
   for (indx = 1; indx < NUM_AOUT; indx++) {
      sprintf(&tempStr[strlen(tempStr)], formatStrAOut[indx],
            USERIOConfig.AnalogOutUse[indx],
            USERIOConfig.AnalogOutUseStatus[indx]);
   }
   paramStr = 0;

}
void WebSite::GetModifyPrestVal(char * paramStr, WebCommand cmd)
/******************************************************************************************/
/*This function fills data specific to  weld-preset page in a format which javascript
 * code on this page is expecting
 * @param paramStr: pointer to parameters array
 * @param cmd: cmd sent by javascript in uri
 */
/******************************************************************************************/
{
   if (cmd == reqRestore) {
      SetDefaultWeldPreset();
      Log::WriteEventLog(EVENT_RESTORE_DEFAULTS,  1, 0, DcxWebUserID, TRUE);
   }
   sprintf(&Response[strlen(Response)], "weldAmp:D,%d,%d,%d@"
         "seekRamp:D,%d,%d,%d@"
         "startRamp:D,%d,%d,%d@"
         "seekTime:D,%d,%d,%d@"
         "SeekFreqOff:D,%d,%d,%d@"
         "FreqOff:D,%d,%d,%d@"
         "endofweldstore:CB,%d@"
         "externalamplitude:CB,%d@"
         "externalfrequency:CB,%d@"
         "powerup:RB,%d@"
         "timedSeek:CB,%d@"
         "stoponalarm:CB,%d@"
         "digitaltune:D,%d,%d,%d@"
         "JP2:RB,%d@"
         "SystemType:Sys,%d@"
         "WinLimitLow:D,%d@"
         "WinLimitHigh:D,%d@"
         "ClrMemReset:CB,%d@"
         "peakpwron:RB,%d@"
         "PCheckbox:CB,%d@"
         "MCheckbox:CB,%d@"
         "Weldmode:RB,%d@"
         "WeldTime:D,%d,%d,%d@"
         "Energy:D,%d,%d,%d@"
         "ClrMemBfrSeek:CB,%d@"
   		"SetDigTuneWithScan:CB,%d@"
   		"ClrMemAtPwrUp:CB,%d@"
   		"Handheld:CB,%d",
         (int) CurrentPreset.WeldAmplitude,
         (int) Preset_Min.WeldAmplitude, (int) Preset_Max.WeldAmplitude,
         (int) (CONVMICRO_MILLI(CurrentPreset.SeekRampTime)),
         (int) (CONVMICRO_MILLI(Preset_Min.SeekRampTime)),
         (int) (CONVMICRO_MILLI(Preset_Max.SeekRampTime)),
         (int) (CONVMICRO_MILLI(CurrentPreset.WeldRampTime)),
         (int) (CONVMICRO_MILLI(Preset_Min.WeldRampTime)),
         (int) (CONVMICRO_MILLI(Preset_Max.WeldRampTime)),
         (int) (CONVMICRO_MILLI(CurrentPreset.SeekTime)),
         (int) (CONVMICRO_MILLI(Preset_Min.SeekTime)),
         (int) (CONVMICRO_MILLI(Preset_Max.SeekTime)),
         (int) CurrentPreset.SeekFreqOffset, (int) Preset_Min.SeekFreqOffset,
         (int) Preset_Max.SeekFreqOffset, (short) CurrentPreset.FreqOffset,
         (int) Preset_Min.FreqOffset, (int) Preset_Max.FreqOffset,
         CurrentPreset.SWDip.Flags.EndOfWeldStore,
         CurrentPreset.SWDip.Flags.ExtAmplitude,
         CurrentPreset.SWDip.Flags.ExtFrequency,
         CurrentPreset.SWDip.Flags.PowerUpOptions,
         CurrentPreset.SWDip.Flags.TimedSeek, CurrentPreset.AlarmResetReq,
         (int) (CurrentPreset.DigitalTune * F_Factor) / 1000,
         (int) (Preset_Min.DigitalTune * F_Factor) / 1000,
         (int) (Preset_Max.DigitalTune * F_Factor) / 1000,
         CurrentPreset.Jumper2, (int) EPromData.Systemtype,
         CurrentPreset.PowerWindowLimitLow, CurrentPreset.PowerWindowLimitHigh,
         CurrentPreset.SWDip.Flags.ClearMemAtRst,
         CurrentPreset.SWDip.Flags.PowerLimitOn,
         CurrentPreset.SWDip.Flags.PlusLimitCheck,
         CurrentPreset.SWDip.Flags.MinusLimitCheck, CurrentPreset.WeldMode,
         CurrentPreset.WeldTime, Preset_Min.WeldTime, Preset_Max.WeldTime,
         CurrentPreset.EnergyCP, Preset_Min.EnergyCP, Preset_Max.EnergyCP,
         CurrentPreset.SWDip.Flags.ClrMemBfrSeek,
         CurrentPreset.SWDip.Flags.SetDigTuneWithScan,
         CurrentPreset.SWDip.Flags.ClrMemAtPwrUp,
         CurrentPreset.SWDip.Flags.HandHeldMode);
   paramStr = 0;
}

void WebSite::GetModifyPrestAdvancedVal(char * paramStr, WebCommand cmd)
/**********************************************************************************************/
/*This function fills data specific to Advanced preset page in a format which javascript
 * code on this page is expecting
 * @param paramStr: pointer to parameters array
 * @param cmd: cmd sent by jscript in uri
 */
/**********************************************************************************************/
{
   SINT32 freqStep = 0, freqStepMin = 0;
   if (cmd == reqRestore) {
      SetDefaultAdvancedPreset();
   }

   freqStep = ParaScan.FrequencyStep / SCANFREQFACTOR;
   freqStepMin = ScanParamMin.FrequencyStep / SCANFREQFACTOR;

   sprintf(&Response[strlen(Response)], "AmplitudeloopC1:D,%d,%d,%d@"
         "SeekAmplitudeloopC1:D,%d,%d,%d@"
         "Amplitude:D,%d,%d,%d@"

         "AmplitudeloopC2:D,%d,%d,%d@"
         "SeekAmplitudeloopC2:D,%d,%d,%d@"
         "Current:D,%d,%d,%d@"

         "Phaseloop:D,%d,%d,%d@"
         "SeekPhaseloop:D,%d,%d,%d@"
         "FreqStart:D,%d,%d,%d@"

         "PhaseloopCF:D,%d,%d,%d@"
         "SeekPhaseloopCF:D,%d,%d,%d@"
         "FreqEnd:D,%d,%d,%d@"

         "FreqLow:D,%d,%d,%d@"
         "SeekFreqLow:D,%d,%d,%d@"
         "StepDelay:D,%d,%d,%d@"

         "FreqHigh:D,%d,%d,%d@"
         "SeekFreqHigh:D,%d,%d,%d@"

         "PhaseLimitTime:D,%d,%d,%d@"
         "SeekAmp:D,%d,%d,%d@"

         "PhaseLimit:D,%d,%d,%d@"/**/

         "AmplitudeloopC1New:D,%d,%d,%d@"
         "AmplitudeloopC2New:D,%d,%d,%d@"
         "PhaseloopNew:D,%d,%d,%d@"
         "PhaseloopPropNew:D,%d,%d,%d@"
         "FreqWindowSize:D,%d,%d,%d@"
         "PhaseloopCFNew:D,%d,%d,%d@"
         "FreqLowNew:D,%d,%d,%d@"
         "FreqHighNew:D,%d,%d,%d@"
         "PhaseLimitTimeNew:D,%d,%d,%d@"
         "PhaseLimitNew:D,%d,%d,%d@"
         "UseTrueValues:CB,%d@",

   (int) CurrentPreset.WeldAmplitudeLoopC1,
         (int) Preset_Min.WeldAmplitudeLoopC1,
         (int) Preset_Max.WeldAmplitudeLoopC1,
         (int) CurrentPreset.SeekAmplitudeLoopC1,
         (int) Preset_Min.SeekAmplitudeLoopC1,
         (int) Preset_Max.SeekAmplitudeLoopC1, (int) ParaScan.MaxAmplitude,
         (int) ScanParamMin.MaxAmplitude, (int) ScanParamMax.MaxAmplitude,

         (int) CurrentPreset.WeldAmplitudeLoopC2,
         (int) Preset_Min.WeldAmplitudeLoopC2,
         (int) Preset_Max.WeldAmplitudeLoopC2,
         (int) CurrentPreset.SeekAmplitudeLoopC2,
         (int) Preset_Min.SeekAmplitudeLoopC2,
         (int) Preset_Max.SeekAmplitudeLoopC2, (int) ParaScan.MaxCurrent,
         (int) ScanParamMin.MaxCurrent, (int) ScanParamMax.MaxCurrent,

         (int) CurrentPreset.WeldPhaseloopI, (int) Preset_Min.WeldPhaseloopI,
         (int) Preset_Max.WeldPhaseloopI, (int) CurrentPreset.SeekPhaseloopI,
         (int) Preset_Min.SeekPhaseloopI, (int) Preset_Max.SeekPhaseloopI,
         (int) (ParaScan.FrequencyStart) / SCANFREQFACTOR,
         (int) ((ScanParamMin.FrequencyStart) / SCANFREQFACTOR),
         (int) ((ScanParamMax.FrequencyStart) / SCANFREQFACTOR),

         (int) CurrentPreset.WeldPhaseloopCF, (int) Preset_Min.WeldPhaseloopCF,
         (int) Preset_Max.WeldPhaseloopCF, (int) CurrentPreset.SeekPhaseloopCF,
         (int) Preset_Min.SeekPhaseloopCF, (int) Preset_Max.SeekPhaseloopCF,
         (int) ((ParaScan.FrequencyStop) / SCANFREQFACTOR),
         (int) ((ScanParamMin.FrequencyStop) / SCANFREQFACTOR),
         (int) ((ScanParamMax.FrequencyStop) / SCANFREQFACTOR),

         (int) ((CurrentPreset.WeldFrequencyLimitLow * F_Factor) / 1000),
         (int) ((Preset_Min.WeldFrequencyLimitLow * F_Factor) / 1000),
         (int) ((Preset_Max.WeldFrequencyLimitLow * F_Factor) / 1000),
         (int) ((CurrentPreset.SeekFrequLimitLow * F_Factor) / 1000),
         (int) ((Preset_Min.SeekFrequLimitLow * F_Factor) / 1000),
         (int) ((Preset_Max.SeekFrequLimitLow * F_Factor) / 1000),
         (int) (CONVMICRO_MILLI(ParaScan.TimeDelay)),
         (int) (CONVMICRO_MILLI(ScanParamMin.TimeDelay)),
         (int) (CONVMICRO_MILLI(ScanParamMax.TimeDelay)),

         (int) ((CurrentPreset.WeldFrequencyLimitHigh * F_Factor) / 1000),
         (int) ((Preset_Min.WeldFrequencyLimitHigh * F_Factor) / 1000),
         (int) ((Preset_Max.WeldFrequencyLimitHigh * F_Factor) / 1000),
         (int) ((CurrentPreset.SeekFrequLimitHigh * F_Factor) / 1000),
         (int) ((Preset_Min.SeekFrequLimitHigh * F_Factor) / 1000),
         (int) ((Preset_Max.SeekFrequLimitHigh * F_Factor) / 1000),

         (int) (CONVMICRO_MILLI(CurrentPreset.WeldPhaseLimitTime)),
         (int) (CONVMICRO_MILLI(Preset_Min.WeldPhaseLimitTime)),
         (int) (CONVMICRO_MILLI(Preset_Max.WeldPhaseLimitTime)),
         (int) CurrentPreset.SeekAmplitude, (int) Preset_Min.SeekAmplitude,
         (int) Preset_Max.SeekAmplitude,

         (int) CurrentPreset.WeldPhaseLimit, (int) Preset_Min.WeldPhaseLimit,
         (int) Preset_Max.WeldPhaseLimit,/**/

         // new values
         (int) CurrentPreset.WeldAmplitudeLoopC1New,
         (int) Preset_Min.WeldAmplitudeLoopC1New,
         (int) Preset_Max.WeldAmplitudeLoopC1New,
         (int) CurrentPreset.WeldAmplitudeLoopC2New,
         (int) Preset_Min.WeldAmplitudeLoopC2New,
         (int) Preset_Max.WeldAmplitudeLoopC2New,
         (int) CurrentPreset.WeldPhaseloopINew,
         (int) Preset_Min.WeldPhaseloopINew, (int) Preset_Max.WeldPhaseloopINew,
         (int) CurrentPreset.WeldPhaseloopPNew,
         (int) Preset_Min.WeldPhaseloopPNew, (int) Preset_Max.WeldPhaseloopPNew,
         (int) CurrentPreset.FreqWindowSize, (int) Preset_Min.FreqWindowSize,
         (int) Preset_Max.FreqWindowSize,
         (int) CurrentPreset.WeldPhaseloopCFNew,
         (int) Preset_Min.WeldPhaseloopCFNew,
         (int) Preset_Max.WeldPhaseloopCFNew,
         (int) ((CurrentPreset.WeldFrequencyLimitLowNew * F_Factor) / 1000),
         (int) ((Preset_Min.WeldFrequencyLimitLowNew * F_Factor) / 1000),
         (int) ((Preset_Max.WeldFrequencyLimitLowNew * F_Factor) / 1000),
         (int) ((CurrentPreset.WeldFrequencyLimitHighNew * F_Factor) / 1000),
         (int) ((Preset_Min.WeldFrequencyLimitHighNew * F_Factor) / 1000),
         (int) ((Preset_Max.WeldFrequencyLimitHighNew * F_Factor) / 1000),
         (int) CurrentPreset.WeldPhaseLimitTimeNew,
         (int) Preset_Min.WeldPhaseLimitTimeNew,
         (int) Preset_Max.WeldPhaseLimitTimeNew,
         (int) CurrentPreset.WeldPhaseLimitNew,
         (int) Preset_Min.WeldPhaseLimitNew, (int) Preset_Max.WeldPhaseLimitNew,
         (int) CurrentPreset.UseTrueValues);
   paramStr = 0;
}

void WebSite::GetSystemInformationVal(char * paramStr, WebCommand cmd)
/**********************************************************************************************/
/*This function fills data specific to system information page in a format which javascript
 * code on this page is expecting
 * @param paramStr: pointer to parameters array
 * @param cmd: Command sent by jscript in uri
 */
/**********************************************************************************************/
{
   //TODO: variables below are updated elsewhere
   char Crc[12];
   const char * currSys;
   const char * disp = "Monochrome LCD";

   switch (BIT(EPromData.Systemtype)) {
   case DCX_S:
      currSys = "DCX RM B";
      break;
   case RACKMOUNT_S:
      currSys = "DCX S";
      break;
   case RACKMOUNT_STD:
   	if(EPromData.SIGSpecial == TRUE)
   		currSys = "DCX 222 RM";
   	else
   		currSys = "DCX RM S";
      break;
   default:
      currSys = "Unknown";
      disp = "Unknown";
      break;
   }

   sprintf(Crc, "%08X",
         ((ReceiveCommandHandler::thisptr->LCDCodeCRCHigh << 8)
               | ReceiveCommandHandler::thisptr->LCDCodeCRCLow));
   //sprintf(Version, "%d",ReceiveCommandHandler::thisptr->LCDSWVersion);

   sprintf(&Response[strlen(Response)], "pspwr:S,%d %s"
         "@psfrq:S,%d %s"
         "@lifetimecycles:S,%d"
         "@GeneralAlarms:S,%d"
         "@hoursofsonics:S,%02d hr %02d min %02d sec"
         "@poweronhours:S,%02d days %02d hr %02d min %02d sec"
         "@currsys:S,%s"
         "@Display:S,%s"
         "@swversion:S,%s"
         "@dcxcrc:S,%08X"
         "@snumber:S,%s"
         "@syscrc:S,%s"
         "@sysversion:S,%d.%d"
         "@SpecPwd:S,%s"
         "@SpecPwdEnable:DP,%d"
         "@fpgaversion:S,%s", (int) EPromData.PSpower, "Watts",
         (int) EPromData.PSfrequency, "kHz", (int) ReadCycleCounter(),
         (int) ReadErrorCounter(), (int) (ReadSonicOnSecCounter() / 3600),
         (int) ((ReadSonicOnSecCounter() % 3600) / 60),
         (int) ((ReadSonicOnSecCounter() % 3600) % 60),
         (int) ((ReadPowerOnSecCounter() / 3600) / 24),
         (int) ((ReadPowerOnSecCounter() / 3600) % 24),
         (int) ((ReadPowerOnSecCounter() % 3600) / 60),
         (int) ((ReadPowerOnSecCounter() % 3600) % 60), currSys, disp,
         SWVersionLabel, (unsigned int) sysConfig.DCXCRC, EPromData.SerialNo,
         Crc, (ReceiveCommandHandler::thisptr->LCDSWVersion) / 10,
         (ReceiveCommandHandler::thisptr->LCDSWVersion) % 10,
         EPromData.EnergyPowerSpecial ?
               SPECIALPASSWORD : (EPromData.SIGSpecial ? SIG_SPECIAL : ""),
         (int) (EPromData.EnergyPowerSpecial | EPromData.SIGSpecial),
         FPGAVersionLabel);
   cmd = reqNone;
   paramStr = 0;
}
char * WebSite::GetFunctionString(int signalType, int funcID, int activeStatus,
      char * fileBuff)
/*************************************************************************************************/
/*This function finds the function string from language file
 * based on funcID
 * @param funcID: Function ID to determine language string ID for that function
 * @param signalType: the type of of function i.e. dig in/out or nalog in/out
 * @param filebuff: pointer to the content of language file
 * @return: Function string
 */
/*************************************************************************************************/
{
   int nFunctions;
   int indx;
   int langStrID = 0;
   BOOLEAN ExitFlag;
   BOOLEAN bypass = FALSE;
   char * funcStr = funcStrUnassigned;
   if ((funcID > 0) && (activeStatus == ACTIVATED)) {
      switch (signalType) {
      case sigDigIn:   //digital input
         nFunctions = sizeof(webDigIOfuncTableIn)
               / sizeof(WebDigitalIOFunctionTable);
         ExitFlag = FALSE;
         for (indx = 1; ((indx < nFunctions) && (ExitFlag == FALSE)); indx++) {
            if ((webDigIOfuncTableIn[indx].ActivehighID == funcID)
                  || (webDigIOfuncTableIn[indx].ActivelowID == funcID)) {
               langStrID = webDigIOfuncTableIn[indx].stringID;
               ExitFlag = TRUE;
            }
         }
         break;
      case sigDigOut:	//digital output
         if (((BIT(EPromData.Systemtype) == DCX_S)
               || (BIT(EPromData.Systemtype) == RACKMOUNT_STD))
               && funcID == GENALARM_ACTIVEHIGH) {
            funcStr = "O/L Inverted";
            bypass = TRUE;
         }

         else if (((BIT(EPromData.Systemtype) == DCX_S)
               || (BIT(EPromData.Systemtype) == RACKMOUNT_STD))
               && funcID == GENALARM_ACTIVELOW) {
            funcStr = "Overload";
            bypass = TRUE;
         }
         else {
            nFunctions = sizeof(webDigIOfuncTableOut)
                  / sizeof(WebDigitalIOFunctionTable);
            ExitFlag = FALSE;
            for (indx = 1; ((indx < nFunctions) && (ExitFlag == FALSE));
                  indx++) {
               if ((webDigIOfuncTableOut[indx].ActivehighID == funcID)
                     || (webDigIOfuncTableOut[indx].ActivelowID == funcID)) {
                  langStrID = webDigIOfuncTableOut[indx].stringID;
                  ExitFlag = TRUE;
               }
            }
         }
         break;
      case sigAnalogIn:   //analog input
         nFunctions = sizeof(webAnaIOfuncTableIn)
               / sizeof(WebAnalogIOFunctionTable);
         ExitFlag = FALSE;
         for (indx = 1; ((indx < nFunctions) && (ExitFlag == FALSE)); indx++) {
            if ((webAnaIOfuncTableIn[indx].functionID == funcID)) {
               langStrID = webAnaIOfuncTableIn[indx].stringID;
               ExitFlag = TRUE;
            }
         }
         break;
      case sigAnalogOut:	//analog output
         nFunctions = sizeof(webAnaIOfuncTableOut)
               / sizeof(WebAnalogIOFunctionTable);
         ExitFlag = FALSE;
         for (indx = 1; ((indx < nFunctions) && (ExitFlag == FALSE)); indx++) {
            if ((webAnaIOfuncTableOut[indx].functionID == funcID)) {
               langStrID = webAnaIOfuncTableOut[indx].stringID;
               ExitFlag = TRUE;
            }
         }
         break;
      }
      if (!bypass) funcStr = FindLanguageString(langStrID, fileBuff, TRUE);
   }
   return funcStr;
}
void WebSite::GetDiagnosticDataVal(char * paramStr, WebCommand cmd)
/**********************************************************************************************/
/*This function fills data specific to diagnostic IO page in a format which javascript
 * code on this page is expecting
 * @param cmd: cmd sent by jscript in uri
 */
/**********************************************************************************************/
{
   if ((UserIO::GetGenAlarmFlag() || WindowLimitAlarm || UserIO::GetReadyFlag())
         && !MFCycleFlag) {
      sprintf(&Response[strlen(Response)], "%s@", "READY");
      diagFlag = TRUE;
      EnableWebIO = TRUE;
      diagnosticTimeOut = WEBDIAG_TIMEOUT;
   }
   else
      sprintf(&Response[strlen(Response)], "%s@", "NOTREADY");

   PrepareDiagnosticResponse();
   cmd = reqNone;
   paramStr = 0;
}
void WebSite::PrepareDiagnosticResponse(bool inputsOnly)
/**********************************************************************************/
/*This function is called by GetDiagnosticDataVal() function during
 * the DiagnosticIO page load. This function is also called in a
 * fixed interval from DiagnosticIO page during diagnostic. It fills
 * the current Digital and Analog Input state in global Response array
 * @param inputsOnly:Flag determines that whether only Input pins to
 * be read.
 */
/**********************************************************************************/
{
   int indx;
   SINT32 ain, aout, AmpIn, freqOut;
   char * PINS[] = { 0, "PIN1", "PIN2", "PIN3", "PIN4", "PIN7", "PIN8", "PIN9",
         "PIN10", "PIN19", "PIN17", "PIN18", "PIN24", "PIN25" };
   sprintf(&Response[strlen(Response)], "SystemType:Sys,%d,%d@",
         (int) EPromData.Systemtype, CurrentPreset.Jumper2);

   for (indx = 1; indx < NUM_DIN; indx++) {
      sprintf(&Response[strlen(Response)], "%s:S,DI,%s,%d@", PINS[indx],
            GetFunctionString(0, USERIOConfig.DigInUse[indx],
                  USERIOConfig.DigInUseStatus[indx], languageBuff),
            UserIO::GetDigINPutActiveStatus((DIGITAL_INPUTS) indx));
   }
   if (!inputsOnly) {
      for (indx = 1; indx < NUM_DOUT; indx++) {
         sprintf(&Response[strlen(Response)], "%s:S,DO,%s,%d@", PINS[indx + 4],
               GetFunctionString(1, USERIOConfig.DigOutUse[indx],
                     USERIOConfig.DigOutUseStatus[indx], languageBuff),
               UserIO::GetDigOutPutActiveStatus((DIGITAL_OUTPUTS) indx));
      }
   }
   for (indx = 1; indx < NUM_AIN; indx++) {
      if ((BIT(EPromData.Systemtype) == DCX_S)
            || (BIT(EPromData.Systemtype) == RACKMOUNT_STD)) {
         ain = UserIO::GetAnalogINPutValue((ANALOG_INPUTS) indx);
         AmpIn = (int) ((ain * MI_ANALOGIP_MAXMV_AMP) / ADC11BITVALUE - 10000);
         sprintf(&Response[strlen(Response)], "%s:S,AI,%s,%d@", PINS[indx + 9],
               GetFunctionString(2, USERIOConfig.AnalogInUse[indx],
                     USERIOConfig.AnalogInUseStatus[indx], languageBuff),
               AmpIn);
      }
      else {
         ain = UserIO::GetAnalogINPutValue((ANALOG_INPUTS) indx);
         if (ain < 0)   //returns -1 if unassigned. Only positive range expected on Analog inputs.
            ain = 0;
         sprintf(&Response[strlen(Response)], "%s:S,AI,%s,%d@", PINS[indx + 9],
               GetFunctionString(2, USERIOConfig.AnalogInUse[indx],
                     USERIOConfig.AnalogInUseStatus[indx], languageBuff),
               (int) (ain * MI_ANALOGIP_MAXMV) / ADC11BITVALUE);
      }
   }
   if (!inputsOnly) {
      for (indx = 1; indx < NUM_AOUT; indx++) {
         if ((JP3_23FREQ == CurrentPreset.Jumper2) && (AOUT2 == indx)
               && (DCX_S == BIT(EPromData.Systemtype))) {
            aout = UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS) indx);
            freqOut = (int) ((aout * MI_ANALOGIP_MAXMV_FREQ) / DAC_MULFACTOR
                  - 10000);
            sprintf(&Response[strlen(Response)], "%s:S,AO,%s,%d@",
                  PINS[indx + 11],
                  GetFunctionString(3, USERIOConfig.AnalogOutUse[indx],
                        USERIOConfig.AnalogOutUseStatus[indx], languageBuff),
                  freqOut);
         }
         else {
            sprintf(&Response[strlen(Response)], "%s:S,AO,%s,%d@",
                  PINS[indx + 11],
                  GetFunctionString(3, USERIOConfig.AnalogOutUse[indx],
                        USERIOConfig.AnalogOutUseStatus[indx], languageBuff),
                  (int) ((UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS) indx))
                        * MI_ANALOGIP_MAXMV) / DAC_MULFACTOR);
         }
      }
   }
}
void WebSite::GetLogData(char * paramStr, WebCommand cmd)
/******************************************************************************************/
/*This function fills data specific to Event and Alarm Log page.
 * in download request case data format is comma seperated to support csv doenload
 * else the data format is what the javascript code on these pages is expecting
 * @param cmd: Type of request alarm or event
 * @param paramStr: pointer to parameters array
 */
/******************************************************************************************/
{
   int nLogs = 0, indx;
   char * jscriptTableID = 0;
   char * dataFormat = 0;
   char * log;
   char TempUserID[MAXUSERIDLEN];
   BOOLEAN download =
         ((cmd == reqDownloadEventdata) || (cmd == reqDownloadAlarmData)) ?
               TRUE :
               FALSE;
   switch (cmd) {
   case reqClearAlarmLog:
      Log::ResetLog(clearAlarm);   //only clearing alarm log
   case reqAlarmdata:
   case reqDownloadAlarmData:
      memset(webLogData, 0, sizeof(webLogData));
      nLogs = Log::ReadAlarmLog(webLogData, Log::GetLatestAlarmEntryIndex(),
      MAX_ALARM_ENTRIES);
      jscriptTableID = "alarmLog";
      log = "Alarm#";
      break;
   case reqClearEventLog:   //only clearing event log
      Log::ResetLog(clearEvent);
   case reqEventdata:
   case reqDownloadEventdata:
      memset(webLogData, 0, sizeof(webLogData));
      nLogs = Log::ReadEventLog(webLogData, Log::GetLatestEventEntryIndex(),
      MAX_EVENT_ENTRIES);
      jscriptTableID = "eventLog";
      log = "Event#";
      break;
   default:
      break;
   };

   if (download) {
      if (cmd == reqDownloadEventdata)
         sprintf(&Response[strlen(Response)],
               "Event#,Days,Hours,Event,UserID,Data1,Data2\r\n");
      else
         sprintf(&Response[strlen(Response)],
               "Alarm#,Days,Hours,Alarm,UserID,Data1,Data2\r\n");

      dataFormat = "%d,%s,%s,%s,%s,%d,%d\r\n";
   }
   else {
      sprintf(&Response[strlen(Response)], "%s$LOG,", jscriptTableID);	//separator changed to $ for html code
      dataFormat = "%d*%s*%s*%s*%s*%d*%d#";
   }
   for (indx = 0; indx < nLogs; indx++) {
      if (webLogData[indx].userID == USERIDBRANSON)
         strcpy(TempUserID, "Branson");
      else if (webLogData[indx].userID == USERIDMANUF1)
         strcpy(TempUserID, "Manuf1");
      else if (webLogData[indx].userID == USERIDMANUF2)
         strcpy(TempUserID, "Manuf2");
      else
         sprintf(TempUserID, "%d", webLogData[indx].userID);
      char * time = strchr(webLogData[indx].timeStamp, ' ') + 1;
      time[-1] = 0;
      sprintf(&Response[strlen(Response)], dataFormat, webLogData[indx].eventNo,
            webLogData[indx].timeStamp, time, webLogData[indx].logStr,
            TempUserID, webLogData[indx].data1, webLogData[indx].data2);
   }
   paramStr = 0;
}

/*This function will be handy when we will support languages other than english
 * Function parse the language file an finds the strings corresponding to all IDs
 * given in strIds param
 * Currently not using this function
 */
void WebSite::GetLanguageStrings(char * strIds, char * response)
{
   char * tempStr = strIds;
   BOOLEAN firstID = TRUE;
   do {
      int strID;
      if (firstID)
         strID = atoi(tempStr);
      else
         strID = atoi(tempStr + 1);
      firstID = FALSE;
      strcat(response, FindLanguageString(strID, languageBuff, FALSE));
      tempStr = strchr(tempStr + 1, ',');
   } while (tempStr);
}

char* WebSite::FindLanguageString(int StringId, char * fileBuff,
      BOOLEAN strOnly)
/****************************************************************************************/
/*This function finds the string in the file pointed by fileBuff
 * param and returns the same
 * @param StringID: Type of request alarm or even
 * @param fileBuff: pointer to language file
 * @param strOnly: flag to determine whether string is needed in format
 * javascript code expects or without format
 * @return: Function string
 */
/****************************************************************************************/
{
   char * tempStr = fileBuff;
   BOOLEAN firstStr = TRUE;
   do {
      int strID;
      if (!firstStr) tempStr = tempStr + 2;

      strID = atoi(tempStr);
      if (strID == StringId) {
         char * temp2 = strstr(tempStr, "\r\n");
         if ((temp2 - tempStr) < MAX_LANGUAGESTRING_LEN)
            strncpy(langString, tempStr, temp2 - tempStr);
         else
            CyGOS::WriteSystemEvent(LEN_INVALID, (short) (5), (short) 0, 0,
                  false);
         break;
      }
      firstStr = FALSE;
      tempStr = strstr(tempStr, "\r\n");
   } while (tempStr);
   if (!strOnly) {
      if (strlen(langString) < MAX_LANGUAGESTRING_LEN) strcat(langString, "@");
   }
   else {
      if (strlen(langString) < MAX_LANGUAGESTRING_LEN)
         strcpy(langString, strchr(langString, ',') + 1);
   }

   return langString;
}
void WebSite::SetLanguageAndUserID(char * paramStr, WebCommand cmd)
/***************************************************************************/
/*This function fills data specific to after user login from website
 * @param paramStr: pointer to parameters array
 * @param cmd: cmd sent in uri.(ignored by this function)
 */
/***************************************************************************/
{
   strcpy(Response, "OK");
   cmd = reqNone;
   paramStr = 0;
}

void WebSite::UpdateIPSetup(char * paramStr, WebCommand cmd)
/*********************************************************************************/
/*This function gets called on save button click on IPsetup page
 @param paramStr: new parameters send by javascript code after validation
 @param cmd: Command sent by jscript in uri
 */
/*********************************************************************************/
{
   NetworkConfiguration config;
   char paramVal[100];
   char * tempPtr1, *tempPtr2;
   unsigned int SysIP, SubnetIP, GatewayIP;
   int retValue = FALSE;
   int MaxIPStrLen = 16;	//"255.255.255.255"
   if (cmd == reqRestore) {	//restore IP setup
      SetDHCPServerIPDefaults();
      SetStaticIPDefaults();
      retValue = TRUE;
   }
   else {
      tempPtr1 = strstr(paramStr, "&ipconfig:");
      tempPtr2 = strchr(tempPtr1, ':') + 1;
      config = (NetworkConfiguration) atoi(tempPtr2);
      if (config <= configDefault) {
         sysConfig.currNetworkConfig = config;
         if (config == configDhcpClient) {
            //ignore other values
            retValue = TRUE;
         }
         else {
            //Set the values for DHCP Server configuration
            tempPtr1 = strstr(paramStr, "IPAddress:");
            tempPtr1 = strchr(tempPtr1, ':') + 1;   //Points to IP Address Value
            tempPtr2 = strstr(tempPtr1, "&SubnetMask:");
            if ((tempPtr2 - tempPtr1) < MaxIPStrLen) {
               strncpy(paramVal, tempPtr1, tempPtr2 - tempPtr1);
               SysIP = IPStrtoUint(paramVal);

               tempPtr1 = strstr(paramStr, "SubnetMask:");
               tempPtr1 = strchr(tempPtr1, ':') + 1;	//Points to Subnet mask Value
               tempPtr2 = strstr(tempPtr1, "&GateWay:");
               if ((tempPtr2 - tempPtr1) < MaxIPStrLen) {
                  strncpy(paramVal, tempPtr1, tempPtr2 - tempPtr1);
                  SubnetIP = IPStrtoUint(paramVal);

                  tempPtr1 = strstr(paramStr, "GateWay:");
                  tempPtr1 = strchr(tempPtr1, ':') + 1;	//Points to gateway Value
                  tempPtr2 = strstr(tempPtr1, "&ipconfig:");
                  if ((tempPtr2 - tempPtr1) < MaxIPStrLen) {
                     strncpy(paramVal, tempPtr1, tempPtr2 - tempPtr1);
                     GatewayIP = IPStrtoUint(paramVal);
                     retValue = TRUE;
                     if (config == configDhcpServer) {
                        sysConfig.systemIP = SysIP;	//IPStrtoUint(paramVal);
                        sysConfig.dhcpConfig.dns = SysIP;   //IPStrtoUint(paramVal);//NNS server is same
                        sysConfig.dhcpConfig.gateway.addr = GatewayIP;	//IPStrtoUint(paramVal);
                        sysConfig.dhcpConfig.netmask.addr = SubnetIP;   //IPStrtoUint(paramVal);
                        sysConfig.dhcpConfig.poolStart.addr =
                              (sysConfig.systemIP.addr
                                    & sysConfig.dhcpConfig.netmask.addr)
                                    + htonl(1);

                     }
                     if (config == configStatic) {
                        sysConfig.systemIPStatic = SysIP;   //IPStrtoUint(paramVal);
                        sysConfig.netmask.addr = SubnetIP;	//IPStrtoUint(paramVal);
                        sysConfig.gw.addr = GatewayIP;   //IPStrtoUint(paramVal);
                     }
                  }
               }
            }
         }
      }
   }
   RegUpdateCmdFlag = TRUE;
   if (retValue == TRUE)
      strcpy(Response, "OK");
   else {
      strcpy(Response, "NOK");
      CyGOS::WriteSystemEvent(LEN_INVALID, (short) (6), (short) 0, 0, false);
   }

}
void WebSite::UpdateIOConfiguration(char * paramStr, WebCommand cmd)
/*******************************************************************************/
/*This function gets called on save button click on IO config page
 @param paramStr: new parameters send by javascript code after validation
 @param cmd: command sent by jscript in uri
 */
/*******************************************************************************/
{
   int indx, funcID, activeStatus;
   UINT16 digOutputsActiveConfiguration = 0;
   BOOLEAN writeLowSelectEvent = FALSE;
   BOOLEAN cableDetectInput = FALSE;
   char * tmpStr = strstr(paramStr, "DI:");   //TODO: Validate string present
   UINT16 EnergyFuncNum = 0;
   tmpStr = strchr(tmpStr, ':') + 1;
   for (indx = 1; indx < NUM_DIN; indx++) {
      funcID = atoi(tmpStr);
      tmpStr = strchr(tmpStr, ',') + 1;
      activeStatus = atoi(tmpStr);
      UserIO::AssignDigitalInputFunction((DIGITAL_INPUTS) indx, funcID,
            activeStatus);
      if (funcID == CABLEDETECT_ACTIVEHIGH) cableDetectInput =
            (BOOLEAN) activeStatus;
      tmpStr = strchr(tmpStr, ',') + 1;
   }
   tmpStr = strstr(paramStr, "DO:");
   tmpStr = strchr(tmpStr, ':') + 1;
   for (indx = 1; indx < NUM_DOUT; indx++) {
      funcID = atoi(tmpStr);
      tmpStr = strchr(tmpStr, ',') + 1;
      activeStatus = atoi(tmpStr);
      UserIO::AssignDigitalOutputFunction((DIGITAL_OUTPUTS) indx, funcID,
            activeStatus);
      //don't care if not active. Let bootloader do whatever it is doing
      digOutputsActiveConfiguration |= (OutPtr[funcID].Active << (indx - 1));
      tmpStr = strchr(tmpStr, ',') + 1;
   }

   tmpStr = strstr(paramStr, "AI:");
   tmpStr = strchr(tmpStr, ':') + 1;
   for (indx = 1; indx < NUM_AIN; indx++) {
      funcID = atoi(tmpStr);
      tmpStr = strchr(tmpStr, ',') + 1;
      activeStatus = atoi(tmpStr);
      UserIO::AssignAnalogInputFunction((ANALOG_INPUTS) indx, funcID,
            activeStatus);
      if ((funcID == ENERGY_CONTROL_RANGE1) || (funcID == ENERGY_CONTROL_RANGE2)
      		|| (funcID == ENERGY_CONTROL_RANGE3)|| (funcID == ENERGY_CONTROL_RANGE4)
            || (funcID == ENERGY_CONTROL_RANGE5)) {
         EnergyFuncNum = funcID;
      }
      tmpStr = strchr(tmpStr, ',') + 1;
   }

   tmpStr = strstr(paramStr, "AO:");
   tmpStr = strchr(tmpStr, ':') + 1;
   for (indx = 1; indx < NUM_AOUT; indx++) {
      funcID = atoi(tmpStr);
      tmpStr = strchr(tmpStr, ',') + 1;
      activeStatus = atoi(tmpStr);
      UserIO::AssignAnalogOutputFunction((ANALOG_OUTPUTS) indx, funcID,
            activeStatus);
      tmpStr = strchr(tmpStr, ',') + 1;
   }
   tmpStr = strstr(tmpStr, "&lowselect");
   tmpStr = strchr(tmpStr, ',') + 1;
   writeLowSelectEvent = (BOOLEAN) atoi(tmpStr);
   DcxLogUserID = DcxWebUserID;
   if (writeLowSelectEvent) {
      pprintf("\n low select");
      Log::WriteEventLog(EVENT_ACTIVELOWDIGIN, 0, 0);
   }
   //save this at FRAM start
   __FRAM_START = digOutputsActiveConfiguration;
   UserIO::SetCableDetect((BOOLEAN) cableDetectInput);
   UserIO::SetEnergyFuncNum(EnergyFuncNum);
   Log::WriteEventLog(USERIO_DEFINED, 0, 0);
   DcxLogUserID = 0;
   strcpy(Response, "OK");
   cmd = reqNone;
}
void WebSite::ProcessDiagnosticIOCommand(char * paramStr, WebCommand cmd)
/*******************************************************************************/
/*This function gets called during diagnostic of IOs
 @param paramStr: pointer to reponse buffer
 @para cmd: Diagnostic command from website
 */
/*******************************************************************************/
{
   switch (cmd) {
   case reqDiagStart:
      UpdateDiagnosticIO(paramStr);
      diagnosticTimeOut = WEBDIAG_TIMEOUT;
      break;
   case reqDiagStop:
      StopIODiagnostic();
      diagnosticTimeOut = WEBDIAG_TIMEOUT;
      break;
   case reqDiagPoll:
      diagnosticTimeOut = WEBDIAG_TIMEOUT;
      StartDiagTimeOut = WEBDIAG_TIMEOUT;
      if (diagFlag
            || ((UserIO::GetGenAlarmFlag() || UserIO::GetReadyFlag())
                  && !MFCycleFlag /*&&!diagPSFlag*/)) {
         strcpy(Response, "OK@");
         EnableWebIO = TRUE;
         if (diagFlag)
            PrepareDiagnosticResponse(TRUE);
         else
            PrepareDiagnosticResponse();
         diagFlag = TRUE;
      }
      else {
         strcpy(Response, "NOTOK@");
      }
      break;
   case reqDiagLoopbackStart:
      DoDiagnosticLoopBackStart();
      diagnosticTimeOut = WEBDIAG_TIMEOUT;
      break;
   case reqDiagLoopbackPoll:
      diagnosticTimeOut = WEBDIAG_TIMEOUT;
      StartDiagTimeOut = LOOPBACK_TIMEOUT;
      DoDiagnosticLoopBack();
      strcpy(Response, "OK@");
      PrepareDiagnosticResponse(TRUE);
      break;
   default:
      strcpy(Response, "NOTOK@");
      break;

   }
}
void WebSite::ProcessResetCommand(char * paramStr, WebCommand cmd)
/*****************************************************************************************/
/*This function gets called on Reset from Diagnostic/PS page
 @param paramStr: pointer to response buffer
 @para cmd: Optional command from website.
 */
/*****************************************************************************************/
{
   //If alarm is there then only reset is needed. There was a case when website resetFlag was true
   //due to reset button was pressed from website. This caused very next alarm to be cleared
   //as soon as it occurred. We can't control on this web site code because reset their should work
   //in the cases where alarm is caused by some other means(i.e without activating TEST,SCAN etc from website
   //) Or if Reset is received from website to Stop cycle or Reset to Clear Memory offset when Clear Memory at Reset is checked.
   if (((AlarmFlag == TRUE) && !CheckForInputs()) || (StartInputEnable == TRUE) || (CurrentPreset.SWDip.Flags.ClearMemAtRst == TRUE && UserIO::GetReadyFlag() == TRUE))
      resetFlag = TRUE;

   strcpy(Response, "OK");
   cmd = reqNone;
   paramStr = 0;
}
void WebSite::UpdateDiagnosticIO(char * paramStr)
/*************************************************************************************/
/*This function gets called on save button click on diagnostic IO page
 @param paramStr: new parameters send by javascript code after validation
 */
/*************************************************************************************/
{
   char * tempStr = paramStr;
   int indx, freqOut, freqoutval;
   //first check system is ready for diagnostic
   for (indx = 1; indx < NUM_DOUT; indx++)
      tempDigOutPutStatus[indx] = UserIO::GetDigOutPutActiveStatus(
            (DIGITAL_OUTPUTS) indx);

   for (indx = 1; indx < NUM_AOUT; indx++) {
      tempAnalogOutPutVals[indx] = UserIO::GetAnalogOUTPutValue(
            (ANALOG_OUTPUTS) indx);
   }

   DiagStartFlag = TRUE;
   StartDiagTimeOut = WEBDIAG_TIMEOUT;
   for (indx = 1; indx < NUM_DOUT; indx++) {
      UserIO::SetDiagnosticOutput((DIGITAL_OUTPUTS) indx,
            BOOLEAN(atoi(tempStr)));
      tempStr = strchr(tempStr, ',') + 1;
   }
   for (indx = 1; indx < NUM_AOUT; indx++) {
      if ((CurrentPreset.Jumper2 == JP3_23FREQ) && (indx == AOUT2)
            && ((BIT(EPromData.Systemtype) == DCX_S))) {
         if (tempStr[0] == '-') {
            freqoutval = atoi(tempStr + 1) * (-1);
            freqOut = ((freqoutval + 10000) * DAC_MULFACTOR)
                  / MI_ANALOGIP_MAXMV_FREQ;
         }
         else {
            freqOut = ((atoi(tempStr) + 10000) * DAC_MULFACTOR)
                  / MI_ANALOGIP_MAXMV_FREQ;
         }

         UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS) indx, (freqOut));
      }

      else
         UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS) indx,
               (((atoi(tempStr)) * DAC_MULFACTOR) / MI_ANALOGIP_MAXMV));

      tempStr = strchr(tempStr, ',') + 1;
   }
   strcpy(Response, "OK");
}
void WebSite::StopIODiagnostic(void)
/*********************************************************************************/
/*This function gets called on stop button click on diagnostic IO page
 */
/*********************************************************************************/
{
   if (DiagStartFlag) {
      DiagStartFlag = FALSE;
      RestoreOutPutStatus();
      strcpy(Response, "OK@");
      PrepareDiagnosticResponse();
   }
}

void WebSite::RestoreOutPutStatus(void)
/**************************************************************************************/
/* This function restores the original output values(i.e. before diagnostic)
 *  of Digital and analog outputs and starts the state machine to receive
 *  commands from other interfaces
 */
/**************************************************************************************/
{
   int indx;
   for (indx = 1; indx < NUM_DOUT; indx++)
      UserIO::SetDigOutPutActiveStatus((DIGITAL_OUTPUTS) indx,
            (tempDigOutPutStatus[indx]) ? TRUE : FALSE);

   for (indx = 1; indx < NUM_AOUT; indx++)
      UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS) indx,
            tempAnalogOutPutVals[indx]);
}
/*This function gets called on LoopBack button click on diagnostic IO page
 * it toggles the all digital and analog outputs
 */
void WebSite::DoDiagnosticLoopBack(void)
{
   BOOLEAN currState;
   SINT32 currDacOutPut;
   int indx;

   for (indx = 1; indx < NUM_DOUT; indx++) {
      currState = UserIO::GetDigOutPutActiveStatus((DIGITAL_OUTPUTS) indx);
      UserIO::SetDigOutPutActiveStatus((DIGITAL_OUTPUTS) indx,
            (currState) ? FALSE : TRUE);
   }

   for (indx = 1; indx < NUM_AOUT; indx++) {
      currDacOutPut = UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS) indx);
      //Toggle AnalogOutput
      UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS) indx,
            (currDacOutPut) ? 0 : DAC_MULFACTOR);
   }
}

void WebSite::DoDiagnosticLoopBackStart(void)
/********************************************************************************************************/
/*This function gets called initial;y on start of loopback
 * from DiagnosticIO page. It stores the current state of
 * digital and analog outputs in an temporary global array
 */
/********************************************************************************************************/
{
   int indx;
   DiagStartFlag = TRUE;
   StartDiagTimeOut = LOOPBACK_TIMEOUT;
   for (indx = 1; indx < NUM_AOUT; indx++) {   //Store AnalogOutput
      tempAnalogOutPutVals[indx] = UserIO::GetAnalogOUTPutValue(
            (ANALOG_OUTPUTS) indx);
      UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS) indx, 0);
   }
   for (indx = 1; indx < NUM_DOUT; indx++) {   // Store Digital Output
      tempDigOutPutStatus[indx] = UserIO::GetDigOutPutActiveStatus(
            (DIGITAL_OUTPUTS) indx);
   }
   strcpy(Response, "OK");
}

void WebSite::UpdatePreset(char * paramStr, WebCommand cmd)
/***********************************************************************************************************/
/*This function gets called on save button click on weld-preset page
 @param paramStr: new parameters send by javascript code after validation
 @param cmd:command sent by jscript in uri
 */
/***********************************************************************************************************/
{
	SINT32 tempOffset = 0, tempdigitaltune;
	char * tempStr = strstr(paramStr, "weldAmp=");
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, WeldAmplitude, (atoi(strchr(tempStr, '=') + 1)), WcParamAmplitude1, (atoi(strchr(tempStr, '=') + 1)),DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&startRamp=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, WeldRampTime, (CONV_MILLI_MICROSEC(atoi(strchr(tempStr, '=') + 1))), DcpParamWeldRampTime, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&FreqOff=") + 1;
	tempStr = strchr(tempStr, '=') + 1;
	if (tempStr[0] == '-')
		tempOffset = atoi(tempStr + 1) * (-1);
	else
		tempOffset = atoi(tempStr);
	tempStr = strstr(paramStr, "&endofweldstore=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.EndOfWeldStore, atoi(strchr(tempStr, '=') + 1), DcpParamMemoryStoreFlag, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&externalamplitude=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.ExtAmplitude, (atoi(strchr(tempStr, '=') + 1)), WcParamAmpStepCondition, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&externalfrequency=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.ExtFrequency, (atoi(strchr(tempStr, '=') + 1)), WcParamFreqOffsetFlag, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	if (CurrentPreset.SWDip.Flags.ExtFrequency == false)
		CHECKANDLOG_PRESETCHANGE(CurrentPreset, FreqOffset, tempOffset, WcParamFreqOffset, tempOffset, DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "seekRamp=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SeekRampTime, (CONV_MILLI_MICROSEC(atoi(strchr(tempStr, '=') + 1))), ParaSeekSeekRampTime, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&seekTime=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SeekTime, (CONV_MILLI_MICROSEC(atoi(strchr(tempStr, '=') + 1))), ParaSeekSeekTime, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&SeekFreqOff=") + 1;
	tempStr = strchr(tempStr, '=') + 1;
	if (tempStr[0] == '-')
		tempOffset = atoi(tempStr + 1) * (-1);
	else
		tempOffset = atoi(tempStr);
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SeekFreqOffset,tempOffset, ParaSeekFreqOffset, tempOffset, DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&powerup=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.PowerUpOptions, (atoi(strchr(tempStr, '=') + 1)), PowerUpOptions, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&ClrMemAtPwrUp=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.ClrMemAtPwrUp, (atoi(strchr(tempStr, '=') + 1)), ClrMemAtPwrUp, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&timedSeek=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.TimedSeek, (atoi(strchr(tempStr, '=') + 1)), DcpParamTimedSeekFlag, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&stoponalarm=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, AlarmResetReq, (atoi(strchr(tempStr, '=') + 1)), AlarmResetReq, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&digitaltune=") + 1;
	tempdigitaltune = (atoi(strchr(tempStr, '=') + 1) * 1000) / F_Factor;
	if ((F_Factor == F30KHZF_FACTOR)
	      && (((atoi(strchr(tempStr, '=') + 1) * 1000) % 150) != 0)) {
		tempdigitaltune = tempdigitaltune + 1;
	}
	if (CurrentPreset.DigitalTune != tempdigitaltune) {
		CurrentPreset.MemoryFreq = 0;
		MemoryValue = 0;
	}
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, DigitalTune, tempdigitaltune, DigitalTune, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&JP2=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, Jumper2, (JP2)(atoi(strchr(tempStr, '=') + 1)), Jumper2, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	if (BIT(EPromData.Systemtype) == DCX_S) {
		if (CurrentPreset.Jumper2 == JP3_23FREQ)
			UserIO::AssignAnalogOutputFunction(AOUT2, FREQ_OUT, TRUE);
		else
			UserIO::AssignAnalogOutputFunction(AOUT2, AMPL_OUT, TRUE);
	}
	tempStr = strstr(paramStr, "&ClrMemReset=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.ClearMemAtRst, (BOOLEAN)(atoi(strchr(tempStr, '=') + 1)), ClearMemAtRst, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&ClrMemBfrSeek=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.ClrMemBfrSeek, (BOOLEAN)(atoi(strchr(tempStr, '=') + 1)), ClrMemBfrSeek, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	tempStr = strstr(paramStr, "&SetDigTuneWithScan=") + 1;
	CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.SetDigTuneWithScan, (BOOLEAN)(atoi(strchr(tempStr, '=') + 1)), SetDigTuneWithScan, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	if (BIT(EPromData.Systemtype) == RACKMOUNT_STD) {
		tempStr = strstr(paramStr, "&PwrLtLow=") + 1;
		CHECKANDLOG_PRESETCHANGE(CurrentPreset, PowerWindowLimitLow, (atoi(strchr(tempStr, '=') + 1)), PowerWindowLimitLow, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
		tempStr = strstr(paramStr, "&PwrLtHigh=") + 1;
		CHECKANDLOG_PRESETCHANGE(CurrentPreset, PowerWindowLimitHigh, (atoi(strchr(tempStr, '=') + 1)), PowerWindowLimitHigh, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
		tempStr = strstr(paramStr, "&PCheckbox=") + 1;
		CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.PlusLimitCheck, (atoi(strchr(tempStr, '=') + 1)), PlusLimitCheck, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
		tempStr = strstr(paramStr, "&MCheckbox=") + 1;
		CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.MinusLimitCheck, (atoi(strchr(tempStr, '=') + 1)), MinusLimitCheck, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
		tempStr = strstr(paramStr, "&peakpwron=") + 1;
		CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.PowerLimitOn, (atoi(strchr(tempStr, '=') + 1)), PowerLimitOn, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
		tempStr = strstr(paramStr, "&Weldmode=") + 1;
		CHECKANDLOG_PRESETCHANGE(CurrentPreset, WeldMode, (MODES)(atoi(strchr(tempStr, '=') + 1)), WcParamWeldMode, (MODES)(atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
		tempStr = strstr(paramStr, "&WeldTime=") + 1;
		CHECKANDLOG_PRESETCHANGE(CurrentPreset, WeldTime, (atoi(strchr(tempStr, '=') + 1)), WcParamWeldTimeCP, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
		tempStr = strstr(paramStr, "&Energy=") + 1;
		CHECKANDLOG_PRESETCHANGE(CurrentPreset, EnergyCP, (atoi(strchr(tempStr, '=') + 1)), WcParamEnergyCP, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
		tempStr = strstr(paramStr, "&Handheld=") + 1;
		CHECKANDLOG_PRESETCHANGE(CurrentPreset, SWDip.Flags.HandHeldMode, (BOOLEAN)(atoi(strchr(tempStr, '=') + 1)), HandHeldMode, (atoi(strchr(tempStr, '=') + 1)), DcxWebUserID, TRUE);
	}
	SavePreset(0);
	RegUpdateCmdFlag = TRUE;
	strcpy(Response, "OK");
	cmd = reqNone;
}

void WebSite::UpdatePresetAdvanced(char * paramStr, WebCommand cmd)
/*********************************************************************************************************/
/*This function gets called on save button click on advanced-preset page
 @param paramStr: new parameters send by javascript code after validation
 @param cmd: Command sent by jscript in uri
 */
/********************************************************************************************************/
{
   char * tempStr = strstr(paramStr, "AmplitudeloopC1=");
   CurrentPreset.WeldAmplitudeLoopC1 = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&AmplitudeloopC2=") + 1;
   CurrentPreset.WeldAmplitudeLoopC2 = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&Phaseloop=") + 1;
   CurrentPreset.WeldPhaseloopI = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&PhaseloopCF=") + 1;
   CurrentPreset.WeldPhaseloopCF = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&FreqLow=") + 1;
   CurrentPreset.WeldFrequencyLimitLow = ((atoi(strchr(tempStr, '=') + 1))
         * 1000) / F_Factor;
   tempStr = strstr(paramStr, "&FreqHigh=") + 1;
   CurrentPreset.WeldFrequencyLimitHigh = ((atoi(strchr(tempStr, '=') + 1))
         * 1000) / F_Factor;
   tempStr = strstr(paramStr, "&PhaseLimitTime=") + 1;
   CurrentPreset.WeldPhaseLimitTime = CONV_MILLI_MICROSEC(
         atoi(strchr(tempStr, '=') + 1));
   tempStr = strstr(paramStr, "&PhaseLimit=") + 1;
   CurrentPreset.WeldPhaseLimit = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "UseTrueValues=");
   char * tempStr2 = strchr(tempStr, '=') + 1;
   if (*tempStr2 == 'f') {
      CurrentPreset.UseTrueValues = false;
   }
   else {
      CurrentPreset.UseTrueValues = true;
   }

   tempStr = strstr(paramStr, "AmplitudeloopC1New=");
   CurrentPreset.WeldAmplitudeLoopC1New = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&AmplitudeloopC2New=") + 1;
   CurrentPreset.WeldAmplitudeLoopC2New = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&PhaseloopNew=") + 1;
   CurrentPreset.WeldPhaseloopINew = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&PhaseloopPropNew=") + 1;
   CurrentPreset.WeldPhaseloopPNew = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&FreqWindowSize=") + 1;
   CurrentPreset.FreqWindowSize = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&PhaseloopCFNew=") + 1;
   CurrentPreset.WeldPhaseloopCFNew = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&FreqLowNew=") + 1;
   CurrentPreset.WeldFrequencyLimitLowNew = ((atoi(strchr(tempStr, '=') + 1))
         * 1000) / F_Factor;
   tempStr = strstr(paramStr, "&FreqHighNew=") + 1;
   CurrentPreset.WeldFrequencyLimitHighNew = ((atoi(strchr(tempStr, '=') + 1))
         * 1000) / F_Factor;
   tempStr = strstr(paramStr, "&PhaseLimitTimeNew=") + 1;
   CurrentPreset.WeldPhaseLimitTimeNew = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&PhaseLimitNew=") + 1;
   CurrentPreset.WeldPhaseLimitNew = atoi(strchr(tempStr, '=') + 1);

   tempStr = strstr(paramStr, "SeekAmplitudeloopC1=") + 1;
   CurrentPreset.SeekAmplitudeLoopC1 = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&SeekAmplitudeloopC2=") + 1;
   CurrentPreset.SeekAmplitudeLoopC2 = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&SeekPhaseloop=") + 1;
   CurrentPreset.SeekPhaseloopI = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&SeekPhaseloopCF=") + 1;
   CurrentPreset.SeekPhaseloopCF = atoi(strchr(tempStr, '=') + 1);
   tempStr = strstr(paramStr, "&SeekFreqLow=") + 1;
   CurrentPreset.SeekFrequLimitLow = ((atoi(strchr(tempStr, '=') + 1)) * 1000)
         / F_Factor;
   tempStr = strstr(paramStr, "&SeekFreqHigh=") + 1;
   CurrentPreset.SeekFrequLimitHigh = ((atoi(strchr(tempStr, '=') + 1)) * 1000)
         / F_Factor;
   tempStr = strstr(paramStr, "&SeekAmp=") + 1;
   CurrentPreset.SeekAmplitude = (atoi(strchr(tempStr, '=') + 1));

   tempStr = strstr(paramStr, "Amplitude=") + 1;
   ParaScan.MaxAmplitude = (atoi(strchr(tempStr, '=') + 1));
   tempStr = strstr(paramStr, "&Current=") + 1;
   ParaScan.MaxCurrent = (atoi(strchr(tempStr, '=') + 1));
   tempStr = strstr(paramStr, "&FreqStart=") + 1;
   ParaScan.FrequencyStart =
         ((atoi(strchr(tempStr, '=') + 1)) * SCANFREQFACTOR);
   tempStr = strstr(paramStr, "&FreqEnd=") + 1;
   ParaScan.FrequencyStop = ((atoi(strchr(tempStr, '=') + 1)) * SCANFREQFACTOR);
   tempStr = strstr(paramStr, "&StepDelay=") + 1;
   ParaScan.TimeDelay = CONV_MILLI_MICROSEC(atoi(strchr(tempStr, '=') + 1));

   SavePreset(0);
   SaveScanParamCRC();
   RegUpdateCmdFlag = TRUE;
   strcpy(Response, "OK");
   cmd = reqNone;
}

void WebSite::ViewWeldGraphGetDatahandeler(char * paramStr, WebCommand cmd)
/*************************************************************************************************************/
/*This function gets called on Update Graph button click on PS-Test page
 @param paramStr: new parameters send by javascript code after validation
 @param cmd: Command sent by jscript in uri
 */
/*************************************************************************************************************/
{
   int i, indx, strtindx, endindx;
   strtindx = WeldBufferIndex - GRAPHPOINT;
   endindx = WeldBufferIndex;
   if (strtindx < 0) strtindx = SIZE_WELDBUFFER - GRAPHPOINT + WeldBufferIndex;
   if (WeldBufferTime < GRAPHPOINT) {
      endindx = WeldBufferIndex;
      strtindx = 1;
   }
   else
      endindx = GRAPHPOINT;
   indx = strtindx - 1;
   switch (cmd) {
   case reqTimeData:
      strcpy(Response, "Time,");
      for (i = 0; i < endindx; i++) {
         if (WeldBuffer[indx].logValid == TRUE)
            sprintf(&Response[strlen(Response)], "%d,", WeldBuffer[indx].Time);
         if (indx == SIZE_WELDBUFFER - 1)
            indx = 0;
         else
            indx++;
      }
      break;
   case reqCurrentData:
      strcpy(Response, "Current,");
      for (i = 0; i < endindx; i++) {
         if (WeldBuffer[indx].logValid == TRUE)
            sprintf(&Response[strlen(Response)], "%d,",
                  WeldBuffer[indx].Current);
         if (indx == SIZE_WELDBUFFER - 1)
            indx = 0;
         else
            indx++;
      }
      break;
   case reqPhaseData:
      strcpy(Response, "Phase,");
      for (i = 0; i < endindx; i++) {
         if (WeldBuffer[indx].logValid == TRUE)
            sprintf(&Response[strlen(Response)], "%d,", WeldBuffer[indx].Phase);
         if (indx == SIZE_WELDBUFFER - 1)
            indx = 0;
         else
            indx++;
      }
      break;
   case reqAmplitudeData:
      strcpy(Response, "Amplitude,");
      for (i = 0; i < endindx; i++) {
         if (WeldBuffer[indx].logValid == TRUE)
            sprintf(&Response[strlen(Response)], "%d,",
                  WeldBuffer[indx].ActualAmplitude);
         if (indx == SIZE_WELDBUFFER - 1)
            indx = 0;
         else
            indx++;
      }
      break;
   case reqPowerData:
      strcpy(Response, "Power,");
      for (i = 0; i < endindx; i++) {
         if (WeldBuffer[indx].logValid == TRUE)
            sprintf(&Response[strlen(Response)], "%d,", WeldBuffer[indx].Power);
         if (indx == SIZE_WELDBUFFER - 1)
            indx = 0;
         else
            indx++;
      }
      break;
   case reqPWMAmplitudeData:
      strcpy(Response, "PWMAmplitude,");
      for (i = 0; i < endindx; i++) {
         if (WeldBuffer[indx].logValid == TRUE)
            sprintf(&Response[strlen(Response)], "%d,",
                  WeldBuffer[indx].PWMValue);
         if (indx == SIZE_WELDBUFFER - 1)
            indx = 0;
         else
            indx++;
      }
      break;
   case reqFrequencyData:
      strcpy(Response, "Frequency,");
      for (i = 0; i < endindx; i++) {
         if (WeldBuffer[indx].logValid == TRUE)
            sprintf(&Response[strlen(Response)], "%d,",
                  WeldBuffer[indx].Frequency);
         if (indx == SIZE_WELDBUFFER - 1)
            indx = 0;
         else
            indx++;
      }
      break;
   case reqPresetData:
      strcpy(Response, "PresetData,");			// Pre formatted preset data
      sprintf(&Response[strlen(Response)],
            "%d,%d,Weld Amplitude LoopC1,%d \nWeld Amplitude LoopC2,%d \n"
                  "Weld Phase LoopI,%d \nWeld Phase LoopCF,%d \n"
                  "Weld Amplitude LoopC1 New,%d \nWeld Amplitude LoopC2 New,%d \n"
                  "Weld Phase LoopI New,%d \nWeld Phase LoopP New,%d \n"
                  "Weld Phase LoopCF New,%d \nSeek Phase LoopI,%d \n"
                  "Frequency Window Size,%d \nWeld Frequency LimitHigh,%d \n"
                  "Weld Frequency LimitLow,%d \nWeld Phase Limit,%d \n"
                  "Weld Phase Limit Time,%d \nWeld Frequency Limit High New,%d \n"
                  "Weld Frequency Limit Low New,%d \n Weld Phase Limit New,%d \n"
                  "Weld Phase Limit Time New,%d \nSeek Amplitude,%d \n"
                  "Seek Amplitude LoopC1,%d \n Seek Amplitude LoopC2,%d \n"
                  "Seek Frequency Limit High,%d \nSeek Frequency Limit Low,%d \n"
                  "Seek Phase LoopCF,%d \nWeld Amplitude,%d \n"
                  "Frequency Offset,%d \nWeld Ramp Time,%d \n"
                  "Seek Ramp Time,%d \nSeek Time,%d \n"
                  "Alarm Reset Request,%d",
            (int) (((Midband + FREQ_MIN) * F_Factor) / 1000),
            (int) (((Midband + FREQ_MAX) * F_Factor) / 1000),
            (int) CurrentPreset.WeldAmplitudeLoopC1,
            (int) CurrentPreset.WeldAmplitudeLoopC2,
            (int) CurrentPreset.WeldPhaseloopI,
            (int) CurrentPreset.WeldPhaseloopCF,
            (int) CurrentPreset.WeldAmplitudeLoopC1New,
            (int) CurrentPreset.WeldAmplitudeLoopC2New,
            (int) CurrentPreset.WeldPhaseloopINew,
            (int) CurrentPreset.WeldPhaseloopPNew,
            (int) CurrentPreset.WeldPhaseloopCFNew,
            (int) CurrentPreset.SeekPhaseloopI,
            (int) CurrentPreset.FreqWindowSize,
            (int) CurrentPreset.WeldFrequencyLimitHigh,
            (int) CurrentPreset.WeldFrequencyLimitLow,
            (int) CurrentPreset.WeldPhaseLimit,
            (int) CurrentPreset.WeldPhaseLimitTime,
            (int) CurrentPreset.WeldFrequencyLimitHighNew,
            (int) CurrentPreset.WeldFrequencyLimitLowNew,
            (int) CurrentPreset.WeldPhaseLimitNew,
            (int) CurrentPreset.WeldPhaseLimitTimeNew,
            (int) CurrentPreset.SeekAmplitude,
            (int) CurrentPreset.SeekAmplitudeLoopC1,
            (int) CurrentPreset.SeekAmplitudeLoopC2,
            (int) CurrentPreset.SeekFrequLimitHigh,
            (int) CurrentPreset.SeekFrequLimitLow,
            (int) CurrentPreset.SeekPhaseloopCF,
            (int) CurrentPreset.WeldAmplitude, (int) CurrentPreset.FreqOffset,
            (int) CurrentPreset.WeldRampTime, (int) CurrentPreset.SeekRampTime,
            (int) CurrentPreset.SeekTime, (int) CurrentPreset.AlarmResetReq);
      break;
   default:
      break;
   }
   paramStr = 0;
}

void WebSite::ViewGraphHornSignatureGetDatahandeler(char * paramStr,
      WebCommand cmd)
/*************************************************************************************************************/
/*This function gets called on Update Graph button click on HornSignature page
 @param paramStr: new parameters send by javascript code after validation
 @param cmd: Command sent by jscript in uri
 */
/*************************************************************************************************************/
{
   int i, frequency;
   if ((ScanInitiate == TRUE) || (ScanStarted == TRUE) || (ScanRun == TRUE))
      strcpy(Response, "SCANINPROGRESS");
   else {
      switch (cmd) {
      case reqCurrentData:
         strcpy(Response, "Current,");
         for (i = 0; i <= HornScanGraphPointCount; i++)
            sprintf(&Response[strlen(Response)], "%d,",
                  (ScanBuffer[i].Current * SCALING_FACTOR) / ADC11BITVALUE);
         break;
      case reqPhaseData:
         strcpy(Response, "Phase,");
         for (i = 0; i <= HornScanGraphPointCount; i++)
            sprintf(&Response[strlen(Response)], "%d,", ScanBuffer[i].Phase);
         break;
      case reqAmplitudeData:
         strcpy(Response, "Amplitude,");
         for (i = 0; i <= HornScanGraphPointCount; i++)
            sprintf(&Response[strlen(Response)], "%d,",
                  (ScanBuffer[i].Amplitude * SCALING_FACTOR) / ADC11BITVALUE);
         break;
      case reqFrequencyData:
         strcpy(Response, "Frequency,");
         for (i = 0; i <= HornScanGraphPointCount; i++)
            sprintf(&Response[strlen(Response)], "%d,",
                  (int) ScanBuffer[i].Frequency);
         break;
      case reqPresetData:
         strcpy(Response, "PresetData,");		// Pre formatted preset data
         sprintf(&Response[strlen(Response)],
               "%d,%d,%d,\n\nFrequency Start (Hz),%d"
                     "\nFrequency Stop (Hz),%d"
                     "\nFrequency Step (Hz),%d"
                     "\nStep - Delay (ms),%d"
                     "\nAmplitude(%%),%d"
                     "\nCurrent(%%),%d",
               (int) (ParaScan.FrequencyStep / SCANFREQFACTOR),
               (int) (ParaScan.FrequencyStart / SCANFREQFACTOR) + 1,
               (int) (ParaScan.FrequencyStop / SCANFREQFACTOR) + 1,
               (int) (ParaScan.FrequencyStart / SCANFREQFACTOR),
               (int) (ParaScan.FrequencyStop / SCANFREQFACTOR),
               (int) (ParaScan.FrequencyStep / SCANFREQFACTOR),
               (int) (CONVMICRO_MILLI(ParaScan.TimeDelay)),
               (int) ParaScan.MaxAmplitude, (int) ParaScan.MaxCurrent);
         for (int i = 0; i < DCP_NUMRESONANCES; i++) {
            frequency = (int) ResonanceFreqs[i];
            if (frequency)
               if (ResonanceFreqType[i] == FREQ_PARALLEL)
                  sprintf(&Response[strlen(Response)],
                        "\nParallel Resonant Point %d (Hz),%d", (i / 2 + 1),
                        (int) ResonanceFreqs[i]);
               else if (ResonanceFreqType[i] == FREQ_SERIES)
                  sprintf(&Response[strlen(Response)],
                        "\nSeries Resonant Point %d (Hz),%d", (i / 2 + 1),
                        (int) ResonanceFreqs[i]);
         }
         break;
      case reqResFreq:
         strcpy(Response, "ResonFreq,");
         for (int i = 0; i < DCP_NUMRESONANCES; i++) {
            if (ResonanceFreqType[i] == FREQ_PARALLEL)
               sprintf(&Response[strlen(Response)], "P:%d,",
                     (int) ResonanceFreqs[i]);
            else if (ResonanceFreqType[i] == FREQ_SERIES)
               sprintf(&Response[strlen(Response)], "S:%d,",
                     (int) ResonanceFreqs[i]);
         }
         break;
      default:
         break;
      }
   }
   paramStr = 0;
}

UINT32 CalculateDDSFreqinPercent(void)
/****************************************************************************************************************/
/*This function calculates the number of segments
 * to be on, on frequency bar during TEST from website
 */
/****************************************************************************************************************/
{
   //Convert current Freq
   UINT16 CurrFreq = (((DDSFrequency) * F_Factor) / 1000);

   if (CurrFreq >= FREQ40KHz_START) {
      CurrFreq = ((CurrFreq - FREQ40KHz_START) / FREQ40KHz_HzPERSEGMENT) + 1;

   }
   else if (CurrFreq >= FREQ30KHz_START) {
      CurrFreq = ((CurrFreq - FREQ30KHz_START) / FREQ30KHz_HzPERSEGMENT) + 1;

   }
   else {
      CurrFreq = ((CurrFreq - FREQ20KHz_START) / FREQ20KHz_HzPERSEGMENT) + 1;

   }
   if (CurrFreq > NUM_BARGRAPH_SEGMENT)   // Total 20 icons for power level.
      CurrFreq = NUM_BARGRAPH_SEGMENT;
   return CurrFreq;
}

UINT16 CalculateMemoryFreqinPercent(void)
/****************************************************************************************************************/
/*This function calculates the segment number
 * to set on , on memory bar during TEST from website
 */
/****************************************************************************************************************/
{
   //Convert MemoryFreq
   UINT16 MemoryFreq = (((CurrentPreset.MemoryFreq + CurrentPreset.DigitalTune)
         * F_Factor) / 1000);

   switch (EPromData.PSfrequency) {
   case FREQ_40KHZ:
      MemoryFreq = ((MemoryFreq - FREQ40KHz_START) / FREQ40KHz_HzPERSEGMENT)
            + 1;
      break;
   case FREQ_30KHZ:
      MemoryFreq = ((MemoryFreq - FREQ30KHz_START) / FREQ30KHz_HzPERSEGMENT)
            + 1;
      break;
   case FREQ_20KHZ:
   default:
      MemoryFreq = ((MemoryFreq - FREQ20KHz_START) / FREQ20KHz_HzPERSEGMENT)
            + 1;
      break;
   }

   if (MemoryFreq > NUM_BARGRAPH_SEGMENT)   // Total 20 icons for power level.
      MemoryFreq = NUM_BARGRAPH_SEGMENT;

   return MemoryFreq;
}

UINT16 CalculatePowerinPercent(UINT16 PowerVal)
/****************************************************************************************************************/
/*This function calculates the segment numbers
 * to set on , on power bar during TEST from website
 */
/****************************************************************************************************************/
{
   //Convert current power input in percentage form
   UINT16 currPower = (PowerVal * NUM_BARGRAPH_SEGMENT) / 100;

   if (currPower > NUM_BARGRAPH_SEGMENT)   // Total 20 icons for power level.
      currPower = NUM_BARGRAPH_SEGMENT;

   return currPower;
}

UINT16 CalculateAmplitudeinPercent(UINT16 AmpVal)
/****************************************************************************************************************/
/*This function calculates the segment numbers
 * to set on , on amplitude bar during TEST from website
 */
/****************************************************************************************************************/
{
   UINT16 Amplitude = (AmpVal * NUM_BARGRAPH_SEGMENT) / 100;

   if (Amplitude > NUM_BARGRAPH_SEGMENT)   // Total 20 icons for power level.
      Amplitude = NUM_BARGRAPH_SEGMENT;

   return Amplitude;
}

void WebSite::ProcessTestCommand(char * paramStr, WebCommand cmd)
/****************************************************************************************************************/
/*This function gets called during TEST from website
 * and fills the live data to be shown on diagnostic-ps
 * page during TEST
 * @param cmd:Test command from website e.g start,stop,get live data etc.
 * @param paramStr: pointer to parameters
 */
/****************************************************************************************************************/
{
   UINT16 TempPwr, TempAmp;
   diagnosticPSTimeOut = WEBTEST_TIMEOUT;
   CurrentState CurrSt;
   switch (cmd) {
   case reqSeekStart:
      if (UserIO::GetReadyFlag() && !diagFlag && !MFCycleFlag) {
         sprintf(&Response[strlen(Response)], "TESTSTARTED:%d,%d,%d,%d,%d",
               (int) stateSeek, (int) CalculateDDSFreqinPercent(),
               (int) CalculateMemoryFreqinPercent(),
               (int) CalculateAmplitudeinPercent(ActualAmplitude),
               (int) CalculatePowerinPercent(PowerInput));
         //testStartFlag = TRUE;
         WebSeek = TRUE;
         DcxLogUserID = DcxWebUserID;
      }
      else
         strcat(Response, "SYSTEMBUSY:");
      break;
   case reqSeekPoll:
      CurrSt = CheckForCurrentFunction();
      if (CurrSt == stateSeek) {
         sprintf(&Response[strlen(Response)],
               "TESTPOLLRESPONSE:%d,%d,%d,%d,%d@%d,%d,%d,%d",
               (int) CheckForCurrentFunction(),
               (int) CalculateDDSFreqinPercent(),
               (int) CalculateMemoryFreqinPercent(),
               (int) CalculateAmplitudeinPercent(
                     (ActualAmplitude * 100) / ADC11BITVALUE),
               (int) CalculatePowerinPercent(
                     (PowerInput * 100) / ADC11BITVALUE),
               (int) ((DDSFrequency * F_Factor) / 1000),
               (int) (((CurrentPreset.MemoryFreq + CurrentPreset.DigitalTune)
                     * F_Factor) / 1000),
               (int) (ActualAmplitude * 100) / ADC11BITVALUE,
               (int) (PowerInput * 100) / ADC11BITVALUE);

      }
      else {
         TempPwr = 0;
         TempAmp = 0;
         if ((WeldBufferIndex >= 0) && (WeldBufferIndex < SIZE_WELDBUFFER)) {
            TempPwr = WeldBuffer[WeldBufferIndex].Power;
            TempAmp = WeldBuffer[WeldBufferIndex].ActualAmplitude;
         }
         sprintf(&Response[strlen(Response)],
               "TESTSTOPPED:%d,%d,%d,%d,%d@%d,%d,%d,%d",
               (int) CheckForCurrentFunction(),
               (int) CalculateDDSFreqinPercent(),
               (int) CalculateMemoryFreqinPercent(),
               (int) CalculateAmplitudeinPercent(TempAmp),
               (int) CalculatePowerinPercent(TempPwr),
               (int) ((DDSFrequency * F_Factor) / 1000),
               (int) (((CurrentPreset.MemoryFreq + CurrentPreset.DigitalTune)
                     * F_Factor) / 1000), (int) (TempAmp), (int) (TempPwr));
      }

      break;
   case reqSeekResult:
      TempPwr = 0;
      TempAmp = 0;
      if ((WeldBufferIndex >= 0) && (WeldBufferIndex < SIZE_WELDBUFFER)) {
         TempPwr = WeldBuffer[WeldBufferIndex].Power;
         TempAmp = WeldBuffer[WeldBufferIndex].ActualAmplitude;
      }
      sprintf(&Response[strlen(Response)], "TESTRESULT:%d,%d,%d,%d,%d",
            (int) CheckForCurrentFunction(), (int) CalculateDDSFreqinPercent(),
            (int) CalculateMemoryFreqinPercent(),
            (int) CalculateAmplitudeinPercent(TempAmp),
            (int) CalculatePowerinPercent(TempPwr));
      WebSeek = FALSE;
      break;
   default:
      strcpy(Response, "OK");
      break;
   }
   paramStr = 0;
}

void WebSite::ProcessScanCommand(char * paramStr, WebCommand cmd)
/****************************************************************************************************************/
/*This function gets called during SCAN from website
 * and fills the scan result to be shown on diagnostic-ps
 * page during SCAN
 * @param cmd:SCAN command from website e.g start,abort etc
 * @param paramStr: pointer to response buffer
 */
/****************************************************************************************************************/
{
   diagnosticPSTimeOut = WEBSCAN_TIMEOUT;

   switch (cmd) {
   case reqScanStart:
      if (UserIO::GetReadyFlag() && !MFCycleFlag && !diagFlag) {
         strcat(Response, "SCANSTARTED:");
         scanAbortFlag = FALSE;
         scanStartFlag = TRUE;
         webtimeOutScan = WEBSCAN_TIMEOUT;
         DcxLogUserID = DcxWebUserID;
         EnableWebIO = TRUE;
      }
      else
         strcat(Response, "SYSTEMBUSY:");
      break;
   case reqScanPoll:
      if ((ScanInitiate == TRUE) || (ScanStarted == TRUE) || (ScanRun == TRUE))
         sprintf(&Response[strlen(Response)], "SCANINPROGRESS:%d", PercentScan);
      else if (ScanComplete == TRUE) {
         strcat(Response, "SCANCOMPLETED:");
         for (int i = 0; i < DCP_NUMRESONANCES; i++) {
            sprintf(&Response[strlen(Response)], "%d,%d,",
                  (int) ResonanceFreqType[i], (int) ResonanceFreqs[i]);
         }
         scanStartFlag = FALSE;
         EnableWebIO = FALSE;
         IsIOConfigured = TRUE;
         DcxLogUserID = 0;
      }
      else if (ScanAbort == TRUE) {
         strcat(&Response[strlen(Response)], "SCANFAILED:");
         scanStartFlag = FALSE;
         EnableWebIO = FALSE;
         IsIOConfigured = TRUE;
         DcxLogUserID = 0;
      }

      webtimeOutScan = WEBSCAN_TIMEOUT;
      break;
   case reqScanAbort:
      strcat(&Response[strlen(Response)], "SCANABORTED:");
      webtimeOutScan = WEBSCAN_TIMEOUT;
      scanStartFlag = FALSE;
      scanAbortFlag = TRUE;
      EnableWebIO = FALSE;
      break;
   default:
      strcpy(Response, "OK");
      break;
   }
   paramStr = 0;
}

unsigned int WebSite::GetUserID(void)
/****************************************************************************************************************/
/*This function returns the currently logged in user ID for logging
 */
/****************************************************************************************************************/
{
   return DcxLogUserID;
}

BOOLEAN WebSite::CheckForWebTestCommand(void)
/****************************************************************************************************************/
/*This function gets called by state machine to determine
 * the test command status from website
 * @return: true if TEST is started from website else false
 */
/****************************************************************************************************************/
{
   return (mfTestFlag);
}

void WebSite::ResetTestFlag(void)
/****************************************************************************************************************/
/*This function gets called by state machine to reset
 * the test command status from website
 */
/****************************************************************************************************************/
{
   mfTestFlag = FALSE;
   EnableWebIO = FALSE;
}

BOOLEAN WebSite::CheckForWebScanCommand(void)
/****************************************************************************************************************/
/*This function gets called by state machine to determine
 * the SCAN command status from website
 * @return: true if SCAN is started from website else false
 */
/****************************************************************************************************************/
{
   return scanStartFlag;
}

BOOLEAN WebSite::CheckForWebScanAbortCommand(void)
/****************************************************************************************************************/
/*This function gets called by state machine to determine
 * the SCAN abort status from website
 * @return: true if SCAN is aborted from website else false
 */
/****************************************************************************************************************/
{
   return scanAbortFlag;
}

void WebSite::ResetScanFlags(void)
/****************************************************************************************************************/
/*This function gets called by state machine to reset
 *  the scan flags after SCAN is completed or aborted.
 */
/****************************************************************************************************************/
{
   scanStartFlag = FALSE;
   scanAbortFlag = FALSE;
   DcxLogUserID = 0;
}

BOOLEAN WebSite::CheckForDiagnosticFlag(void)
/****************************************************************************************************************/
/*This function gets called by state machine to determine
 * that whether the diagnostic is in progress.
 * @return: returns true if diagnostic is under progress
 */
/****************************************************************************************************************/
{
   return diagFlag;
}

BOOLEAN WebSite::CheckForWebReset(void)
/****************************************************************************************************************/
/*This function gets called by state machine to check
 *the reset command from website
 * @return: returns true if if reset is sent
 * by website
 */
/****************************************************************************************************************/
{
   return resetFlag;
}

void WebSite::ResetWebResetFlag(void)
/****************************************************************************************************************/
/*This function gets called by state machine to reset
 *the reset flag after Reset is done
 */
/****************************************************************************************************************/
{
   resetFlag = FALSE;
}

BOOLEAN WebSite::CheckForMFBoardTestFlag(void)
/****************************************************************************************************************/
/*This function gets called by state machine to check
 *whether DCP board is in test mode or not.
 */
/****************************************************************************************************************/
{
   return mfBoardTestFlag;
}

BOOLEAN WebSite::CheckForManufacturingFlag(void)
/****************************************************************************************************************/
/*This function gets called by ReceiveCommandHandler to check
 *whether the Manufacturing page is currently active and we need
 *to lock the LCD
 */
/****************************************************************************************************************/
{
   return mfFlag;
}

void WebSite::GetLastSucessfullHornScanData(char * paramStr, WebCommand cmd)
/****************************************************************************************************************/
/*This function gets called on load of hornsignature page
 *  from website.It fills the last successful horn scan
 *  result in Response buffer
 * @param cmd:parameter gets ignored
 * @param paramStr: pointer to possible param list(ignored)
 */
/****************************************************************************************************************/
{
   sprintf(&Response[strlen(Response)], "%s@PSFreqType:%d,%d@", "READY",
         (int) (ParaScan.FrequencyStart / SCANFREQFACTOR) + 1,
         (int) (ParaScan.FrequencyStop / SCANFREQFACTOR) + 1);
   if (ScanComplete == TRUE) {
      strcat(Response, "SCANCOMPLETED:");
      for (int i = 0; i < DCP_NUMRESONANCES; i++) {
         sprintf(&Response[strlen(Response)], "%d,%d,",
               (int) ResonanceFreqType[i], (int) ResonanceFreqs[i]);
      }
   }
   cmd = reqNone;
   paramStr = 0;
}

void WebSite::ProcessPSTestOnLoad(char * paramStr, WebCommand cmd)
/****************************************************************************************************************/
/*This function gets called during ps-test on load from website
 * and checks whether system is OK to run diagnostic
 * @param cmd:parameter gets ignored
 * @param paramStr: pointer to possible param list(ignored)
 */
/****************************************************************************************************************/
{
   sprintf(&Response[strlen(Response)], "%s@%d@%d@%d@%d@%d", "OK",
         (int) (((Midband + FREQ_MIN) * F_Factor) / 1000),
         (int) (((Midband + FREQ_MAX) * F_Factor) / 1000),
         (int) CalculateMemoryFreqinPercent(),
         (int) (((CurrentPreset.MemoryFreq + CurrentPreset.DigitalTune)
               * F_Factor) / 1000), (SINT32) CheckForCurrentFunction());
   cmd = reqNone;
   paramStr = 0;
}

void WebSite::ProcessMFSavesystemSetupCommand(char * param)
/****************************************************************************************************************/
/*
 * This function gets called on Save System Setup from
 * Manufacturing Page.
 * @param:String containing IP, Serial number,Frequency and powerLevel
 */
/****************************************************************************************************************/
{
   UINT32 currFreq;
   UINT8 currSystemtype;
   char tempVal[100];
   char * temp1;
   currFreq = EPromData.PSfrequency;
   currSystemtype = EPromData.Systemtype;

   char * temp = strstr(param, "freq:") + strlen("freq:");
   EPromData.PSfrequency = (FREQUENCY) atoi(temp);

   temp = strstr(param, "&systemtype:") + strlen("&systemtype:");
   EPromData.Systemtype = (atoi(temp)) - 1;

   temp = strstr(param, "&systemtypeedp:") + strlen("&systemtypeedp:");
   EPromData.EDP = (atoi(temp)) - 1;

   temp = strstr(param, "&pwr:") + strlen("&pwr:");
   EPromData.PSpower = (atoi(temp));

   temp = strstr(param, "&ipaddr:") + strlen("&ipaddr:");   //Points to IP Address Value
   temp1 = strstr(temp, "&snumber:");

   if (temp1 - temp < 100) {
      strncpy(tempVal, temp, temp1 - temp);
      sysConfig.systemIPStatic = IPStrtoUint(tempVal);
      sysConfig.systemIP = IPStrtoUint(tempVal);
      sysConfig.dhcpConfig.dns = IPStrtoUint(tempVal);   //DNS server is same
      temp = strstr(param, "&snumber:") + strlen("&snumber:");
      if (strlen(temp) < SERAILNUMBER_LEN)
         strcpy((char*) EPromData.SerialNo, temp);
   }
   //save power supply and frequency to MAC chip
   //save serial number to MAC chip
   ModifyEEPROMdata();
   if ((currFreq != EPromData.PSfrequency)) {
      UpdateMidBandAndFFactor();
      SetDefaultWeldPreset();
      SetDefaultAdvancedPreset();
      SetMaxMinValues();
      SetMinMaxScanParameter();
   }
   if ((currSystemtype != EPromData.Systemtype)) {
      IOTaskStop = TRUE;
      StopStateMachine();
      CurrentWorkNumber = 0;
      SetMaxMinValues();
      RegMaxMinUpdateFlag = true;
	  SetDefaultWeldPreset();
      for (int j = 0; j < MAX_WORK_NUMBER; j++) {
         SavePreset(j);
      }
      RegUpdateCmdFlag = true;
      ReceiveCommandHandler::SystemInfoReceivedFlag = FALSE;
      SetMinMaxScanParameter();
      UserIO::ResetUserIO();
      Log::ResetLog();
      StartStateMachine();
      IOTaskStop = FALSE;
   }
}


void WebSite::ProcessMFStartStopCommand(char * param)
/****************************************************************************************************************/
/*
 * This function gets called on S System Setup from
 * Manufacturing Page.
 * @param:String containing cycle parameters
 */
/****************************************************************************************************************/
{

   char * temp = strstr(param, "cyctime:") + strlen("cyctime:");
   TempCurrentCycleParam.totalCycleTime = atoi(temp);

   temp = strstr(param, "&sontime:") + strlen("&sontime:");
   TempCurrentCycleParam.sonicsOnTimePerWeldCycle = atoi(temp);

   temp = strstr(param, "&sofftime:") + strlen("&sofftime:");
   TempCurrentCycleParam.sonicsOffTimePerWeldCycle = atoi(temp);

   temp = strstr(param, "&weldamp:") + strlen("&weldamp:");
   TempCurrentCycleParam.Amplitude = atoi(temp);

   temp = strstr(param, "&alarmrst:") + strlen("&alarmrst:");
   TempCurrentCycleParam.stopOnAlarm = atoi(temp);
}

BOOLEAN GetCurrentStateOnManufacturing(void)
/****************************************************************************************************************/
/*This function determines the current state of system.i.e busy etc
 * This is sent to Manufacturing page
 */
/****************************************************************************************************************/
{
   BOOLEAN retVal = FALSE;
   if (UserIO::GetReadyFlag() || MFCycleFlag || UserIO::GetGenAlarmFlag()
         || WebSite::mfTestFlag) retVal = TRUE;
   return retVal;
}

void WebSite::ProcessManufacturingCmd(char * paramStr, WebCommand cmd)
/****************************************************************************************************************/
/*This function gets called during Manufacturing web Page
 * is open on website. Its under implementation.
 */
/****************************************************************************************************************/
{
   int wldrCnt = 0, indx;
   char * currStat = 0, *TempStr = 0;
   WelderInfoParam * wInfo = 0;
   BOOLEAN PresetChanged = FALSE;
   mfTimeOut = MF_TIMEOUT;
   int index = 0;
   switch (EPromData.PSfrequency) {
   case FREQ_20KHZ:
      index = 1;
      break;
   case FREQ_30KHZ:
      index = 2;
      break;
   case FREQ_40KHZ:
      index = 3;
      break;
   }
   switch (cmd) {
   case reqSearchWelders:
      wldrCnt = WelderSearchTask::thiPtr->GetWelderCount();
      if (wldrCnt > 0) {
         wInfo = new WelderInfoParam[wldrCnt];
         WelderSearchTask::thiPtr->GetWeldersInformation(wInfo);
         for (indx = 0; indx < wldrCnt; indx++)
            sprintf(&Response[strlen(Response)], "%s/%s@",
                  wInfo[indx].ipaddr.ToString(),
                  wInfo[indx].WelderInfo.SeriallNumber);
      }
      else
         strcpy(Response, "No welder found");
      break;
   case reqMFOnLoad:
      if (GetCurrentStateOnManufacturing()) {
         mfFlag = TRUE;
         EnableWebIO = TRUE;
         currStat = "OK";
      }
      else
         currStat = "BUSY";

      sprintf(&Response[strlen(Response)], "%s@"
            "ipaddr:D,%s@"
            "snumber:D,%s@"
            "cyctime:D,%d,%d,%d@"
            "sonicontime:D,%d,%d,%d@"
            "sonicofftime:D,%d,%d,%d@"
            "amplitude:D,%d,%d,%d@"
            "stoponalarm:CB,%d@"
            "freqval:S,%d@"
            "pwrval:S,%d@"
            "ncycles:S,%d@"
            "nhrs:S,%d@"
            "crsonicsontime:S,%d@"
            "nalarms:S,%d@"
            "crremainingtime:S,%d@"
            "stat:RB,%d@"
            "freqpwroption:DDV,%d@"
            "SetupPwrWatt:D,%d@"
            "systemtypeoption:DDV,%d@"
            "systemtypeedp:DDV,%d@", currStat,
            CyGOS::GetIPAddressStr(interface), EPromData.SerialNo,
            (int) CurrentCycleParam.totalCycleTime,
            (int) MinCycleParam.totalCycleTime,
            (int) MaxCycleParam.totalCycleTime,
            (int) CurrentCycleParam.sonicsOnTimePerWeldCycle,
            (int) MinCycleParam.sonicsOnTimePerWeldCycle,
            (int) MaxCycleParam.sonicsOnTimePerWeldCycle,
            (int) CurrentCycleParam.sonicsOffTimePerWeldCycle,
            (int) MinCycleParam.sonicsOffTimePerWeldCycle,
            (int) MaxCycleParam.sonicsOffTimePerWeldCycle,
            (int) CurrentCycleParam.Amplitude, (int) MinCycleParam.Amplitude,
            (int) MaxCycleParam.Amplitude, (int) CurrentCycleParam.stopOnAlarm,
            (int) ((MFcycleResults.resultantFrequency * F_Factor) / 1000),
            (int) ((MFcycleResults.resultantPower * 100) / ADC11BITVALUE),
            (int) MFcycleResults.nCycles,
            (int) (SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)),
            (int) (SCALEDTIMETOWEBSITE(MFcycleResults.sonicsOnTime)),
            (int) MFcycleResults.nAlarms,
            (int) (CurrentCycleParam.totalCycleTime
                  - SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)),
            (int) MFcycleResults.cycleStatus, index, (int) EPromData.PSpower,
            (int) (EPromData.Systemtype + 1), (int) (EPromData.EDP + 1));
      break;
   case reqRestoreDefaults:
      SetManFDefaultFromWeb();
      strcpy(Response, "OK");
      break;
   case reqClearAlarmLogMF:
      Log::ResetLog(clearAlarm);
      strcpy(Response, "OK");
      break;
   case reqClearEventLogMF:
      Log::ResetLog(clearEvent);
      strcpy(Response, "OK");
      break;
   case reqResetAlarmCounter:
      ResetManFErrorCounter();
      strcpy(Response, "OK");
      break;
   case reqStartStopCycle:
      if (CurrentCycleParam.MfTest) {
         MFcycleResults.cycleStatus = statStopped;
         SonicsOff();
         SonicsOnCounter = 0;
         SonicsOffCounter = 0;
         CurrentCycleParam.MfTest = FALSE;
         StartMFCycle = MFCycleFlag = CurrentCycleParam.MfTest;
         strcpy(Response, "STOPPED");
         CurrentCycleParam.CRC = ByteCRC((UINT8*) &CurrentCycleParam,
               sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
         regAmplitudeUpdateFlag = TRUE;
      }
      else {
         if (UserIO::GetReadyFlag()) {
            ProcessMFStartStopCommand(paramStr);
            if (CheckForCycleParameterChange()) {
               memset(&MFcycleResults, 0, sizeof(MFcycleResults));
               if ((CurrentCycleParam.sonicsOnTimePerWeldCycle == 0)
                     || (CurrentCycleParam.sonicsOffTimePerWeldCycle == 0))
                  TempSonicsOnTimePerWeldCycle =
                        CurrentCycleParam.totalCycleTime;
               else
                  TempSonicsOnTimePerWeldCycle =
                        CurrentCycleParam.sonicsOnTimePerWeldCycle;
            }
            CurrentCycleParam.MfTest = TRUE;
            StartMFCycle = MFCycleFlag = CurrentCycleParam.MfTest;
            strcpy(Response, "STARTED");
            CurrentCycleParam.CRC = ByteCRC((UINT8*) &CurrentCycleParam,
                  sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
            regAmplitudeUpdateFlag = TRUE;
         }
         else
            strcpy(Response, "BUSY");
      }
      break;
   case reqSaveSysSetup:
      ProcessMFSavesystemSetupCommand(paramStr);
      strcpy(Response, "OK");
      break;
   case reqMFPoll:
      if (GetCurrentStateOnManufacturing()) {
         mfFlag = TRUE;
         EnableWebIO = TRUE;
         currStat = "OK";
      }
      else
         currStat = "BUSY";

      sprintf(Response, "%s@"
            "freqval:S,%d@"
            "pwrval:S,%d@"
            "ncycles:S,%d@"
            "nhrs:S,%d@"
            "crsonicsontime:S,%d@"
            "nalarms:S,%d@"
            "crremainingtime:S,%d@"
            "stat:RB,%d@", currStat,
            (int) ((MFcycleResults.resultantFrequency * F_Factor) / 1000),
            (int) ((MFcycleResults.resultantPower * 100) / ADC11BITVALUE),
            (int) MFcycleResults.nCycles,
            (int) (SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)),
            (int) (SCALEDTIMETOWEBSITE(MFcycleResults.sonicsOnTime)),
            (int) MFcycleResults.nAlarms,
            (int) (CurrentCycleParam.totalCycleTime
                  - SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)),
            (int) MFcycleResults.cycleStatus);
      break;
   case regMFTestStart:
      if (UserIO::GetReadyFlag()) {
         strcpy(Response, "TESTSTARTED");
         mfTestFlag = TRUE;
         webtimeOutTest = WEBTEST_TIMEOUT;
         DcxLogUserID = DcxWebUserID;
      }
      else
         strcpy(Response, "SYSTEMBUSY");
      break;
   case regMFTestStop:
      strcpy(Response, "TESTSTOPPED");
      DcxLogUserID = 0;
      mfTestFlag = FALSE;
      break;
   case reqMFTestPoll:
      webtimeOutTest = MF_TIMEOUT;
      sprintf(Response, "MFTESTPOLLRESPONSE:%d", CheckForCurrentFunction());
      break;
   case reqSpecialEnter:
      TempStr = strstr(paramStr, "&SpecialPassword=") + 1;
      TempStr = (strchr(TempStr, '=') + 1);
      if (strcmp(TempStr, SPECIALPASSWORD) == 0) {
         EPromData.EnergyPowerSpecial = TRUE;
         EPromData.SIGSpecial = FALSE;
         CurrentPreset.SIG = FALSE;
         PresetChanged = TRUE;
         ColdStart();

         sprintf(Response, "Special %s has been Activated.", SPECIALPASSWORD);
      }
      else if (strcmp(TempStr, SIG_SPECIAL) == 0) {
         EPromData.EnergyPowerSpecial = FALSE;
         EPromData.SIGSpecial = TRUE;
         CurrentPreset.SIG = TRUE;
         PresetChanged = TRUE;
         ColdStart();

         sprintf(Response, "Special %s has been Activated.", SIG_SPECIAL);
      }
      else if (strcmp(TempStr, CLEAR_SPECIALS) == 0) {
         EPromData.EnergyPowerSpecial = FALSE;
         EPromData.SIGSpecial = FALSE;
         CurrentPreset.SIG = FALSE;
         PresetChanged = TRUE;
         ColdStart();

         sprintf(Response, "All specials have been deactivated");
      }
      else
         strcpy(Response, "Wrong Password.");
      if (PresetChanged) {
         ModifyEEPROMdata();
         SavePreset(0);
         RegUpdateCmdFlag = TRUE;

      }
      break;
   case reqNone:
      if (GetCurrentStateOnManufacturing()) {
         strcpy(Response, "OK");
         mfFlag = TRUE;
         EnableWebIO = TRUE;
      }
      else
         strcpy(Response, "BUSY");
      break;
   default:
      strcpy(Response, "ErrCommand");
   }
   if (wInfo) delete wInfo;
}

BOOLEAN WebSite::CheckForCycleParameterChange(void)
/****************************************************************************************************************/
/*This function is called from ProcessManufacturingCommand function on Start/Stop
 * Cycling button click.
 * @retVal: return TRUE if cycle parameters sent from the website
 * are not the same in FRAM else returns FALSE.
 */
/****************************************************************************************************************/
{
   BOOLEAN result = FALSE;

   if (CurrentCycleParam.totalCycleTime
         != TempCurrentCycleParam.totalCycleTime) {
      CurrentCycleParam.totalCycleTime = TempCurrentCycleParam.totalCycleTime;
      result = TRUE;
   }

   if (CurrentCycleParam.sonicsOnTimePerWeldCycle
         != TempCurrentCycleParam.sonicsOnTimePerWeldCycle) {
      CurrentCycleParam.sonicsOnTimePerWeldCycle =
            TempCurrentCycleParam.sonicsOnTimePerWeldCycle;
      result = TRUE;
   }

   if (CurrentCycleParam.sonicsOffTimePerWeldCycle
         != TempCurrentCycleParam.sonicsOffTimePerWeldCycle) {
      CurrentCycleParam.sonicsOffTimePerWeldCycle =
            TempCurrentCycleParam.sonicsOffTimePerWeldCycle;
      result = TRUE;
   }

   if (CurrentCycleParam.Amplitude != TempCurrentCycleParam.Amplitude) {
      CurrentCycleParam.Amplitude = TempCurrentCycleParam.Amplitude;
      result = TRUE;
   }

   if (CurrentCycleParam.stopOnAlarm != TempCurrentCycleParam.stopOnAlarm) {
      CurrentCycleParam.stopOnAlarm = TempCurrentCycleParam.stopOnAlarm;
      result = TRUE;
   }

   if (MFcycleResults.cycleStatus != statStopped) result = TRUE;

   return result;
}

void WebSite::ProcessMFBoardTestPollResponse(void)
/****************************************************************************************************************/
/*This function gets called during Board Test on polling
 * It fills the reponse buffer with current input states
 * from instrumentation board including phase sign.
 */
/****************************************************************************************************************/
{
   UINT8 inputs = fpga_input;
   inputs = ~inputs;
   //maintain the sequence
   sprintf(&Response[strlen(Response)], "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
         (inputs & OVL_FUNCTION) ? 1 : 0, (inputs & OL_TEMPERATURE) ? 1 : 0,
         (inputs & OL_POWER) ? 1 : 0, (inputs & OL_VOLTAGE) ? 1 : 0,
         (inputs & OL_CURRENT) ? 1 : 0, ((~(inputs) & FPGA_SIGN) >> 2) ? 1 : 0,
         (int) PowerInput, (int) InputTemperature, (int) ActualAmplitude,
         (int) CurrentInput, (int) PhaseInput);
}

void WebSite::ProcessMFBoardTestRefreshCmd(char * param)
/****************************************************************************************************************/
/*This function gets called during Board Test on refresh command
 * @param: Pointer to parameters e.g digital outputs from fpga to
 * instrumentation board and test frequency.
 */
/****************************************************************************************************************/
{
   char * temp;
   UINT8 digOutput;
   UINT8 maskSet[] = { OLRST_BIT, DPSSTOP_BIT, PHASEA_BIT, PHASEB_BIT };
   char * idArr[] = { "&olrst:", "&stop:", "&phaseA:", "&phaseB:" };

   temp = strstr(param, "testFreq:") + strlen("testFreq:");
   //test frequency range gets validated in javscript code itself
   //so it doesnot need to be validated here
   BoardTestFrequency = atoi(temp) * SCANFREQFACTOR;
   BoardTestOutput |= TEST_BIT;
   for (int indx = 0; indx < 4; indx++) {
      temp = strstr(temp, idArr[indx]) + strlen(idArr[indx]);
      digOutput = atoi(temp);
      (digOutput) ? (BoardTestOutput |= maskSet[indx]) : (BoardTestOutput &=
                          ~maskSet[indx]);
   }
}

void WebSite::ProcessManufacturingBoardTestCmd(char * paramStr, WebCommand cmd)
/****************************************************************************************************************/
/*This function gets called during Board Test from website
 * @param cmd:oard test command from website e.g login, poll inputs,
 * refresh outputs etc.
 * @param paramStr: pointer to parameters. ignored in all cases
 * except refresh command. On refresh it monis to the digital outputs
 * and test frequecy values sent from website
 */
/****************************************************************************************************************/
{
   char * mfpassword = strstr(paramStr, "password:") + strlen("password:");
   switch (cmd) {
   case reqBoardTestLogin:
      if (!strcmp(mfpassword, MFBOARDTESTPASSWORD)) {
         EnableWebIO = TRUE;
         mfBoardTestFlag = TRUE;
         mfBoardTestTimeOut = MFBOARDTEST_TIMEOUT;
         strcpy(Response, "OK");
      }
      else
         strcpy(Response, "NOTOK");
      break;
   case reqBoardTestRefresh:
      EnableWebIO = TRUE;
      mfBoardTestFlag = TRUE;
      mfBoardTestTimeOut = MFBOARDTEST_TIMEOUT;
      ProcessMFBoardTestRefreshCmd(paramStr);
      strcpy(Response, "OK");
      break;
   case reqBoardTestPoll:
      EnableWebIO = TRUE;
      mfBoardTestFlag = TRUE;
      mfBoardTestTimeOut = MFBOARDTEST_TIMEOUT;
      strcpy(Response, "OK@");
      ProcessMFBoardTestPollResponse();
      break;
   default:
      strcpy(Response, "NOTOK");
   };
}

void WebSite::Tick(int callbackus)
/****************************************************************************************************************/
/* This function gets called every RTOS tick.
 * it determines whether the SCAN, TEST or diagnostic
 * commands from website has been timed out in some accidental
 * case(e.g ethernet cable removed, website window closed etc).
 * It disables the corresponding flags in that case.
 * @callbackus: RTOS tick in microseconds
 */
/****************************************************************************************************************/
{
   if (scanStartFlag) {
      webtimeOutScan -= callbackus;
      if (webtimeOutScan <= 0) {
         scanStartFlag = FALSE;
         scanAbortFlag = TRUE;
         EnableWebIO = FALSE;
         IsIOConfigured = TRUE;
         DcxLogUserID = 0;
      }
   }

   if (mfTestFlag) {
      webtimeOutTest -= callbackus;
      if (webtimeOutTest <= 0) {
         EnableWebIO = FALSE;
         IsIOConfigured = TRUE;
         DcxLogUserID = 0;
         mfTestFlag = FALSE;
      }
   }

   if (diagFlag) {
      diagnosticTimeOut -= callbackus;
      if (diagnosticTimeOut <= 0) {
         diagFlag = FALSE;
         EnableWebIO = FALSE;
         IsIOConfigured = TRUE;
         DcxLogUserID = 0;
      }
   }
   if (DiagStartFlag) {
      StartDiagTimeOut -= callbackus;
      if (StartDiagTimeOut <= 0) {
         DiagStartFlag = FALSE;
         RestoreOutPutStatus();
      }
   }

   if (mfBoardTestFlag) {
      mfBoardTestTimeOut -= callbackus;
      if (mfBoardTestTimeOut <= 0) {
         EnableWebIO = FALSE;
         IsIOConfigured = TRUE;
         mfBoardTestFlag = FALSE;
         BoardTestFrequency = MIN_BOARD_TESTFREQUENCY;
         BoardTestOutput = 0;
      }
   }

   if (mfFlag) {
      mfTimeOut -= callbackus;
      if (mfTimeOut <= 0) {
         mfFlag = FALSE;
         EnableWebIO = MFCycleFlag;
         if (!EnableWebIO) IsIOConfigured = TRUE;
      }
   }
}
