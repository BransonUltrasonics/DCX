// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/stdlib/Assert.cpp_v   1.0   29 Oct 2010 10:39:46   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements the assertion routine.

#include "assert.h"
#include "stdio.h"
#include "RTOS.h"
#include "DebugNew.h"
#include "Debug.h"
#include "CPU.h"

/**
 * AssertFailed: Print the 'Assertion failed' message with file and line as
 * provided. Function disables interrupts and prints the same message over and
 * over again, so that diagnostics terminal can be connected to the serial port
 * after the system crashed, and the message would not get lost. Also, latest
 * change to RTOS::Stop should display all other queued up printf() output.
 * The delay to let printf task have some time to display the output data is no
 * longer needed.
 * TODO Add some deeper diagnostics code into this mess!
 * @param expr Expression that failed.
 * @param file File name where the expression failed.
 * @param line Line number.
 */
void AssertFailed(char * expr, char *file, int line)
{
	RTOS::Stop();
	for (;;)
	{
		printf("Assertion '%s' failed in %s at line %d\r", expr, file, line);
		Debug::Poll();
	}
}

/**
 * Display system crash message with file and line as provided. Function
 * disables interrupts and prints the same message over and over again, so that
 * diagnostics terminal can be connected to the serial port after the system
 * crashed, and the message would not get lost. Also, latest change to
 * RTOS::Stop should display all other queued up printf() output.
 * The delay to let printf task have some time to display the output data is no
 * longer needed.
 * TODO Add some deeper diagnostics code into this mess!
 * @param text System crash message.
 * @param file File name where the system crashed.
 * @param line Line number.
 */
void SystemCrash(char * text, char *file, int line)
{
	RTOS::Stop();
	for (;;)
	{
		printf("System crashed '%s' in %s at line %d\r", text, file, line);
		Debug::Poll();
	}
}
