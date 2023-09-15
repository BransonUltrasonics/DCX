// $Header:
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements watchdog.

#ifndef WATCHDOG_CPP_
#define WATCHDOG_CPP_

#include "WatchDog.h"
#include "CPUdefs.h"
#include "CPU.h"
#include "stdio.h"
#include "assert.h"
#include "Terminal.h"
#include "CyGOS.h"
#include "RTOS.h"
#include "Board.h"
extern TaskIntLoadCallBack LoadLogCallBack;
int TaskPriority, VectorNumber;
unsigned int MaxTaskTime, MaxVectorTime;
extern int LastInterruptLevel;
WatchDog::WatchDog()
{

}

WatchDog::~WatchDog()
{

}
//extern int currentTime11;
void WatchDog::InitWatchDog(int Cwcr_CWT)
{
	//Core watchdog time-out period.
	if(Cwcr_CWT < 8 && Cwcr_CWT > 31)
		Cwcr_CWT = 28;
	MCF_SCM_CWCR = MCF_SCM_CWCR_CWT(Cwcr_CWT);
	//Core watchdog reset/interrupt
	MCF_SCM_CWCR |= MCF_SCM_CWCR_CWRI(1);
    CPU::SetupHandler(IntHandler, intWatchdogTimer, 7, 0, 128);
}

void WatchDog::EnableWatchDog(bool CanDisable)
{
	MCF_SCM_CWCR |= MCF_SCM_CWCR_CWE;
	// Make CWCR register Read Only..
	if(!CanDisable)
		MCF_SCM_CWCR |= MCF_SCM_CWCR_RO;
}

void WatchDog::DisableWatchDog()
{
	MCF_SCM_CWCR &= (~MCF_SCM_CWCR_CWE);
}

void WatchDog::kickWatchDog()
{
	MCF_SCM_CWSR = MCF_SCM_CWSR_CWSR(0x55);
	MCF_SCM_CWSR = MCF_SCM_CWSR_CWSR(0xAA);
}

void WatchDog::IntHandler(int level, void * frame)
{
    MCF_SCM_SCMISR |= MCF_SCM_SCMISR_CWIC;
//    int currentTime = Board::GetProfilerTimerCounter(0);
//    pprintf("\n\n Time diff = %d \n\n ",currentTime - currentTime11);

//    TaskPriority = VectorNumber = MaxTaskTime = MaxVectorTime = 0;
    if (level == IntVector(intWatchdogTimer)){
    	CyGOS::WriteSystemEvent(WD_ERROR, (unsigned short)MCF5XXX_RD_SF_FS(frame) , (unsigned short) MCF5XXX_SF_SR(frame), level, true);
    	CyGOS::WriteSystemEvent(WD_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), level, true);
		CyGOS::WriteSystemEvent(WD_ERROR, RTOS::taskHighest->priority,RTOS::taskCurrent->priority,LastInterruptLevel, true);
		CyGOS::LogTaskLoad();
		CPU::Restart();
    }
	else {
		ASSERT(0);
		ASSERT(frame);
	}
}


#endif /* WATCHDOG_CPP_ */
