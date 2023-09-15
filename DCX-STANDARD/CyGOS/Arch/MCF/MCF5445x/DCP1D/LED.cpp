// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/DCP1D/LED.cpp_v   1.1   01 Sep 2011 13:25:56   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object encapsulates board-specific LED handling.
//
// TODO Everything is copied from M54455EVB, only flash and SDRAM settings are verified. The rest must be checked.

#include "LED.h"
#include "MMU.h"
#include "CPU.h"
#include "assert.h"
#include "stdio.h"

volatile char * LED::lptr;

/**
 * Init: Initialize LED outputs. Just in case, program CPLD chip select if noone
 * has done it yet.
 */
void LED::Init()
{
	// 08000000 matches all M54455EVB documentation, so DCP1B will use the same
	// setting.
	lptr = (char *) 0x08000000;
	MMU::Map((int) lptr);
	CPU::SetupCS8bit(2, 20, 8, lptr);
	*lptr = 0xFF;
}

/**
 * Set: Set specified LED to specified state. DS5 is LED1 and DS6 is LED2.
 * @param led LED number, 0 - all, or 1...2
 * @param state LED state 0 - Off, 1 - Red, 2 - Green, 3 - Yellow
 */
void LED::Set(int led, int state)
{
	ASSERT((led >= 0) && (led <= 2));
	ASSERT((state >= 0) || (state <= 3));

	if (led == 0)
		*lptr = state | (state << 2);
	else
	{
		led--;
		led <<= 1;
		*lptr &= ~(3 << led);
		*lptr |= state << led;
	}
}

/**
 * SetAll: Set all LEDs to specified states.
 * @param state LED state buffer. Only 4 LSB from first byte are used.
 */
void LED::SetAll(unsigned char * state)
{
	*lptr = *state;
}
