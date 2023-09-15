// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/RTOS/PrintfTask.cpp_v   1.3   03 Sep 2013 19:02:52   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines Printf task object. Task updates behavior of printf
// function when RTOS is already running.

#include "PrintfTask.h"
#include "RTOS.h"
#include "CPUdefs.h"
#include "Board.h"
#include "DebugNew.h"
#include "stdio.h"

/**
 * PrintfTask: Initialize the printf() task object.
 */
PrintfTask::PrintfTask() :
	Task("Printf", 4096), queue(PRINTF_OUTPUT_SIZE, "Printf"), port(
			PRINTF_INPUT_SIZE, PRINTF_OUTPUT_SIZE)
{
}

/**
 * Run: Run the Printf task. Task suspends until printf function wakes it up
 * explicitly. Then, the task gets all bytes accumulated in the queue and sends
 * them into serial port queue. When done, task goes back to sleep. This design
 * guarantees that no printf is interrupted by another printf unless there is a
 * task switch in the middle of one printf, and the printf itself takes minimal
 * time to execute with buffering of data instead of putting it out into serial
 * port on the fly..
 */
//void UpdateTPVal(int);
void PrintfTask::Run()
{
	port.Init(__TERMINAL_PORT, __TERMINAL_BAUD, 2, 3, false);
	for (;;)
	{
		Suspend();
		unsigned char ch;
		while (queue.Read(this, ch, 0))
			port.Send(this, ch, -1);		
	}
}

/**
 * Print: Print a single character. Function is called from TerminalOut for
 * each character. Function is non-blocking, but the assumption here is that
 * buffers are large enough to handle all the traffic in single printf.
 */
void PrintfTask::Print(unsigned char ch)
{
	queue.Write(0, ch, 0);
}

/**
 * getchar: Read a character from terminal serial port. Function waits until a
 * character is available.
 * @return int Character code.
 */
int getchar()
{
	return getcharWait(-1);
}

/**
 * getcharWait: Read a character from terminal serial port. Function waits
 * until a character is available or timeout expires.
 * @param timeout Timeout to wait, in RTOS ticks.
 * @return int Character code, or -1 if timeout expired.
 */
int getcharWait(int timeout)
{
	unsigned char ch;
	PrintfTask * task = (PrintfTask *) RTOS::GetTask(prioPrintf);
	bool ok = task->port.Receive(RTOS::GetCurrentTask(), ch, timeout);
	if (!ok)
		return -1;
	return ch;
}

/**
 * gets: Get a line from serial port. Input line is not limited and may
 * overwrite other memory areas if input is longer than buffer.
 * @param buf Storage location for input string.
 * @return char * buf value passed into this function. There are no error
 * conditions.
 */
char * gets(char * buf)
{
	char * ptr = buf;
	for (;;)
	{
		int ch = getchar();
		if (ch == 0x08)
		{
			if (ptr != buf)
			{
				putchar(0x08);
				putchar(0x20);
				putchar(0x08);
				ptr--;
			}
		}
		else if ((ch == '\n') || (ch == '\r'))
		{
			putchar('\r');
			putchar('\n');
			*ptr = 0;
			break;
		}
		else
		{
			putchar(ch);
			*ptr++ = ch;
		}
	}
	return buf;
}
