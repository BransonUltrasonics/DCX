// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/WC/LED.h_v   1.0   21 Mar 2012 12:33:30   PDwivedi  $
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

#ifndef LED_H_
#define LED_H_

/**
 * enum LedState: LED color state definitions.
 */
enum LedState
{
	/**
	 * LedOff: LED is off. Control bits are 00.
	 */
	LedOff,
	/**
	 * LedRed: LED is red. Control bits are 01.
	 */
	LedRed,
	/**
	 * LedGreen: LED is green. Control bits are 10.
	 */
	LedGreen,
	/**
	 * LedYellow: LED is yellow. Control bits are 11.
	 */
	LedYellow,
};

/**
 * Class LED: Class implements board-specific functionality to support LEDs.
 * All functions and data members are static, so no instance of this class is
 * necessary.
 */
class LED
{
public:
	static void Init();
	static void Set(int led, int state);
	static void SetAll(unsigned char * state);
protected:
	static volatile char * lptr;
};

#endif
