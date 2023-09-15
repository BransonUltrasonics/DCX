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

WatchDog::WatchDog()
{

}

WatchDog::~WatchDog()
{

}

void WatchDog::InitWatchDog()
{
	//Core watchdog time-out period.
	MCF_SCM_CWCR = MCF_SCM_CWCR_CWT(25);

	//Core watchdog reset/interrupt
	MCF_SCM_CWCR |= MCF_SCM_CWCR_CWRI(1);
}

void WatchDog::EnableWatchDog()
{
	MCF_SCM_CWCR |= MCF_SCM_CWCR_CWE;
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

#endif /* WATCHDOG_CPP_ */
