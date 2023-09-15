// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/Terminal.h_v   1.0   21 Mar 2012 12:31:58   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements terminal I/O routines. Terminal port and speed are
// defined in the linker file. Routines here use polling, so they are safe to
// use even before the multitasker starts. Polling is done with interrupts
// enabled, so this code should have minimal impact on multitasker, except
// for the cases when terminal I/O takes too long time and causes other
// timeouts.

#ifndef TERMINAL_H_
#define TERMINAL_H_

/**
 * Class Terminal: Class implements functionality required by serial port in
 * polling mode. Terminal port and speed settings are defined in configuration
 * area, values are initialized in linker file. All routines in this class are
 * static, so no object of this type is needed. All routines use polling,
 * except for Out() when CyGOS is started, in this case output will go through
 * interrupt-driven serial port driver.
 */
class Terminal
{
public:
	static void Init();
	static void Out(int ch);
	static int In();
	static void OutPoll(int ch);
	static void OutPoll(char * string);
	static bool Peek();
};

#endif
