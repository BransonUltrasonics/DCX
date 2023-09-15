/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/FUNC.h_v   1.21   04 Mar 2014 07:49:22   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/FUNC.h_v  $
 * 
 *    Rev 1.21   04 Mar 2014 07:49:22   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
 * 
 *    Rev 1.20   03 Sep 2013 12:38:42   ygupta
 * Moved to tip from 1.18.1.1
 * 
 *    Rev 1.18.1.1   03 Sep 2013 11:14:06   amaurya
 * Tracker fixes and Synchronization fixes for User Outputs.
 * 
 *    Rev 1.18.1.0   24 Aug 2013 17:14:02   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.18   17 Jul 2013 06:19:04   amaurya
 * Fixed WeldTime Issue on History page and added diagnostic code.
 * 
 *    Rev 1.17   21 May 2013 12:25:50   amaurya
 * Code review fixes.
 * 
 *    Rev 1.15   06 May 2013 09:15:34   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.14   08 Apr 2013 08:23:12   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.13   15 Mar 2013 10:48:50   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.12   11 Mar 2013 02:25:22   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.11   18 Feb 2013 03:06:06   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.10   29 Jan 2013 02:19:10   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.9   22 Jan 2013 10:38:16   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.8   17 Jan 2013 07:25:00   ygupta
 * Profibus Related changes
 * 
 *    Rev 1.7   16 Jan 2013 06:25:36   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.6   26 Oct 2012 02:34:44   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.4   21 Sep 2012 01:31:00   rjamloki
 * Added State machine
 * 
 *    Rev 1.3   05 Jul 2012 04:45:24   ygupta
 * Cleanup after review summary on database and BSP.
 * 
 *    Rev 1.2   02 Jul 2012 13:04:08   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.1   29 Jun 2012 12:13:44   ygupta
 * First Compilable code, First check in after review, Needs more clanup
 * 
 *    Rev 1.0   21 Mar 2012 12:20:36   PDwivedi
 * Initial revision.
 *
 */

#ifndef FUNC_H_
#define FUNC_H_

#include "TYP_DEF.h"

UINT16 ByteCRC(UINT8 *Ptr, UINT32 Length);

/* WCTask.cpp */
void PowerUpInitialization(void) ;

///*****preset.c*********/
//BOOLEAN SavePreset(SINT32 PresetNum);
//BOOLEAN RecallPreset(SINT32 PresetNum);
//BOOLEAN VerifyPreset(void);
void UpdateRunningPreset(void);
void ColdStart(void);
void ReadMACAddressFromMacChip(UINT8 * macPtr);
void SetDHCPServerIPDefaults(void);
void SetStaticIPDefaults(void);
void SetFBStaticIPDefaults(void);
////
void ModifyMACCHIPRWData(void);
UINT32 ReadManFErrorCounter(void);
void ResetManFErrorCounter(BOOLEAN ResetAlarmFromSysTest = FALSE);
void ResetCounters(void);
void SonicsOff(void);
void DetectResonance(void);
//
void StoreWeldData(void);
void CollectScanPoints(void);
void SetSystemConfigurationDefaults(void);
void UpdateMidBandAndMinMaxFreq(void);
void SetStaticIPDefaultsOnDCPInterface(void);
void InitGlobalBuffers(void);
void UpdateFlashCRC(void);
void IncrementManFErrorCycleCounter(void);
BOOLEAN GetAccessToken(CommIfc CalledFromCommIfc);
BOOLEAN IsAccessTokenAvailable(void);
BOOLEAN IsManualFunctionAvailable(void);
BOOLEAN ReleaseAccessToken(CommIfc CalledFromCommIfc);
void SetDCPTimeControlParams();
void CheckForRTCBattery(void);
void UpdatePeakPower(void);
void UpdateFrequencyOffset(void);
void SetupMinMaxPresetIndexes(void);
void UpdateTPVal(int TaskPriority);
#endif /* FUNC_H_ */
