/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/counter.cpp_v   1.11   16 Sep 2011 09:46:02   RatneshA  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/counter.cpp_v  $
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

#include "ReceiveCommandHandler.h"
#include "GLOBALS.h"
#include "FUNC.h"
#include "CRC.h"

void ResetCycleCounter(void)
/******************************************************************************/
/* ResetCycleCounter()											              				*/
/* Description:-                                                            	*/
/* In this function ,Cycle counter is intiliazed to zero. 		             	*/
/*																              						*/
/******************************************************************************/
{
	DCPCounter.CycleCounter = 0;
}

UINT32 ReadCycleCounter(void)
/******************************************************************************/
/* ReadCycleCounter()									      		          			*/
/* Description:-                                                            	*/
/* In this function ,Cycle counter value is read from FRAM and returned       */
/*	returns cycle counter value.															   */
/******************************************************************************/
{
	return(DCPCounter.CycleCounter);
}

void IncrementCycleCounter(void)
/******************************************************************************/
/* IncrementCycleCounter()									      		      			*/
/* Description:-                                                            	*/
/* In this function ,Cycle counter in incremented until it reaches maximum.   */
/*																		      						*/
/******************************************************************************/
{
	DCPCounter.CycleCounter++;
	if(DCPCounter.CycleCounter > MAX_CYCLE_COUNTER)
	{
		DCPCounter.CycleCounter = 0;
	}
}

void ResetErrorCounter(void)
/******************************************************************************/
/* ResetErrorCounter()														  					*/
/* Description:-                                                            	*/
/* In this function ,Error counter in intiliazed to zero. 		               */
/*																			  							*/
/******************************************************************************/
{
	DCPCounter.ErrorCounter = 0;
}

void ResetManFErrorCounter(void)
/******************************************************************************/
/* ResetErrorCounter()														  					*/
/* Description:-                                                            	*/
/* In this function ,Manufacturing Error counter in intiliazed to zero.       */
/*																			  							*/
/******************************************************************************/
{
	MFcycleResults.nAlarms = 0;
	MFcycleResults.nCycles = 0;
}

UINT32 ReadErrorCounter(void)
/******************************************************************************/
/* ReadErrorCounter()									      		          			*/
/* Description:-                                                            	*/
/* In this function ,Error counter value is read from FRAM and returned       */
/*	returns Error counter value stored in FRAM.														   	*/
/******************************************************************************/
{
	return(DCPCounter.ErrorCounter);
}

UINT32 ReadManFErrorCounter(void)
/******************************************************************************/
/* ReadErrorCounter()									      		          			*/
/* Description:-                                                            	*/
/* In this function ,Manufacturing Error counter value is read from FRAM and  */
/*	returned  returns Error counter value stored in FRAM.							   */
/******************************************************************************/
{
	return(MFcycleResults.nAlarms);
}

void IncrementErrorCounter(void)
/******************************************************************************/
/* IncrementErrorCounter()									      	          			*/
/* Description:-                                                            	*/
/* In this function ,Error counter in incremented until it reaches maximum.   */
/*																	          						*/
/******************************************************************************/
{
	DCPCounter.ErrorCounter++;
	if((DCPCounter.ErrorCounter) > MAX_ERROR_COUNTER)
	{
		DCPCounter.ErrorCounter=0;
	}
}

void IncrementManFErrorCycleCounter()
/******************************************************************************/
/* IncrementErrorCounter()									      	          			*/
/* Description:-                                                            	*/
/* In this function ,Manufacturing Error counter in incremented until it      */
/*	reaches maximum. 																            */
/******************************************************************************/
{
	MFcycleResults.nAlarms ++;
	if((MFcycleResults.nAlarms) > MAX_ERROR_COUNTER)
	{
		MFcycleResults.nAlarms = 0;
	}
}

void ResetPowerOnSecCounter(void)
/******************************************************************************/
/* ResetPowerOnSecCounter()												      			*/
/* Description:-                                                            	*/
/* In this function ,Power On Second counter in intiliazed to zero. 	      	*/
/*																	          						*/
/******************************************************************************/
{
	DCPCounter.PowerOnSecond = 0;
}

UINT32 ReadPowerOnSecCounter(void)
/******************************************************************************/
/* ReadPowerOnSecCounter()									      		               */
/* Description:-                                                            	*/
/* In this function ,Power On Second counter value is read from FRAM          */
/*	and returned.																		         */
/* returns Power On Second Counter value from FRAM.									*/
/******************************************************************************/
{
   return(DCPCounter.PowerOnSecond);
}

void IncrementPowerOnSecCounter(void)
/******************************************************************************/
/* IncrementPowerOnSecCounter()									      	      		*/
/* Description:-                                                            	*/
/* In this function ,Power on Second counter in incremented until             */
/*	it reaches maximum.													      				*/
/******************************************************************************/
{
	DCPCounter.PowerOnSecond++;
	if(DCPCounter.PowerOnSecond > MAX_SEC_COUNTER)
	{
		DCPCounter.PowerOnSecond=0;
	}
}

void ResetSonicOnSecCounter(void)
/******************************************************************************/
/* ResetSonicOnSecCounter()											          			*/
/* Description:-                                                            	*/
/* In this function ,Sonics On Second counter in initialized to zero.          */
/*																		      						*/
/******************************************************************************/
{
	DCPCounter.SecondsOfSonic=0;
}

UINT32 ReadSonicOnSecCounter(void)
/******************************************************************************/
/* ReadSonicOnSecCounter()									      	          			*/
/* Description:-                                                            	*/
/* In this function ,Sonics On Second counter value is read from FRAM         */
/*	and returned.													          					*/
/* returns SonicOn Second Counter value from FRAM.										*/
/******************************************************************************/
{
   return(DCPCounter.SecondsOfSonic);
}

void IncrementSonicOnSecCounter()
/******************************************************************************/
/* IncrementSonicOnSecCounter()									              			*/
/* Description:-                                                            	*/
/* In this function ,Sonics on Second counter in incremented until            */
/*	it reaches maximum.												          				*/
/******************************************************************************/
{
	DCPCounter.SecondsOfSonic++;
	if((DCPCounter.SecondsOfSonic) > MAX_SEC_COUNTER)
	{
		DCPCounter.SecondsOfSonic=0;
	}
}

void ResetCounters()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for initializing counter values in FRAM to  */
/* zero.                                                                    */
/****************************************************************************/
{
	pprintf("\n Reset Counters value");
	//Cycle Counter reset

	ResetCycleCounter();

	//Error Counter reset
	ResetErrorCounter();

	DCPCounter.PowerOnMilliseconds = 0;

	//Power On Sec Counter reset
	ResetPowerOnSecCounter();

	//Sonic On Milli Sec Counter reset
	DCPCounter.MillisecondOfSonic = 0;

	ResetSonicOnSecCounter();
	DCPCounter.CRC = COUNTER_CRC;

}

void CheckDCPCounterCRC()
/******************************************************************************/
/* Description:-                                                              */
/* This function is responsible for calculating CRC for counter structure,if  */
/* calculated CRC is not equal to stored CRC , then counters are initialized. */
/******************************************************************************/
{
	 pprintf("DCPCounter.CRC %08X", DCPCounter.CRC);
    if(DCPCounter.CRC != COUNTER_CRC)
    {
   	 ResetCounters();
    }
    DCPCounter.CRC = COUNTER_CRC;
}
