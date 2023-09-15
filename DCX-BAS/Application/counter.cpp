/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/counter.cpp_v   1.8   13 Mar 2014 15:53:46   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/counter.cpp_v  $
 * 
 *    Rev 1.8   13 Mar 2014 15:53:46   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.7   24 Aug 2013 17:13:56   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.6   31 May 2013 10:37:12   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.5   08 Apr 2013 08:21:18   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.4   11 Mar 2013 02:23:22   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.3   18 Feb 2013 03:04:22   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.2   29 Jan 2013 02:18:44   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.1   16 Jan 2013 06:23:56   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.0   26 Oct 2012 02:52:28   rjamloki
 * Initial revision.
 * 
 *    Rev 1.11   16 Sep 2011 09:46:02   RatneshA
 * counters get cleared from manufacturing page only and not from coldstart.
 * 
 *    Rev 1.10   16 Sep 2011 08:26:44   RatneshA
 * cold start would not clear the lifecycle counters
 * 
 *    Rev 1.9   16 Aug 2011 08:21:46   pdwivedi
 * code cleanup
 * 
 *    Rev 1.8   02 Aug 2011 07:36:22   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.7   18 Jul 2011 09:24:48   pdwivedi
 * Added changes for serial buffer size.
 * 
 *    Rev 1.6   15 Jul 2011 09:44:14   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.5   20 Apr 2011 11:36:42   ygupta
 * Added changes for tracker issues.
 * 
 *    Rev 1.4   04 Apr 2011 10:58:28   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.3   29 Nov 2010 07:47:32   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.2   22 Nov 2010 09:04:22   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.1   15 Nov 2010 08:47:10   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:40   ygupta
 * Initial revision.

******************************************************************************/

#include "GLOBALS.h"
#include "FUNC.h"
#include "CRC.h"
#include "preset.h"

#define printf
#define pprintf

void ResetManFErrorCounter(BOOLEAN ResetAlarmFromSysTest)
/******************************************************************************/
/* ResetErrorCounter()														  					*/
/* Description:-                                                            	*/
/* In this function ,Manufacturing Error counter in intiliazed to zero.       */
/*																			  							*/
/******************************************************************************/
{
	MFcycleResults->AlarmsCount = 0;
	MFcycleResults->CyclesCount = 0;
	if(ResetAlarmFromSysTest){
		SysCounters->GeneralAlarmCount = 0;
		SysCounters->LifeTimeCycleCount = 0;
		SysCounters->PowerOnHours = 0;
		SysCounters->SonicsOnHours = 0;
	}
}

UINT32 ReadManFErrorCounter(void)
/******************************************************************************/
/* ReadErrorCounter()									      		          			*/
/* Description:-                                                            	*/
/* In this function ,Manufacturing Error counter value is read from FRAM and  */
/*	returned  returns Error counter value stored in FRAM.							   */
/******************************************************************************/
{
	return(MFcycleResults->AlarmsCount);
}

void IncrementManFErrorCycleCounter(void)
/******************************************************************************/
/* IncrementErrorCounter()									      	          			*/
/* Description:-                                                            	*/
/* In this function ,Manufacturing Error counter in incremented until it      */
/*	reaches maximum. 																            */
/******************************************************************************/
{
	MFcycleResults->AlarmsCount ++;
	if((MFcycleResults->AlarmsCount) > MAX_ERROR_COUNTER)
		MFcycleResults->AlarmsCount = 0;
}
