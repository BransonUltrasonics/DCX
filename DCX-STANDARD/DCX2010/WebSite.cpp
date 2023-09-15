/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/WebSite.cpp_v   1.38.1.4   02 Jul 2014 09:15:44   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/WebSite.cpp_v  $
 * 
 *    Rev 1.38.1.4   02 Jul 2014 09:15:44   amaurya
 * Removed Amplitude Decay function.
 * 
 *    Rev 1.38.1.3   30 Jun 2014 10:04:36   amaurya
 * Removed Amplitude decay from digital I/O.
 * 
 *    Rev 1.38.1.2   30 Apr 2014 06:20:44   amaurya
 * Added change for Digital Tune
 * 
 *    Rev 1.38.1.1   12 Mar 2014 07:52:30   rjamloki
 * Sequencing Error Fix, Generic OL bit taken care, Time field in logs separated by colon.
 * 
 *    Rev 1.38.1.0   09 Sep 2013 08:59:58   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.38   01 Mar 2013 08:17:46   amaurya
 * Added Energy Range In 250J.
 * 
 *    Rev 1.37   12 Feb 2013 00:40:56   amaurya
 * Fixwd issue of resetting alarms under some conditions.
 * 
 *    Rev 1.36   07 Feb 2013 05:16:52   amaurya
 * Fixed PS Frequency issue on maufacturing page.
 * 
 *    Rev 1.35   05 Feb 2013 06:29:08   amaurya
 * Fixed Energy Special disable on power down issue.
 * 
 *    Rev 1.34   15 Jan 2013 04:43:54   amaurya
 * Energy Special.
 * 
 *    Rev 1.32.1.0.1.9   09 Nov 2012 01:21:38   amaurya
 * Fixed Bar Graph wrap issue.
 * 
 *    Rev 1.32.1.0.1.8   01 Nov 2012 05:15:56   amaurya
 * Added multiple resonance marker.
 * 
 *    Rev 1.32.1.0.1.7   23 Oct 2012 07:51:24   amaurya
 * Fixed Frequency Step to 1.
 * 
 *    Rev 1.32.1.0.1.6   18 Oct 2012 10:06:04   ygupta
 * Fixed Horn Scan Graph issue for 30 Khz.
 * 
 *    Rev 1.32.1.0.1.5   12 Oct 2012 10:05:46   ygupta
 * Fixed Reset Overload Clear Issue.
 * 
 *    Rev 1.32.1.0.1.4   09 Oct 2012 09:57:44   ygupta
 * Fixed Scaling Issue.
 * 
 *    Rev 1.32.1.0.1.3   09 Oct 2012 07:08:36   ygupta
 * Fixed scaling issue on 30 kHz horn scan.
 * 
 *    Rev 1.32.1.0.1.2   05 Oct 2012 07:36:00   ygupta
 * Added resonance marker in Horn Scan graph.
 * 
 *    Rev 1.32.1.0.1.1   07 Sep 2012 08:54:56   amaurya
 * Fixed resonance frequency issue of Horn Scan.
 * 
 *    Rev 1.32.1.0.1.0   23 Aug 2012 11:34:44   amaurya
 * Implemented S & V Graph Plot.
 * 
 *    Rev 1.32.1.0   30 Mar 2012 06:43:22   PDwivedi
 * Table driven CRC routine for all CRC calculations
 * 
 *    Rev 1.32   21 Oct 2011 11:05:38   Builder
 * Release V1.2 with Cable detect and inconsistency fixes
 * 
 *    Rev 1.31   17 Oct 2011 07:46:02   RatneshA
 * Added low force event from IO config page
 * 
 *    Rev 1.30   16 Sep 2011 08:05:20   RatneshA
 * changes regarding new IP setup page(ver1.0R)
 * 
 *    Rev 1.29   06 Sep 2011 14:00:08   ygupta
 * Called ReadPowerOnSecCounter to get poweron Hours.
 * 
 *    Rev 1.28   28 Aug 2011 11:42:28   ygupta
 * Ver1.01M
 * 
 *    Rev 1.27   16 Aug 2011 08:30:40   pdwivedi
 * code cleanup
 * 
 *    Rev 1.26   11 Aug 2011 08:57:36   pdwivedi
 * Added test button on manufacturing.
 * 
 *    Rev 1.25   05 Aug 2011 10:35:58   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.23   02 Aug 2011 07:40:20   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.22   22 Jul 2011 16:54:26   tayars
 * Added New Control Loops
 * 
 *    Rev 1.21   18 Jul 2011 09:26:12   pdwivedi
 * Added changes for serial buffer size.
 * 
 *    Rev 1.20   15 Jul 2011 09:49:50   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.19   21 Jun 2011 11:08:10   PDwivedi
 * Fixes after mongle horde round1
 * 
 *    Rev 1.18   13 Jun 2011 09:16:40   PDwivedi
 * Default step size is doubled for 30Khz PS
 * 
 *    Rev 1.17   03 Jun 2011 05:53:26   PDwivedi
 * Frequency Offsert range fixed for web
 * 
 *    Rev 1.16   24 May 2011 04:20:18   ygupta
 * Startup digital output fix
 * 
 *    Rev 1.15   09 May 2011 08:35:30   ygupta
 * Timeouts increased
 * 
 *    Rev 1.13   27 Apr 2011 10:48:38   PDwivedi
 * minor bug fixes, system info page GUI updated for max serail number,Debug re enabled
 * 
 *    Rev 1.12   26 Apr 2011 13:19:04   PDwivedi
 * System box in Sytem Information page now displays LCD code version and CRC
 * 
 *    Rev 1.11   25 Apr 2011 08:45:48   PDwivedi
 * Changes after MAC chip addition
 * 
 *    Rev 1.10   25 Apr 2011 06:31:44   PDwivedi
 * Disable Diagnostic IO/PS page in case of Manufacturing Cycle
 * 
 *    Rev 1.9   21 Apr 2011 11:08:32   PDwivedi
 * modified Ver1.0T for tracker issues
 * 
 *    Rev 1.8   20 Apr 2011 11:38:50   ygupta
 * Added changes for tracker issues.
 * 
 *    Rev 1.7   19 Apr 2011 10:50:26   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.6   14 Apr 2011 06:23:54   PDwivedi
 * Added changes for IP Setup issue and added comments.
 * 
 *    Rev 1.5   04 Apr 2011 10:58:20   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.4   02 Mar 2011 08:18:12   PDwivedi
 * Added Manufacturing Board Test Webpage support.
 * 
 *    Rev 1.3   14 Jan 2011 07:55:46   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.2   16 Dec 2010 08:17:42   PDwivedi
 * Added changes for Website Code Clean Up and Compressed file download from serial.
 * 
 *    Rev 1.1   14 Dec 2010 11:15:16   PDwivedi
 * Added changes for WebSite support.
Revision 1.1  2010/12/08 16:24:14  Noor
code to test http server problem.

 * 
 *    Rev 1.0   29 Oct 2010 10:40:48   ygupta
 * Initial revision.

******************************************************************************/

#include "WebSite.h"
#include "FileSystem.h"
#include "string.h"
#include "SysConfig.h"
#include "Globals.h"
#include "stdio.h"
#include "stdlib.h"
#include "FUNC.h"
#include "Version.h"
#include "LOG.h"
#include "RTOS.h"
#include "StateMachineDrv.h"
#include "UserIO.h"
#include "TYP_DEF.h"
#include "UploadFirmwareTask.h"
#include "WelderSearchTask.h"
#include "Board.h"
#include "Flash.h"
#include "CRC.h"
#include "ReceiveCommandHandler.h"
#include "DebugNew.h"

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

int WebSite::port;//port this website is running on
int WebSite::interface;//ethernet interface this website is running on

BOOLEAN WebSite::scanStartFlag;
BOOLEAN WebSite::scanAbortFlag;
BOOLEAN WebSite::diagFlag;
BOOLEAN WebSite::diagStartFlag;
BOOLEAN WebSite::resetFlag;
BOOLEAN WebSite::mfBoardTestFlag;
BOOLEAN WebSite::mfFlag;
BOOLEAN WebSite::mfTestFlag;

extern BOOLEAN AlarmFlag;

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

extern DIGITALIOMAP  OutPtr[NUM_FUNC_OUT + 1];
//disable debug
//#define printf
//#define pprintf


//Users with random IDs,
//TODO: support for adding users and modifying their rights is not there
Users USERS[]= {
		{"32923646",Branson},
		{"287646",Manufacturing1},
		{"26273",Manufacturing2},
};


//Digital input function table to be shown on IOConfig page for function selection
//put 0 in high ID if function is only active low.put 0 in low ID if function is only active high
WebDigitalIOFunctionTable webDigIOfuncTableIn[] = {
		{0,0,0},
		{100, EXTRESET_ACTIVEHIGH, EXTRESET_ACTIVELOW},//ExtReset
		{101, EXTSEEK_ACTIVEHIGH, EXTSEEK_ACTIVELOW,},//External Seek
		{102, EXTSTART_ACTIVEHIGH, EXTSTART_ACTIVELOW},//ExtStart
		{103, EXTTEST_ACTIVEHIGH, EXTTEST_ACTIVELOW},//ExtTest
		{104, MEMCLEAR_ACTIVEHIGH, MEMCLEAR_ACTIVELOW},//memClear
		{105, STARTHORNSCAN_ACTIVEHIGH, STARTHORNSCAN_ACTIVELOW},//ExtScan
		{106, LOCKOUT_ACTIVEHIGH, LOCKOUT_ACTIVELOW},//LockOutChangeLCD
		{107, CABLEDETECT_ACTIVEHIGH, 0},//Input signal to prevent accidental sonics
};

//Digital output function table to be shown on IOConfig page for function selection
//put 0 in high ID if function is only active low.put 0 in low ID if function is only active high
WebDigitalIOFunctionTable webDigIOfuncTableOut[] = {
		{0,0,0},
		{200, READY_ACTIVEHIGH, READY_ACTIVELOW},//ReadyFlg
		{201, SONICACTIVE_ACTIVEHIGH, SONICACTIVE_ACTIVELOW},//WeldOn
		{202, GENALARM_ACTIVEHIGH, GENALARM_ACTIVELOW},//GenAlarm
		{203, OVERLOADALARM_ACTIVEHIGH, OVERLOADALARM_ACTIVEHIGH},//Overload alarm
		{204, SEEKOUT_ACTIVEHIGH, SEEKOUT_ACTIVELOW},//CycleOK
};

//Analog input function table to be shown on IOConfig page for function selection
WebAnalogIOFunctionTable webAnaIOfuncTableIn[] = {
		{0,0,0},
		{300, FREQ_CONTROL,0},//freqin
		{301, AMPL_CONTROL,0},//amplitudein
		{302, ENERGY_CONTROL_RANGE1,0},//Energyin range 250
		{303, ENERGY_CONTROL_RANGE2,0},//Energyin range 1K
		{304, ENERGY_CONTROL_RANGE3,0},//Energyin range 10K
};

//Analog output function table to be shown on IOConfig page for function selection
WebAnalogIOFunctionTable webAnaIOfuncTableOut[] = {
		{0,0,0},
		{400, POWER_OUT,0},//powerout
		{401, AMPL_OUT,0},//amplitude out
//		{402, MEMFREQ_OUT},//memoryOut
		{403, FREQ_OUT,0},//frequencyOut
		{404, ENERGY_OUT,0},//EnergyOut
		{405, POWER_OUT400W,400},
		{406, POWER_OUT750W,750},//406:S,Power Out-750 Watts
		{407, POWER_OUT800W,800},//407:S,Power Out-800 Watts
		{408, POWER_OUT1100W,1100},//408:S,Power Out-1100 Watts
		{409, POWER_OUT1250W,1250},//409:S,Power Out-1250 Watts
		{410, POWER_OUT1500W,1500},//410:S,Power Out-1500 Watts
		{411, POWER_OUT1600W,1600},//411:S,Power Out-1600 Watts
		{412, POWER_OUT2200W,2200},//412:S,Power Out-2200 Watts
		{413, POWER_OUT2400W,2400},//413:S,Power Out-2400 Watts
		{414, POWER_OUT2500W,2500},//414:S,Power Out-2500 Watts
		{415, POWER_OUT3000W,3000},//415:S,Power Out-3000 Watts
		{416, POWER_OUT3300W,3300},//416:S,Power Out-3300 Watts
		{417, POWER_OUT4000W,4000},//417:S,Power Out-4000 Watts
};

/*
 * Website command handlers array
 */
webSiteCommand webCommandHandlers[] =
{
		{0},
		{WebSite::GetIPSetupVal},//1
		{WebSite::SetLanguageAndUserID},//2
		{WebSite::GetIOConfigurationVal},//3
		{WebSite::GetModifyPrestVal},//4
		{WebSite::GetModifyPrestAdvancedVal},//5
		{WebSite::GetDiagnosticDataVal},//6
		{WebSite::GetSystemInformationVal},//7
		{WebSite::UpdateIPSetup},//8
		{WebSite::UpdateIOConfiguration},//9
		{WebSite::ProcessDiagnosticIOCommand},//10
		{WebSite::UpdatePreset},//11
		{WebSite::UpdatePresetAdvanced},//12
		{WebSite::ProcessTestCommand},//13
		{WebSite::ProcessScanCommand},//14
		{WebSite::GetLogData},//15
		{WebSite::ProcessResetCommand},//16
		{WebSite::GetLastSucessfullHornScanData},//17
		{WebSite::ProcessPSTestOnLoad},//18
		{WebSite::ProcessManufacturingCmd},//19
		{WebSite::ProcessManufacturingBoardTestCmd},//20
		{WebSite::ViewWeldGraphGetDatahandeler }, //21        New Weld-Graph Handler
		{WebSite::ViewGraphHornSignatureGetDatahandeler}//22
};

/*Function validates the logged in user ID
@param userID:  The unique ID of DCP Event.
@return: Err user if the ID doesn't math with any in USERS array
		else return user's privilege
*/
UserType ValidateUser(char * userID)
{
	BOOLEAN ExitFlag = FALSE;
	int size = sizeof(USERS) / sizeof(Users);
	UserType userType = User;
	for(int i = 0; ((i < size) && (ExitFlag == FALSE)); i++)
	{
		if(!strcmp(userID, USERS[i].UserID))
		{
			userType = USERS[i].User;
			ExitFlag = TRUE;
		}
	}
	return userType;
}

/*Function return the type of file requested from website
 @param fileName: The fileNam with extension whose type is to be determined
 @return: the type of file image.gtml or gif etc.
 */
int GetContentType(char * fileName)
{
	char * ext = strchr(fileName, '.');
	char * ext1 = ext;
	while(ext)
	{
		ext1 = ext;
		ext = strchr(ext + 1, '.');
	}
	int ret = texthtml;
	if(!strcmp(ext1,".css"))
		ret = textcss;
	else if(!strcmp(ext1,".gif"))
		ret = imagegif;
	else if(!strcmp(ext1,".jpg"))
			ret = imagejpg;
	else if(!strcmp(ext1,".png"))
			ret = imagepng;
	else if(!strcmp(ext1,".js"))
	    ret = textjs;

	return ret;
}

/*Function converts the IP in dotted string format(e.g XX.XX.XX.XX) into integer format
 @param ipStr: The fileNam with extension whose type is to be determined
 @return: IP in integer format
 */
unsigned int IPStrtoUint(char * ipStr)
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


/*
 Function should be called once at powerup. It assigns the handler
functions for HTTP GET and POST requests coming from website(HTTP Client on PC)
 to HTTPServer. HTTPServer calls these handler functions on receiving HTTP GET or POST requests.
@param intf: The index of Ethernet module of processor
@param srvrPort: The HTTP port this website belongs to
 */
void WebSite::InitWebSite(int intf, int srvrPort)
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

/*function gets called on every POST request
 * it opens the Language file and  copy the file contents
 *  in language buffer if the buffer is not already initialized
 *  Function will be handy when we will deal with more languages
 *  @param: The language in which language buffer is to be initialized
 */
void WebSite::SetLanguageFileBuffer(Language lang)
{

	File * file = 0;
	int len = 0;
	if(!languageBuff)
	{
		CyGOS::FileOpen("A:\\ENGLISH.lan", O_RDONLY, &file);
		if (file)
		{
			len = CyGOS::FileLength(file);
			languageBuff = new char[len + 1];
			if (CyGOS::FileRead(file, languageBuff, &len) == 0)
			{
				CyGOS::FileClose(file);
			}
			languageBuff[len] = 0;
		}
	}
	lang = English;
}


/*This function is called by HTTPServer task on receiving POST request
@param postData : The data submitted by a web form in a predefined
 standard format.
@param uri : Identifier of resource.  It can be an HTML file or some
 other data specific to the page on website.
@header: Object which will be filled with response length, response type etc.
 @return: returns true in case of valid request. otherwise returns false
*/
BOOLEAN WebSite:: WebPostHandler(char * postData, char * uri, HTTPReplyHeader & header)
{
	//pprintf("\n uri POST = %s", uri);
	BOOLEAN retVal = TRUE;
	BOOLEAN htmlRequest = (strstr(uri,".html") != 0);//see if its html a file request
	BOOLEAN downLoadRequest = (strstr(uri,".logdownload") != 0);//see if its a download request
	BOOLEAN uploadRequest = (strstr(uri,".dcxfileupload") != 0);//see if its a upload request
	char * tempStr = 0, * res = resDefault;
	int langID = 0;
	char fileName[50];
	File * file = 0;
	UserType userType;
	char * UID = 0;
	int content_length = 0;
	header.contentLength = strlen(res);
	header.reqType = reqData;
	fileName[0] = 0;
	if(downLoadRequest)//see if its a download request
	{
		res = DownLoadRequest(uri, header.downloadFileName);
		content_length = strlen(res);
		header.reqType = reqFileDownload;
	}
	else if(uploadRequest)
	{
		pprintf("\n requested file upload");
		res = HandleInitiateUpload();
		content_length = strlen(res);
		header.reqType = reqFileUpload;
	}
	else if(htmlRequest)
	{
		header.reqType = reqFile;
		tempStr = strstr(postData, "lang=");
		langID = atoi(strchr(tempStr , '=') + 1);
		SetLanguageFileBuffer(Language(langID));
		tempStr = strstr(postData, "&userid1=");
		tempStr = strchr(tempStr , '=') + 1;
		userType = ValidateUser(tempStr);
		UID = tempStr;
		DcxWebUserID = atoi(UID);
		if(userType == ErrUser)
			retVal = FALSE;
		else
		{
			strcpy(fileName, "A:\\");
			if(strlen(uri) > MAXFILELEN)
			{
				Log::WriteEventLog(POST_FREQ, (short)strlen(uri), 0, 0, false);
				pprintf("\n Post File Name Too Long\n");
				pprintf("\n fileName = %s", uri);
				return FALSE;
			}
			strcat(fileName, uri);
			CyGOS::FileOpen(fileName, O_RDONLY, &file);
			retVal = FALSE;
			if (file)
			{
				retVal = TRUE;
				content_length = CyGOS::FileLength(file);
			}
		}
	}
	else//if its not a file or download request, its a request for some data
		//web page doesnt get refreshed in this case
	{
		header.reqType = reqData;
		tempStr = strstr(uri, "&lang=");
		langID = atoi(strchr(tempStr , '=') + 1);
		SetLanguageFileBuffer(Language(langID));
		tempStr = strstr(uri, "&userid1=");
		tempStr = strchr(tempStr , '=') + 1;
		userType = ValidateUser(tempStr);
		DcxWebUserID = atoi(tempStr);
		if(userType == ErrUser)
			retVal = FALSE;
		else
		{
		  if((tempStr = strstr(uri, "&lang=")))
			  *tempStr = 0;

		  res = PostResponse(uri, userType);
		  content_length = strlen(res);
		}
	}
	header.file = file;
	header.contentLength = content_length;
	header.data = res;

	return retVal;

}


/*This function handles the file upload request.
@param data : pointer to file data.
@param dataLen: The length of file data
@param fileName: The file name
@return: returns true if file is successfully send
to Upload Firmware task.
*/
BOOLEAN WebSite::WebFileUploadHandler(char * data, int dataLen, char * fileName)
{
	char * ext, * ext1;
	ext = strchr(fileName, '.');
	ext1 = ext;
	while(ext)
	{
		ext1 = ext;
		ext = strchr(ext + 1, '.');
	}
	FileType type = (!strcmp(ext1, ".bin"))?fileTypeBin:((!strcmp(ext1, ".cyg"))?fileTypeCyg:fileTypeErr);
	char * buff = new char[dataLen];
	memcpy(buff, data, dataLen);
	return UploadFirmware::thisPtr->ProgramFirmware(type, buff, dataLen);

}

/*This function gets called from DownLoadRequest() function on "save results to file"
 * button from Manufacturing Webpage.It converts the cycle time parametes which are in
 * milliseconds  to hours and minutes format. It also prepare the response according to
 * CSV file format.
*/
void FormatCycleResults()
{
	int temp, CycTimeHrs, CycTimeMins, sonicOnTimeHrs, sonicOnTimeMins;
	char * date, * time;
	char dateTime[MAX_TIMESTAMP_LENGTH];
	LogTime tm;
	temp = ReadPowerOnSecCounter();//SecondsSincePowerUp();
	CalculateTimeSincePowerUp(temp , &tm);
	sprintf(dateTime, "%02d-%02d-%04d %02d-%02d-%02d", tm.tm_mon, tm.tm_day,
					tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec);
	date = dateTime;
	time = strchr(dateTime , ' ') + 1;
	time[-1] = 0;

	temp = SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime);
	CycTimeHrs =  temp / NMSINHOUR;
	CycTimeMins = temp % NMSINHOUR;
	CycTimeMins = CycTimeMins / NMSINMINUTE;

	temp = SCALEDTIMETOWEBSITE(MFcycleResults.sonicsOnTime);
	sonicOnTimeHrs =  temp / NMSINHOUR;
	sonicOnTimeMins = temp % NMSINHOUR;
	sonicOnTimeMins = sonicOnTimeMins / NMSINMINUTE;

   sprintf(Response, "Date,Time,Serial Number,Cycle Time,Cycles Run,Hours Run,Alarms,Frequency,Power\r\n"
   							  "%s,%s,%s,%2d:%2d,%d,%2d:%2d,%d,%d,%d", date, time,ePromData.SerialNo, CycTimeHrs, CycTimeMins,
   							  (int)MFcycleResults.nCycles, (int)sonicOnTimeHrs, (int)sonicOnTimeMins, (int)ReadManFErrorCounter(),
   							  (int)((MFcycleResults.resultantFrequency * F_Factor) /1000), (int)((MFcycleResults.resultantPower * 100) / ADC11BITVALUE));

}

char * WebSite::HandleInitiateUpload()
{
	//check if readu for upload
	Response[0] = 0;
	strcpy(Response,"READY");
	return Response;
}

/*This function handles the file download request.
@param uri : The data submitted by a web form in a predefined
 standard format.It used to determine the file to be downloaded
@param file: The file name specific to request is filled in this pointer
 @return: returns the file data to be sent
*/
char * WebSite::DownLoadRequest(char * uri, char * file)
{
	char * tempStr;
	Response[0] = 0;
	WebCommand cmd;
	tempStr = strstr(uri , "cmd=");
	tempStr = strchr(tempStr, '=') + 1;
	cmd = (WebCommand)(atoi(tempStr));
	if(cmd == reqDownloadAlarmData)
	{
		GetLogData(Response, cmd);
		 strcpy(file, "alarmLog.csv");
	}
	else if(cmd == reqDownloadEventdata)
	{
	  GetLogData(Response, cmd);
	  strcpy(file, "eventLog.csv");
	}
	else if(cmd == reqDownloadCycleResults)
	{
		FormatCycleResults();
		strcpy(file, "cycleResults.csv");
	}
	else
	{
		strcpy(Response, "err,err,err,err\r\nerr,err,err,err\r\n");
		strcpy(file, "error.csv");
	}

	return Response;
}

/*This function is Called by Posthandler. It finds out the
 * appropriate handler for post request based on function ID
 * and command received in uri.
 * @param uri: Identifier of resource.  It can be an HTML file or some
 other data specific to the page on website.
 * @ param usr: user privilege to be filled in response
 * @return: The response data
 * */
 char* WebSite::PostResponse(char* uri , UserType usr)
 {
	 char * tempStr;
	 int funcID, webCmd;
	 Response[0] = 0;
	 UpdateUser(usr);
	 tempStr = strstr(uri , "func=");
	 tempStr = strchr(tempStr, '=') + 1;
	 funcID = atoi(tempStr);
	 tempStr = strstr(uri , "cmd=");
	 tempStr = strchr(tempStr, '=') + 1;
	 webCmd = atoi(tempStr);
	 tempStr = strstr(uri , "param=");
	 tempStr = strchr(tempStr, '=') + 1;

	// pprintf("\n Functions = %d", sizeof(webCommandHandlers)/sizeof(webCommandHandlers[0]));
	 if(((unsigned int)funcID > 0) && ((unsigned int)funcID < (sizeof(webCommandHandlers)/sizeof(webCommandHandlers[0]))))
	 {
	  if(webCommandHandlers[funcID].handler)
		 webCommandHandlers[funcID].handler(tempStr , (WebCommand)webCmd);
	 }
	 else
		 pprintf("func ID %d", funcID);

	 return Response;
 }


 /*This function is called by HTTPServer task on receiving GET request
 @header: Object which will be filled with the response length, response type etc.
 @param uri: Identifier of resource.  It can be an HTML file or some other
  data specific to the page on website. It may also contain parameter IDs
   to be received.
   @return: returns true in case of valid request. otherwise returns false
 */
BOOLEAN WebSite:: WebGetHandler (char * uri, HTTPReplyHeader & header)
{
	BOOLEAN retVal = FALSE;
	char * temp;
	char fileName[50];
	File * file = 0;
	//pprintf("\n uri GET = %s", uri);
	strcpy(fileName, "A:\\");
	if(strlen(uri + 1) > MAXFILELEN)
	{
		Log::WriteEventLog(GET_FREQ, (short)strlen(uri + 1), 0, 0, false);
		pprintf("\n Get File Name Too Long\n");
		return FALSE;
	}
	strcat(fileName, uri + 1);
	if((uri + 1)[0]){
		temp = strstr(fileName, "UserComment.html");
		if(temp)
		{
			//pprintf("file get %s", fileName);
		}
		else
		{
		temp = strstr(fileName, ".html");
		//if GET request for html file
		//send index.html
		if(temp)
			strcpy(fileName, "A:\\index.html");
		}
	}//no file name means the home page request
	else
		strcpy(fileName, "A:\\index.html");

	if(strcmp(header.cacheReq , MODIFIED_SINCE_STR) == 0)
	{
		header.statusCode = httpNotModified;
		retVal =  TRUE;
	}
	else
	{
		CyGOS::FileOpen(fileName, O_RDONLY, &file);
		int len = 0;
		if (file)
		{
			len = CyGOS::FileLength(file);
			retVal = TRUE;
		}

		if(strstr(fileName, "index.html"))//TODO:Error A;//?? reset user ID on home page
		{
			DcxWebUserID = 0;
		}

		header.file = file;
		header.contentLength = len;
		header.statusCode = httpOk;
		header.contentType =  GetContentType(fileName);
	}
	return retVal;
}

/*This function set the user privilege before any response data
 * base on this privilege javascript code on webpage decides
 * that which tabs need to be shown, or any other
 * things which is specific to branson user
 * @ param usr: user privilege to be filled in response
 */
void WebSite::UpdateUser(UserType usr)
{
	sprintf(Response, "%d@", usr);
}


/*This function fills data specific to IPSetup page
  * @param paramStr: pointer to parameter array
  * #param cmd: Command sent by jscript in uri.
  * 	        parameter is ignored by this function
 */
void WebSite::GetIPSetupVal(char * paramStr, WebCommand cmd)
{
	char temp[18];
	int networkConfigJS = currNetworkConfig;//network configuration to be sent to javascript code on website
	//1= dhcp client 2= dhcp server or static configuration selected
	strcat(Response,"IPAddress:D,");
	strcat(Response,CyGOS::GetIPAddressStr(interface));
	strcat(Response,"@SubnetMask:D,");
	strcat(Response,CyGOS::GetSubnetMaskStr(interface));
	strcat(Response,"@GateWay:D,");

	if(currNetworkConfig == configDhcpClient)
		strcat(Response,CyGOS::GetGatewayAddressStr(interface));
	else if(currNetworkConfig == configDhcpServer)
	{
		networkConfigJS = configStatic;
		strcat(Response,sysConfig.dhcpConfig.gateway.ToString());
	}
	else if(currNetworkConfig == configStatic)
		strcat(Response,sysConfig.gw.ToString());

	strcat(Response,"@ipconfig:RB,");
	sprintf(temp,"%d",networkConfigJS);
	strcat(Response, temp);
	strcat(Response,"@dhcpserver:CB,");
	sprintf(temp,"%d",(currNetworkConfig == configDhcpServer));
	strcat(Response, temp);
	unsigned char * macAddr = CyGOS::GetMacAddress(interface);
	sprintf(temp,"%02X-%02X-%02X-%02X-%02X-%02X",
			macAddr[0], macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
	strcat(Response,"@MAC:S,");
	strcat(Response, temp);
	cmd = reqNone;
	paramStr = 0;
}

UINT16 GetNumberofAnalogOut()
{
	UINT16 nFunctions = sizeof(webAnaIOfuncTableOut)/sizeof(WebAnalogIOFunctionTable);
	UINT16 OutFunctions = 1;
	for(int i = 1; i < nFunctions ; i++)
	{
		if(ePromData.PSpower >= webAnaIOfuncTableOut[i].Power)
			OutFunctions++;
	}
	return OutFunctions;

}

/*This function fills data specific to IO configuration page in a format whic javascript
 * code on webpage is expecting
 * @param paramStr: ponter to parameters array
 * @param cmd:cmd sent in uri by jscript code
 */
void WebSite::GetIOConfigurationVal(char * paramStr, WebCommand cmd)
{
	int indx;
	int nFunctions;
	if(cmd == reqRestore)
	{
		DcxLogUserID = DcxWebUserID;
		UserIO::ConfigureDefaultUserIOs();
		DcxLogUserID = 0;
	}

	char * tempStr = Response + strlen(Response);
	strcat(tempStr, "dd_pin1,dd_pin2,dd_pin3,dd_pin4:DD,");
	nFunctions = sizeof(webDigIOfuncTableIn)/sizeof(WebDigitalIOFunctionTable);
	for(int i = 1; i < nFunctions ; i++)
	{
		strcat(tempStr, FindLanguageString(webDigIOfuncTableIn[i].stringID, languageBuff, TRUE));
		sprintf(&tempStr[strlen(tempStr)] ,"#%d#%d", webDigIOfuncTableIn[i].ActivehighID,
				webDigIOfuncTableIn[i].ActivelowID);
		if( i < (nFunctions - 1))
			strcat(tempStr, ",");

	}
	strcat(tempStr, "@dd_pin7,dd_pin8,dd_pin9,dd_pin10:DD,");
	nFunctions = sizeof(webDigIOfuncTableOut)/sizeof(WebDigitalIOFunctionTable);
	for(int i = 1; i < nFunctions ; i++)
	{
		strcat(tempStr, FindLanguageString(webDigIOfuncTableOut[i].stringID, languageBuff, TRUE));
		sprintf(&tempStr[strlen(tempStr)] ,"#%d#%d", webDigIOfuncTableOut[i].ActivehighID,
				webDigIOfuncTableOut[i].ActivelowID);
		if( i < (nFunctions - 1))
			strcat(tempStr, ",");

	}

	strcat(tempStr, "@dd_pin17,dd_pin18:DD,");
	if(ePromData.EnergyPowerSpecial == TRUE){
		nFunctions = sizeof(webAnaIOfuncTableIn)/sizeof(WebAnalogIOFunctionTable);
	}
	else
		nFunctions = NUM_AIN;
	for(int i = 1; i < nFunctions; i++)
	{
		strcat(tempStr, FindLanguageString(webAnaIOfuncTableIn[i].stringID, languageBuff, TRUE));
		sprintf(&tempStr[strlen(tempStr)] ,"#%d", webAnaIOfuncTableIn[i].functionID);
		if( i < (nFunctions -1))
			strcat(tempStr, ",");

	}
	strcat(tempStr, "@dd_pin24,dd_pin25:DD,");
	if(ePromData.EnergyPowerSpecial == TRUE){
		nFunctions = GetNumberofAnalogOut();
		//nFunctions = sizeof(webAnaIOfuncTableOut)/sizeof(WebAnalogIOFunctionTable);
	}
	else
		nFunctions = NUM_AOUT + 1;
	for(int i = 1; i < nFunctions ; i++)
	{
		strcat(tempStr, FindLanguageString(webAnaIOfuncTableOut[i].stringID, languageBuff, TRUE));
		sprintf(&tempStr[strlen(tempStr)] ,"#%d", webAnaIOfuncTableOut[i].functionID);
		if( i < (nFunctions -1))
			strcat(tempStr, ",");
	}

	//get current functions after function list seperated by @
	strcat(tempStr , "@");
	char * formatStrDigIn[] = {0 ,"dd_pin1:DDV,%d,%d@","dd_pin2:DDV,%d,%d@",	"dd_pin3:DDV,%d,%d@", "dd_pin4:DDV,%d,%d@"};
	char * formatStrDigOut[] = {0 ,"dd_pin7:DDV,%d,%d@","dd_pin8:DDV,%d,%d@",	"dd_pin9:DDV,%d,%d@", "dd_pin10:DDV,%d,%d@"};
	char * formatStrAIn[] = {0 ,"dd_pin17:DDV,%d,%d@","dd_pin18:DDV,%d,%d@"};
	char * formatStrAOut[] = {0,"dd_pin24:DDV,%d,%d@","dd_pin25:DDV,%d,%d@"};

	for(indx = 1; indx < NUM_DIN; indx++)
		sprintf(&tempStr[strlen(tempStr)], formatStrDigIn[indx], USERIOConfig.DigInUse[indx],USERIOConfig.DigInUseStatus[indx]);
	for(indx = 1; indx < NUM_DOUT; indx++)
		sprintf(&tempStr[strlen(tempStr)], formatStrDigOut[indx], USERIOConfig.DigOutUse[indx],USERIOConfig.DigOutUseStatus[indx]);
	for(indx =1; indx < NUM_AIN; indx++)
		sprintf(&tempStr[strlen(tempStr)], formatStrAIn[indx], USERIOConfig.AnalogInUse[indx],USERIOConfig.AnalogInUseStatus[indx]);
	for(indx =1; indx < NUM_AOUT; indx++)
		sprintf(&tempStr[strlen(tempStr)], formatStrAOut[indx], USERIOConfig.AnalogOutUse[indx],USERIOConfig.AnalogOutUseStatus[indx]);
	paramStr = 0;

}

/*This function fills data specific to  weld-preset page in a format which javascript
 * code on this page is expecting
 * @param paramStr: ponter to parameters array
 * @param cmd: cmd sent by jscript in uri
 */
void WebSite::GetModifyPrestVal(char * paramStr,  WebCommand cmd)
{
	if(cmd == reqRestore)
	{
		SetDefaultWeldPreset(FALSE);
	}

	sprintf(&Response[strlen(Response)],
			"weldAmp:D,%d,%d,%d@"
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
			"digitaltune:D,%d,%d,%d",
			(int)CurrentPreset.WeldAmplitude ,(int)Preset_Min.WeldAmplitude ,(int)Preset_Max.WeldAmplitude,
			(int)(CONVMICRO_MILLI(CurrentPreset.SeekRampTime)),(int)(CONVMICRO_MILLI(Preset_Min.SeekRampTime)),(int)(CONVMICRO_MILLI(Preset_Max.SeekRampTime)),
			(int)(CONVMICRO_MILLI(CurrentPreset.WeldRampTime)),(int)(CONVMICRO_MILLI(Preset_Min.WeldRampTime)),(int)(CONVMICRO_MILLI(Preset_Max.WeldRampTime)),
			(int)(CONVMICRO_MILLI(CurrentPreset.SeekTime)),(int)(CONVMICRO_MILLI(Preset_Min.SeekTime)),(int)(CONVMICRO_MILLI(Preset_Max.SeekTime)),
			(int)CurrentPreset.SeekFreqOffset ,(int)Preset_Min.SeekFreqOffset ,(int)Preset_Max.SeekFreqOffset,
			(int)CurrentPreset.FreqOffset , (int)Preset_Min.FreqOffset , (int)Preset_Max.FreqOffset,
			CurrentPreset.SWDip.Flags.EndOfWeldStore,CurrentPreset.SWDip.Flags.ExtAmplitude,
			CurrentPreset.SWDip.Flags.ExtFrequency,
			CurrentPreset.SWDip.Flags.PowerUpOptions,CurrentPreset.SWDip.Flags.TimedSeek,
			CurrentPreset.AlarmResetReq,
			(int)(CurrentPreset.DigitalTune * F_Factor)/1000 ,(int)(Preset_Min.DigitalTune * F_Factor)/1000 ,(int)(Preset_Max.DigitalTune * F_Factor)/1000
			);
	paramStr = 0;
}


/*This function fills data specific to Advanced preset page in a format which javascript
 * code on this page is expecting
 * @param paramStr: pointer to parameters array
 * @param cmd: cmd sent by jscript in uri
 */
void WebSite::GetModifyPrestAdvancedVal(char * paramStr, WebCommand cmd)
{
	SINT32 freqStep = 0, freqStepMin = 0;
	if(cmd == reqRestore)
	{
		SetDefaultWeldPreset(TRUE);
	}

	freqStep = ParaScan.FrequencyStep/SCANFREQFACTOR;
	freqStepMin = ScanParamMin.FrequencyStep/SCANFREQFACTOR;

	sprintf(&Response[strlen(Response)],
			"AmplitudeloopC1:D,%d,%d,%d@"
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

			(int)CurrentPreset.WeldAmplitudeLoopC1,(int)Preset_Min.WeldAmplitudeLoopC1,(int)Preset_Max.WeldAmplitudeLoopC1,
			(int)CurrentPreset.SeekAmplitudeLoopC1,(int)Preset_Min.SeekAmplitudeLoopC1,(int)Preset_Max.SeekAmplitudeLoopC1,
			(int)ParaScan.MaxAmplitude ,(int)ScanParamMin.MaxAmplitude, (int)ScanParamMax.MaxAmplitude ,

			(int)CurrentPreset.WeldAmplitudeLoopC2,(int)Preset_Min.WeldAmplitudeLoopC2,(int)Preset_Max.WeldAmplitudeLoopC2,
			(int)CurrentPreset.SeekAmplitudeLoopC2,(int)Preset_Min.SeekAmplitudeLoopC2,(int)Preset_Max.SeekAmplitudeLoopC2,
			(int)ParaScan.MaxCurrent , (int)ScanParamMin.MaxCurrent , (int)ScanParamMax.MaxCurrent ,

			(int)CurrentPreset.WeldPhaseloopI, (int)Preset_Min.WeldPhaseloopI, (int)Preset_Max.WeldPhaseloopI,
			(int)CurrentPreset.SeekPhaseloopI, (int)Preset_Min.SeekPhaseloopI, (int)Preset_Max.SeekPhaseloopI,
			(int)(ParaScan.FrequencyStart) / SCANFREQFACTOR, (int)((ScanParamMin.FrequencyStart) / SCANFREQFACTOR), (int)((ScanParamMax.FrequencyStart) / SCANFREQFACTOR),

			(int)CurrentPreset.WeldPhaseloopCF, (int)Preset_Min.WeldPhaseloopCF, (int)Preset_Max.WeldPhaseloopCF,
			(int)CurrentPreset.SeekPhaseloopCF, (int)Preset_Min.SeekPhaseloopCF, (int)Preset_Max.SeekPhaseloopCF,
			(int)((ParaScan.FrequencyStop) / SCANFREQFACTOR), (int)((ScanParamMin.FrequencyStop ) / SCANFREQFACTOR), (int)((ScanParamMax.FrequencyStop) / SCANFREQFACTOR),

			(int)((CurrentPreset.WeldFrequencyLimitLow * F_Factor) / 1000), (int)((Preset_Min.WeldFrequencyLimitLow * F_Factor) / 1000), (int)((Preset_Max.WeldFrequencyLimitLow * F_Factor) / 1000),
			(int)((CurrentPreset.SeekFrequLimitLow * F_Factor) / 1000), (int)((Preset_Min.SeekFrequLimitLow * F_Factor) / 1000), (int)((Preset_Max.SeekFrequLimitLow * F_Factor) / 1000),
			(int)(CONVMICRO_MILLI(ParaScan.TimeDelay)),(int)( CONVMICRO_MILLI(ScanParamMin.TimeDelay)), (int)(CONVMICRO_MILLI(ScanParamMax.TimeDelay)),

			(int)((CurrentPreset.WeldFrequencyLimitHigh * F_Factor) / 1000), (int)((Preset_Min.WeldFrequencyLimitHigh * F_Factor) / 1000), (int)((Preset_Max.WeldFrequencyLimitHigh * F_Factor) / 1000),
			(int)((CurrentPreset.SeekFrequLimitHigh * F_Factor) / 1000), (int)((Preset_Min.SeekFrequLimitHigh * F_Factor) / 1000), (int)((Preset_Max.SeekFrequLimitHigh * F_Factor) / 1000),

			(int)(CONVMICRO_MILLI(CurrentPreset.WeldPhaseLimitTime)),(int)(CONVMICRO_MILLI(Preset_Min.WeldPhaseLimitTime)), (int)(CONVMICRO_MILLI(Preset_Max.WeldPhaseLimitTime)),
			(int)CurrentPreset.SeekAmplitude , (int)Preset_Min.SeekAmplitude , (int)Preset_Max.SeekAmplitude ,

			(int)CurrentPreset.WeldPhaseLimit, (int)Preset_Min.WeldPhaseLimit, (int)Preset_Max.WeldPhaseLimit,/**/

			// new values
			(int)CurrentPreset.WeldAmplitudeLoopC1New,(int)Preset_Min.WeldAmplitudeLoopC1New,(int)Preset_Max.WeldAmplitudeLoopC1New,
			(int)CurrentPreset.WeldAmplitudeLoopC2New,(int)Preset_Min.WeldAmplitudeLoopC2New,(int)Preset_Max.WeldAmplitudeLoopC2New,
			(int)CurrentPreset.WeldPhaseloopINew, (int)Preset_Min.WeldPhaseloopINew, (int)Preset_Max.WeldPhaseloopINew,
			(int)CurrentPreset.WeldPhaseloopPNew, (int)Preset_Min.WeldPhaseloopPNew, (int)Preset_Max.WeldPhaseloopPNew,
			(int)CurrentPreset.FreqWindowSize, (int)Preset_Min.FreqWindowSize, (int)Preset_Max.FreqWindowSize,
			(int)CurrentPreset.WeldPhaseloopCFNew, (int)Preset_Min.WeldPhaseloopCFNew, (int)Preset_Max.WeldPhaseloopCFNew,
			(int)((CurrentPreset.WeldFrequencyLimitLowNew * F_Factor) / 1000), (int)((Preset_Min.WeldFrequencyLimitLowNew * F_Factor) / 1000), (int)((Preset_Max.WeldFrequencyLimitLowNew * F_Factor) / 1000),
			(int)((CurrentPreset.WeldFrequencyLimitHighNew * F_Factor) / 1000), (int)((Preset_Min.WeldFrequencyLimitHighNew * F_Factor) / 1000), (int)((Preset_Max.WeldFrequencyLimitHighNew * F_Factor) / 1000),
			(int)CurrentPreset.WeldPhaseLimitTimeNew, (int)Preset_Min.WeldPhaseLimitTimeNew, (int)Preset_Max.WeldPhaseLimitTimeNew,
			(int)CurrentPreset.WeldPhaseLimitNew, (int)Preset_Min.WeldPhaseLimitNew, (int)Preset_Max.WeldPhaseLimitNew,
			(int)CurrentPreset.UseTrueValues
			);
	paramStr = 0;
}

/*This function fills data specific to system information page in a format which javascript
 * code on this page is expecting
 * @param paramStr: pointer to parameters array
 * @param cmd: Command sent by jscript in uri
*/
void WebSite::GetSystemInformationVal(char * paramStr, WebCommand cmd)
{
	//TODO: variables below are updated elsewhere
	char Crc[12] , Version[12];
	const char * currSys = (sysConfig.CurrSystemType == OEM)?"DCX V":
			((sysConfig.CurrSystemType == STD)?"DCX S":"Unknown");
	const char *disp = (sysConfig.CurrSystemType == OEM)?"None":
			((sysConfig.CurrSystemType == STD)?"Monochrome LCD":"Unknown");

	if(sysConfig.CurrSystemType == OEM)
	{
		strcpy(Crc, "N/A");
		strcpy(Version, "N/A");
	}
	else if(sysConfig.CurrSystemType == STD)
	{
		sprintf(Crc, "%08X", ((ReceiveCommandHandler::thisptr->LCDCodeCRCHigh << 8) | ReceiveCommandHandler::thisptr->LCDCodeCRCLow) );
		sprintf(Version, "%d",ReceiveCommandHandler::thisptr->LCDSWVersion);
	}
	else
	{
		strcpy(Crc, "N/A");
		strcpy(Version, "N/A");
	}

	sprintf(&Response[strlen(Response)],
			"pspwr:S,%d %s"
			"@psfrq:S,%d %s"
			"@lifetimecycles:S,%d"
			"@GeneralAlarms:S,%d"
			"@hoursofsonics:S,%02d hr%02d min %02d sec"
			"@poweronhours:S,%02d hr%02d min %02d sec"
			"@currsys:S,%s"
			"@Display:S,%s"
			"@swversion:S,%s"
			"@dcxcrc:S,%08X"
			"@snumber:S,%s"
			"@syscrc:S,%s"
			"@sysversion:S,%s"
			"@SpecPwd:S,%s"
			"@SpecPwdEnable:DP,%d",
			(int)ePromData.PSpower,"Watts", (int)ePromData.PSfrequency, "kHz",
			(int)ReadCycleCounter() ,(int)ReadErrorCounter(),(int)(ReadSonicOnSecCounter() / 3600),(int)((ReadSonicOnSecCounter()%3600)/60),(int)((ReadSonicOnSecCounter()%3600)%60),
			(int)(ReadPowerOnSecCounter() / 3600),(int)((ReadPowerOnSecCounter()%3600)/60),(int)((ReadPowerOnSecCounter()%3600)%60),currSys,disp,
			SWVersionLabel, (unsigned int)sysConfig.DCXCRC, ePromData.SerialNo, Crc, Version,
			SPECIALPASSWORD,(int)ePromData.EnergyPowerSpecial);
	  cmd = reqNone;
	  paramStr = 0;
}

/*This function finds the function string from language file
 * based on funcID
 * @param funcID: Function ID to determine language string ID for that function
 * @param signalType: the type of of function i.e. dig in/out or nalog in/out
 * @param filebuff: pointer to the content of language file
 * @return: Function string
 */
char * WebSite::GetFunctionString(int signalType, int funcID, int activeStatus, char * fileBuff)
{
	int nFunctions;
	int indx;
	int langStrID = 0;
	BOOLEAN ExitFlag;
	char * funcStr = funcStrUnassigned;
	if((funcID > 0) && (activeStatus == ACTIVATED))
	{
		switch(signalType)
		{
			case sigDigIn://digital input
				nFunctions = sizeof(webDigIOfuncTableIn)/sizeof(WebDigitalIOFunctionTable);
				ExitFlag = FALSE ;
				for(indx = 1; ((indx < nFunctions) && (ExitFlag == FALSE)) ; indx++)
				{
					if((webDigIOfuncTableIn[indx].ActivehighID == funcID)
							||(webDigIOfuncTableIn[indx].ActivelowID == funcID))
					{
						langStrID = webDigIOfuncTableIn[indx].stringID;
						ExitFlag =  TRUE;
					}
				}
				break;
			case sigDigOut://digital output
				nFunctions = sizeof(webDigIOfuncTableOut)/sizeof(WebDigitalIOFunctionTable);
				ExitFlag = FALSE ;
				for(indx = 1; ((indx < nFunctions) && (ExitFlag == FALSE)) ; indx++)
				{
					if((webDigIOfuncTableOut[indx].ActivehighID == funcID)
							||(webDigIOfuncTableOut[indx].ActivelowID == funcID))
					{
						langStrID = webDigIOfuncTableOut[indx].stringID;
						ExitFlag = TRUE ;
					}
				}
				break;
			case sigAnalogIn://analog input
				nFunctions = sizeof(webAnaIOfuncTableIn)/sizeof(WebAnalogIOFunctionTable);
				ExitFlag = FALSE ;
				for(indx = 1; ((indx < nFunctions) && (ExitFlag == FALSE)) ; indx++)
				{
					if((webAnaIOfuncTableIn[indx].functionID == funcID))
					{
							langStrID = webAnaIOfuncTableIn[indx].stringID;
							ExitFlag = TRUE;
					}
				}
				break;
			case sigAnalogOut://analog output
				nFunctions = sizeof(webAnaIOfuncTableOut)/sizeof(WebAnalogIOFunctionTable);
				ExitFlag = FALSE ;
				for(indx = 1; ((indx < nFunctions) && (ExitFlag == FALSE)) ; indx++)
				{
					if((webAnaIOfuncTableOut[indx].functionID == funcID))
					{
							langStrID = webAnaIOfuncTableOut[indx].stringID;
							ExitFlag = TRUE ;
					}
				}
				break;
		}
		funcStr =  FindLanguageString(langStrID, fileBuff, TRUE);
	}
	return funcStr;
}

/*This function fills data specific to diagnostic IO page in a format which javascript
 * code on this page is expecting
 * @param cmd: cmd sent by jscript in uri
*/
void WebSite::GetDiagnosticDataVal(char * paramStr, WebCommand cmd)
{
	if((UserIO::GetGenAlarmFlag() || UserIO::GetReadyFlag()) && !MFCycleFlag
			/*&& !diagPSFlag*/)
	{
		sprintf(&Response[strlen(Response)],"%s@","READY");
		diagFlag = TRUE;
		EnableWebIO = TRUE;
		diagnosticTimeOut = WEBDIAG_TIMEOUT;
	}
	else
		sprintf(&Response[strlen(Response)],"%s@","NOTREADY");

	PrepareDiagnosticResponse();
	cmd = reqNone;
	paramStr = 0;
}

/*This function is called by GetDiagnosticDataVal() function during
 * the DiagnosticIO page load. This function is also called in a
 * fixed interval from DiagnosticIO page during diagnostic. It fills
 * the current Digital and Analog Input state in global Response array
 * @param inputsOnly:Flag determines that whether only Input pins to
 * be read.
*/
void WebSite::PrepareDiagnosticResponse(bool inputsOnly)
{
	int indx;
	SINT32 ain;
	char * PINS[] = {0, "PIN1", "PIN2", "PIN3", "PIN4", "PIN7", "PIN8","PIN9", "PIN10",
		"PIN17", "PIN18", "PIN24", "PIN25"};
	for(indx = 1; indx < NUM_DIN; indx++)
	{
		sprintf(&Response[strlen(Response)],"%s:S,DI,%s,%d@",PINS[indx],
						GetFunctionString(0, USERIOConfig.DigInUse[indx], USERIOConfig.DigInUseStatus[indx] , languageBuff),
						UserIO::GetDigINPutActiveStatus((DIGITAL_INPUTS)indx));
	}
	if(!inputsOnly)
	{
		for(indx = 1; indx < NUM_DOUT; indx++)
		{
			sprintf(&Response[strlen(Response)],"%s:S,DO,%s,%d@",PINS[indx + 4],
							GetFunctionString(1, USERIOConfig.DigOutUse[indx], USERIOConfig.DigOutUseStatus[indx],languageBuff),
							UserIO::GetDigOutPutActiveStatus((DIGITAL_OUTPUTS)indx));
		}
	}
	for(indx = 1; indx < NUM_AIN; indx++)
	{
		ain = UserIO::GetAnalogINPutValue((ANALOG_INPUTS)indx);
		if(ain < 0)//returns -1 if unassigned. Only positive range expected on Analog inputs.
			ain = 0;
		sprintf(&Response[strlen(Response)],"%s:S,AI,%s,%d@",PINS[indx + 8],
						GetFunctionString(2, USERIOConfig.AnalogInUse[indx], USERIOConfig.AnalogInUseStatus[indx], languageBuff),
						(int)(ain * MI_ANALOGIP_MAXMV)/ADC11BITVALUE);
	}
	if(!inputsOnly)
	{
		for(indx = 1; indx < NUM_AOUT; indx++)
		{
			sprintf(&Response[strlen(Response)],"%s:S,AO,%s,%d@",PINS[indx + 10],
							GetFunctionString(3, USERIOConfig.AnalogOutUse[indx], USERIOConfig.AnalogOutUseStatus[indx], languageBuff),
							(int)((UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS)indx))* MI_ANALOGIP_MAXMV)/DAC_MULFACTOR);
		}
	}
}

/*This function fills data specific to Event and Alarm Log page.
 * in download request case data format is comma seperated to support csv doenload
 * else the data format is what the javascript code on these pages is expecting
 * @param cmd: Type of request alarm or event
 * @param paramStr: pointer to parameters array
*/
void WebSite::GetLogData(char * paramStr, WebCommand cmd)
{
	int nLogs = 0, indx;
	char * jscriptTableID = 0;
	char * dataFormat = 0;
	char * log;
	char TempUserID[MAXUSERIDLEN];
	BOOLEAN download = ((cmd == reqDownloadEventdata) || (cmd == reqDownloadAlarmData))?TRUE:FALSE;
	switch(cmd)
	{
		case reqClearAlarmLog:
			Log::ResetLog(clearAlarm);//only clearing alarm log
		case reqAlarmdata:
		case reqDownloadAlarmData:
			memset(webLogData,0,sizeof(webLogData));
			nLogs = Log::ReadAlarmLog(webLogData, Log::GetLatestAlarmEntryIndex(), MAX_ALARM_ENTRIES);
			jscriptTableID = "alarmLog";
			log = "Alarm#";
			break;
		case reqClearEventLog://only clearing event log
			Log::ResetLog(clearEvent);
		case reqEventdata:
		case reqDownloadEventdata:
			memset(webLogData,0,sizeof(webLogData));
			nLogs = Log::ReadEventLog(webLogData, Log::GetLatestEventEntryIndex(), MAX_EVENT_ENTRIES);
			jscriptTableID = "eventLog";
			log = "Event#";
			break;
		default:
			break;
	};

	if(download)
	{
		sprintf(&Response[strlen(Response)], "log,Date,Time,Event,UserID,Data1,Data2\r\n");
		dataFormat = "%d,%s,%s,%s,%s,%d,%d\r\n";
	}
	else
	{
		sprintf(&Response[strlen(Response)], "%s$LOG,", jscriptTableID);//separator changed to $ for html code
		dataFormat = "%d*%s*%s*%s*%s*%d*%d#";
	}
	for(indx = 0; indx < nLogs; indx++)
	{
		if(webLogData[indx].userID == USERIDBRANSON)
			strcpy(TempUserID,"Branson");
		else if(webLogData[indx].userID == USERIDMANUF1)
			strcpy(TempUserID,"Manuf1");
		else if(webLogData[indx].userID == USERIDMANUF2)
			strcpy(TempUserID,"Manuf2");
		else
			sprintf(TempUserID,"%d", webLogData[indx].userID);
		char * time = strchr(webLogData[indx].timeStamp , ' ') + 1;
		time[-1] = 0;
		sprintf(&Response[strlen(Response)], dataFormat,
				webLogData[indx].eventNo, webLogData[indx].timeStamp, time, webLogData[indx].logStr,
				TempUserID,
				webLogData[indx].data1, webLogData[indx].data2);
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
	do
	{
		int strID;
		if(firstID)
			strID = atoi(tempStr);
		else
			strID = atoi(tempStr + 1);
		firstID = FALSE;
		strcat(response,FindLanguageString(strID, languageBuff, FALSE));
		tempStr = strchr(tempStr + 1, ',');
	}
	while(tempStr);
}


/*This function finds the string in the file pointed by fileBuff
 * param and returns the same
 * @param StringID: Type of request alarm or even
 * @param fileBuff: pointer to language file
 * @param strOnly: flag to determine whether string is needed in format
 * javascript code expects or without format
 * @return: Function string
*/
 char* WebSite::FindLanguageString(int StringId, char * fileBuff, BOOLEAN strOnly)
 {
	char * tempStr = fileBuff;
	BOOLEAN firstStr = TRUE;
	do
	{
		int strID;
		if(!firstStr)
			tempStr = tempStr + 2;

		strID = atoi(tempStr);
		if(strID == StringId)
		{
			char * temp2 = strstr(tempStr  , "\r\n");
			if((temp2 - tempStr) < MAX_LANGUAGESTRING_LEN)
				strncpy(langString ,tempStr,temp2 - tempStr);
			else
				Log::WriteEventLog(LEN_INVALID, (short)(5) , (short)0, 0, false);
			break;
		}
		firstStr = FALSE;
		tempStr = strstr(tempStr , "\r\n");
	}while(tempStr);
	if(!strOnly)
	{
		if(strlen(langString) < MAX_LANGUAGESTRING_LEN)
			strcat(langString, "@");
	}
	else
	{
		if(strlen(langString) < MAX_LANGUAGESTRING_LEN)
			strcpy(langString , strchr(langString, ',') + 1);
	}

	return langString;
 }

 /*This function fills data specific to after user login from website
  * @param paramStr: pointer to parameters array
  * @param cmd: cmd sent in uri.(ignored by this function)
  */
 void WebSite::SetLanguageAndUserID(char * paramStr, WebCommand cmd)
 {
	 strcpy(Response,"OK");
	 cmd = reqNone;
	 paramStr = 0;
 }


 /*This function gets called on save button click on IPsetup page
  @param paramStr: new parameters send by javascript code after validation
  @param cmd: Command sent by jscript in uri
 */
 void WebSite::UpdateIPSetup(char * paramStr, WebCommand cmd)
 {
	 NetworkConfiguration config;
	 char paramVal[100];
	 char * tempPtr1, * tempPtr2;
	 unsigned int SysIP,SubnetIP,GatewayIP;
	 int retValue = FALSE;
	 int MaxIPStrLen = 16;//"255.255.255.255"
	 if(cmd == reqRestore)
	 {//restore IP setup
			SetDHCPServerIPDefaults();
			SetStaticIPDefaults();
			retValue = TRUE;
	 }
	 else
	 {
		 tempPtr1 = strstr(paramStr, "&ipconfig:");
		 tempPtr2 = strchr(tempPtr1, ':') + 1;
		 config = (NetworkConfiguration)atoi(tempPtr2);
		 if(config <= configDefault)
		 {
			 sysConfig.currNetworkConfig = config;
			 if(config == configDhcpClient)
			 {
				 //ignore other values
				 retValue = TRUE;
			 }
			 else
			 {
				 //Set the values for DHCP Server configuration
				 tempPtr1 = strstr(paramStr,"IPAddress:");
				 tempPtr1 = strchr(tempPtr1,':') + 1;//Points to IP Address Value
				 tempPtr2 = strstr(tempPtr1, "&SubnetMask:");
				 if((tempPtr2 - tempPtr1) < MaxIPStrLen)
				 {
					 strncpy(paramVal, tempPtr1, tempPtr2 - tempPtr1);
					 SysIP = IPStrtoUint(paramVal);

					 tempPtr1 = strstr(paramStr,"SubnetMask:");
					 tempPtr1 = strchr(tempPtr1,':') + 1;//Points to Subnet mask Value
					 tempPtr2 = strstr(tempPtr1, "&GateWay:");
					 if((tempPtr2 - tempPtr1) < MaxIPStrLen)
					 {
						 strncpy(paramVal, tempPtr1, tempPtr2 - tempPtr1);
						 SubnetIP = IPStrtoUint(paramVal);

						 tempPtr1 = strstr(paramStr,"GateWay:");
						 tempPtr1 = strchr(tempPtr1,':') + 1;//Points to gateway Value
						 tempPtr2 = strstr(tempPtr1, "&ipconfig:");
						 if((tempPtr2 - tempPtr1) < MaxIPStrLen)
						 {
							 strncpy(paramVal, tempPtr1, tempPtr2 - tempPtr1);
							 GatewayIP = IPStrtoUint(paramVal);
							 retValue = TRUE;
							 if(config == configDhcpServer)
							 {
								 sysConfig.systemIP = SysIP;//IPStrtoUint(paramVal);
								 sysConfig.dhcpConfig.dns = SysIP;//IPStrtoUint(paramVal);//NNS server is same
								 sysConfig.dhcpConfig.gateway.addr = GatewayIP;//IPStrtoUint(paramVal);
								 sysConfig.dhcpConfig.netmask.addr = SubnetIP;//IPStrtoUint(paramVal);
								 sysConfig.dhcpConfig.poolStart.addr =  (sysConfig.systemIP.addr & sysConfig.dhcpConfig.netmask.addr) + htonl(1);

							 }
							 if(config == configStatic)
							 {
								 sysConfig.systemIPStatic = SysIP;//IPStrtoUint(paramVal);
								 sysConfig.netmask.addr = SubnetIP;//IPStrtoUint(paramVal);
								 sysConfig.gw.addr = GatewayIP;//IPStrtoUint(paramVal);
							 }
						 }
					 }
				 }
			 }
		 }
	 }
	 regUpdateCmdFlag = TRUE;
	 if(retValue == TRUE)
		 strcpy(Response,"OK");
	 else
	 {
		 strcpy(Response,"NOK");
		 Log::WriteEventLog(LEN_INVALID, (short)(6) , (short)0, 0, false);
	 }

 }

 /*This function gets called on save button click on IO config page
   @param paramStr: new parameters send by javascript code after validation
   @param cmd: command sent by jscript in uri
  */
void WebSite::UpdateIOConfiguration(char * paramStr, WebCommand cmd)
{
	int indx, funcID, activeStatus;
	UINT16 digOutputsActiveConfiguration = 0;
	BOOLEAN writeLowSelectEvent = FALSE;
	BOOLEAN cableDetectInput = FALSE;
	char * tmpStr = strstr(paramStr , "DI:");//TODO: Validate string present
	UINT16 EnergyFuncNum = 0;
	tmpStr = strchr(tmpStr, ':') + 1;
	for(indx = 1; indx < NUM_DIN; indx++)
	{
		funcID = atoi(tmpStr);
		tmpStr = strchr(tmpStr, ',') + 1;
		activeStatus = atoi(tmpStr);
		UserIO::AssignDigitalInputFunction((DIGITAL_INPUTS)indx , funcID, activeStatus);
		if(funcID == CABLEDETECT_ACTIVEHIGH)
			cableDetectInput = (BOOLEAN)activeStatus;
		tmpStr = strchr(tmpStr , ',') + 1;
	}
	tmpStr = strstr(paramStr , "DO:");
	tmpStr = strchr(tmpStr, ':') + 1;
	for(indx = 1; indx < NUM_DOUT; indx++)
	{
		funcID = atoi(tmpStr);
		tmpStr = strchr(tmpStr, ',') + 1;
		activeStatus = atoi(tmpStr);
		UserIO::AssignDigitalOutputFunction((DIGITAL_OUTPUTS)indx, funcID, activeStatus);
		//don't care if not active. Let bootloader do whatever it is doing
		digOutputsActiveConfiguration |= (OutPtr[funcID].Active << (indx -1));
		tmpStr = strchr(tmpStr , ',') + 1;
	}

	tmpStr = strstr(paramStr , "AI:");
	tmpStr = strchr(tmpStr, ':') + 1;
	for(indx = 1; indx < NUM_AIN; indx++)
	{
		funcID = atoi(tmpStr);
		tmpStr = strchr(tmpStr, ',') + 1;
		activeStatus = atoi(tmpStr);
		UserIO::AssignAnalogInputFunction((ANALOG_INPUTS)indx, funcID, activeStatus);
		if((funcID == ENERGY_CONTROL_RANGE1) || (funcID == ENERGY_CONTROL_RANGE2) || (funcID == ENERGY_CONTROL_RANGE3))
		{
			EnergyFuncNum = funcID;
		}
		tmpStr = strchr(tmpStr , ',') + 1;
	}

	tmpStr = strstr(paramStr , "AO:");
	tmpStr = strchr(tmpStr, ':') + 1;
	for(indx = 1; indx < NUM_AOUT; indx++)
	{
		funcID = atoi(tmpStr);
		tmpStr = strchr(tmpStr, ',') + 1;
		activeStatus = atoi(tmpStr);
		UserIO::AssignAnalogOutputFunction((ANALOG_OUTPUTS)indx, funcID, activeStatus);
		tmpStr = strchr(tmpStr , ',') + 1;
	}
	tmpStr = strstr(tmpStr , "&lowselect");
	tmpStr = strchr(tmpStr, ',') + 1;
	writeLowSelectEvent = (BOOLEAN)atoi(tmpStr);
	DcxLogUserID = DcxWebUserID;
	if(writeLowSelectEvent)
	{
		pprintf("\n low select");
		Log::WriteEventLog(EVENT_ACTIVELOWDIGIN, 0 ,0);
	}
	//save this at FRAM start
	__FRAM_START = digOutputsActiveConfiguration;
	UserIO::SetCableDetect((BOOLEAN)cableDetectInput);
	UserIO::SetEnergyFuncNum(EnergyFuncNum);
	Log::WriteEventLog(USERIO_DEFINED, 0, 0);
	DcxLogUserID = 0;
	strcpy(Response,"OK");
	cmd = reqNone;
}

/*This function gets called during diagnostic of IOs
   @param paramStr: pointer to reponse buffer
   @para cmd: Diagnostic command from website
  */
void WebSite::ProcessDiagnosticIOCommand(char * paramStr, WebCommand cmd)
{
	switch(cmd)
	{
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
			if(diagFlag || ((UserIO::GetGenAlarmFlag() || UserIO::GetReadyFlag()) && !MFCycleFlag /*&&!diagPSFlag*/))
			{
					strcpy(Response , "OK@");
					EnableWebIO = TRUE;
					if(diagFlag)
						PrepareDiagnosticResponse(TRUE);
					else
						PrepareDiagnosticResponse();
					diagFlag = TRUE;
			}
			else
			{
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

/*This function gets called on Reset from Diagnostic/PS page
   @param paramStr: pointer to reponse buffer
   @para cmd: Optional command from website
  */
void WebSite::ProcessResetCommand(char * paramStr, WebCommand cmd)
{
	//If alarm is there then only reset is needed. There was a case when website resetFlag was true
	//due to reset button was pressed from website. This caused very next alarm to be cleared
	//as soon as it occurred. We can't control on this web site code because reset their should work
	//in the cases where alarm is caused by some other means(i.e without activating TEST,SCAN etc from website
	//)
	if(AlarmFlag == TRUE)
		resetFlag = TRUE;
	strcpy(Response, "OK");
	cmd = reqNone;
	paramStr = 0;
}

/*This function gets called on save button click on diagnostic IO page
  @param paramStr: new parameters send by javascript code after validation
 */
void WebSite::UpdateDiagnosticIO(char * paramStr)
{
	char * tempStr = paramStr;
	int indx;
	//first check system is ready for diagnostic
   for(indx = 1; indx < NUM_DOUT ;indx++)
      tempDigOutPutStatus[indx] = UserIO::GetDigOutPutActiveStatus((DIGITAL_OUTPUTS)indx);

   for(indx = 1; indx < NUM_AOUT; indx++)
   {
      tempAnalogOutPutVals[indx] = UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS)indx);
      //pprintf("\n at diag start an%d val=%d",indx, tempAnalogOutPutVals[indx]);
   }

   diagStartFlag = TRUE;
   StartDiagTimeOut = WEBDIAG_TIMEOUT;
   for(indx = 1; indx < NUM_DOUT; indx++)
   {
      UserIO::SetDigOutPutActiveStatus((DIGITAL_OUTPUTS)indx, BOOLEAN(atoi(tempStr)));
      tempStr = strchr(tempStr, ',') + 1;
   }
   for(indx = 1; indx < NUM_AOUT; indx ++)
   {
      //pprintf("\n setting aout %d val = %d", indx ,((atoi(tempStr) * DAC_MULFACTOR) / MI_ANALOGIP_MAXMV));
      UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)indx, ((atoi(tempStr) * DAC_MULFACTOR) / MI_ANALOGIP_MAXMV));

      tempStr = strchr(tempStr, ',') + 1;
   }
   strcpy(Response, "OK");
}

/*This function gets called on stop button click on diagnostic IO page
  */
void WebSite::StopIODiagnostic()
{
	if(diagStartFlag)
	{
		diagStartFlag = FALSE;
		RestoreOutPutStatus();
		strcpy(Response, "OK@");
		PrepareDiagnosticResponse();
	}
}


/* This function restores the original output values(i.e. before diagnostic)
 *  of Digital and analog outputs and starts the state machine to receive
 *  commands from other interfaces
 */
void WebSite::RestoreOutPutStatus()
{
	int indx;
	for(indx = 1; indx < NUM_DOUT ;indx++)
		UserIO::SetDigOutPutActiveStatus((DIGITAL_OUTPUTS)indx, (tempDigOutPutStatus[indx])?TRUE:FALSE);

	for(indx = 1; indx < NUM_AOUT; indx++)
		UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)indx, tempAnalogOutPutVals[indx]);
}

/*This function gets called on LoopBack button click on diagnostic IO page
 * it toggles the all digital and analog outputs
 */
void WebSite::DoDiagnosticLoopBack()
{
	BOOLEAN currState;
	SINT32 currDacOutPut;
	int indx;

	for(indx = 1; indx < NUM_DOUT; indx++)
	{
		currState = UserIO::GetDigOutPutActiveStatus((DIGITAL_OUTPUTS)indx);
		//TODO: inverting currState like ~currstate is not working
		UserIO::SetDigOutPutActiveStatus((DIGITAL_OUTPUTS)indx, (currState)?FALSE:TRUE);
	}

	for(indx = 1; indx < NUM_AOUT; indx++)
	{
		currDacOutPut = UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS)indx);
		//Toggle AnalogOutput
		UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)indx,(currDacOutPut)?0:DAC_MULFACTOR);
	}
}

/*This function gets called initial;y on start of loopback
 * from DiagnosticIO page. It stores the current state of
 * digital and analog outputs in an temporary global array
 */
void WebSite::DoDiagnosticLoopBackStart()
{
	int indx;
   diagStartFlag = TRUE;
   StartDiagTimeOut = LOOPBACK_TIMEOUT;
   for(indx = 1; indx < NUM_AOUT; indx++)
   {	//Store AnalogOutput
      tempAnalogOutPutVals[indx] = UserIO::GetAnalogOUTPutValue((ANALOG_OUTPUTS)indx);
      //pprintf(" \n Set Analog to zero !!!!!!!!!!");
      UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)indx, 0);
   }
   for(indx = 1; indx < NUM_DOUT ;indx++)
   {  // Store Digital Output
      tempDigOutPutStatus[indx] = UserIO::GetDigOutPutActiveStatus((DIGITAL_OUTPUTS)indx);
   }
   strcpy(Response, "OK");
}

/*This function gets called on save button click on weld-preset page
  @param paramStr: new parameters send by javascript code after validation
  @param cmd:command sent by jscript in uri
 */
void WebSite::UpdatePreset(char * paramStr, WebCommand cmd)
{
	SINT32 tempOffset = 0, tempdigitaltune ;
    char * tempStr = strstr(paramStr, "weldAmp=");
	CurrentPreset.WeldAmplitude = (atoi(strchr(tempStr, '=') + 1));
	tempStr = strstr(paramStr, "&startRamp=") + 1;
	CurrentPreset.WeldRampTime =  CONV_MILLI_MICROSEC(atoi(strchr(tempStr, '=') + 1));
	tempStr = strstr(paramStr, "&FreqOff=") + 1;

	tempStr = strchr(tempStr, '=') + 1;
	if(tempStr[0] == '-')
		tempOffset = atoi(tempStr + 1) * (-1);
	else
		tempOffset = atoi(tempStr);
	tempStr = strstr(paramStr, "&endofweldstore=") + 1;
	CurrentPreset.SWDip.Flags.EndOfWeldStore =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&externalamplitude=") + 1;
	CurrentPreset.SWDip.Flags.ExtAmplitude =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&externalfrequency=") + 1;
	CurrentPreset.SWDip.Flags.ExtFrequency =  atoi(strchr(tempStr, '=') + 1);
	if(CurrentPreset.SWDip.Flags.ExtFrequency == false)
		CurrentPreset.FreqOffset =  tempOffset;

	tempStr = strstr(paramStr, "seekRamp=") + 1;
	CurrentPreset.SeekRampTime =  CONV_MILLI_MICROSEC(atoi(strchr(tempStr, '=') + 1)) ;
	tempStr = strstr(paramStr, "&seekTime=") + 1;
	CurrentPreset.SeekTime =  CONV_MILLI_MICROSEC(atoi(strchr(tempStr, '=') + 1));
	tempStr = strstr(paramStr, "&SeekFreqOff=") + 1;
	tempStr = strchr(tempStr, '=') + 1;
	if(tempStr[0] == '-')
		CurrentPreset.SeekFreqOffset = atoi(tempStr + 1) * (-1);
	else
		CurrentPreset.SeekFreqOffset = atoi(tempStr);
	tempStr = strstr(paramStr, "&powerup=") + 1;
	CurrentPreset.SWDip.Flags.PowerUpOptions =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&timedSeek=") + 1;
	CurrentPreset.SWDip.Flags.TimedSeek =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&stoponalarm=") + 1;
	CurrentPreset.AlarmResetReq =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&digitaltune=") + 1;
	tempdigitaltune = (atoi(strchr(tempStr, '=') + 1) * 1000)/F_Factor;
	if((F_Factor == F30KHZF_FACTOR) && (((atoi(strchr(tempStr, '=') + 1) * 1000) % 150) != 0)){
		tempdigitaltune = tempdigitaltune + 1;
	}
	if(CurrentPreset.DigitalTune != tempdigitaltune){
		CurrentPreset.MemoryFreq = 0;
		MemoryValue = 0;
		CurrentPreset.DigitalTune =  tempdigitaltune;
	}
	SavePreset(0);
	regUpdateCmdFlag = TRUE;
	strcpy(Response,"OK");
	cmd = reqNone;
}

/*This function gets called on save button click on advanced-preset page
  @param paramStr: new parameters send by javascript code after validation
  @param cmd: Command sent by jscript in uri
 */
void WebSite::UpdatePresetAdvanced(char * paramStr, WebCommand cmd)
{
	char * tempStr = strstr(paramStr, "AmplitudeloopC1=");
	CurrentPreset.WeldAmplitudeLoopC1 = atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&AmplitudeloopC2=") + 1;
	CurrentPreset.WeldAmplitudeLoopC2 =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&Phaseloop=") + 1;
	CurrentPreset.WeldPhaseloopI =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&PhaseloopCF=") + 1;
	CurrentPreset.WeldPhaseloopCF =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&FreqLow=") + 1;
	CurrentPreset.WeldFrequencyLimitLow =  ((atoi(strchr(tempStr, '=') + 1)) * 1000) / F_Factor;
	tempStr = strstr(paramStr, "&FreqHigh=") + 1;
	CurrentPreset.WeldFrequencyLimitHigh =  ((atoi(strchr(tempStr, '=') + 1)) * 1000) / F_Factor;
	tempStr = strstr(paramStr, "&PhaseLimitTime=") + 1;
	CurrentPreset.WeldPhaseLimitTime =  CONV_MILLI_MICROSEC(atoi(strchr(tempStr, '=') + 1));
	tempStr = strstr(paramStr, "&PhaseLimit=") + 1;
	CurrentPreset.WeldPhaseLimit = atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "UseTrueValues=");
	char * tempStr2 = strchr(tempStr, '=') + 1;
	if(*tempStr2 == 'f')
	{
		CurrentPreset.UseTrueValues = false;
	}
	else
	{
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
	CurrentPreset.WeldFrequencyLimitLowNew = ((atoi(strchr(tempStr, '=') + 1)) * 1000) / F_Factor;
	tempStr = strstr(paramStr, "&FreqHighNew=") + 1;
	CurrentPreset.WeldFrequencyLimitHighNew = ((atoi(strchr(tempStr, '=') + 1)) * 1000) / F_Factor;
	tempStr = strstr(paramStr, "&PhaseLimitTimeNew=") + 1;
	CurrentPreset.WeldPhaseLimitTimeNew = atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&PhaseLimitNew=") + 1;
	CurrentPreset.WeldPhaseLimitNew = atoi(strchr(tempStr, '=') + 1);

	tempStr = strstr(paramStr, "SeekAmplitudeloopC1=") + 1;
	CurrentPreset.SeekAmplitudeLoopC1 =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&SeekAmplitudeloopC2=") + 1;
	CurrentPreset.SeekAmplitudeLoopC2 =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&SeekPhaseloop=") + 1;
	CurrentPreset.SeekPhaseloopI =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&SeekPhaseloopCF=") + 1;
	CurrentPreset.SeekPhaseloopCF =  atoi(strchr(tempStr, '=') + 1);
	tempStr = strstr(paramStr, "&SeekFreqLow=") + 1;
	CurrentPreset.SeekFrequLimitLow =  ((atoi(strchr(tempStr, '=') + 1))  * 1000) / F_Factor;
	tempStr = strstr(paramStr, "&SeekFreqHigh=") + 1;
	CurrentPreset.SeekFrequLimitHigh =  ((atoi(strchr(tempStr, '=') + 1)) * 1000) / F_Factor;
	tempStr = strstr(paramStr, "&SeekAmp=") + 1;
	CurrentPreset.SeekAmplitude = (atoi(strchr(tempStr, '=') + 1));


	tempStr = strstr(paramStr, "Amplitude=") + 1;
	ParaScan.MaxAmplitude =  (atoi(strchr(tempStr, '=') + 1));
	tempStr = strstr(paramStr, "&Current=") + 1;
	ParaScan.MaxCurrent =  (atoi(strchr(tempStr, '=') + 1));
	tempStr = strstr(paramStr, "&FreqStart=") + 1;
	ParaScan.FrequencyStart =  ((atoi(strchr(tempStr, '=') + 1)) * SCANFREQFACTOR);
	tempStr = strstr(paramStr, "&FreqEnd=") + 1;
	ParaScan.FrequencyStop =  ((atoi(strchr(tempStr, '=') + 1)) * SCANFREQFACTOR) ;
	tempStr = strstr(paramStr, "&StepDelay=") + 1;
	ParaScan.TimeDelay =  CONV_MILLI_MICROSEC(atoi(strchr(tempStr, '=') + 1));

	SavePreset(0);
	SaveScanParamCRC();
	regUpdateCmdFlag = TRUE;
	strcpy(Response,"OK");
	cmd = reqNone;
}
/*This function gets called on Update Graph button click on PS-Test page
  @param paramStr: new parameters send by javascript code after validation
  @param cmd: Command sent by jscript in uri
 */
void WebSite::ViewWeldGraphGetDatahandeler(char * paramStr,  WebCommand cmd)
{
	int i,indx,strtindx,endindx;
	strtindx = WeldBufferIndex - GRAPHPOINT;
	endindx = WeldBufferIndex;
	if(strtindx < 0)
			strtindx = SIZE_WELDBUFFER - GRAPHPOINT + WeldBufferIndex;
	if(WeldBufferTime < GRAPHPOINT){
	   endindx = WeldBufferIndex;
	   strtindx = 1;
	 }
	else
		endindx = GRAPHPOINT;
	indx = strtindx - 1;
	switch(cmd)
	{
		case reqTimeData:
			strcpy(Response,"Time,");
			for(i =0; i < endindx; i++)
			{
				if(WeldBuffer[indx].logValid == TRUE)
					sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[indx].Time);
				if(indx == SIZE_WELDBUFFER - 1)
					indx = 0;
				else
					indx++;
			}
			break;
		case reqCurrentData:
			strcpy(Response,"Current,");
			for(i = 0; i < endindx; i++ )
			{
				if(WeldBuffer[indx].logValid == TRUE)
					sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[indx].Current);
				if(indx == SIZE_WELDBUFFER - 1)
					indx = 0;
				else
					indx++;
			}
			break;
		case reqPhaseData:
			strcpy(Response,"Phase,");
			for(i = 0; i < endindx; i++ )
			{
				if(WeldBuffer[indx].logValid == TRUE)
					sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[indx].Phase);
				if(indx == SIZE_WELDBUFFER - 1)
					indx = 0;
				else
					indx++;
			}
			break;
		case reqAmplitudeData:
			strcpy(Response,"Amplitude,");
			for(i = 0; i < endindx; i++ )
			{
				if(WeldBuffer[indx].logValid == TRUE)
					sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[indx].ActualAmplitude);
				if(indx == SIZE_WELDBUFFER - 1)
					indx = 0;
				else
					indx++;
			}
			break;
		case reqPowerData:
			strcpy(Response,"Power,");
			for(i = 0; i < endindx; i++ )
			{
				if(WeldBuffer[indx].logValid == TRUE)
					sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[indx].Power);
				if(indx == SIZE_WELDBUFFER - 1)
					indx = 0;
				else
					indx++;
			}
			break;
		case reqPWMAmplitudeData:
			strcpy(Response,"PWMAmplitude,");
			for(i = 0; i < endindx; i++ )
			{
				if(WeldBuffer[indx].logValid == TRUE)
					sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[indx].PWMValue);
				if(indx == SIZE_WELDBUFFER - 1)
					indx = 0;
				else
					indx++;
			}
			break;
		case reqFrequencyData:
			strcpy(Response,"Frequency,");
			for(i = 0; i < endindx; i++ )
			{
				if(WeldBuffer[indx].logValid == TRUE)
					sprintf(&Response[strlen(Response)],"%d,", WeldBuffer[indx].Frequency);
				if(indx == SIZE_WELDBUFFER - 1)
					indx = 0;
				else
					indx++;
			}
			break;
		case reqPresetData:
			strcpy(Response,"PresetData,");			// Pre formatted preset data
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
					(int)(((Midband + FREQ_MIN)* F_Factor) / 1000),(int)(((Midband + FREQ_MAX)* F_Factor) / 1000),
					(int)CurrentPreset.WeldAmplitudeLoopC1,(int)CurrentPreset.WeldAmplitudeLoopC2,
					(int)CurrentPreset.WeldPhaseloopI,(int)CurrentPreset.WeldPhaseloopCF,
					(int)CurrentPreset.WeldAmplitudeLoopC1New,(int)CurrentPreset.WeldAmplitudeLoopC2New,
					(int)CurrentPreset.WeldPhaseloopINew,(int)CurrentPreset.WeldPhaseloopPNew,
					(int)CurrentPreset.WeldPhaseloopCFNew,(int)CurrentPreset.SeekPhaseloopI,
					(int)CurrentPreset.FreqWindowSize,(int)CurrentPreset.WeldFrequencyLimitHigh,
					(int)CurrentPreset.WeldFrequencyLimitLow,(int)CurrentPreset.WeldPhaseLimit,
					(int)CurrentPreset.WeldPhaseLimitTime,(int)CurrentPreset.WeldFrequencyLimitHighNew,
					(int)CurrentPreset.WeldFrequencyLimitLowNew,(int)CurrentPreset.WeldPhaseLimitNew,
					(int)CurrentPreset.WeldPhaseLimitTimeNew,(int)CurrentPreset.SeekAmplitude,
					(int)CurrentPreset.SeekAmplitudeLoopC1,(int)CurrentPreset.SeekAmplitudeLoopC2,
					(int)CurrentPreset.SeekFrequLimitHigh,(int)CurrentPreset.SeekFrequLimitLow,
					(int)CurrentPreset.SeekPhaseloopCF,(int)CurrentPreset.WeldAmplitude,
					(int)CurrentPreset.FreqOffset,(int)CurrentPreset.WeldRampTime,
					(int)CurrentPreset.SeekRampTime,(int)CurrentPreset.SeekTime,
					(int)CurrentPreset.AlarmResetReq
			  	);
			break;
		default:
			break;
	}
	paramStr = 0;
}
/*This function gets called on Update Graph button click on HornSignature page
  @param paramStr: new parameters send by javascript code after validation
  @param cmd: Command sent by jscript in uri
 */
void WebSite::ViewGraphHornSignatureGetDatahandeler(char * paramStr,  WebCommand cmd)
{  
	int i,frequency;
	if((ScanInitiate == TRUE) || (ScanStarted == TRUE) || (ScanRun == TRUE))
		strcpy(Response , "SCANINPROGRESS");
	else
	{
		switch(cmd)
		{
			case reqCurrentData:
				strcpy(Response,"Current,");
				for(i = 0; i <= HornScanGraphPointCount; i++ )
					sprintf(&Response[strlen(Response)],"%d,", (ScanBuffer[i].Current * SCALING_FACTOR) / ADC11BITVALUE);
				break;
			case reqPhaseData:
				strcpy(Response,"Phase,");
				for(i = 0; i <= HornScanGraphPointCount; i++ )
					sprintf(&Response[strlen(Response)],"%d,", ScanBuffer[i].Phase);
				break;
			case reqAmplitudeData:
				strcpy(Response,"Amplitude,");
				for(i = 0; i <= HornScanGraphPointCount; i++ )
					sprintf(&Response[strlen(Response)],"%d,", (ScanBuffer[i].Amplitude * SCALING_FACTOR) / ADC11BITVALUE);
				break;
			case reqFrequencyData:
				strcpy(Response,"Frequency,");
				for(i = 0; i <= HornScanGraphPointCount; i++ )
					sprintf(&Response[strlen(Response)],"%d,",(int)ScanBuffer[i].Frequency);
				break;
			case reqPresetData:
				strcpy(Response,"PresetData,");		// Pre formatted preset data
				sprintf(&Response[strlen(Response)],
						"%d,%d,%d,\n\nFrequency Start (Hz),%d"
						"\nFrequency Stop (Hz),%d"
						"\nFrequency Step (Hz),%d"
						"\nStep - Delay (ms),%d"
						"\nAmplitude(%%),%d"
						"\nCurrent(%%),%d",
						(int)(ParaScan.FrequencyStep/SCANFREQFACTOR),
						(int)(ParaScan.FrequencyStart/SCANFREQFACTOR) + 1,
						(int)(ParaScan.FrequencyStop/SCANFREQFACTOR) + 1,
						(int)(ParaScan.FrequencyStart/SCANFREQFACTOR),
						(int)(ParaScan.FrequencyStop/SCANFREQFACTOR),
						(int)(ParaScan.FrequencyStep/SCANFREQFACTOR),
						(int)(CONVMICRO_MILLI(ParaScan.TimeDelay)),
						(int)ParaScan.MaxAmplitude ,
						(int)ParaScan.MaxCurrent
					);
				for(int i = 0; i < DCP_NUMRESONANCES; i++)
				{
					frequency = (int)ResonanceFreqs[i];
					if (frequency)
					if (ResonanceFreqType[i] == FREQ_PARALLEL)
						sprintf(&Response[strlen(Response)],
							"\nParallel Resonant Point %d (Hz),%d",
							(i/2 + 1),(int)ResonanceFreqs[i]);
					else if(ResonanceFreqType[i] == FREQ_SERIES)
						sprintf(&Response[strlen(Response)],
							"\nSeries Resonant Point %d (Hz),%d",
							(i/2 + 1),(int)ResonanceFreqs[i]);
				}
				break;
			case reqResFreq:
				strcpy(Response,"ResonFreq,");
				for(int i = 0;i < DCP_NUMRESONANCES  ; i++)
				{
					if (ResonanceFreqType[i] == FREQ_PARALLEL)
						sprintf(&Response[strlen(Response)],"P:%d,",(int)ResonanceFreqs[i]);
					else if(ResonanceFreqType[i] == FREQ_SERIES)
						sprintf(&Response[strlen(Response)],"S:%d,",(int)ResonanceFreqs[i]);
				}
				break;
			default:
				break;
		}
	}
	paramStr = 0;
}
/*This function calculates the number of segments
 * to be on, on frequency bar during TEST from website
 */
UINT32 CalculateDDSFreqinPercent()
{
	//Convert current Freq
	UINT16 CurrFreq = (((DDSFrequency)* F_Factor)/1000);

	if(CurrFreq >= FREQ40KHz_START) {
		CurrFreq = ((CurrFreq - FREQ40KHz_START)/ FREQ40KHz_HzPERSEGMENT) + 1;

	} else if(CurrFreq >= FREQ30KHz_START ){
		CurrFreq = ((CurrFreq - FREQ30KHz_START)/ FREQ30KHz_HzPERSEGMENT) + 1;

	} else  {
		CurrFreq = ((CurrFreq - FREQ20KHz_START)/ FREQ20KHz_HzPERSEGMENT) + 1;

	}
	if (CurrFreq > NUM_BARGRAPH_SEGMENT) // Total 20 icons for power level.
		CurrFreq = NUM_BARGRAPH_SEGMENT;
	return CurrFreq;
}

/*This function calculates the segment number
 * to set on , on memory bar during TEST from website
 */
UINT16 CalculateMemoryFreqinPercent()
{
	//Convert MemoryFreq
	UINT16 MemoryFreq = (((CurrentPreset.MemoryFreq + CurrentPreset.DigitalTune)* F_Factor)/1000);

	if(MemoryFreq >= FREQ40KHz_START) {
		MemoryFreq = ((MemoryFreq - FREQ40KHz_START)/ FREQ40KHz_HzPERSEGMENT) + 1;

	} else if(MemoryFreq >= FREQ30KHz_START ){
		MemoryFreq = ((MemoryFreq - FREQ30KHz_START)/ FREQ30KHz_HzPERSEGMENT) + 1;

	} else  {
		MemoryFreq = ((MemoryFreq - FREQ20KHz_START)/ FREQ20KHz_HzPERSEGMENT) + 1;

	}
	if (MemoryFreq > NUM_BARGRAPH_SEGMENT) // Total 20 icons for power level.
		MemoryFreq = NUM_BARGRAPH_SEGMENT;
	return MemoryFreq;
}

/*This function calculates the segment numbers
 * to set on , on power bar during TEST from website
 */
UINT16 CalculatePowerinPercent()
{
	//Convert current power input in percentage form
	UINT8 currPower = (PowerInput * 100) / ADC11BITVALUE;
	currPower = (currPower * NUM_BARGRAPH_SEGMENT)/ 100;

	if (currPower > NUM_BARGRAPH_SEGMENT) // Total 20 icons for power level.
		currPower = NUM_BARGRAPH_SEGMENT;

	return currPower;
}

/*This function calculates the segment numbers
 * to set on , on amplitude bar during TEST from website
 */
UINT16 CalculateAmplitudeinPercent()
{
	UINT8  Amplitude = (ActualAmplitude * 100) / ADC11BITVALUE;
	Amplitude = (Amplitude * NUM_BARGRAPH_SEGMENT)/ 100;

	if (Amplitude > NUM_BARGRAPH_SEGMENT) // Total 20 icons for power level.
		Amplitude = NUM_BARGRAPH_SEGMENT;

	return Amplitude;
}

/*This function gets called during TEST from website
 * and fills the live data to be shown on diagnostic-ps
 * page during TEST
 * @param cmd:Test command from website e.g start,stop,get live data etc.
 * @param paramStr: pointer to parameters
 */
void WebSite::ProcessTestCommand(char * paramStr , WebCommand cmd)
{
	diagnosticPSTimeOut = WEBTEST_TIMEOUT;
	switch(cmd)
	{
		case reqSeekStart:
			if(UserIO::GetReadyFlag() && !diagFlag && !MFCycleFlag)
			{
				sprintf(&Response[strlen(Response)] , "TESTSTARTED:%d,%d,%d,%d,%d",(int)stateSeek, (int)CalculateDDSFreqinPercent(),(int)CalculateMemoryFreqinPercent(),(int)CalculateAmplitudeinPercent(),(int)CalculatePowerinPercent());
				//testStartFlag = TRUE;
				WebSeek = TRUE;
				DcxLogUserID = DcxWebUserID;
			}
			else
				strcat(Response , "SYSTEMBUSY:");
			break;
		case reqSeekPoll:
			sprintf(&Response[strlen(Response)] , "TESTPOLLRESPONSE:%d,%d,%d,%d,%d@%d,%d,%d,%d",
					(int)CheckForCurrentFunction(), (int)CalculateDDSFreqinPercent(),
					(int)CalculateMemoryFreqinPercent(),(int)CalculateAmplitudeinPercent(),
					(int)CalculatePowerinPercent(),(int)((DDSFrequency* F_Factor)/1000),(int)(((CurrentPreset.MemoryFreq + CurrentPreset.DigitalTune) * F_Factor)/1000),
					(int)(ActualAmplitude * 100) / ADC11BITVALUE,(int)(PowerInput * 100) / ADC11BITVALUE);

			break;
	  case reqSeekResult:
			sprintf(&Response[strlen(Response)] , "TESTRESULT:%d,%d,%d,%d,%d",
					(int)CheckForCurrentFunction(), (int)CalculateDDSFreqinPercent(),
					(int)CalculateMemoryFreqinPercent(),(int)CalculateAmplitudeinPercent(),
					(int)CalculatePowerinPercent());
			WebSeek = FALSE;
			break;
		default:
			strcpy(Response,"OK");
			break;
	}
	paramStr = 0;
}

/*This function gets called during SCAN from website
 * and fills the scan result to be shown on diagnostic-ps
 * page during SCAN
 * @param cmd:SCAN command from website e.g start,abort etc
 * @param paramStr: pointer to response buffer
 */
void WebSite::ProcessScanCommand(char * paramStr, WebCommand cmd)
{
	diagnosticPSTimeOut = WEBSCAN_TIMEOUT;

	switch(cmd)
	{
		case reqScanStart:
			if(UserIO::GetReadyFlag() && !MFCycleFlag && !diagFlag)
			{
				strcat(Response , "SCANSTARTED:");
				scanAbortFlag = FALSE;
				scanStartFlag = TRUE;
				webtimeOutScan = WEBSCAN_TIMEOUT;
				DcxLogUserID = DcxWebUserID;
				EnableWebIO = TRUE;
			}
			else
				strcat(Response , "SYSTEMBUSY:");
			break;
		case reqScanPoll:
			if((ScanInitiate == TRUE) || (ScanStarted == TRUE) || (ScanRun == TRUE))
				sprintf(&Response[strlen(Response)] , "SCANINPROGRESS:%d", PercentScan);
			else if(ScanComplete == TRUE)
			{
				strcat(Response , "SCANCOMPLETED:");
				for(int i = 0; i < DCP_NUMRESONANCES; i++)
				{
					sprintf(&Response[strlen(Response)] , "%d,%d,", (int)ResonanceFreqType[i] , (int)ResonanceFreqs[i]);
				}
				scanStartFlag = FALSE;
				EnableWebIO = FALSE;
				IsIOConfigured = TRUE;
				DcxLogUserID = 0;
			}
			else if(ScanAbort == TRUE){
				strcat(&Response[strlen(Response)] , "SCANFAILED:");
				scanStartFlag = FALSE;
				EnableWebIO = FALSE;
				IsIOConfigured = TRUE;
				DcxLogUserID = 0;
			}

			webtimeOutScan = WEBSCAN_TIMEOUT;
			break;
		case reqScanAbort:
			strcat(&Response[strlen(Response)] , "SCANABORTED:");
			webtimeOutScan = WEBSCAN_TIMEOUT;
			scanStartFlag = FALSE;
			scanAbortFlag = TRUE;
			EnableWebIO = FALSE;
			break;
		default:
			strcpy(Response,"OK");
			break;
	}
	paramStr = 0;
}

/*This function returns the currently logged in user ID for logging
 */
unsigned int WebSite::GetUserID()
{
	return DcxLogUserID;
}


/*This function gets called by state machine to determine
 * the test command status from website
 * @return: true if TEST is started from website else false
 */
BOOLEAN  WebSite::CheckForWebTestCommand()
{
	return (mfTestFlag);
}

/*This function gets called by state machine to reset
 * the test command status from website
 */
void WebSite::ResetTestFlag(void)
{
	mfTestFlag = FALSE;
	EnableWebIO = FALSE;
}
/*This function gets called by state machine to determine
 * the SCAN command status from website
 * @return: true if SCAN is started from website else false
 */
BOOLEAN WebSite::CheckForWebScanCommand()
{
	return scanStartFlag;
}


/*This function gets called by state machine to determine
 * the SCAN abort status from website
 * @return: true if SCAN is aborted from website else false
 */
BOOLEAN WebSite::CheckForWebScanAbortCommand()
{
	return scanAbortFlag;
}

/*This function gets called by state machine to reset
 *  the scan flags after SCAN is completed or aborted.
 */
void WebSite::ResetScanFlags()
{
	scanStartFlag = FALSE;
	scanAbortFlag = FALSE;
	DcxLogUserID = 0;
}

/*This function gets called by state machine to determine
 * that whether the diagnostic is in progress.
 * @return: returns true if diagnostic is under progress
 */
BOOLEAN WebSite::CheckForDiagnosticFlag()
{
	return diagFlag;
}

/*This function gets called by state machine to check
 *the reset command from website
 * @return: returns true if if reset is sent
 * by website
 */
BOOLEAN WebSite::CheckForWebReset()
{
	return resetFlag;
}

/*This function gets called by state machine to reset
 *the reset flag after Reset is done
 */
void WebSite::ResetWebResetFlag()
{
	resetFlag = FALSE;
}

/*This function gets called by state machine to check
 *whether DCP board is in test mode or not.
 */
BOOLEAN WebSite::CheckForMFBoardTestFlag()
{
	return mfBoardTestFlag;
}

/*This function gets called by ReceiveCommandHandler to check
 *whether the Manufacturing page is currently active and we need
 *to lock the LCD
 */
BOOLEAN WebSite::CheckForManufacturingFlag()
{
	return mfFlag;
}


/*This function gets called on load of hornsignature page
 *  from website.It fills the last successful horn scan
 *  result in Response buffer
 * @param cmd:parameter gets ignored
 * @param paramStr: pointer to possible param list(ignored)
 */
void WebSite::GetLastSucessfullHornScanData(char * paramStr, WebCommand cmd)
{
	sprintf(&Response[strlen(Response)],"%s@PSFreqType:%d,%d@","READY",(int)(ParaScan.FrequencyStart/SCANFREQFACTOR) + 1,
				(int)(ParaScan.FrequencyStop/SCANFREQFACTOR) + 1);
	if(ScanComplete == TRUE)
	{
		strcat(Response , "SCANCOMPLETED:");
		for(int i = 0; i < DCP_NUMRESONANCES; i++)
		{
			sprintf(&Response[strlen(Response)] , "%d,%d,", (int)ResonanceFreqType[i] , (int)ResonanceFreqs[i]);
		}
	}
	cmd = reqNone;
	paramStr = 0;
}

/*This function gets called during ps-test on load from website
 * and checks whether system is OK to run diagnostic
 * @param cmd:parameter gets ignored
 * @param paramStr: pointer to possible param list(ignored)
 */
void WebSite:: ProcessPSTestOnLoad(char * paramStr, WebCommand cmd)
{
	sprintf(&Response[strlen(Response)], "%s@%d@%d@%d@%d@%d" , "OK",(int)(((Midband + FREQ_MIN)* F_Factor) / 1000),(int)(((Midband + FREQ_MAX)* F_Factor) / 1000),
			(int)CalculateMemoryFreqinPercent(),
			(int)(((CurrentPreset.MemoryFreq + CurrentPreset.DigitalTune ) * F_Factor)/1000),
			(SINT32)CheckForCurrentFunction()
	);
	cmd = reqNone;
	paramStr = 0;
}

/*
 * This function gets called on Save System Setup from
 * Manufacturing Page.
 * @param:String containing IP, Serial number,Frequency and powerLevel
 */
void WebSite::ProcessMFSavesystemSetupCommand(char * param)
{
	UINT32 currFreq;
	char tempVal[100];
	char * temp1;
	currFreq = ePromData.PSfrequency;

	char * temp = strstr(param, "freq:") + strlen("freq:");
	ePromData.PSfrequency =(FREQUENCY)atoi(temp);

	temp = strstr(param, "&pwr:") + strlen("&pwr:");
	ePromData.PSpower = (atoi(temp));

	temp = strstr(param,"&ipaddr:") + strlen("&ipaddr:");//Points to IP Address Value
	temp1 = strstr(temp, "&snumber:");

	if(temp1 - temp < 100)
	{
		strncpy(tempVal, temp, temp1 - temp);
		sysConfig.systemIPStatic = IPStrtoUint(tempVal);
		sysConfig.systemIP = IPStrtoUint(tempVal);
		sysConfig.dhcpConfig.dns = IPStrtoUint(tempVal);//DNS server is same
		temp = strstr(param, "&snumber:") + strlen("&snumber:");
		if(strlen(temp) < SERAILNUMBER_LEN)
			strcpy((char*)ePromData.SerialNo, temp);
	}
	//save power supply and frequency to MAC chip
	//save serial number to MAC chip
	ModifyEEPROMdata();
	if(currFreq != ePromData.PSfrequency)
	{
		UpdateMidBandAndFFactor();
		SetDefaultWeldPreset(TRUE);
		SetMaxMinValues();
		SetMinMaxScanParameter();
	}
}

/*
 * This function gets called on S System Setup from
 * Manufacturing Page.
 * @param:String containing cycle parameters
 */
void WebSite::ProcessMFStartStopCommand(char * param)
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


/*
 * This function checks the the current power supply specification
 * e.g(20Khz 3300W) and returns option value to be used by js code.
 * @retVal: The return value is the index of option in frequency/power
 * drop down box on manufacturing page.
 */
//int GetPowerSupplySpecification()
//{
//
//	int retVal = 0, indx;
//	bool exitLoop = FALSE;
//
//	for(indx = 0; ((indx < ((int)(sizeof(PSSpecs) / sizeof(PSSpecs[0])))) && (!exitLoop)) ; indx++)
//	{
//		if((ePromData.PSpower == ((UINT32)PSSpecs[indx].pwr)) && (ePromData.PSfrequency == ((UINT32)PSSpecs[indx].freq)))
//		{
//			retVal = (int)PSSpecs[indx].type;
//			exitLoop = TRUE;
//		}
//	}
//	return retVal;
//}

/*This function determines the current state of system.i.e busy etc
 * This is sent to Manufacturing page
 */
BOOLEAN GetCurrentStateOnManufacturing()
{
	BOOLEAN retVal = FALSE;
	if(UserIO::GetReadyFlag() || MFCycleFlag || UserIO::GetGenAlarmFlag() || WebSite::mfTestFlag)
		retVal = TRUE;
	return retVal;
}

/*This function gets called during Manufacturing web Page
 * is open on website. Its under implementation.
 */
void WebSite::ProcessManufacturingCmd(char * paramStr, WebCommand cmd)
{
	int wldrCnt = 0, indx;
	char * currStat = 0 , * TempStr = 0;
	WelderInfoParam * wInfo = 0;
	mfTimeOut = MF_TIMEOUT;
	 int index = 0;
	   switch(ePromData.PSfrequency){
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
	switch(cmd)
	{
		case reqSearchWelders:
			wldrCnt = WelderSearchTask::thiPtr->GetWelderCount();
			if(wldrCnt > 0)
			{
				wInfo = new WelderInfoParam[wldrCnt];
				WelderSearchTask::thiPtr->GetWeldersInformation(wInfo);
				for(indx = 0;indx < wldrCnt;indx++)
					sprintf(&Response[strlen(Response)], "%s/%s@", wInfo[indx].ipaddr.ToString(), wInfo[indx].WelderInfo.SeriallNumber);
			}
			else
				strcpy(Response,"No welder found");
			break;
		case reqMFOnLoad:
			if(GetCurrentStateOnManufacturing())
			{
				mfFlag = TRUE;
				EnableWebIO = TRUE;
				currStat = "OK";
			}else
				currStat = "BUSY";

			sprintf(&Response[strlen(Response)],
						"%s@"
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
						"SetupPwrWatt:D,%d@",
						currStat,
						CyGOS::GetIPAddressStr(interface),
						ePromData.SerialNo,
						(int)CurrentCycleParam.totalCycleTime,(int)MinCycleParam.totalCycleTime,(int)MaxCycleParam.totalCycleTime,
						(int)CurrentCycleParam.sonicsOnTimePerWeldCycle,(int)MinCycleParam.sonicsOnTimePerWeldCycle,(int)MaxCycleParam.sonicsOnTimePerWeldCycle,
						(int)CurrentCycleParam.sonicsOffTimePerWeldCycle,(int)MinCycleParam.sonicsOffTimePerWeldCycle,(int)MaxCycleParam.sonicsOffTimePerWeldCycle,
						(int)CurrentCycleParam.Amplitude,(int)MinCycleParam.Amplitude,(int)MaxCycleParam.Amplitude,
						(int)CurrentCycleParam.stopOnAlarm,
						(int)((MFcycleResults.resultantFrequency * F_Factor)/1000),(int)((MFcycleResults.resultantPower  * 100) / ADC11BITVALUE),
						(int)MFcycleResults.nCycles, (int)(SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)),
						(int)(SCALEDTIMETOWEBSITE(MFcycleResults.sonicsOnTime)), (int)MFcycleResults.nAlarms,
						(int)(CurrentCycleParam.totalCycleTime - SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)), (int)MFcycleResults.cycleStatus, index,(int)ePromData.PSpower);
			break;
		case  reqRestoreDefaults:
			SetManFDefaultFromWeb();
			strcpy(Response, "OK");
			break;
		case 	reqClearAlarmLogMF:
			Log::ResetLog(clearAlarm);
			strcpy(Response, "OK");
			break;
		case 	reqClearEventLogMF:
			Log::ResetLog(clearEvent);
			strcpy(Response, "OK");
			break;
		case 	reqResetAlarmCounter:
			ResetManFErrorCounter();
			strcpy(Response, "OK");
			break;
		case 	reqStartStopCycle:
			if(CurrentCycleParam.MfTest)
			{
				MFcycleResults.cycleStatus = statStopped;
				SonicsOff();
				SonicsOnCounter = 0;
				SonicsOffCounter = 0;
				CurrentCycleParam.MfTest = FALSE;
				StartMFCycle = MFCycleFlag = CurrentCycleParam.MfTest;
				strcpy(Response, "STOPPED");
				CurrentCycleParam.CRC = ByteCRC((UINT8*)&CurrentCycleParam, sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
				regAmplitudeUpdateFlag = TRUE;
			}
			else
			{
				if(UserIO::GetReadyFlag())
				{
					ProcessMFStartStopCommand(paramStr);
					if(CheckForCycleParameterChange())
					{
						memset(&MFcycleResults, 0 , sizeof(MFcycleResults));
						if((CurrentCycleParam.sonicsOnTimePerWeldCycle == 0) || (CurrentCycleParam.sonicsOffTimePerWeldCycle == 0))
							TempSonicsOnTimePerWeldCycle = CurrentCycleParam.totalCycleTime;
						else
							TempSonicsOnTimePerWeldCycle = CurrentCycleParam.sonicsOnTimePerWeldCycle;
					}
					CurrentCycleParam.MfTest = TRUE;
					StartMFCycle = MFCycleFlag = CurrentCycleParam.MfTest;
					strcpy(Response, "STARTED");
					CurrentCycleParam.CRC = ByteCRC((UINT8*)&CurrentCycleParam, sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
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
			if(GetCurrentStateOnManufacturing())
			{
				mfFlag = TRUE;
				EnableWebIO = TRUE;
				currStat = "OK";
			}else
				currStat = "BUSY";

			sprintf(Response,"%s@"
			"freqval:S,%d@"
			"pwrval:S,%d@"
			"ncycles:S,%d@"
			"nhrs:S,%d@"
			"crsonicsontime:S,%d@"
			"nalarms:S,%d@"
			"crremainingtime:S,%d@"
			"stat:RB,%d@",
			currStat, (int)((MFcycleResults.resultantFrequency * F_Factor) /1000), (int)((MFcycleResults.resultantPower * 100) / ADC11BITVALUE),
			(int)MFcycleResults.nCycles, (int)(SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)),
			(int)(SCALEDTIMETOWEBSITE(MFcycleResults.sonicsOnTime)), (int)MFcycleResults.nAlarms,
			(int)(CurrentCycleParam.totalCycleTime - SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)), (int)MFcycleResults.cycleStatus);
			break;
		case regMFTestStart:
			if(UserIO::GetReadyFlag())
			{
				strcpy(Response,"TESTSTARTED");
				mfTestFlag = TRUE;
				webtimeOutTest = WEBTEST_TIMEOUT;
				DcxLogUserID = DcxWebUserID;
			}
			else
				strcpy(Response , "SYSTEMBUSY");
			break;
		case regMFTestStop:
			strcpy(Response, "TESTSTOPPED");
			DcxLogUserID = 0;
			mfTestFlag = FALSE;
			break;
		case reqMFTestPoll:
			webtimeOutTest = MF_TIMEOUT;
			sprintf(Response, "MFTESTPOLLRESPONSE:%d",CheckForCurrentFunction());
			break;
		case reqSpecialEnter:
			TempStr = strstr(paramStr, "&SpecialPassword=") + 1;
			TempStr = (strchr(TempStr, '=') +1);
			if(strcmp(TempStr,SPECIALPASSWORD) == 0)
			{
				ePromData.EnergyPowerSpecial = TRUE;
				ModifyEEPROMdata();
				sprintf(Response, "Special %s has been Activated.",SPECIALPASSWORD);
			}
			else
				strcpy(Response, "Wrong Password.");
			break;
		case reqNone:
			if(GetCurrentStateOnManufacturing())
			{
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
	if(wInfo)
		delete wInfo;
}

/*This function is called from ProcessManufacturingCommand function on Start/Stop
 * Cycling button click.
 * @retVal: return TRUE if cycle parameters sent from the website
 * are not the same in FRAM else returns FALSE.
 */
BOOLEAN WebSite::CheckForCycleParameterChange(void)
{
	BOOLEAN result = FALSE;

	if(CurrentCycleParam.totalCycleTime != TempCurrentCycleParam.totalCycleTime)
	{
		CurrentCycleParam.totalCycleTime = TempCurrentCycleParam.totalCycleTime;
		result = TRUE;
	}

	if(CurrentCycleParam.sonicsOnTimePerWeldCycle != TempCurrentCycleParam.sonicsOnTimePerWeldCycle)
	{
		CurrentCycleParam.sonicsOnTimePerWeldCycle = TempCurrentCycleParam.sonicsOnTimePerWeldCycle;
		result = TRUE;
	}

	if(CurrentCycleParam.sonicsOffTimePerWeldCycle != TempCurrentCycleParam.sonicsOffTimePerWeldCycle)
	{
		CurrentCycleParam.sonicsOffTimePerWeldCycle = TempCurrentCycleParam.sonicsOffTimePerWeldCycle;
		result = TRUE;
	}

	if(CurrentCycleParam.Amplitude != TempCurrentCycleParam.Amplitude)
	{
		CurrentCycleParam.Amplitude = TempCurrentCycleParam.Amplitude;
		result = TRUE;
	}

	if(CurrentCycleParam.stopOnAlarm != TempCurrentCycleParam.stopOnAlarm)
	{
		CurrentCycleParam.stopOnAlarm = TempCurrentCycleParam.stopOnAlarm;
		result = TRUE;
	}

	if(MFcycleResults.cycleStatus != statStopped)
		result = TRUE;

	return result;
}

/*This function gets called during Board Test on polling
 * It fills the reponse buffer with current input states
 * from instrumentation board including phase sign.
 */
void WebSite::ProcessMFBoardTestPollResponse()
{
	UINT8 inputs = fpga_input;
	inputs = ~inputs;
	//maintain the sequence
	sprintf(&Response[strlen(Response)], "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
			(inputs & OVL_FUNCTION)?1:0, (inputs & OL_TEMPERATURE)?1:0,(inputs & OL_POWER)?1:0,
			(inputs & OL_VOLTAGE)?1:0,(inputs & OL_CURRENT)?1:0, ((~(inputs) & FPGA_SIGN) >> 2)?1:0,
			      (int)PowerInput, (int)InputTemperature,(int)ActualAmplitude,(int)CurrentInput,(int)PhaseInput);
}


/*This function gets called during Board Test on refresh command
 * @param: Pointer to parameters e.g digital outputs from fpga to
 * instrumentation board and test frequency.
 */
void WebSite::ProcessMFBoardTestRefreshCmd(char * param)
{
	char * temp;
	UINT8 digOutput;
	UINT8 maskSet[] = {OLRST_BIT, DPSSTOP_BIT, PHASEA_BIT, PHASEB_BIT};
	char * idArr[] ={"&olrst:", "&stop:", "&phaseA:", "&phaseB:"};

	temp = strstr(param, "testFreq:") + strlen("testFreq:");
	//test frequency range gets validated in javscript code itself
	//so it doesnot need to be validated here
	BoardTestFrequency = atoi(temp) * SCANFREQFACTOR;
	BoardTestOutput |= TEST_BIT;
	for(int indx = 0; indx < 4; indx++)
	{
		temp = strstr(temp, idArr[indx]) + strlen(idArr[indx]);
		digOutput = atoi(temp);
		(digOutput)?(BoardTestOutput |= maskSet[indx]):(BoardTestOutput &= ~maskSet[indx]);
	}
}


/*This function gets called during Board Test from website
 * @param cmd:oard test command from website e.g login, poll inputs,
 * refresh outputs etc.
 * @param paramStr: pointer to parameters. ignored in all cases
 * except refresh command. On refresh it monis to the digital outputs
 * and test frequecy values sent from website
 */
void WebSite::ProcessManufacturingBoardTestCmd(char * paramStr, WebCommand cmd)
{
	char * mfpassword = strstr(paramStr, "password:") + strlen("password:");
	strcpy(Response, "NOTOK");
	switch(cmd)
	{
      case reqBoardTestLogin:
         if(!strcmp(mfpassword, MFBOARDTESTPASSWORD))
         {
//            if(UserIO::GetReadyFlag())
//            {
               EnableWebIO = TRUE;
               mfBoardTestFlag = TRUE;
               mfBoardTestTimeOut = MFBOARDTEST_TIMEOUT;
               strcpy(Response, "OK");
//            }
//            else
//               strcpy(Response, "BUSY");
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
       //  if(UserIO::GetReadyFlag())
         {
            EnableWebIO = TRUE;
            mfBoardTestFlag = TRUE;
            mfBoardTestTimeOut = MFBOARDTEST_TIMEOUT;
            strcpy(Response, "OK@");
            ProcessMFBoardTestPollResponse();
         }
//         else
//            strcpy(Response, "BUSY@");
         break;
      default:
         strcpy(Response, "NOTOK");
	};
}

/* This function gets called every RTOS tick.
 * it determines whether the SCAN, TEST or diagnostic
 * commands from website has been timed out in some accidental
 * case(e.g ethernet cable removed, website window closed etc).
 * It disables the corresponding flags in that case.
 * @callbackus: RTOS tick in microseconds
 */
void WebSite::Tick(int callbackus)
{
	if(scanStartFlag)
	{
		webtimeOutScan -= callbackus;
		if(webtimeOutScan <= 0)
		{
			scanStartFlag = FALSE;
			scanAbortFlag = TRUE;
			EnableWebIO = FALSE;
			IsIOConfigured = TRUE;
			DcxLogUserID = 0;
		}
	}

	if(mfTestFlag)
	{
		webtimeOutTest -= callbackus;
		if(webtimeOutTest <= 0)
		{
			EnableWebIO = FALSE;
			IsIOConfigured = TRUE;
			DcxLogUserID = 0;
			mfTestFlag = FALSE;
		}
	}

	if(diagFlag)
	{
		diagnosticTimeOut -= callbackus;
		if(diagnosticTimeOut <= 0)
		{
			diagFlag = FALSE;
			EnableWebIO = FALSE;
			IsIOConfigured = TRUE;
			DcxLogUserID = 0;
		}
	}
	if(diagStartFlag)
	{
		StartDiagTimeOut -= callbackus;
		if(StartDiagTimeOut <= 0)
		{
			diagStartFlag = FALSE;
			RestoreOutPutStatus();
		}
	}

	if(mfBoardTestFlag)
	{
		mfBoardTestTimeOut -= callbackus;
		if(mfBoardTestTimeOut <= 0)
		{
			EnableWebIO = FALSE;
			IsIOConfigured = TRUE;
			mfBoardTestFlag = FALSE;
			BoardTestFrequency = MIN_BOARD_TESTFREQUENCY;
			BoardTestOutput = 0;
		}
	}

	if(mfFlag)
	{
		mfTimeOut -= callbackus;
		if(mfTimeOut <= 0)
		{
			mfFlag = FALSE;
			EnableWebIO = MFCycleFlag;
			if(!EnableWebIO)
					IsIOConfigured = TRUE;
		}
	}
}
