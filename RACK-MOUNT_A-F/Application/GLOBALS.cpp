/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/GLOBALS.cpp_v   1.1   24 Jul 2015 08:35:52   sesharma  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/GLOBALS.cpp_v  $
 * 
 *    Rev 1.1   24 Jul 2015 08:35:52   sesharma
 * Added function pointers used for common binary changes.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:32   tayars
 * Initial revision.
 * 
 *    Rev 1.27   24 Mar 2014 14:06:34   rjamloki
 * Removed redundant ram buffers.
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

//The debug section removed from FRAM and put into RAM. These variables are accessed by CyGOS
//At crash the information from these variables moved to FRAM in StoreInfoAtCrash function
WCBOARD BoardType;

//Board specific functions
void (* InitExternalWatchDogTrigger)(void);
void (* ConfigDigitalOutputLatch)(void);
void (* ConfigureLCDRst)(void);
void (* SendResetToLCD)(void);
void (* ConfigureResetPHYandHilscher)(void);
void (* SendResetToPHYandHilscher)(void);
void (* InitBDMPins)(void);
void (* SetReset)(int);
void (* SetBkgd)(int);
int  (* GetBkgd)(void);
void (* SetBkgdOut)(void);
void (* SetBkgdIn)(void);


