// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/stdlib/Assert.cpp_v   1.1   27 Jul 2015 08:50:48   sesharma  $
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
#include "CyGOS.h"
extern TaskIntLoadCallBack LoadLogCallBack;

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
	CyGOS::DisableWatchDog();
	RTOS::Stop();
	CyGOS::KickWatchDog();

	char * tst = file, * fname = file;
	do{
		tst = strchr(tst, '/');
		if(tst)
			fname = ++tst;
	}while(tst);
	CyGOS::WriteSystemEvent(ASSERT_ERROR, line ,((fname[4] << 24) | (fname[5] << 16) | (fname[6] << 8) | fname[7]),
			((fname[0] << 24) | (fname[1] << 16) | (fname[2] << 8) | fname[3]),
			 true);
	CyGOS::LogTaskLoad();
	for (;;)
	{
		CyGOS::KickWatchDog();
		printf("Assertion '%s' failed in %s at line %d\r", expr, file, line);
		CPU::Restart();
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
	CyGOS::DisableWatchDog();
	RTOS::Stop();
	CyGOS::KickWatchDog();

	char * tst = file, * fname = file;
		do{
			tst = strchr(tst, '/');
			if(tst)
				fname = ++tst;
	}while(tst);
	CyGOS::WriteSystemEvent(CRASH_ERROR, line ,((fname[4] << 24) | (fname[5] << 16) | (fname[6] << 8) | fname[7]),
			((fname[0] << 24) | (fname[1] << 16) | (fname[2] << 8) | fname[3]),
			 true);
	CyGOS::LogTaskLoad();
	for (;;)
	{
		CyGOS::KickWatchDog();
        CPU::Restart();
		printf("System crashed '%s' in %s at line %d\r", text, file, line);
		Debug::Poll();
	}
}
