/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/GLOBALS.cpp_v   1.27   24 Mar 2014 14:06:34   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/GLOBALS.cpp_v  $
 * 
 *    Rev 1.27   24 Mar 2014 14:06:34   rjamloki
 * Removed redundant ram buffers.
 * 
 *    Rev 1.26   13 Mar 2014 15:54:36   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.25   04 Mar 2014 07:49:22   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
 * 
 *    Rev 1.24   08 Oct 2013 06:17:36   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.23   20 Sep 2013 07:00:12   rjamloki
 * Added changes to build bootloader with same CyGOS.
 * 
 *    Rev 1.22   24 Aug 2013 17:14:02   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.21   21 May 2013 12:25:50   amaurya
 * Code review fixes.
 * 
 *    Rev 1.20   06 May 2013 09:15:48   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.19   30 Apr 2013 11:19:12   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.18   15 Mar 2013 10:49:02   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.17   18 Feb 2013 03:06:18   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.16   29 Jan 2013 02:19:24   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.15   22 Jan 2013 10:38:18   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.14   16 Jan 2013 06:25:50   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.13   11 Jan 2013 02:50:48   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.12   27 Dec 2012 06:43:24   ygupta
 * Changes to fix Issues
 * 
 *    Rev 1.11   18 Dec 2012 07:37:12   ygupta
 * Changes for Multiple Overload alarm in case of 15V lost
 * 
 *    Rev 1.10   13 Dec 2012 00:09:16   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.9   29 Nov 2012 08:44:16   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.8   05 Nov 2012 08:56:06   amaurya
 * State Machine changes
 * 
 *    Rev 1.7   26 Oct 2012 02:34:58   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.5   21 Sep 2012 01:31:14   rjamloki
 * Added State machine
 * 
 *    Rev 1.4   10 Sep 2012 03:11:36   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.3   29 Aug 2012 13:27:48   rjamloki
 * WC Modbus checkin and related changes
 * 
 *    Rev 1.2   02 Jul 2012 13:04:08   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.1   29 Jun 2012 12:13:56   ygupta
 * First Compilable code, First check in after review, Needs more clanup
 * 
 *    Rev 1.0   21 Mar 2012 12:20:40   PDwivedi
 * Initial revision.
 *
 */

#include "DEF_DEF.h"
#include "TYP_DEF.h"
#include "preset.h"
#include "StateMachineDrv.h"

SINT32 InputTemperature = 0;
SINT32 BoardTestFrequency = 0;
UINT8  BoardTestOutput = 0;
CycleParam TempCurrentCycleParam;
UINT16 WeldBufferTime = 0;

/*****************State machine variables********************/
UINT32 Midband = 0;
UINT32   MinFrequency = 0;
UINT32   MaxFrequency = 0;
SINT32  MaxOffset = 0;
BOOLEAN MemoryReset = FALSE;
BOOLEAN WebIOEnabled = FALSE;

////////////////Manufacturing///////////////////
CycleParam MinCycleParam = {0, 0 ,0 ,0, 0, 0 ,0};
CycleParam MaxCycleParam = {0, 0 ,0 ,0, 0, 0 ,0};

UINT32 ManFCycleCounterInc = 0;
UINT32 ManFErrorCycleCounterInc = 0;
BOOLEAN NoInput = FALSE;
UINT32 SonicsOnCounter = 0;
UINT32 SonicsOffCounter = 0;
UINT32 TempSonicsOnTimePerWeldCycle = 0;
/*********************************************************/
//EEPROM Data

SINT32 MinPSNum = 0, MaxPSNum = 0, MinHSPSNum = 0, MaxHSPSNum = 0;
//Total points of Last 5 second Weld data
SINT16 WeldBufferIndex = 0;

//dummy data for pages
//Number of Scan point collected
SINT16 NumScanPoints = 0;
//Scan points stored in RAM
ScanData * ScanBuffer = 0;
//Last 5 Second Weld Data stored in RAM
WeldData * WeldBuffer = 0;


DCXFUNC DCXFunc = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
//DCXHORNFUNC DCXHornFunc;
//The gobal variable value represents which interface currently have access token
CommIfc CommIfcAccessToken = COMM_IFC_NO_IFC;
//The global variable contains the value of current operating mode
OperatingMode OpMode = NOMODE;

DCPTimeControl DCPTimeControlParam = {0, 0, 0, 0, 0, 0, 0};


//The debug section remvoed from FRAM and put into RAM. These variables are accessed by CyGOS
//At crash the information from these varibales moved to FRAM in StoreInfoAtCrash function
TestPoints TPoints;
unsigned int CrashValid;
char TaskName[TASKNAMELEN];
unsigned int CrashBuffer[4096 + 35];
unsigned int CurrStackPtr;
unsigned int Stackbegin;
unsigned int StackEnd;
unsigned int CrashBuffPrintLen;
unsigned int MMUAR;
