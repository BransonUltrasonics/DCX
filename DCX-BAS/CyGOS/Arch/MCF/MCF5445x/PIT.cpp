// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/PIT.cpp_v   1.0   21 Mar 2012 12:31:14   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements processor-specific Periodic Interval Timer. In
// different processor families this peripheral device may be called
// differently, such as slice timer in MCF548x processors. The actual
// implementation of the device will use closest peripheral device that is
// capable of generating periodic interrupts.

#include "PIT.h"
#include "CPU.h"
#include "CPUdefs.h"
#include "ArchDefs.h"
#include "assert.h"
#include "DebugNew.h"

/**
 * pit: Array of pointers to all PIT objects.
 */
PIT * PIT::pit[4] =
{ 0, 0, 0, 0 };

/**
 * PIT: Initialize slice timer to generate periodic interrupts.
 * @param channel Programmable interrupt timer channel number (0-3)
 * @param period Timer period in clock cycles
 * @param intlevel Interrupt level for the timer (1..7)
 * @param intpriority Interrupt priority within level (0..7)
 * @param handler Interrupt handler for the timer
 */
PIT::PIT(int channel, int period, int intlevel, int intpriority,
		PitCallback callback, int stackSize)
{
	ASSERT((channel >= 0) || (channel <= 3));
	ASSERT(pit[channel] == 0);
	pit[channel] = this;
	callbackFunction = callback;
	int modulus = period;
	int prescaler = 0;
	while (modulus > 0xFFFF)
	{
		modulus >>= 1;
		prescaler++;
	}
	ASSERT(prescaler <= 15);
	switch (channel)
	{
	case 0:
		CPU::SetupHandler(IntHandler0, intPIT0, intlevel, intpriority,
				stackSize);
		break;
	case 1:
		CPU::SetupHandler(IntHandler1, intPIT1, intlevel, intpriority,
				stackSize);
		break;
	case 2:
		CPU::SetupHandler(IntHandler2, intPIT2, intlevel, intpriority,
				stackSize);
		break;
	case 3:
		CPU::SetupHandler(IntHandler3, intPIT3, intlevel, intpriority,
				stackSize);
		break;
	}

	MCF_PIT_PMR(channel) = modulus;
	MCF_PIT_PCSR(channel) = 0 | MCF_PIT_PCSR_PRE(prescaler) | MCF_PIT_PCSR_PIE
			| MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_EN;
}

/**
 * ResetInt: Reset a PIT channel interrupt.
 * @param channel Channel number, 0...3.
 */
void PIT::ResetInt(int channel)
{
	ASSERT((channel >= 0) || (channel <= 3));
	// Timer runs continuously - immediately reloaded on timeout
	unsigned short pcsr = MCF_PIT_PCSR(channel);
	MCF_PIT_PCSR(channel) = pcsr | MCF_PIT_PCSR_PIF;
}

void PIT::StartInt(int channel)
{
	ASSERT((channel >= 0) || (channel <= 3));
	ASSERT(!(pit[channel] == 0));
	// Timer runs continuously - immediately reloaded on timeout
	MCF_PIT_PCSR(channel) |= MCF_PIT_PCSR_EN;
	ResetInt(channel);
}

void PIT::StopInt(int channel)
{
	ASSERT((channel >= 0) || (channel <= 3));
	// Timer runs continuously - immediately reloaded on timeout
	ASSERT(!(pit[channel] == 0));
	MCF_PIT_PCSR(channel) &= ~MCF_PIT_PCSR_EN;
}

/**
 * IntHandler0: PIT channel 0 interrupt handler.
 * @param vector CPU interrupt vector (unused).
 * @param frame CPU interrupt frame pointer (unused).
 */
void PIT::IntHandler0(int vector, void * frame)
{
	ASSERT(vector == IntVector(intPIT0));
	ASSERT(pit[0] != 0);
	ResetInt(0);
	pit[0]->callbackFunction();
	return;
	ASSERT(frame);
}

/**
 * IntHandler1: PIT channel 1 interrupt handler.
 * @param vector CPU interrupt vector (unused).
 * @param frame CPU interrupt frame pointer (unused).
 */
void PIT::IntHandler1(int vector, void * frame)
{
	ASSERT(vector == IntVector(intPIT1));
	ASSERT(pit[1] != 0);
	ResetInt(1);
	pit[1]->callbackFunction();
	return;
	ASSERT(frame);
}

/**
 * IntHandler2: PIT channel 2 interrupt handler.
 * @param vector CPU interrupt vector (unused).
 * @param frame CPU interrupt frame pointer (unused).
 */
void PIT::IntHandler2(int vector, void * frame)
{
	ASSERT(vector == IntVector(intPIT2));
	ASSERT(pit[2] != 0);
	ResetInt(2);
	pit[2]->callbackFunction();
   return;
   ASSERT(frame);
}

/**
 * IntHandler3: PIT channel 3 interrupt handler.
 * @param vector CPU interrupt vector (unused).
 * @param frame CPU interrupt frame pointer (unused).
 */
void PIT::IntHandler3(int vector, void * frame)
{
	ASSERT(vector == IntVector(intPIT3));
	ASSERT(pit[3] != 0);
	ResetInt(3);
	pit[3]->callbackFunction();
   return;
   ASSERT(frame);
}
