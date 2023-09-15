// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Arch/MCF/MCF5445x/PIT.h_v   1.0   18 Aug 2014 09:15:38   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares processor-specific Periodic Interval Timer. In different
// processor families this peripheral device may be called differently, such as
// slice timer in MCF548x processors. The actual implementation of the device
// will use closest peripheral device that is capable of generating periodic
// interrupts.

#ifndef PIT_H_
#define PIT_H_

#include "Exception.h"

/**
 * PitCallback: A callback function for the programmable interrupt timer.
 */
typedef void (*PitCallback)();

/**
 * class PIT: Programmable interrupt timer implementation. In different
 * processor families this peripheral device may be called differently, such as
 * slice timer in MCF548x processors. The actual implementation of the device
 * will use closest peripheral device that is capable of generating periodic
 * interrupts.
 */
class PIT
{
public:
	PIT(int channel, int period, int intlevel, int intpriority,
			PitCallback callback, int stackSize);
	static void StartInt(int channel);
	static void StopInt(int channel);
	static void Remove(int channel);
protected:
	static void ResetInt(int channel);
	static void IntHandler0(int vector, void * frame);
	static void IntHandler1(int vector, void * frame);
	static void IntHandler2(int vector, void * frame);
	static void IntHandler3(int vector, void * frame);
	/**
	 * callbackFunction: A function to call every time when PIT generates an
	 * interrupt.
	 */
	PitCallback callbackFunction;
	static PIT * pit[4];
};

#endif
