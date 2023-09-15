/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/FRAM.cpp_v   1.6   09 Sep 2013 08:59:50   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/FRAM.cpp_v  $
 * 
 *    Rev 1.6   09 Sep 2013 08:59:50   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.5   15 Jan 2013 04:39:44   amaurya
 * Energy Special.
 * 
 *    Rev 1.4   15 Jul 2011 09:45:02   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.3   19 Apr 2011 10:46:52   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.2   04 Apr 2011 10:58:32   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.1   02 Mar 2011 08:32:38   PDwivedi
 * Added placeholder
 * 
 *    Rev 1.0   29 Nov 2010 07:50:00   PDwivedi
 * Initial revision.
******************************************************************************/

#include "DEF_DEF.h"
#include "TYP_DEF.h"
#include "SysConfig.h"
#include "LOG.h"
#include "UserIO.h"

unsigned char dummyPlaceHolder[438] __attribute__((section (".FRAM")));

//Current Preset Name
SINT8 ParmName[21] __attribute__((section (".FRAM")));

//Current Preset number stored in FRAM
UINT8 CurrentWorkNumber __attribute__((section (".FRAM")));

//Total points of Last 5 second Weld data
SINT16 WeldBufferIndex __attribute__((section (".FRAM")));

//Number of Scan point collected
UINT16 NumScanPoints __attribute__((section (".FRAM")));

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
USERIOConfiguration  USERIOConfig __attribute__ ((section (".FRAM")));

//Scan points stored in FRAM
ScanData ScanBuffer[SIZE_SCANBUFFER]__attribute__((section (".FRAM")));

//Last 5 Second Weld Data stored in FRAM
WeldData WeldBuffer[SIZE_WELDBUFFER]__attribute__((section (".FRAM")));

//Array of Event objects.
DCPLog Log::eventlogObjs[MAX_EVENT_ENTRIES] __attribute__ ((section (".FRAM")));

//Array of Alarm Event objects.
DCPLog Log::alarmlogObjs[MAX_ALARM_ENTRIES] __attribute__ ((section (".FRAM")));

//Log information object
DCPLOGInfo Log::LOGInfo __attribute__ ((section (".FRAM")));
