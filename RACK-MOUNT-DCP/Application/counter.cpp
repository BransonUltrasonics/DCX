/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/counter.cpp_v   1.1   19 Mar 2015 06:37:00   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/counter.cpp_v  $
 * 
 *    Rev 1.1   19 Mar 2015 06:37:00   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.0   18 Aug 2014 09:17:22   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:58   amaurya
 * Initial revision.
 */

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
	return (DCPCounter.CycleCounter);
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
	if (DCPCounter.CycleCounter > MAX_CYCLE_COUNTER) {
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
	return (DCPCounter.ErrorCounter);
}

UINT32 ReadManFErrorCounter(void)
/******************************************************************************/
/* ReadErrorCounter()									      		          			*/
/* Description:-                                                            	*/
/* In this function ,Manufacturing Error counter value is read from FRAM and  */
/*	returned  returns Error counter value stored in FRAM.							   */
/******************************************************************************/
{
	return (MFcycleResults.nAlarms);
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
	if ((DCPCounter.ErrorCounter) > MAX_ERROR_COUNTER) {
		DCPCounter.ErrorCounter = 0;
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
	MFcycleResults.nAlarms++;
	if ((MFcycleResults.nAlarms) > MAX_ERROR_COUNTER) {
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
	return (DCPCounter.PowerOnSecond);
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
	if (DCPCounter.PowerOnSecond > MAX_SEC_COUNTER) {
		DCPCounter.PowerOnSecond = 0;
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
	DCPCounter.SecondsOfSonic = 0;
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
	return (DCPCounter.SecondsOfSonic);
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
	if ((DCPCounter.SecondsOfSonic) > MAX_SEC_COUNTER) {
		DCPCounter.SecondsOfSonic = 0;
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
	if (DCPCounter.CRC != COUNTER_CRC) {
		ResetCounters();
	}
	DCPCounter.CRC = COUNTER_CRC;
}
