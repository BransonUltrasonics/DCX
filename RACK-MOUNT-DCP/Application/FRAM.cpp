/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/FRAM.cpp_v   1.9   30 Oct 2015 08:52:08   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/FRAM.cpp_v  $
 * 
 *    Rev 1.9   30 Oct 2015 08:52:08   sesharma
 * Added UpgradeFRAM_V3ToV4() to Upgrade and set default to parameter added in FRAM version 4
 * 
 *    Rev 1.8   30 Sep 2015 09:02:54   RJamloki
 * Changed default value of Clear Memory at Power up to TRUE
 * 
 *    Rev 1.7   28 Sep 2015 06:46:02   RJamloki
 * Conversion Pointer table fixed to have FRAM Version 1 to 2 upgrade routine.
 * 
 *    Rev 1.6   01 Sep 2015 02:30:54   amaurya
 * Changed for FRAM version upgrade.
 * 
 *    Rev 1.5   04 Jun 2015 07:38:06   sesharma
 * Re-organized Data in FRAM.
 * 
 *    Rev 1.4   19 Mar 2015 06:45:22   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.3   28 Oct 2014 16:58:44   ewomack
 * Add FRAM version number.
 * 
 *    Rev 1.2   16 Oct 2014 03:47:36   sesharma
 * Tracker Issues Resolved
 * 
 *    Rev 1.1   17 Sep 2014 08:49:00   amaurya
 * Code review Typedefs changes
 * 
 *    Rev 1.0   18 Aug 2014 09:17:24   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:58   amaurya
 * Initial revision.
 */

#include "DEF_DEF.h"
#include "TYP_DEF.h"
#include "SysConfig.h"
#include "LOG.h"
#include "UserIO.h"
#include "GLOBALS.h"
#include "FUNC.h"

UINT8 dummyPlaceHolder[433] __attribute__((section (".FRAM")));

//Current Version Number
UINT32 FRAM_Version __attribute__((section (".FRAM")));
UINT32 FRAM_Version_Inverted __attribute__((section (".FRAM")));

//Current Preset Name
SINT8 ParmName[21] __attribute__((section (".FRAM")));

//Current Preset number stored in FRAM
UINT8 CurrentWorkNumber __attribute__((section (".FRAM")));

//First Time Check Sum
//UINT32 FirstTimeCheck __attribute__((section (".FRAM")));

SINT8 DCPSoftwareVersionLabel[MAXPARAMNAME_LEN] __attribute__((section (".FRAM")));

//Presets stored in FRAM
Parameter storedPreset[MAX_WORK_NUMBER]__attribute__((section (".FRAM")));

//Scan Preset stored in FRAM
ScanParameter ParaScan __attribute__((section (".FRAM")));

//Various Counters stored in FRAM
SystemCounters DCPCounter __attribute__((section (".FRAM")));

CycleParam CurrentCycleParam __attribute__((section (".FRAM")));
CycleResults MFcycleResults __attribute__((section (".FRAM")));

//System Configuration data stored in FRAM
SystemConfiguration sysConfig __attribute__((section(".FRAM")));

//A sort of checksum to determine that whether the IOs are ever
//configured or not, should be stored here.
unsigned int UserIO::validIOConfiguration __attribute__((section(".FRAM")));

//USERIO configuration information
USERIOConfiguration USERIOConfig __attribute__ ((section (".FRAM")));

//Array of Event objects.
DCPLog Log::eventlogObjs[MAX_EVENT_ENTRIES] __attribute__ ((section (".FRAM")));

//Array of Alarm Event objects.
DCPLog Log::alarmlogObjs[MAX_ALARM_ENTRIES] __attribute__ ((section (".FRAM")));

//Log information object
DCPLOGInfo Log::LOGInfo __attribute__ ((section (".FRAM")));

//Total points of Last 5 second Weld data
SINT16 WeldBufferIndex __attribute__((section (".FRAM")));

//Number of Scan point collected
UINT16 NumScanPoints __attribute__((section (".FRAM")));

//Scan points stored in FRAM
ScanData ScanBuffer[SIZE_SCANBUFFER]__attribute__((section (".FRAM")));

//Last 5 Second Weld Data stored in FRAM
WeldData WeldBuffer[SIZE_WELDBUFFER]__attribute__((section (".FRAM")));


/*----------------------------FRAM Version change support----------------- --------------------------*/

void InitFRAM(void);
void UpgradeFRAM_V1ToV2(void);
void UpgradeFRAM_V2ToV3(void);
void UpgradeFRAM_V3ToV4(void);

typedef void (*FramPowerUpHandler)(void);
FramPowerUpHandler ConversionPointers[] = {
		NULL,					// Location 0 not used
		InitFRAM,				// Version 1 should force an FRAM initialization
		UpgradeFRAM_V1ToV2,	// Version 1 to version 2 conversion
		UpgradeFRAM_V2ToV3,	// Version 2 to version 3 conversion
		UpgradeFRAM_V3ToV4,	// Version 3 to version 4 conversion
};

/*void InitFRAM(void)
*  Purpose:
*      This function needs to be called to initialize the FRAM at least once when the system is programmed
*      for the very first time.  It also has to be called if the software is being down graded ( was running
*      version 3 and now ram version 1 has been detected).  Since backwards compatibility is not
*      supported, the FRAM will need to be initialized.
*/
void InitFRAM(void)
{
	ColdStart();
	ResetManFErrorCounter();
	ResetCounters();
	SetManFDefaultFromWeb();
}

/*  void UpgradeFRAM_V1ToV2(void)
 *
 *  Purpose:
 *    Upgrade FRAM_V1 To V2.
 *
 *  Entry condition:
 *    When old FRAM has Ver 1
 *
 *  Exit condition:
 *    None.
 */
void UpgradeFRAM_V1ToV2(void)
{
	InitFRAM();
}

/*  void UpgradeFRAM_V2ToV3(void)
 *
 *  Purpose:
 *    Upgrade FRAM_V2 To V3.
 *
 *  Entry condition:
 *    When old FRAM has Ver 2
 *
 *  Exit condition:
 *    None.
 */
void UpgradeFRAM_V2ToV3(void)
{
	RecallPreset(0);
	//Only change between V2 and V3 FRAM is added variables in SWDip Union without affecting the size of
	//any section in FRAM. This routine just put the default values for added parameters.
	CurrentPreset.SWDip.Flags.ClrMemAtPwrUp = TRUE;
	CurrentPreset.SWDip.Flags.SetDigTuneWithScan = FALSE;
	SavePreset(0);
}

/*  void UpgradeFRAM_V3ToV4(void)
 *
 *  Purpose:
 *    Upgrade FRAM_V3 To V4.
 *
 *  Entry condition:
 *    When old FRAM has Ver 3
 *
 *  Exit condition:
 *    None.
 */
void UpgradeFRAM_V3ToV4(void)
{
	RecallPreset(0);
	//Only change between V3 and V4 FRAM is added variables in SWDip Union without affecting the size of
	//any section in FRAM. This routine just put the default values for added parameters.
	CurrentPreset.SWDip.Flags.HandHeldMode = FALSE;
	SavePreset(0);
}

/*void FramCheck(void)
 *	Purpose:
 * 		This function is called from PowerUpInitilization. It checks for the FRAMVersion stored
 * 		in FRAM.
 * 		If the stored FRAMVersion is invalid, InitFRAM will be called.
 * 		If the stored FRAMversion is less than power up FRAMVersion needed, then there will be software
 * 		upgrade till the required version.
 * 		If the stored FRAMVersion is greater than power up FRAMVersion needed, then there
 * 		will be software degrade, InitFRAM will be called.
 *********************************************************************************************/
void FramCheck(void)
{
	UINT16 ConversionArrayIndx;
	SINT32 Indx;
	if((FRAM_VERSION == 1) && (FRAM_Version != INIT_VERSION)){ //first time
		ConversionArrayIndx = INIT_FRAM_INDEX;
		ConversionPointers[ConversionArrayIndx]();
	}
	else if(FRAM_Version < FRAM_VERSION){//upgrade
		ConversionArrayIndx = FRAM_Version + 1;
		for(Indx = ConversionArrayIndx; Indx <= FRAM_VERSION; Indx++ )
			ConversionPointers[Indx]();
	}
	else if(FRAM_Version > FRAM_VERSION){//downgrade
		ConversionArrayIndx = INIT_FRAM_INDEX;
		ConversionPointers[ConversionArrayIndx]();
	}
	FRAM_Version = FRAM_VERSION;
	FRAM_Version_Inverted = FRAM_VERISON_INVERTED;
}
